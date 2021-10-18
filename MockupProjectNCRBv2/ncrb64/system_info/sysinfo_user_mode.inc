; TODO.
; MAKE SEPARATE INCLUDE FILES = BINARY + TEXT + SCREEN 0 TEXT AT ONE FILE,
; DON'T USE SEQUENCE "ALL BINARY + ALL TEXT".


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
; Can destroy registers, volatile by Microsoft x64 calling convention.         ;
;------------------------------------------------------------------------------;

SysinfoUserMode:
push rbx rsi rdi rbp r12 r13 r14 r15
push 0                       ; qword at [r13] = temporary variable
mov r13,rsp                  ; R13 used for RSP restore before return 
and rsp,0FFFFFFFFFFFFFFF0h   ; Stack alignment by Microsoft x64 calling convention
sub rsp,32                   ; Parameter shadow by Microsoft x64 calling convention 
cld
mov r15,[Registry]           ; R15 = Registry base address

;---------- Check CPUID instruction support, get 12-chars vendor string -------;

lea rdi,[r15 + REGISTRY64.cpuData.vendorString]  ; RDI = Destination pointer
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

xor eax,eax
mov rsi,[r15 + REGISTRY64.appData.lockedDataCpuCommon]
test rsi,rsi
jz @f
call HelperBuildBitmap
@@:
bts rax,63
stosq

;------- Build CPU AVX512 features bitmap, for system information screen ------;

xor eax,eax
mov rsi,[r15 + REGISTRY64.appData.lockedDataCpuAvx512]
test rsi,rsi
jz @f
call HelperBuildBitmap
@@:
stosq

;------- Build OS context features bitmap, for system information screen ------;

xor eax,eax
mov rsi,[r15 + REGISTRY64.appData.lockedDataOsContext]
test rsi,rsi
jz @f
call HelperBuildBitmap
@@:
test rax,rax
jz @f
bts rax,63
@@:
stosq

;------- Build CPU methods features bitmap, for system information screen -----;

; xor eax,eax    ; *** DEBUG ***
; stosq          ; *** DEBUG ***

;---------- Measure TSC frequency ---------------------------------------------;

call HelperMeasureTsc
; TODO. Check for errors, CF=1

;---------- Initializing dynamical import WinAPI list -------------------------; 

lea rsi,[NameKernel32]       ; RSI = Pointer to libraries names array
mov rbx,[r15 + REGISTRY64.appData.lockedImportList] ; RBX = to functions names
lea rdi,[r15 + REGISTRY64.dynaImport]               ; RDI = to fnc. addresses               
.scanLibs:
cmp byte [rsi],0
je .doneLibs
mov rcx,rsi                 ; RCX = Parm#1 = Pointer to module name
call [GetModuleHandle]      ; RAX = Return module handle
xchg r14,rax                ; R14 = DLL handle, XCHG is compact
.scanFnc:
cmp byte [rbx],0
je .doneFnc
mov rcx,r14                 ; RCX = Parm#1 = Module handle
mov rdx,rbx                 ; RDX = Parm#2 = Pointer to function name
call [GetProcAddress]       ; RAX = Return function address
stosq                       ; Store function address to import list
.skipName:
inc rbx
cmp byte [rbx],0
jne .skipName
inc rbx
jmp .scanFnc                ; Internal cycle for functions list per one library
.doneFnc:
inc rbx
.skipLib:
lodsb
cmp al,0
jne .skipLib
jmp .scanLibs               ; External cycle for libraries list
.doneLibs:

;---------- Get system and memory information parameters ----------------------;

lea rsi,[r15 + REGISTRY64.dynaImport]
lea rdi,[r15 + REGISTRY64.osData]
mov rax,[rsi + DYNAIMPORT._GlobalMemoryStatusEx]
test rax,rax
jz .error
lea rcx,[rdi + OSDATA.memoryStatusEx]
mov [rcx + MEMORYSTATUSEX_DEF.dwLength],sizeof.MEMORYSTATUSEX_DEF
call rax 
test rax,rax
jz .error
lea rcx,[rdi + OSDATA.systemInfo]
call [GetSystemInfo]
mov rax,[rsi + DYNAIMPORT._GetActiveProcessorGroupCount]
test rax,rax
jz @f
call rax
movzx eax,ax
@@:
mov [rdi + OSDATA.activeProcessorGroupCount],eax
mov rax,[rsi + DYNAIMPORT._GetActiveProcessorCount]
test rax,rax
jz @f
mov ecx,ALL_PROCESSOR_GROUPS
call rax
@@:
mov [rdi + OSDATA.activeProcessorCount],eax
mov rax,[rsi + DYNAIMPORT._GetNumaHighestNodeNumber]
test rax,rax
jz @f
lea rbx,[rdi + OSDATA.numaNodeCount]
mov rcx,rbx
call rax
test rax,rax
jz @f
mov eax,[rbx]
inc eax
@@:               ; If no data, EAX=0 at this point, means no nodes found
mov [rbx],eax

;---------- Get large page size -----------------------------------------------;

mov rax,[rsi + DYNAIMPORT._GetLargePageMinimum]
test rax,rax
jz @f
call rax
@@:
mov [rdi + OSDATA.largePageSize],rax

;---------- Get large page information: try allocation ------------------------;

mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,128 + 32
xor eax,eax
mov [rdi + OSDATA.largePageEnable],eax
cmp [rsi + DYNAIMPORT._OpenProcessToken],rax
je .exit
cmp [rsi + DYNAIMPORT._AdjustTokenPrivileges],rax 
je .exit
cmp [rdi + OSDATA.largePageSize],rax 
je .exit
call [GetCurrentProcess]
test rax,rax
jz .skip
mov rcx,rax
mov edx,MAXIMUM_ALLOWED 
lea r8,[rsp + 120 - 32]
call [rsi + DYNAIMPORT._OpenProcessToken] 
test rax,rax 
jz .skip   
mov rcx,[rsp + 120 - 32]  
xor edx,edx
lea r8,[rsp + 48]
xor r9d,r9d
mov [rsp + 32],r9
mov [rsp + 40],r9
mov [tp.PrivilegeCount],1 
mov [tp.Privileges.Luid.usedpart],SE_LOCK_MEMORY_PRIVILEGE 
and [tp.Privileges.Luid.ignorehigh32bitpart],0  
mov [tp.Privileges.Attributes],SE_PRIVILEGE_ENABLED
call [rsi + DYNAIMPORT._AdjustTokenPrivileges] 
mov rbx,rax 
mov rcx,[rsp + 120 - 32] 
call [CloseHandle] 
.skip:
test rax,rax
jz .exit
xor eax,eax
test rbx,rbx
jz .exit 
call [GetCurrentProcess]
test rax,rax
jz .exit
mov rbx,rax
mov rcx,rax 
xor edx,edx
mov r8,[rdi + OSDATA.largePageSize]
mov r9d,MEM_COMMIT + MEM_LARGE_PAGES
push 0
push PAGE_READWRITE
sub rsp,32
call [VirtualAllocEx]
add rsp,32 + 16
test rax,rax
jz @f
mov rcx,rbx
mov rdx,rax
xor r8d,r8d
mov r9d,MEM_RELEASE
call [VirtualFreeEx]
@@:
test rax,rax
setnz al
movzx eax,al
mov [rdi + OSDATA.largePageEnable],eax 
.exit:
mov rsp,rbp

;---------- Get native operating system information ---------------------------;

mov rax,[rsi + DYNAIMPORT._GetNativeSystemInfo]
test rax,rax
jz @f
lea rcx,[rdi + OSDATA.nativeSystemInfo]
call rax
@@:

;---------- Get platform topology by WinAPI -----------------------------------;
; Start operations with unformatted part of application registry,
; RBP = Pointer for unformatted part addressing 

