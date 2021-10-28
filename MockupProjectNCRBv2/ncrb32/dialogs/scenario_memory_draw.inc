

; UNDER CONSTRUCTION.
; TODO. PRE-BLANK RESULTS, OTHERWISE CAN SHOW PREVIOUS RESULTS WHEN ERROR.
; TODO. RUNTIME ERROR REPORTING.
; TODO. SOURCE STYLE: ".exit:", no ".Exit:", "L" labels names change.


; ScenarioMemoryDraw:
; pushad
; cld
; ; TODO. Error handling.
; .error:
; popad
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
pushad

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]

call InfoProcessor

mov al,1  ; Run context = drawings, with memory allocation multiplication
call GuiToUserParms   ; Update UPB
call SessionStart     ; Initializing, allocate events, threads, memory, IPB
jc .exit              ; Go if error returned, AL = Error code

; Initializing pointers
cld

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov ebx,[Registry]

; mov ebp,[ebx + REGISTRY32.allocatorBindBuffer.objectStart]
lea ebp,[BIND_LIST]

; lea ebx,[ebx + REGISTRY32.memIpb]

lea ebx,[MEM_IPB]

; Down comments strings, assembler method
;lea esi,[AM_Strings_Prefix]
;mov ecx,[ebx + IPB.UpdatedAsm]
;cmp ecx,ASM_RESULT_LIMIT
;jae .L10 
;lea esi,[AM_Strings_Values]
;jecxz .L10
;.L11:
;lodsb
;cmp al,0
;jne .L11
;loop .L11                ; find string indexed by RCX
;.L10:
;sub esi,BasePointGui     ; calculate ESI.[15-0] = compact 16-bit offset
;mov [DialogueContent_Win1 + HANDLE_TEXT_METHOD1*16 - 256 + 12],si

lea edi,[ebp + BINDLIST.mdAsm]
mov eax,[ebx + MEMIPB.updatedAsm]
mov cx,STR_AM_IA32_00
cmp al,5
jbe @f
mov cx,STR_AM_X64_00
@@:
add ax,cx
call PoolStringWrite
mov al,0
stosb

; Down comments strings, prefetch distance (default, medium, long, N/A)
;lea esi,[PdUnknown_String]
;mov ecx,[ebx + IPB.UpdatedDistance]
;cmp ecx,PD_LIMIT
;ja .L40 
;lea esi,[PdDefault_String]
;jecxz .L40
;.L41:
;lodsb
;cmp al,0
;jne .L41
;loop .L41                ; find string indexed by ECX
;.L40:
;sub esi,BasePointGui     ; calculate ESI.[15-0] = compact 16-bit offset
;mov [DialogueContent_Win1 + HANDLE_TEXT_DISTANCE1*16 - 256 + 12],si

lea edi,[ebp + BINDLIST.mdPrefetch]
mov eax,[ebx + MEMIPB.updatedDistance]
add ax,STR_MD_PD_DEFAULT
call PoolStringWrite
mov al,0
stosb


; Down comments strings, target object (Cache, DRAM, Custom block)
;lea esi,[Unknown_Object]
;mov ecx,[ebx + IPB.UpdatedTarget]
;cmp ecx,TARGET_LIMIT
;jae .L12 
;lea esi,[Target_00]
;jecxz .L12
;.L13:
;lodsb
;cmp al,0
;jne .L13
;loop .L13                ; find string indexed by RCX
;.L12:
;sub esi,BasePointGui     ; calculate RSI.[15-0] = compact 16-bit offset
;mov [DialogueContent_Win1 + HANDLE_TEXT_OBJECT1*16 - 256 + 12],si

lea edi,[ebp + BINDLIST.mdObject]
mov eax,[ebx + MEMIPB.updatedTarget]
add ax,STR_MEMORY_L1
call PoolStringWrite
mov al,0
stosb


; Down comments strings, number of threads
;lea edi,[Threads_Number]
;mov word [edi],0000h + '?'
;mov eax,[ebx + IPB.UpdatedThreads]
;cmp eax,MAX_THREADS
;ja .L14
;push ebx    ; Store EBX = pointer to IPB
;mov bl,0    ; BL = print template mode
;call DecimalPrint32
;pop ebx
;.L14:

