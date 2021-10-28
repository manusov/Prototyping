

; UNDER CONSTRUCTION.
; TODO. PRE-BLANK RESULTS, OTHERWISE CAN SHOW PREVIOUS RESULTS WHEN ERROR.
; TODO. RUNTIME ERROR REPORTING.
; TODO. SOURCE STYLE: ".exit:", no ".Exit:", "L" labels names change.

; ScenarioMemoryDraw:
; push rbx rsi rdi rbp r12 r13 r14 r15
; cld
; TODO. Error handling.
; .error:
; pop r15 r14 r13 r12 rbp rdi rsi rbx
; ret



;--- Initializing and start measurement and drawings session, ---------------;
;    used for draw performance = f( block size ) function.                   ;
;    Include run  DrawSessionThread  subroutine as parallel thread           ;
;                                                                            ;
; INPUT:   None at registers                                                 ;
;          DialogueContent with user options settings data                   ;
;                                                                            ;
; OUTPUT:  CF = status flag, 0(NC)=No errors, 1(C)=Error                     ;
;          AL = error status, valid only if error detected (when CF=1)       ; 
;               see file "thread manager\sessionstart.inc" for decode        ;
;          UPB = Update user parameters block with GUI widgets settings      ;
;          IPB = Update input parameters block with benchmark scenario data  ;   
;                                                                            ;
; UPB = f (GUI) , IPB = f (UPB)                                              ;
;                                                                            ;
; All CPU registers can be corrupted, this rule for complex modules:         ;
; directory threads_manager\:                                                ;
;   sessionstart.inc, sessionstop.inc, sessionprogress.inc.                  ;
; directory gui\:                                                            ;
;   drawsessionstart.inc, drawsessionstop.inc                                ;
;   but not drawsessionthread.inc because OS callback                        ;       
; Remember about PUSH-POP in the callers.                                    ;
;----------------------------------------------------------------------------;

DrawSessionStart:
push rbx rsi rdi rbp r12 r13 r14 r15

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r15,[Registry]

call InfoProcessor

mov al,1  ; Run context = drawings, with memory allocation multiplication
call GuiToUserParms   ; Update UPB
call SessionStart     ; Initializing, allocate events, threads, memory, IPB
jc .exit              ; Go if error returned, AL = Error code

; Initializing pointers
cld

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r14,[Registry]

; lea rbx,[r14 + REGISTRY64.memIpb]  ;  [InputParms]
lea rbx,[MEM_IPB]


;lea r14,[DialogueContent_Win1]
; mov r14,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
lea r14,[BIND_LIST]

; TODO. CHECK REQUIRED THIS OR NOT ?
lea r15,[BasePointGui]


; mdAsm          db  80 + 1  dup ?
; mdPrefetch     db  40 + 1  dup ?
; mdObject       db  40 + 1  dup ?
; mdThreads      db  40 + 1  dup ?
; mdRepeats      db  40 + 1  dup ?
; mdPages        db  40 + 1  dup ?
; mdNuma         db  40 + 1  dup ?


; Down comments strings, assembler method
;lea rsi,[AM_Strings_Prefix]
;mov ecx,[rbx + IPB.UpdatedAsm]
;cmp ecx,ASM_RESULT_LIMIT
;jae .L10 
;lea rsi,[AM_Strings_Values]
;jrcxz .L10
;.L11:
;lodsb
;cmp al,0
;jne .L11
;loop .L11     ; find string indexed by RCX
;.L10:
;sub rsi,r15   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [r14 + HANDLE_TEXT_METHOD1*16 - 256 + 12],si

lea rdi,[r14 + BINDLIST.mdAsm]
mov eax,[rbx + MEMIPB.updatedAsm]
add ax,STR_AM_X64_00
call PoolStringWrite
mov al,0
stosb


; Down comments strings, prefetch distance (default, medium, long, N/A)
;lea rsi,[PdUnknown_String]
;mov ecx,[rbx + IPB.UpdatedDistance]
;cmp ecx,PD_LIMIT
;ja .L40 
;lea rsi,[PdDefault_String]
;jrcxz .L40
;.L41:
;lodsb
;cmp al,0
;jne .L41
;loop .L41                ; find string indexed by ECX
;.L40:
;sub rsi,r15   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [r14 + HANDLE_TEXT_DISTANCE1*16 - 256 + 12],si

