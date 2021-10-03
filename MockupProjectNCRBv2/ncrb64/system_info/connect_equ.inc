;------------------------------------------------------------------------------;
;                                                                              ;
;                System Information Library: data definitions.                 ; 
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Operating system definitions for large pages support --------------;

MEM_LARGE_PAGES          =  020000000h 
SE_PRIVILEGE_ENABLED     = 2 
SE_LOCK_MEMORY_PRIVILEGE = 4

struct LUID 
usedpart             dd ?   
ignorehigh32bitpart  dd ? 
ends 

struct LUID_AND_ATTRIBUTES 
Luid                 LUID 
Attributes           dd ?  
ends 

struct TOKEN_PRIVILEGES 
PrivilegeCount       dd ? 
Privileges           LUID_AND_ATTRIBUTES 
ends 

virtual at r8 
tp                   TOKEN_PRIVILEGES 
end virtual

;--- Application definitions for Kernel Mode Driver Service Control Program ---;  

; SCP_TEXT_BUFFER_SIZE = 512   ; Buffer size for text strings build
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

