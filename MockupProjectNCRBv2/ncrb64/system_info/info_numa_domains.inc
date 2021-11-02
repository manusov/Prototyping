;---------- Get NUMA domains information --------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoNumaDomains:
sub rsp,32 + 8
mov rax,[rsi + DYNAIMPORT._GetNumaHighestNodeNumber]
test rax,rax
jz .skipNumaList
mov rcx,r13           ; RCX = Parm#1 = Pointer to output variable
call rax
test rax,rax
jz .error
mov r14,rdi           ; Can't PUSH because parameter shadow for functions
mov rdi,rbp           ; RDI = Pointer, RBP = Start
xor ebx,ebx           ; RBX = Node number and compare pattern
mov r12d,[r13]        ; R12 = Maximum node number
cmp [rsi + DYNAIMPORT._GetNumaNodeProcessorMaskEx],rbx
je .standardTry
cmp [rsi + DYNAIMPORT._GetNumaAvailableMemoryNodeEx],rbx
je .standardTry
mov eax,1         ; dword 00000001h = marker for extended topology scan results
stosd
.extendedNumaList:
mov [rdi],ebx
lea rdx,[rdi + 4]     ; RDX = Parm#2 = Pointer to output variable: group + mask
mov ecx,ebx           ; RCX = Parm#1 = Node 
call [rsi + DYNAIMPORT._GetNumaNodeProcessorMaskEx]
test rax,rax
jz @f
lea rdx,[rdi + 20]    ; RDX = Parm#2 = Pointer to output variable: memory size
mov ecx,ebx           ; RCX = Parm#1 = Node 
call [rsi + DYNAIMPORT._GetNumaAvailableMemoryNodeEx]
test rax,rax
jz @f
add rdi,28
@@:
inc ebx
cmp ebx,r12d
jbe .extendedNumaList 
jmp .doneNumaList
.standardTry:
cmp [rsi + DYNAIMPORT._GetNumaNodeProcessorMask],rbx
je .skipNumaList
cmp [rsi + DYNAIMPORT._GetNumaAvailableMemoryNode],rbx
je .skipNumaList
xor eax,eax       ; dword 00000000h = marker for standard topology scan results
stosd
.standardNumaList:
mov [rdi],ebx
lea rdx,[rdi + 4]     ; RDX = Parm#2 = Pointer to output variable: mask
mov ecx,ebx           ; RCX = Parm#1 = Node 
call [rsi + DYNAIMPORT._GetNumaNodeProcessorMask]
test rax,rax
jz @f
lea rdx,[rdi + 12]    ; RDX = Parm#2 = Pointer to output variable: memory size
mov ecx,ebx           ; RCX = Parm#1 = Node 
call [rsi + DYNAIMPORT._GetNumaAvailableMemoryNode]
test rax,rax
jz @f
add rdi,20
@@:
inc ebx
cmp ebx,r12d
jbe .standardNumaList 
.doneNumaList:
mov [DYNA_PTR.listNuma.objectStart],rbp
mov [DYNA_PTR.listNuma.objectStop],rdi
mov rbp,rdi
mov rdi,r14
.skipNumaList:
add rsp,32 + 8
ret
.error:
add rsp,32 + 8
stc
ret
;--- Build NUMA domains information for text report and GUI widgets -----------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextNumaDomains:
mov rsi,[DYNA_PTR.listNuma.objectStart]
mov rcx,[DYNA_PTR.listNuma.objectStop]
test rcx,rcx
jz .noNuma
mov [DYNA_PTR.textNuma.objectStart],rbp
lodsd
test eax,eax
jnz .numaExt
.numaStd:
cmp rsi,rcx
jae .numaDoneStd
lodsd
call HelperNumberString
lea rdx,[rbp - 82 - 02]
call HelperAffinity
lea rdi,[rbp - 82 + 43]
mov bl,2
mov rax,[rsi + 08]
call SizePrint64
add rsi,16
inc ebx
jmp .numaStd
.numaDoneStd:
mov al,0
stosb
push rdi
mov rsi,[DYNA_PTR.listNuma.objectStart]
lodsd  ; this for ADD RSI,4
lea rdi,[BIND_LIST]
add rdi,BINDLIST.bindSys.masksList
xor edx,edx
.masksStd:
cmp rsi,rcx
jae .numaDone
test edx,edx
jz .firstMaskStd
cmp edx,2
jb .nextMaskStd
ja .skipMaskStd
mov eax,', ..'
stosd
mov al,'.'
stosb
jmp .skipMaskStd
.nextMaskStd:
mov ax,', '
stosw
.firstMaskStd:
mov rax,[rsi + 04]
call HexPrint64
mov al,'h'
stosb
.skipMaskStd:
add rsi,20
inc edx
jmp .masksStd
.numaExt:
cmp rsi,rcx
jae .numaDoneExt
lodsd
call HelperNumberString
lea rdx,[rbp - 82 - 02]
mov al,0
call HelperAffinityGroup
lea rdi,[rbp - 82 + 43]
mov bl,2
mov rax,[rsi + 16]
call SizePrint64
add rsi,24
jmp .numaExt
.numaDoneExt:
mov al,0
stosb
push rdi
mov rsi,[DYNA_PTR.listNuma.objectStart]
lodsd  ; this for ADD RSI,4
lea rdi,[BIND_LIST]
add rdi,BINDLIST.bindSys.masksList
xor edx,edx
.masksExt:
cmp rsi,rcx
jae .numaDone
test edx,edx
jz .firstMaskExt
cmp edx,2
jb .nextMaskExt
ja .skipMaskExt 
mov eax,', ..'
stosd
mov al,'.'
stosb
jmp .skipMaskExt 
.nextMaskExt:
mov ax,', '
stosw
.firstMaskExt:
mov rax,[rsi + 04]
call HexPrint64
mov al,'h'
stosb
.skipMaskExt:
add rsi,28
inc edx
jmp .masksExt
.numaDone:
mov al,0
stosb
pop rdi
mov rbp,rdi
mov [DYNA_PTR.textNuma.objectStop],rbp
.noNuma:
ret

