;------------------------------------------------------------------------------;
;                Registers and memory dump subroutines library:                ;
;                     dump CPU registers and memory areas.                     ;
;------------------------------------------------------------------------------;

;--- Dump 16 64-bit general purpose registers ----------;
; INPUT:   GPR registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpGPR64:
; Save registers for non-volatile and for dump
push rax rbx rcx rdx rsi rdi rbp
lea rax,[rsp + 7*8 + 8]
push rax
push r8 r9 r10 r11 r12 r13 r14 r15
; Initializing dump cycle
cld
mov ebx,16
lea rsi,[NamesGPR64]
lea rbp,[rsp + 15*8 ]
; Dump cycle with 16 Read instructions
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
mov rax,[rbp]
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
sub rbp,8           ; Select next register at stack frame
add rsi,4           ; Select next text string for register name
dec ebx             ; Cycle counter for 16 general-purpose registers
jnz .L0
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea rcx,[CrLf]
call InternalConsoleWrite
; Restore registers and return
pop r15 r14 r13 r12 r11 r10 r9 r8
pop rax rbp rdi rsi rdx rcx rbx rax
ret

;--- Dump 6 16-bit segment selectors registers ---------;
; INPUT:   Segment selectors registers values for dump  ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpSelectors:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Push 6 selectors
xor eax,eax
mov ax,gs
push rax      ; PUSH #1
mov ax,fs
push rax      ; PUSH #2
mov ax,ss
push rax      ; PUSH #3
mov ax,es
push rax      ; PUSH #4
mov ax,ds
push rax      ; PUSH #5
mov ax,cs
push rax      ; PUSH #6
; Initializing dump cycle
cld
mov ebx,6
lea rsi,[NamesSelectors]
; Dump cycle with pop 6 selectors
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[6-1] 
call HexPrint16
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,3           ; Select next text string for register name
dec ebx             ; Cycle counter for 6 segment selectors registers
jnz .L0
; Entry point for return
DumpReturn:
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea rcx,[CrLf]
call InternalConsoleWrite
; Restore registers and return
pop r11 r10 r9 r8 rbp rdi rsi rdx rcx rbx rax
ret