lea rdi,[r14 + BINDLIST.mdPrefetch]
mov eax,[rbx + MEMIPB.updatedDistance]
add ax,STR_MD_PD_DEFAULT
call PoolStringWrite
mov al,0
stosb

; Down comments strings, target object (Cache, DRAM, Custom block)
;lea rsi,[Unknown_Object]
;mov ecx,[rbx + IPB.UpdatedTarget]
;cmp ecx,TARGET_LIMIT
;jae .L12 
;lea rsi,[Target_00]
;jrcxz .L12
;.L13:
;lodsb
;cmp al,0
;jne .L13
;loop .L13     ; find string indexed by RCX
;.L12:
;sub rsi,r15   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [r14 + HANDLE_TEXT_OBJECT1*16 - 256 + 12],si

lea rdi,[r14 + BINDLIST.mdObject]
mov eax,[rbx + MEMIPB.updatedTarget]
add ax,STR_MEMORY_L1
call PoolStringWrite
mov al,0
stosb


; Down comments strings, number of threads
;lea rdi,[Threads_Number]
;mov word [rdi],0000h + '?'
;mov eax,[rbx + IPB.UpdatedThreads]
;cmp eax,MAX_THREADS
;ja .L14
;push rbx    ; Store RBX = pointer to IPB
;mov bl,0    ; BL = print template mode
;call DecimalPrint32
;pop rbx
;.L14:

lea rdi,[r14 + BINDLIST.mdThreads]
mov ax,STR_MD_THREADS
call PoolStringWrite
push rbx
mov eax,[rbx + MEMIPB.updatedThreads]
mov bl,0
call DecimalPrint32
pop rbx
mov al,0
stosb

; TODO. REPEATS WRITE.

; Down comments strings, number of measurement repeats
;lea rdi,[Repeats_Number]
;push rdi 
;mov ecx,12
;mov al,' '
;rep stosb   ; Pre-blank text field, 
;pop rdi     ; note can be previous long text at buffer or screen 
;mov rax,[rbx + IPB.MeasureRepeats]
;push rbx    ; Store RBX = pointer to IPB
;mov rbx,0FFFFFFFF00000000h
;test rax,rbx   ; Required support overflows RAX=4GB, actual for adaptive modes
;jz .L18        ; Go if no overflow
;mov eax,' >4G'
;stosd
;jmp .L19
;.L18:
;mov bl,0    ; BL = print template mode
;call DecimalPrint32
;.L19:
;pop rbx

; TODO. MEASUREMENT MODE WRITE.

; Special strings for adaptive repeats modes
;lea rax,[Repeats_String]   ; Text string 'Repeats=' + numeric field
;cmp [rbx + IPB.UpdatedMeasure],MEASURE_CAREFUL
;jbe .L30                   ; Go if brief or careful, text string + numeric field
;lea rax,[Measure_D_02]
;cmp [rbx + IPB.UpdatedMeasure],MEASURE_B_ADAPTIVE
;je .L30                    ; Go if brief adaptive, text string only
;lea rax,[Measure_D_03]     ; otherwise careful adaptive, text string only
;.L30:
;sub rax,r15   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [r14 + HANDLE_TEXT_REPEATS1*16 - 256 + 12],ax

lea rdi,[r14 + BINDLIST.mdRepeats]
mov eax,[rbx + MEMIPB.updatedMeasure]
add ax,STR_MEASURE_BRIEF
call PoolStringWrite
mov al,0
stosb

; Down comments strings, normal or large pages
;lea rax,[NPages_String]
;cmp [rbx + IPB.UpdatedLP],LP_USED
;jne .L15 
;lea rax,[LPages_String]
;.L15:
;sub rax,r15   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [r14 + HANDLE_LARGE_PAGES1*16 - 256 + 12],ax


lea rdi,[r14 + BINDLIST.mdPages]
mov eax,[rbx + MEMIPB.updatedLP]
add ax,STR_MD_4K_PAGES
call PoolStringWrite
mov al,0
stosb


; Down comments strings, NUMA optimization mode
;lea rsi,[NumaOff_String]
;mov ecx,[rbx + IPB.UpdatedNUMA]
;cmp ecx,NUMA_LIMIT
;ja .L16             ; old bug = jae .L16  
;jrcxz .L16
;.L17:
;lodsb
;cmp al,0
;jne .L17
;loop .L17
;.L16:
;sub rsi,r15   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [r14 + HANDLE_NUMA_MODE1*16 - 256 + 12],si

