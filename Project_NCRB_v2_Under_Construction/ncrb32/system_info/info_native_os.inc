;---------- Get Native OS information -----------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoNativeOs:
mov eax,[ebx + DYNAIMPORT._GetNativeSystemInfo]
test eax,eax
jz @f
lea ecx,[OS_DATA.nativeSystemInfo]
push ecx
call eax
@@:
ret
;--- Build Native OS information for text report and GUI widgets --------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextNativeOs:
mov [DYNA_PTR.textNativeOs.objectStart],ebp
mov bh,1                                  ; BH = 1 means native system info
call HelperSystemInfo
mov al,0
stosb
mov ebp,edi
mov [DYNA_PTR.textNativeOs.objectStop],ebp
ret

