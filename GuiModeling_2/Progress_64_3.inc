;---------- Callback dialogue procedure ---------------------------------------;
;           Handler for "Progress" window, item in the application main menu.  ; 
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
DialogProcProgress:
push rbx rsi rdi rbp r12 r13 r14 r15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
xchg eax,edx
mov r14,rcx                 ; R14 = HWND, dialog box handle
lea r15,[PROGRESS_BOX]      ; R15 = Pointer to data structure PROGRESS_BOX
;---------- Detect message type -----------------------------------------------;
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_CLOSE
je .wmclose
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_MOUSEMOVE
je .wmmousemove
cmp eax,WM_PAINT
je .wmpaint
.statusZero:
xor eax,eax
jmp .status
.statusOne:
mov eax,1
.status:
mov rsp,rbp
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret

;------------------------------------------------------------------------------;
;                                                                              ;
;               WM_INITDIALOG handler: create "Progress" window.               ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wminitdialog:
mov edx,IDC_WAIT
xor ecx,ecx
call [LoadCursor]
mov [r15 + PROGRESSBOX.hCursor],rax
mov ecx,16
mov r10d,FW_DONTCARE
call HelperCreateFont
mov [r15 + PROGRESSBOX.hFont1],rax 
mov ecx,29
mov r10d,FW_BOLD
call HelperCreateFont
mov [r15 + PROGRESSBOX.hFont2],rax 
mov ecx,PS_SOLID
xor edx,edx
mov r8,0008F808h
call [CreatePen]
mov [r15 + PROGRESSBOX.hPen],rax
mov [r15 + PROGRESSBOX.progressPercentage],33   ; 50  ; 68  ; 25  ; 75   ; 0  ; 100
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
; TODO. Check all created objects for unselect, close, delete requirements.
;       Inspect initialization code for this.
mov rcx,[r15 + PROGRESSBOX.hFont1]
jrcxz @f
call [DeleteObject]
@@:
mov rcx,[r15 + PROGRESSBOX.hPen]
jrcxz @f
call [DeleteObject]
@@:
mov rcx,r14
call [DestroyWindow]
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_COMMAND handler: interpreting user input.                 ; 
;                Detect click "OK" button at "Progress" window.                ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmcommand:
;cmp r8w,IDB_OK
;je .wmclose
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                             WM_MOUSEMOVE handler.                            ; 
;               Provide mouse cursor consistency when mouse move.              ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmmousemove:
mov rcx,[r15 + PROGRESSBOX.hCursor]
jrcxz @f
call [SetCursor]
@@:
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_PAINT handler. Build GUI window content.                  ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmpaint:
lea r13,[r15 + PROGRESSBOX.ps]       ; R13 = PAINTSTRUCT, R14 = HWND, R15 = PROGRESS_BOX
mov rdx,r13
; rcx valid here
call [BeginPaint]
test rax,rax
jz .statusZero
xchg r12,rax                         ; R12 = HDC
lea rsi,[r13 + PAINTSTRUCT.rcPaint]  ; RSI = Pointer to GUI rectangle returned
lea rdi,[r15 + PROGRESSBOX.rect]     ; RDI = Pointer to GUI rectangle for build
;---------- Fill window rectangle ---------------------------------------------;
PERCENTAGE_BACKGROUND_COLOR = 00F81010h
push rsi rdi
mov ecx,4
rep movsd
pop rdi rsi
mov ecx,PERCENTAGE_BACKGROUND_COLOR
call [CreateSolidBrush]                  ; TODO. 1) Check errors. 2) Delete brush after drawings.
xchg r8,rax
mov rdx,rdi
mov rcx,r12
call [FillRect] 
mov edx,TRANSPARENT
mov rcx,r12
call [SetBkMode]
;---------- Percentage text string --------------------------------------------; 
PERCENTAGE_TEXT_COLOR = 00E8E8E8h   ; 00B0B0B0h  ;  00010101h  ;  0001C001h
mov edx,PERCENTAGE_TEXT_COLOR
mov rcx,r12
call [SetTextColor]
mov rdx,[r15 + PROGRESSBOX.hFont2]
mov rcx,r12
call [SelectObject]
push rdi
lea rdi,[r15 + PROGRESSBOX.progressString]
mov rdx,rdi
mov eax,[r15 + PROGRESSBOX.progressPercentage]
mov bl,0
call DecimalPrint32
mov ax,0000h + '%'
stosw
mov r8,rdi
sub r8,rdx
dec r8
pop rdi
mov rbx,r8
lea r9,[r15 + PROGRESSBOX.sz]
; R8  valid
; RDX valid
mov rcx,r12
call [GetTextExtentPoint32]          ; TODO. Check status.
push rbx
lea rbx,[r15 + PROGRESSBOX.sz]       ; TODO. X centering is redundant ?
mov ecx,[rsi + RECT.right]
sub ecx,[rsi + RECT.left]
shr ecx,1                            ; ECX = Window rectangle Xcenter
mov edx,[rbx + SIZE.cx]
shr edx,1                            ; EDX = Text string size/2
lea eax,[ecx + edx + 1]
mov [rdi + RECT.right],eax           ; Percentage string right point
neg edx
lea eax,[ecx + edx - 1]
mov [rdi + RECT.left],eax            ; Percentage string left point 
mov ecx,[rsi + RECT.bottom]
sub ecx,[rsi + RECT.top]
shr ecx,1                            ; ECX = Window rectangle Xcenter
mov edx,[rbx + SIZE.cy]
shr edx,1                            ; EDX = Text string size/2
lea eax,[ecx + edx + 1]
mov [rdi + RECT.bottom],eax          ; Percentage string bottom point
neg edx
lea eax,[ecx + edx - 1]
mov [rdi + RECT.top],eax             ; Percentage string top point 
pop r8
push 0
push DT_CENTER + DT_NOPREFIX
mov r9,rdi
; R8 valid
lea rdx,[r15 + PROGRESSBOX.progressString]
mov rcx,r12
sub rsp,32
call [DrawText]
add rsp,32 + 16
;---------- Operation name text string ----------------------------------------;
OPERATION_TEXT_COLOR  = PERCENTAGE_TEXT_COLOR  ;  00010101h
OPERATION_TEXT_OFFSET = 5
mov edx,OPERATION_TEXT_COLOR
mov rcx,r12
call [SetTextColor]
mov rdx,[r15 + PROGRESSBOX.hFont1]
mov rcx,r12
call [SelectObject]
push rsi rdi
lea rsi,[lineProgress]
lea rdi,[r15 + PROGRESSBOX.operationString]
mov rdx,rdi
call StringWrite
mov al,0
stosb
mov r8,rdi
sub r8,rdx
dec r8
pop rdi rsi
mov rbx,r8
lea r9,[r15 + PROGRESSBOX.sz]
; R8  valid
; RDX valid
mov rcx,r12
call [GetTextExtentPoint32]          ; TODO. Check status.
push rbx
lea rbx,[r15 + PROGRESSBOX.sz]       ; TODO. X centering is redundant ?
mov ecx,[rsi + RECT.right]
sub ecx,[rsi + RECT.left]
shr ecx,1                            ; ECX = Window rectangle Xcenter
mov edx,[rbx + SIZE.cx]
shr edx,1                            ; EDX = Text string size/2
lea eax,[ecx + edx + 1]
mov [rdi + RECT.right],eax           ; Percentage string right point
neg edx
lea eax,[ecx + edx - 1]
mov [rdi + RECT.left],eax            ; Percentage string left point 
mov eax,[rsi + RECT.bottom]
sub eax,OPERATION_TEXT_OFFSET
mov [rdi + RECT.bottom],eax          ; Percentage string bottom point 
sub eax,[rbx + SIZE.cy]
mov [rdi + RECT.top],eax             ; Percentage string top point  
pop r8
push 0
push DT_CENTER + DT_NOPREFIX
mov r9,rdi
; R8 valid
lea rdx,[r15 + PROGRESSBOX.operationString]
mov rcx,r12
sub rsp,32
call [DrawText]
add rsp,32 + 16
;---------- Progress indicator as circle (arc) --------------------------------;
PROGRESS_ARC_COLOR = PERCENTAGE_TEXT_COLOR  ;  0001F801h
PROGRESS_ARC_WIDTH = 3
PROGRESS_RADIUS    = 48
mov r8d,PROGRESS_ARC_COLOR
mov edx,PROGRESS_ARC_WIDTH
mov ecx,PS_SOLID
call [CreatePen]
mov rdx,rax
mov rcx,r12
call [SelectObject]
mov r10d,[rsi + RECT.right]
sub r10d,[rsi + RECT.left]
shr r10d,1                   ; R10D = Window rectangle Xcenter
mov r11d,[rsi + RECT.bottom]
sub r11d,[rsi + RECT.top]
shr r11d,1                   ; R11D = Window rectangle Ycenter
mov eax,[r15 + PROGRESSBOX.progressPercentage]
push rax
finit
mov ecx,25
cdq
div ecx
push r11 r10 rcx rdx
fild dword [rsp + 16]
fidiv dword [rsp + 08]
fimul dword [rsp + 00]
fistp dword [rsp + 16]     ; Store Xaddend
fild dword [rsp + 24]
fidiv dword [rsp + 08]
fimul dword [rsp + 00]
fistp dword [rsp + 24]     ; Store Yaddend
pop rdx rcx r8 r9 rax      ; R8D = Xaddend, R9D = Yaddend. Can use fact X=Y ?   
mov eax,ecx                                     ; TODO. Optimize POP for remove MOV.
mov ecx,[r15 + PROGRESSBOX.progressPercentage]  ; TODO. Can use AL compact 8-bit form.
test ecx,ecx
jz .percent_0
cmp ecx,eax
jb .percent_0_24 
sub ecx,eax
cmp ecx,eax
jb .percent_25_49
sub ecx,eax
cmp ecx,eax
jb .percent_50_74
sub ecx,eax
cmp ecx,eax
jb .percent_75_99
.percent_100:
xor ecx,ecx               ; xmin
xor edx,edx               ; ymin
xor r8d,r8d               ; xmax
xor r9d,r9d               ; ymax
jmp .percent_value
.percent_75_99:
neg r9d
mov ecx,r8d               ; xmin
lea edx,[r11d + r9d]      ; ymin
jmp .percent_non_zero
.percent_50_74:
neg r8d
neg r9d
lea ecx,[r10d + r8d]      ; xmin
lea edx,[r11d * 2 + r9d]  ; ymin
jmp .percent_non_zero
.percent_25_49:
neg r8d
lea ecx,[r10d * 2 + r8d]  ; xmin
lea edx,[r11d + r9d]      ; ymin
jmp .percent_non_zero
.percent_0_24:
lea ecx,[r10d + r8d]      ; xmin  ; TODO. 64-bit LEA IS MORE COMPACT ?
mov edx,r9d               ; ymin
.percent_non_zero:
mov r8d,r10d              ; xmax  ; TODO. This 2 instructions repeated.
xor r9d,r9d               ; ymax
.percent_value:
push 0
push r9        ; 100 ; ymax
push r8        ; 100 ; xmax
push rdx       ; 0   ; ymin
push rcx       ; 0   ; xmin
lea eax,[r11d + PROGRESS_RADIUS]
push rax
lea r9d,[r10d + PROGRESS_RADIUS]
lea r8d,[r11d - PROGRESS_RADIUS]
lea edx,[r10d - PROGRESS_RADIUS]
mov rcx,r12
sub rsp,32
call [Arc]
add rsp,32 + 40
.percent_0:
;---------- Close paint context -----------------------------------------------;
mov rdx,r13
mov rcx,r14
call [EndPaint]
jmp .statusZero


