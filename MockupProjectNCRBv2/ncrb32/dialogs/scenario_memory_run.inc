

; UNDER CONSTRUCTION.
; TODO. Can run as simple run or helper for draw, use input flag.
; TODO. PRE-BLANK RESULTS, OTHERWISE CAN SHOW PREVIOUS RESULTS WHEN ERROR.
; TODO. RUNTIME ERROR REPORTING.

GuiToUserParms:

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]

;---------- Build User Parameters Block, UPB = F( GUI widgets state ) ---------;
; TODO. Possible make this step as subroutine: GuiToUserParms.

; lea edi,[esi + REGISTRY32.memUpb]
lea edi,[MEM_UPB]


; mov esi,[esi + REGISTRY32.allocatorBindBuffer.objectStart]
lea esi,[BIND_LIST]

; TODO. Maks selector for use same code for simple and for draw
; movzx eax,al
; xor eax,eax
movzx eax,al

mov [edi + MEMUPB.runContext],eax  ; Setup run context for memory allocation multiplication mode control  

mov eax,[esi + BINDLIST.getMemMethod]  ; TODO. Optimize offsets size by nested structures.
bsf eax,eax
mov [edi + MEMUPB.optionAsm],eax 

movzx eax,[esi + BINDLIST.getMemPref]
bsf eax,eax
mov [edi + MEMUPB.optionDistance],eax 

mov cl,[esi + BINDLIST.setMemSmp]
mov ch,[esi + BINDLIST.getMemSmp]
xor eax,eax
test cl,00000001b
jz @f
inc eax
test ch,00000001b
jz @f
inc eax
@@:
mov [edi + MEMUPB.optionParallel],eax

xor eax,eax
test cl,00000100b
jz @f
inc eax
test ch,00000010b
jz @f 
inc eax
@@:
mov [edi + MEMUPB.optionHT],eax

xor eax,eax
test cl,00010000b
jz @f
inc eax
test ch,00000100b
jz @f 
inc eax
@@:
mov [edi + MEMUPB.optionPG],eax

xor eax,eax
test [esi + BINDLIST.setMemNuma],00000001b
jz @f
movzx ecx,[esi + BINDLIST.getMemNuma]
bsf ecx,ecx
add eax,ecx
@@:
mov [edi + MEMUPB.optionNUMA],eax 

movzx eax,[esi + BINDLIST.getMemObject]
bsf eax,eax
mov [edi + MEMUPB.optionTarget],eax 

mov cl,[esi + BINDLIST.setMemLpages]  ; Optimize this, don't use CL, direct test
mov ch,[esi + BINDLIST.getMemLpages]
xor eax,eax
test cl,00000001b
jz @f
inc eax
test ch,00000001b
jz @f
inc eax
@@:
mov [edi + MEMUPB.optionLP],eax

movzx eax,[esi + BINDLIST.getMemMeas]
bsf eax,eax
mov [edi + MEMUPB.optionMeasure],eax 

movzx eax,[esi + BINDLIST.getMemPix]
bsf eax,eax
mov [edi + MEMUPB.optionApprox],eax 

mov eax,dword [esi + BINDLIST.blockSizeEdit]
mov [edi + MEMUPB.customBlockStart],eax

ret



ScenarioMemoryRun:
pushad
cld

;---------- Get system configuration ------------------------------------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]

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

;******************************************************************************;
;
; TODO. TIMEOUT ERROR AT SECOND SUBROUTINE 
; IF TRACE THIS 3 SUBROUTINES BY OLLYDBG.
; SAME AT NCRB V1.XX.XX.
;
; INT3
;
;******************************************************************************;

call SessionStart
jc .error            ; Go if error at start

; DEBUG
; PUSH ECX
; MOV ECX,00000000H
; LOOP $
; POP ECX
; DEBUG

call SessionProgress
jc .error            ; Go if error at execution

call SessionStop
jc .error            ; Go if error at termination

