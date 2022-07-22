;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                   connect include files with subroutines.                    ; 
;------------------------------------------------------------------------------;

;--- Kernel Mode Shell application scenario -----------------------------------;
; Input/Output parameters and volatile registers compatible with               ; 
; Microsoft x64 calling convention.                                            ;
; INPUT:   RCX = Pointer to HardwareShell status variables block               ;
;          RDX = Pointer to HardwareShell temporary buffer                     ;
; OUTPUT:  RAX = Status, 0=Error, otherwise no errors                          ;
;------------------------------------------------------------------------------;
ApplicationKmdShell:
push rbx rsi rdi rbp r12 r13 r14 r15 
mov r15,rcx                   ; R15 = Pointer to error status block
mov rsi,rcx
mov rdi,rdx
xor eax,eax
mov ERROR_STRING_1,rax
mov ERROR_STRING_2,rax
mov ERROR_CODE,rax
;---------- Skip if inconsistent action selector option -----------------------;
cmp [OptionAction],OPTION_ACTION_LIMIT
ja .kmdReturn
;---------- Load driver -------------------------------------------------------;
call LoadKernelModeDriver
test eax,eax
jz .kmdLoadError
;---------- Blank buffer, also required for prevent page faults at ring 0 -----; 
push rdi
add rdi,KERNEL_BUFFER_OFFSET
mov ecx,KERNEL_BLOCK_LIMIT / 8 
mov rax,1111111111111111h
cld
rep stosq
pop rdi
;---------- Call target operation handler -------------------------------------;
; Operation, results dump, internal error messages at this subroutine.
push rsi rdi r15
movzx eax,[OptionAction]
call qword [ActionsHandlers + rax * 8]
pop r15 rdi rsi
;---------- Unload driver -----------------------------------------------------;
mov rcx,rsi
call UnloadKernelModeDriver
;---------- Exit points -------------------------------------------------------;
.kmdReturn:
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret
.kmdLoadError:
push qword [rsi + 00]
push qword [rsi + 08]
push qword [rsi + 16]
mov rcx,rsi
mov rdx,rdi
call UnloadKernelModeDriver
pop qword [rsi + 16]
pop qword [rsi + 08]
pop qword [rsi + 00]
xor eax,eax
jmp .kmdReturn

