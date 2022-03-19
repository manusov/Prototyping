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
mov rbx,rcx
lea r14,[PROGRESS_BOX]
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
; Locked because WS_CAPTION + WS_SYSMENU disabled.
; mov r9,[hIcons + 0]
; mov r8d,ICON_SMALL
; mov edx,WM_SETICON 
; ; rcx valid here
; call [SendMessage]
mov edx,IDC_WAIT
xor ecx,ecx
call [LoadCursor]
mov [r14 + PROGRESSBOX.hCursor],rax
mov ecx,18                   ; Parm#1 = RCX = Height
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
push FW_DONTCARE             ; Parm#5  = Weight of the font
sub rsp,32                   ; Create parameters shadow
call [CreateFont]
add rsp,32+80                ; Remove parameters shadow and 10 parameters
mov [r14 + PROGRESSBOX.hFont],rax 
mov ecx,PS_SOLID
xor edx,edx
mov r8,0008F808h
call [CreatePen]
mov [r14 + PROGRESSBOX.hPen],rax
mov [r14 + PROGRESSBOX.progressPercentage],34   ; 50  ; 68  ; 25  ; 75   ; 0  ; 100
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
; TODO. Check all created objects for unselect, close, delete requirements.
;       Inspect initialization code for this.
mov rcx,[r14 + PROGRESSBOX.hFont]
jrcxz @f
call [DeleteObject]
@@:
mov rcx,[r14 + PROGRESSBOX.hPen]
jrcxz @f
call [DeleteObject]
@@:
mov rcx,rbx
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
mov rcx,[r14 + PROGRESSBOX.hCursor]
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
lea rsi,[r14 + PROGRESSBOX.ps]
mov rdx,rsi
; rcx valid here
call [BeginPaint]
test rax,rax
jz .statusZero
xchg r13,rax
lea rdi,[r14 + PROGRESSBOX.rect]
push rsi rdi
add rsi,PAINTSTRUCT.rcPaint + RECT.left
mov ecx,4
cld
rep movsd
pop rdi rsi
mov r8d,COLOR_WINDOW + 1
mov rdx,rdi
mov rcx,r13
call [FillRect] 
xor eax,eax
mov rdx,[r14 + PROGRESSBOX.hFont]
test rdx,rdx
jz @f
mov rcx,r13
call [SelectObject]
@@:
mov [r14 + PROGRESSBOX.hFontBack],rax
lea rdx,[r14 + PROGRESSBOX.tm]
mov rcx,r13
call [GetTextMetrics]
;---------- Percentage string -------------------------------------------------;
CENTER_SHIFT = 12    ; TODO. This value must be auto calculated by text string metrics

push rbx rdi
lea rdi,[r14 + PROGRESSBOX.progressString]
mov rdx,rdi
mov eax,[r14 + PROGRESSBOX.progressPercentage]
mov bl,0
call DecimalPrint32
mov ax,0000h + '%'
stosw
mov r8,rdi
sub r8,rdx
dec r8
pop rdi rbx
; R13 = Handle Device Context (HDC)
; RDX = Pointer to text string
; R8  = Length of text string or -1 if 0-terminated string
; RDI = Pointer to RECT structure for string positioning at window
mov [rdi + RECT.top],50 + CENTER_SHIFT
mov [rdi + RECT.bottom],67 + CENTER_SHIFT
call HelperCenterText
;---------- Percentage circle -------------------------------------------------;
HYPOTENUSE_1   = 30
HYPOTENUSE_2   = 50
CIRCLE_POINTS  = 3600
PERCENT_POINTS = 3600 / 100

mov rcx,r13
mov rdx,[r14 + PROGRESSBOX.hPen]
call [SelectObject]
lea r12,[r14 + PROGRESSBOX.ps + PAINTSTRUCT.rcPaint + RECT.left]
mov esi,[r12 + RECT.right]
sub esi,[r12 + RECT.left]
shr esi,1
mov edi,[r12 + RECT.bottom]
sub edi,[r12 + RECT.top]
shr edi,1
; Build small and big external circles, constant
mov r12d,HYPOTENUSE_1
mov r15d,CIRCLE_POINTS
call HelperCircleExternal
mov r12d,HYPOTENUSE_2
mov r15d,CIRCLE_POINTS
call HelperCircleExternal
; Build internal circles, by percentage
mov r12d,HYPOTENUSE_1
mov eax,PERCENT_POINTS
mul [r14 + PROGRESSBOX.progressPercentage]
xchg r15d,eax
call HelperCircleInternal
;---------- Close paint context -----------------------------------------------;
mov rdx,rsi
mov rcx,rbx
call [EndPaint]
jmp .statusZero

