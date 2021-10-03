;------------------------------------------------------------------------------;
;                                                                              ;
;                  This performance patterns used in the                       ; 
;                    "Run", "Draw" benchmark scenarios.                        ;
;          Library of performance patterns for bandwidth measurement:          ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

; Non-temporal (non-cacheable write) mode
include 'ntwrite_sse128.inc'   ; Write by MOVNTPS (128 bit)
include 'ntcopy_sse128.inc'    ; Copy by PREFETCHNTA + MOVAPS + MOVNTPS (128 bit)
include 'ntwrite_avx256.inc'   ; Write by VMOVNTPD (256 bit)
include 'ntcopy_avx256.inc'    ; Copy by MOVAPD + MOVNTPD (256 bit)
include 'ntwrite_avx512.inc'   ; Write by VMOVNTPD (512 bit)
include 'ntcopy_avx512.inc'    ; Copy by MOVAPD + MOVNTPD (512 bit) 
; Non-temporal (non-cacheable read and write) mode
include 'ntread_sse128.inc'    ; Read by MOVNTDQA (128 bit) 
include 'ntrcopy_sse128.inc'   ; Copy by MOVNTDQA + MOVNTPD (128 bit)
include 'ntread_avx256.inc'    ; Read by VMOVNTDQA (256 bit)
include 'ntrcopy_avx256.inc'   ; Copy by VMOVNTDQA + VMOVNTPD (256 bit)  
include 'ntread_avx512.inc'    ; Read by VMOVNTDQA (512 bit)
include 'ntrcopy_avx512.inc'   ; Copy by VMOVNTDQA + VMOVNTPD (512 bit)
; Non-temporal (non-cacheable read-by-prefetch and write) mode
include 'ntpread_sse128.inc'   ; Read by PREFETCHNTA + MOVAPS (128 bit)
include 'ntpread_avx256.inc'   ; Read by PREFETCHNTA + VMOVAPD (256 bit)
; Non-temporal read mode with default and extended prefetch distance options,
; for AVX512
include 'ntpread_avx512.inc'   ; Read by PREFETCHNTA + VMOVAPD (512 bit)
