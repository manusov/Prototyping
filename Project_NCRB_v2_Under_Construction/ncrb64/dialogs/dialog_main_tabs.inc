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
lea r15,[APP_DATA]             ; R15 = Pointer to application data structure
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
cmp ax,IDB_SYSINFO_VBRF
je .vectorBriefRun
jmp .processed
;---------- Handler for "Run" button ------------------------------------------;
.memoryRun:
mov ax,BINDER_GET_MEMORY
call Binder
call ScenarioMemoryRun
jc .runtimeError                    ; Go error message box if status bad
lea r9,[DialogProcChildMemoryRun]   ; R9  = Parm#4 = Pointer to dialogue proced.
mov edx,IDD_CHILD_MEMORY_RUN        ; RDX = Parm#2 = Resource ( template ) id
.entryDialog:
push 0 0                            ; Parm#5 = Pass value, plus alignment qword
mov r8,rbx                          ; R8  = Parm#3 = Owner window handle
mov rcx,[r15 + APPDATA.hResources]  ; RCX = Parm#1 = Handle of resource module  
sub rsp,32                          ; Allocate parameters shadow
call [DialogBoxParam]               ; Create modal dialogue 
add rsp,32 + 16                     ; Remove parameters shadow and 2 parameters
jmp .processed
;---------- Handler for "Draw" button -----------------------------------------;
.memoryDraw:
mov ax,BINDER_GET_MEMORY
call Binder
call DrawSessionStart
jc .runtimeError                    ; Go error message box if status bad
push 0 0                            ; Parm#5 = Pass value, plus alignment qword
lea r9,[DialogProcChildMemoryDraw]
mov r8,rbx                          ; R8  = Parm#3 = Owner window handle
mov edx,IDD_CHILD_MEMORY_DRAW
mov rcx,[r15 + APPDATA.hResources]  ; RCX = Parm#1 = Handle of resource module  
sub rsp,32                          ; Allocate parameters shadow
call [DialogBoxParam]               ; Create modal dialogue 
add rsp,32 + 16                     ; Remove parameters shadow and 2 parameters
call DrawSessionStop
jc .runtimeError                    ; Go error message box if status bad
jmp .processed
;---------- Handler for "Vector brief" button ---------------------------------;
.vectorBriefRun:
call ScenarioVectorBrief
jc .runtimeError                    ; Go error message box if status bad
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

;---------- Handler for runtime errors ----------------------------------------;
.runtimeError:
mov rsi,[APP_DATA.lockedStrings]
movzx eax,al
add eax,STR_RUNTIME_ALLOC
call IndexString       ; Return ESI = Selected string address 
mov r9d,MB_ICONERROR   ; R9  = Parm#4 = Attributes
lea r8,[PROGRAM_NAME]  ; R8  = Parm#3 = Pointer to title (caption) string
mov rdx,rsi            ; RDX = Parm#2 = Pointer to string: error name 
mov rcx,rbx            ; RCX = Parm#1 = Parent Window - this window
call [MessageBox]  
jmp .processed
 