lea rbp,[r15 + REGISTRY64.unformatted]
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
mov [r15 + REGISTRY64.listTopology.objectStart],rbp
add rbp,r14
mov [r15 + REGISTRY64.listTopology.objectStop],rbp
@@:

;---------- Get platform extended topology by WinAPI --------------------------;
; Separate get each type of information for build ordered list.

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
mov [r15 + REGISTRY64.listTopologyEx.objectStart],rbp
add rbp,r14
mov [r15 + REGISTRY64.listTopologyEx.objectStop],rbp
@@:

;---------- Get NUMA nodes list -----------------------------------------------;

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
mov [r15 + REGISTRY64.listNuma.objectStart],rbp
mov [r15 + REGISTRY64.listNuma.objectStop],rdi
mov rbp,rdi
mov rdi,r14
.skipNumaList:

;---------- Get processor groups list -----------------------------------------;

xor ebx,ebx           ; RBX = Node number and compare pattern
cmp [rsi + DYNAIMPORT._GetActiveProcessorGroupCount],rbx  
je .skipGroupList 
cmp [rsi + DYNAIMPORT._GetActiveProcessorCount],rbx       
je .skipGroupList
call [rsi + DYNAIMPORT._GetActiveProcessorGroupCount]
test ax,ax
jz .skipGroupList
movzx r12d,ax 
mov r14,rdi           ; Can't PUSH because parameter shadow for functions
mov rdi,rbp           ; RDI = Pointer, RBP = Start
.groupList:
mov eax,ebx
stosd
mov rcx,rbx           ; RCX = Parm#1 = Group number
call [rsi + DYNAIMPORT._GetActiveProcessorCount]
; no check for EAX = 0, empty group is acceptable ?
stosd
inc ebx
cmp ebx,r12d
jb .groupList
.doneGroupList:
mov [r15 + REGISTRY64.listGroup.objectStart],rbp
mov [r15 + REGISTRY64.listGroup.objectStop],rdi
mov rbp,rdi
mov rdi,r14
.skipGroupList:

;---------- Get ACPI tables list ----------------------------------------------;

push rsi rdi
mov rcx,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea rdx,[rcx + BINDLIST.bindMadt]
mov r8d,2
.writeNotAvl:  ; TODO. USE BINDERS FOR THIS OPERATION.
mov ax,STR_N_A
lea rdi,[rdx + BINDACPI.oem]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDACPI.manufact]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDACPI.oemRev]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDACPI.comment]
call PoolStringWrite
add rdx,sizeof.BINDACPI
dec r8d
jnz .writeNotAvl
pop rdi rsi

xor eax,eax
cmp [rsi + DYNAIMPORT._EnumSystemFirmwareTables],rax
je .skipAcpiList
cmp [rsi + DYNAIMPORT._GetSystemFirmwareTable],rax
je .skipAcpiList
call HelperAlignment
mov r8d,VALUE_BUFFER_LIMIT  ; R8  = Parm#3 = Buffer size limit
lea rdx,[rbp + 16]          ; RDX = Parm#2 = Pointer to buffer
mov ecx,'IPCA'              ; RCX = Parm#1 = Signature, select data type = ACPI
call [rsi + DYNAIMPORT._EnumSystemFirmwareTables]
test rax,rax
jz .error
cmp rax,VALUE_BUFFER_LIMIT
ja .error
mov rbx,rbp
lea r14,[rbp + 16 + rax]    ; R14 = Pointer to 36-byte entries list
mov [r13],r14               ; [R13] = Limit of 4-byte entries  
mov [rbx],r14
add rbx,16                  ; RBX = Pointer to 4-byte tables signatures list
imul rax,rax,9
lea r12,[r14 + rax]         ; R12 = Pointer to buffer for read table
test r12l,00001111b
jz .scanAcpiList
and r12l,11110000b
add r12,10h 

.scanAcpiList:
mov r9,VALUE_BUFFER_LIMIT   ; R9  = Parm#4 = Buffer size limit  
mov r8,r12                  ; R8  = Parm#3 = Buffer pointer
mov edx,[rbx]               ; RDX = Parm#2 = Table signature
mov ecx,'IPCA'              ; RCX = Parm#1 = Table provider signature
call [rsi + DYNAIMPORT._GetSystemFirmwareTable]
test rax,rax
jz .skipOneTable
cmp rax,VALUE_BUFFER_LIMIT
ja .skipOneTable 
push rsi rdi
cld
mov ecx,9
mov rsi,r12
mov rdi,r14
rep movsd
mov r14,rdi

;--- start of addition for show ACPI tables MADT and SRAT at screen 0 ---------;

mov rcx,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea rdx,[rcx + BINDLIST.acpiEnable]
or byte [rdx],00000001b
mov r8d,[r12]
cmp r8d,'APIC'
je .madtFound
cmp r8d,'SRAT'
je .sratFound
jmp .skipTable
.madtFound:
or byte [rdx],00000010b
lea rdx,[rcx + BINDLIST.bindMadt]
jmp .copyAcpi
.sratFound:
or byte [rdx],00000100b
lea rdx,[rcx + BINDLIST.bindSrat]
.copyAcpi:
lea rsi,[r12 + 10]
lea rdi,[rdx + BINDACPI.oem]
mov ecx,6
call HelperCopyAcpi
lea rsi,[r12 + 16]
lea rdi,[rdx + BINDACPI.manufact]
mov ecx,8
call HelperCopyAcpi
lea rdi,[rdx + BINDACPI.oemRev]
mov ax,STR_OEM_REV
call PoolStringWrite
mov eax,[r12 + 24]
call HexPrint32
mov ax,0000 + 'h'
stosw 
cmp r8d,'SRAT'
je .sratParse
.madtParse:        ; Interpreting ACPI MADT table, get number of APICs
lea rsi,[r12 + 44] ; RSI = Pointer to APIC list in the MADT
mov ecx,[r12 + 4]
add rcx,r12        ; RCX = Table limit
xor r8d,r8d        ; R8D = Counter of Local APICs (MADT), start clear R8D = 0
xor r9d,r9d        ; R9D = Counter of I/O APICs (MADT), start clear R9D = 0
.madtScanApics:    ; This code for parse ACPI MADT, signature = "APIC"
mov al,[rsi]
cmp al,0           ; 0 = Local APIC
jne @f             ; Go skip if not a Local APIC
test byte [rsi + 4],00000001b
jz @f              ; Go skip if Local APIC disabled
inc r8d            ; counter+1 if local APIC found
@@:
cmp al,1			     ; 1 = I/O APIC
jne @f				     ; Go skip if not a I/O APIC
inc r9d 			     ; +1 if found
@@:
movzx rax,byte [rsi + 1] ; RAX = Size of structure, for skip it and access next
add rsi,rax
cmp rsi,rcx
jb .madtScanApics  ; Cycle for scan structures in the MADT
push rbx
lea rdi,[rdx + BINDACPI.comment]
mov ax,STR_LOCAL_APICS
call PoolStringWrite 
mov bl,0
xchg eax,r8d
call DecimalPrint32
mov ax,', '
stosw
mov ax,STR_IO_APICS
call PoolStringWrite 
xchg eax,r9d
call DecimalPrint32
mov al,0
stosb
pop rbx
jmp .skipTable
.sratParse:
push rbx

; UNDER CONSTRUCTION.

lea rdi,[rdx + BINDACPI.comment]
mov ax,STR_DOMAINS
call PoolStringWrite 
mov bl,0
xchg eax,r8d
call DecimalPrint32
mov ax,', '
stosw
mov ax,STR_CPUS
call PoolStringWrite 
xchg eax,r9d
call DecimalPrint32
mov ax,', '
stosw
mov ax,STR_RAMS
call PoolStringWrite 
xchg eax,r10d
call DecimalPrint32
mov al,0
stosb
pop rbx
.skipTable:

;--- end of addition for show ACPI tables MADT and SRAT at screen 0 -----------;

pop rdi rsi
.skipOneTable:
add rbx,4
cmp rbx,[r13]
jb .scanAcpiList 

mov [r15 + REGISTRY64.listAcpi.objectStart],rbp
mov [r15 + REGISTRY64.listAcpi.objectStop],r14
mov rbp,r14
.skipAcpiList:

;---------- Get affinized CPUID dump ------------------------------------------;

push rdi r13
sub rsp,32
mov rdi,rbp

cmp [rsi + DYNAIMPORT._SetThreadAffinityMask],0
je .skipAffCpuid
call [GetCurrentThread]
test rax,rax
jz .skipAffCpuid 
mov r13,rax              ; R13 = Current thread handle
mov r14d,[r15 + REGISTRY64.osData.systemInfo.dwNumberOfProcessors]
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
mov [r15 + REGISTRY64.listAffCpuid.objectStart],rbp
mov [r15 + REGISTRY64.listAffCpuid.objectStop],rdi
mov rbp,rdi
.skipAffCpuid:

add rsp,32
pop r13 rdi

;---------- Build text block for operating system information screen ----------; 
; Start text blocks build operations.

mov r14,rdi
mov [r15 + REGISTRY64.textOs.objectStart],rbp
mov ax,STR_OS_MEMORY_LOAD                        ; Memory load percentage
call HelperOsInfoString
mov eax,[r14 + OSDATA.memoryStatusEx.dwMemoryLoad]
push rax
mov bl,0
call DecimalPrint32
mov ax,' %'
stosw
pop rax
mov rdi,r8
call HexPrint32 
mov ax,STR_OS_TOTAL_PHYSICAL                     ; Total physical memory
call HelperOsInfoString
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
mov rdi,rbp
mov ax,0A0Dh                                     ; Interval
stosw
mov rbp,rdi 
mov bh,0                                         ; BH = 0 means system info
call HelperSystemInfo
mov rdi,rbp
mov ax,0A0Dh                                     ; Interval
stosw
mov rbp,rdi 
mov ax,STR_OS_PROC_CURRENT                       ; Processors at currebt group
call HelperOsInfoString
mov al,SYSTEM_INFO.dwNumberOfProcessors
call HelperOsInfoNumber32
mov ecx,[r15 + REGISTRY64.osData.activeProcessorCount]      ; Processors total
jecxz @f
push rcx
mov ax,STR_OS_PROC_TOTAL
call HelperOsInfoString
pop rax
call HelperOsInfoValue32
@@:
mov ecx,[r15 + REGISTRY64.osData.activeProcessorGroupCount] ; Processors groups
jecxz @f
push rcx
mov ax,STR_OS_PROC_GROUPS
call HelperOsInfoString
pop rax
call HelperOsInfoValue32
@@:
mov ecx,[r15 + REGISTRY64.osData.numaNodeCount]  ; NUMA domains
jecxz @f
push rcx
mov ax,STR_OS_NUMA_DOMAINS
call HelperOsInfoString
pop rax
call HelperOsInfoValue32
@@:
mov ax,STR_OS_NORMAL_PAGE                        ; Normal page size
call HelperOsInfoString
mov al,SYSTEM_INFO.dwPageSize
call HelperOsInfoSizeAuto32
mov rcx,[r15 + REGISTRY64.osData.largePageSize]  ; Large page size and status
jrcxz .skipLargePages
push rcx
mov ax,STR_OS_LARGE_PAGE
call HelperOsInfoString
pop rax
push rax
mov bl,0FFh
call SizePrint64
cmp [r15 + REGISTRY64.osData.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
call PoolStringWrite
pop rax
mov rdi,r8
call HexPrint64
.skipLargePages:
mov al,0
stosb
mov rbp,rdi
mov [r15 + REGISTRY64.textOs.objectStop],rbp

;---------- Additions for screen 0 information --------------------------------;

mov rdx,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
add rdx,BINDLIST.bindSys
lea rdi,[rdx + BINDSYS.procTotal]
mov eax,[r15 + REGISTRY64.osData.activeProcessorCount]      ; Processors total
mov bl,0
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.groups]
mov eax,[r15 + REGISTRY64.osData.activeProcessorGroupCount] ; Processors groups
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.procCur]
mov eax,[r15 + REGISTRY64.osData.systemInfo.dwNumberOfProcessors]
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.numaNodes]
mov eax,[r15 + REGISTRY64.osData.numaNodeCount]  ; NUMA domains
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.memPhys]
mov rax,[r15 + REGISTRY64.osData.memoryStatusEx.ullTotalPhys]
mov bl,2
call SizePrint64
mov al,0
stosb
lea rdi,[rdx + BINDSYS.memAvail]
mov rax,[r15 + REGISTRY64.osData.memoryStatusEx.ullAvailPhys]
call SizePrint64
mov al,0
stosb
lea rdi,[rdx + BINDSYS.largePage]
mov rax,[r15 + REGISTRY64.osData.largePageSize]
mov bl,0FFh
call SizePrint64
mov al,0
stosb
lea rdi,[rdx + BINDSYS.largeEnable]
cmp [r15 + REGISTRY64.osData.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
call PoolStringWrite
mov al,0
stosb

;---------- Build text block for native operating system information screen ---;

mov [r15 + REGISTRY64.textNativeOs.objectStart],rbp
mov bh,1                                  ; BH = 1 means native system info
call HelperSystemInfo
mov al,0
stosb
mov rbp,rdi
mov [r15 + REGISTRY64.textNativeOs.objectStop],rbp

;---------- Build text block for processor details screen ---------------------;

; UNDER CONSTRUCTION.

;---------- Build text block for topology screen, first block = list ----------;

; TODO. REDESIGN CYCLE WITH MORE EFFECTIVE CPU REGISTERS USAGE,
; WITHOUT STACK VARIABLES.

TEMP_THREADS   EQU  dword [rsp + 00 + 08]
TEMP_CORES     EQU  dword [rsp + 04 + 08]
TEMP_SOCKETS   EQU  dword [rsp + 08 + 08]
TEMP_RESERVED  EQU  dword [rsp + 12 + 08]   

mov [r15 + REGISTRY64.textTopology1.objectStart],rbp
mov rsi,[r15 + REGISTRY64.listTopology.objectStart]
mov rcx,[r15 + REGISTRY64.listTopology.objectStop]
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
mov [r15 + REGISTRY64.textTopology1.objectStop],rbp

;---------- Support screen 0 threads, cores, sockets --------------------------;

lea rax,[r15 + REGISTRY64.sysParms.summaryTopology]
mov qword [rax + SUMMARYTOPOLOGY.threads],r8
mov [rax + SUMMARYTOPOLOGY.sockets],r9d

;---------- Build text block for topology screen, second block = summary ------;

; TODO. OPTIMIZE BY ELIMINATE DUAL ADDRESS CALCULATION: FOR "N/A" AND FOR VALUES
push rsi rcx
mov rcx,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
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

mov [r15 + REGISTRY64.textTopology2.objectStart],rbp
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
mov [r15 + REGISTRY64.textTopology2.objectStop],rbp

;--- Build text block for extended topology screen, first block = list --------;

mov [r15 + REGISTRY64.textTopologyEx1.objectStart],rbp
mov rsi,[r15 + REGISTRY64.listTopologyEx.objectStart]
mov rcx,[r15 + REGISTRY64.listTopologyEx.objectStop]
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
mov [r15 + REGISTRY64.textTopologyEx1.objectStop],rbp

;---------- Support screen 0 threads, cores, sockets ( variables ) ------------;

lea rax,[r15 + REGISTRY64.sysParms.summaryTopology]
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

mov rdx,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea r8,[r15 + REGISTRY64.sysParms.summaryTopology]
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

mov [r15 + REGISTRY64.textTopologyEx2.objectStart],rbp
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
mov [r15 + REGISTRY64.textTopologyEx2.objectStop],rbp

;---------- Build text block for NUMA domains list screen ---------------------;

mov rsi,[r15 + REGISTRY64.listNuma.objectStart]
mov rcx,[r15 + REGISTRY64.listNuma.objectStop]
test rcx,rcx
jz .noNuma
mov [r15 + REGISTRY64.textNuma.objectStart],rbp
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
mov rsi,[r15 + REGISTRY64.listNuma.objectStart]
lodsd  ; this for ADD RSI,4
mov rdi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
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
mov rsi,[r15 + REGISTRY64.listNuma.objectStart]
lodsd  ; this for ADD RSI,4
mov rdi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
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
mov [r15 + REGISTRY64.textNuma.objectStop],rbp

.noNuma:

;---------- Build text block for processor groups list screen -----------------;

mov rsi,[r15 + REGISTRY64.listGroup.objectStart]
mov rcx,[r15 + REGISTRY64.listGroup.objectStop]
test rcx,rcx
jz .noGroups
mov [r15 + REGISTRY64.textGroup.objectStart],rbp
.groupsScan:
cmp rsi,rcx
jae .groupsDone
lodsd
call HelperNumberString
lea rdi,[rbp - 82 + 10]
lodsd
mov bl,0
call DecimalPrint32
jmp .groupsScan 
.groupsDone:
mov al,0
stosb
mov rbp,rdi
mov [r15 + REGISTRY64.textGroup.objectStop],rbp
.noGroups:

;---------- Build text block for ACPI screen, first block = tables list -------;

mov rsi,[r15 + REGISTRY64.listAcpi.objectStart]
mov rcx,[r15 + REGISTRY64.listAcpi.objectStop]
test rcx,rcx
jz .noAcpi
mov rcx,[rsi]
mov rbx,rcx
sub rcx,rsi
sub ecx,16
shr ecx,2
jrcxz .noAcpi
mov [r15 + REGISTRY64.textAcpi1.objectStart],rbp
.acpiScan:
mov rdi,rbp
push rcx
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
add rbx,36
add rbp,82
pop rcx
loop .acpiScan 
.acpiDone:
mov al,0
stosb
mov rbp,rdi
mov [r15 + REGISTRY64.textAcpi1.objectStop],rbp
.noAcpi: 

;--- Build text block for ACPI screen, second block = tables detection --------;

mov r14,[r15 + REGISTRY64.appData.lockedDataAcpi]
test r14,r14
jz .noAcpiData 
mov rsi,[r15 + REGISTRY64.listAcpi.objectStart]
mov rcx,[r15 + REGISTRY64.listAcpi.objectStop]
jrcxz .noAcpiData
mov rcx,[rsi]
sub rcx,rsi
sub ecx,16
shr ecx,2
jrcxz .noAcpiData
lea rbx,[rsi + 16]
mov [r15 + REGISTRY64.textAcpi2.objectStart],rbp
.acpiDataScan:
mov rdi,rbp
push rcx
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov eax,00040100h
call HelperCopyString
mov ax,', '
stosw
mov rsi,r14
mov edx,[rbx]
.findAcpi:
cmp byte [rsi],0
je .notFoundAcpi
cmp edx,[rsi]
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
add rsi,4
call StringWrite
.doneAcpi:
add rbx,4
add rbp,82
pop rcx
loop .acpiDataScan
.acpiDataDone:
mov al,0
stosb
mov rbp,rdi
mov [r15 + REGISTRY64.textAcpi2.objectStop],rbp
.noAcpiData:

;---------- Build text block for affinized CPUID dump -------------------------;

mov rsi,[r15 + REGISTRY64.listAffCpuid.objectStart]
mov rcx,[r15 + REGISTRY64.listAffCpuid.objectStop]
test rcx,rcx
jz .noAffCpuidData
mov [r15 + REGISTRY64.textAffCpuid.objectStart],rbp
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
mov [r15 + REGISTRY64.textAffCpuid.objectStop],rbp
.noAffCpuidData:

;---------- Build bind buffer -------------------------------------------------;
; Start operations with bind buffer part of application registry,
; RDI = Pointer for bind buffer addressing.
; R14 = Pointer for CPU data addressing in the application registry 

mov rdi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea r14,[r15 + REGISTRY64.cpuData.vendorString]

;---------- CPU vendor string, 12 chars ---------------------------------------; 

mov rax,r14
stosq

;---------- CPU signature: type, family, model, stepping ----------------------;

mov ax,STR_TFMS
call PoolStringWrite
mov eax,[r14 + CPUDATA.cpuSignature]
call HexPrint32
mov ax,STR_H
call PoolStringWrite
mov al,0
stosb 

;---------- CPU model string, maximum 48 chars --------------------------------;

lea rax,[r14 + CPUDATA.modelString]
stosq

;---------- TSC frequency -----------------------------------------------------;

mov ecx,32
mov al,0
rep stosb
push rdi
sub rdi,32
mov ax,STR_TSC
call PoolStringWrite
finit
push 1000000
fild qword [r14 + CPUDATA.tscClockHz] 
fidiv dword [rsp]
fstp qword [rsp]
pop rax
mov bx,0100h
call DoublePrint
mov ax,STR_MHZ
call PoolStringWrite
pop rdi

;---------- CPU instructions and OS context management features bitmaps -------;

mov rax,[r14 + CPUDATA.extractedFeaturesBitmap]
stosq
mov rax,[r14 + CPUDATA.extractedAvx512Bitmap]
stosq
mov rax,[r14 + CPUDATA.extractedContextBitmap]
stosq

;---------- Skip some screen 0 information ------------------------------------;

add rdi, 1 + ( sizeof.BINDACPI * 2 ) + sizeof.BINDCACHE + sizeof.BINDTOPOLOGY + sizeof.BINDSYS      ; RESERVED

;---------- Memory and cache benchmarks methods availability bitmap -----------;

; mov rax,[r14 + CPUDATA.extractedMethodsBitmap]
; stosq

;---------- Text blocks for viewers -------------------------------------------;

mov rax,[r15 + REGISTRY64.textOs.objectStart]
stosq       ; Operating system information
mov rax,[r15 + REGISTRY64.textNativeOs.objectStart]
stosq       ; Native operating system information
mov rax,[r15 + REGISTRY64.textTopology1.objectStart]
stosq       ; Topology information by operating system, text block 1, list
mov rax,[r15 + REGISTRY64.textTopology2.objectStart]
stosq       ; Topology information by operating system, text block 2, summary
mov rax,[r15 + REGISTRY64.textTopologyEx1.objectStart]
stosq       ; Extended topology information by OS, text block 1, list
mov rax,[r15 + REGISTRY64.textTopologyEx2.objectStart]
stosq       ; Extended topology information by OS, text block 2, summary
mov rax,[r15 + REGISTRY64.textNuma.objectStart]
stosq       ; NUMA domains list by OS
mov rax,[r15 + REGISTRY64.textGroup.objectStart]
stosq       ; Processor groups list by OS
mov rax,[r15 + REGISTRY64.textAcpi1.objectStart]
stosq       ; ACPI tables, text block 1 = tables list
mov rax,[r15 + REGISTRY64.textAcpi2.objectStart]
stosq       ; ACPI tables, text block 2 = tables detection by data base
mov rax,[r15 + REGISTRY64.textAffCpuid.objectStart]
stosq       ; Affinized CPUID dump

;---------- Data for memory and cache benchmark setup -------------------------;
; TODO. Move this to separate include file with subroutine.
; TODO. Remove "magic numbers" use names for bits positions.

mov rdi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]