;--- Dump 8 x87 FPU registers --------------------------;
; INPUT:   FPU registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpFPU:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 8 registers
sub rsp,64
fstp qword [rsp + 8*0]
fstp qword [rsp + 8*1]
fstp qword [rsp + 8*2]
fstp qword [rsp + 8*3]
fstp qword [rsp + 8*4]
fstp qword [rsp + 8*5]
fstp qword [rsp + 8*6]
fstp qword [rsp + 8*7]
; Initializing dump cycle
cld
mov ebp,8
lea rsi,[NamesFPU]
; Dump cycle with pop 8 registers
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[8-1] 
mov bx,0700h
call DoublePrint
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,4           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 FPU selectors registers
jnz .L0
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 8 MMX registers ------------------------------;
; INPUT:   MMX registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpMMX:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 8 registers
sub rsp,64
movq [rsp + 8*0],mm0
movq [rsp + 8*1],mm1
movq [rsp + 8*2],mm2
movq [rsp + 8*3],mm3
movq [rsp + 8*4],mm4
movq [rsp + 8*5],mm5
movq [rsp + 8*6],mm6
movq [rsp + 8*7],mm7
; Initializing dump cycle
cld
mov ebp,8
lea rsi,[NamesMMX]
; Dump cycle with pop 8 registers
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[8-1] 
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,4           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .L0
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 16 SSE registers -----------------------------;
; INPUT:   SSE registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpSSE:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,256
movups [rsp + 16*00],xmm0
movups [rsp + 16*01],xmm1
movups [rsp + 16*02],xmm2
movups [rsp + 16*03],xmm3
movups [rsp + 16*04],xmm4
movups [rsp + 16*05],xmm5
movups [rsp + 16*06],xmm6
movups [rsp + 16*07],xmm7
movups [rsp + 16*08],xmm8
movups [rsp + 16*09],xmm9
movups [rsp + 16*10],xmm10
movups [rsp + 16*11],xmm11
movups [rsp + 16*12],xmm12
movups [rsp + 16*13],xmm13
movups [rsp + 16*14],xmm14
movups [rsp + 16*15],xmm15
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameSSE]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, XMM0-XMM15
cmp ebp,9
ja .L1
mov al,' '
stosb
.L1:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
; XMM[i] data frame start 
mov rax,[rsp+00]
call HexPrint64
mov al,' '
stosb
mov rax,[rsp+08] 
call HexPrint64
add rsp,16
; XMM[i] data frame start
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,16
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 16 AVX256 registers --------------------------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpAVX256:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,512
vmovupd [rsp + 32*00],ymm0
vmovupd [rsp + 32*01],ymm1
vmovupd [rsp + 32*02],ymm2
vmovupd [rsp + 32*03],ymm3
vmovupd [rsp + 32*04],ymm4
vmovupd [rsp + 32*05],ymm5
vmovupd [rsp + 32*06],ymm6
vmovupd [rsp + 32*07],ymm7
vmovupd [rsp + 32*08],ymm8
vmovupd [rsp + 32*09],ymm9
vmovupd [rsp + 32*10],ymm10
vmovupd [rsp + 32*11],ymm11
vmovupd [rsp + 32*12],ymm12
vmovupd [rsp + 32*13],ymm13
vmovupd [rsp + 32*14],ymm14
vmovupd [rsp + 32*15],ymm15
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX256]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .L1
mov al,' '
stosb
.L1:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
; YMM[i] data frame start 
mov rax,[rsp + 24]
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 16] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 08] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 00] 
call HexPrint64
add rsp,32
; YMM[i] data frame end
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,16
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 16 AVX256 registers as double numbers --------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpAVX256asDouble:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,512
vmovupd [rsp + 32*00],ymm0
vmovupd [rsp + 32*01],ymm1
vmovupd [rsp + 32*02],ymm2
vmovupd [rsp + 32*03],ymm3
vmovupd [rsp + 32*04],ymm4
vmovupd [rsp + 32*05],ymm5
vmovupd [rsp + 32*06],ymm6
vmovupd [rsp + 32*07],ymm7
vmovupd [rsp + 32*08],ymm8
vmovupd [rsp + 32*09],ymm9
vmovupd [rsp + 32*10],ymm10
vmovupd [rsp + 32*11],ymm11
vmovupd [rsp + 32*12],ymm12
vmovupd [rsp + 32*13],ymm13
vmovupd [rsp + 32*14],ymm14
vmovupd [rsp + 32*15],ymm15
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX256]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .L1
mov al,' '
stosb
.L1:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov al,' '
stosb
; YMM[i] data frame start 
mov rax,[rsp + 24]
call HelperDoubleDump
mov rax,[rsp + 16] 
call HelperDoubleDump
mov rax,[rsp + 08] 
call HelperDoubleDump
mov rax,[rsp + 00] 
call HelperDoubleDump
add rsp,32
; YMM[i] data frame end
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,16
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

HelperDoubleDump:
push rdi rax
mov ecx,8
mov al,' '
rep stosb
pop rax rdi
push rdi
add rdi,2
mov bx,0200h
push rax
test rax,rax
js .L1
mov al,'+'
stosb
.L1:
pop rax
call DoublePrint
pop rdi
add rdi,7
mov al,' '
cmp byte [rdi],' '
je .L0
mov al,'\'
.L0:
stosb
ret

