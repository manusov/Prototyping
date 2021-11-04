;---------- Get Native OS information -----------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoNativeOs:
sub rsp,32 + 8
mov rax,[rsi + DYNAIMPORT._GetNativeSystemInfo]
test rax,rax
jz @f
lea rcx,[rdi + OSDATA.nativeSystemInfo]
call rax
@@:
add rsp,32 + 8
ret
;--- Build Native OS information for text report and GUI widgets --------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextNativeOs:
mov [DYNA_PTR.textNativeOs.objectStart],rbp
mov bh,1                                  ; BH = 1 means native system info
call HelperSystemInfo
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textNativeOs.objectStop],rbp
ret

