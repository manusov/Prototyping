;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                   connect include files with subroutines.                    ; 
;------------------------------------------------------------------------------;

;--- Advanced Matrix Extension (AMX) test and dump application scenario -------;
; Input/Output parameters and volatile registers compatible with               ; 
; Microsoft x64 calling convention.                                            ;
; INPUT:   RCX = Pointer to HardwareShell status variables block               ;
;          RDX = Pointer to HardwareShell temporary buffer                     ;
; OUTPUT:  RAX = Status, 0=Error, otherwise no errors                          ;
;------------------------------------------------------------------------------;
ApplicationTestAmx:
push rbx rsi rdi rbp r12 r13 r14 r15 
mov rbp,rsp              ; RBP = Storage for RSP 
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov r15,rcx              ; R15 = Pointer to error status block
xor eax,eax
mov ERROR_STRING_1,rax
mov ERROR_STRING_2,rax
mov ERROR_CODE,rax
mov r14,rdx              ; R14 = Temporary buffer base
xor r13,r13              ; R13 = Memory buffer base, yet blank for skip release
;---------- Check CPUID support by check bit RFLAGS.21 writeable --------------; 
mov ERROR_STRING_1, MsgErrorCpuid  ; Write status for step: check CPUID support
mov ebx,21               ; Start check ID bit writeable for "1"
pushf                    ; In the 64-bit mode, push RFLAGS
pop rax                  
bts eax,ebx              ; Set EAX.21=1
push rax
popf                     ; Load RFLAGS with RFLAGS.21=1
pushf                    ; Store RFLAGS
pop rax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=1, Set EAX.21=0
jnc .error               ; Go error branch if cannot set EFLAGS.21=1
push rax                 ; Start check ID bit writeable for "0"
popf                     ; Load RFLAGS with RFLAGS.21=0
pushf                    ; Store RFLAGS
pop rax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=0
jc .error                ; Go error branch if cannot set EFLAGS.21=0
;---------- Check Intel AMX and AMX-INT8 support ------------------------------;
mov ERROR_STRING_1, MsgErrorAmx
mov esi,7
xor eax,eax
cpuid
cmp eax,esi
jb .error                ; Go error if CPUID function 7 not supported
xchg eax,esi
xor ecx,ecx
cpuid
bt edx,24
jnc .error               ; Go error if AMX not supported
mov ERROR_STRING_1, MsgErrorFormat
bt edx,25
jnc .error               ; Go error if AMX-INT8 not supported
;---------- Check XCR0 register supported and enabled -------------------------;
mov ERROR_STRING_1, MsgErrorContext
mov eax,1
cpuid
bt ecx,27
jnc .error               ; Go error if XSETBV/XGETBV not supported or disabled
;---------- Check AMX context bits at XCR0 register ---------------------------; 
mov ERROR_STRING_1, MsgErrorAmxContext
xor ecx,ecx
xgetbv
mov ecx,00060000h
and eax,ecx
cmp eax,ecx
jne .error               ; Go error if XTILEDATA, XTILECFG context not enabled
;---------- Check Palette configuration ---------------------------------------;
; TODO. This fragment required more flexibility for different Intel AMX
; implementations. Yet constants only.
mov ERROR_STRING_1, MsgErrorAmxPalette
mov esi,1Dh
xor eax,eax
cpuid
cmp eax,esi
jb .error           ; Go error if CPUID function 1Dh not supported
mov eax,esi
xor ecx,ecx
cpuid
test eax,eax
jz .error           ; Go error if subfunction 1 not supported
js .error           ; Go error if subfunctions limit invalid 
xchg eax,esi
mov ecx,1
cpuid
cmp eax,04002000h
jne .error          ; Go error if incompatible bytes per tile, total tile bytes
cmp ebx,00080040h
jne .error          ; Go error if incompatible registers count, bytes per reg.
cmp cx,0010h
jne .error          ; Go error if incompatible matrix maximum rows count 
;---------- Check TMUL configuration ------------------------------------------;
; TODO. This fragment required more flexibility for different Intel AMX
; implementations. Yet constants only.
mov ERROR_STRING_1, MsgErrorAmxTmul
mov esi,01Eh
xor eax,eax
cpuid
cmp eax,esi
jb .error           ; Go error if CPUID function 1Eh not supported
mov eax,esi
xor ecx,ecx
cpuid
and ebx,00FFFFFFh
cmp ebx,00004010h
jne .error          ; Go error if incompatible column bytes, elements count 
;---------- Initialize AMX context in memory ----------------------------------;
cld
lea rdi,[CONTEXT_BASE]
mov rsi,rdi
mov ecx,CONTEXT_SIZE / 8
xor eax,eax
rep stosq
mov byte [rsi + 00],1
mov rax,0040004000400040h
mov [rsi + 16],rax
mov [rsi + 24],rax
mov rax,1010101010101010h
mov [rsi + 48],rax 
;---------- Load AMX configuration --------------------------------------------;
DB 0C4h, 0E2h, 078h, 049h, 006h         ; ldtilecfg [rsi] 
;---------- Fill buffer with pattern 11h for verify read-back -----------------;
mov rdi,rsi
mov ecx,CONTEXT_SIZE / 8 * 2
mov rax,1111111111111111h
rep stosq
;---------- Read-back AMX configuration for verify ----------------------------;
DB 0C4h, 0E2h, 079h, 049h, 006h         ; sttilecfg [rsi] 
;---------- Initialize matrix operands in memory ------------------------------;
lea rdi,[TMM_BASE]
mov ecx,ALL_TMM_SIZE / 8
mov rax,1111111111111111h   ; xor eax,eax
rep stosq
lea rdi,[TMM0_BASE]
mov ecx,ONE_TMM_SIZE / 8
mov rax,0101010101010101h
rep stosq
lea rdi,[TMM1_BASE]
mov ecx,ONE_TMM_SIZE / 8
rep stosq
;---------- Load TMM registers for matrix multiply operation ------------------; 
lea rcx,[TMM0_BASE]                    ; RCX = Base address
mov eax,64                             ; RAX = Index = Stride for AMX load/save
DB 0C4h, 0E2h, 07Bh, 04Bh, 004h, 001h  ; tileload tmm0,[rcx + rax]
lea rcx,[TMM1_BASE]                    ; RCX = Base address
DB 0C4h, 0E2h, 07Bh, 04Bh, 00Ch, 001h  ; tileload tmm1,[rcx + rax]
;---------- Make matrix multiply operation ------------------------------------; 
DB 0C4h, 0E2h, 078h, 05Eh, 0D1h        ; tdpbuud tmm2,tmm1,tmm0
;---------- Additional test, load data to TMM4 --------------------------------;
lea rdi,[TMM0_BASE]
mov ecx,ONE_TMM_SIZE / 4
mov eax,11110000h
@@:
stosd
inc eax
dec ecx
jnz @b
lea rcx,[TMM0_BASE]                    ; RCX = Base address
mov eax,64                             ; RAX = Index = Stride for AMX load/save
DB 0C4h, 0E2h, 07Bh, 04Bh, 024h, 001h  ; tileload tmm4,[rcx + rax]
;---------- Exit points -------------------------------------------------------;
mov eax,1
jmp .exit
.error:
xor eax,eax
.exit:
mov rsp,rbp
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret

