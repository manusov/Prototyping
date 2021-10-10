;------------------------------------------------------------------------------;
;                                                                              ;
;        Scan system configuration and store results t system registry.        ;
;             This procedure for user mode resources, x64 version.             ;
;                                                                              ;
; INPUT:   Application registry information,                                   ;
;          unformatted part of registry and yet non initialized structures     ;
;          must be pre-blanked (contain 0) before this procedure call.         ;
;                                                                              ;
; OUTPUT:  Application registry information                                    ; 
;                                                                              ;
; Can destroy registers, volatile by Microsoft ia32 calling convention.        ;
;------------------------------------------------------------------------------;

SysinfoUserMode:
push ebx esi edi ebp
push 0                  ; Variable dword at [esp], if original ESP, no pushes
cld
mov esi,[Registry]      ; ESI = Registry base address

;---------- Check CPUID instruction support, get 12-chars vendor string -------;

lea edi,[esi + REGISTRY32.cpuData.vendorString]
call HelperCheckCpuid
jc .error
cmp eax,1
jb .error

;---------- Get 48-chars CPU model string -------------------------------------;

call HelperGetCpuName

;---------- Get CPU signature: type, family, model, stepping ------------------;

mov eax,1
cpuid
stosd

;------- Build CPU common features bitmap, for system information screen ------; 

push esi
mov ebx,esi
xor eax,eax
mov esi,[ebx + REGISTRY32.appData.lockedDataCpuCommon]
test esi,esi
jz @f
call HelperBuildBitmap
@@:
stosd
mov eax,80000000h
stosd

;------- Build CPU AVX512 features bitmap, for system information screen ------;

xor eax,eax
mov esi,[ebx + REGISTRY32.appData.lockedDataCpuAvx512]
test esi,esi
jz @f
call HelperBuildBitmap
@@:
stosd
xor eax,eax
stosd

;------- Build OS context features bitmap, for system information screen ------;

xor eax,eax
mov esi,[ebx + REGISTRY32.appData.lockedDataOsContext]
test esi,esi
jz @f
call HelperBuildBitmap
@@:
xor edx,edx
test eax,eax
jz @f
bts edx,31
@@:
stosd
xchg eax,edx
stosd

;------- Build CPU methods features bitmap, for system information screen -----;

xor eax,eax    ; *** DEBUG ***
stosd          ; *** DEBUG ***
stosd          ; *** DEBUG ***
pop esi

;---------- Measure TSC frequency ---------------------------------------------;

call HelperMeasureTsc
; TODO. Check for errors, CF=1

;---------- Initializing dynamical import WinAPI list -------------------------;

push esi
mov ebx,[esi + REGISTRY32.appData.lockedImportList]  ; EBX = to functions names
lea edi,[esi + REGISTRY32.dynaImport]                ; EDI = to fnc. addresses
lea esi,[NameKernel32]      ; ESI = Pointer to libraries names array
.scanLibs:
cmp byte [esi],0
je .doneLibs
push esi                    ; Parm#1 = Pointer to module name
call [GetModuleHandle]      ; EAX = Return module handle
xchg ebp,eax                ; EBP = DLL handle, XCHG is compact
.scanFnc:
cmp byte [ebx],0
je .doneFnc
push ebx                    ; Parm#2 = Pointer to function name
push ebp                    ; Parm#1 = Module handle
call [GetProcAddress]       ; EAX = Return function address
stosd                       ; Store function address to import list
.skipName:
inc ebx
cmp byte [ebx],0
jne .skipName
inc ebx
jmp .scanFnc                ; Internal cycle for functions list per one library
.doneFnc:
inc ebx
.skipLib:
lodsb
cmp al,0
jne .skipLib
jmp .scanLibs               ; External cycle for libraries list
.doneLibs:
pop esi

;---------- Check for WoW64 process: ia32 application under Win64 -------------;

lea edi,[esi + REGISTRY32.osData.isWow64]
mov ecx,[esi + REGISTRY32.dynaImport._IsWow64Process]
jecxz .noWow64
call [GetCurrentProcess]
test eax,eax
jz .noWow64
push edi                ; Parm#2 = Pointer to BOOL WoW64 Process
push eax                ; Parm#1 = hProcess handle 
call ecx                ; Call function: IsWow64Process 
test eax,eax
jnz .doneWow64          ; Go if WoW64 detected
.noWow64:
mov dword [edi],0       ; Clear WoW64 flag if WoW64 not detected  
.doneWow64: 

;---------- Get system and memory information parameters ----------------------;

lea ebx,[esi + REGISTRY32.dynaImport]
lea edi,[esi + REGISTRY32.osData]
mov eax,[ebx + DYNAIMPORT._GlobalMemoryStatusEx]
test eax,eax
jz .error
lea ecx,[edi + OSDATA.memoryStatusEx]
mov [ecx + MEMORYSTATUSEX_DEF.dwLength],sizeof.MEMORYSTATUSEX_DEF
push ecx
call eax 
test eax,eax
jz .error
lea ecx,[edi + OSDATA.systemInfo]
push ecx
call [GetSystemInfo]
mov eax,[ebx + DYNAIMPORT._GetActiveProcessorGroupCount]
test eax,eax
jz @f
call eax
movzx eax,ax
@@:
mov [edi + OSDATA.activeProcessorGroupCount],eax
mov eax,[ebx + DYNAIMPORT._GetActiveProcessorCount]
test eax,eax
jz @f
push ALL_PROCESSOR_GROUPS
call eax
@@:
mov [edi + OSDATA.activeProcessorCount],eax
mov eax,[ebx + DYNAIMPORT._GetNumaHighestNodeNumber]
test eax,eax
jz @f
lea ecx,[edi + OSDATA.numaNodeCount]
push ecx
call eax
test eax,eax
jz @f
mov eax,[edi + OSDATA.numaNodeCount]
inc eax
@@:               ; If no data, EAX=0 at this point, means no nodes found
mov [edi + OSDATA.numaNodeCount],eax

;---------- Get large page size -----------------------------------------------;

mov eax,[ebx + DYNAIMPORT._GetLargePageMinimum]
test eax,eax
jz @f
call eax
@@:
mov [edi + OSDATA.largePageSize],eax

;---------- Get large page information: try allocation ------------------------;

push esi
sub esp,128
xor eax,eax
mov [edi + OSDATA.largePageEnable],eax
cmp [ebx + DYNAIMPORT._OpenProcessToken],eax
je .exit
cmp [ebx + DYNAIMPORT._AdjustTokenPrivileges],eax 
je .exit
cmp [edi + OSDATA.largePageSize],eax 
je .exit
call [GetCurrentProcess]
test eax,eax
jz .skip
lea ebp,[esp + 120]
push ebp 
push MAXIMUM_ALLOWED
push eax
call [ebx + DYNAIMPORT._OpenProcessToken]
test eax,eax 
jz .skip   
xor eax,eax  
mov esi,esp
push eax
push eax
push eax
push esi 
push eax
push dword [ebp]
mov [tp.PrivilegeCount],1 
mov [tp.Privileges.Luid.usedpart],SE_LOCK_MEMORY_PRIVILEGE 
and [tp.Privileges.Luid.ignorehigh32bitpart],0  
mov [tp.Privileges.Attributes],SE_PRIVILEGE_ENABLED
call [ebx + DYNAIMPORT._AdjustTokenPrivileges] 
mov esi,eax 
push dword [ebp] 
call [CloseHandle] 
.skip:
test eax,eax
jz .exit
xor eax,eax
test esi,esi
jz .exit 
call [GetCurrentProcess]
test eax,eax
jz .exit
mov esi,eax
push PAGE_READWRITE
push MEM_COMMIT + MEM_LARGE_PAGES
push [edi + OSDATA.largePageSize]
push 0
push esi
call [VirtualAllocEx]
test eax,eax
jz @f
push MEM_RELEASE
push 0
push eax
push esi
call [VirtualFreeEx]
@@:
test eax,eax
setnz al
movzx eax,al
mov [edi + OSDATA.largePageEnable],eax 
.exit:
add esp,128
pop esi