lea edi,[ebp + BINDLIST.mdThreads]
mov ax,STR_MD_THREADS
call PoolStringWrite
push ebx
mov eax,[ebx + MEMIPB.updatedThreads]
mov bl,0
call DecimalPrint32
pop ebx
mov al,0
stosb

; TODO. REPEATS WRITE.

; Down comments strings, number of measurement repeats
;lea edi,[Repeats_Number]
;push edi 
;mov ecx,12
;mov al,' '
;rep stosb   ; Pre-blank text field, 
;pop edi     ; note can be previous long text at buffer or screen 
;mov eax,dword [ebx + IPB.MeasureRepeats + 0]
; Required support overflows Repeats Count > 4GB, actual for adaptive modes
;cmp dword [ebx + IPB.MeasureRepeats + 4],0
;je .L18        ; Go if no overflow
;mov eax,' >4G'
;stosd
;jmp .L19
;.L18:
;push ebx
;mov bl,0    ; BL = print template mode
;call DecimalPrint32
;pop ebx
;.L19:

; TODO. MEASUREMENT MODE WRITE.

; Special strings for adaptive repeats modes
;lea eax,[Repeats_String]   ; Text string 'Repeats=' + numeric field
;cmp [ebx + IPB.UpdatedMeasure],MEASURE_CAREFUL
;jbe .L30                   ; Go if brief or careful, text string + numeric field
;lea eax,[Measure_D_02]
;cmp [ebx + IPB.UpdatedMeasure],MEASURE_B_ADAPTIVE
;je .L30                    ; Go if brief adaptive, text string only
;lea eax,[Measure_D_03]     ; otherwise careful adaptive, text string only
;.L30:
;sub eax,BasePointGui        ; calculate ESI.[15-0] = compact 16-bit offset
;mov [DialogueContent_Win1 + HANDLE_TEXT_REPEATS1*16 - 256 + 12],ax

lea edi,[ebp + BINDLIST.mdRepeats]
mov eax,[ebx + MEMIPB.updatedMeasure]
add ax,STR_MEASURE_BRIEF
call PoolStringWrite
mov al,0
stosb


; Down comments strings, normal or large pages
;lea eax,[NPages_String]
;cmp [ebx + IPB.UpdatedLP],LP_USED
;jne .L15 
;lea eax,[LPages_String]
;.L15:
;sub eax,BasePointGui        ; calculate RSI.[15-0] = compact 16-bit offset
;mov [DialogueContent_Win1 + HANDLE_LARGE_PAGES1*16 - 256 + 12],ax

lea edi,[ebp + BINDLIST.mdPages]
mov eax,[ebx + MEMIPB.updatedLP]
add ax,STR_MD_4K_PAGES
call PoolStringWrite
mov al,0
stosb


; Down comments strings, NUMA optimization mode
;lea esi,[NumaOff_String]
;mov ecx,[ebx + IPB.UpdatedNUMA]
;cmp ecx,NUMA_LIMIT
;ja .L16             ; old bug = jae .L16  
;jecxz .L16
;.L17:
;lodsb
;cmp al,0
;jne .L17
;loop .L17
;.L16:
;sub esi,BasePointGui   ; calculate RSI.[15-0] = compact 16-bit offset
;mov [DialogueContent_Win1 + HANDLE_NUMA_MODE1*16 - 256 + 12],si

lea edi,[ebp + BINDLIST.mdNuma]
mov eax,[ebx + MEMIPB.updatedNUMA]
add ax,STR_MD_NUMA_OFF
call PoolStringWrite
mov al,0
stosb


; Support option, actual for drawings only: adaptive measurement modes
lea esi,[AdaptiveRepeats]
mov eax,[ebx + MEMIPB.updatedMeasure]
sub eax,MEASURE_B_ADAPTIVE
jc .L20
cmp eax,1
ja .L20
mov ecx,[esi + eax*8 + 0]
mov dword [ebx + MEMIPB.adaptiveSeconds + 0],ecx
mov ecx,[esi + eax*8 + 4]
mov dword [ebx + MEMIPB.adaptiveSeconds + 4],ecx

