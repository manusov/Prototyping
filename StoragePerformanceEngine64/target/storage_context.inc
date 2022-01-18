;------- Part of mass storage measurement scenario: context support -----------;
;                                                                              ;
; Support context for one of 4 measurement scenarios:                          ;
;  1) mbps    : file I/O bandwidth,                                            ;
;  2) iops    : file I/O performance,                                          ;
;  3) mapmbps : memory mapped file I/O bandwidth,                              ;
;  4) mapiops : memory mapped file I/O performance.                            ;
;                                                                              ;
; YET UNDER CONSTRUCTION, SUPPORTED ONLY MeasureGroupFileMBPS subroutine call. ;
;                                                                              ;
; INPUT:    RCX = Pointer to temporary buffer for messages, minimum 32 KB      ;
;           RDX = Pointer to error status variables, for error reporting:      ;
;                 3 QWORDS, 2 pointers to strings + 1 OS API error code        ;         
;                                                                              ;
; OUTPUT:   RAX = Status, 0 = error, error status variables valid              ;
;                         1 = no errors, error status variables not used       ;
;                                                                              ;
;------------------------------------------------------------------------------;

StoragePerformanceContext:

push rbx rsi rdi rbp r12 r13 r14 r15
mov rbp,rcx                      ; RBP = Non volatile copy of buffer pointer
mov r12,rdx                      ; R12 = Pointer for error reporting 

; Pre-clear status
xor eax,eax
mov [r12 + 00],rax  ; Clear pointer to first error description string
mov [r12 + 08],rax  ; Clear pointer to second error description string
mov [r12 + 16],rax  ; Clear WinAPI error code

;--- (1) --- Check CPU features and measure CPU clock -------------------------;

lea r15,[Step01_MeasureClock]    ; R15 = Pointer to phase name for error report
call MessageStepName

lea r14,[Status_CPUID]           ; R14 = Status (error description) for report
call CheckCpuId
jc .Error                        ; Go if CPUID not supported or locked
lea r14,[Status_CPUID_1]
cmp eax,1
jc .Error                        ; Go if CPUID function 1 not sup. or locked 
;--- Detect RDTSC support ---
lea r14,[Status_RDTSC]
mov eax,1
cpuid
test dl,10h
jz .Error                        ; Go if TSC not supported
;--- Measure TSC clock and verify results ---
lea r14,[Status_CLK]
call MeasureCpuClk
jc .Error                        ; Go if TSC clock measurement error
lea r14,[Status_Frequency]
test rax,rax
jz .Error                        ; Go if TSC error: frequency=0
mov [TscFrequencyHz],rax         ; Store TSC frequency, integer, Hz

finit
fild [Const1E6]
fild [TscFrequencyHz]
fdiv st0,st1
fst [TscFrequencyMHz]
fdivp st1,st0
fstp [TscPeriodPs]
mov rdi,rbp
mov al,' '
stosb
mov bx,0200h
mov rax,[TscFrequencyMHz]
call DoublePrint
mov eax,' MHz'
stosd
mov ax,' ('
stosw
mov rax,[TscPeriodPs]
call DoublePrint
mov eax,' ps)'
stosd
mov ax,0A0Dh
stosw
mov al,0
stosb
mov rcx,rbp
call InternalConsoleWrite

;--- (2) --- OS API detection -------------------------------------------------;

lea r15,[Step02_OsApiDetection]  ; R15 = Pointer to phase name for error report
call MessageStepName