;---------- Benchmark methods based on CPU instructions -----------------------; 

mov eax,dword [r15 + REGISTRY64.cpuData.extractedFeaturesBitmap]
mov edx,dword [r15 + REGISTRY64.cpuData.extractedContextBitmap]
mov rbx,0001000000000555h   ; Latency by LCM, REP MOVS, REP STOS, NOT, MOV (3)
test al,00000001b
jz @f
or rbx,00015000h        ; MMX (3) 
@@:
test al,00000010b
jz @f
or rbx,00540000h        ; SSE (3) 
@@:
test al,10000000b
jz @f
test dl,00000010b
jz @f
or rbx,15000000h        ; AVX (3) 
@@:
test ah,00000010b
jz @f
mov cl,dl
and cl,00011100b
cmp cl,00011100b
jne @f
mov rcx,144540000000h   ; AVX512 (3), FMA 512, VGATHERQPD 512, VSCATTERQPD 512
or rbx,rcx 
@@:
test ah,01000000b
jz @f
bts rbx,36              ; FMA 256
@@:
test ah,00000001b
jz @f
bts rbx,40              ; VGATHERQPD 256
@@:
test ah,10000000b
jz @f
bts rbx,46              ; CLZERO (AMD)
@@:
test ah,00000100b
jz @f
bts rbx,50              ; Latency by RDRAND
@@:

mov ecx,0C0000000h
test ebx,ecx
jnz @f
mov ecx,03000000h
test ebx,ecx
jnz @f
mov ecx,00C00000h
test ebx,ecx
jnz @f
mov ecx,00003000h
test ebx,ecx
jnz @f
mov ecx,00000003h
@@:
or rbx,rcx
mov qword [rdi + BINDLIST.setMemMethod],rbx

;--- Benchmark objects: cache memory, DRAM or user-defined block size ---------;
; Plus custom block size

lea rsi,[r15 + REGISTRY64.sysParms.summaryCache]
mov ebx,010100000000b      ; Custom block, DRAM
xor eax,eax
mov rcx,[rsi + SUMMARYCACHE.sizeL1D]
jrcxz @f 
or bl,00000011b            ; L1 data cache
xchg rax,rcx
@@:
mov rcx,[rsi + SUMMARYCACHE.sizeL2U]
jrcxz @f
or bl,00000100b            ; L2 unified cache
test rax,rax
jnz @f
xchg rax,rcx
or bl,00001000b
@@:
mov rcx,[rsi + SUMMARYCACHE.sizeL3U]
jrcxz @f
or bl,00010000b            ; L3 unified cache
test rax,rax
jnz @f
xchg rax,rcx
or bl,00100000b
@@:
mov rcx,[rsi + SUMMARYCACHE.sizeL4U]
jrcxz @f
or bl,00010000b            ; L4 unified cache
test rax,rax
jnz @f
xchg rax,rcx
or bl,10000000b
@@:
test rax,rax
jnz @f
or bh,00001000b
mov eax,32768
@@:
mov dword [rdi + BINDLIST.setMemObject],ebx
mov [rdi + BINDLIST.setBlkCustom],rax

;---------- Benchmark options: SMP/SMT ----------------------------------------;

lea rsi,[r15 + REGISTRY64.sysParms.summaryTopology]
xor ebx,ebx
mov eax,[rsi + SUMMARYTOPOLOGY.threads]
cmp eax,2
jb @f
inc ebx           ; Parallel threads (SMP)
@@:
shr eax,1
jz @f
cmp eax,[rsi + SUMMARYTOPOLOGY.cores]
jne @f
or bl,00000100b   ; Hyper threading (SMT)
@@:
lea rsi,[r15 + REGISTRY64.osData]
cmp [rsi + OSDATA.activeProcessorGroupCount],2
jb @f
or bl,00010000b   ; Processor groups (PG)
@@:
mov word [rdi + BINDLIST.setMemSmp],bx

;---------- Benchmark option: NUMA --------------------------------------------;

mov bx,00000010b
cmp [rsi + OSDATA.numaNodeCount],2
jb @f
mov bl,01110101b   ; Unaware, Single domain, optimal, non-optimal
@@:
mov word [rdi + BINDLIST.setMemNuma],bx

;---------- Benchmark option: Large Pages -------------------------------------;

xor ebx,ebx
cmp [rsi + OSDATA.largePageEnable],ebx
je @f
inc ebx
@@:
mov word [rdi + BINDLIST.setMemLpages],bx

;---------- Benchmark options, unconditionally set ----------------------------;

mov word [rdi + BINDLIST.setMemAccess],00000010b  ; Access step
mov word [rdi + BINDLIST.setMemPref],00000010b    ; 01010101b  ; Prefetch modes
mov word [rdi + BINDLIST.setMemMeas],11010101b    ; Measure precision modes
mov word [rdi + BINDLIST.setMemPix],00010111b     ; Pixels approximation modes

;---------- Exit points -------------------------------------------------------;

; TODO. Error Handling.
.error:
; TODO. Error Handling.
mov rsp,r13
pop rcx     ; this POP for remove temporary variable
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret



;---------- Align memory pointer, required for WinAPI calls -------------------;
; INPUT:   RBP = Pointer                                                       ;
; OUTPUT:  RBP = Aligned pointer, updated if required                          ;
;------------------------------------------------------------------------------;

HelperAlignment:
test bpl,00001111b
jz @f
and bpl,0F0h
add rbp,010h
@@: 
ret

;---------- Detect CPUID support and execute CPUID function #0. ---------------;
; Note CPUID can be supported by CPU but locked by Virtual Monitor.            ;
; Note check bit EFLAGS.21 toggleable, it is CPUID support indicator.          ;
; Note probably wrong result if debug trace this subroutine code.              ;
;                                                                              ;
; INPUT:   RDI = Destination pointer for save CPU Vendor String                ;
;                                                                              ;
; OUTPUT:  CF flag = Status: 0(NC)=Support OK, 1(C)=Not supported              ;
;          Output EAX, RDI, Destination memory valid only if CF=0(NC)          ;
;          EAX = Largest standard CPUID function supported                     ;
;          RDI = Input RDI + 13,                                               ; 
;                string size fixed = 12 chars + 0 = terminator byte            ;
;          Destination memory at [input RDI] =                                 ;
;           bytes [00-11] = CPU vendor string                                  ;
;           byte  [12-12] = 00h, string terminator                             ;
;------------------------------------------------------------------------------;

HelperCheckCpuid:

;---------- Check for ID bit writeable for "1" --------------------------------;

mov ebx,21
pushf                     ; In the 64-bit mode, push RFLAGS
pop rax
bts eax,ebx               ; Set EAX.21=1
push rax
popf                      ; Load RFLAGS with RFLAGS.21=1
pushf                     ; Store RFLAGS
pop rax                   ; Load RFLAGS to RAX
btr eax,ebx               ; Check EAX.21=1, Set EAX.21=0
jnc .absent               ; Go error branch if cannot set EFLAGS.21=1

;---------- Check for ID bit writeable for "0" --------------------------------;

push rax
popf                      ; Load RFLAGS with RFLAGS.21=0
pushf                     ; Store RFLAGS
pop rax                   ; Load RFLAGS to RAX
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
; INPUT:   RDI = Destination pointer for save CPU Vendor String                ;
;                                                                              ;
; OUTPUT:  RDI = Input RDI + 48 + 1 , string size fixed = 48 bytes             ;
;          Destination memory at [input RDI] =                                 ; 
;           bytes [00-47] = CPU Vendor String                                  ;
;           byte  [48-48] = 00h, terminator for copy by StringWrite            ;
;          String formatted by subroutine, left spaces deleted.                ;
;          If feature not supported, string contain "None",                    ;
;          but errors not reported, NCRB can work without this feature         ;
;------------------------------------------------------------------------------;

