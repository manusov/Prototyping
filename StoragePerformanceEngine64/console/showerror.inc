;---------- Show details about detected error and wait key press -----------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:  RCX = Pointer to error description first string, 0 means skip     ;
;         RDX = Pointer to error description second string, 0 means skip    ;
;         R8D = Windows style error code for decoding by WinAPI and         ;
;               show string "<Error name> (code)", 0 means skip             ;
;         R9  = Pointer to work (transit) buffer for prepare text data      ;
;               no limits provided, caller must associate buffer size and   ;
;               text output size, typically additional space available      ;
;                                                                           ;
; OUTPUT: None                                                              ;
;         Use memory at [input R9]                                          ;
;                                                                           ;         
;---------------------------------------------------------------------------;

ShowError:

cld
push rbx rsi rdi rbp 
mov rbx,rcx                     ; RBX = String #1, non volatile
mov rsi,rdx                     ; RSI = String #2, non volatile
mov edi,r8d                     ; EDI = WinAPI error code, non volatile
mov rbp,r9                      ; RBP = Buffer pointer, non volatile
                                
; Set color and write "Error: " message part
mov ecx,FOREGROUND_RED + FOREGROUND_INTENSITY
call SetFgColor                 ; Color for "Error: " message part
lea rcx,[MsgError]
call InternalConsoleWrite

; Set color and conditionally write first string
mov ecx, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
call SetFgColor
mov rcx,rbx
jrcxz @f                        ; Go skip if string pointer = 0
call InternalConsoleWrite       ; First string about error
@@:

; Conditionally write second string with alignment for "Error: " part
test rsi,rsi
jz @f                           ; Go skip if string pointer = 0
lea rcx,[CrLf]
call InternalConsoleWrite       ; Next string
lea rcx,[MsgErrorTab]
call InternalConsoleWrite       ; Tabulation for alignment for "Error: " part
mov rcx,rsi
call InternalConsoleWrite       ; Second string about error
@@:

; Conditionally write third string with alignment for "Error: " part
test rdi,rdi
jz @f                           ; Go skip if error code = 0
lea rcx,[CrLf]
call InternalConsoleWrite       ; Next string
lea rcx,[MsgErrorTab]
call InternalConsoleWrite       ; Tabulation for alignment for "Error: " part 
push rdi
lea rsi,[MsgErrorOS]            ; RSI = Pointer to string
mov rdi,rbp                     ; RDI = Pointer to buffer
call StringWrite                ; Write "OS error" to buffer
pop rax                         ; EAX = Error code
mov bl,0                        ; BL  = numeric template code
mov esi,eax                     ; ESI = Error code, backup
call DecimalPrint32             ; Write error code decimal number to buffer
mov ax,' ='
stosw
stosb
xchg eax,esi                    ; EAX = WinAPI error code
call DecodeError                ; Write OS error description string to buffer
mov al,0
stosb
mov rcx,rbp
call InternalConsoleWrite       ; Write from buffer to console 
@@:

; Restore console color, skip string and write done message "Press ENTER..."
call GetColor
xchg ecx,eax
call SetColor              ; Restore original color
lea rcx,[CrLf2]
call InternalConsoleWrite
lea rcx,[DoneMsgNoWait]         ; Parm#1 = RCX = Pointer to message
cmp [OptionWaitkey],0
je  @f
lea rcx,[DoneMsgWait]           ; Parm#1 = RCX = Pointer to message
@@:
call InternalConsoleWrite

; Wait key press, after key pressed skip string
lea rcx,[TEMP_BUFFER]
call ConsoleRead
lea rcx,[CrLf2]
call InternalConsoleWrite

; Exit
pop rbp rdi rsi rbx 
ret