; Select Bandwidth or Latency measurement mode
;lea esi,[ThreadsList]  ; ESI = storage for ECX reloads
;mov esi,[Registry]
;add esi,REGISTRY32.mpe32csb.threadList
lea esi,[THREAD_LIST]

mov ecx,esi
cmp [ebx + MEMIPB.updatedAsm],LATENCY_MODE
jae .L22

; Run with default recommended repeats count: Bandwidth mode
mov edx,[ebx + MEMIPB.updatedThreads]
call ThreadsReStart
jc .Error02                  ; If error, go to error handling branch
jmp .L21

; Run with default recommended repeats count: Latency mode
; Setup pattern for prepare walk list
.L22:
push ebx
push [ebx + MEMIPB.updatedThreads]
mov eax,[ebx + MEMIPB.patternRoutine]    ; for first call, prepare list for walk
mov ebx,ecx
pop ecx
call ThreadsUpdatePattern
pop ebx
; Run service pattern, prepare walk list, delta TSC ignored here
mov ecx,esi
mov edx,[ebx + MEMIPB.updatedThreads]
call ThreadsReStart
jc .Error02                  ; If error, go to error handling branch
; Setup pattern for walk
push ebx
mov ecx,[ebx + MEMIPB.updatedThreads]
mov eax,[ebx + MEMIPB.walkRoutine]       ; for second call, walk at prepared list
mov ebx,esi
call ThreadsUpdatePattern
pop ebx

; Run measurement pattern, walk by prepared walk list, get delta TSC in the RAX
mov ecx,esi
mov edx,[ebx + MEMIPB.updatedThreads]
call ThreadsReStart
jc .Error02                  ; If error, go to error handling branch

; Done point for adaptive modes ( bandwidth and latency branches ),
; pre-measurements executed, 
; next, test control = f ( results ) must be calculated
.L21:

; Calculate seconds and ratio after run with default recommended repeats count
; for measurements calibration 

push edx eax                        ; EDX:EAX = Delta TSC after ThreadsReStart
finit

;---

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

mov ecx,dword [CPU_DATA.tscClockHz + 0]
mov edx,dword [CPU_DATA.tscClockHz + 4]

mov dword [MEM_OPB.tscFrequencyHz + 0],ecx
mov dword [MEM_OPB.tscFrequencyHz + 4],edx

fld1
fild [MEM_OPB.tscFrequencyHz]  
fdivp st1,st0
fstp [MEM_OPB.tscPeriodS]

push 1000000000
fild dword [esp]
fild [MEM_OPB.tscFrequencyHz]  
fdivp st1,st0
fstp [MEM_OPB.tscPeriodNs]
add esp,4                                       ; TODO. Optimize this, select free register for POP

push 1000000
fild [MEM_OPB.tscFrequencyHz]
fild dword [esp]
fdivp st1,st0
fstp [MEM_OPB.tscFrequencyMHz]
add esp,4                                       ; TODO. Optimize this, select free register for POP

;---

fld [ebx + MEMIPB.adaptiveSeconds]     ; ST0 = Target seconds
fild qword [esp]                    ; ST0 = Measured delta TSC , ST1 = Target seconds

; fmul [OutputParms.tscPeriodS]       ; ST0 = Measured seconds
fmul [MEM_OPB.tscPeriodS]

fdivp st1,st0                       ; ST0 = Target / Measured 
fild [ebx + MEMIPB.measureRepeats]     ; ST0 = Product repeats
fmulp st1,st0                       ; ST0 = Corrected product
fild [ebx + MEMIPB.startBlockSize]
fmulp st1,st0                       ; ST0 = Corrected product for divide per bytes count 
fistp [ebx + MEMIPB.adaptiveProduct]
pop eax edx                         ; This for release stack only

; Skip point for non-adaptive modes
.L20:

; Note option: X-approximation modes, 
; interpreted and supported at drawsessionthread.inc module. 

; Load pointer to drawings parameters

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov edi,[Registry]

mov [GUI_PARMS.childWinHandle],0

