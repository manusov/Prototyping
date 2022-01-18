;---------- Parse scenario file and update options variables ---------------;
;                                                                           ;
; INPUT:   ECX = Pointer to buffer with loaded scenario file                ;  
;          EDX = Limit for this buffer, address of first not-valid byte     ;          
;          ESI = Pointer to options descriptors list                        ;
;          EDI = Pointer to error status variables, for error reporting:    ;
;                3 DWORDS, 2 pointers to strings + 1 OS API error code      ;         
;                                                                           ;
; OUTPUT:  RAX = Status, 0 = error, error status variables valid            ;
;                        1 = no errors, error status variables not used     ;
;          Update options values variables, addressed by descriptors at R8  ;
;          Update status variables, addressed by R9, if error               ;
;                                                                           ;         
;---------------------------------------------------------------------------;

SCENARIO_POINTER  EQU  dword [ebp + 00]  ; Pointer to buffer with scenario file ( R8 at x64 )
SCENARIO_LIMIT    EQU  dword [ebp + 04]  ; Buffer limit, addr. of first not valid byte ( R9 at x64 ) 
ERROR_POINTER     EQU  dword [ebp + 12]  ; Pointer to error status variables, 3 qwords ( R12 at x64 )
OPTIONS_LIST      EQU  dword [ebp + 16]  ; Pointer to options descriptors list ( R11 at x64 )
                                         ; EBX = Dynamical copy of this options pointer ( R10 at x64 )
 
ParseScenario:

cld
push ebx esi edi ebp edx ecx
mov ebp,esp

; Pre-clear status
xor eax,eax
mov [edi + 00],eax
mov [edi + 04],eax
mov [edi + 08],eax

; This cycle for strings in the scenario
StringsCycle:
mov ecx,ERROR_POINTER
mov edx,SCENARIO_POINTER
mov [ecx + 04],edx       ; EDX = Pointer to parsed error cause string
mov ebx,OPTIONS_LIST     ; EBX = Reload pointer to options descriptors list 

; This cycle for options descriptors
OptionsCycle:
mov al,[ebx + X0]         ; AL = Option type from option descriptor
cmp al,XEND
je ParseError1            ; Go error if option not found at list, unknown option
cmp al,XLAST
ja ParseError1            ; Go error if option not found at list, unknown option
mov esi,SCENARIO_POINTER  ; ESI = Pointer to text file buffer 
mov edi,[ebx + X3]        ; EDI = Pointer to option keyword

; This cycle for option name word compare
DetectName:
cmp esi,SCENARIO_LIMIT  ; ESI = Pointer to scenario data, compare with limit
jae ParseExitOK         ; Go if scenario done  
mov ah,[edi]            ; AH = Current char from option descriptor, keyword 
inc edi
test ah,ah              ; AH = 0 means keyword done, means keyword match 
jz DetectedThisOption   ; Go if keyword match detected 
lodsb                   ; AL = Current char from scenario file
cmp al,0Ah
je DetectNextString     ; Go if LF(Line Feed), keyword done, try next
cmp al,0Dh
je DetectNextString     ; Go if CR(Carriage Return), keyword done, try next
cmp al,';'             
je DetectTailString     ; Go if comments, keyword done, try next
cmp al,' '
je DetectNextOption     ; Go if SPACE, keyword done, try next
cmp al,09h
je DetectNextOption     ; Go if TAB, keyword done, try next 
cmp al,'='
je DetectNextOption     ; Go if EQUAL, keyword done, try next 
cmp al,'0'
jb ParseError2          ; Go error if unexpected char
cmp al,'z'
ja ParseError2          ; Go error if unexpected char
cmp al,'A'
jb @f                   ; Skip if not a text char
cmp al,'z'
ja @f                   ; Skip if not a text char 
and ax,0DFDFh           ; If text char, convert both compare chars to UPPER CASE 
@@:
cmp al,ah               ; Compare chars from keyword pattern and scenario
je DetectName           ; Continue compare if this chars match 

