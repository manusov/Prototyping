;------------------------------------------------------------------------------;
;                                                                              ;
;                  This performance patterns used in the                       ; 
;                    "Run", "Draw" benchmark scenarios.                        ;
;          Library of performance patterns for bandwidth measurement:          ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Include files connect for performance patterns --------------------;
; Temporal ( cacheable read and write ) mode, 
; temporal is default cacheable memory access

include 'read_mov64.inc'       ; Read by MOV (64 bit)
include 'write_mov64.inc'      ; Write by MOV (64 bit) 
include 'copy_mov64.inc'       ; Copy by MOV + MOV (64 bit)
include 'modify_not64.inc'     ; Modify = Read-Modify-Write by NOT (64 bit) 
include 'write_stosq.inc'      ; Write by REP STOSQ (64 bit)
include 'copy_movsq.inc'       ; Copy by REP MOVSQ (64 bit)
include 'read_mmx64.inc'       ; Read by MOVQ (64 bit)
include 'write_mmx64.inc'      ; Write by MOVQ (64 bit)
include 'copy_mmx64.inc'       ; Copy by MOVQ + MOVQ (64 bit)
include 'read_sse128.inc'      ; Read by MOVAPS (128 bit) 
include 'write_sse128.inc'     ; Write by MOVAPS (128 bit)
include 'copy_sse128.inc'      ; Copy by MOVAPS + MOVAPS (128 bit)
include 'read_avx256.inc'      ; Read by VMOVAPD (256 bit)
include 'write_avx256.inc'     ; Write by VMOVAPD (256 bit)
include 'copy_avx256.inc'      ; Copy by VMOVAPD + VMOVAPD (256 bit)
include 'read_avx512.inc'      ; Read by VMOVAPD (512 bit)  
include 'write_avx512.inc'     ; Write by VMOVAPD (512 bit)
include 'copy_avx512.inc'      ; Copy by VMOVAPD + VMOVAPD (512 bit)
include 'dot_fma256.inc'       ; Fused multiply and add, VFMADD231PD (256 bit)
include 'dot_fma512.inc'       ; Fused multiply and add, VFMADD231PD (512 bit) 