; lea edi,[DrawParms]
lea edi,[DRAW_PARMS]

; Calculate X grid step values, must be integer power of 2 for next step correct
; Note this operation increase memory size requirements, because increase RAX
mov eax,[ebx + MEMIPB.startBlockSize]
bsr ecx,eax
mov eax,1
shl eax,cl   ; RAX = Target block size, note example here EAX=8K for L1=32K

; Special support for DRAM and Custom modes
mov ecx,[ebx + MEMIPB.updatedTarget]
cmp ecx,TARGET_DRAM
je .L1                    ; Go for size / 8 if target = DRAM
cmp ecx,TARGET_CUSTOM
jne .L2                   ; Go skip without size / 8 if target = not a Custom
.L1:
shr eax,3                 ; size / 8 , for DRAM and Custom modes
.L2:

; Set Xstart, Xdelta for drawings
push eax                            ; EAX = Block size per X-grid
xor edx,edx                         ; RDX = Dividend high 64 bits, =0
mov ecx,GRIDSTEPX                   ; RCX = Pixels per X-grid
div ecx                             ; (Block size per grid) / (pixels per grid)
mov [ebx + MEMIPB.startBlockSize],eax  ; X start value
mov [ebx + MEMIPB.deltaBlockSize],eax  ; X addend per draw iteration 
pop eax

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
mov [edi + DRPM.valueGridX],eax
mov [edi + DRPM.selectUnits],edx

; Set first approximation constant for Y-grid step values
mov eax,DEFAULT_Y_MBPS_PER_GRID
xor edx,edx
cmp [ebx + MEMIPB.updatedAsm],LATENCY_MODE
jb @f
mov eax,DEFAULT_Y_NS_PER_GRID
inc edx
@@:
mov [edi + DRPM.valueGridY],eax
mov [edi + DRPM.selectMode],edx

; Set Ymultiplier scale constant for conversion 
; measurement results to Y-coordinate
; Note same operation at win1.inc for re-scaling, 
finit
push eax -GRIDSTEPY
fild dword [esp]        ; ST0 = - grid step, pixels
fidiv dword [esp + 4]   ; ST0 = ( - grid step, pixels ) / ( Y units per grid )      
fstp [edi + DRPM.yMultiplier]
pop eax eax

; Initializing handles and run thread with DrawSessionThread  subroutine 
;lea esi,[DrawThreadParms]

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]


lea esi,[DRAW_THREAD_PARMS]  ; TODO. Possible ADD.

mov edi,esi
cld
; Create event for thread start, store event handle
push 0               ; Parm#4 = Name, 0=None
push 0               ; Parm#3 = bInitialState
push 1               ; Parm#2 = bManualReset
push 0               ; Parm#1 = lpEventAttributes
call [CreateEvent]   ; Call WinAPI function
test eax,eax         ; Status=0 means error
jz .error            ; Go if error creating event
stosd                ; Store handle in the thread parameters, EventStart 
; Create event for thread done, store event handle
push 0               ; Parm#4 = Name, 0=None
push 0               ; Parm#3 = bInitialState
push 1               ; Parm#2 = bManualReset
push 0               ; Parm#1 = lpEventAttributes
call [CreateEvent]   ; Call WinAPI function
test eax,eax         ; Status=0 means error
jz .error            ; Go if error creating event
stosd                ; Store handle in the thread parameters, EventDone 
; Create thread, store event handle
xor eax,eax          ; EAX = 0 for compact PUSH 0
push eax                    ; Parm#6 = Thread ID return , 0=None
push eax                    ; Parm#5 = Creating Flags = Not suspended, run immediately
push eax                    ; Parm#4 = Subroutine parameter, not used here
push DrawSessionThread      ; Parm#3 = Execute routine pointer
push eax                    ; Parm#2 = Stack size, 0=default
push eax                    ; Parm#1 = Security
call [CreateThread]         ; Call WinAPI function
test eax,eax                ; Status=0 means error
jz .error                   ; Go if error creating thread
stosd                       ; Store handle in the thread parameters, ThreadHandle 
; Clear measurement counter and array of measurement results
;- mov ecx,( DRAW_POINTS_COUNT + 20 - 3 ) * 2  ; OLD: + 4 dwords: 2 counters + break flag + align
;-                                             ; -3 because 3 qwords already stored
mov ecx,( DRAW_POINTS_COUNT + 15 ) * 2 + 3
xor eax,eax
rep stosd

