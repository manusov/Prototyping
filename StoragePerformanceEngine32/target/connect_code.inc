;------------------------------------------------------------------------------;
;              Set of target fragments for debug, this part is:                ;
;                   connect include files with subroutines.                    ; 
;------------------------------------------------------------------------------;

include 'storage_context.inc'  ; Context support for storage I/O performance
include 'groupmbps.inc'        ; Measure bandwidth, Megabytes per second
include 'singlembps.inc'       ; Same for single file mode
include 'iops.inc'             ; Measure performance, IO operations per second
include 'mapmbps.inc'          ; Measure bandwidth (MBPS) by memory mapped file
include 'mapiops.inc'          ; Measure perf. (IOPS) by memory mapped file

;---------- Local helpers -----------------------------------------------------;

;---------- Write step name to console before step execution ------------------;
; INPUT:    EAX = Pointer to source string for write                           ;
;           EBP = Pointer to destination buffer for write                      ;
; OUTPUT:   None                                                               ;
;           Must save RSI, RDI                                                 ;    
;------------------------------------------------------------------------------;

MessageStepName:
push esi edi
mov esi,eax                   ; ESI = Pointer to source string for write
mov edi,ebp                   ; EDI = Pointer to destination buffer for write
call StringWrite              ; Write string to buffer 
mov eax,'...'
stosd                         ; Add string "..."  0 to step name in buffer 
mov ecx,ebp                   ; ECX = Pointer to buffer
call InternalConsoleWrite     ; Write from buffer to console
pop edi esi
ret

;---------- Write step result to console after step execution -----------------;
; INPUT:    EBP = Pointer to destination buffer for write                      ;
; OUTPUT:   None                                                               ;
;           Must save RSI, RDI                                                 ;    
;------------------------------------------------------------------------------;

MessageStepDone:
push esi edi
mov edi,ebp                   ; EDI = Pointer to destination buffer for write
mov al,' '
stosb                         ; Add string " " to buffer
mov eax,'done'
stosd                         ; Add string "done" to buffer
mov ax,0A0Dh
stosw                         ; Add CR, LF to buffer
mov al,0
stosb                         ; Add string terminator = 0 to buffer
mov ecx,ebp                   ; RCX = Pointer to buffer
call InternalConsoleWrite
pop edi esi
ret

;---------- Helper foe print Read/Write/Copy speed in MBPS --------------------;
; INPUT:   EDX:EAX = MBPS value as floating point double precision number      ;
;          ESI = Pointer to source string                                      ;           
;          EDI = Pointer to destination buffer                                 ; 
; OUTPUT:  ESI = updated by string copy operation                              ;  
;          EDI = updated by string copy operation                              ;
;------------------------------------------------------------------------------;

HelperMBPS:
push eax edx
call ItemWrite_CRLF  ; Write CR, LF, string, " = " 
pop edx eax
mov ecx,eax
or ecx,edx           ; Check entire EDX:EAX = 0
jz .L0               ; Skip operation and write "?" if value zero
mov bx,0200h         ; This means 2 digits after "."
call DoublePrint     ; Print floating point double precision value
mov al,' '
stosb
mov eax,'MBPS'
stosd
ret
.L0:
mov al,'?'           ; This branch used if value = 0 
stosb
ret

;---------- Extract prefix and postfix from path and filename string ----------;
; For example, if string = "C:\TEMP\DATA.BIN" ,                                ;
; prefix = "C:\TEMP\DATA" , postfix = ".BIN"                                   ;
; INPUT:   ECX = Pointer to complex path string, source data                   ; 
;          EDX = Pointer to buffer for write prefix, destination data 1        ;
;          ESI = Pointer to buffer for write postfix, destination data 2       ;
;          EDI = Destination buffers ( prefix and postfix ) maximum size       ;   
; OUTPUT:  None                                                                ;
;          Destination buffers updated                                         ;
;------------------------------------------------------------------------------;

