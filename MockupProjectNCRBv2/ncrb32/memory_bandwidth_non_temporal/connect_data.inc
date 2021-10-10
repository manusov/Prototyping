;------------------------------------------------------------------------------;
;                                                                              ;
;                  This performance patterns used in the                       ; 
;                    "Run", "Draw" benchmark scenarios.                        ;
;          Library of performance patterns for bandwidth measurement:          ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;

;--- List of performance patterns: bandwidth measur., non temporal access -----;

align 2
TableBandwidthNonTemporal:
; Non-temporal (non-cacheable write) mode 
BANDWIDTH_NON_TEMPORAL  Pattern_NtWrite_SSE128 , DumpStart_NtWrite_SSE128 , DumpStop_NtWrite_SSE128 , 16
BANDWIDTH_NON_TEMPORAL  Pattern_NtCopy_SSE128  , DumpStart_NtCopy_SSE128  , DumpStop_NtCopy_SSE128  , 16    ; This duplicated 1
BANDWIDTH_NON_TEMPORAL  Pattern_NtWrite_AVX256 , DumpStart_NtWrite_AVX256 , DumpStop_NtWrite_AVX256 , 32
BANDWIDTH_NON_TEMPORAL  Pattern_NtCopy_AVX256  , DumpStart_NtCopy_AVX256  , DumpStop_NtCopy_AVX256  , 32
BANDWIDTH_NON_TEMPORAL  Pattern_NtWrite_AVX512 , DumpStart_NtWrite_AVX512 , DumpStop_NtWrite_AVX512 , 64
BANDWIDTH_NON_TEMPORAL  Pattern_NtCopy_AVX512  , DumpStart_NtCopy_AVX512  , DumpStop_NtCopy_AVX512  , 64
; Non-temporal (non-cacheable read and write) mode
BANDWIDTH_NON_TEMPORAL  Pattern_NtRead_SSE128  , DumpStart_NtRead_SSE128  , DumpStop_NtRead_SSE128  , 16
BANDWIDTH_NON_TEMPORAL  Pattern_NtRCopy_SSE128 , DumpStart_NtRCopy_SSE128 , DumpStop_NtRCopy_SSE128 , 16
BANDWIDTH_NON_TEMPORAL  Pattern_NtRead_AVX256  , DumpStart_NtRead_AVX256  , DumpStop_NtRead_AVX256  , 32
BANDWIDTH_NON_TEMPORAL  Pattern_NtRCopy_AVX256 , DumpStart_NtRCopy_AVX256 , DumpStop_NtRCopy_AVX256 , 32
BANDWIDTH_NON_TEMPORAL  Pattern_NtRead_AVX512  , DumpStart_NtRead_AVX512  , DumpStop_NtRead_AVX512  , 64   
BANDWIDTH_NON_TEMPORAL  Pattern_NtRCopy_AVX512 , DumpStart_NtRCopy_AVX512 , DumpStop_NtRCopy_AVX512 , 64
; Non-temporal (non-cacheable read-by-prefetch and write) mode
BANDWIDTH_NON_TEMPORAL  Pattern_NtpRead_SSE128 , DumpStart_NtpRead_SSE128 , DumpStop_NtpRead_SSE128 , 16
BANDWIDTH_NON_TEMPORAL  Pattern_NtCopy_SSE128  , DumpStart_NtCopy_SSE128  , DumpStop_NtCopy_SSE128  , 16    ; This duplicated 1
BANDWIDTH_NON_TEMPORAL  Pattern_NtpRead_AVX256 , DumpStart_NtpRead_AVX256 , DumpStop_NtpRead_AVX256 , 32
BANDWIDTH_NON_TEMPORAL  Pattern_NtCopy_SSE128  , DumpStart_NtCopy_SSE128  , DumpStop_NtCopy_SSE128  , 16    ; This duplicated 1
BANDWIDTH_NON_TEMPORAL  Pattern_NtCopy_SSE128  , DumpStart_NtCopy_SSE128  , DumpStop_NtCopy_SSE128  , 16    ; This duplicated 1
BANDWIDTH_NON_TEMPORAL  Pattern_NtpRead_AVX512 , DumpStart_NtpRead_AVX512 , DumpStop_NtpRead_AVX512 , 64