; Done, no errors, parallel thread with  DrawSessionThread  subroutine RUNNING
clc
.L90:
.exit:                 ; Note AL = error status must be valid at this point
popad
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
pushad

call SessionStop    ; De-initializing, release events, threads, memory
jc .exit            ; Go if error returned, AL = Error code

; Initializing pointer
;lea esi,[DrawThreadParms]

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]

lea esi,[DRAW_THREAD_PARMS]

cld

; Close handles, halt and remove thread with DrawSessionThread  subroutine
; Terminate threads
mov ecx,[esi + DTHP.threadHandle]   ; ECX = THREAD handle
jecxz @f                            ; Skip if this entry not allocated
push 0                              ; Parm#2 = Thread exit code
push ecx                            ; Parm#1 = THREAD handle 
call [TerminateThread]              ; Call WinAPI function
test eax,eax                        ; Status=0 means error
jz .error                           ; Go exit if error
@@:
; Close Start Events
lodsd                               ; EAX = Load EVENT handle
test eax,eax
jz @f                               ; Skip if this entry not allocated
push eax                            ; Parm#1 = EVENT handle
call [CloseHandle]                  ; Call WinAPI function
test eax,eax                        ; Status=0 means error
jz .error                           ; Go exit if error 
@@:
; Close Done Events
lodsd                               ; EAX = Load EVENT handle
test eax,eax
jz @f                               ; Skip if this entry not allocated
push eax                            ; Parm#1 = EVENT handle 
call [CloseHandle]                  ; Call WinAPI function
test eax,eax                        ; Status=0 means error
jz .error                           ; Go exit if error 
@@:
; Close Threads
lodsd                               ; EAX = Load THREAD handle 
test eax,eax
jz @f                               ; Skip if this entry not allocated
push eax                            ; Parm#1 = THREAD handle
call [CloseHandle]                  ; Call WinAPI function
test eax,eax                        ; Status=0 means error
jz .error                           ; Go exit if error
@@:

; Done, parallel thread with  DrawSessionThread  subroutine HALTED and REMOVED
clc
.L90:
.exit:                 ; Note AL = error status must be valid at this point
popad
ret

; Exit point for errors detected
.error:
stc
jmp .L90


;--- This subroutine runs as parallel background thread, make measurements ----; 
;    and store results. Used for draw performance = f( block size ) function.  ;
;                                                                              ;
; INPUT:   None at CPU registers                                               ;
;          But reserve space for one DWORD parameter in the stack              ;
;                                                                              ;
; OUTPUT:  EAX = Return code = 0                                               ;
;          Non-volatile regs. unchanged for Microsoft ia32  calling convention ;
;                                                                              ;
; CPU registers volatile / non volatile by Microsoft ia32 calling convention,  ;
; note about this subroutine is object for OS callback.                        ;
;------------------------------------------------------------------------------;

DrawSessionThread:

; Entry point, must be compliant Microsoft ia32 calling convention.
; TODO. Use PUSHAD/POPAD.

push ebx esi edi ebp

; DEBUG MODE,
; REMOVE REGISTRY AT UPDATED VERSION, USE TEMPLATE AT EXE FILE.

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

; Drawings window invalidation for silent mode: show at drawings start 
cmp [GUI_PARMS.silentMode],0
je .SkipSilent
mov eax,[GUI_PARMS.childWinHandle]
test eax,eax
jz .SkipSilent
push 0                 ; Parm#3 = Background erase flag, 0=Background unchanged
push 0                 ; Parm#2 = Pointer to invalidated region, 0=All window
push eax               ; Parm#1 = Window handle
call [InvalidateRect]  ; This cause callback WM_PAINT operation
.SkipSilent:

; Signaling thread START

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

