;---------- Get Extended Topology information ---------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoExtendedTopology:
call HelperAlignment
mov ebx,[DYNA_IMPORT._GetLogicalProcessorInformationEx]
test ebx,ebx
jz @f
xor edi,edi
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
mov [DYNA_PTR.listTopologyEx.objectStart],ebp
add ebp,edi
mov [DYNA_PTR.listTopologyEx.objectStop],ebp
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
TEMP_THREADS   EQU  dword [esp + 00 + 04]
TEMP_CORES     EQU  dword [esp + 04 + 04]
TEMP_SOCKETS   EQU  dword [esp + 08 + 04]
;
mov [DYNA_PTR.textTopologyEx1.objectStart],ebp
mov ebx,[DYNA_PTR.listTopologyEx.objectStart]
mov ecx,[DYNA_PTR.listTopologyEx.objectStop]
test ecx,ecx
jz .noTopologyEx
mov edi,ebp
push ebx ecx esi 0 0 0
mov esi,ebx
.scanRelationsEx:
mov edx,edi
push ecx
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop ecx
push edi
mov eax,[esi + 00]
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
cmp byte [esi + 8],1
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
mov al,[esi + 08]
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
mov eax,[esi + 16]
cmp eax,3
ja .typeBadEx
add ax,STR_TP_UNIFIED
jmp .typeStoreEx
.typeBadEx:
mov ax,STR_TP_UNKNOWN_TYPE
.typeStoreEx:
lea edi,[edx + 04] 
call RelationNameEntry
mov al,40
call HelperAffinityGroup
mov eax,( STR_TP_CACHE_WAYS SHL 16 ) + 2909h
call HelperParameter8
mov eax,( STR_TP_CACHE_LINE SHL 16 ) + 320Ah
call HelperParameter16
lea edi,[edx + 3Fh]
push ecx esi
mov eax,[esi + 0Ch]
push eax
mov ax,STR_TP_CACHE_SIZE
call PoolStringWrite
mov bl,0
pop eax
call DecimalPrint32
pop esi ecx
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
pop edi
add esi,[esi + 04]
cmp esi,ecx
jb .scanRelationsEx
;---------- Support screen 0 threads, cores, sockets ( variables ) ------------;
mov esi,[esp + 12]
lea ebx,[SYS_PARMS.summaryTopology]
pop ecx
jecxz @f
mov [ebx + SUMMARYTOPOLOGY.threads],ecx
@@:
pop ecx
jecxz @f
mov [ebx + SUMMARYTOPOLOGY.cores],ecx
@@:
pop ecx
jecxz @f
mov [ebx + SUMMARYTOPOLOGY.sockets],ecx
@@:
;---------- End of support screen 0 threads, cores, sockets -------------------;
pop esi ecx ebx
mov al,0
stosb
mov ebp,edi
mov [DYNA_PTR.textTopologyEx1.objectStop],ebp
.noTopologyEx:
;---------- Support screen 0 threads, cores, sockets ( text ) -----------------;
push ebx ecx
lea edx,[BIND_LIST]
lea ecx,[SYS_PARMS.summaryTopology]
mov bl,0
lea edi,[edx + BINDLIST.bindTopology.threads]
mov eax,[ecx + SUMMARYTOPOLOGY.threads]
call DecimalPrint32
mov al,0
stosb
lea edi,[edx + BINDLIST.bindTopology.cores]
mov eax,[ecx + SUMMARYTOPOLOGY.cores]
call DecimalPrint32
mov al,0
stosb
lea edi,[edx + BINDLIST.bindTopology.sockets]
mov eax,[ecx + SUMMARYTOPOLOGY.sockets]
call DecimalPrint32
mov al,0
stosb
pop ecx ebx
;--- Build text block for extended topology screen, second block = summary ----;
mov [DYNA_PTR.textTopologyEx2.objectStart],ebp
push esi
mov edi,ebp
mov esi,ebx
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
mov ebp,edi
pop esi
mov [DYNA_PTR.textTopologyEx2.objectStop],ebp
ret

