;---------- Get console color, saved at start-------------------------------;
; INPUT:  None                                                              ;
; OUTPUT: EAX = Color code                                                  ;
;---------------------------------------------------------------------------;

GetColor:
mov eax,[ScreenInfo.wAttributes]
ret