lea r14,[Status_Handle_Module]
lea rcx,[NameKernel32]           ; RCX = Parm#1 = Pointer to module name string
call [GetModuleHandle]           ; RAX = Return module handle
test rax,rax
jz .Error                        ; Go if KERNEL32.DLL not found
xchg rbx,rax                     ; RBX = KERNEL32.DLL module handle 
;--- Get WinAPI handler for NUMA-aware memory allocation --- 
lea r14,[Status_NUMA_Allocation]
cmp [OptionNumanode],-1
je @f
mov rcx,rbx                      ; RCX = Parm#1 = KERNEL32.DLL module handle
lea rdx,[NameVirtualAllocExNuma] ; RDX = Parm#2 = Pointer to function name
call [GetProcAddress]            ; RAX = Return function address
test rax,rax
jz .Error                        ; Go if function not found
mov [PVirtualAllocExNuma],rax
@@:
;--- Get WinAPI handler for set thread affinity mask ---
lea r14,[Status_Affinity_Mask]
mov rax,[OptionAffinity]
test rax,rax
jz @f                            ; Go skip if affinity mask = 0 
cmp rax,-1
je @f                            ; Go skip if affinity mask = FF...FFh = - 1
mov rcx,rbx
lea rdx,[NameSetThreadAffinityMask]
call [GetProcAddress]
test rax,rax
jz .Error
mov [PSetThreadAffinityMask],rax
@@:
call MessageStepDone
mov rdi,rbp
lea rsi,[MsgApi1]
call StringWrite
mov rax,[PVirtualAllocExNuma]
call HexPrint64
mov al,'h'
stosb
mov ax,0A0Dh
stosw
lea rsi,[MsgApi2]
call StringWrite
mov rax,[PSetThreadAffinityMask]
call HexPrint64
mov al,'h'
stosb
mov ax,0A0Dh
stosw
mov al,0
stosb
mov rcx,rbp
call InternalConsoleWrite

;--- (3) --- Memory allocation, optional NUMA-aware ---------------------------;

lea r15,[Step03_MemoryAllocation]
call MessageStepName

lea r14,[Status_Handle_Process]
call [GetCurrentProcess]         ; Return RAX = Handle
test rax,rax
jz .Error                        ; Go if get current process handle error
xchg rcx,rax                     ; Parm#1 = RCX = Handle
mov eax,IPB_CONST + OPB_CONST
add rax,[OptionBlocksize]
mov edx,[OptionFilecount]
imul rdx,rdx,HANDLE_SIZE
add rax,rdx
test eax,0FFFh
jz @f
and eax,0FFFFF000h
add rax,00000000000001000h       ; This for size alignment
@@:
mov [MemorySize],rax
xchg r8,rax         ; Parm#3 = R8  = Memory block size, with base align rsvd.
xor edx,edx			    ; Parm#2 = RDX = Base address, 0 = auto
mov r9d,MEM_RESERVE + MEM_COMMIT  ; Parm#4 = R9  = Alloc. type
mov eax,[OptionNumanode]
push rax			        ; Parm#6 = NUMA node preferred = 0 , use RDX=0 for compact
pushq PAGE_READWRITE	; Parm#5 = Pages protection attributes = Read/Write
sub rsp,32			      ; Create parameters shadow
;--- Select NUMA-aware or NUMA-unaware memory allocation ---
cmp [OptionNumaaware],1
je .AllocNUMA 
call [VirtualAllocEx]
lea r14,[Status_Memory_No_NUMA]
jmp .EndAlloc
.AllocNUMA:
call [PVirtualAllocExNuma]
lea r14,[Status_Memory_NUMA] 
.EndAlloc:
;--- NUMA-aware or NUMA-unaware branch done ---
add rsp,32+16			          ; Remove parameters shadow + parameters 5,6
test rax,rax
jz .Error                   ; Go if memory allocation error
mov [TrueMemoryBase],rax    ; Store true base for release memory
test eax,0FFFh
jz @f
and rax,0FFFFFFFFFFFFF000h
add rax,00000000000001000h  ; This for size alignment
@@:
mov [MemoryBase],rax        ; Store aligned base for use memory
mov [PIPB],rax              ; Setup Input Parameters Block base address
add rax,IPB_CONST
mov [POPB],rax              ; Setup Output Parameters Block base address

call MessageStepDone
mov rdi,rbp
lea rsi,[MsgMemoryBase]
call StringWrite
mov rax,[MemoryBase]
call HexPrint64
mov al,'h'
stosb
mov ax,0A0Dh
stosw
lea rsi,[MsgMemorySize]
call StringWrite
mov rax,[MemorySize]
push rax
call HexPrint64
mov ax,'h '
stosw
mov al,'('
stosb
pop rax
mov bl,0FFh
call SizePrint64
mov al,')' 
stosb
mov ax,0A0Dh
stosw
mov al,0
stosb
mov rcx,rbp
call InternalConsoleWrite