;---------- Load Kernel Mode Driver -------------------------------------------;
; This loader for NCRB64 loads 64-bit driver only.                             ;
; Note loader for NCRB32 loads 32-bit driver if Win32,                         ;
; or 64-bit driver if Win64 with WoW64 detected (if NCRB32 under Win64).       ;
;                                                                              ;
; INPUT:   RCX = Pointer to error status variables, for error reporting:       ;
;                3 QWORDS, 2 pointers to strings + 1 OS API error code.        ;         
;                                                                              ;
; OUTPUT:  RAX = Win API Status, 0 means error, non zero means no errors.      ;
;          RCX = Win API detail error code, valid if errors (when RAX=0 only). ;
;          DX  = Error name string ID, valid if errors (when RAX=0 only).      ;  
;                                                                              ;
;------------------------------------------------------------------------------;
LoadKernelModeDriver:
push rbx rsi rdi rbp r14 r15 rcx
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
lea rbx,[SCP_DATA]
;---------- Initializing data structure ---------------------------------------;
cld
mov rdi,rbx
mov ecx,sizeof.SCPDATA
mov al,0
rep stosb
lea rax,[TEMP_BUFFER]
mov [rbx + SCPDATA.drvPath],rax
mov [rbx + SCPDATA.driver],INVALID_HANDLE_VALUE
;---------- Initializing full path to driver file -----------------------------;
lea rcx,[STRING_NAME_KMD64]
mov edx,SCP_PATH_BUFFER_SIZE   ; RDX = Parm#2 = Buffer size 
mov r8,[rbx + SCPDATA.drvPath] ; R8  = Parm#3 = Pointer to buffer
lea r9,[rbx + SCPDATA.drvFile] ; R9  = Parm#4 = Pointer to pointer to file name 
call [GetFullPathNameA]
test rax,rax
jz errorGetPath                ; Go if error status returned when get path 
;---------- Open Service Control Manager (SCM) --------------------------------;
xor ecx,ecx             ; RCX = Parm#1 = Pointer to machine name, 0 = not used 
xor edx,edx             ; RDX = Parm#2 = Pointer to database name, 0 = not used
mov r8d,SC_MANAGER_ALL_ACCESS  ; R8 = Parm#3 = Desired access rights  
call [OpenSCManagerA]
test rax,rax
jz errorOpenScm         ; Go if error status returned when open SCM
mov [rbx + SCPDATA.manager],rax 
;---------- Try Open Service, check for already exist -------------------------;
xchg rcx,rax
lea rdx,[STRING_DRV_NAME]
mov r8d,SERVICE_ALL_ACCESS        ; R8  = Parm#3 = Desired access rights
call [OpenServiceA]
test rax,rax
jnz .skipCreate         ; Go if open success, means this service already exist
;---------- Create Service ----------------------------------------------------;
xor eax,eax             ; RAX = 0 for compact push 0
push rax                ; Alignment
push rax                ; Parm#13 = Pointer to password, 0 = not used
push rax                ; Parm#12 = Pointer to service name, 0 = default
push rax                ; Parm#11 = Pointer to dependencies, 0 = none
push rax                ; Parm#10 = Pointer to Tag ID, 0 = none
push rax                ; Parm#9  = Pointer to Load Order Groups list, 0 = none
push [rbx + SCPDATA.drvPath] ; Parm#8  = Pointer to driver binary file path
push SERVICE_ERROR_NORMAL    ; Parm#7 = Error control option
push SERVICE_DEMAND_START    ; Parm#6 = Service start option
push SERVICE_KERNEL_DRIVER   ; Parm#5 = Service type
mov r9d,SERVICE_ALL_ACCESS   ; Parm#4 = R9  = Desired access
lea r8,[STRING_DRV_NAME]     ; Parm#3 = R8  = Pointer to display name string
mov rdx,r8                   ; Parm#2 = RDX = Pointer to service name string
mov rcx,[rbx + SCPDATA.manager]  ; Parm#1 = RCX = Handle to SCM database
sub rsp,32                   ; Create parameters shadow
call [CreateServiceA]
add rsp,32 + 10 * 8          ; Remove parameters shadow and stack parameters
test rax,rax
jz errorCreateService        ; Go if error status returned when create service
;---------- Skip create point -------------------------------------------------;
.skipCreate:
mov [rbx + SCPDATA.service],rax 
;---------- Check service status ----------------------------------------------;
xchg rcx,rax            ; RCX = Parm#1 = Handle 
lea rdi,[rbx + SCPDATA.status]
mov rdx,rdi             ; RDX = Parm#2 = Pointer to status structure
call [QueryServiceStatus]
test rax,rax
jz errorGetStatus       ; Go if error status returned when get status
cmp [rdi + SERVICE_STATUS.dwCurrentState],SERVICE_RUNNING
je .skipStart           ; Go skip start if service already running
;---------- Start service -----------------------------------------------------;
mov rcx,[rbx + SCPDATA.service]  ; RCX = Parm#1 = Handle
xor edx,edx                      ; RDX = Parm#2 = Arguments count
lea r8,[rbx + SCPDATA.vectors]   ; R8  = Parm#3 = Pointer to arguments vectors, empty 
call [StartService]
test rax,rax
jz errorStartService    ; Go if error status returned when start service
;---------- Skip start point --------------------------------------------------;
.skipStart:
;---------- Create (open) device file -----------------------------------------;
xor eax,eax                    ; RAX = 0 for compact push 0
push rax                       ; Alignment
push rax                       ; Parm#7 = Template file handle, 0 = not used
push rax                       ; Parm#6 = Flags and attributes, 0 = not used
push OPEN_EXISTING             ; Parm#5 = Creation disposition 
xor r9d,r9d                    ; Parm#4 = R9 = Security attributes  
mov r8d,FILE_SHARE_READ or FILE_SHARE_WRITE  ; Parm#3 = R8  = Share mode 
mov edx,GENERIC_WRITE or GENERIC_READ        ; Parm#2 = RDX = Desired access
lea rcx,[STRING_DRV_DEVICE]    ; Parm#1 = RCX = Pointer to file=device name
sub rsp,32
call [CreateFile]
add rsp,32 + 4 * 8
test rax,rax
jz errorCreateDevice    ; Go if error status returned when create device file
cmp rax,INVALID_HANDLE_VALUE
je errorCreateDevice    ; Go if invalid handle returned when create device file
mov [rbx + SCPDATA.driver],rax
;---------- Load kernel mode driver done, return status -----------------------;
; This entry points can be used from other subroutines also (unloader).
kmdOk:
mov eax,1                  ; EAX = 1 means kernel mode driver loaded OK
kmdDone:
push rax rdx
xor r15d,r15d              ; Pre-clear OS error code
mov r14d,1                 ; Set yet no errors
call HelperOsErrorCode     ; Get OS error code if error detected
mov ecx,r15d               ; ECX = OS error code for return
pop rdx rax
;---------- This means loader -------------------------------------------------; 
lea rbx,[STRING_DRV_ERROR_LOAD]
;--- Exit with update error details string if error, or blank if no errors ----;
; This 3 qwords (2 pointers + OS error code) valid only if error, when EAX = 0. 
; RBX = Offset string 1
; RDX = Offset string 2
kmdExitStrings:
test eax,eax
jnz kmdReturn     ; Skip error decoding if no errors
mov rdi,[rbp]
push rax rcx rdx
mov [rdi + 16],rcx
mov [rdi + 00],rbx
mov qword [rdi + 08],0
cmp rdx,-1                  ; replave to RDX ?
je @f
mov [rdi + 08],rdx
@@:
pop rdx rcx rax
;---------- Shared return point -----------------------------------------------;
kmdReturn:
mov rsp,rbp
pop rbx r15 r14 rbp rdi rsi rbx
ret
;---------- Errors Handling ---------------------------------------------------;
; This entry points can be used from other subroutines also (unloader).
errorGetPath:
lea rdx,[STRING_DRE_GET_PATH]
jmp kmdFailed
errorOpenScm:
lea rdx,[STRING_DRE_OPEN_SERVICE]
jmp kmdFailed
errorCreateService:
lea rdx,[STRING_DRE_CREATE_SERVICE]
jmp kmdFailed
errorGetStatus:
lea rdx,[STRING_DRE_GET_STATUS]
jmp kmdFailed
errorStartService:
lea rdx,[STRING_DRE_START_SERVICE]
jmp kmdFailed
errorCreateDevice:
lea rdx,[STRING_DRE_CREATE_DEVICE]
kmdFailed:
xor eax,eax          ; EAX = 0 means error loading kernel mode driver
jmp kmdDone

