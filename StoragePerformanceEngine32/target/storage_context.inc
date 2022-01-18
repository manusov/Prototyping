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
; INPUT:    ECX = Pointer to temporary buffer for messages, minimum 32 KB      ;
;           EDX = Pointer to error status variables, for error reporting:      ;
;                 3 DWORDS, 2 pointers to strings + 1 OS API error code        ;         
;                                                                              ;
; OUTPUT:   EAX = Status, 0 = error, error status variables valid              ;
;                         1 = no errors, error status variables not used       ;
;                                                                              ;
;------------------------------------------------------------------------------;

ERROR_POINTER  EQU  dword [esp + 00]

StoragePerformanceContext:

push ebx esi edi ebp edx
mov ebp,ecx               ; EBP = Non volatile copy of buffer pointer

; Pre-clear status
xor eax,eax
mov [edx + 00],eax        ; Clear pointer to first error description string
mov [edx + 04],eax        ; Clear pointer to second error description string
mov [edx + 08],eax        ; Clear WinAPI error code

;--- (1) --- Check CPU features and measure CPU clock -------------------------;

lea eax,[Step01_MeasureClock]    ; EAX = Pointer to phase name for error report
call MessageStepName

lea ebx,[Status_CPUID]           ; EBX = Status (error description) for report
call CheckCpuId
jc .Error                        ; Go if CPUID not supported or locked
lea ebx,[Status_CPUID_1]
cmp eax,1
jc .Error                        ; Go if CPUID function 1 not sup. or locked 
;--- Detect RDTSC support ---
lea ebx,[Status_RDTSC]
mov eax,1
cpuid
test dl,10h
jz .Error                        ; Go if TSC not supported
;--- Measure TSC clock and verify results ---
lea ebx,[Status_CLK]
call MeasureCpuClk
jc .Error                        ; Go if TSC clock measurement error
lea ebx,[Status_Frequency]
test eax,eax
jz .Error                              ; Go if TSC error: frequency=0
mov dword [TscFrequencyHz + 0],eax     ; Store TSC frequency, integer, Hz
mov dword [TscFrequencyHz + 4],edx     ; High dword

finit
fild [Const1E6]
fild [TscFrequencyHz]
fdiv st0,st1
fst [TscFrequencyMHz]
fdivp st1,st0
fstp [TscPeriodPs]
mov edi,ebp
mov al,' '
stosb
mov bx,0200h
mov eax,dword [TscFrequencyMHz + 0]
mov edx,dword [TscFrequencyMHz + 4]
call DoublePrint
mov eax,' MHz'
stosd
mov ax,' ('
stosw
mov eax,dword [TscPeriodPs + 0]
mov edx,dword [TscPeriodPs + 4]
call DoublePrint
mov eax,' ps)'
stosd
mov ax,0A0Dh
stosw
mov al,0
stosb
mov ecx,ebp
call InternalConsoleWrite

;--- (2) --- OS API detection -------------------------------------------------;

lea eax,[Step02_OsApiDetection]  ; EAX = Pointer to phase name for error report
call MessageStepName

lea ebx,[Status_Handle_Module]
push NameKernel32                ; Parm#1 = Pointer to module name string
call [GetModuleHandle]           ; EAX = Return module handle
test eax,eax
jz .Error                        ; Go if KERNEL32.DLL not found
xchg esi,eax                     ; ESI = KERNEL32.DLL module handle 
;--- Get WinAPI handler for NUMA-aware memory allocation --- 
lea ebx,[Status_NUMA_Allocation]
cmp [OptionNumanode],-1
je @f
push NameVirtualAllocExNuma      ; Parm#2 = Pointer to function name
push esi                         ; Parm#1 = KERNEL32.DLL module handle
call [GetProcAddress]            ; EAX = Return function address
test eax,eax
jz .Error                        ; Go if function not found
mov [PVirtualAllocExNuma],eax
@@:
;--- Get WinAPI handler for set thread affinity mask ---
lea ebx,[Status_Affinity_Mask]
mov eax,dword [OptionAffinity]
test eax,eax
jz @f                            ; Go skip if affinity mask = 0 
cmp eax,-1
je @f                            ; Go skip if affinity mask = FF...FFh = - 1
push NameSetThreadAffinityMask
push esi
call [GetProcAddress]
test eax,eax
jz .Error
mov [PSetThreadAffinityMask],eax
@@:
call MessageStepDone
mov edi,ebp
lea esi,[MsgApi1]
call StringWrite
mov eax,[PVirtualAllocExNuma]
call HexPrint32
mov al,'h'
stosb
mov ax,0A0Dh
stosw
lea esi,[MsgApi2]
call StringWrite
mov eax,[PSetThreadAffinityMask]
call HexPrint32
mov al,'h'
stosb
mov ax,0A0Dh
stosw
mov al,0
stosb
mov ecx,ebp
call InternalConsoleWrite

