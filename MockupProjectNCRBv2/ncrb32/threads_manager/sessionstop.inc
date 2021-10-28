;--- Stop measurement session -------------------------------------------------;
; Release allocated memory and resources handles when session stops            ;
; Main actions is:                                                             ;
;  1) Release threads list: handles for events and threads.                    ;
;  2) Release memory, allocated at NUMA nodes.                                 ;
; See complementar routines: sessionstart.inc , sessionprogress.inc.           ; 
;                                                                              ;
; INPUT:   None in the registers                                               ;
;          IPB = Memory and Threads allocation data, used for release          ;
;          Threads List, NUMA nodes list                                       ;
;                                                                              ;
; OUTPUT:  CF = Error flag, 0(NC) = No errors, 1(C) = Error detected.          ;
;          AL = Error code, valid if CF=1(C)                                   ;
;               00 = Memory allocation error                                   ;
;               01 = Memory release error                                      ;
;               02 = Time measurement error                                    ;
;               04 = Address arithmetics error                                 ;
;               04-0FFh = Reserved                                             ;
;          OPB = Benchmark results data                                        ;
;          Threads List, NUMA nodes list                                       ;
;                                                                              ;
; All CPU registers can be corrupted, this rule for complex modules:           ;
; directory threads_manager\:                                                  ;
;   sessionstart.inc, sessionstop.inc, sessionprogress.inc.                    ;
; directory gui\:                                                              ;
;   drawsessionstart.inc, drawsessionstop.inc                                  ;
;   but not drawsessionthread.inc because OS callback                          ;       
; Remember about PUSH-POP in the callers.                                      ;
;------------------------------------------------------------------------------;

SessionStop:

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

; lea edi,[eax + REGISTRY32.memIpb]   ; RDI = Pointer to IPB (benchmark scenario)
lea edi,[MEM_IPB]

;---------- Threads stop, release handles for Threads and Events --------------;

; lea ecx,[eax + REGISTRY32.mpe32csb.threadList]  ; ECX = Pointer to threads list
lea ecx,[THREAD_LIST]


mov edx,[edi + MEMIPB.updatedThreads]           ; EDX = Threads count
call ReleaseThreadsList
mov al,1  ; AL = error status, valid if CF=1 only
jc .exit  ; Go if error

;---------- NUMA-aware or unaware memory release ------------------------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]
;lea ebx,[eax + REGISTRY32.mpe32csb.numaNodeList]  ; EBX = Pointer to NUMA nodes list

lea ebx,[NUMA_LIST]

mov edx,[edi + MEMIPB.domainsCount]               ; EDX = NUMA domains count
call MemReleaseNuma
mov al,1  ; AL = error status, valid if CF=1 only

;---------- Done, return CF=Error flag, AL=Error code (valid if CF=1) ---------;

.exit:
ret
