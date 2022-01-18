;---------- Set console color ----------------------------------------------;
; INPUT:   ECX = New color code                                             ;
;          Use global variable [StdOut]                                     ;  
; OUTPUT:  EAX = OS Status                                                  ;
;---------------------------------------------------------------------------;

SetColor:
push ecx                    ; Parm#2 = Color for set
push [StdOut]               ; Parm#1 = Handle for console output
call [SetConsoleTextAttribute]
ret