;--- (4) --- Memory blank -----------------------------------------------------;
; No error branches for this operation, error reporting is reserved

cld
mov rdi,[MemoryBase]         ; RDI = Block base
mov rcx,[MemorySize]         ; RCX = Block size, bytes count
shr rcx,3                    ; RCX = QWords count = Bytes count / 8
mov rax,'DATA    '           ; This data for fill buffer
rep stosq

;--- (5) --- Set affinity mask ------------------------------------------------;

lea r15,[Step05_SetAffinity]
call MessageStepName

;--- Check affinity mask required or not ---
mov rbx,[OptionAffinity]
test rbx,rbx
jz @f                  ; Skip affinitization if mask = 0
cmp rbx,-1
je @f                  ; Skip affinitization if mask = -1
;--- Get thread handle ---
lea r14,[Status_Handle_Thread]
call [GetCurrentThread]
test rax,rax
jz .Error               ; Go if thread error 
;--- Set affinity mask ---
lea r14,[Status_Set_Affinity]
xchg rcx,rax           ; RCX = Thread handle
mov rdx,rbx            ; RDX = Thread affinity mask
call [SetThreadAffinityMask]
test rax,rax           ; RAX = Old affinity mask
jz .Error              ; Go if affinity mask error 
mov [OrigAffinity],rax
@@:

call MessageStepDone
mov rdi,rbp
lea rsi,[MsgOrigAffinity]
call StringWrite
mov rax,[OrigAffinity]
call HexPrint64
mov al,'h'
stosb
mov ax,0A0Dh
stosw
lea rsi,[MsgSetAffinity]
call StringWrite
mov rax,[OptionAffinity]
call HexPrint64
mov al,'h'
stosb
mov ax,0A0Dh
stosw
mov al,0
stosb
mov rcx,rbp
call InternalConsoleWrite

;--- (6.1) --- Verify parameters for execute IO functions ---------------------;

lea r15,[Step06_InitializeFileIO]
call MessageStepName

mov rsi,[PIPB]           ; RSI = Pointer to Input Parameters Block
mov rdi,[POPB]           ; RDI = Pointer to Output Parameters Block
;--- One file size ---
lea r14,[Status_File_Size]
mov rax,[OptionFilesize]
test rax,rax
jz .Error                ; Go if value  = 0
js .Error                ; Go if value bit[63] = 1 , too big or negative 
mov IPB_REQUEST_SIZE,rax
;--- Block size, per API call ---
lea r14,[Status_Block_Size]
mov rcx,[OptionBlocksize]
test rcx,rcx
jz .Error                ; Go if value  = 0
js .Error                ; Go if value bit[63] = 1 , too big or negative 
mov IPB_BLOCK_SIZE,rcx
;--- Check File Size and Block Size ratio integer ---
lea r14,[Status_Block_Ratio]
xor edx,edx
div rcx                  ; Divide ( RAX = File size ) / ( RCX = Block size ) 
test rdx,rdx             ; RDX = Modulo
jnz .Error               ; Go if File / Block size ratio not integer
;--- Measurement repeat count ---
lea r14,[Status_Repeat_Count]
mov eax,[OptionFilerepeats]
test eax,eax
jz .Error                ; Go if value  = 0
js .Error                ; Go if value bit[31] = 1 , too big or negative 
mov IPB_ITERATIONS,rax
;--- Cache mode flags ---
lea r14,[Status_Cache_Mode]
xor eax,eax
cmp [OptionReadsync],0
je @f
or eax,0001b
@@:
cmp [OptionWritesync],0
je @f
or eax,0010b
@@:
cmp eax,3                ; This situation (EAX>3) yet impossible, RESERVED
ja .Error                ; Go if wrong mode bitmap 
mov IPB_SRC_ATTRIBUTES,rax
mov IPB_DST_ATTRIBUTES,rax

