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
include 'latency_lcm.inc'      ; Build walk list, use linear congruental method
include 'latency_rdrand.inc'   ; Build walk list, use RDRAND instruction
include 'latency_walk.inc'     ; Walk on pre-builded walk list