;---------- Get native operating system information ---------------------------;

mov eax,[ebx + DYNAIMPORT._GetNativeSystemInfo]
test eax,eax
jz @f
lea ecx,[edi + OSDATA.nativeSystemInfo]
push ecx
call eax
@@:

;---------- Get platform topology by WinAPI -----------------------------------;
; Start operations with unformatted part of application registry,
; EBP = Pointer for unformatted part addressing 

lea ebp,[esi + REGISTRY32.unformatted]
call HelperAlignment
mov ebx,[esi + REGISTRY32.dynaImport._GetLogicalProcessorInformation]
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
mov [esi + REGISTRY32.listTopology.objectStart],ebp
add ebp,edi
mov [esi + REGISTRY32.listTopology.objectStop],ebp
@@:

;---------- Get platform extended topology by WinAPI --------------------------;
; Separate get each type of information for build ordered list.

call HelperAlignment
mov ebx,[esi + REGISTRY32.dynaImport._GetLogicalProcessorInformationEx]
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
mov [esi + REGISTRY32.listTopologyEx.objectStart],ebp
add ebp,edi
mov [esi + REGISTRY32.listTopologyEx.objectStop],ebp
@@:

;---------- Get NUMA nodes list -----------------------------------------------;

mov eax,[esi + REGISTRY32.dynaImport._GetNumaHighestNodeNumber]
test eax,eax
jz .skipNumaList
mov ecx,esp
push ecx               ; Parm#1 = Pointer to output variable
call eax
test eax,eax
jz .error              ; dword [esp] = Maximum node number
mov edi,ebp            ; RDI = Pointer, RBP = Start
xor ebx,ebx            ; EBX = Node number and compare pattern
cmp [esi + REGISTRY32.dynaImport._GetNumaNodeProcessorMaskEx],ebx
je .standardTry
cmp [esi + REGISTRY32.dynaImport._GetNumaAvailableMemoryNodeEx],ebx
je .standardTry
mov eax,1         ; dword 00000001h = marker for extended topology scan results
stosd
.extendedNumaList:
mov [edi],ebx
lea eax,[edi + 4]
push eax               ; Parm#2 = Pointer to output variable: group + mask
push ebx               ; Parm#1 = Node 
call [esi + REGISTRY32.dynaImport._GetNumaNodeProcessorMaskEx]
test eax,eax
jz @f
lea eax,[edi + 20]     
push eax               ; Parm#2 = Pointer to output variable: memory size
push ebx               ; Parm#1 = Node 
call [esi + REGISTRY32.dynaImport._GetNumaAvailableMemoryNodeEx]
test eax,eax
jz @f
add edi,28
@@:
inc ebx
cmp ebx,[esp]
jbe .extendedNumaList 
jmp .doneNumaList
.standardTry:
cmp [esi + REGISTRY32.dynaImport._GetNumaNodeProcessorMask],ebx
je .skipNumaList
cmp [esi + REGISTRY32.dynaImport._GetNumaAvailableMemoryNode],ebx
je .skipNumaList
xor eax,eax       ; dword 00000000h = marker for standard topology scan results
stosd
.standardNumaList:
mov [edi],ebx
lea eax,[edi + 4]
push eax               ; Parm#2 = Pointer to output variable: mask
push ebx               ; Parm#1 = Node 
call [esi + REGISTRY32.dynaImport._GetNumaNodeProcessorMask]
test eax,eax
jz @f
lea eax,[edi + 12]    
push eax               ; Parm#2 = Pointer to output variable: memory size
push ebx               ; Parm#1 = Node 
call [esi + REGISTRY32.dynaImport._GetNumaAvailableMemoryNode]
test eax,eax
jz @f
add edi,20
@@:
inc ebx
cmp ebx,[esp]
jbe .standardNumaList 
.doneNumaList:
mov [esi + REGISTRY32.listNuma.objectStart],ebp
mov [esi + REGISTRY32.listNuma.objectStop],edi
mov ebp,edi
.skipNumaList:

;---------- Get processor groups list -----------------------------------------;

xor ebx,ebx           ; EBX = Node number and compare pattern
cmp [esi + REGISTRY32.dynaImport._GetActiveProcessorGroupCount],ebx  
je .skipGroupList 
cmp [esi + REGISTRY32.dynaImport._GetActiveProcessorCount],ebx       
je .skipGroupList
call [esi + REGISTRY32.dynaImport._GetActiveProcessorGroupCount]
test ax,ax
jz .skipGroupList
mov [esp],ax 
mov edi,ebp           ; EDI = Pointer, EBP = Start
.groupList:
mov eax,ebx
stosd
push ebx              ; Parm#1 = Group number
call [esi + REGISTRY32.dynaImport._GetActiveProcessorCount]
; no check for EAX = 0, empty group is acceptable ?
stosd
inc ebx
cmp bx,[esp]
jb .groupList
.doneGroupList:
mov [esi + REGISTRY32.listGroup.objectStart],ebp
mov [esi + REGISTRY32.listGroup.objectStop],edi
mov ebp,edi
.skipGroupList:

;---------- Get ACPI tables list ----------------------------------------------;

xor eax,eax
cmp [esi + REGISTRY32.dynaImport._EnumSystemFirmwareTables],eax
je .skipAcpiList
cmp [esi + REGISTRY32.dynaImport._GetSystemFirmwareTable],eax
je .skipAcpiList
call HelperAlignment
push VALUE_BUFFER_LIMIT      ; Parm#3 = Buffer size limit
lea eax,[ebp + 16]
push eax                     ; Parm#2 = Pointer to buffer
push 'IPCA'                  ; Parm#1 = Signature, select data type = ACPI
call [esi + REGISTRY32.dynaImport._EnumSystemFirmwareTables]
test eax,eax
jz .error
cmp eax,VALUE_BUFFER_LIMIT
ja .error
mov ebx,ebp
lea edi,[ebp + 16 + eax]     ; EDI = Pointer to 36-byte entries list
mov ecx,edi                  ; ECX = Limit of 4-byte entries  
mov [ebx],edi
add ebx,16                   ; EBX = Pointer to 4-byte tables signatures list
imul eax,eax,9
lea edx,[edi + eax]          ; EDX = Pointer to buffer for read table
test dl,00001111b
jz .scanAcpiList
and dl,11110000b
add edx,10h 
.scanAcpiList:
push ecx edx
push VALUE_BUFFER_LIMIT      ; Parm#4 = Buffer size limit  
push edx                     ; Parm#3 = Buffer pointer
push dword [ebx]             ; Parm#2 = Table signature
push 'IPCA'                  ; Parm#1 = Table provider signature
call [esi + REGISTRY32.dynaImport._GetSystemFirmwareTable]
pop edx ecx
test eax,eax
jz .skipOneTable
cmp eax,VALUE_BUFFER_LIMIT
ja .skipOneTable 
push ecx esi
cld
mov ecx,9
mov esi,edx
rep movsd
pop esi ecx
.skipOneTable:
add ebx,4
cmp ebx,ecx
jb .scanAcpiList 
.doneAcpiList:
mov [esi + REGISTRY32.listAcpi.objectStart],ebp
mov [esi + REGISTRY32.listAcpi.objectStop],edi
mov ebp,edi
.skipAcpiList:

;---------- Get affinized CPUID dump ------------------------------------------;

