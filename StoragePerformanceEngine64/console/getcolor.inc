;---------- Get console color, saved at start-------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:  None                                                              ;  
;                                                                           ;
; OUTPUT: EAX = Color code                                                  ;
;---------------------------------------------------------------------------;

GetColor:
mov eax,[ScreenInfo.wAttributes]
ret