;---------- Get Topology information ------------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoTopology:
sub rsp,32 + 8
mov rbp,[APP_MEMORY]
call HelperAlignment
mov rbx,[rsi + DYNAIMPORT._GetLogicalProcessorInformation]
test rbx,rbx
jz @f
mov dword [r13],0            ; dword [r13]  = Buffer size variable, stack
mov rdx,r13                  ; RDX = Parm#2 = Pointer to buffer size variable 
mov rcx,rbp                  ; RCX = Parm#1 = Pointer to buffer 
call rbx
test rax,rax
jnz .error                   ; Go if unexpected error: zero-sized buffer
call [GetLastError]
cmp rax,ERROR_BUFFER_LIMIT
jne .error                   ; Go if other errors detected
mov r14d,[r13]
cmp r14d,VALUE_BUFFER_LIMIT
ja .error                    ; Go if required buffer size too big 
mov rdx,r13                  ; RDX = Parm#2 = Pointer to buffer size variable 
mov rcx,rbp                  ; RCX = Parm#1 = Pointer to buffer 
call rbx
test rax,rax
jz .error                    ; Go if unexpected error: zero-sized buffer
mov [DYNA_PTR.listTopology.objectStart],rbp
add rbp,r14
mov [DYNA_PTR.listTopology.objectStop],rbp
@@:
add rsp,32 + 8
ret
.error:
add rsp,32 + 8
stc
ret
;--- Build Platform topology information for text report and GUI widgets ------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextTopology:
;
TEMP_THREADS   EQU  dword [rsp + 00 + 08]
TEMP_CORES     EQU  dword [rsp + 04 + 08]
TEMP_SOCKETS   EQU  dword [rsp + 08 + 08]
TEMP_RESERVED  EQU  dword [rsp + 12 + 08]   
;
mov [DYNA_PTR.textTopology1.objectStart],rbp
mov rsi,[DYNA_PTR.listTopology.objectStart]
mov rcx,[DYNA_PTR.listTopology.objectStop]
sub rcx,rsi
test cl,00011111b
jnz .error
shr ecx,5
jz .error
push rsi rcx 0 0  ; Include local variables
mov rdi,rbp
.scanRelations:
mov rdx,rdi
push rcx
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop rcx
push rdi
mov eax,[rsi + 08]
cmp eax,3
ja .relationUnknown
je .relationPackage
cmp al,2
je .relationCache
cmp al,1
je .relationNuma
cmp al,0
je .relationCore
.relationUnknown:
mov ax,STR_TP_UNKNOWN_ID
call HelperRelationName 
jmp .doneRelation
.relationCore:
inc TEMP_CORES
inc TEMP_THREADS
cmp byte [rsi + 12],1
jne @f
inc TEMP_THREADS
@@:
mov ax,STR_TP_CPU_CORE
call HelperRelationName 
call HelperAffinity
mov eax,( STR_TP_HT SHL 16 ) + 2910h
call HelperParameter8
jmp .doneRelation
.relationNuma: 
mov ax,STR_TP_NUMA_NODE
call HelperRelationName 
call HelperAffinity
mov eax,( STR_TP_NODE SHL 16 ) + 2910h
call HelperParameter32
jmp .doneRelation
.relationCache:
mov ax,STR_TP_L
call HelperRelationName
mov al,[rsi + 16]
cmp al,1
jb .levelBad
cmp al,4
ja .levelBad
or al,30h
jmp .levelStore
.levelBad:
mov al,'?'
.levelStore:
mov ah,' '
stosw
mov eax,[rsi + 24]
cmp eax,3
ja .typeBad
add ax,STR_TP_UNIFIED
jmp .typeStore
.typeBad:
mov ax,STR_TP_UNKNOWN_TYPE
.typeStore:
lea rdi,[rdx + 04] 
call RelationNameEntry
call HelperAffinity
mov eax,( STR_TP_CACHE_WAYS SHL 16 ) + 2911h
call HelperParameter8
mov eax,( STR_TP_CACHE_LINE SHL 16 ) + 3212h
call HelperParameter16
lea rdi,[rdx + 3Fh]
push rcx rsi
mov eax,[rsi + 14h]
push rax
mov ax,STR_TP_CACHE_SIZE
call PoolStringWrite
mov bl,0
pop rax
call DecimalPrint32
pop rsi rcx
jmp .doneRelation 
.relationPackage:
inc TEMP_SOCKETS
mov ax,STR_TP_CPU_PACKAGE
call HelperRelationName 
call HelperAffinity
.doneRelation:
add rsi,32
pop rdi
dec ecx
jnz .scanRelations
pop r8 r9 rcx rsi
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textTopology1.objectStop],rbp
;---------- Support screen 0 threads, cores, sockets --------------------------;
lea rax,[SYS_PARMS.summaryTopology]
mov qword [rax + SUMMARYTOPOLOGY.threads],r8
mov [rax + SUMMARYTOPOLOGY.sockets],r9d
;---------- Build text block for topology screen, second block = summary ------;
; TODO. OPTIMIZE BY ELIMINATE DUAL ADDRESS CALCULATION: FOR "N/A" AND FOR VALUES
push rsi rcx
lea rcx,[BIND_LIST]
lea rdx,[rcx + BINDLIST.bindCache]
mov ax,STR_N_A
lea rdi,[rdx + BINDCACHE.l1c]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDCACHE.l1d]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDCACHE.l2u]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDCACHE.l3u]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDCACHE.l4u]
call PoolStringWrite
pop rcx rsi
mov [DYNA_PTR.textTopology2.objectStart],rbp
mov rdi,rbp
mov ax,0101h                 ; Instruction cache , L1
call HelperCacheSummary 
mov ax,0201h                 ; Data cache , L1
call HelperCacheSummary 
mov ax,0002h                 ; Unified cache , L2
call HelperCacheSummary 
mov ax,0003h                 ; Unified cache , L3
call HelperCacheSummary 
mov ax,0004h                 ; Unified cache , L4
call HelperCacheSummary 
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textTopology2.objectStop],rbp
ret

.error:
stc
ret