mov edi,ebp
cmp [esi + REGISTRY32.dynaImport._SetThreadAffinityMask],0
je .skipAffCpuid
call [GetCurrentThread]
test eax,eax
jz .skipAffCpuid 
xchg ebx,eax             ; EBX = Current thread handle
mov ecx,[esi + REGISTRY32.osData.systemInfo.dwNumberOfProcessors]
jecxz .skipAffCpuid
cmp ecx,16              ; Yet limited for maximum 16 logical processors
jbe @f
mov ecx,16      
@@:                      ; ECX = Logical processors count
xor edx,edx              ; EDX = Storage for original affinity mask
mov eax,1                ; EAX = Variable affinity mask for dump
.affinizedDump:
push eax ecx edx
push eax                 ; Parm#2 = Affinity mask
push ebx                 ; Parm#1 = Current thread handle 
call [esi + REGISTRY32.dynaImport._SetThreadAffinityMask]
pop edx
test edx,edx
jnz @f
xchg edx,eax
@@:
push edx
call GetCPUID
mov eax,[edi]
shl eax,5
lea edi,[edi + eax + 32]
pop edx ecx eax
rol eax,1
push eax
bsf eax,eax
cmp eax,ecx
pop eax
jb .affinizedDump 
test edx,edx
jz @f
push edx                 ; Parm#2 = Affinity mask
push ebx                 ; Parm#1 = Current thread handle 
call [esi + REGISTRY32.dynaImport._SetThreadAffinityMask]
@@:
mov [esi + REGISTRY32.listAffCpuid.objectStart],ebp
mov [esi + REGISTRY32.listAffCpuid.objectStop],edi
mov ebp,edi
.skipAffCpuid:

;---------- Build text block for operating system information screen ----------;
; Start text blocks build operations.

mov [esi + REGISTRY32.textOs.objectStart],ebp
mov ax,STR_OS_MEMORY_LOAD
call HelperOsInfoString                          ; Memory load percentage
mov eax,[esi + REGISTRY32.osData.memoryStatusEx.dwMemoryLoad]
push eax
mov bl,0
call DecimalPrint32
mov ax,' %'
stosw
pop eax
mov edi,ecx
call HexPrint32 
mov ax,STR_OS_TOTAL_PHYSICAL
call HelperOsInfoString                          ; Total physical memory
mov al,MEMORYSTATUSEX_DEF.ullTotalPhys
call HelperOsInfoSize
mov ax,STR_OS_AVAIL_PHYSICAL                     ; Available physical memory
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullAvailPhys
call HelperOsInfoSize
mov ax,STR_OS_TOTAL_PAGE_FILE                    ; Total page file 
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullTotalPageFile
call HelperOsInfoSize
mov ax,STR_OS_AVAIL_PAGE_FILE                    ; Available page file
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullAvailPageFile
call HelperOsInfoSize
mov ax,STR_OS_TOTAL_VIRTUAL                      ; Total virtual user space
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullTotalVirtual
call HelperOsInfoSize
mov ax,STR_OS_AVAIL_VIRTUAL                      ; Available virtual user space
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullAvailVirtual
call HelperOsInfoSize
mov ax,STR_OS_EXT_VIRTUAL                        ; Extended virtual
call HelperOsInfoString
mov al,'-'
stosb
mov al,MEMORYSTATUSEX_DEF.ullAvailExtendedVirtual
call HelperOsInfoHex
mov edi,ebp
mov ax,0A0Dh                                     ; Interval
stosw
mov ebp,edi 
mov bh,0                                         ; BH = 0 means system info
call HelperSystemInfo
mov edi,ebp
mov ax,0A0Dh                                     ; Interval
stosw
mov ebp,edi 
mov ax,STR_OS_PROC_CURRENT                       ; Processors at currebt group
call HelperOsInfoString
mov al,SYSTEM_INFO.dwNumberOfProcessors
call HelperOsInfoNumber32
mov ecx,[esi + REGISTRY32.osData.activeProcessorCount]      ; Processors total
jecxz @f
push ecx
mov ax,STR_OS_PROC_TOTAL
call HelperOsInfoString
pop eax
call HelperOsInfoValue32
@@:
mov ecx,[esi + REGISTRY32.osData.activeProcessorGroupCount] ; Processors groups
jecxz @f
push ecx
mov ax,STR_OS_PROC_GROUPS
call HelperOsInfoString
pop eax
call HelperOsInfoValue32
@@:
mov ecx,[esi + REGISTRY32.osData.numaNodeCount]  ; NUMA domains
jecxz @f
push ecx
mov ax,STR_OS_NUMA_DOMAINS
call HelperOsInfoString
pop eax
call HelperOsInfoValue32
@@:
mov ax,STR_OS_NORMAL_PAGE                        ; Normal page size
call HelperOsInfoString
mov al,SYSTEM_INFO.dwPageSize
call HelperOsInfoSizeAuto32
mov ecx,[esi + REGISTRY32.osData.largePageSize]  ; Large page size and status
jecxz .skipLargePages
push ecx
mov ax,STR_OS_LARGE_PAGE
call HelperOsInfoString
pop eax
push eax ecx esi
mov bl,0FFh
xor edx,edx
call SizePrint64
cmp [esi + REGISTRY32.osData.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
call PoolStringWrite
pop esi edi eax
xor edx,edx
call HexPrint64
.skipLargePages:
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textOs.objectStop],ebp

;---------- Build text block for native operating system information screen ---;

mov [esi + REGISTRY32.textNativeOs.objectStart],ebp
mov bh,1                                  ; BH = 1 means native system info
call HelperSystemInfo
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textNativeOs.objectStop],ebp

;---------- Build text block for processor details screen ---------------------;



;---------- Build text block for topology screen, first block = list ----------;

mov [esi + REGISTRY32.textTopology1.objectStart],ebp
mov ebx,[esi + REGISTRY32.listTopology.objectStart]
mov eax,[esi + REGISTRY32.listTopology.objectStop]
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
push ebx ecx esi
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
mov ax,STR_TP_CPU_PACKAGE
call HelperRelationName 
call HelperAffinity
.doneRelation:
add esi,24
pop edi
dec ecx
jnz .scanRelations
pop esi ecx ebx
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textTopology1.objectStop],ebp

;---------- Build text block for topology screen, second block = summary ------;

mov [esi + REGISTRY32.textTopology2.objectStart],ebp
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
mov [esi + REGISTRY32.textTopology2.objectStop],ebp

;--- Build text block for extended topology screen, first block = list --------;

mov [esi + REGISTRY32.textTopologyEx1.objectStart],ebp
mov ebx,[esi + REGISTRY32.listTopologyEx.objectStart]
mov ecx,[esi + REGISTRY32.listTopologyEx.objectStop]
test ecx,ecx
jz .noTopologyEx
mov edi,ebp
push ebx ecx esi
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
pop esi ecx ebx
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textTopologyEx1.objectStop],ebp
.noTopologyEx:

;--- Build text block for extended topology screen, second block = summary ----;

mov [esi + REGISTRY32.textTopologyEx2.objectStart],ebp
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
mov [esi + REGISTRY32.textTopologyEx2.objectStop],ebp

;---------- Build text block for NUMA domains list screen ---------------------;

mov edx,[esi + REGISTRY32.listNuma.objectStart]
mov ecx,[esi + REGISTRY32.listNuma.objectStop]
test ecx,ecx
jz .noNuma
mov [esi + REGISTRY32.textNuma.objectStart],ebp
push esi
mov esi,edx
lodsd
test eax,eax
jnz .numaExt
.numaStd:
cmp esi,ecx
jae .numaDone
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
.numaExt:
cmp esi,ecx
jae .numaDone
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
.numaDone:
pop esi
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textNuma.objectStop],ebp
.noNuma:

;---------- Build text block for processor groups list screen -----------------;

mov edx,[esi + REGISTRY32.listGroup.objectStart]
mov ecx,[esi + REGISTRY32.listGroup.objectStop]
test ecx,ecx
jz .noGroups
mov [esi + REGISTRY32.textGroup.objectStart],ebp
push esi
mov esi,edx
.groupsScan:
cmp esi,ecx
jae .groupsDone
lodsd
call HelperNumberString
lea edi,[ebp - 82 + 10]
lodsd
mov bl,0
call DecimalPrint32
jmp .groupsScan 
.groupsDone:
pop esi
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textGroup.objectStop],ebp
.noGroups:

;---------- Build text block for ACPI screen, first block = tables list -------;