;--- Dump 32 AVX512 registers --------------------------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpAVX512:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,2048
vmovupd [rsp + 64*00],zmm0
vmovupd [rsp + 64*01],zmm1
vmovupd [rsp + 64*02],zmm2
vmovupd [rsp + 64*03],zmm3
vmovupd [rsp + 64*04],zmm4
vmovupd [rsp + 64*05],zmm5
vmovupd [rsp + 64*06],zmm6
vmovupd [rsp + 64*07],zmm7
vmovupd [rsp + 64*08],zmm8
vmovupd [rsp + 64*09],zmm9
vmovupd [rsp + 64*10],zmm10
vmovupd [rsp + 64*11],zmm11
vmovupd [rsp + 64*12],zmm12
vmovupd [rsp + 64*13],zmm13
vmovupd [rsp + 64*14],zmm14
vmovupd [rsp + 64*15],zmm15
vmovupd [rsp + 64*16],zmm16
vmovupd [rsp + 64*17],zmm17
vmovupd [rsp + 64*18],zmm18
vmovupd [rsp + 64*19],zmm19
vmovupd [rsp + 64*20],zmm20
vmovupd [rsp + 64*21],zmm21
vmovupd [rsp + 64*22],zmm22
vmovupd [rsp + 64*23],zmm23
vmovupd [rsp + 64*24],zmm24
vmovupd [rsp + 64*25],zmm25
vmovupd [rsp + 64*26],zmm26
vmovupd [rsp + 64*27],zmm27
vmovupd [rsp + 64*28],zmm28
vmovupd [rsp + 64*29],zmm29
vmovupd [rsp + 64*30],zmm30
vmovupd [rsp + 64*31],zmm31
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX512]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .L1
mov al,' '
stosb
.L1:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
; ZMM[i] data frame start 
mov rax,[rsp + 56]
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 48] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 40] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 32] 
call HexPrint64
lea rsi,[IntervalAVX512]
call StringWrite
mov rax,[rsp + 24]
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 16] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 08] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 00] 
call HexPrint64
add rsp,64
; ZMM[i] data frame end
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,32
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 32 AVX512 registers as double numbers --------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpAVX512asDouble:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,2048
vmovupd [rsp + 64*00],zmm0
vmovupd [rsp + 64*01],zmm1
vmovupd [rsp + 64*02],zmm2
vmovupd [rsp + 64*03],zmm3
vmovupd [rsp + 64*04],zmm4
vmovupd [rsp + 64*05],zmm5
vmovupd [rsp + 64*06],zmm6
vmovupd [rsp + 64*07],zmm7
vmovupd [rsp + 64*08],zmm8
vmovupd [rsp + 64*09],zmm9
vmovupd [rsp + 64*10],zmm10
vmovupd [rsp + 64*11],zmm11
vmovupd [rsp + 64*12],zmm12
vmovupd [rsp + 64*13],zmm13
vmovupd [rsp + 64*14],zmm14
vmovupd [rsp + 64*15],zmm15
vmovupd [rsp + 64*16],zmm16
vmovupd [rsp + 64*17],zmm17
vmovupd [rsp + 64*18],zmm18
vmovupd [rsp + 64*19],zmm19
vmovupd [rsp + 64*20],zmm20
vmovupd [rsp + 64*21],zmm21
vmovupd [rsp + 64*22],zmm22
vmovupd [rsp + 64*23],zmm23
vmovupd [rsp + 64*24],zmm24
vmovupd [rsp + 64*25],zmm25
vmovupd [rsp + 64*26],zmm26
vmovupd [rsp + 64*27],zmm27
vmovupd [rsp + 64*28],zmm28
vmovupd [rsp + 64*29],zmm29
vmovupd [rsp + 64*30],zmm30
vmovupd [rsp + 64*31],zmm31
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX512]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .L1
mov al,' '
stosb
.L1:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov al,' '
stosb
; ZMM[i] data frame start 
mov rax,[rsp + 56]
call HelperDoubleDump
mov rax,[rsp + 48] 
call HelperDoubleDump
mov rax,[rsp + 40] 
call HelperDoubleDump
mov rax,[rsp + 32] 
call HelperDoubleDump
mov rax,[rsp + 24]
call HelperDoubleDump
mov rax,[rsp + 16] 
call HelperDoubleDump
mov rax,[rsp + 08] 
call HelperDoubleDump
mov rax,[rsp + 00] 
call HelperDoubleDump
add rsp,64
; ZMM[i] data frame end
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,32
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump memory region --------------------------------;
; INPUT:   RSI = Pointer to region for dump             ;
;          EBX = Region length, bytes                   ;
;          Memory [RSI] = data for dump                 ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;

DumpMemory:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11 r15

xor r15,r15

.L0:
test r15,0Fh
jnz .L1
mov ecx,DUMP_ADDRESS_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov eax,r15d
call HexPrint32
mov ax,'  '
stosw
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
.L1:
mov ecx,DUMP_DATA_COLOR
call SetFgColor

mov ecx,16
mov ebp,ebx
push rcx r15
lea rdi,[TEMP_BUFFER]
.L2:

dec ebp
js .L7

mov al,[rsi+r15]
call HexPrint8
mov al,' '
stosb
jmp .L8

.L7:
mov ax,'  '
stosw
stosb

.L8:
inc r15
loop .L2
mov al,' '
stosb
pop r15 rcx

cmp ecx,ebx
jbe .L3
mov ecx,ebx
.L3:

.L4:
mov al,[rsi+r15]
cmp al,' '
jb .L6
cmp al,'z'
jbe .L5
.L6:
mov al,'.'
.L5:
stosb
inc r15
loop .L4

mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite

; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
sub ebx,16
ja .L0 

; Go to restore original color, insert empty string, restore registers
pop r15
jmp DumpReturn
