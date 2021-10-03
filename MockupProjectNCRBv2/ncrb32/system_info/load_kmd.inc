; UNDER CONSTRUCTION.

LoadKernelModeDriver:
push ebx esi edi ebp
mov ebx,[Registry]

;---------- Initializing data structure ---------------------------------------;

cld
mov edi,ebx
mov ecx,sizeof.SCPDATA
mov al,0
rep stosb
mov eax,[eax + REGISTRY32.allocatorTempBuffer.objectStart]
mov [ebx + REGISTRY32.scpData.drvPath],eax
mov [ebx + REGISTRY32.scpData.driver],INVALID_HANDLE_VALUE

;---------- Initializing full path to driver file -----------------------------;

mov esi,[ebx + REGISTRY32.appData.lockedStrings]
mov ax,STR_NAME_KMD32
call IndexString
lea eax,[ebx + REGISTRY32.scpData.drvFile] 
push eax                     ; Parm#4 = Pointer to pointer to updated path 
push [ebx + REGISTRY32.scpData.drvPath]  ; Parm#3 = Pointer to buffer
push SCP_PATH_BUFFER_SIZE    ; Parm#2 = Buffer size
push esi                     ; Parm#1 = Pointer to file name
call [GetFullPathNameA]
test eax,eax
jz .errorGetPath             ; Go if error status returned when get path 

;---------- Open Service Control Manager (SCM) --------------------------------;

push SC_MANAGER_ALL_ACCESS  ; Parm#3 = Desired access rights  
push 0                      ; Parm#2 = Pointer to database name, 0 = not used
push 0                      ; Parm#1 = Pointer to machine name, 0 = not used
call [OpenSCManagerA]
test eax,eax
jz .errorOpenScm            ; Go if error status returned when open SCM
mov [ebx + REGISTRY32.scpData.manager],eax 

;---------- Try Open Service, check for already exist -------------------------;

push eax
mov esi,[ebx + REGISTRY32.appData.lockedStrings]
mov ax,STR_DRV_NAME
call IndexString
pop eax

push SERVICE_ALL_ACCESS  ; Parm#3 = Desired access rights
push esi                 ; Parm#2 = Pointer to service name
push eax                 ; Parm#1 = Service handle
call [OpenServiceA]      
test eax,eax
jnz .skipCreate          ; Go if open success, means this service already exist

;---------- Create Service ----------------------------------------------------;

xor eax,eax             ; EAX = 0 for compact push 0
push eax                ; Parm#13 = Pointer to password, 0 = not used
push eax                ; Parm#12 = Pointer to service name, 0 = default
push eax                ; Parm#11 = Pointer to dependencies, 0 = none
push eax                ; Parm#10 = Pointer to Tag ID, 0 = none
push eax                ; Parm#9  = Pointer to Load Order Groups list, 0 = none
push [ebx + REGISTRY32.scpData.drvPath]   ; Parm#8  = Pointer to driver binary file path
push SERVICE_ERROR_NORMAL   ; Parm#7 = Error control option
push SERVICE_DEMAND_START   ; Parm#6 = Service start option
push SERVICE_KERNEL_DRIVER  ; Parm#5 = Service type
push SERVICE_ALL_ACCESS     ; Parm#4 = Desired access
push esi                ; Parm#3 = Pointer to display name string
push esi                ; Parm#2 = Pointer to service name string
push [ebx + REGISTRY32.scpData.manager]  ; Parm#1 = Handle to SCM database
call [CreateServiceA]
test eax,eax
jz .errorCreateService  ; Go if error status returned when create service

;---------- Skip create point -------------------------------------------------;

.skipCreate:
mov [ebx + REGISTRY32.scpData.service],eax 

;---------- Check service status ----------------------------------------------;

lea edi,[ebx + REGISTRY32.scpData.status]
push edi                    ; Parm#2 = Pointer to status structure
push eax                    ; Parm#1 = Handle
call [QueryServiceStatus]
test eax,eax
jz .errorGetStatus          ; Go if error status returned when get status
cmp [edi + SERVICE_STATUS.dwCurrentState],SERVICE_RUNNING
je .skipStart               ; Go if service already running

