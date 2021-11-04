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
SystemInfo:
push rbx rsi rdi rbp r12 r13 r14 r15
push 0                       ; qword at [r13] = temporary variable
mov r13,rsp                  ; R13 used for RSP restore before return 
and rsp,0FFFFFFFFFFFFFFF0h   ; Stack alignment by Microsoft x64 calling convention
sub rsp,32                   ; Parameter shadow by Microsoft x64 calling convention 
cld
;---------- Processor information ---------------------------------------------;
call InfoProcessor
jc .error
;---------- Initializing dynamical import WinAPI list -------------------------; 
lea rsi,[NAME_KERNEL32]       ; RSI = Pointer to libraries names array
mov rbx,[APP_DATA.lockedImportList] ; RBX = Pointer to functions names
lea rdi,[DYNA_IMPORT]               ; RDI = Pointer to fnc. addresses               
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

; TODO. 
; Optimize for use single call per subject, instead 2 calls: Info_ and Text_

call InfoOperatingSystem
;---------- Get native operating system information ---------------------------;
call InfoNativeOs
;---------- Get platform topology by WinAPI -----------------------------------;
; Start operations with unformatted part of application registry,
; RBP = Pointer for unformatted part addressing 
call InfoTopology
;---------- Get platform extended topology by WinAPI --------------------------;
; Separate get each type of information for build ordered list.
call InfoExtendedTopology
;---------- Get NUMA nodes list -----------------------------------------------;
call InfoNumaDomains
;---------- Get processor groups list -----------------------------------------;
call InfoProcessorGroups
;---------- Get ACPI tables list ----------------------------------------------;
call InfoAcpi
;---------- Get affinized CPUID dump ------------------------------------------;
call InfoAffinizedCpuid
;---------- Build text block for operating system information screen ----------; 
; Start text blocks build operations.
call TextOperatingSystem
;---------- Build text block for native operating system information screen ---;
call TextNativeOs
;---------- Build text block for topology screen, first block = list ----------;
; TODO. REDESIGN CYCLE WITH MORE EFFECTIVE CPU REGISTERS USAGE,
; WITHOUT STACK VARIABLES.
call TextTopology
;--- Build text block for extended topology screen, first block = list --------;
call TextExtendedTopology
;---------- Build text block for NUMA domains list screen ---------------------;
call TextNumaDomains
;---------- Build text block for processor groups list screen -----------------;
call TextProcessorGroups
;---------- Build text block for ACPI screen, first block = tables list -------;
call TextAcpi
;---------- Build text block for affinized CPUID dump -------------------------;
call TextAffinizedCpuid

;---------- Text blocks for viewers -------------------------------------------;

; TODO.
; MAKE CYCLE FOR THIS OPERATION.
; TODO.
; USE BINARY-TEXT-BINARY-TEXT-... sequence instead ALL BINARY - ALL TEXT,
; because can delete temporary data.

lea rdi,[BIND_LIST]
add rdi,BINDLIST.viewOs
mov rax,[DYNA_PTR.textOs.objectStart]
stosq       ; Operating system information
mov rax,[DYNA_PTR.textNativeOs.objectStart]
stosq       ; Native operating system information
mov rax,[DYNA_PTR.textTopology1.objectStart]
stosq       ; Topology information by operating system, text block 1, list
mov rax,[DYNA_PTR.textTopology2.objectStart]
stosq       ; Topology information by operating system, text block 2, summary
mov rax,[DYNA_PTR.textTopologyEx1.objectStart]
stosq       ; Extended topology information by OS, text block 1, list
mov rax,[DYNA_PTR.textTopologyEx2.objectStart]
stosq       ; Extended topology information by OS, text block 2, summary
mov rax,[DYNA_PTR.textNuma.objectStart]
stosq       ; NUMA domains list by OS
mov rax,[DYNA_PTR.textGroup.objectStart]
stosq       ; Processor groups list by OS
mov rax,[DYNA_PTR.textAcpi1.objectStart]
stosq       ; ACPI tables, text block 1 = tables list
mov rax,[DYNA_PTR.textAcpi2.objectStart]
stosq       ; ACPI tables, text block 2 = tables detection by data base
mov rax,[DYNA_PTR.textAffCpuid.objectStart]
stosq       ; Affinized CPUID dump
;---------- Data for memory and cache benchmark setup -------------------------;
; TODO. Move this to separate include file with subroutine.
; TODO. Remove "magic numbers" use names for bits positions.
; TODO. All operations with BINDLIST locate here, subject-specific at
;       info_***.inc

lea rdi,[BIND_LIST]
;---------- Benchmark methods based on CPU instructions -----------------------; 
mov eax,dword [CPU_DATA.extractedFeaturesBitmap]
mov edx,dword [CPU_DATA.extractedContextBitmap]
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
test al,00000100b
jz @f
bts rbx,60             ; SSE2 (for vector brief)
@@:
bt eax,16
jnc @f
bts rbx,62             ; x87 (for vector brief, redundant because run criteria)
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
lea rsi,[SYS_PARMS.summaryCache]
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
lea rsi,[SYS_PARMS.summaryTopology]
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
lea rsi,[OS_DATA]
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
