;------------------------------------------------------------------------------;
;                Registers and memory dump subroutines library:                ;
;             connect include files with globally used subroutines.            ; 
;------------------------------------------------------------------------------;

;--- Dump 8 32-bit general purpose registers ----;
; INPUT:   GPR registers values for dump         ;
; OUTPUT:  None                                  ; 
;------------------------------------------------;

DumpGPR32:
; Save registers for non-volatile and for dump
push eax ebx ecx edx esi edi ebp
lea eax,[esp + 7*4 + 4]
push eax
; Initializing dump cycle
cld
mov ebx,8
lea esi,[NamesGPR32]
lea ebp,[esp + 7*4 ]
; Dump cycle with 8 Read instructions
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call ConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
mov eax,[ebp]
call HexPrint32
mov al,0
stosb
call ConsoleWrite
lea ecx,[CrLf]
call ConsoleWrite
sub ebp,4           ; Select next register at stack frame
add esi,4           ; Select next text string for register name
dec ebx             ; Cycle counter for 16 general-purpose registers
jnz .L0
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea ecx,[CrLf]
call ConsoleWrite
; Restore registers and return
pop eax ebp edi esi edx ecx ebx eax
ret

;--- Dump 6 16-bit segment selectors registers ---------;
; INPUT:   Segment selectors registers values for dump  ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpSelectors:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Push 6 selectors
xor eax,eax
mov ax,gs
push eax      ; PUSH #1
mov ax,fs
push eax      ; PUSH #2
mov ax,ss
push eax      ; PUSH #3
mov ax,es
push eax      ; PUSH #4
mov ax,ds
push eax      ; PUSH #5
mov ax,cs
push eax      ; PUSH #6
; Initializing dump cycle
cld
mov ebx,6
lea esi,[NamesSelectors]
; Dump cycle with pop 6 selectors
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call ConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax             ; POP #[6-1] 
call HexPrint16
mov al,0
stosb
call ConsoleWrite
lea ecx,[CrLf]
call ConsoleWrite
add esi,3           ; Select next text string for register name
dec ebx             ; Cycle counter for 6 segment selectors registers
jnz .L0
; Entry point for return
DumpReturn:
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea ecx,[CrLf]
call ConsoleWrite
; Restore registers and return
pop ebp edi esi edx ecx ebx eax
ret

;--- Dump 8 x87 FPU registers --------------------------;
; INPUT:   FPU registers values for dump                ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpFPU:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 8 registers
sub esp,64
fstp qword [esp + 8*0]
fstp qword [esp + 8*1]
fstp qword [esp + 8*2]
fstp qword [esp + 8*3]
fstp qword [esp + 8*4]
fstp qword [esp + 8*5]
fstp qword [esp + 8*6]
fstp qword [esp + 8*7]
; Initializing dump cycle
cld
mov ebp,8
lea esi,[NamesFPU]
; Dump cycle with pop 8 registers
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call ConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax edx         ; POP #[8-1] 
mov bx,0700h
call DoublePrint
mov al,0
stosb
call ConsoleWrite
lea ecx,[CrLf]
call ConsoleWrite
add esi,4           ; Select next text string for register name
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
push eax ebx ecx edx esi edi ebp
; Store 8 registers
sub esp,64
movq [esp + 8*0],mm0
movq [esp + 8*1],mm1
movq [esp + 8*2],mm2
movq [esp + 8*3],mm3
movq [esp + 8*4],mm4
movq [esp + 8*5],mm5
movq [esp + 8*6],mm6
movq [esp + 8*7],mm7
; Initializing dump cycle
cld
mov ebp,8
lea esi,[NamesMMX]
; Dump cycle with pop 8 registers
.L0:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call ConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax edx         ; POP #[8-1] 
call HexPrint64
mov al,0
stosb
call ConsoleWrite
lea ecx,[CrLf]
call ConsoleWrite
add esi,4           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .L0
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 8 SSE registers ------------------------------;
; INPUT:   SSE registers values for dump                ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpSSE:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,128
movups [esp + 16*00],xmm0
movups [esp + 16*01],xmm1
movups [esp + 16*02],xmm2
movups [esp + 16*03],xmm3
movups [esp + 16*04],xmm4
movups [esp + 16*05],xmm5
movups [esp + 16*06],xmm6
movups [esp + 16*07],xmm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameSSE]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
; XMM[i] data frame start 
mov eax,[esp+00]
mov edx,[esp+04]
call HexPrint64
mov al,' '
stosb
mov eax,[esp+08]
mov edx,[esp+12] 
call HexPrint64
add esp,16
; XMM[i] data frame start
mov al,0
stosb
call ConsoleWrite
; Cycle
lea ecx,[CrLf]
call ConsoleWrite
inc ebp
cmp ebp,8
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 8 AVX256 registers ---------------------------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpAVX256:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,256
vmovupd [esp + 32*00],ymm0
vmovupd [esp + 32*01],ymm1
vmovupd [esp + 32*02],ymm2
vmovupd [esp + 32*03],ymm3
vmovupd [esp + 32*04],ymm4
vmovupd [esp + 32*05],ymm5
vmovupd [esp + 32*06],ymm6
vmovupd [esp + 32*07],ymm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX256]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
; YMM[i] data frame start 
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 16]
mov edx,[esp + 16 + 4] 
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 08]
mov edx,[esp + 08 + 4] 
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 00]
mov edx,[esp + 00 + 4] 
call HexPrint64
add esp,32
; YMM[i] data frame end
mov al,0
stosb
call ConsoleWrite
; Cycle
lea ecx,[CrLf]
call ConsoleWrite
inc ebp
cmp ebp,8
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 8 AVX256 registers as double numbers ---------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpAVX256asDouble:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,256
vmovupd [esp + 32*00],ymm0
vmovupd [esp + 32*01],ymm1
vmovupd [esp + 32*02],ymm2
vmovupd [esp + 32*03],ymm3
vmovupd [esp + 32*04],ymm4
vmovupd [esp + 32*05],ymm5
vmovupd [esp + 32*06],ymm6
vmovupd [esp + 32*07],ymm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX256]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov al,' '
stosb
; YMM[i] data frame start 
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HelperDoubleDump
mov eax,[esp + 16]
mov edx,[esp + 16 + 4] 
call HelperDoubleDump
mov eax,[esp + 08]
mov edx,[esp + 08 + 4] 
call HelperDoubleDump
mov eax,[esp + 00]
mov edx,[esp + 00 + 4] 
call HelperDoubleDump
add esp,32
; YMM[i] data frame end
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Cycle
lea ecx,[CrLf]
call ConsoleWrite
inc ebp
cmp ebp,8
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

