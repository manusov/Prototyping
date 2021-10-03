;------------------------------------------------------------------------------;
;                                                                              ;
;                   This performance patterns used in the                      ; 
;                     "Run", "Draw" benchmark scenarios.                       ;
;           Library of performance patterns for latency measurement:           ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

; Start offset for compact offsets encoding
LatencyPatternsBase:
; Latency measurement
include 'latency_lcm_32.inc'       ; Build walk list, use linear congruental method, for one 32-bit load
include 'latency_rdrand_32.inc'    ; Build walk list, use RDRAND instruction, for one 32-bit load
include 'latency_walk_32.inc'      ; Walk on pre-builded walk list, one 32-bit load
include 'latency_lcm_32x2.inc'     ; Build walk list, use linear congruental method, for two 32-bit loads match x64 version
include 'latency_rdrand_32x2.inc'  ; Build walk list, use RDRAND instruction, for two 32-bit loads match x64 version
include 'latency_walk_32x2.inc'    ; Walk on pre-builded walk list, two 32-bit loads match x64 version

