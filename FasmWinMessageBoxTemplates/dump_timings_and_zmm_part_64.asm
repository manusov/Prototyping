;==============================================================================;
;                                                                              ;
;                               Debug sample:                                  ; 
;             Dump x86-64 general-purpose registers, x87 registers,            ; 
;             ZMM[0-15] registers, but without ZMM[16-31] registers            ;
;      measure CPU clock frequency and timings for fragment under debug.       ; 
;                              Win64 edition.                                  ; 
;                                                                              ;
;        Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 )        ;
;                         http://flatassembler.net/                            ;
;                                                                              ;
;       Edit by FASM Editor 2.0, use this editor for correct tabulations.      ;
;              https://fasmworld.ru/instrumenty/fasm-editor-2-0/               ;
;                                                                              ;
;==============================================================================;

;------------------------------------------------------------------------------;
;                                Definitions.                                  ;
;------------------------------------------------------------------------------;

WORK_CYCLES  EQU  100000
include 'win64a.inc'

;------------------------------------------------------------------------------;
;                                Code section.                                 ;
;------------------------------------------------------------------------------;

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
; Create and align stack frame for WinAPI parameters shadow
sub rsp,8*5
; Check CPUID support, note can be supported but (for example) disabled by VMM
lea rdi,[_CpuidBuffer]
call CheckCpuId
jc .errorCpuid           ; Go error branch if CPUID not supported
; Check CPUID features: TSC, SSE
cmp eax,1
jb .errorTsc             ; Go error branch if CPUID function 1 not supported
mov eax,1
cpuid
test dl,01h
jz .errorX87             ; Go error branch if x87 FPU not supported
test dl,10h
jz .errorTsc             ; Go error branch if TSC not supported
bt edx,25
jnc .errorSse            ; Go error branch if SSE not supported
bt edx,26
jnc .errorSse2           ; Go error branch if SSE2 (double precision) not sup.
; CPUID additions for AVX
bt ecx,28
jnc .errorAvx
bt ecx,27
jnc .errorOsxsave
; CPUID additions for AVX512
xor eax,eax
cpuid
cmp eax,7
jb .errorAvx512
mov eax,7
xor ecx,ecx
cpuid
bt ebx,16                ; Check AVX512F (Foundation)
jnc .errorAvx512
; XCR0 additions for AVX512
xor ecx,ecx
xgetbv
and al,11100110b
cmp al,11100110b
jne .errorContextAvx
; Measure CPU clock frequency, store frequency and period
call MeasureTsc
jc .errorRdtsc
; Get OS timer value at start of measured interval
lea	rcx,[_FileTime1]             ; RCX = Parm#1 = Pointer to QWORD variable 
call	[GetSystemTimeAsFileTime]  ; Update qword variable = time in 100 ns units
; Get TSC value at start of measured interval
lea rbx,[_TscTime1]
rdtsc
mov [rbx + 0],eax
mov [rbx + 4],edx

;------------------------------------------------------------------------------;
;                         Start of code under debug.                           ;
;              Place here code for debug instead of this fragment.             ;
;              Can be measurement cycle and set registers values.              ;  
;------------------------------------------------------------------------------;

mov rcx,WORK_CYCLES
mov [_DefinedCycles],rcx
lea rbx,[_DataBuffer]
vxorpd zmm0,zmm0,zmm0

@@:
vmovapd [rbx + 64*00],zmm0
vmovapd [rbx + 64*01],zmm0
vmovapd [rbx + 64*02],zmm0
vmovapd [rbx + 64*03],zmm0
vmovapd [rbx + 64*04],zmm0
vmovapd [rbx + 64*05],zmm0
dec rcx
jnz @b

fldpi
fld1
fst qword [rbx + 8*0]
fstp qword [rbx + 8*3]
fst qword [rbx + 8*1]
fstp qword [rbx + 8*2]
vmovapd ymm0,[rbx + 00]
vmovapd [rbx + 32],ymm0
vmovapd zmm0,[rbx + 00]

