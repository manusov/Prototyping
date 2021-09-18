;==============================================================================;
;                                                                              ;
;   Kernel Mode Driver for NCRB ( NUMA CPU&RAM Benchmarks ). Win64 Edition.    ;
;                     Service Control Program (SCP).                           ;
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
include 'WIN64A.INC'
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
; Structure for service request execution status (see MSDN)
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

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
; Application entry point
sub rsp,8*5
cld
; Initializing full path to driver file
lea rcx,[DrvFileName]     ; RCX = Parm#1 = Pointer to file name
mov edx,PATH_BUFFER_SIZE  ; RDX = Parm#2 = Buffer size 
mov r8,[DrvPath]          ; R8  = Parm#3 = Pointer to buffer
lea r9,[DrvFile]          ; R9  = Parm#4 = Pointer to pointer to updated path 
call [GetFullPathNameA]
test rax,rax
jz .errorGetPath          ; Go if error status returned when get path 
; Open Service Control Manager (SCM)
xor ecx,ecx             ; RCX = Parm#1 = Pointer to machine name, 0 = not used 
xor edx,edx             ; RDX = Parm#2 = Pointer to database name, 0 = not used
mov r8d,SC_MANAGER_ALL_ACCESS  ; R8 = Parm#3 = Desired access rights  
call [OpenSCManagerA]
test rax,rax
jz .errorOpenScm        ; Go if error status returned when open SCM
mov [Manager],rax 
; Try Open Service, check for already exist
xchg rcx,rax                ; RCX = Parm#1 = Service handle
lea rdx,[DrvName]           ; RDX = Parm#2 = Pointer to service name 
mov r8d,SERVICE_ALL_ACCESS  ; R8  = Parm#3 = Desired access rights
call [OpenServiceA]
test rax,rax
jnz .skipCreate         ; Go if open success, means this service already exist
; Create Service
xor eax,eax             ; RAX = 0 for compact push 0
push rax                ; Alignment
push rax                ; Parm#13 = Pointer to password, 0 = not used
push rax                ; Parm#12 = Pointer to service name, 0 = default
push rax                ; Parm#11 = Pointer to dependencies, 0 = none
push rax                ; Parm#10 = Pointer to Tag ID, 0 = none
push rax                ; Parm#9  = Pointer to Load Order Groups list, 0 = none
push [DrvPath]          ; Parm#8  = Pointer to driver binary file path
push SERVICE_ERROR_NORMAL   ; Parm#7 = Error control option
push SERVICE_DEMAND_START   ; Parm#6 = Service start option
push SERVICE_KERNEL_DRIVER  ; Parm#5 = Service type
mov r9d,SERVICE_ALL_ACCESS  ; Parm#4 = R9  = Desired access
lea r8,[DrvName]            ; Parm#3 = R8  = Pointer to display name string
mov rdx,r8                  ; Parm#2 = RDX = Pointer to service name string
mov rcx,[Manager]           ; Parm#1 = RCX = Handle to SCM database
sub rsp,32                  ; Create parameters shadow
call [CreateServiceA]
add rsp,32 + 10 * 8     ; Remove parameters shadow and stack parameters
test rax,rax
jz .errorCreateService  ; Go if error status returned when create service
; Skip create point
.skipCreate:
mov [Service],rax 
; Check service status
xchg rcx,rax            ; RCX = Parm#1 = Handle 
lea rbx,[Status]
mov rdx,rbx             ; RDX = Parm#2 = Pointer to status structure
call [QueryServiceStatus]
test rax,rax
jz .errorGetStatus      ; Go if error status returned when get status
cmp [rbx + SERVICE_STATUS.dwCurrentState], SERVICE_RUNNING
je .skipStart           ; Go skip start if service already running
; Start service
mov rcx,[Service]          ; RCX = Parm#1 = Handle
xor edx,edx                ; RDX = Parm#2 = Arguments count
lea r8,[Vectors]           ; R8  = Parm#3 = Pointer to arguments vectors, empty 
call [StartService]
test rax,rax
jz .errorStartService   ; Go if error status returned when start service
; Skip start point
.skipStart:
; Create Device file
xor eax,eax                    ; RAX = 0 for compact push 0
push rax                       ; Alignment
push rax                       ; Parm#7 = Template file handle, 0 = not used
push rax                       ; Parm#6 = Flags and attributes, 0 = not used
push OPEN_EXISTING             ; Parm#5 = Creation disposition 
xor r9d,r9d                    ; Parm#4 = R9 = Security attributes  
mov r8d,FILE_SHARE_READ or FILE_SHARE_WRITE  ; Parm#3 = R8  = Share mode 
mov edx,GENERIC_WRITE or GENERIC_READ        ; Parm#2 = RDX = Desired access
lea rcx,[DrvDevice]            ; Parm#1 = RCX = Pointer to file=device name
sub rsp,32
call [CreateFile]
add rsp,32 + 4 * 8
test rax,rax
jz .errorCreateDevice   ; Go if error status returned when create device file
cmp rax,INVALID_HANDLE_VALUE
je .errorCreateDevice   ; Go if invalid handle returned when create device file
mov [Driver],rax
; Write to device file, this is requests for driver
push 0                         ; Alignment
push 0                         ; Parm#5 = Pointer to overlapped, 0 = not used
lea r9,[Bytes]                 ; Parm#4 = R9  = Pointer to byte count variable
mov r8d,sizeof.SERVICE_QUERY   ; Parm#3 = R8  = Requested byte count
lea rdx,[Query]                ; Parm#2 = RDX = Pointer to file buffer=request
mov rcx,[Driver]               ; Parm#1 = RCX = File=device handle
mov eax,RZ_REQUEST_CODE
mov qword [rdx + SERVICE_QUERY.iocode],rax    ; Request code
lea rax,[DataPattern]
mov qword [rdx + SERVICE_QUERY.parm1],rax     ; Parameter = destination pointer
lea rax,[kmdRoutine]
mov qword [rdx + SERVICE_QUERY.userproc],rax  ; Callback pointer
sub rsp,32
call [WriteFile]
add rsp,32 + 2 * 8
test rax,rax
jz .errorWriteDevice           ; Go if error status returned when send request
; Initialization and execution done 
xor r15d,r15d                  ; Set normal status, R15 = 0 
xor r14d,r14d                  ; Blank OS error code, R14 = 0
jmp @f
; Errors handling points, set error code R15 = pointer to error string 
.errorGetPath:
lea r15,[ErrorGetPath]
jmp @f
.errorOpenScm:
lea r15,[ErrorOpenScm]
jmp @f
.errorCreateService:
lea r15,[ErrorCreateService]
jmp @f
.errorGetStatus:
lea r15,[ErrorGetStatus]
jmp @f
.errorStartService:
lea r15,[ErrorStartService]
jmp @f
.errorCreateDevice:
lea r15,[ErrorCreateDevice]
jmp @f
.errorWriteDevice:
lea r15,[ErrorWriteDevice]
@@:
; Conditionally assign R14 = OS error code
xor r14d,r14d
call GetOsErrorCode
; Close device file, uninstall driver. Note close operations sequence continued
; even if error, but error logged at R15 if R15 = 0 (if yet no errors) 
mov rcx,[Driver]             ; RCX = Parm#1 = Manager handle
jrcxz @f
cmp rcx,INVALID_HANDLE_VALUE
je @f
call [CloseHandle]
test rax,rax
jnz @f                       ; Go skip error log if no errors
test r15,r15
jnz @f                       ; Go skip error log if errors from previous steps 
lea r15,[ErrorCloseDevice]
call GetOsErrorCode
@@:
; Stop service
mov rbx,[Service]
mov rcx,rbx                  ; RCX = Parm#1 = Handle 
jrcxz .skipStop              ; Go skip if handle = 0
mov edx,SERVICE_CONTROL_STOP ; RDX = Parm#2 = Control code
lea r8,[Status]              ; R8  = Parm#3 = Pointer to status structure
call [ControlService]
test rax,rax
jnz @f                       ; Go skip error log if no errors
test r15,r15
jnz @f                       ; Go skip error log if errors from previous steps 
lea r15,[ErrorStopService]
call GetOsErrorCode
@@:
; Delete service
mov rcx,rbx                  ; RCX = Parm#1 = Service handle
call [DeleteService]
test rax,rax
jnz @f                       ; Go skip error log if no errors
test r15,r15
jnz @f                       ; Go skip error log if errors from previous steps 
lea r15,[ErrorDeleteService]
call GetOsErrorCode
@@:
; Close service handle
mov rcx,rbx                  ; RCX = Parm#1 = Service handle
call [CloseServiceHandle]
test rax,rax
jnz @f                       ; Go skip error log if no errors
test r15,r15
jnz @f                       ; Go skip error log if errors from previous steps 
lea r15,[ErrorCloseService]
call GetOsErrorCode
@@:
; Skip stop point
.skipStop:
; Close manager handle 
mov rcx,[Manager]            ; RCX = Parm#1 = Manager handle
jrcxz @f
call [CloseServiceHandle]
test rax,rax
jnz @f                       ; Go skip error log if no errors
test r15,r15
jnz @f                       ; Go skip error log if errors from previous steps 
lea r15,[ErrorCloseManager]
call GetOsErrorCode
@@:
; De-Initialization done, check results, select normal result or error message
test r15,r15
jnz .errorDetected     ; Go error message mode if errors detected
; Normal result message mode  
lea rdi,[TEXT_BUFFER]  ; RDI = Pointer to text buffer
lea rsi,[MsrName]
call StringWrite
mov rax,[DataPattern]  ; RAX = Data after read from IA32_APIC_BASE MSR 
call HexPrint64        ; Build hexadecimal string
mov ax,0000h + 'h'
stosw                  ; Write "h" and terminator byte 0
lea r8,[WinCaption]    ; R8  = Parm #3 = Caption (upper message)
xor r9d,r9d            ; R9  = Parm #4 = Message flags
jmp .done              ; Go exit
; Errors handling
.errorDetected:
lea rdi,[TEXT_BUFFER]
lea rsi,[ErrorString]
call StringWrite
mov rsi,r15            ; R15 = Pointer to text string about failed step
call StringWrite
lea rsi,[FailedString]
call StringWrite
lea rsi,[ErrorCodeString]
call StringWrite
mov rax,r14            ; R14 = OS error code
call HexPrint64
mov ax,0000h + 'h'
stosw                  ; Write "h" and terminator byte 0
xor r8d,r8d	           ; R8  = Parm #3 = Caption (upper message) 
mov r9d,MB_ICONERROR   ; R9  = Parm #4 = Message flags 
; Show GUI box and exit
.done:
xor ecx,ecx            ; RCX = Parm #1 = Parent window
lea rdx,[TEXT_BUFFER]  ; RDX = Parm #2 = Message
call [MessageBoxA]
xor ecx,ecx
call [ExitProcess]

