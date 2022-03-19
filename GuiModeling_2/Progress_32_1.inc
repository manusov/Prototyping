;---------- Callback dialogue procedure ---------------------------------------;
;           Handler for "Progress" window, item in the application main menu.  ; 
;                                                                              ;
; INPUT:   [esp + 04] = Parm#1 = HWND = Dialog box handle                      ; 
;          [esp + 08] = Parm#2 = UINT = Message                                ; 
;          [esp + 12] = Parm#3 = WPARAM, message-specific                      ;
;          [esp + 16] = Parm#4 = LPARAM, message-specific                      ;
;                                                                              ;
; OUTPUT:  EAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;                                                                              ;
;------------------------------------------------------------------------------;
DialogProcProgress:
push ebx esi edi ebp
mov eax,[esp + 08 + 16]
mov ebx,[esp + 04 + 16]
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
pop ebp edi esi ebx
ret 16

;------------------------------------------------------------------------------;
;                                                                              ;
;               WM_INITDIALOG handler: create "About" window.                  ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wminitdialog:
; Locked because WS_CAPTION + WS_SYSMENU disabled.
; push [hIcons + 0]
; push ICON_SMALL
; push WM_SETICON 
; push dword [esp + 16 + 16]
; call [SendMessage]
push IDC_WAIT
push 0
call [LoadCursor]
mov [PROGRESS_BOX.hCursor],eax
xor eax,eax                      ; EAX = 0 for compact push 0
push eax                         ; Parm#14 = Pointer to font typename string, here not used
push VARIABLE_PITCH              ; Parm#13 = Font pitch and family
push CLEARTYPE_QUALITY           ; Parm#12 = Output quality
push CLIP_DEFAULT_PRECIS         ; Parm#11 = Clip precision
push OUT_OUTLINE_PRECIS          ; Parm#10 = Output precision
push DEFAULT_CHARSET             ; Parm#9  = Charset
push eax                         ; Parm#8  = Strike, here=0=none
push eax                         ; Parm#7  = Underline, here=0=none
push eax                         ; Parm#6  = Italic, here=0=none
push FW_DONTCARE                 ; Parm#5  = Weight of the font
push eax                         ; Parm#4 = Orientation
push eax                         ; Parm#3 = Escapment
push eax                         ; Parm#2 = Width
push 18                          ; Parm#1 = Height
call [CreateFont]
mov [PROGRESS_BOX.hFont],eax 
push 0008F808h
push 0
push PS_SOLID
call [CreatePen]
mov [PROGRESS_BOX.hPen],eax
mov [PROGRESS_BOX.progressPercentage],34    ; 50  ; 68  ; 25  ; 75   ; 0  ; 100
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
; TODO. Check all created objects for unselect, close, delete requirements.
;       Inspect initialization code for this.
mov ecx,[PROGRESS_BOX.hFont]
jecxz @f
push ecx
call [DeleteObject]
@@:
mov ecx,[PROGRESS_BOX.hPen]
jecxz @f
push ecx
call [DeleteObject]
@@:
push ebx
call [DestroyWindow]
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_COMMAND handler: interpreting user input.                 ; 
;                 Detect click "OK" button at "About" window.                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmcommand:
;cmp word [esp + 12 + 16],IDB_OK
;je .wmclose
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                             WM_MOUSEMOVE handler.                            ; 
;               Provide mouse cursor consistency when mouse move.              ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmmousemove:
mov ecx,[PROGRESS_BOX.hCursor]
jecxz @f
push ecx
call [SetCursor]
@@:
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_PAINT handler. Build GUI window content.                  ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmpaint:
lea esi,[PROGRESS_BOX.ps]
push esi
push ebx
call [BeginPaint]
test eax,eax
jz .statusZero

xchg ebp,eax
lea edi,[PROGRESS_BOX.rect]
push esi edi
add esi,PAINTSTRUCT.rcPaint + RECT.left
mov ecx,4
cld
rep movsd
pop edi esi
push COLOR_WINDOW + 1
push edi
push ebp
call [FillRect] 
xor eax,eax
mov ecx,[PROGRESS_BOX.hFont]
jecxz @f
push ecx
push ebp
call [SelectObject]
@@:
mov [PROGRESS_BOX.hFontBack],eax
push PROGRESS_BOX.tm
push ebp
call [GetTextMetrics]
;---------- Percentage string -------------------------------------------------;
CENTER_SHIFT = 12    ; TODO. This value must be auto calculated by text string metrics

push ebx edi
lea edi,[PROGRESS_BOX.progressString]
mov ecx,edi
mov eax,[PROGRESS_BOX.progressPercentage]
mov bl,0
call DecimalPrint32
mov ax,0000h + '%'
stosw
mov edx,edi
sub edx,ecx
dec edx
pop edi ebx
; EBP = Handle Device Context (HDC)
; ECX = Pointer to text string
; EDX = Length of text string or -1 if 0-terminated string
; EDI = Pointer to RECT structure for string positioning at window
mov [edi + RECT.top],50 + CENTER_SHIFT
mov [edi + RECT.bottom],67 + CENTER_SHIFT
call HelperCenterText
;---------- Percentage circle -------------------------------------------------;
HYPOTENUSE_1   = 30
HYPOTENUSE_2   = 50
CIRCLE_POINTS  = 3600
PERCENT_POINTS = 3600 / 100