mov rax,rsp
mov rbx,2
mov rcx,3
mov rdx,4
mov rbp,01111111111111111h
mov rsi,05555555555555555h
mov rdi,0AAAAAAAAAAAAAAAAh
mov r8,0123456789ABCDEFh
mov r9,0FEDCBA9876543210h
mov r10,100h
mov r11,101h
mov r12,102h
mov r13,103h
mov r14,104h
mov r15,105h 

;------------------------------------------------------------------------------;
;                           End of code under debug.                           ;
;------------------------------------------------------------------------------;

; Write general purpose registers to save state buffer
mov [_GprBuffer],rax
lea rax,[_GprBuffer]
mov [rax + 8*01],rbx
mov [rax + 8*02],rcx
mov [rax + 8*03],rdx
mov [rax + 8*04],rsp
mov [rax + 8*05],rbp
mov [rax + 8*06],rsi
mov [rax + 8*07],rdi
mov [rax + 8*08],r8
mov [rax + 8*09],r9
mov [rax + 8*10],r10
mov [rax + 8*11],r11
mov [rax + 8*12],r12
mov [rax + 8*13],r13
mov [rax + 8*14],r14
mov [rax + 8*15],r15
; Write FPU registers to save state buffer
lea rax,[_FpuBuffer]
fstp qword [rax + 8*00]
fstp qword [rax + 8*01]
fstp qword [rax + 8*02]
fstp qword [rax + 8*03]
fstp qword [rax + 8*04]
fstp qword [rax + 8*05]
fstp qword [rax + 8*06]
fstp qword [rax + 8*07]
; Write AVX registers to save state buffer  
lea rax,[_Avx512Buffer]
vmovupd [rax + 64*00],zmm0
vmovupd [rax + 64*01],zmm1
vmovupd [rax + 64*02],zmm2
vmovupd [rax + 64*03],zmm3
vmovupd [rax + 64*04],zmm4
vmovupd [rax + 64*05],zmm5
vmovupd [rax + 64*06],zmm6
vmovupd [rax + 64*07],zmm7
vmovupd [rax + 64*08],zmm8
vmovupd [rax + 64*09],zmm9
vmovupd [rax + 64*10],zmm10
vmovupd [rax + 64*11],zmm11
vmovupd [rax + 64*12],zmm12
vmovupd [rax + 64*13],zmm13
vmovupd [rax + 64*14],zmm14
vmovupd [rax + 64*15],zmm15
; Get TSC value at end of measured interval, calculate delta, units=TSC clocks
lea rbx,[_TscTime1]
rdtsc
mov [rbx + 8 + 0],eax
mov [rbx + 8 + 4],edx
sub eax,[rbx + 0]
sbb edx,[rbx + 4]
mov [rbx + 16 + 0],eax           ; Store delta TSC, low 32 bits
mov [rbx + 16 + 4],edx           ; Store delta TSC, high 32 bits
; Get OS timer value at end of measured interval, calculate delta, units=100 ns   
lea	rbx,[_FileTime1]
lea	rcx,[rbx + 8]                ; RCX = Parm#1 = Pointer to QWORD variable 
call	[GetSystemTimeAsFileTime]  ; Update qword variable = time in 100 ns units
mov rax,[rbx + 8]
sub rax,[rbx + 0]
mov [rbx + 16],rax               ; Store delta OS time
; Calculate values = f ( measurement results )
; [rbx + 00] = Number of measurement iterations
; [rbx + 08] = _DeltaTSC = Delta TSC per all measurement, milliseconds
; ]rbx + 16] = _DeltaFT = Delta File Time per all measurement, milliseconds  
; [rbx + 24] = _DeltaPass = Delta TSC per one measurement iteratin, clocks
lea rbx,[_DefinedCycles]
finit
; Target interval time, measured by TSC
fild qword [_TscTimeDelta]  ; ST0 = Number of TSC clocks per target interval
fld st0
fmul [_RdtscBuffer + 24]    ; ST0 = Target interval time, seconds
fmul [_value1E3]            ; ST0 = Target interval time, milliseconds
fstp qword [rbx + 08]       ; Store target interval, measured by TSC, ms 
; Target interval time, measured by WinAPI timer
fild qword [_FileTimeDelta]  ; ST0 = Number 100 ns units per target interval
fmul [_value1EM4]            ; ST0 = Target interval time, milliseconds
fstp qword [rbx + 16]        ; Store target interval, measured by WinAPI
; Delta TSC per one measurement iteratin, clocks
fild qword [rbx]             ; ST0 = Work iterations , ST1 = Delta TSC
fdivp st1,st0                ; ST0 = Delta TSC clocks / Work iterations  
fstp qword [rbx + 24]        ; Store one iteration time, TSC clocks
; Start build text blocks, build text block for general purpose registers
cld
lea rsi,[_Message]     ; RSI = Pointer to source text strings
lea rdi,[_TextBuffer]  ; RDI = Pointer to destination buffer for build text 
lea rbx,[_GprBuffer]   ; RBX = Pointer to source binary data, registers
mov ecx,16             ; ECX = Number of general purpose registers
.dumpGPR:
movsw
movsb
mov eax,' =  '
stosd
mov rax,[rbx]
add rbx,8
call HexPrint64
mov ax,0D0Ah
stosw
loop .dumpGPR          ; Cycle for 16 registers
stosw                  ; Interval between GPR and FPU registers
; Build text block for FPU registers
mov ecx,8              ; ECX = Number of FPU registers
.dumpFPU:
mov ax,[rsi]
stosw                  ; Write "ST"
push rbx
mov eax,8
sub eax,ecx
mov bl,0
call DecimalPrint32    ; Write digit 0-7 for ST0-ST7
pop rbx
mov eax,' =  '
stosd
mov rax,[rbx]
add rbx,8
push rbx
mov bx,0800h
call DoublePrint
pop rbx
mov ax,0D0Ah
stosw
loop .dumpFPU          ; Cycle for 8 registers
stosw                  ; Interval between FPU and AVX registers
add rsi,2              ; Skip "ST" string
; Build text block for AVX registers
mov ecx,16             ; ECX = Number of AVX512 registers ( only 16 here used )
.dumpAVX512:
mov ax,[rsi]
stosw                  ; Write "ZM"
mov al,[rsi+2]
stosb                  ; Write "M"
push rbx rdi
mov eax,16
sub eax,ecx
mov bl,0
mov word [rdi],'  '
call DecimalPrint32    ; Write digit 0-7 for ST0-ST7
pop rdi rbx
add rdi,2
mov eax,' =  '
stosd
mov ebp,8              ; EBP = Number of double precision values per one ZMM 
.dumpZMM:
mov rax,[rbx]
add rbx,8
push rbx
mov bx,0300h
call DoublePrint
pop rbx
mov ax,'  '
stosw
dec ebp
jnz .dumpZMM 
mov ax,0D0Ah
stosw
loop .dumpAVX512       ; Cycle for 16 registers
add rsi,3              ; Skip "ST" string
; Build text block for TSC frequency
lea rsi,[_TSC]
call StringWrite
mov rax,[_RdtscBuffer + 16]
mov bx,0200h
call DoublePrint
lea rsi,[_MHz]
call StringWrite
; Build text block for delta time by TSC
lea rsi,[_dTSC]
call StringWrite
mov rax,[_TscTimeDelta]
call HexPrint64
lea rsi,[_h]
call StringWrite
mov rax,[_DeltaTSC]
mov bx,0200h
call DoublePrint
lea rsi,[_ms]
call StringWrite
; Build text block for delta time by OS timer ( delta File Time )
lea rsi,[_dFT]
call StringWrite
mov rax,[_FileTimeDelta]
call HexPrint64
lea rsi,[_h]
call StringWrite
mov rax,[_DeltaFT]
mov bx,0200h
call DoublePrint
lea rsi,[_ms]
call StringWrite
; Build text block for number of iterations
lea rsi,[_Iterations]
call StringWrite
mov rax,[_DefinedCycles]
mov rcx,100000000h
cmp rax,rcx
ja .itHex
mov bl,0
call DecimalPrint32
jmp .itDone
.itHex:
call HexPrint64
lea rsi,[_h1]
call StringWrite
.itDone:
; Build text block for one iteration time: dTSC/Pass
lea rsi,[_dTSCPass]
call StringWrite
mov rax,[_DeltaPass]
mov bx,0300h
call DoublePrint
; Write terminator byte
mov al,0
stosb

