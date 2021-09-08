;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                     globally used source definitions.                        ; 
;------------------------------------------------------------------------------;

ERROR_STRING_1     EQU  dword [ebx + 00]
ERROR_STRING_2     EQU  dword [ebx + 04]
ERROR_CODE         EQU  dword [ebx + 08]
BUFFER_SIZE        EQU  16 * 1024

; CPUID_NAME_COLOR   EQU  DUMP_ADDRESS_COLOR  ;  REGISTER_NAME_COLOR
; CPUID_VALUE_COLOR  EQU  DUMP_DATA_COLOR     ; REGISTER_VALUE_COLOR
CPUID_NAME_COLOR   EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
CPUID_VALUE_COLOR  EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE 

PROCESSORS_LIMIT   EQU  64  ; TODO. Support Processor Groups for >64 CPUs