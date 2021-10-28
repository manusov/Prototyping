;---------- Callback dialogue procedures for operations child windows ---------;
;           Handler for "Run" button, vector brief mode at "Math" screen       ; 
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

DialogProcChildVectorBrief:
push ebp
mov ebp,esp
push ebx esi edi

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov edi,[Registry]               ; EDI = Pointer to global registry

; add edi,REGISTRY32.appData       ; EDI = Pointer to registry.appData
lea edi,[APP_DATA]

mov ebx,PARM_HWNDDLG             ; EBX = Window handle

;---------- Detect message type -----------------------------------------------;

mov eax,PARM_MSG
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_CLOSE
je .wmclose
.skip:
xor eax,eax
jmp .finish

;--- WM_INITDIALOG handler: create child window: vector perf. brief report ----;

.wminitdialog:                   ; Build main window
mov ax,BINDER_BUILD_VECTOR_BRIEF
call Binder
push [edi + APPDATA.createdIcons + VBRF_ICON_NUMBER * 4]
push ICON_SMALL
push WM_SETICON 
push PARM_HWNDDLG
call [SendMessage]
mov esi,[edi + APPDATA.lockedStrings]
mov ax,STR_CHILD_VECTOR_BRIEF
call IndexString
push esi
push PARM_HWNDDLG
call [SetWindowText]
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------; 

.wmcommand:                      ; User input: cancel button or close window

;---------- Detect click "Exit" item in the main menu -------------------------; 

cmp ax,IDM_EXIT
je .wmclose
jmp .processed

;---------- WM_CLOSE handler: close window ------------------------------------;

.wmclose:
push 1                           ; Parm#2 = Result for return
push ebx                         ; Parm#1 = Window handle
call [EndDialog]

;---------- Exit points -------------------------------------------------------;

.processed:
mov eax,1
.finish:
pop edi esi ebx ebp
ret 16