;------------------------------------------------------------------------------;
;       Fragment executed at kernel mode, as callback from KMD handler.        ;
;               This is debug example: read IA32_APIC_BASE MSR.                ;
;                                                                              ;
; INPUT:     RAX = Pointer to data buffer                                      ;
; OUTPUT:    8 bytes at [RAX] = IA32_APIC_BASE MSR value, 64-bit               ;
; Destroyed: RAX, RBX, RCX, RDX.                                               ; 
;------------------------------------------------------------------------------;
kmdRoutine:
mov rbx,rax          ; RBX = Pointer for store data after read MSR
mov ecx,01Bh         ; ECX = address of IA32_APIC_BASE MSR
rdmsr                ; Read MSR selected by ECX
mov [rbx + 0],eax    ; Store bits [31-00] of MSR
mov [rbx + 4],edx    ; Store bits [63-32] of MSR 
retn

;------------------------------------------------------------------------------;
;                                Libraries                                     ;
;------------------------------------------------------------------------------;

;---------- Get OS error code --------------------------------------;
; INPUT:   R15 = Error name string, 0 if no errors                  ;
;          R14 = OS error code, 0 if yet not assigned               ;
; OUTPUT:  R14 updated by OS error code, if error detected          ;
;          ( R15 != 0 ) and code yet not assigned ( input R14 = 0 ) ;  
;-------------------------------------------------------------------;
GetOsErrorCode:
test r15,r15
jz @f              ; Skip if no errors
test r14,r14
jnz @f             ; Skip if already assigned
push rax rcx rdx r8 r9 r10 r11 rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
call [GetLastError]
xchg r14,rax
add rsp,32
mov rsp,rbp
pop rbp r11 r10 r9 r8 rdx rcx rax
@@:
ret

