;==============================================================================;
;                                                                              ;
;           Kernel Mode Driver for NCRB ( NUMA CPU&RAM Benchmarks ).           ;
;                     Service Control Program (SCP).                           ;
;               This edition for WoW64 (Windows-on-Windows):                   ;
;                     32-bit SCP access to 64-bit driver.                      ;
;      Debug example: load driver, read IA32_APIC_BASE MSR, unload driver.     ; 
;                           (C)2021 Ilya Manusov.                              ;
;                           manusov1969@gmail.com                              ;
;                                                                              ;
;        Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 ).       ;
;                         http://flatassembler.net/                            ;
;                                                                              ;
;       Edit by FASM Editor 2.0, use this editor for correct tabulations.      ;
;              https://fasmworld.ru/instrumenty/fasm-editor-2-0/               ;
;                                                                              ;
;              Special thanks to @L.CHEMIST ( Andrey A. Meshkov )              ;
;   for Kernel Mode Driver (KMD) and Service Control Program (SCP) examples    ;
;                        http://maalchemist.narod.ru                           ;
;                                                                              ;
;                                                                              ;
;         IMPORTANT NOTE. NON SIGNED KERNEL MODE DRIVERS LIMITATIONS.          ;
;      For Windows 7 x64 can use "Disable driver signature enforcement".       ;
;                For Windows 10 x64 required test signing.                     ;
;                   For Windows XP x64 no limitations.                         ;
;    For Windows XP ia32,  Windows 7 ia32,  Windows 10 ia32 no limitations.    ;
;                                                                              ;
;==============================================================================;

;------------------------------------------------------------------------------;
;                               Definitions.                                   ;
;      Note SCP = Service Control Program, SCM = Service control Manager.      ;
;------------------------------------------------------------------------------;

; FASM definitions 
include 'WIN32A.INC'
; Application definitions 
TEXT_BUFFER_SIZE = 512     ; Buffer size for text strings build
PATH_BUFFER_SIZE = 260     ; Buffer size for driver file string path build
; Kernel Mode Driver (KMD) definitions
RZ_DRIVER_QUERY_BUFFER_SIZE = 24     ; Buffer size for driver request structure
RZ_REQUEST_CODE             = 41h    ; Driver request code = user routine call 
; Service Control Program (SCP) definitions
SC_MANAGER_ALL_ACCESS = 0000F003Fh   ; Used as desired access rights for SCM
SERVICE_ALL_ACCESS    = 0000F01FFh   ; Used as desired acc. rights for service
SERVICE_KERNEL_DRIVER = 000000001h   ; Used as service type for service
SERVICE_DEMAND_START  = 000000003h   ; Used as service start option for service
SERVICE_ERROR_NORMAL  = 000000001h   ; Used as error control option for service
SERVICE_CONTROL_STOP  = 000000001h   ; Used as control code for stop service
SERVICE_RUNNING       = 000000004h   ; Used for detect service current state
; structure for service request execution status (see MSDN)
struct SERVICE_STATUS
 dwServiceType              dd ?     ; Type of system service
 dwCurrentState             dd ?     ; State of service, run/stop/pause
 dwControlsAccepted         dd ?     ; Accepted service operations flags
 dwWin32ExitCode            dd ?     ; Service unified error code
 dwServiceSpecificExitCode  dd ?     ; Service-specific error code
 dwCheckPoint               dd ?     ; Incremnted progress indicator value
 dwWaitHint                 dd ?     ; Estimated time of operation for tracking
ends
; Structure for driver query
struct SERVICE_QUERY
 iocode    dd ?   ; user I/O code, request type selector
 iodata    dd ?   ; user I/O data, request input parameter 
 userproc  dq ?   ; procedure offset, callback address
 parm1     dq ?   ; parameter A, callback routine optional input parameter 1
 parm2     dq ?   ; parameter B, callback routine optional input parameter 2
 result    dq ?   ; result, usage example: AL after IN AL,DX
 buffer    db RZ_DRIVER_QUERY_BUFFER_SIZE dup (?)
ends

