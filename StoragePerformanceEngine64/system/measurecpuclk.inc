;------------------------------------------------------------------------;
; Measure CPU TSC (Time Stamp Counter) clock frequency, store results    ;
; F=Frequency=[Hz].                                                      ;
;                                                                        ;
; INPUT:   None                                                          ;
;                                                                        ;
; OUTPUT:  CF flag = Status: 0(NC)=Measured OK, 1(C)=Measurement error	 ;
;          Output RAX valid only if CF=0(NC)                             ;
;          RAX = TSC Frequency, Hz, F = Delta TSC per 1 second           ;
;------------------------------------------------------------------------;

MeasureCpuClk:
cld                         ; Clear direction, because STOSQ used
push rbx rcx rdx rsi rbp r8 r9 r10 r11 rax	 ; R8-R11 because API, RAX = Var.
mov rbp,rsp                 ; RBP used for restore RSP and addressing variables
and rsp,0FFFFFFFFFFFFFFF0h  ; Align stack (16)
sub rsp,32                  ; Make parameters shadow
;--- Start measure frequency, wait toggle ---
mov rcx,rbp
call [GetSystemTimeAsFileTime]  ; Get current count
mov rsi,[rbp]
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]  ; Get next count for wait 100 ns
cmp rsi,[rbp]
je @b
;--- Start time point ---
mov rsi,[rbp]
add rsi,10000000                ; RSI = 10^7 * 100ns = 1 second
rdtsc
shl rdx,32
lea rbx,[rax+rdx]               ; RBX = 64-bit TSC at operation start
;--- Delay 1 second ---
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]  ; Get count for wait 1 second
cmp rsi,[rbp]                   ; Compare target=rsi and returned=[rbp] 
ja @b                           ; Go wait if target > returned, must 1 second
;--- Stop time point ---
rdtsc
shl rdx,32
or rax,rdx         ; RAX = 64-bit TSC at operation end
sub rax,rbx        ; RAX = Delta TSC = frequency (1 second), CF flag also valid
;--- Restore RSP, pop extra registers, exit ---
mov rsp,rbp                            ; Restore RSP after alignment and shadow
pop rbx r11 r10 r9 r8 rbp rsi rdx rcx rbx  ; First POP RBX for RSP-8 only 
ret