;---------- Start service -----------------------------------------------------;

lea eax,[ebx + REGISTRY32.scpData.vectors]
push eax                    ; Parm#3 = Pointer to arguments vectors, empty 
push 0                      ; Parm#2 = Arguments count
push [ebx + REGISTRY32.scpData.service]   ; Parm#1 = Handle
call [StartService]
test eax,eax
jz .errorStartService      ; Go if error status returned when start service

;---------- Skip start point --------------------------------------------------;

.skipStart:

;---------- Create device file ------------------------------------------------;

mov esi,[ebx + REGISTRY32.appData.lockedStrings]
mov ax,STR_DRV_DEVICE
call IndexString

xor eax,eax                    ; EAX = 0 for compact push 0
push eax                       ; Parm#7 = Template file handle, 0 = not used
push eax                       ; Parm#6 = Flags and attributes, 0 = not used
push OPEN_EXISTING             ; Parm#5 = Creation disposition 
push 0                         ; Parm#4 = Security attributes  
push FILE_SHARE_READ or FILE_SHARE_WRITE  ; Parm#3 = Share mode 
push GENERIC_WRITE or GENERIC_READ        ; Parm#2 = Desired access
push esi                      ; Parm#1 = Pointer to file=device name
call [CreateFile]
test eax,eax
jz .errorCreateDevice   ; Go if error status returned when create device file
cmp eax,INVALID_HANDLE_VALUE
je .errorCreateDevice   ; Go if invalid handle returned when create device file
mov [ebx + REGISTRY32.scpData.driver],eax

; TODO. Errors Handling.
.errorGetPath:
.errorOpenScm:
.errorCreateService:
.errorGetStatus:
.errorStartService:
.errorCreateDevice:

pop ebp edi esi ebx
ret


; UNDER CONSTRUCTION.

UnloadKernelModeDriver:
push ebx esi edi ebp
mov ebx,[Registry]

;---------- Close device file, uninstall driver -------------------------------; 
; Note close operations sequence continued even if error, but error logged
; at R15 if R15 = 0 (if yet no errors) TODO. CORRECT, NO R15 FOR IA32. 

mov ecx,[ebx + REGISTRY32.scpData.driver]
jecxz @f
cmp ecx,INVALID_HANDLE_VALUE
je @f
push ecx                    ; Parm#1 = Manager handle
call [CloseHandle]
test eax,eax
jnz @f                      ; Go if no errors
; lea eax,[ErrorCloseDevice]
; call GetOsErrorCode
@@:

;---------- Stop service ------------------------------------------------------;

mov edi,[ebx + REGISTRY32.scpData.service]
test edi,edi 
jz .skipStop                ; Go skip if handle = 0
lea eax,[ebx + REGISTRY32.scpData.status]
push eax                    ; Parm#3 = Pointer to status structure
push SERVICE_CONTROL_STOP   ; Parm#2 = Control code
push edi                    ; Parm#1 = Handle
call [ControlService]
test eax,eax
jnz @f                      ; Go if no errors
; lea eax,[ErrorStopService]
; call GetOsErrorCode
@@:

;---------- Delete service ----------------------------------------------------;

push edi                    ; Parm#1 = Service handle
call [DeleteService]
test eax,eax
jnz @f                      ; Go if no errors
; lea eax,[ErrorDeleteService]
; call GetOsErrorCode
@@:

;---------- Close service handle ----------------------------------------------;

push edi                    ; Parm#1 = Service handle
call [CloseServiceHandle]
test eax,eax
jnz @f                      ; Go if no errors
; lea eax,[ErrorCloseService]
; call GetOsErrorCode
@@:

;---------- Skip stop point ---------------------------------------------------;

.skipStop:

;---------- Close manager handle ----------------------------------------------; 

mov ecx,[ebx + REGISTRY32.scpData.manager]
jecxz @f
push ecx                    ; Parm#1 = Manager handle
call [CloseServiceHandle]
test eax,eax
jnz @f                      ; Go if no errors
; lea eax,[ErrorCloseManager]
; call GetOsErrorCode
@@:

pop ebp edi esi ebx
ret