;------------------------------------------------------------------------------;
;                                Code section.                                 ;
;------------------------------------------------------------------------------;

format PE GUI 4.0
entry start
section '.text' code readable executable
start:
; Entry point
cld
; Initializing full path to driver file
push DrvFile             ; Parm#4 = Pointer to pointer to updated path 
push [DrvPath]           ; Parm#3 = Pointer to buffer
push PATH_BUFFER_SIZE    ; Parm#2 = Buffer size
push DrvFileName         ; Parm#1 = Pointer to file name
call [GetFullPathNameA]
test eax,eax
jz .errorGetPath         ; Go if error status returned when get path 
; Open Service Control Manager (SCM)
push SC_MANAGER_ALL_ACCESS  ; Parm#3 = Desired access rights  
push 0                   ; Parm#2 = Pointer to database name, 0 = not used
push 0                   ; Parm#1 = Pointer to machine name, 0 = not used
call [OpenSCManagerA]
test eax,eax
jz .errorOpenScm         ; Go if error status returned when open SCM
mov [Manager],eax 
; Try Open Service, check for already exist
push SERVICE_ALL_ACCESS  ; Parm#3 = Desired access rights
push DrvName             ; Parm#2 = Pointer to service name
push eax                 ; Parm#1 = Service handle
call [OpenServiceA]
test eax,eax
jnz .skipCreate          ; Go if open success, means this service already exist
; Create Service
xor eax,eax             ; EAX = 0 for compact push 0
push eax                ; Parm#13 = Pointer to password, 0 = not used
push eax                ; Parm#12 = Pointer to service name, 0 = default
push eax                ; Parm#11 = Pointer to dependencies, 0 = none
push eax                ; Parm#10 = Pointer to Tag ID, 0 = none
push eax                ; Parm#9  = Pointer to Load Order Groups list, 0 = none
push [DrvPath]          ; Parm#8  = Pointer to driver binary file path
push SERVICE_ERROR_NORMAL   ; Parm#7 = Error control option
push SERVICE_DEMAND_START   ; Parm#6 = Service start option
push SERVICE_KERNEL_DRIVER  ; Parm#5 = Service type
push SERVICE_ALL_ACCESS     ; Parm#4 = Desired access
push DrvName                ; Parm#3 = Pointer to display name string
push DrvName                ; Parm#2 = Pointer to service name string
push [Manager]              ; Parm#1 = Handle to SCM database
call [CreateServiceA]
test eax,eax
jz .errorCreateService  ; Go if error status returned when create service
; Skip create point
.skipCreate:
mov [Service],eax 
; Check service status
lea ebx,[Status]
push ebx                   ; Parm#2 = Pointer to status structure
push eax                   ; Parm#1 = Handle
call [QueryServiceStatus]
test eax,eax
jz .errorGetStatus         ; Go if error status returned when get status
cmp [ebx + SERVICE_STATUS.dwCurrentState],SERVICE_RUNNING
je .skipStart              ; Go if service already running
; Start service
push Vectors               ; Parm#3 = Pointer to arguments vectors, empty 
push 0                     ; Parm#2 = Arguments count
push [Service]             ; Parm#1 = Handle
call [StartService]
test eax,eax
jz .errorStartService      ; Go if error status returned when start service
; Skip start point
.skipStart:
; Create device file
xor eax,eax                    ; EAX = 0 for compact push 0
push eax                       ; Parm#7 = Template file handle, 0 = not used
push eax                       ; Parm#6 = Flags and attributes, 0 = not used
push OPEN_EXISTING             ; Parm#5 = Creation disposition 
push 0                         ; Parm#4 = Security attributes  
push FILE_SHARE_READ or FILE_SHARE_WRITE  ; Parm#3 = Share mode 
push GENERIC_WRITE or GENERIC_READ        ; Parm#2 = Desired access
push DrvDevice                 ; Parm#1 = Pointer to file=device name
call [CreateFile]
test eax,eax
jz .errorCreateDevice   ; Go if error status returned when create device file
cmp eax,INVALID_HANDLE_VALUE
je .errorCreateDevice   ; Go if invalid handle returned when create device file
mov [Driver],eax
; Write to device file, this is requests for driver
push 0                         ; Parm#5 = Pointer to overlapped, 0 = not used
push Bytes                     ; Parm#4 = Pointer to byte count variable
push sizeof.SERVICE_QUERY      ; Parm#3 = Requested byte count
lea edx,[Query]
push edx                       ; Parm#2 = Pointer to file buffer=request
push [Driver]                  ; Parm#1 = File=device handle
mov eax,RZ_REQUEST_CODE
mov dword [edx + SERVICE_QUERY.iocode],eax
lea eax,[DataPattern]
mov dword [edx + SERVICE_QUERY.parm1],eax
lea eax,[kmdRoutine]
mov dword [edx + SERVICE_QUERY.userproc],eax
mov dword [edx + SERVICE_QUERY.userproc + 4],0   ; Required for WoW 32/64
call [WriteFile]
test eax,eax
jz .errorWriteDevice           ; Go if error status returned when send request
; Initialization and execution done
xor eax,eax
jmp @f
; Errors handling points, set error code R15 = pointer to error string 
.errorGetPath:
lea eax,[ErrorGetPath]
jmp @f
.errorOpenScm:
lea eax,[ErrorOpenScm]
jmp @f
.errorCreateService:
lea eax,[ErrorCreateService]
jmp @f
.errorGetStatus:
lea eax,[ErrorGetStatus]
jmp @f
.errorStartService:
lea eax,[ErrorStartService]
jmp @f
.errorCreateDevice:
lea eax,[ErrorCreateDevice]
jmp @f
.errorWriteDevice:
lea eax,[ErrorWriteDevice]
@@:
; Conditionally assign R14 = OS error code
mov [ErrorStep],eax
mov [ErrorCode],0
call GetOsErrorCode
; Close device file, uninstall driver. Note close operations sequence continued
; even if error, but error logged at R15 if R15 = 0 (if yet no errors) 
mov ecx,[Driver]
jecxz @f
cmp ecx,INVALID_HANDLE_VALUE
je @f
push ecx                    ; Parm#1 = Manager handle
call [CloseHandle]
test eax,eax
jnz @f                      ; Go if no errors
lea eax,[ErrorCloseDevice]
call GetOsErrorCode
@@:
; Stop service
mov ebx,[Service]
test ebx,ebx 
jz .skipStop                ; Go skip if handle = 0
push Status                 ; Parm#3 = Pointer to status structure
push SERVICE_CONTROL_STOP   ; Parm#2 = Control code
push ebx                    ; Parm#1 = Handle
call [ControlService]
test eax,eax
jnz @f                      ; Go if no errors
lea eax,[ErrorStopService]
call GetOsErrorCode
@@:
; Delete service
push ebx                    ; Parm#1 = Service handle
call [DeleteService]
test eax,eax
jnz @f                      ; Go if no errors
lea eax,[ErrorDeleteService]
call GetOsErrorCode
@@:
; Close service handle
push ebx                    ; Parm#1 = Service handle
call [CloseServiceHandle]
test eax,eax
jnz @f                      ; Go if no errors
lea eax,[ErrorCloseService]
call GetOsErrorCode
@@:
; Skip stop point
.skipStop:
; Close manager handle 
mov ecx,[Manager]
jecxz @f
push ecx                    ; Parm#1 = Manager handle
call [CloseServiceHandle]
test eax,eax
jnz @f                      ; Go if no errors
lea eax,[ErrorCloseManager]
call GetOsErrorCode
@@:
; De-Initialization done, check results, select normal result or error message
cmp [ErrorStep],0
jne .errorDetected     ; Go error message mode if errors detected
; Normal result message mode  
lea edi,[TEXT_BUFFER]  ; EDI = Pointer to text buffer
lea esi,[MsrName]
call StringWrite
mov eax,dword [DataPattern + 0]  ; EAX = Data after read MSR, low 32 bits 
mov edx,dword [DataPattern + 4]  ; EDX = Data after read MSR, high 32 bits
call HexPrint64        ; Build hexadecimal string
mov ax,0000h + 'h'
stosw                  ; Write "h" and terminator byte 0
push 0                 ; Parm #4 = Message flags
push WinCaption        ; Parm #3 = Caption (upper message)
jmp .done              ; Go exit
; Errors handling
.errorDetected:
lea edi,[TEXT_BUFFER]
lea esi,[ErrorString]
call StringWrite
mov esi,[ErrorStep]    ; ESI = Pointer to text string about failed step
call StringWrite
lea esi,[FailedString]
call StringWrite
lea esi,[ErrorCodeString]
call StringWrite
mov eax,[ErrorCode]    ; EAX = OS error code
call HexPrint32
mov ax,0000h + 'h'
stosw                  ; Write "h" and terminator byte 0
push MB_ICONERROR      ; Parm #4 = Message flags
push 0                 ; Parm #3 = Caption (upper message) 
; Show GUI box and exit
.done:
push TEXT_BUFFER       ; Parm #2 = Message
push 0                 ; Parm #1 = Parent window
call [MessageBoxA]
push 0
call [ExitProcess]