push [DRAW_THREAD_PARMS.eventStart]   ; Parm#1 = Handle for thread start event
call [SetEvent]                            ; Call WinAPI function

; Measurement initialization, pre-heat

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

; lea esi,[eax + REGISTRY32.memIpb]                 ; ESI = Pointer to IPB
lea esi,[MEM_IPB]

; lea ecx,[eax + REGISTRY32.mpe32csb.threadList]                ; ECX = Pointer to Threads List
lea ecx,[THREAD_LIST]

mov edx,[esi + MEMIPB.updatedThreads]   ; EDX = Threads count
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread

; Measurement cycle
.BackgroundProcess:

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

cmp [DRAW_THREAD_PARMS.measureCounter],DRAW_POINTS_COUNT
je .SilentBranch
ja .BackgroundSkip     ; Can replace this to wait with minimum utilization ?

; lea esi,[eax + REGISTRY32.memIpb]  ;  InputParms]       ; ESI = Pointer to Input Parameters Block (IPB)
lea esi,[MEM_IPB]


; lea edi,[eax + REGISTRY32.memOpb]  ;  OutputParms]      ; EDI = Pointer to Output Parameters Block (OPB)
lea edi,[MEM_OPB]

; Update block size at threads control block
mov edx,esi                          ; EDX = Pointer to IPB
mov ecx,[esi + MEMIPB.updatedThreads]   ; ECX = Number of threads
mov eax,[esi + MEMIPB.startBlockSize]   ; EAX = Block size per thread, bytes

;lea ebx,[ThreadsList]                ; EBX = Pointer to threads list
;mov ebx,[Registry]
;lea ebx,[ebx + REGISTRY32.mpe32csb.threadList]
lea ebx,[THREAD_LIST]

call ThreadsUpdateBlockSize

cmp [esi + MEMIPB.updatedMeasure],MEASURE_B_ADAPTIVE
jb .NoAdaptive                       ; Go skip if adaptive mode not used

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov ebx,[Registry]

;lea ebx,[ebx + REGISTRY32.mpe32csb.threadList]                ; EBX = Pointer to threads list
lea ebx,[THREAD_LIST] 

mov edx,esi                          ; EDX = Pointer to IPB 
mov ecx,[esi + MEMIPB.updatedThreads]   ; ECX = Number of threads 
call ThreadsUpdateAdaptive
.NoAdaptive:

; Select Bandwidth or Latency measurement mode
cmp [esi + MEMIPB.updatedAsm],LATENCY_MODE
jae .LatencyMode   ; Go if LATENCY mode, otherwise BANDWIDTH mode 

.BandwidthMode:
; Measurement pattern
;mov ecx,[Registry]
;lea ecx,[ecx + REGISTRY32.mpe32csb.threadList]
lea ecx,[THREAD_LIST]

mov edx,[esi + MEMIPB.updatedThreads]
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread
jmp .DoneBandwidthOrLatency

.LatencyMode:
; Setup pattern for prepare walk list
;mov ebx,[Registry]
;lea ebx,[ebx + REGISTRY32.mpe32csb.threadList]                      ; EBX = Pointer to threads list
lea ebx,[THREAD_LIST]

mov ecx,[esi + MEMIPB.updatedThreads]         ; ECX = Number of threads
mov eax,[esi + MEMIPB.patternRoutine]         ; EAX = Pointer to pattern routine
call ThreadsUpdatePattern
; Run service pattern, prepare walk list, delta TSC ignored here
;mov ecx,[Registry]
;lea ecx,[ecx + REGISTRY32.mpe32csb.threadList]                      ; ECX = Pointer to threads list
lea ecx,[THREAD_LIST]

mov edx,[esi + MEMIPB.updatedThreads]         ; EDX = Number of threads
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread

; Setup pattern for walk with list, prepared on previous step
;mov ebx,[Registry]
;lea ebx,[ebx + REGISTRY32.mpe32csb.threadList]

lea ebx,[THREAD_LIST]

mov ecx,[esi + MEMIPB.updatedThreads]
mov eax,[esi + MEMIPB.walkRoutine]
call ThreadsUpdatePattern
; Run measurement pattern, walk by prepared walk list, get delta TSC in the RAX
;mov ecx,[Registry]
;lea ecx,[ecx + REGISTRY32.mpe32csb.threadList]
lea ecx,[THREAD_LIST]

