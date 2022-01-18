;--- Set console foreground color, background color as saved at start ------;
;                                                                           ;
; INPUT:   ECX = New foreground color code                                  ;
;          Use global variable [StdOut]                                     ;
; OUTPUT:  EAX = OS Status                                                  ;
;---------------------------------------------------------------------------;

SetFgColor:
call GetColor                  ; Return EAX = default color
and eax,CLEAR_FOREGROUND
and ecx,CLEAR_BACKGROUND
add eax,ecx
push eax                       ; Parm#2 = Color for set
push [StdOut]                  ; Parm#1 = Handle for console output
call [SetConsoleTextAttribute]
ret



