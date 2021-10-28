

; UNDER CONSTRUCTION.
; TODO. Can run as simple run or helper for draw, use input flag.
; TODO. PRE-BLANK RESULTS, OTHERWISE CAN SHOW PREVIOUS RESULTS WHEN ERROR.
; TODO. RUNTIME ERROR REPORTING.


GuiToUserParms:

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r15,[Registry]

;---------- Build User Parameters Block, UPB = F( GUI widgets state ) ---------;
; TODO. Possible make this step as subroutine: GuiToUserParms.

; mov rsi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea rsi,[BIND_LIST]

; lea rdi,[r15 + REGISTRY64.memUpb]
lea rdi,[MEM_UPB]

; TODO. Maks selector for use same code for simple and for draw
; movzx eax,al
; xor eax,eax
movzx eax,al

mov [rdi + MEMUPB.runContext],eax  ; Setup run context for memory allocation multiplication mode control  

mov eax,[rsi + BINDLIST.getMemMethod]  ; TODO. Optimize offsets size by nested structures.
bsf eax,eax
mov [rdi + MEMUPB.optionAsm],eax 

movzx eax,[rsi + BINDLIST.getMemPref]
bsf eax,eax
mov [rdi + MEMUPB.optionDistance],eax 

mov cl,[rsi + BINDLIST.setMemSmp]
mov ch,[rsi + BINDLIST.getMemSmp]
xor eax,eax
test cl,00000001b
jz @f
inc eax
test ch,00000001b
jz @f
inc eax
@@:
mov [rdi + MEMUPB.optionParallel],eax

xor eax,eax
test cl,00000100b
jz @f
inc eax
test ch,00000010b
jz @f 
inc eax
@@:
mov [rdi + MEMUPB.optionHT],eax

xor eax,eax
test cl,00010000b
jz @f
inc eax
test ch,00000100b
jz @f 
inc eax
@@:
mov [rdi + MEMUPB.optionPG],eax

xor eax,eax
test [rsi + BINDLIST.setMemNuma],00000001b
jz @f
movzx ecx,[rsi + BINDLIST.getMemNuma]
bsf ecx,ecx
add eax,ecx
@@:
mov [rdi + MEMUPB.optionNUMA],eax 

movzx eax,[rsi + BINDLIST.getMemObject]
bsf eax,eax
mov [rdi + MEMUPB.optionTarget],eax 

mov cl,[rsi + BINDLIST.setMemLpages]  ; Optimize this, don't use CL, direct test
mov ch,[rsi + BINDLIST.getMemLpages]
xor eax,eax
test cl,00000001b
jz @f
inc eax
test ch,00000001b
jz @f
inc eax
@@:
mov [rdi + MEMUPB.optionLP],eax

movzx eax,[rsi + BINDLIST.getMemMeas]
bsf eax,eax
mov [rdi + MEMUPB.optionMeasure],eax 

movzx eax,[rsi + BINDLIST.getMemPix]
bsf eax,eax
mov [rdi + MEMUPB.optionApprox],eax 

mov rax,[rsi + BINDLIST.blockSizeEdit]
mov [rdi + MEMUPB.customBlockStart],rax

ret




ScenarioMemoryRun:
push rbx rsi rdi rbp r12 r13 r14 r15
cld

;---------- Get system configuration ------------------------------------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r15,[Registry]

call InfoProcessor

;---------- Build User Parameters Block, UPB = F( GUI widgets state ) ---------;
; TODO. Possible make this step as subroutine: GuiToUserParms.

mov al,0
call GuiToUserParms

;---------- Build Input Parameters Block, IPB = F( UPB, System config. ) ------;
; TODO. Comments.
;---------- Initializing measurement ------------------------------------------;
; TODO. Comments.
;---------- Run measurement ---------------------------------------------------;
; TODO. Comments.

; Required modification.
; Resources (memory) must be relesed even if error detected

call SessionStart
jc .error            ; Go if error at start

call SessionProgress
jc .error            ; Go if error at execution

call SessionStop
jc .error            ; Go if error at termination

; call routines from "Run simple" scenario
; call ResultSimple 

;---------- Calculating results by Output Parameters Block (OPB) --------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r15,[Registry]

; mov r14,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea r14,[BIND_LIST]

; lea r12,[r15 + REGISTRY64.memIpb]
lea r12,[MEM_IPB]

lea rdi,[r14 + BINDLIST.mrApplication]
mov ax,STR_MR_APP_TYPE_64
call PoolStringWrite          ; TODO. Make this 3 instructions as subroutine or complex parametrize with 2 offsets
mov al,0
stosb

; TODO. Add strings for nontemporal variants.
; STR_AM_IA32_00
; STR_AM_X64_00
; STR_AM_06

lea rdi,[r14 + BINDLIST.mrMethod]
mov eax,[r12 + MEMIPB.updatedAsm]
add ax,STR_AM_X64_00
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrWidth]
mov eax,[r12 + MEMIPB.operandWidth]
mov bl,0
call DecimalPrint32
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrThreads]
mov eax,[r12 + MEMIPB.updatedThreads]
call DecimalPrint32
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrHyperT]
mov eax,[r12 + MEMIPB.updatedHT]
add ax,STR_MR_HT_00
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrLargeP]
mov eax,[r12 + MEMIPB.updatedLP]
add ax,STR_MR_LP_00
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrNuma]
mov eax,[r12 + MEMIPB.updatedNUMA]
add ax,STR_MR_NUMA_00
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrGroups]
mov eax,[r12 + MEMIPB.updatedPG]
add ax,STR_MR_PG_00
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrTarget]
mov eax,[r12 + MEMIPB.updatedTarget]
add ax,STR_MEMORY_L1
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrDistance]
mov eax,[r12 + MEMIPB.updatedDistance]
add ax,STR_MR_PD_00
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrSizeTotal]
mov rax,[r12 + MEMIPB.startBlockSize]    ; memoryTotal]
mov bl,0FFh
call SizePrint64
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrSizeThread]
mov rax,[r12 + MEMIPB.startBlockSize]    ; memoryPerThread]
call SizePrint64
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrMeasProf]
mov eax,[r12 + MEMIPB.updatedMeasure]
add ax,STR_MR_MEASURE_00
call PoolStringWrite
mov al,0
stosb

