;---------- Callback dialogue procedures for tab sheets child windows ---------;
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

DialogProcSysinfo:
mov al,BINDER_BUILD_SYSINFO
jmp DialogProcEntry
DialogProcMemory:
mov al,BINDER_BUILD_MEMORY
jmp DialogProcEntry

; DialogProcMath:
; mov al,BINDER_BUILD_MATH
; jmp DialogProcEntry

DialogProcOs:
mov al,BINDER_BUILD_OS
jmp DialogProcEntry
DialogProcNativeOs:
mov al,BINDER_BUILD_NATIVE_OS
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

;---------- Entry point with AL = Binder ID for required child window ---------;

DialogProcEntry:
cld
push rbp rbx rsi rdi r15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov PARM_HWNDDLG,rcx           ; Save input parameters to shadow 
mov PARM_MSG,rdx
mov PARM_WPARAM,r8
mov PARM_LPARAM,r9

;mov r15,[Registry]             ; R15 = Pointer to global registry
;add r15,REGISTRY64.appData     ; R15 = Pointer to registry.appData
lea r15,[APP_DATA]

movzx esi,al                   ; ESI = Binder ID for this child window
mov rbx,rcx                    ; RBX = This window handle

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

;---------- WM_INITDIALOG handler: create sheet window ------------------------;

.wminitdialog:
mov eax,esi
call Binder
xchg eax,esi
cmp al,BINDER_BUILD_MEMORY
jne @f
inc eax
inc eax
call Binder

; *** DEBUG ***
; mov rax,[Registry]
; mov rax,[rax + REGISTRY64.allocatorBindBuffer.objectStart]
; mov [rax + BINDLIST.setMemMethod],0000b ; 0001b
; *** DEBUG ***

mov ax,BINDER_SET_MEMORY
call Binder

@@:
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------;

.wmcommand:
mov eax,r8d
cmp ax,IDB_SYSINFO_CANCEL    ; Detect click "Exit" button in the child window
je .wmclose
cmp ax,IDB_MEMORY_RUN
je .memoryRun
cmp ax,IDB_MEMORY_DRAW
je .memoryDraw

; cmp ax,IDB_MATH_RUN
; je .mathRun
; cmp ax,IDB_MATH_DRAW
; je .mathDraw

cmp ax,IDB_SYSINFO_VBRF
je .mathRun

jmp .processed

.memoryRun:

mov ax,BINDER_GET_MEMORY
call Binder

call ScenarioMemoryRun

; *** DEBUG ***
; mov rax,[Registry]
; mov rax,[rax + REGISTRY64.allocatorBindBuffer.objectStart]
; mov eax,[rax + BINDLIST.getMemMethod]
; mov ax,[rax + BINDLIST.getMemObject]
; mov al,[rax + BINDLIST.getMemMtrr]
; mov cl,[rax + BINDLIST.getMemSmp]
; mov ch,[rax + BINDLIST.getMemNuma]
; mov cl,[rax + BINDLIST.getMemAccess]
; mov ch,[rax + BINDLIST.getMemLpages]
; mov cl,[rax + BINDLIST.getMemPref]
; mov cl,[rax + BINDLIST.getMemMeas]
; mov ch,[rax + BINDLIST.getMemPix]
; mov ch,[rax + BINDLIST.getMem3d]
; INT3
; *** DEBUG ***


lea r9,[DialogProcChildMemoryRun]   ; R9  = Parm#4 = Pointer to dialogue proced.
mov edx,IDD_CHILD_MEMORY_RUN        ; RDX = Parm#2 = Resource ( template ) id

.entryDialog:
push 0 0                            ; Parm#5 = Pass value, plus alignment qword
mov r8,rbx ; PARM_HWNDDLG           ; R8  = Parm#3 = Owner window handle
mov rcx,[r15 + APPDATA.hResources]  ; RCX = Parm#1 = Handle of resource module  
sub rsp,32                          ; Allocate parameters shadow
call [DialogBoxParam]               ; Create modal dialogue 
add rsp,32 + 16                     ; Remove parameters shadow and 2 parameters
jmp .processed

.memoryDraw:
mov ax,BINDER_GET_MEMORY
call Binder

; call ScenarioMemoryDraw

; lea r9,[DialogProcChildMemoryDraw]
; mov edx,IDD_CHILD_MEMORY_DRAW

; jmp .entryDialog

call DrawSessionStart

push 0 0                            ; Parm#5 = Pass value, plus alignment qword
lea r9,[DialogProcChildMemoryDraw]
mov r8,rbx  ; PARM_HWNDDLG          ; R8  = Parm#3 = Owner window handle
mov edx,IDD_CHILD_MEMORY_DRAW
mov rcx,[r15 + APPDATA.hResources]  ; RCX = Parm#1 = Handle of resource module  
sub rsp,32                          ; Allocate parameters shadow
call [DialogBoxParam]               ; Create modal dialogue 
add rsp,32 + 16                     ; Remove parameters shadow and 2 parameters

call DrawSessionStop

jmp .processed



.mathRun:

call ScenarioVectorBrief


;---
; lea r9,[DialogProcChildMathRun]
; mov edx,IDD_CHILD_MATH_RUN
;---
 lea r9,[DialogProcChildVectorBrief]
 mov edx,IDD_CHILD_VECTOR_BRIEF
;---
jmp .entryDialog

.mathDraw:

; lea r9,[DialogProcChildMathDraw]
; mov edx,IDD_CHILD_MATH_DRAW

 lea r9,[DialogProcChildVectorBrief]
 mov edx,IDD_CHILD_VECTOR_BRIEF

jmp .entryDialog

;---------- WM_CLOSE handler: close window ------------------------------------;

.wmclose:
mov rcx,[r15 + APPDATA.hMain]
jrcxz .processed
xor r9d,r9d
xor r8d,r8d
mov edx,WM_CLOSE
call [SendMessage]

;---------- Exit points -------------------------------------------------------;

.processed:
mov eax,1
.finish:
mov rsp,rbp
pop r15 rdi rsi rbx rbp
ret


