;---------- Callback dialogue procedure ---------------------------------------;
;           Handler for "Draw" window, item in the application main menu.      ; 
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
DialogProcDraw:

;push rbp
;mov rbp,rsp
;and rsp,0FFFFFFFFFFFFFFF0h
;sub rsp,32
;cmp edx,WM_CLOSE
;jne .statusZero
;call [DestroyWindow]
;.statusZero:
;xor eax,eax
;mov rsp,rbp
;pop rbp
;ret

push rbx rsi rdi rbp r12 r13 r14
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
xchg eax,edx
mov rbx,rcx
lea r14,[DRAW_BOX]
;---------- Detect message type -----------------------------------------------;
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_CLOSE
je .wmclose
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_PAINT
je .wmpaint
.statusZero:
xor eax,eax
jmp .status
.statusOne:
mov eax,1
.status:
mov rsp,rbp
pop r14 r13 r12 rbp rdi rsi rbx
ret

;------------------------------------------------------------------------------;
;                                                                              ;
;               WM_INITDIALOG handler: create "About" window.                  ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wminitdialog:
;mov r9,[hIcons + 0]
;mov r8d,ICON_SMALL
;mov edx,WM_SETICON 
;; rcx valid here
;call [SendMessage]
;mov edx,IDC_HAND
;xor ecx,ecx
;call [LoadCursor]
;mov [r14 + ABOUTBOX.hCursor],rax
;mov ecx,16                   ; Parm#1 = RCX = Height
;xor edx,edx                  ; Parm#2 = RDX = Width
;xor r8d,r8d                  ; Parm#3 = R8  = Escapment
;xor r9d,r9d                  ; Parm#4 = R9  = Orientation
;xor eax,eax                  ; RAX = 0 for compact push 0
;push rax                     ; Parm#14 = Pointer to font typename string, here not used
;push VARIABLE_PITCH          ; Parm#13 = Font pitch and family
;push CLEARTYPE_QUALITY       ; Parm#12 = Output quality
;push CLIP_DEFAULT_PRECIS     ; Parm#11 = Clip precision
;push OUT_OUTLINE_PRECIS      ; Parm#10 = Output precision
;push DEFAULT_CHARSET         ; Parm#9  = Charset
;push rax                     ; Parm#8  = Strike, here=0=none
;push rax                     ; Parm#7  = Underline, here=0=none
;push rax                     ; Parm#6  = Italic, here=0=none
;push FW_DONTCARE             ; Parm#5  = Weight of the font
;sub rsp,32                   ; Create parameters shadow
;call [CreateFont]
;add rsp,32+80                ; Remove parameters shadow and 10 parameters
;mov [r14 + ABOUTBOX.hFont],rax 
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
;mov rcx,[r14 + ABOUTBOX.hFont]
;jrcxz @f
;call [DeleteObject]
;@@:
;mov rcx,rbx
;xor edx,edx
;call [EndDialog]
;
call [DestroyWindow]
;
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_COMMAND handler: interpreting user input.                 ; 
;                 Detect click "OK" button at "About" window.                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmcommand:
;cmp r8w,IDB_OK
;je .wmclose
jmp .statusZero


;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_PAINT handler. Draw GUI window content.                   ; 
;                                                                              ;
;------------------------------------------------------------------------------;

YBOTTOM_DRAW = 115

.wmpaint:
lea rsi,[r14 + DRAWBOX.ps]
mov rdx,rsi
; rcx valid here
call [BeginPaint]
test rax,rax
jz .statusZero
xchg r13,rax
lea rdi,[r14 + DRAWBOX.rect]
push rsi rdi
add rsi,PAINTSTRUCT.rcPaint + RECT.left
mov ecx,3
cld
rep movsd
lodsd
sub eax,YBOTTOM_DRAW
stosd
pop rdi rsi
mov r8d,COLOR_WINDOW + 1
mov rdx,rdi
mov rcx,r13
call [FillRect] 