mov edx,[esi + REGISTRY32.listAcpi.objectStart]
mov ecx,[esi + REGISTRY32.listAcpi.objectStop]
test ecx,ecx
jz .noAcpi
mov ecx,[edx]
mov ebx,ecx
sub ecx,edx
sub ecx,16
shr ecx,2
jecxz .noAcpi
mov [esi + REGISTRY32.textAcpi1.objectStart],ebp
push esi
mov esi,edx
.acpiScan:
mov edi,ebp
push ecx
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov eax,00040100h
call HelperCopyString
mov eax,0006080Ah
call HelperCopyString 
mov eax,00081110h
call HelperCopyString 
mov eax,0004201Ch
call HelperCopyString 
mov ax,2D18h
call HelperHexString
mov ax,3920h
call HelperHexString 
mov ax,4708h
call HelperDecimalString
add ebx,36
add ebp,82
pop ecx
loop .acpiScan 
.acpiDone:
pop esi
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textAcpi1.objectStop],ebp
.noAcpi: 

;--- Build text block for ACPI screen, second block = tables detection --------;

mov edx,[esi + REGISTRY32.appData.lockedDataAcpi]
test edx,edx
jz .noAcpiData 
mov eax,[esi + REGISTRY32.listAcpi.objectStart]
mov ecx,[esi + REGISTRY32.listAcpi.objectStop]
jecxz .noAcpiData
mov ecx,[eax]
sub ecx,eax
sub ecx,16
shr ecx,2
jecxz .noAcpiData
lea ebx,[eax + 16]
mov [esi + REGISTRY32.textAcpi2.objectStart],ebp
push esi
.acpiDataScan:
mov edi,ebp
push ecx edx
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov eax,00040100h
call HelperCopyString
mov ax,', '
stosw
pop edx
mov esi,edx
mov ecx,[ebx]
.findAcpi:
cmp byte [esi],0
je .notFoundAcpi
cmp ecx,[esi]
je .foundAcpi
.skipAcpi:
lodsb
cmp al,0
jne .skipAcpi
jmp .findAcpi
.notFoundAcpi:
mov ax,STR_ACPI_UNKNOWN
call PoolStringWrite
jmp .doneAcpi
.foundAcpi:
add esi,4
call StringWrite
.doneAcpi:
add ebx,4
add ebp,82
pop ecx
loop .acpiDataScan
pop esi
.acpiDataDone:
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textAcpi2.objectStop],ebp
.noAcpiData:

;---------- Build text block for affinized CPUID dump -------------------------;

mov edx,[esi + REGISTRY32.listAffCpuid.objectStart]
mov ecx,[esi + REGISTRY32.listAffCpuid.objectStop]
test ecx,ecx
jz .noAffCpuidData
mov [esi + REGISTRY32.textAffCpuid.objectStart],ebp
push esi
mov esi,edx
xor edx,edx
.scanBlocks:
push ecx
mov ecx,[esi]
bts ecx,31
add esi,32
.scanLines:
mov edi,ebp
push ecx
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop ecx
btr ecx,31
jnc @f
lea edi,[ebp + 2]
mov bl,0
mov eax,edx
call DecimalPrint32
@@:
mov ebx,esi
push ecx edx
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
pop edx ecx
add esi,32
add ebp,82
dec ecx
jnz .scanLines
mov edi,ebp
mov ax,0A0Dh
stosw
mov ebp,edi
inc edx
pop ecx
cmp esi,ecx
jb .scanBlocks
.affCpuidDataDone:
pop esi
mov al,0
stosb
mov ebp,edi
mov [esi + REGISTRY32.textAffCpuid.objectStop],ebp
.noAffCpuidData:

;---------- Build bind buffer -------------------------------------------------;
; Start operations with bind buffer part of application registry,
; EDI = Pointer for bind buffer addressing.
; EBP = Pointer for CPU data addressing in the application registry

push esi
mov edi,[esi + REGISTRY32.allocatorBindBuffer.objectStart]
lea ebp,[esi + REGISTRY32.cpuData.vendorString]
mov ebx,ebp

;---------- CPU vendor string, 12 chars ---------------------------------------;

xchg eax,ebp
stosd
xor eax,eax
stosd

;---------- CPU signature: type, family, model, stepping ----------------------;

mov ax,STR_TFMS
call PoolStringWrite
mov eax,[ebx + CPUDATA.cpuSignature]
call HexPrint32
mov ax,STR_H
call PoolStringWrite
mov al,0
stosb 

;---------- CPU model string, maximum 48 chars --------------------------------;

lea eax,[ebx + CPUDATA.modelString]
stosd
xor eax,eax
stosd

;---------- TSC frequency -----------------------------------------------------;

mov ecx,32
mov al,0
rep stosb
push edi ebx
sub edi,32
mov ax,STR_TSC
call PoolStringWrite
finit
push 0 1000000
fild qword [ebx + CPUDATA.tscClockHz] 
fidiv dword [esp]
fstp qword [esp]
pop eax edx
mov bx,0100h
call DoublePrint
mov ax,STR_MHZ
call PoolStringWrite
pop ebx edi

;---------- CPU instructions and OS context management features bitmaps -------; 

mov eax,dword [ebx + CPUDATA.extractedFeaturesBitmap + 0]
stosd
mov eax,dword [ebx + CPUDATA.extractedFeaturesBitmap + 4]
stosd
mov eax,dword [ebx + CPUDATA.extractedAvx512Bitmap + 0]
stosd
mov eax,dword [ebx + CPUDATA.extractedAvx512Bitmap + 4]
stosd
mov eax,dword [ebx + CPUDATA.extractedContextBitmap + 0]
stosd
mov eax,dword [ebx + CPUDATA.extractedContextBitmap + 4]
stosd
mov eax,dword [ebx + CPUDATA.extractedMethodsBitmap + 0]
stosd
mov eax,dword [ebx + CPUDATA.extractedMethodsBitmap + 4]
stosd
pop esi

;---------- Text blocks for viewers -------------------------------------------;

mov eax,[esi + REGISTRY32.textOs.objectStart]
stosd       ; Operating system information
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textNativeOs.objectStart]
stosd       ; Native operating system information
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textTopology1.objectStart]
stosd       ; Topology information by operating system, text block 1, list
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textTopology2.objectStart]
stosd       ; Topology information by operating system, text block 2, summary
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textTopologyEx1.objectStart]
stosd       ; Extended topology information by OS, text block 1, list
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textTopologyEx2.objectStart]
stosd       ; Extended topology information by OS, text block 2, summary
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textNuma.objectStart]
stosd       ; NUMA domains list by OS
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textGroup.objectStart]
stosd       ; Processor groups list by OS
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textAcpi1.objectStart]
stosd       ; ACPI tables, text block 1 = tables list
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textAcpi2.objectStart]
stosd       ; ACPI tables, text block 2 = tables detection by data base
xor eax,eax
stosd
mov eax,[esi + REGISTRY32.textAffCpuid.objectStart]
stosd       ; Affinized CPUID dump
xor eax,eax
stosd

; ...

;---------- Exit points -------------------------------------------------------;

; TODO. Error Handling.
.error:
; TODO. Error Handling.
pop ecx     ; this POP for remove temporary variable
pop ebp edi esi ebx
ret



;---------- Align memory pointer, required for WinAPI calls -------------------;
; INPUT:   EBP = Pointer                                                       ;
; OUTPUT:  EBP = Aligned pointer, updated if required                          ;
;------------------------------------------------------------------------------;

HelperAlignment:
test bp,00001111b
jz @f
and ebp,0FFFFFFF0h
add ebp,010h
@@: 
ret

