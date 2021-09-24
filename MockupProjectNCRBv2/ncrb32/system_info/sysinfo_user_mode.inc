; UNDER CONSTRUCTION (ia32).
; Can destroy registers, volatile by Microsoft ia32 calling convention.

SysinfoUserMode:
push ebx esi edi ebp
cld
mov esi,[Registry]
;---------- Initializing dynamical import WinAPI list -------------------------;
push esi
mov ebx,[esi + REGISTRY32.appData.lockedImportList]
lea edi,[esi + REGISTRY32.dynaImport]
lea esi,[NameKernel32]
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
;---------- Build text block for operating system information screen ----------;
lea ebp,[esi + REGISTRY32.unformatted]
push ebp
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
mov eax,[esi + REGISTRY32.osData.systemInfo.dwActiveProcessorMask]
mov edi,ecx
call HexPrint32 
mov ax,STR_OS_PROC_TYPE                          ; Processor type
call HelperOsInfoString
mov al,SYSTEM_INFO.dwProcessorType
call HelperOsInfoNumber32
mov ax,STR_OS_ALLOC_GRAN                         ; Allocation granularity
call HelperOsInfoString
mov al,SYSTEM_INFO.dwAllocationGranularity
call HelperOsInfoNumber32
mov ax,STR_OS_PROC_LEVEL                         ; Processor level
call HelperOsInfoString
mov al,SYSTEM_INFO.wProcessorLevel
call HelperOsInfoNumber16
mov ax,STR_OS_PROC_REVISION                      ; Processor revision
call HelperOsInfoString
mov al,SYSTEM_INFO.wProcessorRevision
call HelperOsInfoNumber16
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
mov [esi + REGISTRY32.textOs.objectStop],ebp
pop ebp
;---------- Check CPUID instruction support, get 12-chars vendor string -------;
lea edi,[esi + REGISTRY32.cpuData.vendorString]
mov ebp,edi
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

;---------- Build bind buffer -------------------------------------------------;
push esi
mov edi,[esi + REGISTRY32.allocatorBindBuffer.objectStart]
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


;---------- Exit points -------------------------------------------------------;
; TODO. Error Handling.
.error:
; TODO. Error Handling.
pop ebp edi esi ebx
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
mov esi,[Registry]
mov esi,[esi + REGISTRY32.appData.lockedStrings]
call IndexString
lea edi,[ebp - 82 + 01]
call StringWrite
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
; OUTPUT:  None                                                                ;
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
lea ecx,[ecx + REGISTRY32.osData.systemInfo]
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
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperOsInfoSizeAuto:
mov bl,0FFh
jmp SizeHelperEntry
;---------- Write memory block size to pre-blanked, hex only ------------------;
;           This variant for memory status structure.                          ;
;           This variant for 64-bit value.                                     ;
; INPUT:   ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the MEMORYSTATUSEX structure                        ; 
; OUTPUT:  None                                                                ;
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
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperOsInfoSizeAuto32:
mov bl,0FFh
movzx eax,al
push ecx
mov ecx,[Registry]
mov eax,[ecx + REGISTRY32.osData.systemInfo + eax]
push eax
xor edx,edx
call SizePrint64
pop eax edi
jmp HexPrint32
;---------- Write 32-bit number to pre-blanked string, decimal and hex --------;
; INPUT:   EDI = Pointer to destination buffer, decimal value position         ;
;          ECX = Pointer to destination buffer, hex value position             ; 
;          AL  = Offset in the SYSTEM_INFO structure                           ; 
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperOsInfoNumber32:
movzx eax,al
push ecx
mov ecx,[Registry]
mov eax,[ecx + REGISTRY32.osData.systemInfo + eax]
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
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperOsInfoNumber16:
movzx eax,al
push ecx
mov ecx,[Registry]
movzx eax,word [ecx + REGISTRY32.osData.systemInfo + eax]
push eax
mov bl,0
call DecimalPrint32
pop eax edi
jmp HexPrint16

