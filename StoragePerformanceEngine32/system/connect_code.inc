;------------------------------------------------------------------------------;
;    Hardware platform and operating System helpers library, this part is:     ;
;                   connect include files with subroutines.                    ; 
;------------------------------------------------------------------------------;

include 'decodeerror.inc'      ; Translation error code to error name string
include 'checkcpuid.inc'       ; Verify CPUID support, max. sup. function
include 'measurecpuclk.inc'    ; Measure CPU TSC clock frequency