lea rdi,[r14 + BINDLIST.mdNuma]
mov eax,[rbx + MEMIPB.updatedNUMA]
add ax,STR_MD_NUMA_OFF
call PoolStringWrite
mov al,0
stosb

; Support option, actual for drawings only: adaptive measurement modes
lea rsi,[AdaptiveRepeats]
mov eax,[rbx + MEMIPB.updatedMeasure]
sub eax,MEASURE_B_ADAPTIVE
jc .L20
cmp eax,1
ja .L20
mov rax,[rsi + rax*8]
mov [rbx + MEMIPB.adaptiveSeconds],rax

; Select Bandwidth or Latency measurement mode
; lea rsi,[ThreadsList]  ; RSI = storage for RCX reloads
;mov rsi,[Registry]
;add rsi,REGISTRY64.mpe64csb.threadList

lea rsi,[THREAD_LIST]
mov rcx,rsi
cmp [rbx + MEMIPB.updatedAsm],LATENCY_MODE
jae .L22

; Run with default recommended repeats count: Bandwidth mode
mov edx,[rbx + MEMIPB.updatedThreads]
call ThreadsReStart
jc .Error02                  ; If error, go to error handling branch
jmp .L21

; Run with default recommended repeats count: Latency mode
; Setup pattern for prepare walk list
.L22:
mov r8d,[rbx + MEMIPB.updatedThreads]
mov r9,[rbx + MEMIPB.patternRoutine]    ; for first call, prepare list for walk
call ThreadsUpdatePattern
; Run service pattern, prepare walk list, delta TSC ignored here
mov rcx,rsi
mov edx,[rbx + MEMIPB.updatedThreads]
call ThreadsReStart
jc .Error02                  ; If error, go to error handling branch
; Setup pattern for walk
mov rcx,rsi
mov r8d,[rbx + MEMIPB.updatedThreads]
mov r9,[rbx + MEMIPB.walkRoutine]       ; for second call, walk at prepared list
call ThreadsUpdatePattern
; Run measurement pattern, walk by prepared walk list, get delta TSC in the RAX
mov rcx,rsi
mov edx,[rbx + MEMIPB.updatedThreads]
call ThreadsReStart
jc .Error02                  ; If error, go to error handling branch

; Done point for adaptive modes ( bandwidth and latency branches ),
; pre-measurements executed, 
; next, test control = f ( results ) must be calculated
.L21:

; Calculate seconds and ratio after run with default recommended repeats count
; for measurements calibration 
push rax                            ; RAX = Delta TSC after ThreadsReStart
finit

;---

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rax,[Registry]

mov rcx,[CPU_DATA.tscClockHz]
mov [MEM_OPB.tscFrequencyHz],rcx

fld1
fild [MEM_OPB.tscFrequencyHz]  
fdivp st1,st0
fstp [MEM_OPB.tscPeriodS]

push 1000000000
fild qword [rsp]
fild [MEM_OPB.tscFrequencyHz]  
fdivp st1,st0
fstp [MEM_OPB.tscPeriodNs]
add rsp,8                                       ; TODO. Optimize this, select free register for POP

push 1000000
fild [MEM_OPB.tscFrequencyHz]
fild qword [rsp]
fdivp st1,st0
fstp [MEM_OPB.tscFrequencyMHz]
add rsp,8                                       ; TODO. Optimize this, select free register for POP

;---

fld [rbx + MEMIPB.adaptiveSeconds]     ; ST0 = Target seconds
fild qword [rsp]                    ; ST0 = Measured delta TSC , ST1 = Target seconds
; fmul [OutputParms.TSCperiodS]       ; ST0 = Measured seconds
fmul [MEM_OPB.tscPeriodS]
fdivp st1,st0                       ; ST0 = Target / Measured 
fild [rbx + MEMIPB.measureRepeats]     ; ST0 = Product repeats
fmulp st1,st0                       ; ST0 = Corrected product
fild [rbx + MEMIPB.startBlockSize]
fmulp st1,st0                       ; ST0 = Corrected product for divide per bytes count 
fistp [rbx + MEMIPB.adaptiveProduct]
pop rax                             ; This for release stack only

