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
cmp bl,0FFh
je .sysinfo
lea rcx,[OS_DATA.memoryStatusEx]
jmp .done
.sysinfo:
test bh,bh
jnz .nativeInfo
lea rcx,[OS_DATA.systemInfo]
jmp .done
.nativeInfo:
lea rcx,[OS_DATA.nativeSystemInfo]
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
test bh,bh
jnz .nativeInfo
mov eax,[OS_DATA.systemInfo + rax]
jmp .doneInfo
.nativeInfo:
mov eax,[OS_DATA.nativeSystemInfo + rax]
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
test bh,bh
jnz .nativeInfo
mov eax,[OS_DATA.systemInfo + rax]
jmp .doneInfo
.nativeInfo:
mov eax,[OS_DATA.nativeSystemInfo + rax]
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
test bh,bh
jnz .nativeInfo
movzx eax,word [OS_DATA.systemInfo + rax]
jmp .doneInfo
.nativeInfo:
movzx eax,word [OS_DATA.nativeSystemInfo + rax]
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
mov rax,[OS_DATA.systemInfo.dwActiveProcessorMask]
jmp .doneInfo
.nativeInfo:
mov rax,[OS_DATA.nativeSystemInfo.dwActiveProcessorMask]
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
lea rcx,[BIND_LIST]
lea rbx,[SYS_PARMS.summaryCache]
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