mov edx,[esi + MEMIPB.updatedThreads]
call ThreadsReStart
jc .BackgroundSkip  ; Go skip operations if error, note this is separate thread

.DoneBandwidthOrLatency:

; Calculte and store CPI = Clocks per Instruction,
; CPI = f ( Delta TSC, Instructions count )

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov ecx,[Registry]  ; TODO. REMOVE REGISTRY.

mov ecx,[DRAW_THREAD_PARMS.measureCounter]
shl ecx,3      ; 2^3 = 8 bytes per store (QWORD)

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov edi,[Registry]  ; TODO. REMOVE REGISTRY.

lea edi,[DRAW_THREAD_PARMS.measureArray + ecx]

push edx eax   ; EDX:EAX = Delta TSC per target operation, integer 64-bit
finit
fild qword [esp]
fild [esi + MEMIPB.measureRepeats]   ; Multiply by total bytes per measurement cycle
fimul [esi + MEMIPB.startBlockSize]  ; Block size, units = bytes 
mov eax,[esi + MEMIPB.operandWidth]  ; EAX = bits per instruction
shr eax,3                         ; EAX = bytes per instruction
mov [esp],eax
fidiv dword [esp]                 ; Divide by BPI, ST0 = Instructions count, ST1 = Delta TSC
fdivp st1,st0     ; ST0 = CPI
fstp qword [esp]
pop eax       ; EDX:EAX = Clocks per Instruction (CPI), floating point, double
stosd         ; CPI, Low DWORD, store this for communication with parent thread
pop eax
stosd         ; CPI, High DWORD

; Update block size for performance = f(size)
mov eax,[esi + MEMIPB.deltaBlockSize]
add [esi + MEMIPB.startBlockSize],eax

; Cycle for measurement draw, increment counter by 1 drawing pixel

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]


inc [DRAW_THREAD_PARMS.measureCounter]

; Support approximation mode, x16 and x32. One pixel already saved, required
; it copy  16-1=15  or  32-1=31  times.
mov eax,[esi + MEMIPB.updatedApprox]
test eax,eax
jz .NoApproxMode       ; go if no approximation mode
mov ecx,15             ; 1 already + 15 duplicated = 16 total stores
cmp eax,APPROX_X16
je .YesApproxMode
mov cl,31              ; 1 already + 31 duplicated = 32 total stores
.YesApproxMode:        ; approximation duplication cycle
mov eax,[edi-8]
stosd                  ; one duplication per loop
mov eax,[edi-8]
stosd                  ; one duplication per loop
mov ebx,[esi + MEMIPB.deltaBlockSize]
add [esi + MEMIPB.startBlockSize],ebx

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

inc [DRAW_THREAD_PARMS.measureCounter]
loop .YesApproxMode    ; approximation duplication cycle
.NoApproxMode:

; Cycle with support termination by break
.BackgroundSkip:

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

cmp [DRAW_THREAD_PARMS.measureBreak],0
je .BackgroundProcess

; Signaling thread STOP
; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

push [DRAW_THREAD_PARMS.eventDone]    ; Parm#1 = Handle for thread done event
call [SetEvent]                            ; Call WinAPI function

; Exit point
pop ebp edi esi ebx
xor eax,eax                                ; EAX = 0, return code = 0
ret 4

; Drawings window invalidation for silent mode: show at drawings end
.SilentBranch:

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov ecx,[Registry]

mov al,0
xchg [GUI_PARMS.silentMode],al
cmp al,0
je .BackgroundSkip
mov eax,[GUI_PARMS.childWinHandle]
test eax,eax
jz .BackgroundSkip
push 0                 ; Parm#3 = Background erase flag, 0=Background unchanged
push 0                 ; Parm#2 = Pointer to invalidated region, 0=All window
push eax               ; Parm#1 = Window handle
call [InvalidateRect]  ; This cause callback WM_PAINT operation
jmp .BackgroundSkip 