; Skip point for non-adaptive modes
.L20:

;******************************************************************************;
; DEBUG POINT

; INT3

;******************************************************************************;


; Note option: X-approximation modes, 
; interpreted and supported at drawsessionthread.inc module. 

; Load pointer to drawings parameters
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rdi,[Registry]

mov [GUI_PARMS.childWinHandle],0

lea rdi,[DRAW_PARMS]

; Calculate X grid step values, must be integer power of 2 for next step correct
; Note this operation increase memory size requirements, because increase RAX
mov rax,[rbx + MEMIPB.startBlockSize]
bsr rcx,rax
mov eax,1
shl eax,cl   ; RAX = Target block size, note example here RAX=8K for L1=32K

; Special support for DRAM and Custom modes
mov ecx,[rbx + MEMIPB.updatedTarget]
cmp ecx,TARGET_DRAM
je .L1                    ; Go for size / 8 if target = DRAM
cmp ecx,TARGET_CUSTOM
jne .L2                   ; Go skip without size / 8 if target = not a Custom
.L1:
shr eax,3                 ; size / 8 , for DRAM and Custom modes
.L2:

; Set Xstart, Xdelta for drawings
push rax                            ; RAX = Block size per X-grid
xor edx,edx                         ; RDX = Dividend high 64 bits, =0
mov ecx,GRIDSTEPX                   ; RCX = Pixels per X-grid
div rcx                             ; (Block size per grid) / (pixels per grid)
mov [rbx + MEMIPB.startBlockSize],rax  ; X start value
mov [rbx + MEMIPB.deltaBlockSize],rax  ; X addend per draw iteration 
pop rax

; Update and store X grid step values
; Select and store X grid units: Bytes, Kilobytes, Megabytes
xor edx,edx                ; EDX = SelectUnits, 0=Bytes / 1=KB / 2=MB
cmp eax,1024
jb @f                      ; Go with Units=Bytes if grid step < 1 KB  
inc edx
shr eax,10
cmp eax,1024
jb @f                      ; Go with Units=KB if grid step < 1 MB
inc edx
shr eax,10
@@:                        ; Otherwise Units=MB
mov [rdi + DRPM.valueGridX],eax
mov [rdi + DRPM.selectUnits],edx

; Set first approximation constant for Y-grid step values
mov eax,DEFAULT_Y_MBPS_PER_GRID
xor edx,edx
cmp [rbx + MEMIPB.updatedAsm],LATENCY_MODE
jb @f
mov eax,DEFAULT_Y_NS_PER_GRID
inc edx
@@:
mov [rdi + DRPM.valueGridY],eax
mov [rdi + DRPM.selectMode],edx

; Set Ymultiplier scale constant for conversion 
; measurement results to Y-coordinate
; Note same operation at win1.inc for re-scaling, 
finit
push rax -GRIDSTEPY
fild dword [rsp]      ; ST0 = - grid step, pixels
fidiv dword [rsp+8]   ; ST0 = ( - grid step, pixels ) / ( Y units per grid )      
fstp [rdi + DRPM.yMultiplier]
pop rax rax

; Align stack and create parameters shadow, required for use WinAPI
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32

; Initializing handles and run thread with DrawSessionThread  subroutine 
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rsi,[Registry]