HelperDoubleDump:
push edi eax
mov ecx,8
mov al,' '
rep stosb
pop eax edi
push edi
add edi,2
mov bx,0200h
push eax
test edx,edx
js .L1
mov al,'+'
stosb
.L1:
pop eax
call DoublePrint
pop edi
add edi,7
mov al,' '
cmp byte [edi],' '
je .L0
mov al,'\'
.L0:
stosb
ret

;--- Dump 8 AVX512 registers ---------------------------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpAVX512:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,512
vmovupd [esp + 64*00],zmm0
vmovupd [esp + 64*01],zmm1
vmovupd [esp + 64*02],zmm2
vmovupd [esp + 64*03],zmm3
vmovupd [esp + 64*04],zmm4
vmovupd [esp + 64*05],zmm5
vmovupd [esp + 64*06],zmm6
vmovupd [esp + 64*07],zmm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX512]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
; ZMM[i] data frame start 
mov eax,[esp + 56]
mov edx,[esp + 56 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 48]
mov edx,[esp + 48 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 40]
mov edx,[esp + 40 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 32]
mov edx,[esp + 32 + 4]
call HexPrint64
lea esi,[IntervalAVX512]
call StringWrite
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 16]
mov edx,[esp + 16 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 08]
mov edx,[esp + 08 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 00]
mov edx,[esp + 00 + 4]
call HexPrint64
add esp,64
; ZMM[i] data frame end
mov al,0
stosb
call ConsoleWrite
; Cycle
lea ecx,[CrLf]
call ConsoleWrite
inc ebp
cmp ebp,8
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump 8 AVX512 registers as double numbers ---------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpAVX512asDouble:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,512
vmovupd [esp + 64*00],zmm0
vmovupd [esp + 64*01],zmm1
vmovupd [esp + 64*02],zmm2
vmovupd [esp + 64*03],zmm3
vmovupd [esp + 64*04],zmm4
vmovupd [esp + 64*05],zmm5
vmovupd [esp + 64*06],zmm6
vmovupd [esp + 64*07],zmm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.L0:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX512]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov al,' '
stosb
; ZMM[i] data frame start 
mov eax,[esp + 56]
mov edx,[esp + 56 + 4]
call HelperDoubleDump
mov eax,[esp + 48]
mov edx,[esp + 48 + 4]
call HelperDoubleDump
mov eax,[esp + 40]
mov edx,[esp + 40 + 4]
call HelperDoubleDump
mov eax,[esp + 32]
mov edx,[esp + 32 + 4]
call HelperDoubleDump
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HelperDoubleDump
mov eax,[esp + 16]
mov edx,[esp + 16 + 4]
call HelperDoubleDump
mov eax,[esp + 08]
mov edx,[esp + 08 + 4]
call HelperDoubleDump
mov eax,[esp + 00]
mov edx,[esp + 00 + 4]
call HelperDoubleDump
add esp,64
; ZMM[i] data frame end
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
; Cycle
lea ecx,[CrLf]
call ConsoleWrite
inc ebp
cmp ebp,8
jnz .L0             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn

;--- Dump memory region --------------------------------;
; INPUT:   ESI = Pointer to region for dump             ;
;          EBX = Region length, bytes                   ;
;          Memory [RSI] = data for dump                 ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;

DumpMemory:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
push 0

.L0:
test dword [esp],0Fh
jnz .L1
mov ecx,DUMP_ADDRESS_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov eax,[esp]
call HexPrint32
mov ax,'  '
stosw
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call ConsoleWrite
.L1:
mov ecx,DUMP_DATA_COLOR
call SetFgColor

mov ecx,16
mov ebp,ebx
push dword [esp] ecx 
lea edi,[TEMP_BUFFER]
.L2:

dec ebp
js .L7

mov eax,dword [esp+4]
mov al,[esi+eax]
call HexPrint8
mov al,' '
stosb
jmp .L8

.L7:
mov ax,'  '
stosw
stosb

.L8:
inc dword [esp+4]
loop .L2
mov al,' '
stosb
pop ecx eax

cmp ecx,ebx
jbe .L3
mov ecx,ebx
.L3:

.L4:
mov eax,dword [esp]
mov al,[esi+eax]
cmp al,' '
jb .L6
cmp al,'z'
jbe .L5
.L6:
mov al,'.'
.L5:
stosb
inc dword [esp]
loop .L4

mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call ConsoleWrite

; Cycle
lea ecx,[CrLf]
call ConsoleWrite
sub ebx,16
ja .L0 

; Go to restore original color, insert empty string, restore registers
pop eax
jmp DumpReturn