HelperGetCpuName:
cld                          ; Clear direction, because STOSB/STOSD used
push rbx rsi

;---------- Blank and prepare for not support mode ----------------------------;

mov ecx,48                   ; RCX = String length, also clear bits RCX[63-32]
mov al,' '                   ; AL = Space
rep stosb                    ; Blank string
mov dword [rdi - 48],'None'  ; Default string, if feature not supported
mov byte [rdi],0             ; Zero terminator byte 

;---------- Check functions supported -----------------------------------------;

mov esi,80000000h            ; ESI = First extended function = 80000000h
mov eax,esi                  ; EAX = Function for verify support = 80000000h
cpuid                        ; Execute CPUID function 80000000h
lea ebx,[esi + 4]            ; EBX = Maximum function 80000004h
cmp eax,ebx
jb .exit                     ; Go exit if required functions not supported
sub rdi,48                   ; Must be +48 if go exit when not supported

;---------- Get and store CPU name string -------------------------------------;

push rdi
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
pop rdi

;---------- Formatting CPU name string, delete left spaces and byte 00h -------;

mov rsi,rdi
mov ecx,48                 ; RCX = String size limit
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
inc rdi                   ; Skip terminator byte
pop rsi rbx
ret

;---------- Measure CPU TSC (Time Stamp Counter) clock frequency --------------;
; Store results F = Frequency=[Hz].                                            ;
; Call this subroutine only if CPUID and RDTSC both supported.                 ;
;                                                                              ;
; INPUT:   RDI = Destination pointer for save TSC frequency                    ;
;                                                                              ;
; OUTPUT:  CF flag = Status: 0(NC)=Measured OK, 1(C)=Measurement error	       ;
;          Output RDI and destination memory valid only if CF=0(NC)            ;
;          RDI = Input RDI + 8 , buffer size fixed = 8 bytes                   ;
;          Destination memory at [input RDI] = Results                         ;
;           Qword [00-07] = TSC frequency, Hz = delta TSC per 1 second         ;
;------------------------------------------------------------------------------;

HelperMeasureTsc:
cld                        ; Clear direction, because STOSQ used
push rbx rbp rbp           ; Last push for reserve local variable space
mov rbp,rsp                ; RBP used for restore RSP and addressing variables
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32                 ; Make parameters shadow

;---------- Start measure frequency -------------------------------------------;

mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get current count
mov rsi,[rbp]
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get next count for wait 100 ns
cmp rsi,[rbp]
je @b
mov rsi,[rbp]
add rsi,10000000                  ; 10^7 * 100ns = 1 second
rdtsc
shl rdx,32
lea rbx,[rax + rdx]               ; RBX = 64-bit TSC at operation start
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp rsi,[rbp]
ja @b
rdtsc
shl rdx,32
or rax,rdx                        ; RAX = 64-bit TSC at operation end
sub rax,rbx                       ; RAX = Delta TSC
jbe .error

;---------- Store result ------------------------------------------------------; 

stosq     ; Store Frequency, as 64-bit integer value, Hz, delta-TSC per second

;---------- Restore RSP, pop extra registers, exit ----------------------------;

clc             ; CF=0 (NC) means CPU clock measured OK
.exit:
mov rsp,rbp
pop rbp rbp rbx
ret
.error:
stc             ; CF=1 (CY) means CPU clock measured ERROR
jmp .exit

;---------- Interpreting list of entries ENTRY_CPUID, ENTRY_XCR0 --------------;
; INPUT:   RSI = Pointer to builder script                                     ;
; OUTPUT:  RAX = Bitmap, result of builder script                              ;  
;------------------------------------------------------------------------------;

HelperBuildBitmap:
push rsi rdi rbp rbx
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
xchg rax,rbx
pop rbx rbp rdi rsi
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
mov dh,[rsi]
inc rsi
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
push rsi rdi rbp rbx
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
pop rbx rbp rdi rsi
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
push rbp rbx
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
pop rbx rbp
jnc .zero
bts ebx,ebp
.zero:
ret

;--- Pre-blank string with CR, LF, write parameter name, set text pointers ----;
; INPUT:   RBP = Pointer to destination buffer                                 ;
;          AX  = Index for parameter name for write to string                  ; 
; OUTPUT:  RDI = Pointer to parameter value position in the string             ;
;          R8  = Pointer to parameter hex value position in the string         ;
;          RBP = Updated pointer to destination buffer                         ;   
;------------------------------------------------------------------------------;

HelperOsInfoString:
push rsi
mov rdi,rbp
push rax
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop rax
mov rbp,rdi
lea rdi,[rbp - 82 + 01]
call PoolStringWrite
lea rdi,[rbp - 82 + 33]
lea r8,[rbp - 82 + 62]
pop rsi
ret

;---------- Write memory block size to pre-blanked string, include hex --------;
;           This variant for units fixed = megabytes.                          ;
;           This variant for memory status structure.                          ;
;           This variant for 64-bit value.                                     ;
; INPUT:   RDI = Pointer to destination buffer, value position                 ;
;          R8  = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the MEMORYSTATUSEX structure                        ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoSize:
mov bl,2
SizeHelperEntry:
movzx eax,al
mov rcx,[Registry]
cmp bl,0FFh
je .sysinfo
lea rcx,[rcx + REGISTRY64.osData.memoryStatusEx]
jmp .done
.sysinfo:
test bh,bh
jnz .nativeInfo
lea rcx,[rcx + REGISTRY64.osData.systemInfo]
jmp .done
.nativeInfo:
lea rcx,[rcx + REGISTRY64.osData.nativeSystemInfo]
.done:
mov rax,[rcx + rax]
cmp bl,0
je .skip
call SizePrint64
.skip:
mov rdi,r8
jmp HexPrint64

;---------- Write memory block size to pre-blanked string, include hex --------;
;           This variant for units auto selected                               ;
;           This variant for system information structure.                     ;
;           This variant for 64-bit value.                                     ;
; INPUT:   RDI = Pointer to destination buffer, value position                 ;
;          R8  = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoSizeAuto:
mov bl,0FFh
jmp SizeHelperEntry

;---------- Write memory block size to pre-blanked, hex only ------------------;
;           This variant for memory status structure.                          ;
;           This variant for 64-bit value.                                     ;
; INPUT:   R8  = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the MEMORYSTATUSEX structure                        ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoHex:
mov bl,0
jmp SizeHelperEntry 

;---------- Write memory block size to pre-blanked string, include hex --------;
;           This variant for units fixed = megabytes.                          ;
;           This variant for system information structure.                     ;
;           This variant for 32-bit value.                                     ;
; INPUT:   RDI = Pointer to destination buffer, value position                 ;
;          R8  = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoSizeAuto32:
mov bl,0FFh
movzx eax,al
mov rcx,[Registry]
test bh,bh
jnz .nativeInfo
mov eax,[rcx + REGISTRY64.osData.systemInfo + rax]
jmp .doneInfo
.nativeInfo:
mov eax,[rcx + REGISTRY64.osData.nativeSystemInfo + rax]
.doneInfo:
push rax
call SizePrint64
pop rax
mov rdi,r8
jmp HexPrint32

;---------- Write 32-bit number to pre-blanked string, decimal and hex --------;
; INPUT:   RDI = Pointer to destination buffer, decimal value position         ;
;          R8  = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoNumber32:
movzx eax,al
mov rcx,[Registry]
test bh,bh
jnz .nativeInfo
mov eax,[rcx + REGISTRY64.osData.systemInfo + rax]
jmp .doneInfo
.nativeInfo:
mov eax,[rcx + REGISTRY64.osData.nativeSystemInfo + rax]
.doneInfo:
HelperOsInfoValue32:     ; Entry point for EAX = value, RDI, R8 same usage
push rax
mov bl,0
call DecimalPrint32
pop rax
mov rdi,r8
jmp HexPrint32