lea rsi,[DRAW_THREAD_PARMS]  ; TODO. Possible ADD.
mov rdi,rsi
cld
; Create event for thread start, store event handle
xor ecx,ecx          ; RCX = Parm#1 = lpEventAttributes  
mov edx,1            ; RDX = Parm#2 = bManualReset
xor r8d,r8d          ; R8  = Parm#3 = bInitialState
xor r9d,r9d          ; R9  = Parm#4 = Name, 0=None
call [CreateEvent]   ; Call WinAPI function
test rax,rax         ; Status=0 means error
jz .error            ; Go if error creating event
stosq                ; Store handle in the thread parameters, EventStart 
; Create event for thread done, store event handle
xor ecx,ecx          ; RCX = Parm#1 = lpEventAttributes  
mov edx,1            ; RDX = Parm#2 = bManualReset
xor r8d,r8d          ; R8  = Parm#3 = bInitialState
xor r9d,r9d          ; R9  = Parm#4 = Name, 0=None
call [CreateEvent]   ; Call WinAPI function
test rax,rax         ; Status=0 means error
jz .error            ; Go if error creating event
stosq                ; Store handle in the thread parameters, EventDone 
; Create thread, store event handle
xor ecx,ecx                 ; RCX = Parm#1 = Security
xor edx,edx                 ; RDX = Parm#2 = Stack size, 0=default
lea r8,[DrawSessionThread]  ; R8  = Parm#3 = Execute routine pointer
xor r9d,r9d                 ; R9  = Parm#4 = Subroutine parameter, not used here
push rcx                    ; Parm#6 = Thread ID return , 0=None
push rcx                    ; Parm#5 = Creating Flags = Not suspended, run immediately
sub rsp,32                  ; Make parameters shadow
call [CreateThread]         ; Call WinAPI function
add rsp,32+16               ; Remove parameters shadow + 2 parameters
test rax,rax                ; Status=0 means error
jz .error                   ; Go if error creating thread
stosq                       ; Store handle in the thread parameters, ThreadHandle 
; Clear measurement counter and array of measurement results
mov ecx,DRAW_POINTS_COUNT + 20 - 3   ; OLD: + 4 dwords: 2 counters + break flag + align
xor eax,eax                          ; -3 because 3 qwords already stored
rep stosq

; Done, no errors, parallel thread with  DrawSessionThread  subroutine RUNNING
clc
.L90:
mov rsp,rbp
.exit:                 ; Note AL = error status must be valid at this point
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret

; Exit point for errors detected
.error:
stc
jmp .L90

; CF=1 with AL=2  means timings measurement error, 
; note this branch without valid RBP
.Error02:
mov al,2
stc
; ret
jmp .exit



;--- Stop measurement and drawings session, release resources ---------------;
;    used for draw performance = f( block size ) function.                   ;
;    Include terminate  DrawSessionThread  subroutine as parallel thread     ;
;                                                                            ;
; INPUT:   None at CPU registers                                             ;
;                                                                            ;
; OUTPUT:  CF = status flag, 0(NC)=No errors, 1(C)=Error                     ;
;          AL = error status, valid only if error detected (when CF=1)       ; 
;               see file "thread manager\sessionstop.inc" for decode         ;   
;                                                                            ;
; All CPU registers can be corrupted, this rule for complex modules:         ;
; directory threads_manager\:                                                ;
;   sessionstart.inc, sessionstop.inc, sessionprogress.inc.                  ;
; directory gui\:                                                            ;
;   drawsessionstart.inc, drawsessionstop.inc                                ;
;   but not drawsessionthread.inc because OS callback                        ;       
; Remember about PUSH-POP in the callers.                                    ;
;----------------------------------------------------------------------------;

DrawSessionStop:
push rbx rsi rdi rbp r12 r13 r14 r15

call SessionStop    ; De-initializing, release events, threads, memory
jc .exit            ; Go if error returned, AL = Error code
; Align stack and create parameters shadow, required for use WinAPI
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
; Initializing pointer
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rsi,[Registry]

lea rsi,[DRAW_THREAD_PARMS]
cld
; Close handles, halt and remove thread with DrawSessionThread  subroutine
; Terminate threads
mov rcx,[rsi + DTHP.threadHandle]   ; RCX = Parm#1 = THREAD handle
jrcxz @f                            ; Skip if this entry not allocated
xor edx,edx                         ; RDX = Parm#2 = Thread exit code
call [TerminateThread]              ; Call WinAPI function
test rax,rax                        ; Status=0 means error
jz .error                           ; Go exit if error
@@:
; Close Start Events
lodsq
xchg rcx,rax                        ; RCX = Parm#1 = Load EVENT handle
jrcxz @f                            ; Skip if this entry not allocated
call [CloseHandle]                  ; Call WinAPI function
test rax,rax                        ; Status=0 means error
jz .error                           ; Go exit if error 
@@:
; Close Done Events
lodsq
xchg rcx,rax                        ; RCX = Parm#1 = Load EVENT handle
jrcxz @f                            ; Skip if this entry not allocated
call [CloseHandle]                  ; Call WinAPI function
test rax,rax                        ; Status=0 means error
jz .error                           ; Go exit if error 
@@:
; Close Threads
lodsq
xchg rcx,rax                        ; RCX = Parm#1 = Load THREAD handle
jrcxz @f                            ; Skip if this entry not allocated
call [CloseHandle]                  ; Call WinAPI function
test rax,rax                        ; Status=0 means error
jz .error                           ; Go exit if error
@@:

; Done, parallel thread with  DrawSessionThread  subroutine HALTED and REMOVED
clc
.L90:
mov rsp,rbp
.exit:                 ; Note AL = error status must be valid at this point

pop r15 r14 r13 r12 rbp rdi rsi rbx
ret

; Exit point for errors detected
.error:
stc
jmp .L90


;--- This subroutine runs as parallel background thread, make measurements ----; 
;    and store results. Used for draw performance = f( block size ) function.  ;
;                                                                              ;
; INPUT:   None at CPU registers                                               ;
;                                                                              ;
; OUTPUT:  RAX = Return code = 0                                               ;
;          Non-volatile regs. unchanged for Microsoft x64  calling convention  ;
;                                                                              ;
; CPU registers volatile / non volatile by Microsoft x64 calling convention,   ;
; note about this subroutine is object for OS callback.                        ;
;------------------------------------------------------------------------------;

DrawSessionThread:

; Entry point, must be compliant Microsoft x64 calling convention.
push rbx rsi rdi rbp r12 r13 r14 r15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
push rbp
sub rsp,32+8

; Drawings window invalidation for silent mode: show at drawings start 

; DEBUG MODE,
; REMOVE REGISTRY AT UPDATED VERSION, USE TEMPLATE AT EXE FILE.
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r14,[Registry]


cmp [GUI_PARMS.silentMode],0
je .SkipSilent
mov rcx,[GUI_PARMS.childWinHandle]  ; RCX = Parm#1 = Window handle
jrcxz .SkipSilent
xor edx,edx            ; RDX = Parm#2 = Pointer to invalidated region, 0=All window
xor r8d,r8d            ; R8  = Parm#3 = Background erase flag, 0=Background unchanged
call [InvalidateRect]  ; This cause callback WM_PAINT operation
.SkipSilent:

; Load pointer to thread control structure
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r15,[Registry]

lea r15,[DRAW_THREAD_PARMS]

; Signaling thread START
mov rcx,[r15 + DTHP.eventStart]   ; RCX = Parm#1 = Handle for thread start event
call [SetEvent]                   ; Call WinAPI function

; Measurement initialization, pre-heat
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rcx,[Registry]

; lea rsi,[rcx + REGISTRY64.memIpb]
lea rsi,[MEM_IPB]

;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov edx,[rsi + MEMIPB.updatedThreads]
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread

; Measurement cycle
.BackgroundProcess:

cmp [r15 + DTHP.measureCounter],DRAW_POINTS_COUNT
je .SilentBranch
ja .BackgroundSkip    ; Can replace this to wait with minimum utilization ?

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rdi,[Registry]

; lea rsi,[rdi + REGISTRY64.memIpb]
lea rsi,[MEM_IPB]

; lea rdi,[rsi + REGISTRY64.memOpb]
lea rdi,[MEM_OPB]

; Update block size at threads control block
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rcx,[Registry]

;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov rdx,rsi
mov r8d,[rsi + MEMIPB.updatedThreads]
mov r9,[rsi + MEMIPB.startBlockSize]
call ThreadsUpdateBlockSize

cmp [rsi + MEMIPB.updatedMeasure],MEASURE_B_ADAPTIVE
jb .NoAdaptive

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rcx,[Registry]

;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov rdx,rsi
mov r8d,[rsi + MEMIPB.updatedThreads]
call ThreadsUpdateAdaptive
.NoAdaptive:

; Select Bandwidth or Latency measurement mode
cmp [rsi + MEMIPB.updatedAsm],LATENCY_MODE
jae .LatencyMode 

.BandwidthMode:
; Measurement pattern
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rcx,[Registry]

;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov edx,[rsi + MEMIPB.updatedThreads]
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread
jmp .DoneBandwidthOrLatency

.LatencyMode:
; Setup pattern for prepare walk list
; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rcx,[Registry]

