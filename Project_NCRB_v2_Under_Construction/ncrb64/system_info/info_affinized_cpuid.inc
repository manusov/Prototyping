;---------- Get Affinized CPUID dump information ------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoAffinizedCpuid:
sub rsp,32 + 8
push rdi r13
sub rsp,32
mov rdi,rbp
cmp [rsi + DYNAIMPORT._SetThreadAffinityMask],0
je .skipAffCpuid
call [GetCurrentThread]
test rax,rax
jz .skipAffCpuid 
mov r13,rax              ; R13 = Current thread handle
mov r14d,[OS_DATA.systemInfo.dwNumberOfProcessors] ; [r15 + REGISTRY64.osData.systemInfo.dwNumberOfProcessors] 
test r14d,r14d
jz .skipAffCpuid
cmp r14d,16              ; Yet limited for maximum 16 logical processors
jbe @f
mov r14d,16      
@@:                      ; R14D = Logical processors count
xor r12d,r12d            ; R12 = Storage for original affinity mask
mov ebx,1                ; RBX = Variable affinity mask for dump
.affinizedDump:
mov rdx,rbx              ; RDX = Parm#2 = Affinity mask
mov rcx,r13              ; RCX = Parm#1 = Current thread handle 
call [rsi + DYNAIMPORT._SetThreadAffinityMask]
test r12,r12
jnz @f
xchg r12,rax
@@:
call GetCPUID
mov eax,[rdi]
shl eax,5
lea rdi,[rdi + rax + 32]
rol rbx,1
bsf rax,rbx
cmp rax,r14
jb .affinizedDump 
test r12,r12
jz @f
mov rdx,r12              ; RDX = Parm#2 = Affinity mask
mov rcx,r13              ; RCX = Parm#1 = Current thread handle 
call [rsi + DYNAIMPORT._SetThreadAffinityMask]
@@:
mov [DYNA_PTR.listAffCpuid.objectStart],rbp
mov [DYNA_PTR.listAffCpuid.objectStop],rdi
mov rbp,rdi
.skipAffCpuid:
add rsp,32
pop r13 rdi
add rsp,32 + 8
ret
;--- Build Affinized CPUID dump information for text report and GUI widgets ---;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextAffinizedCpuid:
mov rsi,[DYNA_PTR.listAffCpuid.objectStart]
mov rcx,[DYNA_PTR.listAffCpuid.objectStop]
test rcx,rcx
jz .noAffCpuidData
mov [DYNA_PTR.textAffCpuid.objectStart],rbp
xor r14,r14
.scanBlocks:
push rcx
mov r12d,[rsi]
bts r12,63
add rsi,32
.scanLines:
mov rdi,rbp
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
btr r12,63
jnc @f
lea rdi,[rbp + 2]
mov bl,0
mov eax,r14d
call DecimalPrint32
@@:
mov rbx,rsi
mov ax,0C04h
call HelperHexStringNh
mov ax,1910h
call HelperHexStringNh
mov ax,2414h
call HelperHexStringNh
mov ax,2F18h
call HelperHexStringNh
mov ax,3A1Ch
call HelperHexStringNh
add rsi,32
add rbp,82
dec r12d
jnz .scanLines
mov rdi,rbp
mov ax,0A0Dh
stosw
mov rbp,rdi
inc r14d
pop rcx
cmp rsi,rcx
jb .scanBlocks
.affCpuidDataDone:
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textAffCpuid.objectStop],rbp
.noAffCpuidData:
ret