; Show GUI box with text block, for normal termination without errors
xor ecx,ecx                 ; RCX = Parm #1 = Parent window
lea rdx,[_TextBuffer]       ; RDX = Parm #2 = Message
lea r8,[_Caption]           ; R8  = Parm #3 = Caption (upper message)
xor r9,r9                   ; R9  = Parm #4 = Message flags
call [MessageBoxA]          ; Call target function - show window
; Exit from application, for normal termination without errors
xor ecx,ecx                 ; RCX = Parm#1 = exit code
call [ExitProcess]

; Errors handling
.errorCpuid:
lea rdx,[_NoCpuid]          ; RDX = Parm #2 = Message
jmp @f
.errorX87:
lea rdx,[_NoX87]            ; RDX = Parm #2 = Message
jmp @f
.errorTsc:
lea rdx,[_NoTsc]            ; RDX = Parm #2 = Message
jmp @f
.errorSse:
lea rdx,[_NoSse]            ; RDX = Parm #2 = Message
jmp @f
.errorSse2:
lea rdx,[_NoSse2]           ; RDX = Parm #2 = Message
jmp @f
.errorAvx:
lea rdx,[_NoAvx]            ; RDX = Parm #2 = Message
jmp @f
.errorAvx512:
lea rdx,[_NoAvx512]         ; RDX = Parm #2 = Message
jmp @f
.errorOsxsave:
lea rdx,[_NoOsxsave]        ; RDX = Parm #2 = Message
jmp @f
.errorContextAvx:
lea rdx,[_NoAvxContext]     ; RDX = Parm #2 = Message
jmp @f
.errorRdtsc:
lea rdx,[_ErrorTsc]         ; RDX = Parm #2 = Message
@@:
xor ecx,ecx                 ; RCX = Parm #1 = Parent window
lea r8,[_Caption]           ; R8  = Parm #3 = Caption (upper message)
mov r9d,MB_ICONERROR        ; R9  = Parm #4 = Message flags
call [MessageBoxA]          ; Call target function - show window
mov ecx,1                   ; RCX = Parm#1 = exit code
call [ExitProcess]