;--- Extract file path prefix and postfix from path string ---
lea rcx,[BufferSrc]         ; RCX = Pointer to path string ( source file )
lea rdx,[ReadPrefix]        ; RDX = Pointer for write extracted prefix
lea r8,[ReadPostfix]        ; R8  = Pointer for write extracted postfix 
mov r9d,PATH_BUFFER_SIZE    ; R9  = Maximum size for all 3 buffers 
call PathExtractHelper
lea rcx,[BufferDst]         ; RCX = Pointer to path string ( destination file )
lea rdx,[WritePrefix]       ; RDX = Pointer for write extracted prefix
lea r8,[WritePostfix]       ; R8  = Pointer for write extracted postfix
mov r9d,PATH_BUFFER_SIZE    ; R9  = Maximum size for all 3 buffers 
call PathExtractHelper

;--- (6.2) --- Verify engineering sample restrictions -------------------------;

xor eax,eax

lea r14,[Restriction_IOPS]
cmp [OptionMeasure],al
jne .Error

lea r14,[Restriction_Single]           
cmp [OptionFilemode],al
jne .Error

lea r14,[Restriction_Data]
cmp [OptionFiledata],al
jne .Error

lea r14,[Restriction_AsyncRead]
cmp [OptionReadsync],1
jne .Error

lea r14,[Restriction_AsyncWrite]
cmp [OptionWritesync],1
jne .Error

lea r14,[Restriction_AsyncCopy]
cmp [OptionCopysync],1
jne .Error

lea r14,[Restriction_ReadDelay]
cmp [OptionReaddelay],eax
jne .Error

lea r14,[Restriction_WriteDelay]
cmp [OptionWritedelay],eax
jne .Error

lea r14,[Restriction_CopyDelay]
cmp [OptionCopydelay],eax
jne .Error

lea r14,[Restriction_FileHints]
cmp [OptionFilehint],al
jne .Error

lea r14,[Restriction_MixedRead]
cmp [OptionReadmixed],eax
jne .Error

lea r14,[Restriction_MixedWrite]
cmp [OptionWritemixed],eax
jne .Error

call MessageStepDone
lea rcx,[CrLf]
call InternalConsoleWrite

;--- (7) --- Execute selected IO functions ------------------------------------;

mov rsi,[PIPB]           ; RSI = Pointer to Input Parameters Block (IPB)
mov rdi,[POPB]           ; RDI = Pointer to Output Parameters Block (OPB)
call MeasureGroupFileMBPS

;--- (8) --- Analyzing file IO status -----------------------------------------;

;--- Check OS API status ---
lea r15,[Step08_CheckStatus]      ; R14=0 means get error code from OS
xor r14d,r14d                     ; and use OPB_LAST_OPERATION
mov r13,OPB_LAST_OPERATION 
cmp OPB_LAST_ERROR,0
jne .Error                        ; Go if file IO error
cmp r13,STEP_NONE
jne .ErrorUnknown                 ; Go if error-cause operation code set
;--- Verify data size ---
mov rax,[OptionFilesize]
mov ecx,[OptionFilecount]
mul rcx                           ; RAX = File size * File count
mov ecx,[OptionFilerepeats]
mul rcx                           ; RAX = File size * File count * Repeats
lea r14,[Status_Read_Mismatch]
cmp rax,[TotalReadSizeSum]
jne .Error                        ; Go if read size mismatch  
lea r14,[Status_Write_Mismatch]
cmp rax,[TotalWriteSizeSum]
jne .Error                        ; Go if write size mismatch  
lea r14,[Status_Copy_Mismatch]
cmp rax,[TotalCopySizeSum]
jne .Error                        ; Go if copy size mismatch  

;--- (9) --- Calculate file IO performance results ----------------------------;

lea r15,[Step09_CalculateResults]
lea r14,[Status_FPU]
mov eax,1            ; CPUID function 1, get base features
cpuid
test dl,00000001b    ; Detect x87 FPU presence, CPUID function 1, bit EDX.0
jz .Error            ; Go if x87 FPU absent
;--- Use x87 ---
finit                ; Start calculate base variables by x87 FPU
fild [TotalCopyTimeSum]
fild [TotalWriteTimeSum] 
fild [TotalReadTimeSum] 
fild [Const1E9]
fdiv st1,st0
fdiv st2,st0
fdivp st3,st0        ; ST0 = Read seconds, ST1 = Write seconds, ST2 = Copy sec. 
fild [Const1E6]
fild [OptionFilesize]
fild [OptionFilecount]
fild [OptionFilerepeats]
fmulp st1,st0
fmulp st1,st0        ; ST0 = Total size transferred, Bytes 
fdivrp st1,st0       ; ST0 = Megabytes, ST3,ST4,ST5 = R,W,C Seconds
fdivr st1,st0
fdivr st2,st0
fdivrp st3,st0       ; ST0 = Read MBPS, ST1 = Write MBPS, ST2 = Copy MBPS
fstp [ReadMBPS]
fstp [WriteMBPS]
fstp [CopyMBPS]