;---------- Unload Kernel Mode Driver -----------------------------------------;
;                                                                              ;
; INPUT:   RCX = Pointer to error status variables, for error reporting:       ;
;                3 QWORDS, 2 pointers to strings + 1 OS API error code.        ;         
;                                                                              ;
; OUTPUT:  RAX = Win API Status, 0 means error, non zero means no errors.      ;
;          RCX = Win API detail error code, valid if errors (when RAX=0 only). ;
;          DX  = Reserved.                                                     ;  
;                                                                              ;
;------------------------------------------------------------------------------;
UnloadKernelModeDriver:
push rbx rsi rdi rbp r14 r15 rcx
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
lea rbx,[SCP_DATA]
xor r15d,r15d                     ; Pre-clear OS error code
mov r14d,1                        ; Set yet no errors
;---------- Close device file, uninstall driver -------------------------------;
; Note close operations sequence continued even if error, 
; at R14D, R15D, if R14D = 1 yet no previous errors.
mov rcx,[rbx + SCPDATA.driver]    ; RCX = Parm#1 = Manager handle
jrcxz @f
cmp rcx,INVALID_HANDLE_VALUE
je @f
call [CloseHandle]
call HelperOsErrorCode            ; Log OS error code: R15, R14
@@:
;---------- Stop service ------------------------------------------------------;
mov rdi,[rbx + SCPDATA.service]
mov rcx,rdi                       ; RCX = Parm#1 = Handle 
jrcxz .skipStop                   ; Go skip if handle = 0
mov edx,SERVICE_CONTROL_STOP      ; RDX = Parm#2 = Control code
lea r8,[rbx + SCPDATA.status]     ; R8  = Parm#3 = Pointer to status structure
call [ControlService]
call HelperOsErrorCode            ; Log OS error code: R15, R14
;---------- Delete service ----------------------------------------------------;
mov rcx,rdi                       ; RCX = Parm#1 = Service handle
call [DeleteService]
call HelperOsErrorCode            ; Log OS error code: R15, R14
;---------- Close service handle ----------------------------------------------;
mov rcx,rdi                       ; RCX = Parm#1 = Service handle
call [CloseServiceHandle]
call HelperOsErrorCode            ; Log OS error code: R15, R14
;---------- Skip stop point ---------------------------------------------------;
.skipStop:
;---------- Close manager handle ----------------------------------------------;
mov rcx,[rbx + SCPDATA.manager]   ; RCX = Parm#1 = Manager handle
jrcxz @f
call [CloseServiceHandle]
call HelperOsErrorCode            ; Log OS error code: R15, R14
@@:
;---------- Make status -------------------------------------------------------;
mov eax,r14d                   ; RAX = 0 if error, non zero if no errors
mov ecx,r15d                   ; RCX = OS error code, valid if error
lea rbx,[STRING_DRV_ERROR_UNLOAD]
mov rdx,-1                     ; RDX = -1 means not used for unload procedure
jmp kmdExitStrings