;---------- Write 16-bit number to pre-blanked string, decimal and hex --------;
; INPUT:   RDI = Pointer to destination buffer, decimal value position         ;
;          R8  = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperOsInfoNumber16:
movzx eax,al
mov rcx,[Registry]
test bh,bh
jnz .nativeInfo
movzx eax,word [rcx + REGISTRY64.osData.systemInfo + rax]
jmp .doneInfo
.nativeInfo:
movzx eax,word [rcx + REGISTRY64.osData.nativeSystemInfo + rax]
.doneInfo:
push rax
mov bl,0
call DecimalPrint32
pop rax
mov rdi,r8
jmp HexPrint16

;---------- Write system information strings ----------------------------------;
; INPUT:   RBP = Pointer to destination buffer                                 ;
;          BH = information type: 0 = OS info , 1 = native OS info             ;
; OUTPUT:  RBP = Updated pointer to destination buffer                         ;
;          RDI = Last used and updated destination pointer                     ;
;------------------------------------------------------------------------------;

HelperSystemInfo:
mov ax,STR_OS_MIN_ADDRESS                        ; Application minimum address
call HelperOsInfoString
mov al,SYSTEM_INFO.lpMinimumApplicationAddress
call HelperOsInfoSizeAuto
mov ax,STR_OS_MAX_ADDRESS                        ; Application maximum address
call HelperOsInfoString
mov al,SYSTEM_INFO.lpMaximumApplicationAddress
call HelperOsInfoSizeAuto
mov ax,STR_OS_PROC_MASK                          ; Active processor mask 
call HelperOsInfoString
mov al,'-'
stosb
test bh,bh
jnz .nativeInfo
mov rax,[r15 + REGISTRY64.osData.systemInfo.dwActiveProcessorMask]
jmp .doneInfo
.nativeInfo:
mov rax,[r15 + REGISTRY64.osData.nativeSystemInfo.dwActiveProcessorMask]
.doneInfo:
mov rdi,r8
call HexPrint64 
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
lea rdi,[rdx + 01]
RelationNameEntry:
push rcx rsi
call PoolStringWrite
pop rsi rcx
ret

;--- Write affinity mask string for text generation by topology parsing -------;
; INPUT:   RDX = Destination base address                                      ;
;          RSI = Pointer to parsed topology entry: affinity mask               ;
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperAffinity:
lea rdi,[rdx + 18]   ; RDI = Destination text pointer
mov rax,[rsi + 00]   ; RAX = Affinity mask
lea r8,[rdi + 16]    ; R8  = Mask text format limit
EntryAffinity:
push rcx rdx rsi
bsf rcx,rax
bsr rdx,rax
cmp ecx,edx
je .modeSingle
push rcx rdx
.scanMask:
bt eax,ecx
jz .endMask
inc ecx
cmp ecx,edx
jb .scanMask
.endMask:
cmp ecx,edx
pop rdx rcx
je .modeInterval
.modeIndividual:
xor ecx,ecx
xor edx,edx
.cycleIndividual:
cmp rdi,r8
jae .overIndividual
shr rax,1
jnc .skipIndividual
push rax
test edx,edx
jz .firstIndividual
mov al,','
stosb
.firstIndividual:
inc edx
mov eax,ecx
mov bl,0
call DecimalPrint32
pop rax
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
pop rsi rdx rcx
ret

;--- Write number value string for text generation by topology parsing --------;
; Has 3 entry points depend on parameter width: 8, 16, 32 bits                 ;
; INPUT:   RDX = Destination base address                                      ;
;          RSI = Pointer to parsed topology entry                              ;
;          AL          = Parameter offset in the topology entry structure      ; 
;          AH          = Text string position offset at destination            ;   
;          EAX.[31-16] = Parameter name String ID at application strings pool  ;  
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperParameter8:         ; Entry point for addressed 8-bit parameter
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,byte [rsi + rax]
ParameterEntry:
push rcx rsi
lea rdi,[rdx + rdi]
push rax
xchg eax,ebx
call PoolStringWrite
pop rax
mov bl,0
call DecimalPrint32
pop rsi rcx
ret
HelperParameter16:         ; Same entry point for addressed 16-bit parameter
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,word [rsi + rax]
jmp ParameterEntry
HelperParameter32:         ; Same entry point for addressed 32-bit parameter
shld ebx,eax,16
movzx edi,ah
movzx eax,al
mov eax,dword [rsi + rax]
jmp ParameterEntry

;--- Write cache size and count string for text gener. by topology parsing ----;
; Has 3 entry points depend on parameter width: 8, 16, 32 bits                 ;
; INPUT:   RSI = Pointer to topology information buffer                        ;
;          RDI = Destination text buffer                                       ;
;          ECX = Entries count at topology information buffer                  ;  
;          AL  = Cache level                                                   ;  
;          AH  = Cache type                                                    ;    
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperCacheSummary:
push rcx rsi rax
test rcx,rcx
jz .nodata
movzx ebx,ah
xchg edx,eax
xor r8d,r8d
xor r9d,r9d

.scanCaches:
cmp dword [rsi + 08],2
jne .done
cmp byte [rsi + 16],dl
jne .done
cmp dword [rsi + 24],ebx
jne .done
mov r8d,[rsi + 20]
inc r9d
.done:
add rsi,32
loop .scanCaches

test r8d,r8d
jz .nodata
mov bh,dl
mov rdx,rdi
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
push rdi
lea rdi,[rdx + 01]
mov ax,STR_TP_L
call PoolStringWrite
mov al,bh
or al,30h
stosb
movzx ax,bl
add ax,STR_TP_UNIFIED
lea rdi,[rdx + 04] 
call RelationNameEntry
lea rdi,[rdx + 18]
mov bl,0FFh
mov eax,r8d
call SizePrint64
lea rdi,[rdx + 41]
mov ax,STR_TP_X
call PoolStringWrite
mov eax,r9d
mov bl,0
call DecimalPrint32
pop rdi

;---------- additions for screen 0 --------------------------------------------;

pop rax
push rax
call HelperSummaryCache

.nodata:
pop rax rsi rcx
ret

HelperSummaryCache:
push rdi
mov rbx,[Registry]
mov rcx,[rbx + REGISTRY64.allocatorBindBuffer.objectStart]
add rbx,REGISTRY64.sysParms.summaryCache
add rcx,BINDLIST.bindCache
lea rsi,[rbx + SUMMARYCACHE.sizeL1C]
lea rdi,[rcx + BINDCACHE.l1c]  
mov dl,00000001b
cmp ax,0101h
je .detected 
lea rsi,[rbx + SUMMARYCACHE.sizeL1D]
lea rdi,[rcx + BINDCACHE.l1d]  
mov dl,00000010b
cmp ax,0201h
je .detected 
lea rsi,[rbx + SUMMARYCACHE.sizeL2U]
lea rdi,[rcx + BINDCACHE.l2u]  
mov dl,00000100b
cmp ax,0002h
je .detected 
lea rsi,[rbx + SUMMARYCACHE.sizeL3U]
lea rdi,[rcx + BINDCACHE.l3u]  
mov dl,00001000b
cmp ax,0003h
je .detected 
lea rsi,[rbx + SUMMARYCACHE.sizeL4U]
lea rdi,[rcx + BINDCACHE.l4u]  
mov dl,00010000b
cmp ax,0004h
je .detected 
jmp .nodata
.detected:
or [rcx + BINDCACHE.cacheBitmap],dl
mov [rsi + 0],r8
mov [rsi + 8],r9d
mov bl,0FFh
mov eax,r8d
call SizePrint64
mov al,' '
stosb
mov ax,STR_TP_X
call PoolStringWrite
mov eax,r9d
mov bl,0
call DecimalPrint32
mov al,0
stosb
.nodata:
pop rdi
ret