;--- (10) --- Restore affinity ------------------------------------------------;

lea rcx,[CrLf]
call InternalConsoleWrite
lea r15,[Step10_RestoreAffinity]
call MessageStepName

mov rax,[OptionAffinity]
test rax,rax
je @f                        ; Skip operation if affinity not changed
cmp rax,-1
je @f                        ; Skip operation if affinity not changed
;--- Get thread handle ---
lea r14,[Status_Handle_Thread]
call [GetCurrentThread]
test rax,rax
jz .Error                    ; Go if thread error 
;--- Set affinity mask ---
lea r14,[Status_Set_Affinity]
xchg rcx,rax                 ; RCX = Thread handle
mov rdx,[OrigAffinity]       ; RDX = Thread affinity mask
test rdx,rdx
jz @f                        ; Skip restore if wrong (not initialized) value
call [SetThreadAffinityMask]
test rax,rax                 ; RAX = Old affinity mask
jz .Error                    ; Go if affinity mask error 
@@:

call MessageStepDone

;--- (11) --- Release memory --------------------------------------------------;

lea r15,[Step11_ReleaseMemory]
call MessageStepName

;--- Get process handle ---
lea r14,[Status_Handle_Process]
call [GetCurrentProcess]     ; Return RAX = Handle
test rax,rax
jz .Error                    ; Go if handle error
;--- Release memory ---
lea r14,[Status_Memory_Release]
xchg rcx,rax                 ; Parm#1 = RCX = Process Handle
mov rdx,[TrueMemoryBase]     ; Parm#2 = Address
xor r8d,r8d                  ; Parm#3 = R8 = Release size, 0 = Entire block
mov r9d,MEM_RELEASE	         ; Parm#4 = Release type
test rdx,rdx
jz @f                        ; Skip restore if wrong (not initialized) value
call [VirtualFreeEx]
test rax,rax
jz .Error                    ; Go if function error
@@:

call MessageStepDone

;--- (12) --- Prepare results for visual --------------------------------------;

lea rsi,[Parm_ReadSpeed]
mov rdi,rbp
;--- Benchmarks results: Read speed ---
mov rax,[ReadMBPS]
call HelperMBPS
;--- Benchmarks results: Write speed ---
mov rax,[WriteMBPS]
call HelperMBPS
;--- Benchmarks results: Copy speed ---
mov rax,[CopyMBPS]
call HelperMBPS
;--- Skip string and terminate sequence ---
mov ax,0A0Dh
stosw
mov al,0
stosb

;--- (13) --- Visual results --------------------------------------------------;

mov rcx,rbp
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
jmp .NoError


;--- (14) --- Exit measurement scenario ---------------------------------------;

; Information about error:
; R15 = Reserved for pointer to step name string in GUI version, in this
; console version R15 not used because step name visualized before step and
; visible before error message.
; R14 = Pointer to status string, error description.    

.ErrorUnknown:
lea r14,[Status00_Unknown]

.Error:
test r14,r14
jnz @f                     ; Skip OS error code if R14 = Pointer valid
mov rdi,[POPB]
mov rax,OPB_LAST_ERROR     ; This code (OS error code) valid if R14 = 0
mov [r12 + 16],rax         ; Return OS error code
lea r14,[Status00_WinAPI]  ; R14 = Comment string for WinAPI returned error
@@:
mov [r12 + 00],r14         ; Return pointer to status string, error description

lea rcx,[CrLf]
call InternalConsoleWrite
xor eax,eax    ; RAX = 0, error, status variables at [input RDX] valid
jmp .Exit

.NoError:
mov eax,1      ; RAX = 1, no errors, status variables at [input RDX] not valid
.Exit:
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret



