;---------- Get Topology information ------------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoTopology:
push eax                    ; for create variable
mov ebp,[APP_MEMORY]
call HelperAlignment
mov ebx,[DYNA_IMPORT._GetLogicalProcessorInformation]
test ebx,ebx
jz @f
mov dword [esp],0            ; dword [esp]  = Buffer size variable, stack
mov eax,esp
push eax                     ; Parm#2 = Pointer to buffer size variable 
push ebp                     ; Parm#1 = Pointer to buffer 
call ebx
test eax,eax
jnz .error                   ; Go if unexpected error: zero-sized buffer
call [GetLastError]
cmp eax,ERROR_BUFFER_LIMIT
jne .error                   ; Go if other errors detected
mov edi,[esp]
cmp edi,VALUE_BUFFER_LIMIT
ja .error                    ; Go if required buffer size too big
mov eax,esp 
push eax                     ; Parm#2 = Pointer to buffer size variable 
push ebp                     ; Parm#1 = Pointer to buffer 
call ebx
test eax,eax
jz .error                    ; Go if unexpected error: zero-sized buffer
mov [DYNA_PTR.listTopology.objectStart],ebp
add ebp,edi
mov [DYNA_PTR.listTopology.objectStop],ebp
@@:
pop eax
ret
.error:
pop eax
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
; TODO. REDESIGN CYCLE WITH MORE EFFECTIVE CPU REGISTERS USAGE,
; WITHOUT STACK VARIABLES.
TEMP_THREADS   EQU  dword [esp + 00 + 04]
TEMP_CORES     EQU  dword [esp + 04 + 04]
TEMP_SOCKETS   EQU  dword [esp + 08 + 04]
;
mov [DYNA_PTR.textTopology1.objectStart],ebp
mov ebx,[DYNA_PTR.listTopology.objectStart]
mov eax,[DYNA_PTR.listTopology.objectStop]
sub eax,ebx
mov ecx,24
xor edx,edx
div ecx
test edx,edx
jnz .error
xchg ecx,eax
test ecx,ecx
jz .error
mov edi,ebp
push ebx ecx esi 0 0 0   ; Include local variables
mov esi,ebx
.scanRelations:
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
mov eax,[esi + 04]
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
cmp byte [esi + 8],1
jne @f
inc TEMP_THREADS
@@:
mov ax,STR_TP_CPU_CORE
call HelperRelationName 
call HelperAffinity
mov eax,( STR_TP_HT SHL 16 ) + 2908h
call HelperParameter8
jmp .doneRelation
.relationNuma: 
mov ax,STR_TP_NUMA_NODE
call HelperRelationName 
call HelperAffinity
mov eax,( STR_TP_NODE SHL 16 ) + 2908h
call HelperParameter32
jmp .doneRelation
.relationCache:
mov ax,STR_TP_L
call HelperRelationName
mov al,[esi + 08]
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
mov eax,[esi + 16]
cmp eax,3
ja .typeBad
add ax,STR_TP_UNIFIED
jmp .typeStore
.typeBad:
mov ax,STR_TP_UNKNOWN_TYPE
.typeStore:
lea edi,[edx + 04] 
call RelationNameEntry
call HelperAffinity
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
jmp .doneRelation 
.relationPackage:
inc TEMP_SOCKETS
mov ax,STR_TP_CPU_PACKAGE
call HelperRelationName 
call HelperAffinity
.doneRelation:
add esi,24
pop edi
dec ecx
jnz .scanRelations
;---------- Support screen 0 threads, cores, sockets --------------------------;
mov esi,[esp + 12]
lea ebx,[SYS_PARMS.summaryTopology]
pop dword [ebx + SUMMARYTOPOLOGY.threads]
pop dword [ebx + SUMMARYTOPOLOGY.cores]
pop dword [ebx + SUMMARYTOPOLOGY.sockets]
;---------- End of support screen 0 threads, cores, sockets -------------------;
pop esi ecx ebx
mov al,0
stosb
mov ebp,edi
mov [DYNA_PTR.textTopology1.objectStop],ebp
;---------- Build text block for topology screen, second block = summary ------;
; TODO. OPTIMIZE BY ELIMINATE DUAL ADDRESS CALCULATION: FOR "N/A" AND FOR VALUES
push esi ecx
lea ecx,[BIND_LIST]
lea edx,[ecx + BINDLIST.bindCache]
mov ax,STR_N_A
lea edi,[edx + BINDCACHE.l1c]
push eax
call PoolStringWrite
pop eax
lea edi,[edx + BINDCACHE.l1d]
push eax
call PoolStringWrite
pop eax
lea edi,[edx + BINDCACHE.l2u]
push eax
call PoolStringWrite
pop eax
lea edi,[edx + BINDCACHE.l3u]
push eax
call PoolStringWrite
pop eax
lea edi,[edx + BINDCACHE.l4u]
call PoolStringWrite
pop ecx esi
mov [DYNA_PTR.textTopology2.objectStart],ebp
push esi
mov edi,ebp
mov esi,ebx
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
mov ebp,edi
pop esi
mov [DYNA_PTR.textTopology2.objectStop],ebp
ret
.error:
stc
ret