;---------- Helpers subroutines -----------------------------------------------;


HelperCreateFont:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
; mov ecx,29                 ; Parm#1 = RCX = Height
xor edx,edx                  ; Parm#2 = RDX = Width
xor r8d,r8d                  ; Parm#3 = R8  = Escapment
xor r9d,r9d                  ; Parm#4 = R9  = Orientation
xor eax,eax                  ; RAX = 0 for compact push 0
push rax                     ; Parm#14 = Pointer to font typename string, here not used
push VARIABLE_PITCH          ; Parm#13 = Font pitch and family
push CLEARTYPE_QUALITY       ; Parm#12 = Output quality
push CLIP_DEFAULT_PRECIS     ; Parm#11 = Clip precision
push OUT_OUTLINE_PRECIS      ; Parm#10 = Output precision
push DEFAULT_CHARSET         ; Parm#9  = Charset
push rax                     ; Parm#8  = Strike, here=0=none
push rax                     ; Parm#7  = Underline, here=0=none
push rax                     ; Parm#6  = Italic, here=0=none
push r10  ; FW_BOLD  ; FW_DONTCARE  ; Parm#5  = Weight of the font
sub rsp,32                   ; Create parameters shadow
call [CreateFont]
add rsp,32 + 80              ; Remove parameters shadow and 10 parameters
mov rsp,rbp
pop rbp
ret