;---------- Detect CPUID support and execute CPUID function #0 ----------------;
; Note CPUID can be supported by CPU but locked by Virtual Monitor.            ;
; Note check bit EFLAGS.21 toggleable, it is CPUID support indicator.          ;
; Note probably wrong result if debug trace this subroutine code.              ;
;                                                                              ;
; INPUT:   EDI = Destination pointer for save CPU Vendor String                ;
;                                                                              ;
; OUTPUT:  CF flag = Status: 0(NC)=Support OK, 1(C)=Not supported              ;
;          Output EAX, EDI, Destination memory valid only if CF=0(NC)          ;
;          EAX = Largest standard CPUID function supported                     ;
;          EDI = Input EDI + 13,                                               ; 
;                string size fixed = 12 chars + 0 = terminator byte            ;
;          Destination memory at [input EDI] =                                 ;
;           bytes [00-11] = CPU vendor string                                  ;
;           byte  [12-12] = 00h, string terminator                             ;
;------------------------------------------------------------------------------;

HelperCheckCpuid:

;---------- Check for ID bit writeable for "1" --------------------------------;

mov ebx,21
pushf                     ; In the 32-bit mode, push EFLAGS
pop eax
bts eax,ebx               ; Set EAX.21=1
push eax
popf                      ; Load EFLAGS with EFLAGS.21=1
pushf                     ; Store EFLAGS
pop eax                   ; Load EFLAGS to EAX
btr eax,ebx               ; Check EAX.21=1, Set EAX.21=0
jnc .absent               ; Go error branch if cannot set EFLAGS.21=1

;---------- Check for ID bit writeable for "0" --------------------------------;

push eax
popf                      ; Load EFLAGS with EFLAGS.21=0
pushf                     ; Store EFLAGS
pop eax                   ; Load EFLAGS to EAX
btr eax,ebx               ; Check EAX.21=0
jc .absent                ; Go if cannot set EFLAGS.21=0

;---------- Execute CPUID function 0, store results ---------------------------;

xor eax,eax               ; EAX = Function number for CPUID instruction
cpuid                     ; Execute CPUID function 0
xchg eax,ebx              ; XCHG instead MOV, short code
stosd                     ; Store Vendor String [00-03]
xchg eax,edx	  
stosd                     ; Store Vendor String [04-07]
xchg eax,ecx
stosd                     ; Store Vendor String [08-11]
mov al,0
stosb                     ; Zero terminator byte
xchg eax,ebx              ; Restore EAX = Largest standard function supported

;---------- Exit points -------------------------------------------------------;

ret                       ; Return, at this point CF=0(NC) after XOR EAX,EAX
.absent:
stc                       ; CF=1(C) means error
ret 

;---------- Get CPU name string, execute CPUID functions 80000002h-80000004h --;
; Call this subroutine only if CPUID instruction supported.                    ;
;                                                                              ;
; INPUT:   EDI = Destination pointer for save CPU Vendor String                ;
;                                                                              ;
; OUTPUT:  EDI = Input EDI + 48 + 1 , string size fixed = 48 bytes             ;
;          Destination memory at [input EDI] =                                 ; 
;           bytes [00-47] = CPU Vendor String                                  ;
;           byte  [48-48] = 00h, terminator for copy by StringWrite            ;
;          String formatted by subroutine, left spaces deleted.                ;
;          If feature not supported, string contain "None",                    ;
;          but errors not reported, NCRB can work without this feature         ;
;------------------------------------------------------------------------------;

HelperGetCpuName:
cld                          ; Clear direction, because STOSB/STOSD used
push ebx esi

;---------- Blank and prepare for not support mode ----------------------------;

mov ecx,48                   ; RCX = String length, also clear bits RCX[63-32]
mov al,' '                   ; AL = Space
rep stosb                    ; Blank string
mov dword [edi - 48],'None'  ; Default string, if feature not supported
mov byte [edi],0             ; Zero terminator byte 

;---------- Check functions supported -----------------------------------------;

mov esi,80000000h            ; ESI = First extended function = 80000000h
mov eax,esi                  ; EAX = Function for verify support = 80000000h
cpuid                        ; Execute CPUID function 80000000h
lea ebx,[esi + 4]            ; EBX = Maximum function 80000004h
cmp eax,ebx
jb .exit                     ; Go exit if required functions not supported
sub edi,48                   ; Must be +48 if go exit when not supported

;---------- Get and store CPU name string -------------------------------------;

push edi
.storeCpuName:
lea eax,[esi + 2]          ; EAX = Function number, start from 80000002h
cpuid
stosd
xchg eax,ebx
stosd
xchg eax,ecx
stosd
xchg eax,edx
stosd
inc esi
cmp si,4-2                 ; This give function number EAX=80000004h
jbe .storeCpuName
pop edi

;---------- Formatting CPU name string, delete left spaces and byte 00h -------;

mov esi,edi
mov ecx,48                 ; ECX = String size limit
mov ebx,ecx                ; EBX = Used space count = 48 - Unused space count
.scanCpuName:              ; This cycle for delete left spaces
lodsb                      
dec ebx                   ; Count used space
cmp al,0
je .endCpuName            ; Go blank if end of string without name, here RCX=48
cmp al,' '
loope .scanCpuName        ; Cycle for skip left spaces
mov cl,48
je .endCpuName            ; Go blank if only spaces in the string, here RCX=48
inc ebx
dec esi

;---------- Copy used portion of name -----------------------------------------;

.copyCpuName:             ; This cycle for copy name before terminator = 00h
lodsb
cmp al,0
je .endCpuName
stosb
dec ecx
dec ebx
jnz .copyCpuName

;---------- Blank unused portion of name --------------------------------------;

.endCpuName:              ; Start blank tail with space = 20h
mov al,' '
rep stosb

;---------- Exit point --------------------------------------------------------;

.exit:
inc edi                   ; Skip terminator byte
pop esi ebx
ret

;---------- Measure CPU TSC (Time Stamp Counter) clock frequency --------------;
; Store results F = Frequency=[Hz].                                            ;
; Call this subroutine only if CPUID and RDTSC both supported.                 ;
;                                                                              ;
; INPUT:   EDI = Destination pointer for save TSC frequency                    ;
;                                                                              ;
; OUTPUT:  CF flag = Status: 0(NC)=Measured OK, 1(C)=Measurement error	       ;
;          Output EDI and destination memory valid only if CF=0(NC)            ;
;          EDI = Input EDI + 8 , buffer size fixed = 8 bytes                   ;
;          Destination memory at [input EDI] = Results                         ;
;           Qword [00-07] = TSC frequency, Hz = delta TSC per 1 second         ;
;------------------------------------------------------------------------------;

HelperMeasureTsc:
cld                        ; Clear direction, because STOSQ used
push ebx esi ebp eax eax   ; Last 2 pushes = reserve space for local variables
mov ebp,esp                ; EBP = pointer to local variables

;---------- Start measure frequency, get current change -----------------------;

push ebp                          ; Parm#1 = pointer to output 64-bit variable
call [GetSystemTimeAsFileTime]    ; Get current count
mov esi,[ebp]
@@:                               ; Wait for start 1 second interval
push ebp
call [GetSystemTimeAsFileTime]    ; Get next count for wait 100 ns
cmp esi,[ebp]
je @b
mov esi,[ebp + 0]                 ; Set time interval = 1 second
mov ebx,[ebp + 4]
add esi,10000000                  ; 10^7 * 100ns = 1 second
adc ebx,0
rdtsc                             ; Get start TSC
push edx eax
@@:                               ; Wait for end 1 second interval
push ebp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp [ebp + 4],ebx
jb @b
ja @f
cmp [ebp + 0],esi
jb @b
@@:
rdtsc                             ; Get end TSC, calculate delta-TSC
pop esi ebx
sub eax,esi
sbb edx,ebx     ; EDX:EAX = Delta TSC per 1 second = frequency, Hz
jb .error       ; Go error if frequency < 0               
mov ecx,eax
or ecx,edx
jz .error       ; Go error if frequency = 0

;-------- Store Frequency, as 64-bit integer value, Hz, delta-TSC per second --;

stosd                   ; Store EAX = Frequency, low dword
xchg eax,edx
stosd                   ; Store EAX = Frequency, high dword

;---------- Restore RSP, pop extra registers, exit ----------------------------;

