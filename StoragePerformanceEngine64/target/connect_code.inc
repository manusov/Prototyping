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
; INPUT:    R15 = Pointer to source string for write                           ;
;           RBP = Pointer to destination buffer for write                      ;
; OUTPUT:   None                                                               ;
;           Must save RSI, RDI                                                 ;    
;------------------------------------------------------------------------------;

MessageStepName:
push rsi rdi
mov rsi,r15                     ; RSI = Pointer to source string for write
mov rdi,rbp                     ; RDI = Pointer to destination buffer for write
call StringWrite                ; Write string to buffer 
mov eax,'...'
stosd                           ; Add string "..."  0 to step name in buffer 
mov rcx,rbp                     ; RCX = Pointer to buffer
call InternalConsoleWrite       ; Write from buffer to console
pop rdi rsi
ret

;---------- Write step result to console after step execution -----------------;
; INPUT:    RBP = Pointer to destination buffer for write                      ;
; OUTPUT:   None                                                               ;
;           Must save RSI, RDI                                                 ;    
;------------------------------------------------------------------------------;

MessageStepDone:
push rdi
mov rdi,rbp                     ; RDI = Pointer to destination buffer for write
mov al,' '
stosb                           ; Add string " " to buffer
mov eax,'done'
stosd                           ; Add string "done" to buffer
mov ax,0A0Dh
stosw                           ; Add CR, LF to buffer
mov al,0
stosb                           ; Add string terminator = 0 to buffer
mov rcx,rbp                     ; RCX = Pointer to buffer
call InternalConsoleWrite
pop rdi
ret

;---------- Helper foe print Read/Write/Copy speed in MBPS --------------------;
; INPUT:   RAX = MBPS value as floating point double precision number          ;
;          RSI = Pointer to source string                                      ;           
;          RDI = Pointer to destination buffer                                 ; 
; OUTPUT:  RSI = updated by string copy operation                              ;  
;          RDI = updated by string copy operation                              ;
;------------------------------------------------------------------------------;

HelperMBPS:
push rax
call ItemWrite_CRLF  ; Write CR, LF, string, " = " 
pop rax
test rax,rax
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
; INPUT:   RCX = Pointer to complex path string, source data                   ; 
;          RDX = Pointer to buffer for write prefix, destination data 1        ;
;          R8  = Pointer to buffer for write postfix, destination data 2       ;
;          R9  = Destination buffers ( prefix and postfix ) maximum size       ;   
; OUTPUT:  None                                                                ;
;          Destination buffers updated                                         ;
;------------------------------------------------------------------------------;

PathExtractHelper:
; Calculate limits
lea r10,[rcx+r9]   ; R10 = Limit for RCX , original path string
lea r11,[rdx+r9]   ; R11 = Limit for RDX , for extracted prefix
lea r9,[r8+r9]     ; R9  = Limit for R8  , for extracted postfix
; Start extract prefix
.L0:               ; Cycle for prefix string extract
cmp rcx,r10
jae .L2            ; Go end if buffer size limit for prefix
mov al,[rcx]       ; Read char from path and filename string
cmp al,0
je .L2             ; Go end if end of string
cmp al,'.'
je .L1             ; Go postfix mode if "."
cmp al,'*'
je .L3             ; Skip this char if "*"
cmp rdx,r11
jae .L2            ; Go end if buffer size limit for prefix
mov [rdx],al       ; Store char at prefix string
inc rdx            ; Destination pointer + 1
.L3:
inc rcx            ; Source pointer + 1
jmp .L0            ; Cycle for prefix string extract
; Start extract postfix
.L1:               ; Cycle for postfix string extract
cmp rcx,r10
jae .L2            ; Go end if buffer size limit for prefix
mov al,[rcx]
cmp al,0
je .L2             ; Go end if end of string
cmp al,'*'
je .L4             ; Skip this char if "*"
cmp r8,r9
jae .L2            ; Go end if buffer size limit for postfix
mov [r8],al
inc r8             ; Destination pointer + 1
.L4:
inc rcx            ; Source pointer + 1
jmp .L1            ; Cycle for postfix string extract
.L2:
; Strings termination
mov byte [rdx],0   ; Terminate first destination string - prefix
mov byte [r8],0    ; Terminate second destination string - postfix
ret

;------- Helper for copy 0-terminated string of bytes, INCLUDE last 0 byte ----;
; INPUT:   RCX = Pointer to source string                                      ;
;          RDX = Pointer to destination string                                 ;
; OUTPUT:  RCX, RDX updated by operation                                       ; 
;------------------------------------------------------------------------------;

StringHelper_Z:
@@:
mov al,[rcx]
mov [rdx],al
inc rcx
inc rdx
cmp al,0
jne @b
ret

;------- Helper for copy 0-terminated string of bytes, EXCLUDE last 0 byte ----;
; INPUT:   RCX = Pointer to source string                                      ;
;          RDX = Pointer to destination string                                 ;
; OUTPUT:  RCX, RDX updated by operation                                       ; 
;------------------------------------------------------------------------------;

StringHelper_NZ:
call StringHelper_Z
dec rdx                ; Return destination pointer for exclude last 0 byte
ret

;---------- Helper for build path string, INCLUDE last 0 byte -----------------;
; INPUT:   EBX = File number, 0-based, for example 0 can produce "src0.bin"    ; 
;          RCX = Pointer to source path PREFIX ( path and name ) string        ;
;          RDX = Pointer to source path POSTFIX ( extension ) string           ;
;          RAX = Pointer to destination string                                 ;
; OUTPUT:  None, RBX preserved, RAX, RCX, RDX destroyed                        ;
;------------------------------------------------------------------------------;
 
PathHelper:
push rbx rdi rdx
; Path and file name
mov rdx,rax
call StringHelper_NZ
; File number as part of name
mov rdi,rdx
mov eax,ebx
mov bl,0
call DecimalPrint32
; File extension
pop rcx
mov rdx,rdi
call StringHelper_Z 
pop rdi rbx
ret
