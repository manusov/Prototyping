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
push IDC_WAIT
push 0
call [LoadCursor]
mov [PROGRESS_BOX.hCursor],eax
mov ecx,16
mov edx,FW_DONTCARE
call HelperCreateFont
mov [PROGRESS_BOX.hFont1],eax 
mov ecx,29
mov edx,FW_BOLD
call HelperCreateFont
mov [PROGRESS_BOX.hFont2],eax 
push 0008F808h
push 0
push PS_SOLID
call [CreatePen]
mov [PROGRESS_BOX.hPen],eax
mov [PROGRESS_BOX.progressPercentage],95  ; 34  ; 50  ; 68  ; 25  ; 75   ; 0  ; 100
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
; TODO. Check all created objects for unselect, close, delete requirements.
;       Inspect initialization code for this.
mov ecx,[PROGRESS_BOX.hFont1]
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
;---------- Fill window rectangle ---------------------------------------------;
PERCENTAGE_BACKGROUND_COLOR = 00F81010h
push esi ebx
lea esi,[PROGRESS_BOX.ps + PAINTSTRUCT.rcPaint]  ; ESI = Pointer to GUI rectangle returned
lea edi,[PROGRESS_BOX.rect]                      ; EDI = Pointer to GUI rectangle for build
push esi edi
mov ecx,4
rep movsd
pop edi esi
push PERCENTAGE_BACKGROUND_COLOR
call [CreateSolidBrush]                  ; TODO. 1) Check errors. 2) Delete brush after drawings.
push eax
push edi
push ebp
call [FillRect] 
push TRANSPARENT
push ebp
call [SetBkMode]
;---------- Percentage text string --------------------------------------------; 
PERCENTAGE_TEXT_COLOR = 00E8E8E8h   ; 00B0B0B0h  ;  00010101h  ;  0001C001h
push PERCENTAGE_TEXT_COLOR
push ebp
call [SetTextColor]
push [PROGRESS_BOX.hFont2]
push ebp
call [SelectObject]
push edi
lea edi,[PROGRESS_BOX.progressString]
mov edx,edi
mov eax,[PROGRESS_BOX.progressPercentage]
mov bl,0
call DecimalPrint32
mov ax,0000h + '%'
stosw
mov ecx,edi
sub ecx,edx
dec ecx
pop edi
push ecx
push PROGRESS_BOX.sz
push ecx
push edx
push ebp
call [GetTextExtentPoint32]          ; TODO. Check status.
lea ebx,[PROGRESS_BOX.sz]            ; TODO. X centering is redundant ?
mov ecx,[esi + RECT.right]
sub ecx,[esi + RECT.left]
shr ecx,1                            ; ECX = Window rectangle Xcenter
mov edx,[ebx + SIZE.cx]
shr edx,1                            ; EDX = Text string size/2
lea eax,[ecx + edx + 1]
mov [edi + RECT.right],eax           ; Percentage string right point
neg edx
lea eax,[ecx + edx - 1]
mov [edi + RECT.left],eax            ; Percentage string left point 
mov ecx,[esi + RECT.bottom]
sub ecx,[esi + RECT.top]
shr ecx,1                            ; ECX = Window rectangle Xcenter
mov edx,[ebx + SIZE.cy]
shr edx,1                            ; EDX = Text string size/2
lea eax,[ecx + edx + 1]
mov [edi + RECT.bottom],eax          ; Percentage string bottom point
neg edx
lea eax,[ecx + edx - 1]
mov [edi + RECT.top],eax             ; Percentage string top point 
pop ecx
push DT_CENTER + DT_NOPREFIX
push edi
push ecx
push PROGRESS_BOX.progressString
push ebp
call [DrawText]
;---------- Operation name text string ----------------------------------------;
OPERATION_TEXT_COLOR  = PERCENTAGE_TEXT_COLOR  ;  00010101h
OPERATION_TEXT_OFFSET = 5
push OPERATION_TEXT_COLOR
push ebp
call [SetTextColor]
push [PROGRESS_BOX.hFont1]
push ebp
call [SelectObject]
push esi edi
lea esi,[lineProgress]
lea edi,[PROGRESS_BOX.operationString]
mov edx,edi
call StringWrite
mov al,0
stosb
mov ecx,edi
sub ecx,edx
dec ecx
pop edi esi
push ecx
push PROGRESS_BOX.sz
push ecx
push edx
push ebp
call [GetTextExtentPoint32]          ; TODO. Check status.
lea ebx,[PROGRESS_BOX.sz]            ; TODO. X centering is redundant ?
mov ecx,[esi + RECT.right]
sub ecx,[esi + RECT.left]
shr ecx,1                            ; ECX = Window rectangle Xcenter
mov edx,[ebx + SIZE.cx]
shr edx,1                            ; EDX = Text string size/2
lea eax,[ecx + edx + 1]
mov [edi + RECT.right],eax           ; Percentage string right point
neg edx
lea eax,[ecx + edx - 1]
mov [edi + RECT.left],eax            ; Percentage string left point 
mov eax,[esi + RECT.bottom]
sub eax,OPERATION_TEXT_OFFSET
mov [edi + RECT.bottom],eax          ; Percentage string bottom point 
sub eax,[ebx + SIZE.cy]
mov [edi + RECT.top],eax             ; Percentage string top point  
pop ecx
push DT_CENTER + DT_NOPREFIX
push edi
push ecx
push PROGRESS_BOX.operationString
push ebp
call [DrawText]
;---------- Progress indicator as circle (arc) --------------------------------;
PROGRESS_ARC_COLOR = PERCENTAGE_TEXT_COLOR  ;  0001F801h
PROGRESS_ARC_WIDTH = 3
PROGRESS_RADIUS    = 48
push PROGRESS_ARC_COLOR
push PROGRESS_ARC_WIDTH
push PS_SOLID
call [CreatePen]
push eax
push ebp
call [SelectObject]
mov eax,[esi + RECT.right]
sub eax,[esi + RECT.left]
shr eax,1                      ; EAX = Window rectangle Xcenter
mov ebx,[esi + RECT.bottom]
sub ebx,[esi + RECT.top]
shr ebx,1                      ; EBX = Window rectangle Ycenter

