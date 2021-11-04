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

; TODO. 
; Optimize for use single call per subject, instead 2 calls: Info_ and Text_

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
; TODO. All operations with BINDLIST locate here, subject-specific at
;       info_***.inc

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