clc             ; CF=0 (NC) means CPU clock measured OK
.exit:
pop eax eax ebp esi ebx
ret
.error:
stc             ; CF=1 (CY) means CPU clock measured ERROR
jmp .exit

;---------- Interpreting list of entries ENTRY_CPUID, ENTRY_XCR0 --------------;
; INPUT:   RSI = Pointer to builder script                                     ;
; OUTPUT:  EAX = Bitmap, result of builder script                              ;
;          RAX.[63-31] = 0                                                     ;  
;------------------------------------------------------------------------------;

HelperBuildBitmap:
push esi edi ebp ebx
cld
xor ebp,ebp
xor ebx,ebx
.build:
lodsb
mov dl,al
and eax,00111111b
dec eax
jz .entryCpuid
dec eax
jz .entryCpuidS
dec eax
jz .prefixCpuid
dec eax
jz .prefixCpuidS
dec eax
jz .entryXcr0
dec eax
jz .prefixXcr0
.done:
xchg eax,ebx
pop ebx ebp edi esi
ret
.entryCpuid:
lodsd
xchg ecx,eax
lodsb
mov dh,al
xchg ecx,eax
xor ecx,ecx
.helperCpuid:
call HelperEntryCpuid 
.next:
inc ebp
jmp .build
.entryCpuidS:
lodsd
xchg ecx,eax
lodsd
xchg ecx,eax
mov dh,[esi]
inc esi
jmp .helperCpuid
.prefixCpuid:
jmp .next
.prefixCpuidS:
jmp .build
.entryXcr0:
xor eax,eax
lodsb
call HelperEntryXcr0
jmp .next
.prefixXcr0:
jmp .build

;---------- Interpreting one ENTRY_CPUID object at build bitmap list ----------;
; Check CPUID instruction support before this subroutine call.                 ;
;                                                                              ;
; INPUT:   EAX = CPUID function code                                           ;
;          ECX = CPUID sub-function code                                       ;    
;          EDX = Bit number at DH, register number at DL bits [7-6]            ;
;                DL bits [5-0] undefined, can be non zero                      ;
;          EBX = Bitmap for accumulate features flags by build scenario        ;
;          EBP = Selector for bit of bitmap, used for select one of EBX bits   ;
;                                                                              ;
; OUTPUT:  EBX = Updated 32-bit bitmap, caller can twice execute build cycle   ;
;                for low and high 32-bit dwords of 64-bit bitmap,              ;
;                note about 64-bit code portability to 32-bit code.            ;
;------------------------------------------------------------------------------;

HelperEntryCpuid:
push esi edi ebp ebx
mov esi,eax     ; ESI = function
mov edi,ecx     ; EDI = subfunction 
mov ebp,edx     ; EBP = bit number : register id
mov eax,esi
and eax,80000000h
cpuid
cmp eax,esi
jb .bitNo
xchg eax,esi
mov ecx,edi
cpuid
mov edi,ebp
shr edi,8
and edi,00FFh
and ebp,00FFh
shr ebp,6
jz .regEax
dec ebp
jz .regEbx
dec ebp
jz .regEcx
.regEdx:
bt edx,edi
jmp .bitDone
.regEcx:
bt ecx,edi
jmp .bitDone
.regEbx:
bt ebx,edi
jmp .bitDone
.regEax:
bt eax,edi
jmp .bitDone
.bitNo:
clc
.bitDone:
pop ebx ebp edi esi
jnc .zero
bts ebx,ebp
.zero:
ret

;---------- Interpreting one ENTRY_XCR0 object at build bitmap list -----------;
; Check CPUID instruction support before this subroutine call,                 ;
; XGETBV instruction support verified in this subroutine.                      ;
;                                                                              ;
; INPUT:   EAX = Tested bit number at XCR0                                     ;
;          EBX = Bitmap for accumulate features flags by build scenario        ;
;          EBP = Selector for bit of bitmap, used for select one of EBX bits   ;
;                                                                              ;
; OUTPUT:  EBX = Updated 32-bit bitmap, caller can twice execute build cycle   ;
;                for low and high 32-bit dwords of 64-bit bitmap,              ;
;                note about 64-bit code portability to 32-bit code.            ;
;------------------------------------------------------------------------------;

HelperEntryXcr0:
push ebp ebx
xchg ebp,eax
xor eax,eax
cpuid
cmp eax,1
jb .bitNo
mov eax,1
cpuid
bt ecx,27
jnc .bitNo
mov ebx,ebp
and ebp,00011111b
xor ecx,ecx
xgetbv
test ebx,00100000b
jnz .high32
bt eax,ebp
jmp .bitDone
.high32:
bt edx,ebp
jmp .bitDone
.bitNo:
clc
.bitDone:
pop ebx ebp
jnc .zero
bts ebx,ebp
.zero:
ret

;--- Pre-blank string with CR, LF, write parameter name, set text pointers ----;
; INPUT:   EBP = Pointer to destination buffer                                 ;
;          AX  = Index for parameter name for write to string                  ; 
; OUTPUT:  EDI = Pointer to parameter value position in the string             ;
;          ECX  = Pointer to parameter hex value position in the string        ;
;          EBP = Updated pointer to destination buffer                         ;   
;------------------------------------------------------------------------------;

HelperOsInfoString:
push esi
mov edi,ebp
push eax
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop eax
mov ebp,edi
lea edi,[ebp - 82 + 01]
call PoolStringWrite
lea edi,[ebp - 82 + 33]
lea ecx,[ebp - 82 + 62]
pop esi
ret

;---------- Write memory block size to pre-blanked string, include hex --------;
;           This variant for units fixed = megabytes                           ;
;           This variant for memory status structure.                          ;
;           This variant for 64-bit value.                                     ;
; INPUT:   EDI = Pointer to destination buffer, value position                 ;
;          ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the MEMORYSTATUSEX structure                        ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoSize:
mov bl,2
SizeHelperEntry:
movzx eax,al
push ecx
mov ecx,[Registry]
cmp bl,0FFh
je .sysinfo
lea ecx,[ecx + REGISTRY32.osData.memoryStatusEx]
jmp .done
.sysinfo:
test bh,bh
jnz .nativeInfo
lea ecx,[ecx + REGISTRY32.osData.systemInfo]
jmp .done
.nativeInfo:
lea ecx,[ecx + REGISTRY32.osData.nativeSystemInfo]
.done:
mov edx,[ecx + eax + 4]
mov eax,[ecx + eax + 0]
cmp bl,0
je .skip
call SizePrint64
.skip:
pop edi
jmp HexPrint64

;---------- Write memory block size to pre-blanked string, include hex --------;
;           This variant for units auto selected                               ;
;           This variant for system information structure.                     ;
;           This variant for 64-bit value.                                     ;
; INPUT:   EDI = Pointer to destination buffer, value position                 ;
;          ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the MEMORYSTATUSEX structure                        ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoSizeAuto:
mov bl,0FFh
jmp SizeHelperEntry

;---------- Write memory block size to pre-blanked, hex only ------------------;
;           This variant for memory status structure.                          ;
;           This variant for 64-bit value.                                     ;
; INPUT:   ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the MEMORYSTATUSEX structure                        ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoHex:
mov bl,0
jmp SizeHelperEntry 

;---------- Write memory block size to pre-blanked string, include hex --------;
;           This variant for units fixed = megabytes.                          ;
;           This variant for system information structure.                     ;
;           This variant for 32-bit value.                                     ;
; INPUT:   EDI = Pointer to destination buffer, value position                 ;
;          ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoSizeAuto32:
mov bl,0FFh
movzx eax,al
push ecx
mov ecx,[Registry]
test bh,bh
jnz .nativeInfo
mov eax,[ecx + REGISTRY32.osData.systemInfo + eax]
jmp .doneInfo
.nativeInfo:
mov eax,[ecx + REGISTRY32.osData.nativeSystemInfo + eax]
.doneInfo:
push eax
xor edx,edx
call SizePrint64
pop eax edi
jmp HexPrint32

