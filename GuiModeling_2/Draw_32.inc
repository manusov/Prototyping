;---------- Callback dialogue procedure ---------------------------------------;
;           Handler for "Draw" window, item in the application main menu.      ; 
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
DialogProcDraw:

;cmp dword [esp + 08],WM_CLOSE
;jne .statusZero
;push dword [esp + 04]
;call [DestroyWindow]
;.statusZero:
;xor eax,eax
;ret 16

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
;push [hIcons + 0]
;push ICON_SMALL
;push WM_SETICON 
;push dword [esp + 16 + 16]
;call [SendMessage]
;push IDC_HAND
;push 0
;call [LoadCursor]
;mov [ABOUT_BOX.hCursor],eax
;xor eax,eax                      ; EAX = 0 for compact push 0
;push eax                         ; Parm#14 = Pointer to font typename string, here not used
;push VARIABLE_PITCH              ; Parm#13 = Font pitch and family
;push CLEARTYPE_QUALITY           ; Parm#12 = Output quality
;push CLIP_DEFAULT_PRECIS         ; Parm#11 = Clip precision
;push OUT_OUTLINE_PRECIS          ; Parm#10 = Output precision
;push DEFAULT_CHARSET             ; Parm#9  = Charset
;push eax                         ; Parm#8  = Strike, here=0=none
;push eax                         ; Parm#7  = Underline, here=0=none
;push eax                         ; Parm#6  = Italic, here=0=none
;push FW_DONTCARE                 ; Parm#5  = Weight of the font
;push eax                         ; Parm#4 = Orientation
;push eax                         ; Parm#3 = Escapment
;push eax                         ; Parm#2 = Width
;push 16                          ; Parm#1 = Height
;call [CreateFont]
;mov [ABOUT_BOX.hFont],eax 
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
;mov ecx,[ABOUT_BOX.hFont]
;jecxz @f
;push ecx
;call [DeleteObject]
;@@:
;push 0
;push dword [esp + 08 + 16]
;call [EndDialog]
;
push ebx ; dword [esp + 08 + 16] ; dword [esp + 04]
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
;cmp word [esp + 12 + 16],IDB_OK
;je .wmclose
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_PAINT handler. Draw GUI window content.                   ; 
;                                                                              ;
;------------------------------------------------------------------------------;

YBOTTOM_DRAW = 115

.wmpaint:
lea esi,[DRAW_BOX.ps]
push esi
push ebx
call [BeginPaint]
test eax,eax
jz .statusZero
; push 0
xchg ebp,eax
lea edi,[DRAW_BOX.rect]
push esi edi
add esi,PAINTSTRUCT.rcPaint + RECT.left
mov ecx,3
cld
rep movsd
lodsd
sub eax,YBOTTOM_DRAW
stosd
pop edi esi
push COLOR_WINDOW + 1
push edi
push ebp
call [FillRect] 


;push [hIcons + 04]
;push ICONY
;push ICONX
;push ebp
;call [DrawIcon]
;xor eax,eax
;mov ecx,[ABOUT_BOX.hFont]
;jecxz @f
;push ecx
;push ebp
;call [SelectObject]
;@@:
;mov [ABOUT_BOX.hFontBack],eax
;push ABOUT_BOX.tm
;push ebp
;call [GetTextMetrics]
;;---------- Strings 1-3, without clickable elements ---------------------------;
;push esi
;cld
;lea esi,[ABOUT_BOX.s1]
;.next1:
;lodsd
;xchg ecx,eax
;jecxz .stop1
;lodsb
;movzx eax,al
;mov [edi + RECT.top],eax
;add eax,YADD1
;mov [edi + RECT.bottom],eax
;call HelperCenterText
;jmp .next1
;.stop1:
;pop esi
;;---------- Strings 4-5, with clickable elements ------------------------------;
;push ebx
;lea ebx,[ABOUT_BOX.s4]
;.next2:
;mov edx,[ebx + LINEVAR.pointer]
;test edx,edx
;jz .stop2
;movzx eax,byte [ebx + LINEVAR.vertical]
;mov [ebx + LINEVAR.ymin],eax
;add eax,[ABOUT_BOX.tm + TEXTMETRIC.tmHeight]
;mov [ebx + LINEVAR.ymax],eax
;movzx ecx,byte [ebx + LINEVAR.xs1]
;call HelperTextSize
;mov edx,[esi + PAINTSTRUCT.rcPaint + RECT.right]
;sub edx,[esi + PAINTSTRUCT.rcPaint + RECT.left]
;sub edx,eax
;shr edx,1
;mov [esp + 04],edx
;mov [edi + RECT.left],edx
;movzx eax,byte [ebx + LINEVAR.vertical]
;mov [edi + RECT.top],eax
;add eax,YADD2
;mov [edi + RECT.bottom],eax
;movzx edx,byte [ebx + LINEVAR.xs2]
;mov ecx,[ebx + LINEVAR.pointer]
;call HelperLeftText
;movzx ecx,byte [ebx + LINEVAR.xs2]
;mov edx,[ebx + LINEVAR.pointer]
;call HelperTextSize
;add eax,[esp + 04]
;mov [edi + RECT.left],eax
;mov [ebx + LINEVAR.xmin],eax
;push 00FF0000h
;push ebp
;call [SetTextColor]
;push eax
;movzx edx,byte [ebx + LINEVAR.xs3]
;movzx ecx,byte [ebx + LINEVAR.xs2]
;add ecx,[ebx + LINEVAR.pointer]
;call HelperLeftText
;push ebp
;call [SetTextColor]
;movzx ecx,byte [ebx + LINEVAR.xs2]
;add cl,[ebx + LINEVAR.xs3]
;mov edx,[ebx + LINEVAR.pointer]
;call HelperTextSize
;add eax,[esp + 04]
;mov [edi + RECT.left],eax
;mov [ebx + LINEVAR.xmax],eax
;mov edx,-1
;movzx ecx,byte [ebx + LINEVAR.xs2]
;add cl,[ebx + LINEVAR.xs3]
;add ecx,[ebx + LINEVAR.pointer]
;call HelperLeftText
;add ebx,sizeof.LINEVAR
;jmp .next2 
;.stop2:
;pop ebx eax
;;---------- Paint window elements done ----------------------------------------; 
;mov ecx,[ABOUT_BOX.hFontBack]
;jecxz @f
;push ecx
;push ebp
;call [SelectObject]
;@@:

push esi
push ebx
call [EndPaint]
jmp .statusZero