;---------- Copy text string terminated by 00h ----------------;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   RSI = Source address                                ;
;          RDI = Destination address                           ;
; OUTPUT:  RSI = Modified by copy                              ;
;          RDI = Modified by copy                              ;
;          Memory at [Input RDI] modified                      ; 
;--------------------------------------------------------------;
StringWrite:
cld
@@:
lodsb
cmp al,0
je @f
stosb
jmp @b
@@:
ret
;---------- Print 64-bit Hex Number ---------------------------;
; INPUT:  RAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint64:
push rax
ror rax,32
call HexPrint32
pop rax
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------------------------;
; INPUT:  EAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint32:
push rax
ror eax,16
call HexPrint16
pop rax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------------------------;
; INPUT:  AX  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint16:
push rax
xchg al,ah
call HexPrint8
pop rax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------------------------;
; INPUT:  AL  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint8:
push rax
ror al,4
call HexPrint4
pop rax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------------------------;
; INPUT:  AL  = Number (bits 0-3)                              ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint4:
cld
push rax
and al,0Fh
cmp al,9
ja .modify
add al,'0'
jmp .store
.modify:
add al,'A'-10
.store:
stosb
pop rax
ret
;---------- Print 32-bit Decimal Number -----------------------;
; INPUT:   EAX = Number value                                  ;
;          BL  = Template size, chars. 0=No template           ;
;          RDI = Destination Pointer (flat)                    ;
; OUTPUT:  RDI = New Destination Pointer (flat)                ;
;                modified because string write                 ;
;--------------------------------------------------------------;
DecimalPrint32:
cld
push rax rbx rcx rdx
mov bh,80h-10         ; Bit BH.7 = print zeroes flag
add bh,bl
mov ecx,1000000000    ; ECX = service divisor
.mainCycle:
xor edx,edx
div ecx               ; Produce current digit, EDX:EAX / ECX
and al,0Fh
test bh,bh
js .firstZero
cmp ecx,1
je .firstZero
cmp al,0              ; Not actual left zero ?
jz .skipZero
.firstZero:
mov bh,80h            ; Flag = 1
or al,30h
stosb                 ; Store char
.skipZero:
push rdx              ; Push remainder
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax          ; ECX = Quotient, used as divisor and cycle condition 
pop rax              ; EAX = remainder
inc bh
test ecx,ecx
jnz .mainCycle       ; Cycle if (unsigned) quotient still > 0 
pop rdx rcx rbx rax
ret
;---------- Print double precision value --------------------------------------;
; x87 FPU used, required x87 presence validation by CPUID before call this.    ;
;                                                                              ;
; INPUT:   RAX = Double precision number                                       ;
;          BL  = Number of digits in the INTEGER part,                         ;
;                used for add left non-signed zeroes.                          ; 
;                BL=0 means not print left unsigned zeroes.                    ;
;          BH  = Number of digits in the FLOAT part,                           ;
;                used as precision control.                                    ;
;          RDI = Destination text buffer pointer                               ;
;                                                                              ;
; OUTPUT:  RDI = Modified by text string write                                 ;  
;------------------------------------------------------------------------------;
DoublePrint:
push rax rbx rcx rdx r8 r9 r10 r11
cld
; Detect special cases for DOUBLE format, yet unsigned indication
mov rdx,07FFFFFFFFFFFFFFFh
and rdx,rax
jz .fp64_Zero                ; Go if special cases = 0.0  or  -0.0
mov rcx,07FF8000000000000h
cmp rdx,rcx
je .fp64_QNAN                ; Go if special case = QNAN (Quiet Not a Number)
mov rcx,07FF0000000000000h
cmp rdx,rcx
je .fp64_INF                 ; Go if special case = INF (Infinity)
ja .fp64_NAN                 ; Go if special case = NAN (Not a Number)
; Initializing FPU x87
finit
; Change rounding mode from default (nearest) to truncate  
push rax     ; save input value
push rax     ; reserve space
fstcw [rsp]
pop rax
or ax,0C00h  ; correct Rounding Control, RC = FPU CW bits [11-10]
push rax
fldcw [rsp]
pop rax
; Load input value, note rounding mode already changed
fld qword [rsp]
pop rax
; Separate integer and float parts 
fld st0         ; st0 = value   , st1 = value copy
frndint         ; st0 = integer , st1 = value copy
fxch            ; st0 = value copy , st1 = integer
fsub st0,st1    ; st0 = float , st1 = integer
; Build divisor = f(precision selected) 
mov eax,1
movzx ecx,bh    ; BH = count digits after "."
jrcxz .L6
@@:
imul rax,rax,10
loop @b
.L6:
; Build float part as integer number 
push rax
fimul dword [rsp]
pop rax
; Extract signed Binary Coded Decimal (BCD) to R9:R8, float part .X
push rax rax  ; Make frame for stack variable, used for x87 write data
fbstp [rsp]   ; Store BCD integer and pop, destination is 80 bit = 10 bytes
pop r8 r9     ; R9:R8 = data from x87 write
; Extract signed Binary Coded Decimal (BCD) to R11:R10, integer part X.
push rax rax  ; Make frame for stack variable, used for x87 write data
fbstp [rsp]   ; Store BCD integer and pop, destination is 80 bit = 10 bytes
pop r10 r11     ; R11:R10 = data from x87 write
; Check sign of integer and float part 
bt r11,15     ; R11 bit 15 is bit 79 of 80-bit x87 operand (integer part)
setc dl       ; DL = Sign of integer part
bt r9,15      ; R9 bit 15 is bit 79 of 80-bit x87 operand (floating part)
setc dh       ; DH = Sign of floating part
; Go error if sign of integer and float part mismatch
; This comparision and error branching rejected 
; because bug with -1.0 "-" AND "+", CHECK IF SIGN SAVED ?
; cmp dx,0100h
; je .Error
; cmp dx,0001h
; je .Error
; Write "-" if one of signs "-".
test dx,dx
jz @f            ; Go skip write "-" if both integer/floating signs "+"
; Write "-" if negative value
mov al,'-'
stosb
@@:
; Write INTEGER part 
mov dl,0         ; DL = flag "minimum one digit always printed"
mov ecx,18       ; RCX = maximum number of digits in the integer part 
.L3:             ; Cycle for digits in the INTEGER part
mov al,r11l
shr al,4         ; AL = current digit
cmp cl,1
je .store        ; Go print if last pass, otherwise .X instead 0.X
cmp cl,bl
jbe .store       ; Go print if required by formatting option, BL=count
test dl,dl
jnz .store       ; Go print, if digits sequence already beginned
test al,al
jz .position     ; Otherwise, can go skip print if digit = 0 
.store:
mov dl,1
or al,30h
stosb            ; Write current ASCII digit
.position:
shld r11,r10,4   ; Positioning digits sequence at R11:R10 pair
shl r10,4
loop .L3         ; Cycle for digits in the INTEGER part
; Write decimal point
test bh,bh
jz .exit        ; Skip if not print float part
mov al,'.'
stosb
; Write FLOATING part
std                  ; Write from right to left 
movzx ecx,bh         ; RCX = digits count     
lea rdi,[rdi+rcx]    ; RDI = After last digit (char) position
push rdi
dec rdi
.L4:                 ; Cycle for digits in the FLOATING part
mov al,r8l
and al,0Fh
or al,30h
stosb
shrd r8,r9,4         ; Positioning digits sequence at R9:R8 pair
shr r9,4
loop .L4             ; Cycle for digits in the FLOATING part
pop rdi
cld                  ; Restore strings increment mode
; Go exit subroutine
jmp .exit
; Write strings for different errors types
.fp64_Zero:					; Zero
mov eax,'0.0 '
jmp .fp64special
.fp64_INF:          ; "INF" = Infinity, yet unsigned infinity indicated
mov eax,'INF '
jmp .fp64special
.fp64_NAN:
mov eax,'NAN '      ; "NAN" = (Signaled) Not a number
jmp .fp64special
.fp64_QNAN:
mov eax,'QNAN'      ; "QNAN" = Quiet not a number
.fp64special:
stosd
jmp .exit
.error:
mov al,'?'
stosb
.exit:
; Exit with re-initialize x87 FPU 
finit
pop r11 r10 r9 r8 rdx rcx rbx rax
ret
;--- Detect CPUID support and execute CPUID function #0. ----------------;
; Note CPUID can be supported by CPU but locked by Virtual Monitor.      ;
; Note check bit EFLAGS.21 toggleable, it is CPUID support indicator.    ;
; Note probably wrong result if debug trace this subroutine code.        ;
;                                                                        ;
; INPUT:   RDI = Destination pointer for save CPU Vendor String          ;
;                                                                        ;
; OUTPUT:  CF flag = Status: 0(NC)=Support OK, 1(C)=Not supported        ;
;          Output EAX, RDI, Destination memory valid only if CF=0(NC)    ;
;          EAX = Largest standard CPUID function supported               ;
;          RDI = Input RDI + 12 + 4 , string size fixed = 12 bytes       ;
;          Destination memory at [input RDI] =                           ;
;           bytes [00-11] = CPU Vendor String                            ;
;           byte  [12-12] = 00h, terminator for copy by StringWrite      ;
;           bytes [13-15] = Reserved                                     ;           
;                                                                        ;
; Can destroy registers, volatile by Microsoft x64 calling convention.   ; 
;------------------------------------------------------------------------;
CheckCpuId:
cld                       ; Clear direction, because STOSD used
push rbx
; Check for ID bit writeable for "1"
mov ebx,21
pushf                     ; In the 64-bit mode, push RFLAGS
pop rax
bts eax,ebx               ; Set EAX.21=1
push rax
popf                      ; Load RFLAGS with RFLAGS.21=1
pushf                     ; Store RFLAGS
pop rax                   ; Load RFLAGS to RAX
btr eax,ebx               ; Check EAX.21=1, Set EAX.21=0
jnc .noCpuId              ; Go error branch if cannot set EFLAGS.21=1
; Check for ID bit writeable for "0"
push rax
popf                      ; Load RFLAGS with RFLAGS.21=0
pushf                     ; Store RFLAGS
pop rax                   ; Load RFLAGS to RAX
btr eax,ebx               ; Check EAX.21=0
jc .noCpuId               ; Go if cannot set EFLAGS.21=0
; Execute CPUID function 0, store results
xor eax,eax               ; EAX = Function number for CPUID instruction
cpuid                     ; Execute CPUID function 0
xchg eax,ebx              ; XCHG instead MOV, short code
stosd                     ; Store Vendor String [00-03]
xchg eax,edx	  
stosd                     ; Store Vendor String [04-07]
xchg eax,ecx
stosd                     ; Store Vendor String [08-11]
xor eax,eax
stosd                     ; Zero terminator byte and 3 reserved bytes
xchg eax,ebx              ; Restore EAX = Largest standard function supported
; Exit points
.exitCpuId:
pop rbx
ret                       ; Return, at this point CF=0(NC) after XOR EAX,EAX
.noCpuId:
stc                       ; CF=1(C) means error
jmp .exitCpuId 
;--- Measure CPU TSC (Time Stamp Counter) clock frequency, --------------; 
; store results F=Frequency=[Hz].                                        ;
; Call this subroutine only if CPUID and RDTSC both supported.           ;
;                                                                        ;
; INPUT:   RDI = Destination pointer for save TSC frequency and period	 ;
;                                                                        ;
; OUTPUT:  CF flag = Status: 0(NC)=Measured OK, 1(C)=Measurement error	 ;
;          Output RDI and destination memory valid only if CF=0(NC)      ;
;          RDI = Input RDI + 40 , buffer size fixed = 40 bytes           ;
;          Destination memory at [input RDI] = Results                   ;
;           Qword [00-07] = TSC frequency, Hz = delta TSC per 1 second   ;
;           Qword [08-15] = TSC frequency, Hz, as double precision       ;  
;           Qword [16-23] = TSC frequency, MHz, as double precision      ;
;           Qword [24-31] = TSC period, seconds, as double precision     ;
;           Qword [32-40] = TSC period, nanoeconds, as double precis.    ;
;                                                                        ;
; Can destroy registers, volatile by Microsoft x64 calling convention.   ; 
;------------------------------------------------------------------------;
MeasureTsc:
cld                        ; Clear direction, because STOSQ used
push rbx rbp rbp           ; Last push for reserve local variable space
mov rbp,rsp                ; RBP used for restore RSP and addressing variables
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32                 ; Make parameters shadow
; Start measure frequency
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get current count
mov rsi,[rbp]
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get next count for wait 100 ns
cmp rsi,[rbp]
je @b
mov rsi,[rbp]
add rsi,10000000                  ; 10^7 * 100ns = 1 second
rdtsc
shl rdx,32
lea rbx,[rax+rdx]                 ; RBX = 64-bit TSC at operation start
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp rsi,[rbp]
ja @b
rdtsc
shl rdx,32
or rax,rdx                        ; RAX = 64-bit TSC at operation end
sub rax,rbx                       ; RAX = Delta TSC
jbe .error
; Store Frequency, as 64-bit integer value, Hz, delta-TSC per second
stosq                             ; Store RAX to [RDI], advance pointer RDI+8
; Calculate floating-point double precision values
push 1000000000 0 1000000 rax
finit
fild qword [rsp+00]
fld st0
fidiv dword [rsp+08]
fstp qword [rsp+08]   ; MHz
fld1
fdiv st0,st1
fstp qword [rsp+16]   ; Seconds
fild qword [rsp+24]
fdiv st0,st1
fstp qword [rsp+24]   ; Nanoseconds
fstp qword [rsp+00]   ; Hz
; Store floating-point double precision values
pop rax
stosq           ; Store TSC frequency, Hz, floating point double precision
pop rax
stosq           ; Store TSC frequency, MHz, fl. point double precision
pop rax
stosq           ; Store TSC period, Seconds, fl. point double precision
pop rax
stosq           ; Store TSC period, Nanoseconds, fl. point double precis.
; Restore RSP, pop extra registers, exit
clc             ; CF=0 (NC) means CPU clock measured OK
.exit:
mov rsp,rbp
pop rbp rbp rbx
ret
.error:
stc             ; CF=1 (CY) means CPU clock measured ERROR
jmp .exit