; TODO. Overflow possible, repeats can be above 32-bit value, actual for adaptive modes.

lea rdi,[r14 + BINDLIST.mrMeasRep]
mov rax,[r12 + MEMIPB.measureRepeats]
mov bl,0
call DecimalPrint32
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrBlock1]
mov rax,[r12 + MEMIPB.allocatedBlock1]
call HexPrint64
mov ax,0000h + 'h'
stosw

lea rdi,[r14 + BINDLIST.mrBlock2]
mov rax,[r12 + MEMIPB.allocatedBlock2]
call HexPrint64
mov ax,0000h + 'h'
stosw

lea rdi,[r14 + BINDLIST.mrAllocAll]
mov rax,[r12 + MEMIPB.memoryTotal]
mov bl,0FFh
call SizePrint64
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrAllocThr]
mov rax,[r12 + MEMIPB.memoryPerThread]
call SizePrint64
mov al,0
stosb

finit

; lea r12,[r15 + REGISTRY64.memOpb]
lea r12,[MEM_OPB]

lea rdi,[r14 + BINDLIST.mrDtMs]
fild [r12 + MEMOPB.osTimerDelta]  ; st0 = delta time in 100 ns units 
push 10000                        ; 10000 units * 100 ns = 1000000 nS = 1 ms
fidiv dword [rsp]                 ; st0 = delta time in 1 ms units 
fstp qword [rsp]
pop rax                 ; RAX = bitfield as double value
mov bx,0100h            ; Float part is 1 digit, Integer part without template
call DoublePrint        ; print milliseconds
mov ax,STR_MR_PLUS_TIME
call PoolStringWrite
mov al,0
stosb

lea rdi,[r14 + BINDLIST.mrDtscSec]

mov rax,[CPU_DATA.tscClockHz]  ; optimize by x87 access

push rax 1000000
fild qword [rsp + 8]
fidiv dword [rsp + 0]
fstp qword [rsp + 8]
pop rax rax
mov bx,0100h            ; Float part is 1 digit, Integer part without template
call DoublePrint        ; print MHz
mov al,' '
stosb
mov ax,STR_MHZ
call PoolStringWrite
mov al,0
stosb

; Clocks per Instruction (clocks, CPI)

; lea r11,[r15 + REGISTRY64.memIpb]
lea r11,[MEM_IPB]

lea rdi,[r14 + BINDLIST.mrDtscIns]

mov rax,[r11 + MEMIPB.startBlockSize]  ; RAX = Block size, bytes 
xor edx,edx
mov ecx,[r11 + MEMIPB.operandWidth]    ; Must be not zero, otherwise divide overflow
shr ecx,3                        ; RCX = Bytes per instr., convert bits to bytes
div rcx                          ; RCX = Instructions per block
mov rcx,[r11 + MEMIPB.measureRepeats]
mul rcx                          ; RAX = Total instructions
push rax
fild qword [rsp]                   ; st0 = total instructions
fild qword [r12 + MEMOPB.tscTimerDelta]  ; st0 = total delta TSC, st1 = total instructions  
fdiv st0,st1
fstp qword [rsp]
pop rax                 ; RAX = bitfield as double value
mov r10,rax
mov bx,0300h            ; Float part is 3 digits, Integer part without template
call DoublePrint
mov al,0
stosb

; Branch for Bandwidth
; Speed(MBPS)

lea rdi,[r14 + BINDLIST.mrLastValue]

mov rax,[r11 + MEMIPB.startBlockSize]    ; RAX = Operational block size, bytes 
mov rcx,[r11 + MEMIPB.measureRepeats]    ; RCX = Repeats, iterations, note 64-bit     
mul rcx                                  ; RAX = BlockSize * Repeats
mov ecx,[r11 + MEMIPB.updatedThreads]    ; Required all threads same size
mul rcx                                  ; RAX = BlockSize * Repeats * Threads
push rax
fild qword [rsp]            ; st0 = total bytes executed 
mov dword [rsp],1000000     ; alternative is 1048576 (binary/decimal MB select)
fidiv dword [rsp]           ; st0 = total megabytes executed
fild [r12 + MEMOPB.osTimerDelta]
mov dword [rsp],10000000    ; 10000000 units * 100 ns = 1 second
fidiv dword [rsp]
fdivr st0,st1
fstp qword [rsp]
pop rax                 ; RAX = bitfield as double value
mov bx,0200h            ; Float part is 2 digits, Integer part without template
call DoublePrint
mov al,0
stosb



; TODO.
; Branch for Latency 
; Latency (ns)
;
; .L9:
; lea rsi,[MsgResultLatency]
; call StringWrite_CRLF_Equal
; push r10
; fld qword [rsp]
; fmul [r9+OPB.TSCperiodNs]
; fstp qword [rsp]
; pop rax
; mov bx,0300h            ; Float part is 3 digits, Integer part without template
; call DoublePrint
;
; 0 - termination can be common.


 




; TODO. Error handling.

.error:

pop r15 r14 r13 r12 rbp rdi rsi rbx
ret