;mov r9,[hIcons + 08]
;mov r8d,ICONY
;mov edx,ICONX
;mov rcx,r13
;call [DrawIcon]
;xor eax,eax
;mov rdx,[r14 + ABOUTBOX.hFont]
;test rdx,rdx
;jz @f
;mov rcx,r13
;call [SelectObject]
;@@:
;mov [r14 + ABOUTBOX.hFontBack],rax
;lea rdx,[r14 + ABOUTBOX.tm]
;mov rcx,r13
;call [GetTextMetrics]
;;---------- Strings 1-3, without clickable elements ---------------------------;
;push rsi
;cld
;lea rsi,[r14 + ABOUTBOX.s1]
;.next1:
;lodsq
;xchg rdx,rax
;test rdx,rdx
;jz .stop1
;lodsb
;movzx eax,al
;mov [rdi + RECT.top],eax
;add eax,YADD1
;mov [rdi + RECT.bottom],eax
;call HelperCenterText
;jmp .next1
;.stop1:
;pop rsi
;;---------- Strings 4-5, with clickable elements ------------------------------;
;push rbx
;sub rsp,32 + 8
;lea rbx,[r14 + ABOUTBOX.s4]
;.next2:
;mov rdx,[rbx + LINEVAR.pointer]
;test rdx,rdx
;jz .stop2
;movzx eax,byte [rbx + LINEVAR.vertical]
;mov [rbx + LINEVAR.ymin],eax
;add eax,[r14 + ABOUTBOX.tm + TEXTMETRIC.tmHeight]
;mov [rbx + LINEVAR.ymax],eax
;movzx r8d,byte [rbx + LINEVAR.xs1]
;call HelperTextSize
;mov edx,[rsi + PAINTSTRUCT.rcPaint + RECT.right]
;sub edx,[rsi + PAINTSTRUCT.rcPaint + RECT.left]
;sub edx,eax
;shr edx,1
;mov r12d,edx
;mov [rdi + RECT.left],edx
;movzx eax,byte [rbx + LINEVAR.vertical]
;mov [rdi + RECT.top],eax
;add eax,YADD2
;mov [rdi + RECT.bottom],eax
;movzx r8d,byte [rbx + LINEVAR.xs2]
;mov rdx,[rbx + LINEVAR.pointer]
;call HelperLeftText
;movzx r8d,byte [rbx + LINEVAR.xs2]
;mov rdx,[rbx + LINEVAR.pointer]
;call HelperTextSize
;add eax,r12d
;mov [rdi + RECT.left],eax
;mov [rbx + LINEVAR.xmin],eax
;mov edx,00FF0000h
;mov rcx,r13
;call [SetTextColor]
;push rax
;movzx r8d,byte [rbx + LINEVAR.xs3]
;movzx edx,byte [rbx + LINEVAR.xs2]
;add rdx,[rbx + LINEVAR.pointer]
;call HelperLeftText
;pop rdx
;mov rcx,r13
;call [SetTextColor]
;movzx r8d,byte [rbx + LINEVAR.xs2]
;add r8b,[rbx + LINEVAR.xs3]
;mov rdx,[rbx + LINEVAR.pointer]
;call HelperTextSize
;add eax,r12d
;mov [rdi + RECT.left],eax
;mov [rbx + LINEVAR.xmax],eax
;mov r8,-1
;movzx edx,byte [rbx + LINEVAR.xs2]
;add dl,[rbx + LINEVAR.xs3]
;add rdx,[rbx + LINEVAR.pointer]
;call HelperLeftText
;add rbx,sizeof.LINEVAR
;jmp .next2 
;.stop2:
;add rsp,32 + 8
;pop rbx
;;---------- Paint window elements done ----------------------------------------;
;mov rdx,[r14 + ABOUTBOX.hFontBack]
;test rdx,rdx
;jz @f
;mov rcx,r13
;call [SelectObject]
;@@:
mov rdx,rsi
mov rcx,rbx
call [EndPaint]
jmp .statusZero