;---------- Copy text string terminated by 00h ----------------;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   RSI = Source address                                ;
;          RDI = Destination address                           ;
; OUTPUT:  RSI = Modified by copy                              ;
;          RDI = Modified by copy                              ;
;          Memory at [Input RDI] modified                      ; 
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
; INPUT:  RAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint64:
push rax
ror rax,32
call HexPrint32
pop rax
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------------------------;
; INPUT:  EAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint32:
push rax
ror eax,16
call HexPrint16
pop rax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------------------------;
; INPUT:  AX  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint16:
push rax
xchg al,ah
call HexPrint8
pop rax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------------------------;
; INPUT:  AL  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify	                                       ;
;--------------------------------------------------------------;
HexPrint8:
push rax
ror al,4
call HexPrint4
pop rax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------------------------;
; INPUT:  AL  = Number (bits 0-3)                              ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint4:
cld
push rax
and al,0Fh
cmp al,9
ja .HP4_AF
add al,'0'
jmp .HP4_Store
.HP4_AF:
add al,'A'-10
.HP4_Store:
stosb
pop rax
ret

;------------------------------------------------------------------------------;
;                              Data section.                                   ;
;------------------------------------------------------------------------------;
section '.data' data readable writeable
; Constants, messages text strings 
WinCaption            DB  '  Kernel Mode Driver test (x64)' , 0
MsrName               DB  'IA32_APIC_BASE MSR = '           , 0
ErrorString           DB  'SCP64 error: '                   , 0
FailedString          DB  ' FAILED.'                        , 0
ErrorCodeString       DB  13, 10, 'OS error code: '         , 0
; Names for initialization errors
ErrorGetPath          DB  'get driver file path'            , 0
ErrorOpenScm          DB  'open SCM'                        , 0
ErrorCreateService    DB  'create service'                  , 0
ErrorGetStatus        DB  'get service status'              , 0
ErrorStartService     DB  'start service'                   , 0
ErrorCreateDevice     DB  'create device'                   , 0
ErrorWriteDevice      DB  'write device'                    , 0
; Names for de-initialization errors
ErrorCloseDevice      DB  'close device'                    , 0
ErrorStopService      DB  'stop service'                    , 0
ErrorDeleteService    DB  'delete service'                  , 0
ErrorCloseService     DB  'close service'                   , 0
ErrorCloseManager     DB  'close SCM'                       , 0
; Driver file and service names 
DrvFileName           DB  'KMD64.SYS'  , 0
DrvName               DB  'ICR0'       , 0
DrvDevice             DB  '\\.\ICR0'   , 0
; Driver control constants and variables with pre-defined values
DrvPath               DQ  PATH_BUFFER
DrvFile               DQ  0
Manager               DQ  0
Service               DQ  0
Vectors               DQ  0
Driver                DQ  INVALID_HANDLE_VALUE
Bytes                 DQ  0
; This qword used for data after read IA32_APIC_BASE MSR
DataPattern           DQ  1111111111111111h
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