; call routines from "Run simple" scenario
; call ResultSimple 

;---------- Calculating results by Output Parameters Block (OPB) --------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

; mov ebp,[eax + REGISTRY32.allocatorBindBuffer.objectStart]
lea ebp,[BIND_LIST]

; lea ebx,[eax + REGISTRY32.memIpb]
lea ebx,[MEM_IPB]

; TODO. Differentiate ia32 and ia32 under Win64 (WoW)

lea edi,[ebp + BINDLIST.mrApplication]
mov ax,STR_MR_APP_TYPE_32
call PoolStringWrite          ; TODO. Make this 3 instructions as subroutine or complex parametrize with 2 offsets
mov al,0
stosb

; TODO. Add strings for nontemporal variants.
; STR_AM_IA32_00
; STR_AM_X64_00
; STR_AM_06

lea edi,[ebp + BINDLIST.mrMethod]
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

lea edi,[ebp + BINDLIST.mrWidth]
mov eax,[ebx + MEMIPB.operandWidth]
push ebx
mov bl,0
call DecimalPrint32
pop ebx
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrThreads]
mov eax,[ebx + MEMIPB.updatedThreads]
push ebx
mov bl,0
call DecimalPrint32
pop ebx
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrHyperT]
mov eax,[ebx + MEMIPB.updatedHT]
add ax,STR_MR_HT_00
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrLargeP]
mov eax,[ebx + MEMIPB.updatedLP]
add ax,STR_MR_LP_00
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrNuma]
mov eax,[ebx + MEMIPB.updatedNUMA]
add ax,STR_MR_NUMA_00
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrGroups]
mov eax,[ebx + MEMIPB.updatedPG]
add ax,STR_MR_PG_00
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrTarget]
mov eax,[ebx + MEMIPB.updatedTarget]
add ax,STR_MEMORY_L1
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrDistance]
mov eax,[ebx + MEMIPB.updatedDistance]
add ax,STR_MR_PD_00
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrSizeTotal]
mov eax,[ebx + MEMIPB.startBlockSize]    ; memoryTotal]
xor edx,edx
push ebx
mov bl,0FFh
call SizePrint64
pop ebx
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrSizeThread]
mov eax,[ebx + MEMIPB.startBlockSize]    ; memoryPerThread]
xor edx,edx
push ebx
mov bl,0FFh
call SizePrint64
pop ebx
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrMeasProf]
mov eax,[ebx + MEMIPB.updatedMeasure]
add ax,STR_MR_MEASURE_00
call PoolStringWrite
mov al,0
stosb

; TODO. Overflow possible, repeats can be above 32-bit value, actual for adaptive modes.

lea edi,[ebp + BINDLIST.mrMeasRep]
mov eax,dword [ebx + MEMIPB.measureRepeats]
push ebx
mov bl,0
call DecimalPrint32
pop ebx
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrBlock1]
mov eax,[ebx + MEMIPB.allocatedBlock1]
call HexPrint32
mov ax,0000h + 'h'
stosw

lea edi,[ebp + BINDLIST.mrBlock2]
mov eax,[ebx + MEMIPB.allocatedBlock2]
call HexPrint32
mov ax,0000h + 'h'
stosw

lea edi,[ebp + BINDLIST.mrAllocAll]
mov eax,[ebx + MEMIPB.memoryTotal]
push ebx
mov bl,0FFh
call SizePrint64
pop ebx
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrAllocThr]
mov eax,[ebx + MEMIPB.memoryPerThread]
push ebx
mov bl,0FFh
call SizePrint64
pop ebx
mov al,0
stosb

finit

; mov ebx,[Registry]
; lea ebx,[ebx + REGISTRY32.memOpb]
lea ebx,[MEM_OPB]

