

; UNDER CONSTRUCTION.
; TODO. STRUCTURES ALIGNMENT REQUIRED.
; TODO. PRE-BLANK RESULTS, OTHERWISE CAN SHOW PREVIOUS RESULTS WHEN ERROR.
; TODO. RUNTIME ERROR REPORTING.



VECTOR_BRIEF_DATA_SIZE  EQU  4096     ; Part of L1 ( bytes ) used as data size
REPEATS_SCALE           EQU  10000    ; Measurement repeats scale for performance patterns


ScenarioVectorBrief:
pushad
cld

;---------- Get system configuration ------------------------------------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov esi,[Registry]

call InfoProcessor

;---------- Measurements ------------------------------------------------------;

; TODO. OPTIMIZE BASE-INDEX ACCESS INCLUDE MINIMIZE OFFSETS SIZE.
; mov ebx,[Registry]  ; TODO. Optimize by PUSH-POP

lea esi,[vectorBrief]

;lea edi,[ebx + REGISTRY32.vecbrOpb]
lea edi,[VECBR_OPB]

mov ecx,VB_REPORT_COUNT

.results:
movzx eax,byte [esi + 3]  ; cpuValid
mov edx,dword [CPU_DATA.extractedFeaturesBitmap]
bt edx,eax
jnc .skip

mov al,[esi + 4]  ; osValid
cmp al,0FFh
je .unconditional
mov ah,al
and al,byte [CPU_DATA.extractedContextBitmap] 
cmp al,ah
jne .skip

.unconditional:
push ebx ecx esi edi

push ebx
mov al,[esi + 0]  ; id
test al,al
js .math
and al,07Fh
call GetBandwidthTemporalPattern
jmp .havePattern
.math:
and al,07Fh
call GetMathPattern
.havePattern:
xchg eax,ebx          ; TODO. simplify this
pop ebx               ; TODO. reorder instead extra PUSH-POP

; lea edi,[ebx + REGISTRY32.mpe32csb.threadList]
lea edi,[THREAD_LIST]

mov [edi + THCTRL.entryPoint],eax    ; EAX = Subroutine address
mov cl,[esi + 1]  ; count            ; CL = Power of 2 
mov eax,1
shl eax,cl
mov [edi + THCTRL.sizeInst],eax      ; EAX = Instructions count per 4K

movzx eax,byte [esi + 2]  ; repeats
cdq
mov ecx,REPEATS_SCALE
mul ecx                   ; EDX:EAX = Repeats count
mov dword [edi + THCTRL.repeats + 0],eax
mov dword [edi + THCTRL.repeats + 4],edx
lea eax,[TEMP_BUFFER]
mov [edi + THCTRL.base1],eax
add eax,VECTOR_BRIEF_DATA_SIZE / 2
mov [edi + THCTRL.base2],eax

; If this run for math. pattern, base1 used for load numeric constants vector,
; not for block 1 base.
test byte [esi+0],80h
jz @f
mov [edi + THCTRL.base1],MathData
@@:

mov ebx,edi                  ; Note can remove this if Get...Pattern return in RAX 
call RunTarget               ; Return EDX:EAX = dTSC per pattern
pop edi esi ecx ebx          ; restore after performance patterns run
jmp .next

.skip:
xor eax,eax
cdq
.next:
stosd
xchg eax,edx
stosd
add esi,5
dec ecx        ; DEC/JNZ timings better than LOOP 
jnz .results

;---------- Results calculation and text strings write ------------------------; 

finit
push 0 1000000000
mov ebp,esp
fild qword [ebp]
fild qword [CPU_DATA.tscClockHz]
fdivp st1,st0
fstp qword [ebp]

lea esi,[vectorBrief]

;lea edx,[ebx + REGISTRY32.vecbrOpb]  ; TODO. Use PUSH-POP instead reload
lea edx,[VECBR_OPB]

; mov edi,[ebx + REGISTRY32.allocatorBindBuffer.objectStart]
lea edi,[BIND_LIST]

add edi,BINDLIST.vbSse128read
mov ecx,VB_REPORT_COUNT

.strings:
push ebx ecx edx esi edi

movzx eax,byte [esi + 3]  ; cpuValid
mov ecx,dword [CPU_DATA.extractedFeaturesBitmap]  ; TODO. non optimal
bt ecx,eax
mov ax,STR_NOT_SUPPORTED_CPU
jnc .nocalc

mov al,[esi + 4]          ; osValid
mov ecx,dword [CPU_DATA.extractedContextBitmap]  ; TODO. non optimal
cmp al,0FFh
je .calc
mov ah,al
and al,cl
cmp al,ah
mov ax,STR_NOT_SUPPORTED_OS
jne .nocalc

; This point if result valid, pattern supported and executed
.calc:
mov cl,[esi + 1]               ; Instructions count, 32-bit
mov eax,1
shl eax,cl
mov ecx,edx
movzx edx,byte [esi + 2]       ; Measurement count, 32-bit
imul edx,edx,REPEATS_SCALE
mul edx                        ; EDX:EAX = 64-bit product
push edx eax
push dword [ecx + 4]     ; TODO. can access [ecx] by x87, without PUSH.
push dword [ecx + 0]
fild qword [esp + 8]    ; st0 = instructions count
fild qword [esp]        ; st0 = clocks count , st1 = instructions count
fdiv st0,st1            ; st0 = clocks per instruction (CPI)
fst qword [esp]                      ; store CPI
; fmul [rbx+VECTORBRIEF.TscClockNsF]   ; st0 = nanoseconds per instruction (nsPI)
fmul qword [ebp]                       ; st0 = nanoseconds per instruction (nsPI)
fstp qword [esp + 8]                  ; store nsPI
ffree st0
fincstp
pop eax edx
mov bx,0300h
call DoublePrint   ; print CPI

mov ax,' ('
stosw
pop eax edx
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
mov al,0
stosb
pop edi esi edx ecx ebx 
add esi,5
add edx,8
add edi,VB_REPORT_STEP
dec ecx
jnz .strings

pop eax eax ; remove local variable

popad
ret




