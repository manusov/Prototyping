;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                     globally used source definitions.                        ; 
;------------------------------------------------------------------------------;

KERNEL_BUFFER_OFFSET  EQU  4096
KERNEL_BLOCK_LIMIT    EQU  4096
OPTION_ACTION_LIMIT   EQU  8

ERROR_STRING_1        EQU  dword [ebx + 00]
ERROR_STRING_2        EQU  dword [ebx + 04]
ERROR_CODE            EQU  dword [ebx + 08]

KMD_NAME_COLOR   EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
KMD_VALUE_COLOR  EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE 

;--- Application definitions for Kernel Mode Driver Service Control Program ---;
SCP_PATH_BUFFER_SIZE = 260     ; Buffer size for driver file string path build
;---------- Kernel Mode Driver (KMD) definitions ------------------------------;
RZ_DRIVER_QUERY_BUFFER_SIZE = 24     ; Buffer size for driver request structure
RZ_REQUEST_CODE             = 41h    ; Driver request code = user routine call 
;---------- Service Control Program (SCP) definitions -------------------------;
SC_MANAGER_ALL_ACCESS = 0000F003Fh   ; Used as desired access rights for SCM
SERVICE_ALL_ACCESS    = 0000F01FFh   ; Used as desired acc. rights for service
SERVICE_KERNEL_DRIVER = 000000001h   ; Used as service type for service
SERVICE_DEMAND_START  = 000000003h   ; Used as service start option for service
SERVICE_ERROR_NORMAL  = 000000001h   ; Used as error control option for service
SERVICE_CONTROL_STOP  = 000000001h   ; Used as control code for stop service
SERVICE_RUNNING       = 000000004h   ; Used for detect service current state
;---------- Structure for service request execution status (see MSDN) ---------;
struct SERVICE_STATUS
dwServiceType              dd ?     ; Type of system service
dwCurrentState             dd ?     ; State of service, run/stop/pause
dwControlsAccepted         dd ?     ; Accepted service operations flags
dwWin32ExitCode            dd ?     ; Service unified error code
dwServiceSpecificExitCode  dd ?     ; Service-specific error code
dwCheckPoint               dd ?     ; Incremnted progress indicator value
dwWaitHint                 dd ?     ; Estimated time of operation for tracking
ends
;---------- Structure for driver query ----------------------------------------;
struct SERVICE_QUERY
iocode    dd ?    ; user I/O code, request type selector
iodata    dd ?    ; user I/O data, request input parameter 
userproc  dq ?    ; procedure offset, callback address
parm1     dq ?    ; parameter A, callback routine optional input parameter 1
parm2     dq ?    ; parameter B, callback routine optional input parameter 2
result    dq ?    ; result, usage example: AL after IN AL,DX
buffer    db RZ_DRIVER_QUERY_BUFFER_SIZE dup ?
ends
;---------- Kernel Mode Driver Service Control Program information ------------;
struct SCPDATA
drvPath   dd ?
drvFile   dd ?               
manager   dd ?               
service   dd ?               
vectors   dd ?               
driver    dd ?                
bytes     dd ?                 
status    SERVICE_STATUS ?   ; Driver status structure
query     SERVICE_QUERY  ?   ; Driver request structure 
ends


