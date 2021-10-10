;---------- Callback dialogue procedures for operations child windows ---------;
;           Handler for "Run" button at "Math" screen                          ; 
;                                                                              ;
; INPUT:   RCX = Parm#1 = HWND = Dialog box handle                             ; 
;          RDX = Parm#2 = UINT = Message                                       ; 
;          R8  = Parm#3 = WPARAM, message-specific                             ;
;          R9  = Parm#4 = LPARAM, message-specific                             ;
;                                                                              ;
; OUTPUT:  RAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;                                                                              ;
;------------------------------------------------------------------------------;

DialogProcChildMathRun:
cld
push rbp rbx rsi rdi r15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h     ; Stack alignment
sub rsp,32                     ; Make parameters shadow for next calls
mov PARM_HWNDDLG,rcx           ; Save input parameters to shadow 
mov PARM_MSG,rdx
mov PARM_WPARAM,r8
mov PARM_LPARAM,r9
mov r15,[Registry]
add r15,REGISTRY64.appData

;---------- Detect message type -----------------------------------------------;

cmp rdx,0000FFFFh
jae .skip
xchg eax,edx                   ; Use EAX for compact CMP
cmp eax,WM_INITDIALOG
je .wminitdialog               ; Go if dialogue initialization message 
cmp eax,WM_COMMAND
je .wmcommand                  ; Go if command message
cmp eax,WM_CLOSE
je .wmclose                    ; Go if window close message
.skip:
xor eax,eax
jmp .finish                    ; Go exit if unknown event

;--- WM_INITDIALOG handler: create child window: math performance report ------; 

.wminitdialog: 
mov r9,[r15 + APPDATA.createdIcons + MATH_ICON_NUMBER * 8] 
mov r8d,ICON_SMALL 
mov edx,WM_SETICON 
mov rcx,PARM_HWNDDLG
call [SendMessage]
mov rsi,[r15 + APPDATA.lockedStrings]
mov ax,STR_CHILD_MATH_REPORT
call IndexString
mov rdx,rsi
mov rcx,PARM_HWNDDLG
call [SetWindowText]
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------; 

.wmcommand:                    ; User input: cancel button or close window

;---------- Detect click "Exit" item in the main menu -------------------------; 

cmp ax,IDM_EXIT
je .wmclose
jmp .processed

;---------- WM_CLOSE handler: close window ------------------------------------;

.wmclose:
mov edx,1  ; xor edx,edx       ; RDX = Parm#2 = Result for return
mov rcx,PARM_HWNDDLG           ; RCX = Parm#1 = Window handle
call [EndDialog]

;---------- Exit points -------------------------------------------------------;

.processed:
mov eax,1
.finish:
mov rsp,rbp
pop r15 rdi rsi rbx rbp
ret