;---------- Local subroutines -------------------------------------------------;


; TODO.
; FULL REDESIGN REQUIRED. CAN DRAW ELLIPSE BY WINAPI.
; BOOL WINAPI ARC.

HelperCircleExternal:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rcx,r13
mov edx,esi
mov eax,r12d
neg eax
lea r8d,[edi + eax]   ; HYPOTENUSE
lea r9,[r14 + PROGRESSBOX.oldPoint]
call [MoveToEx]
push CIRCLE_POINTS
mov [r14 + PROGRESSBOX.alpha],0         ; TODO. Use structure and base-index access for this variables or stack frame.
mov [r14 + PROGRESSBOX.a],r12d          ; HYPOTENUSE
finit
fldpi
fadd st0,st0
fidiv dword [rsp]
fstp [r14 + PROGRESSBOX.delta]
pop r12
.circle:
finit
fld [r14 + PROGRESSBOX.alpha]
fld st0
fsin
fimul [r14 + PROGRESSBOX.a]
fistp [r14 + PROGRESSBOX.b]
fcos
fimul [r14 + PROGRESSBOX.a]
fchs
fistp [r14 + PROGRESSBOX.c]
fld [r14 + PROGRESSBOX.delta]
fadd [r14 + PROGRESSBOX.alpha]
fstp [r14 + PROGRESSBOX.alpha]
mov rcx,r13
mov edx,esi
mov r8d,edi
add edx,[r14 + PROGRESSBOX.b]
add r8d,[r14 + PROGRESSBOX.c] 
call [LineTo]
dec r15d
jnz .circle
mov rsp,rbp
pop rbp
ret

; TODO. Use same fragment for subroutines: HelperCircleExternal, HelperCircleInternal
; Remove non-optimal duplication.
 
HelperCircleInternal:
push rbp r12
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov [r14 + PROGRESSBOX.alpha],0
test r15d,r15d
jz .skip
.circle:
mov r12d,[rbp]
push CIRCLE_POINTS
mov [r14 + PROGRESSBOX.a],r12d          ; HYPOTENUSE
finit
fldpi
fadd st0,st0
fidiv dword [rsp]
fstp [r14 + PROGRESSBOX.delta]
pop r12
finit
fld [r14 + PROGRESSBOX.alpha]
fld st0
fsin
fimul [r14 + PROGRESSBOX.a]
fistp [r14 + PROGRESSBOX.b]
fcos
fimul [r14 + PROGRESSBOX.a]
fchs
fistp [r14 + PROGRESSBOX.c]
mov r12d,[rbp]
mov rcx,r13
lea r9,[r14 + PROGRESSBOX.oldPoint]
mov edx,esi
mov r8d,edi
add edx,[r14 + PROGRESSBOX.b]
add r8d,[r14 + PROGRESSBOX.c] 
call [MoveToEx]
mov r12d,[rbp]
push CIRCLE_POINTS
add r12d,HYPOTENUSE_2 - HYPOTENUSE_1 
mov [r14 + PROGRESSBOX.a],r12d          ; HYPOTENUSE
finit
fldpi
fadd st0,st0
fidiv dword [rsp]
fstp [r14 + PROGRESSBOX.delta]
pop r12
finit
fld [r14 + PROGRESSBOX.alpha]
fld st0
fsin
fimul [r14 + PROGRESSBOX.a]
fistp [r14 + PROGRESSBOX.b]
fcos
fimul [r14 + PROGRESSBOX.a]
fchs
fistp [r14 + PROGRESSBOX.c]
fld [r14 + PROGRESSBOX.delta]
fadd [r14 + PROGRESSBOX.alpha]
fstp [r14 + PROGRESSBOX.alpha]
mov rcx,r13
mov edx,esi
mov r8d,edi
add edx,[r14 + PROGRESSBOX.b]
add r8d,[r14 + PROGRESSBOX.c] 
call [LineTo]          ; redundant for debug
dec r15d
jnz .circle
.skip:
mov rsp,rbp
pop rax rbp
ret
