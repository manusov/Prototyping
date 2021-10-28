

; UNDER CONSTRUCTION.
; TODO. STRUCTURES ALIGNMENT REQUIRED.
; TODO. PRE-BLANK RESULTS, OTHERWISE CAN SHOW PREVIOUS RESULTS WHEN ERROR.
; TODO. RUNTIME ERROR REPORTING.


VECTOR_BRIEF_DATA_SIZE  EQU  4096     ; Part of L1 ( bytes ) used as data size
REPEATS_SCALE           EQU  10000    ; Measurement repeats scale for performance patterns


ScenarioVectorBrief:
push rbx rsi rdi rbp r12 r13 r14 r15
cld

;---------- Get system configuration ------------------------------------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS, INCLUDE OFFSETS SIZE MINIMIZING.
; mov r15,[Registry]

call InfoProcessor

;---------- Measurements ------------------------------------------------------;

; lea rax,[r15 + REGISTRY64.cpuData]
lea rax,[CPU_DATA]

mov rbx,[rax + CPUDATA.extractedFeaturesBitmap]
mov rdx,[rax + CPUDATA.extractedContextBitmap] 

lea rsi,[vectorBrief]

; lea rdi,[r15 + REGISTRY64.vecbrOpb]
lea rdi,[VECBR_OPB]

mov ecx,VB_REPORT_COUNT

.results:
movzx eax,byte [rsi + 3]  ; cpuValid
bt rbx,rax
jnc .skip

mov al,[rsi + 4]  ; osValid
cmp al,0FFh
je .unconditional
mov ah,al
and al,dl
cmp al,ah
jne .skip

.unconditional:
push rbx rcx rdx rsi rdi

mov al,[rsi + 0]  ; id
test al,al
js .math
and al,07Fh
call GetBandwidthTemporalPattern
jmp .havePattern
.math:
and al,07Fh
call GetMathPattern
.havePattern:

;lea rdi,[r15 + REGISTRY64.mpe64csb.threadList]
lea rdi,[THREAD_LIST]

mov [rdi + THCTRL.entryPoint],rbx    ; RBX = Subroutine address
mov cl,[rsi + 1]  ; count            ; CL = Power of 2 
mov eax,1
shl rax,cl
mov [rdi + THCTRL.sizeInst],rax      ; RAX = Instructions count per 4K

movzx eax,byte [rsi + 2]  ; repeats
imul rax,rax,REPEATS_SCALE           ; RAX = Repeats count
mov [rdi + THCTRL.repeats],rax       ; RAX = Measurement repeats count
lea rax,[TEMP_BUFFER]
mov [rdi + THCTRL.base1],rax
add rax,VECTOR_BRIEF_DATA_SIZE / 2
mov [rdi + THCTRL.base2],rax
lea r8,[MathData]                ; FIX THIS, USE Base3 ?
mov rbx,rdi                      ; Note can remove this if Get...Pattern return in RAX 
call RunTarget                   ; Return RAX = dTSC per pattern
pop rdi rsi rdx rcx rbx          ; restore after performance patterns run
jmp .next

.skip:
xor eax,eax
.next:
stosq
add rsi,5
dec ecx        ; DEC/JNZ timings better than LOOP 
jnz .results

;---------- Results calculation and text strings write ------------------------; 

; lea rax,[r15 + REGISTRY64.cpuData]  ; TODO. Use PUSH-POP instead reload
lea rax,[CPU_DATA]

mov rbx,[rax + CPUDATA.extractedFeaturesBitmap]
mov rdx,[rax + CPUDATA.extractedContextBitmap] 

finit
push 1000000000
mov rbp,rsp
fild qword [rbp]
fild qword [rax + CPUDATA.tscClockHz]
fdivp st1,st0
fstp qword [rbp]

lea rsi,[vectorBrief]

; lea r8,[r15 + REGISTRY64.vecbrOpb]  ; TODO. Use PUSH-POP instead reload
lea r8,[VECBR_OPB]


; mov r14,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea r14,[BIND_LIST]

add r14,BINDLIST.vbSse128read
mov r13d,VB_REPORT_COUNT

.strings:
push rsi rbx rdx  ; TODO. Optimize registers usage
mov rdi,r14

movzx eax,byte [rsi + 3]  ; cpuValid
bt rbx,rax
mov ax,STR_NOT_SUPPORTED_CPU
jnc .nocalc

mov al,[rsi + 4]          ; osValid
cmp al,0FFh
je .calc
mov ah,al
and al,dl
cmp al,ah
mov ax,STR_NOT_SUPPORTED_OS
jne .nocalc

; This point if result valid, pattern supported and executed
.calc:

mov cl,[rsi + 1]               ; Instructions count, 32-bit
mov eax,1
shl rax,cl
movzx edx,byte [rsi + 2]       ; Measurement count, 32-bit
imul rdx,rdx,REPEATS_SCALE
mul rdx                        ; RAX = 64-bit product
push rax
push qword [r8]         ; TODO. can access [r8] by x87, without PUSH.
fild qword [rsp + 8]    ; st0 = instructions count
fild qword [rsp]        ; st0 = clocks count , st1 = instructions count
fdiv st0,st1            ; st0 = clocks per instruction (CPI)
fst qword [rsp]                      ; store CPI

; fmul [rbx+VECTORBRIEF.TscClockNsF]   ; st0 = nanoseconds per instruction (nsPI)
fmul qword [rbp]                       ; st0 = nanoseconds per instruction (nsPI)

fstp qword [rsp + 8]                  ; store nsPI
ffree st0
fincstp
pop rax
mov bx,0300h
call DoublePrint   ; print CPI

mov ax,' ('
stosw
pop rax
call DoublePrint   ; print nsPI, " ( x ns ) "
mov al,' '
stosb
mov ax,STR_NS
call PoolStringWrite
mov al,')'
stosb
jmp .doneString

.nocalc:
call PoolStringWrite

.doneString:
pop rdx rbx rsi
mov al,0
stosb
add rsi,5
add r8,8
add r14,VB_REPORT_STEP
dec r13d
jnz .strings

pop rax ; remove local variable

pop r15 r14 r13 r12 rbp rdi rsi rbx
ret