; Option not detected, select next element of options descriptors list
DetectNextOption:
mov al,[ebx + X0]       ; AL = Option type from option descriptor
add ebx,XSMALL
cmp al,XKEY
jne OptionsCycle
add ebx,XDELTA
jmp OptionsCycle

; Option detected, select and run option handler
DetectedThisOption:
mov al,[ebx + X0]       ; AL = Option type from option descriptor 
cmp al,XKEY
je HandlerOptionKeys
cmp al,XDEC
je HandlerOptionDecimal32
cmp al,XHEX
je HandlerOptionHex64
cmp al,XSIZE
je HandlerOptionSize64

call OptionString        ; Handler for option type 5 = string 
jmp ParseDone

HandlerOptionKeys:       ; Handler for option type 1 = one of keywords
call OptionKeys
jmp ParseDone

HandlerOptionDecimal32:  ; Handler for option type 2 = 32-bit value as decimal
call OptionDecimal32
jmp ParseDone

HandlerOptionHex64:      ; Handler for option type 3 = 64-bit value as hex 
call OptionHex64
jmp ParseDone

HandlerOptionSize64:     ; Handler for option type 4 = 64-bit value as size
call OptionSize64 
ParseDone:
jc ParseError2           ; Go if error detected

; Detect tail, non-informative part of string
DetectTailString:
cmp esi,SCENARIO_LIMIT
jae ParseExitOK          ; Go if scenario done
lodsb                    ; Read current char, scan for end of this string
cmp al,0Ah
je DetectNextString
cmp al,0Dh
jne DetectTailString   

; Step to next string of scenario file 
DetectNextString:
cmp esi,SCENARIO_LIMIT
jae ParseExitOK             ; Go if scenario done
lodsb                       ; Read current char, scan for start next string
cmp al,0Ah
je DetectNextString 
cmp al,0Dh
je DetectNextString 
dec esi
mov SCENARIO_POINTER,esi   ; Address of first char of next string
jmp StringsCycle

; Error branches
ParseError1:               ; This handler for unknown option keyword
lea eax,[MsgUnknownOption]
jmp EntryError
ParseError2:               ; This handler for errors in option string
lea eax,[MsgOption]
EntryError:
mov ecx,ERROR_POINTER
mov [ecx + 00],eax         ; EAX = Pointer to error comments string
mov edx,[ecx + 04]         ; EDX = Pointer to parsed error cause string
; Terminate error caused string for prevent show all scenario file
mov esi,edx                ; Start scanning end of error cause string
mov edi,SCENARIO_LIMIT
dec edi
.L0:
cmp esi,edi                ; EDI = Loaded scenario file limit in the buffer
jae .L1                    ; Go if scenario file done 
lodsb
cmp al,0Ah
je .L2
cmp al,0Dh
.L2:
jne .L0
.L1:
mov byte [esi],0           ; Mark end of string for output error cause string
; Exit points
xor eax,eax   ; Status = error
jmp ParseExit 
ParseExitOK:
mov eax,1     ; Status = no errors
ParseExit:
pop ebp ebp ebp edi esi ebx
ret

;---------- Local subroutine: OPTION_KEYS handler -----------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT = Limit for RSI, address of first not valid byte    ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;

