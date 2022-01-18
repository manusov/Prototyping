;---------- Set console color ----------------------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:   ECX = New color code                                             ;
;          Use global variable [StdOut]                                     ;  
;                                                                           ;
; OUTPUT:  RAX = OS Status                                                  ;
;---------------------------------------------------------------------------;

SetColor:
; Entry
push rbp
mov rbp,rsp                    ; RBP = storage for RSP and pointer to frame
and rsp,0FFFFFFFFFFFFFFF0h     ; Align stack
; Set console color
mov edx,ecx                    ; EDX = Color for set
mov rcx,[StdOut]               ; RCX = Handle for console output
sub rsp,32                     ; Parameters shadow
call [SetConsoleTextAttribute]
; Exit
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop rbp
ret
