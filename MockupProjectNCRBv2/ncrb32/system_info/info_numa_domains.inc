;---------- Get NUMA domains information --------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoNumaDomains:
push eax
mov eax,[DYNA_IMPORT._GetNumaHighestNodeNumber]
test eax,eax
jz .skipNumaList
mov ecx,esp
push ecx               ; Parm#1 = Pointer to output variable
call eax
test eax,eax
jz .error              ; dword [esp] = Maximum node number
mov edi,ebp            ; RDI = Pointer, RBP = Start
xor ebx,ebx            ; EBX = Node number and compare pattern
cmp [DYNA_IMPORT._GetNumaNodeProcessorMaskEx],ebx
je .standardTry
cmp [DYNA_IMPORT._GetNumaAvailableMemoryNodeEx],ebx
je .standardTry
mov eax,1         ; dword 00000001h = marker for extended topology scan results
stosd
.extendedNumaList:
mov [edi],ebx
lea eax,[edi + 4]
push eax               ; Parm#2 = Pointer to output variable: group + mask
push ebx               ; Parm#1 = Node 
call [DYNA_IMPORT._GetNumaNodeProcessorMaskEx]
test eax,eax
jz @f
lea eax,[edi + 20]     
push eax               ; Parm#2 = Pointer to output variable: memory size
push ebx               ; Parm#1 = Node 
call [DYNA_IMPORT._GetNumaAvailableMemoryNodeEx]
test eax,eax
jz @f
add edi,28
@@:
inc ebx
cmp ebx,[esp]
jbe .extendedNumaList 
jmp .doneNumaList
.standardTry:
cmp [DYNA_IMPORT._GetNumaNodeProcessorMask],ebx
je .skipNumaList
cmp [DYNA_IMPORT._GetNumaAvailableMemoryNode],ebx
je .skipNumaList
xor eax,eax       ; dword 00000000h = marker for standard topology scan results
stosd
.standardNumaList:
mov [edi],ebx
lea eax,[edi + 4]
push eax               ; Parm#2 = Pointer to output variable: mask
push ebx               ; Parm#1 = Node 
call [DYNA_IMPORT._GetNumaNodeProcessorMask]
test eax,eax
jz @f
lea eax,[edi + 12]    
push eax               ; Parm#2 = Pointer to output variable: memory size
push ebx               ; Parm#1 = Node 
call [DYNA_IMPORT._GetNumaAvailableMemoryNode]
test eax,eax
jz @f
add edi,20
@@:
inc ebx
cmp ebx,[esp]
jbe .standardNumaList 
.doneNumaList:
mov [DYNA_PTR.listNuma.objectStart],ebp
mov [DYNA_PTR.listNuma.objectStop],edi
mov ebp,edi
.skipNumaList:
pop eax
ret
.error:
stc
pop eax
ret
;--- Build NUMA domains information for text report and GUI widgets -----------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextNumaDomains:
mov edx,[DYNA_PTR.listNuma.objectStart]
mov ecx,[DYNA_PTR.listNuma.objectStop]
test ecx,ecx
jz .noNuma
mov [DYNA_PTR.textNuma.objectStart],ebp
push esi
mov esi,edx
lodsd
test eax,eax
jnz .numaExt
.numaStd:
cmp esi,ecx
jae .numaDoneStd
lodsd
call HelperNumberString
lea edx,[ebp - 82 - 02]
call HelperAffinity
lea edi,[ebp - 82 + 43]
mov bl,2
mov eax,[esi + 08]
mov edx,[esi + 12]
call SizePrint64
add esi,16
inc ebx
jmp .numaStd
.numaDoneStd:
mov al,0
stosb
push edi
mov esi,[DYNA_PTR.listNuma.objectStart]
lodsd  ; this for ADD ESI,4
lea edi,[BIND_LIST]  ; TODO. Optimize add.
add edi,BINDLIST.bindSys.masksList
xor edx,edx
.masksStd:
cmp esi,ecx
jae .numaDone
test edx,edx
jz .firstMaskStd
cmp edx,4
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
mov eax,[esi + 04]
call HexPrint32
mov al,'h'
stosb
.skipMaskStd:
add esi,20
inc edx
jmp .masksStd
.numaExt:
cmp esi,ecx
jae .numaDoneExt
lodsd
call HelperNumberString
lea edx,[ebp - 82 - 02]
mov al,0
call HelperAffinityGroup
lea edi,[ebp - 82 + 43]
mov bl,2
mov eax,[esi + 16]
mov edx,[esi + 20]
call SizePrint64
add esi,24
jmp .numaExt
.numaDoneExt:
mov al,0
stosb
push edi
mov esi,[DYNA_PTR.listNuma.objectStart]
lodsd  ; this for ADD ESI,4
lea edi,[BIND_LIST]  ;  TODO. Optimize add.
add edi,BINDLIST.bindSys.masksList
xor edx,edx
.masksExt:
cmp esi,ecx
jae .numaDone
test edx,edx
jz .firstMaskExt
cmp edx,4
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
mov eax,[esi + 04]
call HexPrint32
mov al,'h'
stosb
.skipMaskExt:
add esi,28
inc edx
jmp .masksExt
.numaDone:
mov al,0
stosb
pop edi esi
mov ebp,edi
mov [DYNA_PTR.textNuma.objectStop],ebp
.noNuma:
ret