;------------------------------------------------------------------------------;
;       Fragment executed at kernel mode, as callback from KMD handler.        ;
;               This is debug example: read IA32_APIC_BASE MSR.                ;
;                                                                              ;
; INPUT:     EAX = Pointer to data buffer                                      ;
; OUTPUT:    8 bytes at [EAX] = IA32_APIC_BASE MSR value, 64-bit               ;
; Destroyed: EAX, EBX, ECX, EDX.                                               ; 
;------------------------------------------------------------------------------;
kmdRoutine:
mov ebx,eax          ; EBX = Pointer for store data after read MSR
mov ecx,01Bh         ; ECX = address of IA32_APIC_BASE MSR
rdmsr                ; Read MSR selected by ECX
mov [ebx + 0],eax    ; Store bits [31-00] of MSR
mov [ebx + 4],edx    ; Store bits [63-32] of MSR 
retn

;---
; Test fragment for detect IA32 or X64 mode for callback subroutine.
; Return 4 if PUSH EAX interpreted as PUSH EAX means ESP - 4
; Return 8 if PUSH EAX interpreted as PUSH RAX means RSP - 8
; mov ebx,eax
; mov ecx,esp
; push eax
; sub ecx,esp
; pop eax
; mov [ebx],ecx
; retn
;---