;--- (3) --- Memory allocation, optional NUMA-aware ---------------------------;

lea eax,[Step03_MemoryAllocation]
call MessageStepName

lea ebx,[Status_Handle_Process]
call [GetCurrentProcess]         ; Return RAX = Handle
test eax,eax
jz .Error                        ; Go if get current process handle error
xchg ecx,eax                     ; Parm#1 = RCX = Handle
mov eax,IPB_CONST + OPB_CONST
add eax,dword [OptionBlocksize]
mov edx,[OptionFilecount]
imul edx,edx,HANDLE_SIZE
add eax,edx
test eax,0FFFh
jz @f
and eax,0FFFFF000h
add eax,000001000h               ; This for size alignment
@@:
mov [MemorySize],eax
push [OptionNumanode]          ; Parm#6 = NUMA node preferred
push PAGE_READWRITE            ; Parm#5 = Pages protection attributes = Read/Write
push MEM_RESERVE + MEM_COMMIT  ; Parm#4 = Alloc. type
push eax                       ; Parm#3 = Memory block size, with base align rsvd.
push 0                         ; Parm#2 = RDX = Base address, 0 = auto
push ecx                       ; Parm#1 = RCX = Handle
;--- Select NUMA-aware or NUMA-unaware memory allocation ---
cmp [OptionNumaaware],1
je .AllocNUMA 
call [VirtualAllocEx]
add esp,4
lea ebx,[Status_Memory_No_NUMA]
jmp .EndAlloc
.AllocNUMA:
call [PVirtualAllocExNuma]
lea ebx,[Status_Memory_NUMA] 
.EndAlloc:
;--- NUMA-aware or NUMA-unaware branch done ---
test eax,eax
jz .Error                   ; Go if memory allocation error
mov [TrueMemoryBase],eax    ; Store true base for release memory
test eax,0FFFh
jz @f
and eax,0FFFFF000h
add eax,000001000h          ; This for size alignment
@@:
mov [MemoryBase],eax        ; Store aligned base for use memory
mov [PIPB],eax              ; Setup Input Parameters Block base address
add eax,IPB_CONST
mov [POPB],eax              ; Setup Output Parameters Block base address

call MessageStepDone
mov edi,ebp
lea esi,[MsgMemoryBase]
call StringWrite
mov eax,[MemoryBase]
call HexPrint32
mov al,'h'
stosb
mov ax,0A0Dh
stosw
lea esi,[MsgMemorySize]
call StringWrite
mov eax,[MemorySize]
push eax
call HexPrint32
mov ax,'h '
stosw
mov al,'('
stosb
pop eax
mov bl,0FFh
xor edx,edx
call SizePrint64
mov al,')' 
stosb
mov ax,0A0Dh
stosw
mov al,0
stosb
mov ecx,ebp
call InternalConsoleWrite

;--- (4) --- Memory blank -----------------------------------------------------;
; No error branches for this operation, error reporting is reserved

cld
mov edi,[MemoryBase]         ; EDI = Block base
mov ecx,[MemorySize]         ; ECX = Block size, bytes count
shr ecx,2                    ; ECX = DWords count = Bytes count / 4
mov eax,'DATA'               ; This data for fill buffer
rep stosd

;--- (5) --- Set affinity mask ------------------------------------------------;

lea eax,[Step05_SetAffinity]
call MessageStepName

;--- Check affinity mask required or not ---
mov esi,dword [OptionAffinity]
test esi,esi
jz @f                  ; Skip affinitization if mask = 0
cmp esi,-1
je @f                  ; Skip affinitization if mask = -1
;--- Get thread handle ---
lea ebx,[Status_Handle_Thread]
call [GetCurrentThread]
test eax,eax
jz .Error              ; Go if thread error 
;--- Set affinity mask ---
lea ebx,[Status_Set_Affinity]
push esi               ; Parm#2 = Thread affinity mask
push eax               ; Parm#1 = Thread handle
call [SetThreadAffinityMask]
test eax,eax           ; EAX = Old affinity mask
jz .Error              ; Go if affinity mask error 
mov dword [OrigAffinity],eax
@@:
call MessageStepDone
mov edi,ebp
lea esi,[MsgOrigAffinity]
call StringWrite
mov eax,dword [OrigAffinity]
call HexPrint32
mov al,'h'
stosb
mov ax,0A0Dh
stosw
lea esi,[MsgSetAffinity]
call StringWrite
mov eax,dword [OptionAffinity]
call HexPrint32
mov al,'h'
stosb
mov ax,0A0Dh
stosw
mov al,0
stosb
mov ecx,ebp
call InternalConsoleWrite