;---------- Write 32-bit number to pre-blanked string, decimal and hex --------;
; INPUT:   EDI = Pointer to destination buffer, decimal value position         ;
;          ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoNumber32:
movzx eax,al
push ecx
mov ecx,[Registry]
test bh,bh
jnz .nativeInfo
mov eax,[ecx + REGISTRY32.osData.systemInfo + eax]
jmp .doneInfo
.nativeInfo:
mov eax,[ecx + REGISTRY32.osData.nativeSystemInfo + eax]
.doneInfo:
pop ecx
HelperOsInfoValue32:     ; Entry point for EAX = value, EDI, ECX same usage
push eax
mov bl,0
call DecimalPrint32
pop eax
mov edi,ecx
jmp HexPrint32

;---------- Write 16-bit number to pre-blanked string, decimal and hex --------;
; INPUT:   EDI = Pointer to destination buffer, decimal value position         ;
;          ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoNumber16:
movzx eax,al
push ecx
mov ecx,[Registry]
test bh,bh
jnz .nativeInfo
movzx eax,word [ecx + REGISTRY32.osData.systemInfo + eax]
jmp .doneInfo
.nativeInfo:
movzx eax,word [ecx + REGISTRY32.osData.nativeSystemInfo + eax]
.doneInfo:
push eax
mov bl,0
call DecimalPrint32
pop eax edi
jmp HexPrint16

;---------- Write system information strings ----------------------------------;
; INPUT:   EBP = Pointer to destination buffer                                 ;
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  EBP = Updated pointer to destination buffer                         ;
;          EDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperSystemInfo:
mov ax,STR_OS_MIN_ADDRESS                        ; Application minimum address
call HelperOsInfoString
mov al,SYSTEM_INFO.lpMinimumApplicationAddress
call HelperOsInfoSizeAuto32
mov ax,STR_OS_MAX_ADDRESS                        ; Application maximum address
call HelperOsInfoString
mov al,SYSTEM_INFO.lpMaximumApplicationAddress
call HelperOsInfoSizeAuto32
mov ax,STR_OS_PROC_MASK                          ; Active processor mask 
call HelperOsInfoString
mov al,'-'
stosb
test bh,bh
jnz .nativeInfo
mov eax,[esi + REGISTRY32.osData.systemInfo.dwActiveProcessorMask]
jmp .doneInfo
.nativeInfo:
mov eax,[esi + REGISTRY32.osData.nativeSystemInfo.dwActiveProcessorMask]
.doneInfo:
mov edi,ecx
call HexPrint32 
mov ax,STR_OS_PROC_TYPE                          ; Processor type
call HelperOsInfoString
mov al,SYSTEM_INFO.dwProcessorType
call HelperOsInfoNumber32
mov ax,STR_OS_ALLOC_GRAN                         ; Allocation granularity
call HelperOsInfoString
mov al,SYSTEM_INFO.dwAllocationGranularity
call HelperOsInfoSizeAuto32 ; HelperOsInfoNumber32
mov ax,STR_OS_PROC_LEVEL                         ; Processor level
call HelperOsInfoString
mov al,SYSTEM_INFO.wProcessorLevel
call HelperOsInfoNumber16
mov ax,STR_OS_PROC_REVISION                      ; Processor revision
call HelperOsInfoString
mov al,SYSTEM_INFO.wProcessorRevision
jmp HelperOsInfoNumber16

;--- Write relation name string for text generation by topology parsing -------;
; INPUT:   RDX = Destination base address                                      ;
;          AX  = String ID at application strings pool                         ;
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperRelationName:
lea edi,[edx + 01]
RelationNameEntry:
push ecx esi
call PoolStringWrite
pop esi ecx
ret

;--- Write affinity mask string for text generation by topology parsing -------;
; INPUT:   EDX = Destination base address                                      ;
;          ESI = Pointer to parsed topology entry: affinity mask               ;
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperAffinity:
push ecx edx esi ebp
lea edi,[edx + 18]   ; EDI = Destination text pointer
mov eax,[esi + 00]   ; EAX = Affinity mask
lea ebp,[edi + 16]   ; EBP  = Mask text format limit
bsf ecx,eax
bsr edx,eax
cmp ecx,edx
je .modeSingle
push ecx edx
.scanMask:
bt eax,ecx
jz .endMask
inc ecx
cmp ecx,edx
jb .scanMask
.endMask:
cmp ecx,edx
pop edx ecx
je .modeInterval
.modeIndividual:
xor ecx,ecx
xor edx,edx
.cycleIndividual:
cmp edi,ebp
jae .overIndividual
shr eax,1
jnc .skipIndividual
push eax
test edx,edx
jz .firstIndividual
mov al,','
stosb
.firstIndividual:
inc edx
mov eax,ecx
mov bl,0
call DecimalPrint32
pop eax
.skipIndividual:
inc ecx
cmp cl,64
jb .cycleIndividual
jmp .done
.overIndividual:
mov ax,STR_TP_POINTS
call PoolStringWrite
jmp .done
.modeInterval:
xchg eax,ecx
mov bl,0
call DecimalPrint32 
mov al,'-'
stosb
.modeSingle:
xchg eax,edx
mov bl,0
call DecimalPrint32
.done:
pop ebp esi edx ecx
ret

;--- Write number value string for text generation by topology parsing --------;
; Has 3 entry points depend on parameter width: 8, 16, 32 bits                 ;
; INPUT:   EDX = Destination base address                                      ;
;          ESI = Pointer to parsed topology entry                              ;
;          AL          = Parameter offset in the topology entry structure      ; 
;          AH          = Text string position offset at destination            ;   
;          EAX.[31-16] = Parameter name String ID at application strings pool  ;  
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperParameter8:         ; Entry point for addressed 8-bit parameter
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,byte [esi + eax]
ParameterEntry:
push ecx esi
lea edi,[edx + edi]
push eax
xchg eax,ebx
call PoolStringWrite
pop eax
mov bl,0
call DecimalPrint32
pop esi ecx
ret
HelperParameter16:         ; Same entry point for addressed 16-bit parameter
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,word [esi + eax]
jmp ParameterEntry
HelperParameter32:         ; Same entry point for addressed 32-bit parameter
shld ebx,eax,16
movzx edi,ah
movzx eax,al
mov eax,dword [esi + eax]
jmp ParameterEntry

;--- Write cache size and count string for text gener. by topology parsing ----;
; Has 3 entry points depend on parameter width: 8, 16, 32 bits                 ;
; INPUT:   ESI = Pointer to topology information buffer                        ;
;          ECX = Entries count at topology information buffer                  ;  
;          AL  = Cache level                                                   ;  
;          AH  = Cache type                                                    ;    
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperCacheSummary:
push ecx esi 0 0 ebp
mov ebp,esp
test ecx,ecx
jz .nodata
movzx ebx,ah
xchg edx,eax
.scanCaches:
cmp dword [esi + 04],2
jne .done
cmp byte [esi + 08],dl
jne .done
cmp dword [esi + 16],ebx
jne .done
mov eax,[esi + 12]
mov [ebp + 04],eax
inc dword [ebp + 08]
.done:
add esi,24
loop .scanCaches
cmp dword [ebp + 08],0
je .nodata
mov bh,dl
mov edx,edi
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
push edi
lea edi,[edx + 01]
mov ax,STR_TP_L
call PoolStringWrite
mov al,bh
or al,30h
stosb
movzx ax,bl
add ax,STR_TP_UNIFIED
lea edi,[edx + 04] 
call RelationNameEntry
lea edi,[edx + 18]
mov bl,0FFh
push edx
mov eax,[ebp + 04]
xor edx,edx
call SizePrint64
pop edx
lea edi,[edx + 41]
mov ax,STR_TP_X
call PoolStringWrite
mov eax,[ebp + 08]
mov bl,0
call DecimalPrint32
pop edi
.nodata:
pop ebp eax eax esi ecx
ret