;---------- Get OS error code -------------------------------------------------;
;                                                                              ;
; INPUT:   RAX = Status after WinAPI, 0 if error, non zero if OK               ;   
;          R15 = OS error code, 0 if yet not assigned                          ;
;          R14 = Error status, 0 = error, non zero means OK                    ;
;                                                                              ;
; OUTPUT:  R15 updated by OS error code,                                       ; 
;          if error detected ( RAX = 0 ) and                                   ; 
;          if error code yet not assigned ( input R15 = 0 )                    ;
;          R14 cleared to 0 if error                                           ;
;                                                                              ;
; Must not corrupt all registers, include volatile, because called at          ;
; driver load-unload process.                                                  ;
;------------------------------------------------------------------------------;
HelperOsErrorCode:
test rax,rax
jnz @f                      ; Skip error code assignment if no errors
xor r14d,r14d               ; R14 = 0 means error
test r15d,r15d
jnz @f                      ; Skip error code assignment if already assigned 
push rax rcx rdx r8 r9 r10 r11 rbp  ; Save volatile registers and RBP
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h  ; Stack alignment
sub rsp,32                  ; Parameters shadow
call [GetLastError]
xchg r15d,eax
mov rsp,rbp                 ; Restore stack
pop rbp r11 r10 r9 r8 rdx rcx rax
@@:
ret

;---------- Handlers ----------------------------------------------------------;
include 'readers\ReadMemory.inc'
include 'readers\ReadIo.inc'
include 'readers\ReadPci.inc'
include 'readers\ReadPciMcfg.inc'
include 'readers\ReadCrMsr.inc'
include 'readers\ReadCmos.inc'
include 'readers\ReadApic.inc'
include 'readers\ReadSpdAtSmbus.inc'
include 'readers\ReadClkAtSmbus.inc'
