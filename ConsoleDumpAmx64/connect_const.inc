;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;        globally used constants and variables with pre-defined state.         ; 
;------------------------------------------------------------------------------;

MsgErrorCpuid        DB  'CPUID instruction not supported or locked.' , 0
MsgErrorAmx          DB  'Intel AMX technology not supported.'        , 0
MsgErrorFormat       DB  'AMX-INT8 data format not supported.'        , 0
MsgErrorContext      DB  'Context management not not enabled.'        , 0
MsgErrorAmxContext   DB  'Intel AMX context not enabled.'             , 0  
MsgErrorAmxPalette   DB  'Incompatible AMX palette configuration.'    , 0
MsgErrorAmxTmul      DB  'Incompatible AMX TMUL configuration.'       , 0

NameAmxConfig        DB  'TILECFG' , 0Dh, 0Ah, 0
NameAmxTmm           DB  'TMM'     , 0