;------------------------------------------------------------------------------;
;                                Libraries                                     ;
;------------------------------------------------------------------------------;

;---------- Get OS error code --------------------------------------;
; INPUT:   EAX = Pointer to error step name                         ;   
;          [ErrorStep] = Error name string, 0 if no errors          ;
;          [ErrorCode] = OS error code, 0 if yet not assigned       ;
; OUTPUT:  [ErrorCode] updated by OS error code, if error detected  ;
;          ( [ErrorStep] != 0 ) and                                 ; 
;          code yet not assigned ( input [ErrorCode] = 0 )          ;  
;-------------------------------------------------------------------;
GetOsErrorCode:
push eax ecx edx
lea ecx,[ErrorStep]
xor edx,edx
cmp [ecx],edx           ; cmp [ErrorStep],0
jne @f                  ; Go if error step already assigned
mov [ecx],eax
@@:
cmp [ecx],edx           ; cmp [ErrorStep],0
je @f                   ; Skip if no errors
cmp [ecx + 4],edx       ; cmp [ErrorCode],0
jne @f                  ; Skip if already assigned
call [GetLastError]
mov [ErrorCode],eax
@@:
pop edx ecx eax
ret

;---------- Copy text string terminated by 00h ----------------;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   ESI = Source address                                ;
;          EDI = Destination address                           ;
; OUTPUT:  ESI = Modified by copy                              ;
;          EDI = Modified by copy                              ;
;          Memory at [Input EDI] modified                      ; 
;--------------------------------------------------------------;
StringWrite:
cld
@@:
lodsb
cmp al,0
je @f
stosb
jmp @b
@@:
ret

