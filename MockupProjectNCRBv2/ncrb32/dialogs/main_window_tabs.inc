;---------- Callback dialogue procedures for tab sheets window ----------------;
; Note. This ESP values at procedure entry, +4 because EIP pushed.             ;
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

DialogProcSysinfo:
mov al,BINDER_BUILD_SYSINFO
jmp DialogProcEntry
DialogProcMemory:
mov al,BINDER_BUILD_MEMORY
jmp DialogProcEntry
DialogProcMath:
mov al,BINDER_BUILD_MATH
jmp DialogProcEntry
DialogProcOs:
mov al,BINDER_BUILD_OS
jmp DialogProcEntry
DialogProcNativeOs:
mov al,BINDER_BUILD_NATIVE_OS
jmp DialogProcEntry
DialogProcProcessor:
mov al,BINDER_BUILD_PROCESSOR
jmp DialogProcEntry
DialogProcTopology:
mov al,BINDER_BUILD_TOPOLOGY
jmp DialogProcEntry
DialogProcTopologyEx:
mov al,BINDER_BUILD_TOPOLOGY_EX
jmp DialogProcEntry
DialogProcNuma:
mov al,BINDER_BUILD_NUMA
jmp DialogProcEntry
DialogProcGroups:
mov al,BINDER_BUILD_P_GROUPS
jmp DialogProcEntry
DialogProcAcpi:
mov al,BINDER_BUILD_ACPI
jmp DialogProcEntry
DialogProcAffCpuid:
mov al,BINDER_BUILD_AFF_CPUID
jmp DialogProcEntry
DialogProcKmd:
mov al,BINDER_BUILD_KMD

;---------- Entry point with AL = Binder ID for required child window ---------;

DialogProcEntry:
push ebp
mov ebp,esp
push ebx esi edi
movzx esi,al              ; ESI = Binder ID for this child window
mov edi,[Registry]
add edi,REGISTRY32.appData
mov ebx,PARM_HWNDDLG      ; EBX = This window handle

;---------- Detect message type -----------------------------------------------;

mov eax,PARM_MSG
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_COMMAND
je .wmcommand
xor eax,eax
jmp .finish

;---------- WM_INITDIALOG handler: create sheet window ------------------------;

.wminitdialog:
mov eax,esi
call Binder
xchg eax,esi
cmp al,BINDER_BUILD_MEMORY
jne @f
inc eax
call Binder

; *** DEBUG ***
; mov eax,[Registry]
; mov eax,[eax + REGISTRY32.allocatorBindBuffer.objectStart]
; mov [eax + BINDLIST.setMemMethod],0010b ; 0001b
; *** DEBUG ***

mov ax,BINDER_SET_MEMORY
call Binder

@@:
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------;

.wmcommand:
mov eax,PARM_WPARAM
cmp ax,IDB_SYSINFO_CANCEL    ; Detect click "Exit" button in the child window
je .wmclose
cmp ax,IDB_MEMORY_RUN
je .memoryRun
cmp ax,IDB_MEMORY_DRAW
je .memoryDraw
cmp ax,IDB_MATH_RUN
je .mathRun
cmp ax,IDB_MATH_DRAW
je .mathDraw
jmp .processed

.memoryRun:
mov ax,BINDER_GET_MEMORY
call Binder


; *** DEBUG ***
; mov eax,[Registry]
; mov eax,[eax + REGISTRY32.allocatorBindBuffer.objectStart]
; mov eax,[eax + BINDLIST.getMemMethod]
; INT3
; *** DEBUG ***


push 0                          ; Parm#5 = Pass value
push DialogProcChildMemoryRun   ; Parm#4 = Pointer to dialogue procedure
push ebx                        ; Parm#3 = Owner window handle
push IDD_CHILD_MEMORY_RUN       ; Parm#2 = Resource ( template ) id
.entryDialog:
push [edi + APPDATA.hResources] ; Parm#1 = Handle of resource module
call [DialogBoxParam]           ; Create modal dialogue 
jmp .processed

.memoryDraw:
push 0
push DialogProcChildMemoryDraw
push ebx
push IDD_CHILD_MEMORY_DRAW
jmp .entryDialog

.mathRun:
;---
; push 0
; push DialogProcChildMathRun
; push ebx
; push IDD_CHILD_MATH_RUN
;---
 push 0
 push DialogProcChildVectorBrief
 push ebx
 push IDD_CHILD_VECTOR_BRIEF
;---
jmp .entryDialog

.mathDraw:
push 0
push DialogProcChildMathDraw
push ebx
push IDD_CHILD_MATH_DRAW
jmp .entryDialog


;---------- WM_CLOSE handler: close window ------------------------------------;

.wmclose:
mov ecx,[edi + APPDATA.hMain]
jecxz .processed
push 0
push 0
push WM_CLOSE
push ecx
call [SendMessage]

;---------- Exit points -------------------------------------------------------;

.processed:
mov eax,1
.finish:
pop edi esi ebx ebp
ret 16