;--- (6.1) --- Verify parameters for execute IO functions ---------------------;

lea eax,[Step06_InitializeFileIO]
call MessageStepName

mov esi,[PIPB]           ; ESI = Pointer to Input Parameters Block
mov edi,[POPB]           ; EDI = Pointer to Output Parameters Block
;--- One file size ---
lea ebx,[Status_File_Size]
mov eax,dword [OptionFilesize]
test eax,eax
jz .Error                ; Go if value  = 0
js .Error                ; Go if value bit[31] = 1 , too big or negative 
mov IPB_REQUEST_SIZE,eax
;--- Block size, per API call ---
lea ebx,[Status_Block_Size]
mov ecx,dword [OptionBlocksize]
test ecx,ecx
jz .Error                ; Go if value  = 0
js .Error                ; Go if value bit[31] = 1 , too big or negative 
mov IPB_BLOCK_SIZE,ecx
;--- Check File Size and Block Size ratio integer ---
lea ebx,[Status_Block_Ratio]
xor edx,edx
div ecx                  ; Divide ( EAX = File size ) / ( ECX = Block size ) 
test edx,edx             ; EDX = Modulo
jnz .Error               ; Go if File / Block size ratio not integer
;--- Measurement repeat count ---
lea ebx,[Status_Repeat_Count]
mov eax,[OptionFilerepeats]
test eax,eax
jz .Error                ; Go if value  = 0
js .Error                ; Go if value bit[31] = 1 , too big or negative 
mov IPB_ITERATIONS,eax
;--- Cache mode flags ---
lea ebx,[Status_Cache_Mode]
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
mov IPB_SRC_ATTRIBUTES,eax
mov IPB_DST_ATTRIBUTES,eax

;--- Extract file path prefix and postfix from path string ---
lea ecx,[BufferSrc]         ; ECX = Pointer to path string ( source file )
lea edx,[ReadPrefix]        ; EDX = Pointer for write extracted prefix
lea esi,[ReadPostfix]       ; ESI = Pointer for write extracted postfix 
mov edi,PATH_BUFFER_SIZE    ; EDI = Maximum size for all 3 buffers 
call PathExtractHelper
lea ecx,[BufferDst]         ; ECX = Pointer to path string ( destination file )
lea edx,[WritePrefix]       ; EDX = Pointer for write extracted prefix
lea esi,[WritePostfix]      ; ESI = Pointer for write extracted postfix
mov edi,PATH_BUFFER_SIZE    ; EDI = Maximum size for all 3 buffers 
call PathExtractHelper

;--- (6.2) --- Verify engineering sample restrictions -------------------------;

xor eax,eax

lea ebx,[Restriction_IOPS]
cmp [OptionMeasure],al
jne .Error

lea ebx,[Restriction_Single]           
cmp [OptionFilemode],al
jne .Error

lea ebx,[Restriction_Data]
cmp [OptionFiledata],al
jne .Error

lea ebx,[Restriction_AsyncRead]
cmp [OptionReadsync],1
jne .Error

lea ebx,[Restriction_AsyncWrite]
cmp [OptionWritesync],1
jne .Error

lea ebx,[Restriction_AsyncCopy]
cmp [OptionCopysync],1
jne .Error

lea ebx,[Restriction_ReadDelay]
cmp [OptionReaddelay],eax
jne .Error

lea ebx,[Restriction_WriteDelay]
cmp [OptionWritedelay],eax
jne .Error

lea ebx,[Restriction_CopyDelay]
cmp [OptionCopydelay],eax
jne .Error

lea ebx,[Restriction_FileHints]
cmp [OptionFilehint],al
jne .Error

lea ebx,[Restriction_MixedRead]
cmp [OptionReadmixed],eax
jne .Error

lea ebx,[Restriction_MixedWrite]
cmp [OptionWritemixed],eax
jne .Error

call MessageStepDone
lea ecx,[CrLf]
call InternalConsoleWrite

;--- (7) --- Execute selected IO functions ------------------------------------;

mov esi,[PIPB]           ; ESI = Pointer to Input Parameters Block (IPB)
mov edi,[POPB]           ; EDI = Pointer to Output Parameters Block (OPB)
call MeasureGroupFileMBPS

;--- (8) --- Analyzing file IO status -----------------------------------------;

; FILE I/O WIN API ERROR REPORTING UNDER CONSTRUCTION