OptionKeys:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
mov edi,[ebx + X4]   ; EDI = Patterns pointer , ESI = Scenario pointer
xor ecx,ecx          ; ECX = Possible keywords pointer
mov SCENARIO_POINTER,esi  ; Pointer to keyword in the file
.L5:                 ; This cycle for select next possible keyword 
mov esi,SCENARIO_POINTER   ; ESI = Restore pointer to keyword in the file
.L2:                 ; This cycle for compare option current keyword
cmp esi,SCENARIO_LIMIT     ;  Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; AL = current char from scenario file, Pointer + 1
mov ah,[edi]         ; AH = current char from comparision pattern 
inc edi              ; Pattern pointer + 1
test ah,ah
jz .L4               ; Go if possible keyword done, keyword match
cmp al,'0'
jb .L0               ; Go if wrong char in the keyword, next line
cmp al,'z'
ja .L0               ; Go if wrong char in the keyword, next line
cmp al,'A'
jb .L1               ; Go if convert to upper case not required
cmp al,'z'
ja .L1               ; Go if convert to upper case not required
and ax,0DFDFh        ; Convert both compared chars to upper case
.L1:
cmp al,ah
jne .L3              ; Go to next possible keyword comparision if mismatch
test ah,ah
jz .L4               ; Go if keyword match, zero reached at pattern
jmp .L2
.L0:                 ; Go this if wrong char detected after "="
jmp ParseError
.L3:                 ; Go this if next possible keyword compare
mov al,[edi]
inc edi
cmp al,0
jne .L3
cmp byte [edi],0
je .L0               ; Go error if list done but keyword not detected
inc ecx              ; ECX = counter for option value
jmp .L5              ; Otherwise, go compare with next possible keyword
.L4:                 ; Go this if keyword match
mov edx,[ebx + X2]   ; EDX = Pointer to option value
mov [edx],cl         ; Write option value, one byte selector
; Global exit points
ParseOK:           ; Next, return and skip remaining string part
clc
ParseSpecialCase:  ; Return with CF, ZF valid
ret                ; Return with CF=0 means normal status 
EndOfScenario:
or al,1
stc
ret                ; Return with CF=1, ZF=0 means end of scenario file 
ParseError:
xor al,al
stc
ret                ; Return with CF=1, ZF=1 means parse error: unexpected char

;---------- Local subroutine: OPTION_DECIMAL_32 handler -----------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT = Limit for RSI, address of first not valid byte    ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;

OptionDecimal32:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; ECX = Numeric value for extract
.L0:                 ; Cycle for interpreting decimal numeric string
cmp esi,SCENARIO_LIMIT     ; Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb
cmp al,'0'
jb .L1               ; Go if not a decimal digit char '0'...'9'
cmp al,'9'
ja .L1               ; Go if not a decimal digit char '0'...'9'
and eax,0Fh          ; Mask for convert '0'...'9' to 0...9
imul ecx,ecx,10      ; Update value, use 64-bit RCX because unsigned required
add ecx,eax          ; Add current value
jmp .L0              ; Continue cycle for interpreting decimal numeric string
.L1:                 ; This point for first non-decimal char detected
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
jne ParseError       ; Go error if wrong char after digit
.L2:                 ; Otherwise normal termination 
mov edx,[ebx + X2]   ; EDX = Pointer to option value
mov [edx],ecx        ; Write option value, dword, extracted as decimal 
clc                  ; Next, return and skip remaining string part
ret

;---------- Local subroutine: OPTION_HEX_64 handler ---------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT = Limit for RSI, address of first not valid byte    ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;

OptionHex64:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; EDI:ECX = Numeric value for extract
xor edi,edi
.L0:                 ; Cycle for interpreting hex numeric string
cmp esi,SCENARIO_LIMIT   ; Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read char from scenario
cmp al,'0'
jb .L2               ; Go if not a decimal digit char '0'...'9'
cmp al,'9'
jna .L3              ; Go if decimal digit char '0'...'9'
.L2:
mov ah,al
and ah,0DFh          ; Make uppercase for convert 'a'...'f' to 'A'...'F'
cmp ah,'A'
jb .L1               ; Go if not a hex digit char 'A'...'F' 
cmp ah,'F'
ja .L1               ; Go if not a hex digit char 'A'...'F'
mov al,ah
sub al,'A'-10        ; Convert 'A'...'F' to 10...15
.L3:
and eax,0Fh          ; Convert to 00h...0Fh values
shld edi,ecx,4       ; Shift previous extracted 64-bit value at EDI:ECX
shl ecx,4
or ecx,eax           ; Add current char
jmp .L0              ; Continue cycle for interpreting hex numeric string
.L1:                 ; This point for first non-hexadecimal char detected
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
jne ParseError       ; Go error if wrong char after digit
.L4:                 ; Otherwise normal termination, store extracted value 
mov edx,[ebx + X2]   ; EDX = Pointer to option value
mov [edx + 0],ecx    ; Write option value, qword, extracted as decimal
mov [edx + 4],edi 
clc                  ; Next, return and skip remaining string part
ret