;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov r8d,[rsi + MEMIPB.updatedThreads]
mov r9,[rsi + MEMIPB.patternRoutine]
call ThreadsUpdatePattern
; Run service pattern, prepare walk list, delta TSC ignored here
;mov rcx,[Registry]
;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov edx,[rsi + MEMIPB.updatedThreads]
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread

; Setup pattern for walk
;mov rcx,[Registry]
;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]

lea rcx,[THREAD_LIST]

mov r8d,[rsi + MEMIPB.updatedThreads]
mov r9,[rsi + MEMIPB.walkRoutine]
call ThreadsUpdatePattern
; Run measurement pattern, walk by prepared walk list, get delta TSC in the RAX
;mov rcx,[Registry]
;lea rcx,[rcx + REGISTRY64.mpe64csb.threadList]
lea rcx,[THREAD_LIST]

mov edx,[rsi + MEMIPB.updatedThreads]
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread

.DoneBandwidthOrLatency:

; Calculte and store CPI = Clocks per Instruction,
; CPI = f ( Delta TSC, Instructions count )
mov edx,[r15 + DTHP.measureCounter]
shl edx,3      ; 2^3 = 8 bytes per store (QWORD)
lea rdi,[r15 + DTHP.measureArray + rdx]
push rax       ; RAX = Delta TSC per target operation, integer 64-bit
finit
fild qword [rsp]
mov rax,[rsi + MEMIPB.startBlockSize]  ; Block size, units = bytes
mul [rsi + MEMIPB.measureRepeats]      ; Multiply by total bytes per measurement cycle
mov ecx,[rsi + MEMIPB.operandWidth]    ; RCX = bits per instruction
shr ecx,3                           ; RCX = bytes per instruction
xor edx,edx
div rcx                             ; Note can use x87 to prevent exception 0
mov [rsp],rax
fild qword [rsp]  ; ST0 = Instruction cound, ST1 = Delta TSC
fdivp st1,st0     ; ST0 = CPI
fstp qword [rsp]
pop rax        ; RAX = Clocks per Instruction (CPI), floating point, double
stosq          ; Store this for communication with parent thread

; Update block size for performance = f(size)
mov rax,[rsi + MEMIPB.deltaBlockSize]
add [rsi + MEMIPB.startBlockSize],rax

; Cycle for measurement draw, increment counter by 1 drawing pixel
inc [r15 + DTHP.measureCounter]

; Support approximation mode, x16 and x32. One pixel already saved, required
; it copy  16-1=15  or  32-1=31  times.
mov eax,[rsi + MEMIPB.updatedApprox]
test eax,eax
jz .NoApproxMode       ; go if no approximation mode
mov ecx,15             ; 1 already + 15 duplicated = 16 total stores
cmp eax,APPROX_X16
je .YesApproxMode
mov cl,31              ; 1 already + 31 duplicated = 32 total stores
.YesApproxMode:        ; approximation duplication cycle
mov rax,[rdi-8]
stosq                  ; one duplication per loop
mov r8,[rsi + MEMIPB.deltaBlockSize]
add [rsi + MEMIPB.startBlockSize],r8
inc [r15 + DTHP.measureCounter]
loop .YesApproxMode    ; approximation duplication cycle
.NoApproxMode:

; Cycle with support termination by break
.BackgroundSkip:
cmp [r15 + DTHP.measureBreak],0
je .BackgroundProcess

; Signaling thread STOP
mov rcx,[r15 + DTHP.eventDone]    ; RCX = Parm#1 = Handle for thread done event
call [SetEvent]                   ; Call WinAPI function

; Exit point
add rsp,32+8
pop rbp
mov rsp,rbp
pop r15 r14 r13 r12 rbp rdi rsi rbx
xor eax,eax                       ; RAX = 0, return code = 0
ret

; Drawings window invalidation for silent mode: show at drawings end
.SilentBranch:

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov rcx,[Registry]

mov al,0
xchg [GUI_PARMS.silentMode],al
cmp al,0
je .BackgroundSkip
mov rcx,[GUI_PARMS.childWinHandle]  ; Parm#1 = Window handle
jrcxz .BackgroundSkip
xor edx,edx            ; RDX = Parm#2 = Pointer to invalidated region, 0=All window
xor r8d,r8d            ; R8  = Parm#3 = Background erase flag, 0=Background unchanged
call [InvalidateRect]  ; This cause callback WM_PAINT operation
jmp .BackgroundSkip 

