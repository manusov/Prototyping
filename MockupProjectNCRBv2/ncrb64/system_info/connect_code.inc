;------------------------------------------------------------------------------;
;                                                                              ;
;     System Information Library: connect include files with subroutines.      ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Operating system definitions for large pages support --------------;

MEM_LARGE_PAGES          = 020000000h 
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

;---------- Subroutines -------------------------------------------------------;

include 'load_inf.inc'
include 'load_kmd.inc'
include 'sysinfo_user_mode.inc'
include 'sysinfo_kernel_mode.inc'

include 'cpuid_x64.inc'