;------------------------------------------------------------------------------;
;                              Data section.                                   ;
;          Note remember about error if data section exist but empty.          ;     
;------------------------------------------------------------------------------;

section '.data' data readable writeable
; Constants located at exe file, text strings
_Caption        DB  '  GPR64/FPU/ZMM[0-15] and timings dump',0
_Message        DB  'RAXRBXRCXRDXRSPRBPRSIRDI'
                DB  'R8 R9 R10R11R12R13R14R15'
                DB  'ST'
                DB  'ZMM'
_TSC            DB  0Ah, 0Dh, 'TSC CLK = ',0
_MHz            DB  ' MHz',0
_dTSC           DB  0Ah, 0Dh, 'dTSC = ',0
_dFT            DB  0Ah, 0Dh, 'dFT = ',0
_Iterations     DB  0Ah, 0Dh, 'Iterations = ',0
_dTSCPass       DB  0Ah, 0Dh, 0Ah, 0Dh, 'dTSC/Pass = ',0
_Clks           DB  ' Clks',0
_h              DB  'h ( ',0
_ms             DB  ' ms )',0
_h1             DB  'h',0
_NoCpuid        DB  'CPUID not supported or locked.',0
_NoX87          DB  'x87 FPU not supported or locked.',0
_NoTsc          DB  'TSC not supported or locked.',0
_NoSse          DB  'SSE not supported or locked.',0
_NoSse2         DB  'SSE2 not supported or locked.',0
_NoAvx          DB  'AVX not supported or locked.',0
_NoAvx512       DB  'AVX512 not supported or locked.',0
_NoOsxsave      DB  'OSXSAVE not supported or locked.',0
_NoAvxContext   DB  'AVX512 context not supported by OS.',0
_ErrorTsc       DB  'TSC clock measurement failed.',0
; Constants located at exe file, numbers
_value1E3       DQ  1E3    ; Multiplier for convert seconds to milliseconds
_value1EM4      DQ  1E-4   ; Multiplier for convert 100 ns to milliseconds 
; Variables not requires space in the exe file, registers and measur. results
_CpuidBuffer    DB  16      DUP (?)   ; CPUID results: signature, 0, reserved
_RdtscBuffer    DQ  5       DUP (?)   ; TSC clock measurement results 
_FileTime1      DQ  ?                 ; WinAPI timer at start of measured code
_FileTime2      DQ  ?                 ; WinAPI timer at end of measured code
_FileTimeDelta  DQ  ?                 ; WinAPI timer delta
_TscTime1       DQ  ?                 ; CPU TSC at start of measured code
_TscTime2       DQ  ?                 ; CPU TSC at end of measured code
_TscTimeDelta   DQ  ?                 ; CPU TSC delta
_GprBuffer      DQ  16      DUP (?)   ; space for 16 GPR RAX, RBX ... R14, R15
_FpuBuffer      DQ  8       DUP (?)   ; space for 8 FPU registers as doubles
_Avx512Buffer   DQ  16*8*8  DUP (?)   ; space for 16 AVX registers
; Variables not requires space in the exe file, defined and calculated results
_DefinedCycles  DQ  ?                 ; Number of measurement iterations 
_DeltaTSC       DQ  ?                 ; Delta TSC per all measurement, ms
_DeltaFT        DQ  ?                 ; Delta File Time per all measurement, ms  
_DeltaPass      DQ  ?                 ; Delta TSC per one measur. iteratin, ns
; Variables not requires space in the exe file, text buffer
align 4096
_DataBuffer     DB  16384   DUP (?)   ; Buffer for work data
_TextBuffer     DB  16384   DUP (?)   ; Buffer for build text strings

;------------------------------------------------------------------------------;
;                              Import section.                                 ;
;------------------------------------------------------------------------------;

section '.idata' import data readable writeable
library kernel32, 'KERNEL32.DLL', user32,'USER32.DLL'    
include 'api\user32.inc'       ; USER32.DLL required because MessageBoxA used
include 'api\kernel32.inc'     ; KERNEL32.DLL required for System API
