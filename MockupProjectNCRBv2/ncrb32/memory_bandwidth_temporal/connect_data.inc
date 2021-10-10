;------------------------------------------------------------------------------;
;                                                                              ;
;                  This performance patterns used in the                       ; 
;                    "Run", "Draw" benchmark scenarios.                        ;
;          Library of performance patterns for bandwidth measurement:          ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;

;--- List of performance patterns: bandwidth measurement, temporal access -----;

align 2
TableBandwidthTemporal:
BANDWIDTH_TEMPORAL  Pattern_Read_MOV32   , DumpStart_Read_MOV32   , DumpStop_Read_MOV32     , 4
BANDWIDTH_TEMPORAL  Pattern_Write_MOV32  , DumpStart_Write_MOV32  , DumpStop_Write_MOV32    , 4
BANDWIDTH_TEMPORAL  Pattern_Copy_MOV32   , DumpStart_Copy_MOV32   , DumpStop_Copy_MOV32     , 4
BANDWIDTH_TEMPORAL  Pattern_Modify_NOT32 , DumpStart_Modify_NOT32 , DumpStop_Modify_NOT32   , 4
BANDWIDTH_TEMPORAL  Pattern_Write_STOSD  , DumpStart_Write_STOSD  , DumpStop_Write_STOSD    , 4
BANDWIDTH_TEMPORAL  Pattern_Copy_MOVSD   , DumpStart_Copy_MOVSD   , DumpStop_Copy_MOVSD     , 4
BANDWIDTH_TEMPORAL  Pattern_Read_MMX64   , DumpStart_Read_MMX64   , DumpStop_Read_MMX64     , 8
BANDWIDTH_TEMPORAL  Pattern_Write_MMX64  , DumpStart_Write_MMX64  , DumpStop_Write_MMX64    , 8
BANDWIDTH_TEMPORAL  Pattern_Copy_MMX64   , DumpStart_Copy_MMX64   , DumpStop_Copy_MMX64     , 8
BANDWIDTH_TEMPORAL  Pattern_Read_SSE128  , DumpStart_Read_SSE128  , DumpStop_Read_SSE128    , 16
BANDWIDTH_TEMPORAL  Pattern_Write_SSE128 , DumpStart_Write_SSE128 , DumpStop_Write_SSE128   , 16
BANDWIDTH_TEMPORAL  Pattern_Copy_SSE128  , DumpStart_Copy_SSE128  , DumpStop_Copy_SSE128    , 16
BANDWIDTH_TEMPORAL  Pattern_Read_AVX256  , DumpStart_Read_AVX256  , DumpStop_Read_AVX256    , 32
BANDWIDTH_TEMPORAL  Pattern_Write_AVX256 , DumpStart_Write_AVX256 , DumpStop_Write_AVX256   , 32
BANDWIDTH_TEMPORAL  Pattern_Copy_AVX256  , DumpStart_Copy_AVX256  , DumpStop_Copy_AVX256    , 32
BANDWIDTH_TEMPORAL  Pattern_Read_AVX512  , DumpStart_Read_AVX512  , DumpStop_Read_AVX512    , 64
BANDWIDTH_TEMPORAL  Pattern_Write_AVX512 , DumpStart_Write_AVX512 , DumpStop_Write_AVX512   , 64
BANDWIDTH_TEMPORAL  Pattern_Copy_AVX512  , DumpStart_Copy_AVX512  , DumpStop_Copy_AVX512    , 64
BANDWIDTH_TEMPORAL  Pattern_Dot_FMA256   , DumpStart_Dot_FMA256   , DumpStop_Dot_FMA256     , 32
BANDWIDTH_TEMPORAL  Pattern_Dot_FMA512   , DumpStart_Dot_FMA512   , DumpStop_Dot_FMA512     , 64  