;---------- Print 64-bit Hex Number ---------------------------;
; INPUT:  EDX:EAX = Number, EDX=High32, EAX=Low32              ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint64:
xchg eax,edx
call HexPrint32
xchg eax,edx
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------------------------;
; INPUT:  EAX = Number                                         ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint32:
push eax
ror eax,16
call HexPrint16
pop eax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------------------------;
; INPUT:  AX  = Number                                         ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint16:
push eax
xchg al,ah
call HexPrint8
pop eax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------------------------;
; INPUT:  AL  = Number                                         ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify	                                       ;
;--------------------------------------------------------------;
HexPrint8:
push eax
ror al,4
call HexPrint4
pop eax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------------------------;
; INPUT:  AL  = Number (bits 0-3)                              ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint4:
cld
push eax
and al,0Fh
add al,90h
daa
adc al,40h
daa
stosb
pop eax
ret

;------------------------------------------------------------------------------;
;                              Data section.                                   ;
;------------------------------------------------------------------------------;
section '.data' data readable writeable
; Constants, messages text strings 
WinCaption            DB  '  Kernel Mode Driver test (SCP32+KMD64)' , 0
MsrName               DB  'IA32_APIC_BASE MSR = '            , 0
ErrorString           DB  'SCP32 error: '                    , 0
FailedString          DB  ' FAILED.'                         , 0
ErrorCodeString       DB  13, 10, 'OS error code: '          , 0
; Names for initialization errors                           
ErrorGetPath          DB  'get driver file path'             , 0
ErrorOpenScm          DB  'open SCM'                         , 0
ErrorCreateService    DB  'create service'                   , 0
ErrorGetStatus        DB  'get service status'               , 0
ErrorStartService     DB  'start service'                    , 0
ErrorCreateDevice     DB  'create device'                    , 0
ErrorWriteDevice      DB  'write device'                     , 0
; Names for de-initialization errors
ErrorCloseDevice      DB  'close device'                     , 0
ErrorStopService      DB  'stop service'                     , 0
ErrorDeleteService    DB  'delete service'                   , 0
ErrorCloseService     DB  'close service'                    , 0
ErrorCloseManager     DB  'close SCM'                        , 0
; Driver file and service names
DrvFileName           DB  'KMD64.SYS'  , 0  ; Changed for SCP32 + KMD64
DrvName               DB  'ICR0'       , 0
DrvDevice             DB  '\\.\ICR0'   , 0
; Driver control constants and variables with pre-defined values
DrvPath               DD  PATH_BUFFER
DrvFile               DD  0
Manager               DD  0
Service               DD  0
Vectors               DD  0
Driver                DD  INVALID_HANDLE_VALUE
Bytes                 DD  0
; This qword used for data after read IA32_APIC_BASE MSR
DataPattern           DQ  1111111111111111h
; Status and error details information, must be located sequentally, addressing
ErrorStep             DD  ?              ; Pointer to error step name
ErrorCode             DD  ?              ; OS error code
; Continue data section, variables without pre-defined values
Status                SERVICE_STATUS    ; Driver status structure
Query                 SERVICE_QUERY     ; Driver request structure
; Buffer used for text strings 
align 16
TEXT_BUFFER           DB  TEXT_BUFFER_SIZE DUP (?)
; Buffer used for driver file path
align 16
PATH_BUFFER           DB  PATH_BUFFER_SIZE DUP (?)

;------------------------------------------------------------------------------;
;                              Import section.                                 ;
;------------------------------------------------------------------------------;
section '.idata' import data readable writeable
library kernel32, 'KERNEL32.DLL', advapi32, 'ADVAPI32.DLL', \  
        user32, 'USER32.DLL', gdi32, 'GDI32.DLL'
include 'api\kernel32.inc'  ; Win API, OS standard kernel functions
include 'api\advapi32.inc'  ; Win API, advanced API functions
include 'api\user32.inc'    ; Win API, user interface
include 'api\gdi32.inc'     ; Win API, graphics 