;---------- Get topological information ---------------------------------------;
; INPUT:   EBX = WinAPI function address                                       ;
;          EBP = Pointer to buffer for topology information                    ;
;          EDI = Buffer used size accumulator                                  ;
;          AL  = Relation type by WinAPI encoding                              ;
; OUTPUT:  EDI = Updated buffer size accumulator                               ;
;------------------------------------------------------------------------------;             

HelperTopologyEx:
push ebx esi eax 0
mov eax,esp
push eax                     ; Parm#3 = Pointer to size
lea eax,[ebp + edi]
push eax                     ; Parm#2 = Pointer to buffer
movzx eax,byte [esp + 12]
push eax                     ; Parm#1 = Relation type
call ebx
test eax,eax
jnz .error
call [GetLastError]
cmp eax,ERROR_BUFFER_LIMIT
jne .error                   ; Go if other errors detected
mov edx,[esp]
cmp edx,VALUE_BUFFER_LIMIT
ja .error                    ; Go if required buffer size too big 
lea ecx,[ebp + edi]
add edi,edx
cmp edi,VALUE_BUFFER_LIMIT
ja .error                    ; Go if required buffer size too big 
mov eax,esp
push eax                     ; Parm#3 = Pointer to size
push ecx                     ; Parm#2 = Pointer to buffer
movzx eax,byte [esp + 12]
push eax                     ; Parm#1 = Relation type
call ebx
test eax,eax
jz .error
.exit:
pop eax eax esi ebx
ret
.error:
stc
jmp .exit

;--- Write affinity mask string for text generation by topology parsing -------;
; INPUT:   EDX = Destination base address                                      ;
;          ESI = Pointer to parsed topology entry: affinity mask               ;
;          AL  = Field offset in the parsed structure                          ; 
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperAffinityGroup:    ; TODO. Optimize by EntryAffinity
push esi ebp
movzx eax,al
add esi,eax
lea edi,[edx + 18]
lea ebp,[edi + 16]
mov ax,[esi + 04]
call HexPrint16
mov al,'.'
stosb
push ecx edx
mov eax,[esi + 00]
bsf ecx,eax
bsr edx,eax
cmp ecx,edx
je .modeSingle
push ecx edx
.scanMask:
bt eax,ecx
jz .endMask
inc ecx
cmp ecx,edx
jb .scanMask
.endMask:
cmp ecx,edx
pop edx ecx
je .modeInterval
.modeIndividual:
xor ecx,ecx
xor edx,edx
.cycleIndividual:
cmp edi,ebp
jae .overIndividual
shr eax,1
jnc .skipIndividual
push eax
test edx,edx
jz .firstIndividual
mov al,','
stosb
.firstIndividual:
inc edx
mov eax,ecx
mov bl,0
call DecimalPrint32
pop eax
.skipIndividual:
inc ecx
cmp cl,64
jb .cycleIndividual
jmp .done
.overIndividual:
mov ax,STR_TP_POINTS
call PoolStringWrite
jmp .done
.modeInterval:
xchg eax,ecx
mov bl,0
call DecimalPrint32 
mov al,'-'
stosb
.modeSingle:
xchg eax,edx
mov bl,0
call DecimalPrint32
.done:
pop edx ecx ebp esi
ret

;--- Write proc. group string for text generation by topology parsing ---------;
; INPUT:   EDX = Destination base address                                      ;
;          ESI = Pointer to parsed topology entry: affinity mask               ;
;          AL  = Field offset in the parsed structure                          ; 
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperAffinityMask:
push esi
movzx eax,al
add esi,eax
lea edi,[edx + 18]
mov ax,[esi + 08]
call HexPrint16
pop esi
ret

;--- Write cache size and count string for text gener. by topology parsing ----;
; Has 3 entry points depend on parameter width: 8, 16, 32 bits                 ;
; INPUT:   ESI = Pointer to topology information buffer                        ;
;          ECX = Topology information buffer address limit                     ;  
;          AL  = Cache level                                                   ;  
;          AH  = Cache type                                                    ;    
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperCacheSummaryEx:
push ecx esi 0 0 ebp
mov ebp,esp
test ecx,ecx
jz .nodata
movzx ebx,ah
xchg edx,eax
.scanCaches:
cmp dword [esi + 00],2
jne .done
cmp byte [esi + 08],dl
jne .done
cmp dword [esi + 16],ebx
jne .done
mov eax,[esi + 12]
mov [ebp + 04],eax
inc dword [ebp + 08]
.done:
mov eax,[esi + 04]
add esi,eax
cmp esi,ecx
jb .scanCaches
cmp dword [ebp + 08],0
je .nodata
mov bh,dl
mov edx,edi
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
push edi
lea edi,[edx + 01]
mov ax,STR_TP_L
call PoolStringWrite
mov al,bh
or al,30h
stosb
movzx ax,bl
add ax,STR_TP_UNIFIED
lea edi,[edx + 04] 
call RelationNameEntry
lea edi,[edx + 18]
mov bl,0FFh
push edx
mov eax,[ebp + 04]
xor edx,edx
call SizePrint64
pop edx
lea edi,[edx + 41]
mov ax,STR_TP_X
call PoolStringWrite
mov eax,[ebp + 08]
mov bl,0
call DecimalPrint32
pop edi
.nodata:
pop ebp eax eax esi ecx
ret

;--- Pre-blank string with CR, LF, write parameter name, set text pointers ----;
; INPUT:   EBP = Pointer to destination buffer                                 ;
;          EAX = Number write to string                                        ; 
; OUTPUT:  EBP = Updated pointer to destination buffer                         ;   
;------------------------------------------------------------------------------;

HelperNumberString:
push ecx eax
mov edi,ebp
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop eax
mov ebp,edi
lea edi,[ebp - 82 + 01]
mov bl,0
call DecimalPrint32
pop ecx
ret

;---------- Copy text string with chars checks --------------------------------;
; INPUT:   EBX = Source base                                                   ;
;          EBP = Destination base                                              ;
;          AL  = Offset for source string                                      ;
;          AH  = Offset for destination string                                 ;
;          EAX.[31-16] = String size                                           ;    
; OUTPUT:  EDI = Last used updated pointer                                     ;
;          String at [EBP + AH] size EAX.[31-16] updated                       ;   
;------------------------------------------------------------------------------;

HelperCopyString:
movzx ecx,al
lea esi,[ebx + ecx]
movzx ecx,ah
lea edi,[ebp + ecx]
xor ecx,ecx
shld ecx,eax,16
.copy:
lodsb
cmp al,0
je .space
cmp al,' '
jb .change
cmp al,'z'
jbe .store 
.change:
mov al,'.'
jmp .store
.space:
mov al,' '
.store:
stosb
loop .copy
ret

;---------- Write decimal number text string, 8 bit value ---------------------;
; INPUT:   EBX = Source base                                                   ;
;          EBP = Destination base                                              ;
;          AL  = Offset for source data contain 8-bit value                    ;
;          AH  = Offset for destination string                                 ;
; OUTPUT:  EDI = Last used updated pointer                                     ;
;          String at [EBP + AH] updated                                        ;   
;------------------------------------------------------------------------------;

HelperDecimalString:
push ebx
movzx ecx,ah
lea edi,[ebp + ecx]
movzx ecx,al
movzx eax,byte [ebx + ecx]
mov bl,0
call DecimalPrint32 
pop ebx
ret

;---------- Write hex number text string, 32 bit value ------------------------;
; INPUT:   RBX = Source base                                                   ;
;          RBP = Destination base                                              ;
;          AL  = Offset for source data contain 8-bit value                    ;
;          AH  = Offset for destination string                                 ;
; OUTPUT:  EDI = Last used updated pointer                                     ;
;          String at [RBP + AH] updated                                        ;   
;------------------------------------------------------------------------------;

HelperHexString:
call HelperHexStringNh
mov al,'h'
stosb 
ret
HelperHexStringNh:    ; This entry point for write without "h"
movzx ecx,ah
lea edi,[ebp + ecx]
movzx ecx,al
mov eax,[ebx + ecx]
jmp HexPrint32

