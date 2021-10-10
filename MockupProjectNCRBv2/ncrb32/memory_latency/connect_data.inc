;------------------------------------------------------------------------------;
;                                                                              ;
;                   This performance patterns used in the                      ; 
;                     "Run", "Draw" benchmark scenarios.                       ;
;           Library of performance patterns for latency measurement:           ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;

;--- List of performance patterns: latency measurement: build and walk  -------;
; Note for both fill methods (LCM, RDRAND) dump walk method only

align 2
TableLatency:
LATENCY_ENTRY  Pattern_Latency_LCM_32      , DumpStart_Latency_Walk_32   , DumpStop_Latency_Walk_32   , 4
LATENCY_ENTRY  Pattern_Latency_RDRAND_32   , DumpStart_Latency_Walk_32   , DumpStop_Latency_Walk_32   , 4  
LATENCY_ENTRY  Pattern_Latency_Walk_32     , DumpStart_Latency_Walk_32   , DumpStop_Latency_Walk_32   , 4   
LATENCY_ENTRY  Pattern_Latency_LCM_32x2    , DumpStart_Latency_Walk_32x2 , DumpStop_Latency_Walk_32x2 , 8
LATENCY_ENTRY  Pattern_Latency_RDRAND_32x2 , DumpStart_Latency_Walk_32x2 , DumpStop_Latency_Walk_32x2 , 8  
LATENCY_ENTRY  Pattern_Latency_Walk_32x2   , DumpStart_Latency_Walk_32x2 , DumpStop_Latency_Walk_32x2 , 8   

