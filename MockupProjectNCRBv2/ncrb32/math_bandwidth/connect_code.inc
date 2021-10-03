;------------------------------------------------------------------------------;
;                                                                              ;
;        This performance patterns used in the "Math" benchmark scenario.      ;
;           Library of performance patterns for mathematics benchmarks:        ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Start offset for compact offsets encoding -------------------------;

MathBandwidthBase:

;---------- Math performance bandwidth measurement ----------------------------;
; Routines for mathematics instructions bandwidth benchmarks,
; for register operands

include 'sqrt_sse128.inc'       ; Square root, SSE128, 2 x double 
include 'sqrt_avx256.inc'       ; Square root, AVX256, 4 x double
include 'sqrt_avx512.inc'       ; Square root, AVX512, 8 x double
include 'cos_x87.inc'           ; Cosine, x87
include 'sincos_x87.inc'        ; Sine + Cosine, x87