;---------- Kernel Mode Driver requests codes ---------------------------------;
; empty requests
RZ_DRIVER_QUERY_EMPTY                   = 0x0000 ;
RZ_DRIVER_QUERY_NULL                    = 0x0000 ;
; call without parameters and with one parameter
RZ_DRIVER_QUERY_PROC_NOARGS             = 0x0010 ; ...
RZ_DRIVER_QUERY_PROC_REGISTER           = 0x0011 ; paramA->eax(rax) ...
RZ_DRIVER_QUERY_PROC_FASTCALL           = 0x0012 ; paramA->ecx(rcx) ...
RZ_DRIVER_QUERY_PROC_STDCALL            = 0x0013 ; push paramA ...
RZ_DRIVER_QUERY_PROC_CDECL              = 0x0014 ; push paramA ...
; call without parameters and with one parameter, under disabled interrupts
RZ_DRIVER_QUERY_PROC_NOARGS_IFRAME      = 0x0110 ; cli ... sti
RZ_DRIVER_QUERY_PROC_REGISTER_IFRAME    = 0x0111 ; cli paramA->eax(rax) ... sti
RZ_DRIVER_QUERY_PROC_FASTCALL_IFRAME    = 0x0112 ; cli paramA->ecx(rcx) ... sti
RZ_DRIVER_QUERY_PROC_STDCALL_IFRAME     = 0x0113 ; cli push paramA ... sti
RZ_DRIVER_QUERY_PROC_CDECL_IFRAME       = 0x0114 ; cli push paramA ... sti
; call with 2 parameters
RZ_DRIVER_QUERY_PROC_REGISTER_2P        = 0x0015 ; paramA->eax(rax), paramB->edx(rdx) ...
RZ_DRIVER_QUERY_PROC_FASTCALL_2P        = 0x0016 ; paramA->ecx(rcx), paramB->edx(rdx) ...
RZ_DRIVER_QUERY_PROC_STDCALL_2P         = 0x0017 ; push paramB, push paramA ...
RZ_DRIVER_QUERY_PROC_CDECL_2P           = 0x0018 ; push paramB, push paramA ...
; call with 2 parameters, under disabled interrupts
RZ_DRIVER_QUERY_PROC_REGISTER_2P_IFRAME = 0x0115 ; cli paramA->eax(rax), paramB->edx(rdx) ... sti
RZ_DRIVER_QUERY_PROC_FASTCALL_2P_IFRAME = 0x0116 ; cli paramA->ecx(rcx), paramB->edx(rdx) ... sti
RZ_DRIVER_QUERY_PROC_STDCALL_2P_IFRAME  = 0x0117 ; cli push paramB, push paramA ... sti
RZ_DRIVER_QUERY_PROC_CDECL_2P_IFRAME    = 0x0118 ; cli push paramB, push paramA ... sti
; in port
RZ_DRIVER_QUERY_PORT_IN_BYTE            = 0x0020 ;
RZ_DRIVER_QUERY_PORT_IN_WORD            = 0x0021 ;
RZ_DRIVER_QUERY_PORT_IN_DWORD           = 0x0022 ;
; out port
RZ_DRIVER_QUERY_PORT_OUT_BYTE           = 0x0030 ;
RZ_DRIVER_QUERY_PORT_OUT_WORD           = 0x0031 ;
RZ_DRIVER_QUERY_PORT_OUT_DWORD          = 0x0033 ;
; memory-mapped i/o
RZ_DRIVER_QUERY_MAP_IO_SPACE            = 0x0040 ; ...
RZ_DRIVER_QUERY_MAP_IO_SPACE_IFRAME     = 0x0041 ; cli ... sti
RZ_DRIVER_QUERY_UNMAP_IO_SPACE          = 0x0042 ; ...
RZ_DRIVER_QUERY_UNMAP_IO_SPACE_IFRAME   = 0x0043 ; cli ... sti
; create process
RZ_DRIVER_QUERY_CP_SET                  = 0x0050 ; ...
RZ_DRIVER_QUERY_CP_SET_IFRAME           = 0x0051 ; cli ... sti
RZ_DRIVER_QUERY_CP_RET                  = 0x0052 ; ...
RZ_DRIVER_QUERY_CP_RET_IFRAME           = 0x0053 ; cli ... sti
RZ_DRIVER_QUERY_CP_GET                  = 0x0054 ; ...
RZ_DRIVER_QUERY_CP_GET_IFRAME           = 0x0055 ; cli ... sti
; load image
RZ_DRIVER_QUERY_LI_SET                  = 0x0060 ; ...
RZ_DRIVER_QUERY_LI_SET_IFRAME           = 0x0061 ; cli ... sti
RZ_DRIVER_QUERY_LI_RET                  = 0x0062 ; ...
RZ_DRIVER_QUERY_LI_RET_IFRAME           = 0x0063 ; cli ... sti
RZ_DRIVER_QUERY_LI_GET                  = 0x0064 ; ...
RZ_DRIVER_QUERY_LI_GET_IFRAME           = 0x0065 ; cli ... sti

;---------- Kernel Mode Driver structures -------------------------------------;

; structure for driver query
; base
struct RZDriverQuery
  IOCODE dd ? ; user I/O code
  IODATA dd ? ; user I/O data
  PROC   dq ? ; procedure offset
  paramA dq ? ; parameter A
  paramB dq ? ; parameter B
  RESULT dq ? ; result
  Buffer db RZ_DRIVER_QUERY_BUFFER_SIZE dup (?)
ends
; structure for driver query
; I/O, memory map
struct RZDriverIOQuery
  IOCODE dd ? ; user I/O code
  IODATA dd ? ; user I/O data
  SRC    dq ? ; source address
  DST    dq ? ; destination address
  BYTES  dq ? ; memory size in bytes
  RESULT dq ? ; result
  Buffer db RZ_DRIVER_QUERY_BUFFER_SIZE dup (?)
ends
