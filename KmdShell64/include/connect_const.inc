;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;        globally used constants and variables with pre-defined state.         ; 
;------------------------------------------------------------------------------;

;---------- Strings for Kernel Mode Driver and Service Control Program --------;
; STRING_NAME_KMD32         DB  'RZ32.SYS'  , 0  
STRING_NAME_KMD64           DB  'RZ64.SYS'  , 0
STRING_DRV_NAME             DB  'RZ64'      , 0
STRING_DRV_DEVICE           DB  '\\.\RZ64'  , 0

;---------- Errors messages string --------------------------------------------;
STRING_DRV_ERROR_LOAD       DB  'Error loading kernel mode driver :'   , 0
STRING_DRV_ERROR_UNLOAD     DB  'Error unloading kernel mode driver :' , 0
STRING_DRV_ERROR_ACCESS     DB  'Error access kernel mode driver :'    , 0 
STRING_DRE_GET_PATH         DB  'Get path failed.'                     , 0
STRING_DRE_OPEN_SERVICE     DB  'Open service control program failed.' , 0
STRING_DRE_CREATE_SERVICE   DB  'Create service failed.'               , 0
STRING_DRE_GET_STATUS       DB  'Get status failed.'                   , 0
STRING_DRE_START_SERVICE    DB  'Start service failed.'                , 0
STRING_DRE_CREATE_DEVICE    DB  'Create device failed.'                , 0

;---------- Actions handlers pointers -----------------------------------------;
ActionsHandlers:
DQ  ReadMemory
DQ  ReadIo
DQ  ReadPci
DQ  ReadPciMcfg
DQ  ReadCrMsr
DQ  ReadCmos
DQ  ReadApic
DQ  ReadSpdAtSmbus
DQ  ReadClkAtSmbus
