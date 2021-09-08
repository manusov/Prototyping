;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;        globally used constants and variables with pre-defined state.         ; 
;------------------------------------------------------------------------------;

MsgErrorCpuid        DB  'CPUID instruction not supported or locked.' , 0
MsgErrorAlloc        DB  'Memory allocation error.'                   , 0
MsgErrorRelease      DB  'Memory release error.'                      , 0
MsgErrorWinAPI       DB  'WinAPI initialization failed.'              , 0
MsgErrorAffin        DB  'Affinization WinAPI not found.'             , 0
MsgErrorAffinFailed  DB  'Affinization failed.'                       , 0
MsgErrorCpuLimit     DB  'Too many logical processors detected.'      , 0  
MsgErrorCpuidFailed  DB  'Get CPUID information failed.'              , 0
NameCPUID            DB  'CPUID '                                     , 0
NameDll              DB  'KERNEL32.DLL'                               , 0
NameFunctionAffin    DB  'SetThreadAffinityMask'                      , 0