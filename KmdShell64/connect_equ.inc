;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                     globally used source definitions.                        ; 
;------------------------------------------------------------------------------;

KERNEL_BUFFER_OFFSET  EQU  4096
KERNEL_BUFFER_LENGTH  EQU  256

ERROR_STRING_1     EQU  qword [r15 + 00]
ERROR_STRING_2     EQU  qword [r15 + 08]
ERROR_CODE         EQU  qword [r15 + 16]
BUFFER_SIZE        EQU  16 * 1024

KMD_NAME_COLOR     EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
KMD_VALUE_COLOR    EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE 

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
drvPath   dq ?
drvFile   dq ?               
manager   dq ?               
service   dq ?               
vectors   dq ?               
driver    dq ?                
bytes     dq ?                 
status    SERVICE_STATUS ?   ; Driver status structure
query     SERVICE_QUERY  ?   ; Driver request structure 
ends
;---------- Kernel Mode Driver structures -------------------------------------;
; Driver request code
RZ_DRIVER_QUERY_MAP_IO_SPACE = 0040h
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
