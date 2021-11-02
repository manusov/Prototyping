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
SystemInfo:
push ebx esi edi ebp
push 0                  ; Variable dword at [esp], if original ESP, no pushes
cld
;---------- Processor information ---------------------------------------------;
call InfoProcessor
jc .error
;---------- Initializing dynamical import WinAPI list -------------------------;
push esi
mov ebx,[APP_DATA.lockedImportList]  ; EBX = to functions names
lea edi,[DYNA_IMPORT]
lea esi,[NAME_KERNEL32]     ; ESI = Pointer to libraries names array
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
lea edi,[OS_DATA.isWow64]
mov ecx,[DYNA_IMPORT._IsWow64Process]
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
call InfoOperatingSystem
;---------- Get native operating system information ---------------------------;
call InfoNativeOs
;---------- Get platform topology by WinAPI -----------------------------------;
; Start operations with unformatted part of application registry,
; EBP = Pointer for unformatted part addressing 
call InfoTopology
;---------- Get platform extended topology by WinAPI --------------------------;
; Separate get each type of information for build ordered list.
call InfoExtendedTopology
;---------- Get NUMA nodes list -----------------------------------------------;
call InfoNumaDomains
;---------- Get processor groups list -----------------------------------------;
call InfoProcessorGroups
;---------- Get affinized CPUID dump ------------------------------------------;
call InfoAffinizedCpuid
;---------- Build text block for operating system information screen ----------;
; Start text blocks build operations.
call TextOperatingSystem
;---------- Build text block for native operating system information screen ---;
call TextNativeOs
;---------- Build text block for topology screen, first block = list ----------;
call TextTopology
;--- Build text block for extended topology screen, first block = list --------;
call TextExtendedTopology
;---------- Build text block for NUMA domains list screen ---------------------;
call TextNumaDomains
;---------- Build text block for processor groups list screen -----------------;
call TextProcessorGroups
;---------- Build text block for ACPI screen, first block = tables list -------;
call InfoAcpi
;--- Build text block for ACPI screen, second block = tables detection --------;
call TextAcpi
;---------- Build text block for affinized CPUID dump -------------------------;
call TextAffinizedCpuid



;---------- Text blocks for viewers -------------------------------------------;

; TODO.
; OPTIMIZE THIS, MAKE CYCLE WITH TABLE.

lea edi,[BIND_LIST]  ; TODO. Optimize add. 
add edi,BINDLIST.viewOs
mov eax,[DYNA_PTR.textOs.objectStart]
stosd       ; Operating system information
xor eax,eax
stosd
mov eax,[DYNA_PTR.textNativeOs.objectStart]
stosd       ; Native operating system information
xor eax,eax
stosd
mov eax,[DYNA_PTR.textTopology1.objectStart]
stosd       ; Topology information by operating system, text block 1, list
xor eax,eax
stosd
mov eax,[DYNA_PTR.textTopology2.objectStart]
stosd       ; Topology information by operating system, text block 2, summary
xor eax,eax
stosd
mov eax,[DYNA_PTR.textTopologyEx1.objectStart]
stosd       ; Extended topology information by OS, text block 1, list
xor eax,eax
stosd
mov eax,[DYNA_PTR.textTopologyEx2.objectStart]
stosd       ; Extended topology information by OS, text block 2, summary
xor eax,eax
stosd
mov eax,[DYNA_PTR.textNuma.objectStart]
stosd       ; NUMA domains list by OS
xor eax,eax
stosd
mov eax,[DYNA_PTR.textGroup.objectStart]
stosd       ; Processor groups list by OS
xor eax,eax
stosd
mov eax,[DYNA_PTR.textAcpi1.objectStart]
stosd       ; ACPI tables, text block 1 = tables list
xor eax,eax
stosd
mov eax,[DYNA_PTR.textAcpi2.objectStart]
stosd       ; ACPI tables, text block 2 = tables detection by data base
xor eax,eax
stosd
mov eax,[DYNA_PTR.textAffCpuid.objectStart]
stosd       ; Affinized CPUID dump
xor eax,eax
stosd
;---------- Data for memory and cache benchmark setup -------------------------;

; TODO. Move this to separate include file with subroutine. (?)
; TODO. Remove "magic numbers" use names for bits positions.

lea edi,[BIND_LIST]
;---------- Benchmark methods based on CPU instructions -----------------------;
mov eax,dword [CPU_DATA.extractedFeaturesBitmap]
mov edx,dword [CPU_DATA.extractedContextBitmap]
mov ebx,00000555h       ; Latency by LCM, REP MOVS, REP STOS, NOT, MOV (3)
mov ebp,00010000h
test al,00000001b
jz @f
or ebx,00015000h        ; MMX (3) 
@@:
test al,00000010b
jz @f
or ebx,00540000h        ; SSE (3) 
@@:
test al,10000000b
jz @f
test dl,00000010b
jz @f
or ebx,15000000h        ; AVX (3) 
@@:
test ah,00000010b
jz @f
mov cl,dl
and cl,00011100b
cmp cl,00011100b
jne @f
or ebx,40000000h        ; AVX512 (3), FMA 512, VGATHERQPD 512, VSCATTERQPD 512
or ebp,00001445h 
@@:
test ah,01000000b
jz @f
bts ebp,4               ; FMA 256
@@:
test ah,00000001b
jz @f
bts ebp,8               ; VGATHERQPD 256
@@:
test ah,10000000b
jz @f
bts ebp,14              ; CLZERO (AMD)
@@:
test ah,00000100b
jz @f
bts ebp,18              ; Latency by RDRAND
@@:
test al,00000100b
jz @f
bts ebp,28             ; SSE2 (for vector brief)
@@:
bt eax,16
jnc @f
bts ebp,30             ; x87 (for vector brief, redundant because run criteria)
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
or ebx,ecx
mov [edi + BINDLIST.setMemMethod + 0],ebx
mov [edi + BINDLIST.setMemMethod + 4],ebp
;--- Benchmark objects: cache memory, DRAM or user-defined block size ---------;
; Plus custom block size
lea ecx,[SYS_PARMS.summaryCache]
mov ebx,010100000000b      ; Custom block, DRAM
xor eax,eax
cdq
mov ebp,dword [ecx + SUMMARYCACHE.sizeL1D + 0]
or ebp,dword [ecx + SUMMARYCACHE.sizeL1D + 4]
jz @f 
or bl,00000011b            ; L1 data cache
mov eax,dword [ecx + SUMMARYCACHE.sizeL1D + 0]
mov edx,dword [ecx + SUMMARYCACHE.sizeL1D + 4]
@@:
mov ebp,dword [ecx + SUMMARYCACHE.sizeL2U + 0]
or ebp,dword [ecx + SUMMARYCACHE.sizeL2U + 4]
jz @f
or bl,00000100b            ; L2 unified cache
mov ebp,eax
or ebp,edx
jnz @f
mov eax,dword [ecx + SUMMARYCACHE.sizeL2U + 0]
mov edx,dword [ecx + SUMMARYCACHE.sizeL2U + 4]
or bl,00001000b
@@:
mov ebp,dword [ecx + SUMMARYCACHE.sizeL3U + 0]
or ebp,dword [ecx + SUMMARYCACHE.sizeL3U + 4]
jz @f
or bl,00010000b            ; L3 unified cache
mov ebp,eax
or ebp,edx
jnz @f
mov eax,dword [ecx + SUMMARYCACHE.sizeL3U + 0]
mov edx,dword [ecx + SUMMARYCACHE.sizeL3U + 4]
or bl,00100000b
@@:
mov ebp,dword [ecx + SUMMARYCACHE.sizeL4U + 0]
or ebp,dword [ecx + SUMMARYCACHE.sizeL4U + 4]
jz @f
or bl,00010000b            ; L4 unified cache
mov ebp,eax
or ebp,edx
jnz @f
mov eax,dword [ecx + SUMMARYCACHE.sizeL4U + 0]
mov edx,dword [ecx + SUMMARYCACHE.sizeL4U + 4]
or bl,10000000b
@@:
mov ebp,eax
or ebp,edx
jnz @f
or bh,00001000b
mov eax,32768
@@:
mov dword [edi + BINDLIST.setMemObject],ebx
mov dword [edi + BINDLIST.setBlkCustom + 0],eax
mov dword [edi + BINDLIST.setBlkCustom + 4],edx
;---------- Benchmark options: SMP/SMT ----------------------------------------;
lea ecx,[SYS_PARMS.summaryTopology]
xor ebx,ebx
mov eax,[ecx + SUMMARYTOPOLOGY.threads]
cmp eax,2
jb @f
inc ebx           ; Parallel threads (SMP)
@@:
shr eax,1
jz @f
cmp eax,[ecx + SUMMARYTOPOLOGY.cores]
jne @f
or bl,00000100b   ; Hyper threading (SMT)
@@:
lea ecx,[OS_DATA]
cmp [ecx + OSDATA.activeProcessorGroupCount],2
jb @f
or bl,00010000b   ; Processor groups (PG)
@@:
mov word [edi + BINDLIST.setMemSmp],bx
;---------- Benchmark option: NUMA --------------------------------------------;
mov bx,00000010b
cmp [ecx + OSDATA.numaNodeCount],2
jb @f
mov bl,01110101b   ; Unaware, Single domain, optimal, non-optimal
@@:
mov word [edi + BINDLIST.setMemNuma],bx
;---------- Benchmark option: Large Pages -------------------------------------;
xor ebx,ebx
cmp [ecx + OSDATA.largePageEnable],ebx
je @f
inc ebx
@@:
mov word [edi + BINDLIST.setMemLpages],bx
;---------- Benchmark options, unconditionally set ----------------------------;
mov word [edi + BINDLIST.setMemAccess],00000010b  ; Access step
mov word [edi + BINDLIST.setMemPref],00000010b    ; 01010101b  ; Prefetch modes
mov word [edi + BINDLIST.setMemMeas],11010101b    ; Measure precision modes
mov word [edi + BINDLIST.setMemPix],00010111b     ; Pixels approximation modes
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
cmp bl,0FFh
je .sysinfo
lea ecx,[OS_DATA.memoryStatusEx]
jmp .done
.sysinfo:
test bh,bh
jnz .nativeInfo
lea ecx,[OS_DATA.systemInfo]
jmp .done
.nativeInfo:
lea ecx,[OS_DATA.nativeSystemInfo]
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
test bh,bh
jnz .nativeInfo
mov eax,[OS_DATA.systemInfo + eax]
jmp .doneInfo
.nativeInfo:
mov eax,[OS_DATA.nativeSystemInfo + eax]
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
test bh,bh
jnz .nativeInfo
mov eax,[OS_DATA.systemInfo + eax]
jmp .doneInfo
.nativeInfo:
mov eax,[OS_DATA.nativeSystemInfo + eax]
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
test bh,bh
jnz .nativeInfo
movzx eax,word [OS_DATA.systemInfo + eax]
jmp .doneInfo
.nativeInfo:
movzx eax,word [OS_DATA.nativeSystemInfo + eax]
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
mov eax,[OS_DATA.systemInfo.dwActiveProcessorMask]
jmp .doneInfo
.nativeInfo:
mov eax,[OS_DATA.nativeSystemInfo.dwActiveProcessorMask]
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
;          EDI = Destination text buffer                                       ;
;          ECX = Entries count at topology information buffer                  ;  
;          AL  = Cache level                                                   ;  
;          AH  = Cache type                                                    ;    
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperCacheSummary:
push ecx esi 0 0 ebp
mov ebp,esp
push eax
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
;---------- additions for screen 0 --------------------------------------------;
pop eax
push eax
call HelperSummaryCache
.nodata:
pop eax ebp eax eax esi ecx
ret
HelperSummaryCache:
push edi
lea ecx,[BIND_LIST]
lea ebx,[SYS_PARMS.summaryCache]
add ecx,BINDLIST.bindCache
lea esi,[ebx + SUMMARYCACHE.sizeL1C]
lea edi,[ecx + BINDCACHE.l1c]  
mov dl,00000001b
cmp ax,0101h
je .detected 
lea esi,[ebx + SUMMARYCACHE.sizeL1D]
lea edi,[ecx + BINDCACHE.l1d]  
mov dl,00000010b
cmp ax,0201h
je .detected 
lea esi,[ebx + SUMMARYCACHE.sizeL2U]
lea edi,[ecx + BINDCACHE.l2u]  
mov dl,00000100b
cmp ax,0002h
je .detected 
lea esi,[ebx + SUMMARYCACHE.sizeL3U]
lea edi,[ecx + BINDCACHE.l3u]  
mov dl,00001000b
cmp ax,0003h
je .detected 
lea esi,[ebx + SUMMARYCACHE.sizeL4U]
lea edi,[ecx + BINDCACHE.l4u]  
mov dl,00010000b
cmp ax,0004h
je .detected 
jmp .nodata
.detected:
or [ecx + BINDCACHE.cacheBitmap],dl
mov eax,[ebp + 08]
mov [esi + 8],eax
mov eax,[ebp + 04]
xor edx,edx
mov [esi + 0],eax
mov [esi + 4],edx
mov bl,0FFh
mov eax,[ebp + 04]
call SizePrint64
mov al,' '
stosb
mov ax,STR_TP_X
call PoolStringWrite
mov eax,[ebp + 08]
mov bl,0
call DecimalPrint32
mov al,0
stosb
.nodata:
pop edi
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
;          EDI = Destination text buffer                                       ;
;          ECX = Topology information buffer address limit                     ;  
;          AL  = Cache level                                                   ;  
;          AH  = Cache type                                                    ;    
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperCacheSummaryEx:
push ecx esi 0 0 ebp
mov ebp,esp
push eax
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
;---------- additions for screen 0 --------------------------------------------;
pop eax
push eax
call HelperSummaryCache
.nodata:
pop eax ebp eax eax esi ecx
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