;---------- Get topological information ---------------------------------------;
; INPUT:   RBX = WinAPI function address                                       ;
;          RBP = Pointer to buffer for topology information                    ;
;          R14 = Buffer used size accumulator                                  ;
;          R13 = Pointer to temporary DWORD variable, located at stack frame   ;
;          AL  = Relation type by WinAPI encoding                              ;
; OUTPUT:  R14 = Updated buffer size accumulator                               ;
;------------------------------------------------------------------------------;             

HelperTopologyEx:
push rsi rdi
mov rsi,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
movzx edi,al
mov dword [r13],0            ; dword [r13]  = Buffer size variable, stack
mov r8,r13                   ; R8  = Parm#3 = Pointer to size
lea rdx,[rbp + r14]          ; RDX = Parm#2 = Pointer to buffer
mov ecx,edi                  ; RCX = Parm#1 = Relation type
call rbx
test rax,rax
jnz .error
call [GetLastError]
cmp rax,ERROR_BUFFER_LIMIT
jne .error                   ; Go if other errors detected
mov r12d,[r13]
cmp r12d,VALUE_BUFFER_LIMIT
ja .error                    ; Go if required buffer size too big 
mov r8,r13                   ; R8  = Parm#3 = Pointer to size
lea rdx,[rbp + r14]          ; RDX = Parm#2 = Pointer to buffer
mov ecx,edi                  ; RCX = Parm#1 = Relation type
add r14d,r12d
cmp r14d,VALUE_BUFFER_LIMIT
ja .error                    ; Go if required buffer size too big 
call rbx
test rax,rax
jz .error
.exit:
mov rsp,rsi
pop rdi rsi
ret
.error:
stc
jmp .exit

;--- Write affinity mask string for text generation by topology parsing -------;
; INPUT:   RDX = Destination base address                                      ;
;          RSI = Pointer to parsed topology entry: affinity mask               ;
;          AL  = Field offset in the parsed structure                          ; 
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperAffinityGroup:
push rsi
movzx eax,al
add rsi,rax
lea rdi,[rdx + 18]   ; RDI = Destination text pointer
lea r8,[rdi + 16]    ; R8  = Mask text format limit
mov ax,[rsi + 08]    ; AX  = Processor group for affinity mask
call HexPrint16
mov al,'.'
stosb
mov rax,[rsi + 00]   ; RAX = Affinity mask
call EntryAffinity
pop rsi
ret

;--- Write proc. group string for text generation by topology parsing ---------;
; INPUT:   RDX = Destination base address                                      ;
;          RSI = Pointer to parsed topology entry: affinity mask               ;
;          AL  = Field offset in the parsed structure                          ; 
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperAffinityMask:
push rsi
movzx eax,al
add rsi,rax
lea rdi,[rdx + 18]
mov ax,[rsi + 08]
call HexPrint16
pop rsi
ret

;--- Write cache size and count string for text gener. by topology parsing ----;
; Has 3 entry points depend on parameter width: 8, 16, 32 bits                 ;
; INPUT:   RSI = Pointer to topology information buffer                        ;
;          RDI = Destination text buffer                                       ;
;          RCX = Topology information buffer address limit                     ;  
;          AL  = Cache level                                                   ;  
;          AH  = Cache type                                                    ;    
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;

HelperCacheSummaryEx:
push rcx rsi rax
test ecx,ecx
jz .nodata
movzx ebx,ah
xchg edx,eax
xor r8d,r8d
xor r9d,r9d
.scanCaches:
cmp dword [rsi + 00],2
jne .done
cmp byte [rsi + 08],dl
jne .done
cmp dword [rsi + 16],ebx
jne .done
mov r8d,[rsi + 12]
inc r9d
.done:
mov eax,[rsi + 04]
add rsi,rax
cmp rsi,rcx
jb .scanCaches
test r8d,r8d
jz .nodata
mov bh,dl
mov rdx,rdi
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
push rdi
lea rdi,[rdx + 01]
mov ax,STR_TP_L
call PoolStringWrite
mov al,bh
or al,30h
stosb
movzx ax,bl
add ax,STR_TP_UNIFIED
lea rdi,[rdx + 04] 
call RelationNameEntry
lea rdi,[rdx + 18]
mov bl,0FFh
mov eax,r8d
call SizePrint64
lea rdi,[rdx + 41]
mov ax,STR_TP_X
call PoolStringWrite
mov eax,r9d
mov bl,0
call DecimalPrint32
pop rdi

;---------- additions for screen 0 --------------------------------------------;

pop rax
push rax
call HelperSummaryCache

.nodata:
pop rax rsi rcx
ret

;--- Pre-blank string with CR, LF, write parameter name, set text pointers ----;
; INPUT:   RBP = Pointer to destination buffer                                 ;
;          EAX = Number write to string                                        ; 
; OUTPUT:  RBP = Updated pointer to destination buffer                         ;   
;------------------------------------------------------------------------------;

HelperNumberString:
push rcx rax
mov rdi,rbp
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop rax
mov rbp,rdi
lea rdi,[rbp - 82 + 01]
mov bl,0
call DecimalPrint32
pop rcx
ret

;---------- Copy text string with chars checks --------------------------------;
; INPUT:   RBX = Source base                                                   ;
;          RBP = Destination base                                              ;
;          AL  = Offset for source string                                      ;
;          AH  = Offset for destination string                                 ;
;          EAX.[31-16] = String size                                           ;    
; OUTPUT:  RDI = Last used updated pointer                                     ;
;          String at [RBP + AH] size EAX.[31-16] updated                       ;   
;------------------------------------------------------------------------------;

HelperCopyString:
movzx ecx,al
lea rsi,[rbx + rcx]
movzx ecx,ah
lea rdi,[rbp + rcx]
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
; INPUT:   RBX = Source base                                                   ;
;          RBP = Destination base                                              ;
;          AL  = Offset for source data contain 8-bit value                    ;
;          AH  = Offset for destination string                                 ;
; OUTPUT:  RDI = Last used updated pointer                                     ;
;          String at [RBP + AH] updated                                        ;   
;------------------------------------------------------------------------------;

HelperDecimalString:
push rbx
movzx ecx,ah
lea rdi,[rbp + rcx]
movzx ecx,al
movzx eax,byte [rbx + rcx]
mov bl,0
call DecimalPrint32 
pop rbx
ret

;---------- Write hex number text string, 32 bit value ------------------------;
; INPUT:   RBX = Source base                                                   ;
;          RBP = Destination base                                              ;
;          AL  = Offset for source data contain 8-bit value                    ;
;          AH  = Offset for destination string                                 ;
; OUTPUT:  RDI = Last used updated pointer                                     ;
;          String at [RBP + AH] updated                                        ;   
;------------------------------------------------------------------------------;

HelperHexString:
call HelperHexStringNh
mov al,'h'
stosb 
ret
HelperHexStringNh:  ; This entry point for write without "h"
movzx ecx,ah
lea rdi,[rbp + rcx]
movzx ecx,al
mov eax,[rbx + rcx]
jmp HexPrint32

;--- Copy fixed size string and write last 0, verify chars printability -------;
;                                                                              ;
; INPUT:   RSI = Source string                                                 ;
;          RDI = Destination string                                            ;
;          RCX = String size                                                   ;
;                                                                              ;
; OUTPUT:  RSI = Updated by copy                                               ;
;          RDI = Updated by copy                                               ;
;                                                                              ;
;------------------------------------------------------------------------------; 

HelperCopyAcpi:
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
loop HelperCopyAcpi 
mov al,0
stosb
ret