;---------- Local subroutine: OPTION_SIZE_64 handler --------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT  = Limit for RSI, address of first not valid byte   ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;

;OptionSize64:
;call SkipEqual       ; Skip " = " fragment
;jc ParseSpecialCase  ; Go if scenario done or parsing error
;xor ecx,ecx          ; ECX = Numeric value for extract
;.L0:                 ; Cycle for interpreting numeric string
;cmp esi,SCENARIO_LIMIT        ;  Loaded scenario file limit in the buffer
;jae EndOfScenario    ; Go exit if scenario file done 
;lodsb                ; Read char from scenario
;cmp al,'0'
;jb .L3               ; Go if not a digit
;cmp al,'9'
;ja .L3               ; Go if not a digit
;and eax,0Fh          ; Mask digit, '0'...'9' converted to 0...9
;imul ecx,ecx,10      ; Update value, use 64-bit RCX because unsigned required
;add ecx,eax          ; Add current digit to extracted value
;jmp .L0
;.L3:                ; First non-numeric char detected, also cycle for this part
;cmp al,0Ah
;je .L2              ; Go string end if 0Ah (LF)
;cmp al,0Dh
;je .L2              ; Go string end if 0Dh (CR)
;cmp al,';'
;je .L2              ; Go string end if comments
;cmp al,09h
;je .L4
;cmp al,' '
;je .L4
;cmp al,'M'          ; Detect M = megabytes
;je .LM
;cmp al,'m'
;je .LM
;cmp al,'G'          ; Detect G = gigabytes
;je .LG
;cmp al,'g'
;je .LG
;cmp al,'K'          ; Detect K = kilobytes
;je .LK
;cmp al,'k'
;jne ParseError
;.LK:
;imul ecx,ecx,1024            ; Make kilobytes from accumulated value
;jmp .L4
;.LM:
;imul ecx,ecx,1024*1024       ; Make megabytes from accumulated value
;jmp .L4
;.LG:
;imul ecx,ecx,1024*1024*1024  ; Make gigabytes from accumulated value 
;.L4:
;lodsb                ; Get next char after numeric value
;cmp esi,SCENARIO_LIMIT      ; R9 = Loaded scenario file limit in the buffer
;jae EndOfScenario    ; Go exit if scenario file done 
;jmp .L3
;.L2:                   ; Otherwise normal termination, store extracted value 
;mov edx,[ebx + X2]     ; EDX = Pointer to option value
;mov [edx + 0],ecx      ; Write option value, qword, extracted as decimal
;mov dword [edx + 4],0 
;clc                    ; Next, return and skip remaining string part
;ret

; --- variant with fix 32/64 bug ---

OptionSize64:

call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; EDI:ECX = Numeric value for extract
xor edi,edi
.L0:                 ; Cycle for interpreting numeric string
cmp esi,SCENARIO_LIMIT        ;  Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read char from scenario
cmp al,'0'
jb .L3               ; Go if not a digit
cmp al,'9'
ja .L3               ; Go if not a digit
and eax,0Fh          ; Mask digit, '0'...'9' converted to 0...9

push eax
mov edx,10
call LocalMultiply64
pop eax 
add ecx,eax         ; Add current digit to extracted value
adc edi,0           ; High dword of qwoed

jmp .L0
.L3:                ; First non-numeric char detected, also cycle for this part
cmp al,0Ah
je .L2              ; Go string end if 0Ah (LF)
cmp al,0Dh
je .L2              ; Go string end if 0Dh (CR)
cmp al,';'
je .L2              ; Go string end if comments
cmp al,09h
je .L4
cmp al,' '
je .L4
cmp al,'M'          ; Detect M = megabytes
je .LM
cmp al,'m'
je .LM
cmp al,'G'          ; Detect G = gigabytes
je .LG
cmp al,'g'
je .LG
cmp al,'K'          ; Detect K = kilobytes
je .LK
cmp al,'k'
jne ParseError

.LK:
mov edx,1024            ; Make kilobytes from accumulated value
jmp .L5
.LM:
mov edx,1024*1024       ; Make megabytes from accumulated value
jmp .L5
.LG:
mov edx,1024*1024*1024  ; Make gigabytes from accumulated value 
.L5:
call LocalMultiply64