;--- Check OS API status ---
;lea r15,[Step08_CheckStatus]      ; R14=0 means get error code from OS
xor ebx,ebx                        ; and use OPB_LAST_OPERATION
;mov r13,OPB_LAST_OPERATION 
cmp OPB_LAST_ERROR,0
jne .Error                          ; Go if file IO error
;cmp r13,STEP_NONE
;jne .ErrorUnknown                 ; Go if error-cause operation code set
;--- Verify data size ---
;mov rax,[OptionFilesize]  ; TODO. Lose bits RAX.63-32
;mul [OptionFilecount]
;mul [OptionFilerepeats]
;shl rdx,32
;add rax,rdx
;lea r14,[Status_Read_Mismatch]
;cmp rax,[TotalReadSizeSum]
;jne .Error                        ; Go if read size mismatch  
;lea r14,[Status_Write_Mismatch]
;cmp rax,[TotalWriteSizeSum]
;jne .Error                        ; Go if write size mismatch  
;lea r14,[Status_Copy_Mismatch]
;cmp rax,[TotalCopySizeSum]
;jne .Error                        ; Go if copy size mismatch  

;--- (9) --- Calculate file IO performance results ----------------------------;

;lea r15,[Step09_CalculateResults]
lea ebx,[Status_FPU]
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

lea ecx,[CrLf]
call InternalConsoleWrite
lea eax,[Step10_RestoreAffinity]
call MessageStepName

mov eax,dword [OptionAffinity]
test eax,eax
je @f                        ; Skip operation if affinity not changed
cmp eax,-1
je @f                        ; Skip operation if affinity not changed
;--- Get thread handle ---
lea ebx,[Status_Handle_Thread]
call [GetCurrentThread]
test eax,eax
jz .Error                    ; Go if thread error 
;--- Set affinity mask ---
lea ebx,[Status_Set_Affinity]
mov ecx,dword [OrigAffinity]  ; ECX = Thread affinity mask
jecxz @f                      ; Skip restore if wrong (not initialized) value
push ecx                      ; Parm#2 = Affinity mask
push eax                      ; Parm#1 = Thread handle
call [SetThreadAffinityMask]
test eax,eax                 ; RAX = Old affinity mask
jz .Error                    ; Go if affinity mask error 
@@:

call MessageStepDone

;--- (11) --- Release memory --------------------------------------------------;

lea eax,[Step11_ReleaseMemory]
call MessageStepName

;--- Get process handle ---
lea ebx,[Status_Handle_Process]
call [GetCurrentProcess]     ; Return EAX = Handle
test eax,eax
jz .Error                    ; Go if handle error
;--- Release memory ---
lea ebx,[Status_Memory_Release]
mov edx,[TrueMemoryBase]
test edx,edx
jz @f                        ; Skip restore if wrong (not initialized) value
push MEM_RELEASE             ; Parm#4 = Release type
push 0                       ; Parm#3 = Release size, 0 = Entire block
push edx                     ; Parm#2 = Address
push eax                     ; Parm#1 = Process Handle
call [VirtualFreeEx]
test eax,eax
jz .Error                    ; Go if function error
@@:

call MessageStepDone

;--- (12) --- Prepare results for visual --------------------------------------;

lea esi,[Parm_ReadSpeed]
mov edi,ebp
;--- Benchmarks results: Read speed ---
mov eax,dword [ReadMBPS + 0]
mov edx,dword [ReadMBPS + 4]
call HelperMBPS
;--- Benchmarks results: Write speed ---
mov eax,dword [WriteMBPS + 0]
mov edx,dword [WriteMBPS + 4]
call HelperMBPS
;--- Benchmarks results: Copy speed ---
mov eax,dword [CopyMBPS + 0]
mov edx,dword [CopyMBPS + 4]
call HelperMBPS
;--- Skip string and terminate sequence ---
mov ax,0A0Dh
stosw
mov al,0
stosb

;--- (13) --- Visual results --------------------------------------------------;

mov ecx,ebp
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
jmp .NoError

;--- (14) --- Exit measurement scenario ---------------------------------------;

; Information about error:
; EBX = Pointer to status string, error description.    

.ErrorUnknown:
lea ebx,[Status00_Unknown]

.Error:
mov ecx,ERROR_POINTER
test ebx,ebx
jnz @f                     ; Skip OS error code if EBX = Pointer valid
mov edi,[POPB]
mov eax,OPB_LAST_ERROR     ; This code (OS error code) valid if EBX = 0
mov [ecx + 08],eax         ; Return OS error code
lea ebx,[Status00_WinAPI]  ; EBX = Comment string for WinAPI returned error
@@:
mov [ecx + 00],ebx         ; Return pointer to status string, error description
lea ecx,[CrLf]
call InternalConsoleWrite
xor eax,eax   ; EAX = 0, error, status variables at [input EDX] valid
jmp .Exit

.NoError:
mov eax,1     ; EAX = 1, no errors, status variables at [input EDX] not valid
.Exit:
pop ebp ebp edi esi ebx
ret