; TODO. Use stack variables for remove this variables.
mov [vr10d],eax
mov [vr11d],ebx

push eax ebx
mov eax,[PROGRESS_BOX.progressPercentage]
push eax
finit
mov ecx,25
cdq
div ecx
push ecx edx
fild dword [esp + 16]
fidiv dword [esp + 04]
fimul dword [esp + 00]
fistp dword [esp + 16]     ; Store Xaddend
fild dword [esp + 12]
fidiv dword [esp + 04]
fimul dword [esp + 00]
fistp dword [esp + 12]     ; Store Yaddend
pop edx ecx eax edx ebx    ; EBX = Xaddend, EDX = Yaddend. Can use fact X=Y ?   

mov eax,ecx                                 ; TODO. Optimize POP for remove MOV.
mov ecx,[PROGRESS_BOX.progressPercentage]   ; TODO. Can use AL compact 8-bit form.
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
xor eax,eax               ; xmin
xor ecx,ecx               ; ymin
xor ebx,ebx               ; xmax
xor edx,edx               ; ymax
jmp .percent_value
.percent_75_99:
neg edx
mov eax,ebx
mov ecx,[vr11d]
add ecx,edx
jmp .percent_non_zero
.percent_50_74:
neg ebx
neg edx
mov eax,[vr10d]
add eax,ebx
mov ecx,[vr11d]
add ecx,ecx
add ecx,edx
jmp .percent_non_zero
.percent_25_49:
neg ebx
mov eax,[vr10d]
add eax,eax
add eax,ebx
mov ecx,[vr11d]
add ecx,edx
jmp .percent_non_zero
.percent_0_24:
mov eax,ebx               ; xmin
add eax,[vr10d]
mov ecx,edx               ; ymin
.percent_non_zero:
mov ebx,[vr10d]           ; xmax  ; TODO. This 2 instructions repeated.
xor edx,edx               ; ymax
.percent_value:
push edx       ; 100 ; ymax
push ebx       ; 100 ; xmax
push ecx       ; 0   ; ymin
push eax       ; 0   ; xmin
mov eax,[vr11d]
add eax,PROGRESS_RADIUS
push eax
mov eax,[vr10d]
add eax,PROGRESS_RADIUS
push eax
mov eax,[vr11d]
sub eax,PROGRESS_RADIUS
push eax
mov eax,[vr10d]
sub eax,PROGRESS_RADIUS
push eax
push ebp
call [Arc]
.percent_0:

;---------- Close paint context -----------------------------------------------;
pop ebx esi
push esi
push ebx
call [EndPaint]
jmp .statusZero


;---------- Helpers subroutines -----------------------------------------------;

HelperCreateFont:
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
push edx  ;  FW_DONTCARE         ; Parm#5  = Weight of the font
push eax                         ; Parm#4 = Orientation
push eax                         ; Parm#3 = Escapment
push eax                         ; Parm#2 = Width
push ecx  ;  18                  ; Parm#1 = Height
call [CreateFont]
ret