LIMIT_ECX  EQU  dword [ebp + 00]
LIMIT_EDX  EQU  dword [ebp + 04]
LIMIT_ESI  EQU  dword [ebp + 08] 

PathExtractHelper:
push ebp ebp ebp ebp
mov ebp,esp
; Calculate limits
lea eax,[ecx + edi]    ; EAX = Limit for ECX , original path string
mov LIMIT_ECX,eax
lea eax,[edx + edi]    ; EAX = Limit for EDX , for extracted prefix
mov LIMIT_EDX,eax
lea eax,[esi + edi]    ; EAX = Limit for EDI , for extracted postfix
mov LIMIT_ESI,eax
; Start extract prefix
.L0:                   ; Cycle for prefix string extract
cmp ecx,LIMIT_ECX
jae .L2                ; Go end if buffer size limit for prefix
mov al,[ecx]           ; Read char from path and filename string
cmp al,0
je .L2                 ; Go end if end of string
cmp al,'.'
je .L1                 ; Go postfix mode if "."
cmp al,'*'
je .L3                 ; Skip this char if "*"
cmp edx,LIMIT_EDX
jae .L2                ; Go end if buffer size limit for prefix
mov [edx],al           ; Store char at prefix string
inc edx                ; Destination pointer + 1
.L3:
inc ecx                ; Source pointer + 1
jmp .L0                ; Cycle for prefix string extract
; Start extract postfix
.L1:                   ; Cycle for postfix string extract
cmp ecx,LIMIT_ECX
jae .L2                ; Go end if buffer size limit for prefix
mov al,[ecx]
cmp al,0
je .L2                 ; Go end if end of string
cmp al,'*'
je .L4                 ; Skip this char if "*"
cmp esi,LIMIT_ESI
jae .L2                ; Go end if buffer size limit for postfix
mov [esi],al
inc esi                ; Destination pointer + 1
.L4:
inc ecx                ; Source pointer + 1
jmp .L1                ; Cycle for postfix string extract
.L2:
; Strings termination
mov byte [edx],0       ; Terminate first destination string - prefix
mov byte [esi],0       ; Terminate second destination string - postfix
pop ebp ebp ebp ebp
ret

;------- Helper for copy 0-terminated string of bytes, INCLUDE last 0 byte ----;
; INPUT:   ECX = Pointer to source string                                      ;
;          EDX = Pointer to destination string                                 ;
; OUTPUT:  ECX, EDX updated by operation                                       ; 
;------------------------------------------------------------------------------;

StringHelper_Z:
@@:
mov al,[ecx]
mov [edx],al
inc ecx
inc edx
cmp al,0
jne @b
ret

;------- Helper for copy 0-terminated string of bytes, EXCLUDE last 0 byte ----;
; INPUT:   ECX = Pointer to source string                                      ;
;          EDX = Pointer to destination string                                 ;
; OUTPUT:  ECX, EDX updated by operation                                       ; 
;------------------------------------------------------------------------------;

StringHelper_NZ:
call StringHelper_Z
dec edx                ; Return destination pointer for exclude last 0 byte
ret

;---------- Helper for build path string, INCLUDE last 0 byte -----------------;
; INPUT:   EBX = File number, 0-based, for example 0 can produce "src0.bin"    ; 
;          ECX = Pointer to source path PREFIX ( path and name ) string        ;
;          EDX = Pointer to source path POSTFIX ( extension ) string           ;
;          EAX = Pointer to destination string                                 ;
; OUTPUT:  None, EBX preserved, EAX, ECX, EDX destroyed                        ;
;------------------------------------------------------------------------------;
 
PathHelper:
push ebx edi edx
; Path and file name
mov edx,eax
call StringHelper_NZ
; File number as part of name
mov edi,edx
mov eax,ebx
mov bl,0
call DecimalPrint32
; File extension
pop ecx
mov edx,edi
call StringHelper_Z 
pop edi ebx
ret