lea edi,[ebp + BINDLIST.mrDtMs]
fild [ebx + MEMOPB.osTimerDelta]  ; st0 = delta time in 100 ns units 
push 0 10000                      ; 10000 units * 100 ns = 1000000 nS = 1 ms
fidiv dword [esp]                 ; st0 = delta time in 1 ms units 
fstp qword [esp]
pop eax edx              ; EDX:EAX = bitfield as double value
push ebx
mov bx,0100h            ; Float part is 1 digit, Integer part without template
call DoublePrint        ; print milliseconds
pop ebx
mov ax,STR_MR_PLUS_TIME
call PoolStringWrite
mov al,0
stosb

lea edi,[ebp + BINDLIST.mrDtscSec]

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov eax,[Registry]

push dword [CPU_DATA.tscClockHz + 4]  ; optimize by x87 access
push dword [CPU_DATA.tscClockHz + 0]  ; optimize by x87 access
push 1000000
fild qword [esp + 4]
fidiv dword [esp + 0]
fstp qword [esp + 4]
pop eax eax edx
push ebx
mov bx,0100h            ; Float part is 1 digit, Integer part without template
call DoublePrint        ; print MHz
pop ebx
mov al,' '
stosb
mov ax,STR_MHZ
call PoolStringWrite
mov al,0
stosb

; Clocks per Instruction (clocks, CPI)

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]

; lea esi,[esi + REGISTRY32.memIpb]
lea esi,[MEM_IPB]

lea edi,[ebp + BINDLIST.mrDtscIns]

mov eax,[esi + MEMIPB.startBlockSize]  ; EAX = Block size, bytes 
xor edx,edx
mov ecx,[esi + MEMIPB.operandWidth]    ; Must be not zero, otherwise divide overflow
shr ecx,3                        ; RCX = Bytes per instr., convert bits to bytes
div ecx                          ; RCX = Instructions per block
mov ecx,dword [esi + MEMIPB.measureRepeats]
mul ecx                          ; RAX = Total instructions
push edx eax
fild qword [esp]                   ; st0 = total instructions
fild qword [ebx + MEMOPB.tscTimerDelta]  ; st0 = total delta TSC, st1 = total instructions  
fdiv st0,st1
fstp qword [esp]
pop eax edx                 ; EDX:EAX = bitfield as double value

;mov r10,rax

push ebx
mov bx,0300h            ; Float part is 3 digits, Integer part without template
call DoublePrint
pop ebx
mov al,0
stosb

; Branch for Bandwidth
; Speed(MBPS)

lea edi,[ebp + BINDLIST.mrLastValue]

push edx eax
fild [esi + MEMIPB.measureRepeats]   ; st0 = Repeats, iterations, note 64-bit
fimul [esi + MEMIPB.startBlockSize]  ; st0 * Operational block size, bytes 
fimul [esi + MEMIPB.updatedThreads]   ; Required all threads same size
; At this point, st0 = BlockSize * Repeats * Threads
mov dword [esp],1000000     ; alternative is 1048576 (binary/decimal MB select)
fidiv dword [esp]           ; st0 = total megabytes executed
fild [ebx + MEMOPB.osTimerDelta]
mov dword [esp],10000000    ; 10000000 units * 100 ns = 1 second
fidiv dword [esp]
fdivr st0,st1
fstp qword [esp]
pop eax edx             ; EDX:EAX = bitfield as double value
mov bx,0200h            ; Float part is 2 digits, Integer part without template
call DoublePrint

mov al,0
stosb

; jmp .L10


; TODO.
; Branch for Latency 
; Latency (ns)
;.L9:
;;lea esi,[MsgResultLatency]
;call StringWrite_CRLF_Equal
;push edx eax
;fld qword [esp]
;fmul [OutputParms + OPB.TSCperiodNs]
;fstp qword [esp]
;pop eax edx
;mov bx,0300h            ; Float part is 3 digits, Integer part without template
;call DoublePrint




; 0 - termination can be common.





; TODO. Error handling.

.error:


popad
ret