push [PROGRESS_BOX.hPen]
push ebp
call [SelectObject]
push esi
mov esi,[PROGRESS_BOX.ps + PAINTSTRUCT.rcPaint + RECT.right]
sub esi,[PROGRESS_BOX.ps + PAINTSTRUCT.rcPaint + RECT.left]
shr esi,1
mov edi,[PROGRESS_BOX.ps + PAINTSTRUCT.rcPaint + RECT.bottom]
sub edi,[PROGRESS_BOX.ps + PAINTSTRUCT.rcPaint + RECT.top]
shr edi,1
; Build small and big external circles, constant
mov [PROGRESS_BOX.vr12d],HYPOTENUSE_1
mov [PROGRESS_BOX.vr15d],CIRCLE_POINTS
call HelperCircleExternal
mov [PROGRESS_BOX.vr12d],HYPOTENUSE_2
mov [PROGRESS_BOX.vr15d],CIRCLE_POINTS
call HelperCircleExternal
; Build internal circles, by percentage
mov [PROGRESS_BOX.vr12d],HYPOTENUSE_1
mov eax,PERCENT_POINTS
mul [PROGRESS_BOX.progressPercentage]
mov [PROGRESS_BOX.vr15d],eax
call HelperCircleInternal
pop esi
;---------- Close paint context -----------------------------------------------;
push esi
push ebx
call [EndPaint]
jmp .statusZero

;---------- Local subroutines -------------------------------------------------;


; TODO.
; FULL REDESIGN REQUIRED. CAN DRAW ELLIPSE BY WINAPI.
; BOOL WINAPI ARC.


HelperCircleExternal:
mov eax,[PROGRESS_BOX.vr12d]
neg eax
add eax,edi                    ; HYPOTENUSE
push PROGRESS_BOX.oldPoint
push eax
push esi
push ebp
call [MoveToEx]
push CIRCLE_POINTS
mov dword [PROGRESS_BOX.alpha + 0],0
mov dword [PROGRESS_BOX.alpha + 4],0
mov eax,[PROGRESS_BOX.vr12d]
mov [PROGRESS_BOX.a],eax           ; HYPOTENUSE
finit
fldpi
fadd st0,st0
fidiv dword [esp]
fstp [PROGRESS_BOX.delta]
pop [PROGRESS_BOX.vr12d]
.circle:
finit
fld [PROGRESS_BOX.alpha]
fld st0
fsin
fimul [PROGRESS_BOX.a]
fistp [PROGRESS_BOX.b]
fcos
fimul [PROGRESS_BOX.a]
fchs
fistp [PROGRESS_BOX.c]
fld [PROGRESS_BOX.delta]
fadd [PROGRESS_BOX.alpha]
fstp [PROGRESS_BOX.alpha]
mov edx,esi
mov ecx,edi
add edx,[PROGRESS_BOX.b]
add ecx,[PROGRESS_BOX.c] 
push ecx
push edx
push ebp
call [LineTo]
dec [PROGRESS_BOX.vr15d]
jnz .circle
ret

; TODO. Use same fragment for subroutines: HelperCircleExternal, HelperCircleInternal
; Remove non-optimal duplication.
 
HelperCircleInternal:
mov eax,[PROGRESS_BOX.vr12d]
mov [PROGRESS_BOX.vrbp],eax
mov dword [PROGRESS_BOX.alpha + 0],0
mov dword [PROGRESS_BOX.alpha + 4],0
cmp [PROGRESS_BOX.vr15d],0 
je .skip
.circle:
mov eax,[PROGRESS_BOX.vrbp]
mov [PROGRESS_BOX.vr12d],eax
push CIRCLE_POINTS
mov eax,[PROGRESS_BOX.vr12d]
mov [PROGRESS_BOX.a],eax          ; HYPOTENUSE
finit
fldpi
fadd st0,st0
fidiv dword [esp]
fstp [PROGRESS_BOX.delta]
pop [PROGRESS_BOX.vr12d]          ; Optimize operand because this for release stack
finit
fld [PROGRESS_BOX.alpha]
fld st0
fsin
fimul [PROGRESS_BOX.a]
fistp [PROGRESS_BOX.b]
fcos
fimul [PROGRESS_BOX.a]
fchs
fistp [PROGRESS_BOX.c]
mov eax,[PROGRESS_BOX.vrbp]
mov [PROGRESS_BOX.vr12d],eax
mov edx,esi
mov ecx,edi
add edx,[PROGRESS_BOX.b]
add ecx,[PROGRESS_BOX.c] 
push PROGRESS_BOX.oldPoint
push ecx
push edx
push ebp
call [MoveToEx]
mov eax,[PROGRESS_BOX.vrbp]
mov [PROGRESS_BOX.vr12d],eax
push CIRCLE_POINTS
add [PROGRESS_BOX.vr12d],HYPOTENUSE_2 - HYPOTENUSE_1 
mov eax,[PROGRESS_BOX.vr12d]
mov [PROGRESS_BOX.a],eax          ; HYPOTENUSE
finit
fldpi
fadd st0,st0
fidiv dword [esp]
fstp [PROGRESS_BOX.delta]
pop [PROGRESS_BOX.vr12d]          ; Optimize operand because this for release stack
finit
fld [PROGRESS_BOX.alpha]
fld st0
fsin
fimul [PROGRESS_BOX.a]
fistp [PROGRESS_BOX.b]
fcos
fimul [PROGRESS_BOX.a]
fchs
fistp [PROGRESS_BOX.c]
fld [PROGRESS_BOX.delta]
fadd [PROGRESS_BOX.alpha]
fstp [PROGRESS_BOX.alpha]
mov edx,esi
mov ecx,edi
add edx,[PROGRESS_BOX.b]
add ecx,[PROGRESS_BOX.c] 
push ecx
push edx
push ebp
call [LineTo]          ; redundant for debug
dec [PROGRESS_BOX.vr15d]
jnz .circle
.skip:
ret
