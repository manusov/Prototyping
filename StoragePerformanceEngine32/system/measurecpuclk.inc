;------------------------------------------------------------------------;
; Measure CPU TSC (Time Stamp Counter) clock frequency, store results    ;
; F=Frequency=[Hz].                                                      ;
;                                                                        ;
; INPUT:   None                                                          ;
;                                                                        ;
; OUTPUT:  CF flag = Status: 0(NC)=Measured OK, 1(C)=Measurement error	 ;
;          Output EAX, EDX valid only if CF=0(NC)                        ;
;          EDX:EAX = TSC Period, Fs, T=1/F                               ;
;------------------------------------------------------------------------;

MeasureCpuClk:
cld                             ; Clear direction, because STOS used
xor eax,eax
push ebx ecx esi edi ebp eax eax        ; Last EAX = Variable
;--- Start measure frequency, wait toggle ---
push ebp
call [GetSystemTimeAsFileTime]  ; Get current count
mov esi,[ebp]
@@:
push ebp
call [GetSystemTimeAsFileTime]  ; Get next count for wait 100 ns
cmp esi,[ebp]
je @b
;--- Start time point ---
mov esi,[ebp+0]
mov edi,[ebp+4]
add esi,10000000                
adc edi,0                       ; EDI:ESI = 10^7 * 100ns = 1 second
rdtsc
push eax edx                    ; Stack qword = 64-bit TSC at operation start
;--- Delay 1 second ---
@@:
push ebp
call [GetSystemTimeAsFileTime]  ; Get count for wait 1 second
cmp edi,[ebp+4]                 ; Compare high: target=edi and returned=[ebp+4] 
ja @b                           ; Go wait if target > returned, must 1 second
jb @f
cmp esi,[ebp+0]                 ; Compare low: target=esi and returned=[ebp+0] 
ja @b                           ; Go wait if target > returned, must 1 second
@@:
;--- Stop time point ---
rdtsc                           ; EDX:EAX = 64-bit TSC at operation end
pop ecx ebx
sub eax,ebx
sbb edx,ecx                     ; ECX:EBX = Delta TSC = frequency (1 second) 
;--- Restore stack and registers, exit ---
; EDX:EAX = Frequency, high:low
clc                              ; CF=0 (NC) means CPU clock measured OK
pop ebp ebp ebp edi esi ecx ebx  ; First POP EBP for RSP-8 only 
ret
