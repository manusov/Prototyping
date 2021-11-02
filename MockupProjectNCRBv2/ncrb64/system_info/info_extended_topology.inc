;---------- Get Extended Topology information ---------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoExtendedTopology:

call HelperAlignment
mov rbx,[rsi + DYNAIMPORT._GetLogicalProcessorInformationEx]
test rbx,rbx
jz @f
xor r14d,r14d
mov al,0                  ; 0 = RelationProcessorCore
call HelperTopologyEx
jc .errorTopologyEx
mov al,2                  ; 2 = RelationCache 
call HelperTopologyEx
jc .errorTopologyEx
mov al,3                  ; 3 = RelationProcessorPackage
call HelperTopologyEx
jc .errorTopologyEx
mov al,1                  ; 1 = RelationNumaNode
call HelperTopologyEx
jc .errorTopologyEx
mov al,4                  ; 4 = RelationGroup
call HelperTopologyEx
.errorTopologyEx:
jc .error
mov [DYNA_PTR.listTopologyEx.objectStart],rbp
add rbp,r14
mov [DYNA_PTR.listTopologyEx.objectStop],rbp
@@:
ret
.error:
stc
ret



;--- Build Platform ext. topology inform. for text report and GUI widgets -----;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextExtendedTopology:
;
TEMP_THREADS   EQU  dword [rsp + 00 + 08]
TEMP_CORES     EQU  dword [rsp + 04 + 08]
TEMP_SOCKETS   EQU  dword [rsp + 08 + 08]
TEMP_RESERVED  EQU  dword [rsp + 12 + 08]   
;
mov [DYNA_PTR.textTopologyEx1.objectStart],rbp
mov rsi,[DYNA_PTR.listTopologyEx.objectStart]
mov rcx,[DYNA_PTR.listTopologyEx.objectStop]
test rcx,rcx
jz .noTopologyEx
push rsi rcx 0 0
mov rdi,rbp
.scanRelationsEx:
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
mov eax,[rsi + 00]
cmp eax,4
ja .relationUnknownEx
je .relationGroupEx
cmp al,3
je .relationPackageEx
cmp al,2
je .relationCacheEx
cmp al,1
je .relationNumaEx
cmp al,0
je .relationCoreEx
.relationUnknownEx:
mov ax,STR_TP_UNKNOWN_ID
call HelperRelationName 
jmp .doneRelationEx
.relationCoreEx:
inc TEMP_CORES
inc TEMP_THREADS
cmp byte [rsi + 08],1
jne @f
inc TEMP_THREADS
@@:
mov ax,STR_TP_CPU_CORE
call HelperRelationName 
mov al,32
call HelperAffinityGroup
mov eax,( STR_ET_SMT SHL 16 ) + 2908h
call HelperParameter8
mov eax,( STR_ET_EFFICIENCY SHL 16 ) + 3209h
call HelperParameter8
jmp .doneRelationEx
.relationNumaEx: 
mov ax,STR_TP_NUMA_NODE
call HelperRelationName 
mov al,32
call HelperAffinityGroup
mov eax,( STR_TP_NODE SHL 16 ) + 2908h
call HelperParameter32
jmp .doneRelationEx
.relationCacheEx:
mov ax,STR_TP_L
call HelperRelationName
mov al,[rsi + 08]
cmp al,1
jb .levelBadEx
cmp al,4
ja .levelBadEx
or al,30h
jmp .levelStoreEx
.levelBadEx:
mov al,'?'
.levelStoreEx:
mov ah,' '
stosw
mov eax,[rsi + 16]
cmp eax,3
ja .typeBadEx
add ax,STR_TP_UNIFIED
jmp .typeStoreEx
.typeBadEx:
mov ax,STR_TP_UNKNOWN_TYPE
.typeStoreEx:
lea rdi,[rdx + 04] 
call RelationNameEntry
mov al,40
call HelperAffinityGroup
mov eax,( STR_TP_CACHE_WAYS SHL 16 ) + 2909h
call HelperParameter8
mov eax,( STR_TP_CACHE_LINE SHL 16 ) + 320Ah
call HelperParameter16
lea rdi,[rdx + 3Fh]
push rcx rsi
mov eax,[rsi + 0Ch]
push rax
mov ax,STR_TP_CACHE_SIZE
call PoolStringWrite
mov bl,0
pop rax
call DecimalPrint32
pop rsi rcx
jmp .doneRelationEx 
.relationPackageEx:
inc TEMP_SOCKETS
mov ax,STR_TP_CPU_PACKAGE
call HelperRelationName 
mov al,32
call HelperAffinityGroup
mov eax,( STR_ET_SMT SHL 16 ) + 2908h
call HelperParameter8
mov eax,( STR_ET_EFFICIENCY SHL 16 ) + 3209h
call HelperParameter8
jmp .doneRelationEx
.relationGroupEx: 
mov ax,STR_ET_GROUP
call HelperRelationName 
mov al,32
call HelperAffinityMask
.doneRelationEx:
pop rdi
mov eax,[rsi + 04]
add rsi,rax
cmp rsi,rcx
jb .scanRelationsEx
pop r8 r9 rcx rsi
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textTopologyEx1.objectStop],rbp
;---------- Support screen 0 threads, cores, sockets ( variables ) ------------;
lea rax,[SYS_PARMS.summaryTopology]
test r8d,r8d
jz @f
mov [rax + SUMMARYTOPOLOGY.threads],r8d
@@:
shr r8,32
jz @f
mov [rax + SUMMARYTOPOLOGY.cores],r8d
@@:
test r9d,r9d
jz @f
mov [rax + SUMMARYTOPOLOGY.sockets],r9d
@@:
.noTopologyEx:
;---------- Support screen 0 threads, cores, sockets ( text ) -----------------;
lea rdx,[BIND_LIST]
lea r8,[SYS_PARMS.summaryTopology]
mov bl,0
lea rdi,[rdx + BINDLIST.bindTopology.threads]
mov eax,[r8 + SUMMARYTOPOLOGY.threads]
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDLIST.bindTopology.cores]
mov eax,[r8 + SUMMARYTOPOLOGY.cores]
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDLIST.bindTopology.sockets]
mov eax,[r8 + SUMMARYTOPOLOGY.sockets]
call DecimalPrint32
mov al,0
stosb
;--- Build text block for extended topology screen, second block = summary ----;
mov [DYNA_PTR.textTopologyEx2.objectStart],rbp
mov rdi,rbp
mov ax,0101h                 ; Instruction cache , L1
call HelperCacheSummaryEx 
mov ax,0201h                 ; Data cache , L1
call HelperCacheSummaryEx 
mov ax,0002h                 ; Unified cache , L2
call HelperCacheSummaryEx 
mov ax,0003h                 ; Unified cache , L3
call HelperCacheSummaryEx 
mov ax,0004h                 ; Unified cache , L4
call HelperCacheSummaryEx 
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textTopologyEx2.objectStop],rbp
ret