.L4:
lodsb                  ; Get next char after numeric value
cmp esi,SCENARIO_LIMIT      ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario      ; Go exit if scenario file done 
jmp .L3
.L2:                   ; Otherwise normal termination, store extracted value 
mov edx,[ebx + X2]     ; EDX = Pointer to option value
mov [edx + 0],ecx      ; Write option value, qword, extracted as decimal
mov [edx + 4],edi      ; High dword of qword 
clc                    ; Next, return and skip remaining string part
ret

; INPUT:   EDI:ECX = 64-bit value for multiply
;          EDX = Multiplier
; OUTPUT:  EDI:ECX = Multiplied by EAX
;          EAX, EDX destroyed

LocalMultiply64:
push edx
xchg eax,edi
mul edx
xchg edi,eax
pop edx
xchg eax,ecx
mul edx
xchg ecx,eax
add edi,edx
ret

; --- end of variant with fix 32/64 bug ---


;---------- Local subroutine: OPTION_STRING handler ---------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT  = Limit for RSI, address of first not valid byte   ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;

OptionString:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error 
mov edi,[ebx + X2]   ; EDI = Pointer to pointer to string
mov edi,[edi]        ; EDI = Pointer to string
mov ecx,PATH_BUFFER_SIZE - 1   ; Limit for string buffer, exclude last 0
.L0:                 ; Cycle for string copy from scenario to buffer
cmp esi,SCENARIO_LIMIT        ; Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read current char
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
je .L1               ; Go string end if one of this chars
stosb                ; Store char in the destination buffer
loop .L0             ; Cycle for string copy, limited by buffer size (RCX) 
jmp ParseError       ; Go error if too long string
.L1:                 ; End of informative part of string
mov al,0
stosb                ; Write 0-terminator
clc                  ; Next, return and skip remaining string part
ret

;---------- Check line continue by compare current char -----------------------; 
; INPUT:    AL = Char for comparision                                          ;
; OUTPUT    ZF flag, 1(Z) if char detected, 0(NZ) if char not detected         ;
;           char detected if match one of:                                     ;
;           0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)                     ;
;------------------------------------------------------------------------------;

CheckLineChar:
cmp al,0Ah
je @f          ; Go with ZF=1 if LF (Line Feed)
cmp al,0Dh
je @f          ; Go with ZF=1 if CR (Carriage Return)
cmp al,' '
je @f          ; Go with ZF=1 if SPACE
cmp al,09h     
je @f          ; Go with ZF=1 if TAB
cmp al,';'
@@:
ret

;---------- Local subroutine for skip " = " -----------------------------------;
; INPUT:   ESI = Pointer to scenario file current parse fragment               ;
;          SCENARIO_LIMIT = Limit for RSI, address of first not valid byte     ;
;          EBP = Pointer to stack frame variables                              ;
; OUTPUT:  ESI = Updated by skip fragment " = "                                ;
;          CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF   ;
;          ZF flag = special case type, valid if CF = 1                        ;
;          ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached          ;
;------------------------------------------------------------------------------;

SkipEqual:
cmp esi,SCENARIO_LIMIT   ; Check end of file
jae .L0                  ; Go exit if end of file
lodsb                    ; AL = current char
cmp al,' '
je SkipEqual    ; Continue skip if SPACE
cmp al,09h
je SkipEqual    ; Continue skip if TAB
cmp al,'='
jne .L2 
.L1:
cmp esi,SCENARIO_LIMIT   ; Check end of file
jae .L0         ; Go exit if end of file
lodsb           ; AL = current char
cmp al,' '
je .L1          ; Continue skip if SPACE
cmp al,09h
je .L1          ; Continue skip if TAB
dec esi         ; ESI = Pointer to first char after " = " sequence
clc
ret             ; Return with CF=0 means normal status 
.L0:
or al,1
stc
ret             ; Return with CF=1, ZF=0 means end of scenario file 
.L2:
xor al,al
stc
ret             ; Return with CF=1, ZF=1 means parse error: unexpected char
