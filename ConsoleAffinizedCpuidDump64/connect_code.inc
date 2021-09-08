;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                   connect include files with subroutines.                    ; 
;------------------------------------------------------------------------------;

;--- Affinized CPUID application scenario -------------------------------------;
; Input/Output parameters and volatile registers compatible with               ; 
; Microsoft x64 calling convention.                                            ;
; INPUT:   RCX = Pointer to HardwareShell status variables block               ;
;          RDX = Pointer to HardwareShell temporary buffer                     ;
; OUTPUT:  RAX = Status, 0=Error, otherwise no errors                          ;
;------------------------------------------------------------------------------;
ApplicationAffinizedCpuid:
push rbx rsi rdi rbp r12 r13 r14 r15 
mov rbp,rsp              ; RBP = Storage for RSP 
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov r15,rcx              ; R15 = Pointer to error status block
xor eax,eax
mov ERROR_STRING_1,rax
mov ERROR_STRING_2,rax
mov ERROR_CODE,rax
mov r14,rdx              ; R14 = Temporary buffer base
xor r13,r13              ; R13 = Memory buffer base, yet blank for skip release
mov [OriginalAffinityMask],rax
;---------- Check CPUID support by check bit RFLAGS.21 writeable --------------; 
mov ERROR_STRING_1, MsgErrorCpuid  ; Write status for step: check CPUID support
mov ebx,21               ; Start check ID bit writeable for "1"
pushf                    ; In the 64-bit mode, push RFLAGS
pop rax                  
bts eax,ebx              ; Set EAX.21=1
push rax
popf                     ; Load RFLAGS with RFLAGS.21=1
pushf                    ; Store RFLAGS
pop rax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=1, Set EAX.21=0
jnc .error               ; Go error branch if cannot set EFLAGS.21=1
push rax                 ; Start check ID bit writeable for "0"
popf                     ; Load RFLAGS with RFLAGS.21=0
pushf                    ; Store RFLAGS
pop rax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=0
jc .error                ; Go error branch if cannot set EFLAGS.21=0
;---------- Get system information and WinAPI pointers ------------------------;
lea rcx,[SystemInfo]
call [GetSystemInfo] 
mov ERROR_STRING_1, MsgErrorWinAPI
call [GetCurrentThread]
test rax,rax
jz .errorSystem
xchg r12,rax            ; R12 = Current thread handle, used for affinization
lea rcx,[NameDll]
call [GetModuleHandle]
test rax,rax
jz .errorSystem
mov ERROR_STRING_1, MsgErrorAffin
lea rdx,[NameFunctionAffin]
xchg rcx,rax
call [GetProcAddress]
test rax,rax
jz .errorSystem
mov [_SetThreadAffinityMask],rax
;---------- Memory allocation for buffer --------------------------------------;
mov ERROR_STRING_1, MsgErrorAlloc
xor ecx,ecx
mov edx,BUFFER_SIZE
mov r8d,MEM_COMMIT + MEM_RESERVE
mov r9d,PAGE_READWRITE
call [VirtualAlloc]
test rax,rax
jz .errorSystem
xchg r13,rax             ; R13 = Base address of allocated block
;---------- Initializing cycle for logical processors -------------------------;
mov ERROR_STRING_1, MsgErrorAffinFailed
mov eax,[SystemInfo.dwNumberOfProcessors]
test eax,eax
jz .error
mov ERROR_STRING_1, MsgErrorCpuLimit
cmp eax,PROCESSORS_LIMIT
ja .error
mov [ProcessorCounter],0
mov [AffinityMask],1
;---------- Begin cycle for logical processors, thread affinization -----------;
.dumpProcessors:
cld
mov ERROR_STRING_1, MsgErrorAffinFailed
mov rcx,r12
mov rdx,[AffinityMask]
call [_SetThreadAffinityMask]
test rax,rax
jz .errorSystem
lea rdx,[OriginalAffinityMask]
cmp qword [rdx],0
jne .alreadySaved
mov [rdx],rax
.alreadySaved:
;---------- Get CPUID binary dump, iteration for one logical CPU --------------;
mov ERROR_STRING_1, MsgErrorCpuidFailed
mov rdi,r13
call GetCPUID
mov ebx,[rdi]
test ebx,ebx
jz .error
cmp ebx,ENTRIES_LIMIT
ja .error 
;---------- Output CPUID dump to console --------------------------------------;
push rbx rdi
mov rdi,r14
mov ax,'# '
stosw
mov eax,[ProcessorCounter]
mov bl,0
call DecimalPrint32
mov al,0
stosb
mov rcx,r14
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
pop rdi rbx
lea rsi,[rdi + 32]
.dumpFunctions:
mov ecx,CPUID_NAME_COLOR
call SetFgColor
lea rcx,[NameCPUID]
call InternalConsoleWrite 
mov rdi,r14
mov eax,[rsi + 04]
call HexPrint32
mov al,':'
stosb
mov ax,0000h + ' '
stosw
mov rcx,r14
call InternalConsoleWrite
mov ecx,CPUID_VALUE_COLOR
call SetFgColor
mov rdi,r14
mov eax,[rsi + 16]
call HexPrint32
mov al,'-'
stosb
mov eax,[rsi + 20]
call HexPrint32
mov al,'-'
stosb
mov eax,[rsi + 24]
call HexPrint32
mov al,'-'
stosb
mov eax,[rsi + 28]
call HexPrint32
mov al,0
stosb
mov rcx,r14
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,32                  ; Select next CPUID function (subfunction) entry
dec ebx                     ; Cycle counter for CPUID functions (subfunctions)
jnz .dumpFunctions         
call GetColor               ; Restore original color
xchg ecx,eax
call SetColor
lea rcx,[CrLf]              ; Insert empty string
call InternalConsoleWrite
;---------- Make cycle for logical processors ---------------------------------;
shl [AffinityMask],1
lea rdx,[ProcessorCounter]
inc dword [rdx] 
mov eax,[SystemInfo.dwNumberOfProcessors]
cmp [rdx],eax
jb .dumpProcessors
;---------- Exit points -------------------------------------------------------;
mov eax,1
jmp .exit
.errorMemory:
mov ERROR_STRING_1, MsgErrorRelease
jmp .errorSystem
.errorAffinity:
mov ERROR_STRING_1, MsgErrorAffinFailed
.errorSystem:
call [GetLastError]
mov ERROR_CODE,rax 
.error:
xor eax,eax
.exit:
mov rbx,rax
xor ecx,ecx
xchg rcx,r13
jrcxz .free
xor edx,edx
mov r8d,MEM_RELEASE
call [VirtualFree]
test rax,rax
jz .errorMemory
.free:
mov rcx,r12
xor rdx,rdx
xchg rdx,[OriginalAffinityMask]
test rdx,rdx
jz .unchanged
call [_SetThreadAffinityMask]
test rax,rax
jz .errorAffinity
.unchanged:
mov rax,rbx
mov rsp,rbp
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret

;---------- This code imported from Java CPUID project, -----------------------;
;---------- from JNI native library for Windows x64, with some changes --------;

;---------- Get CPUID dump ----------------------------------------------------;
; Parm#1 = RDI = Pointer to buffer for status and dump data                    ;
; Output = RAX = Status: 0=Error, Non-Zero=OK, set external. at transit caller ;
;          Buffer DWORD[0] = Number of entries returned                        ;
;          Bytes [4-31] = Reserved for alignment                               ;
;          Bytes [32-16383] = Buffer, maximum (16384-32)/32 = 511 entries ret. ;
;------------------------------------------------------------------------------;
GetCPUID:
push rdi
cld
mov ecx,8
xor eax,eax
rep stosd      ; blank first 8 * 4 = 32 bytes
mov rcx,rdi
call Internal_GetCPUID
pop rdi
mov [rdi],eax
ret
; Output buffer restrictions, for prevent buffer overflow
; when invalid CPUID data returned
; 511 (not 512) because entry 0 used for data size return
; -1 because, for example, Limit=1 means only function 0 supported
; -2 because 511 entries, not 512
ENTRIES_LIMIT  = 511      ; Output buffer maximum size = 16352 bytes = 511*32
STANDARD_LIMIT = 192 - 1  ; Limit for standard CPUID functions 000000xxh
EXTENDED_LIMIT = 192 - 1  ; Limit for extended CPUID functions 800000xxh
VIRTUAL_LIMIT  = 128 - 2  ; Limit for virtual CPUID functions  400000xxh
;---------- Target subroutine -------------------------------------------------;
; INPUT:  Parameter#1 = RCX = Pointer to output buffer
; OUTPUT: RAX = Number of output entries
;         Output buffer updated
;---
; Output buffer maximum size is 16352 bytes, 511 entries * 32 bytes
; Each entry is 32 bytes, 8 dwords:
; dword   offset in entry(hex)   comments
;--------------------------------------------------------------------------
;   0     00-03                  Information type tag, 0 for CPUID info                  
;   1     04-07                  CPUID function number
;   2     08-0B                  CPUID subfunction number
;   3     0C-0F                  CPUID pass number (as for function #2)
;   4     10-13                  Result EAX after CPUID
;   5     14-17                  Result EBX after CPUID
;   6     18-1B                  Result ECX after CPUID
;   7     1C-1F                  Result EDX after CPUID
;--------------------------------------------------------------------------
Internal_GetCPUID:
;---------- Initializing ------------------------------------------------------;
cld
push rbx rbp rsi rdi
mov rdi,rcx
xor ebp,ebp               ; EBP = Global output entries counter
;---
; This skipped, because CPUID support verified in the caller at
; HardwareShell project.
;
; call CheckCPUID         ; Return CF=Error flag, EAX=Maximum standard function
; jc NoCpuId
;---
;---------- Get standard CPUID results ----------------------------------------;
xor r9d,r9d               ; R9D  = standard functions start
;--- added for HardwareShell variant ---
mov eax,r9d               ; EAX = Function number for CPUID instruction
xor ecx,ecx               ; ECX = Subfunction, here redundant
cpuid
;--- end of added for HardwareShell variant ---
cmp eax,STANDARD_LIMIT    ; EAX = maximum supported standard function number
ja ErrorCpuId             ; Go if invalid limit
call SequenceCpuId
jc ErrorCpuId             ; Exit if output buffer overflow at subfunction
;---------- Get virtual CPUID results -----------------------------------------;
mov r9d,040000000h        ; R9D = virtual functions start number
mov eax,r9d               ; EAX = Function number for CPUID instruction
xor ecx,ecx               ; ECX = Subfunction, here redundant
cpuid
mov ecx,eax
and ecx,0FFFFFF00h        ; ECX = Pattern bits for check virtual CPUID support
cmp ecx,r9d               ; Compare pattern bits
jne NoVirtual             ; Skip virtual CPUID if not supported
cmp eax,40000000h + VIRTUAL_LIMIT  ; EAX = maximum extended function number
ja ErrorCpuId             ; Go if invalid limit, too big function number
call SequenceCpuId
jc ErrorCpuId             ; Exit if output buffer overflow at subfunction
NoVirtual:                ; This label for skip virtual functions
;---------- Get extended CPUID results ----------------------------------------;
mov r9d,80000000h         ; R9D  = extended functions start
mov eax,r9d
cpuid
test eax,eax
jns NoExtended            ; Go skip extended functions if bit EAX.31 = 0
cmp eax,80000000h + EXTENDED_LIMIT  ; EAX = maximum extended function number
ja ErrorCpuId             ; Go if invalid limit
call SequenceCpuId
jc ErrorCpuId             ; Exit if output buffer overflow
NoExtended:
;---------- Return points -----------------------------------------------------;
xchg eax,ebp              ; Normal exit point, return RAX = number of entries
ExitCpuId:
pop rdi rsi rbp rbx
ret
NoCpuId:                  ; Exit for CPUID not supported, RAX=0  
xor eax,eax
jmp ExitCpuId
ErrorCpuId:               ; Exit for CPUID error, RAX=-1=FFFFFFFFFFFFFFFFh
mov rax,-1
jmp ExitCpuId 
;---------- Subroutine, sequence of CPUID functions ---------------------------;
; INPUT:  R9D = Start CPUID function number
;         EAX = Limit CPUID function number (inclusive)
;         RDI = Pointer to memory buffer
; OUTPUT: RDI = Modified by store CPUID input parms + output parms entry
;         Flags condition code: Carry (C) = means entries count limit
;------------------------------------------------------------------------------;
SequenceCpuId:
mov r10d,eax              ; R10D = standard or extended functions limit 
CycleCpuId:
;--- Specific handling for functions with subfunctions ---
mov eax,r9d           ; EAX = function number, input at R9D
cmp eax,04h
je Function04
cmp eax,07h
je Function07
cmp eax,0Bh
je Function0B
cmp eax,0Dh
je Function0D
cmp eax,0Fh
je Function0F
cmp eax,10h
je Function10
cmp eax,14h
je Function14
cmp eax,17h
je Function17
cmp eax,18h
je Function18
cmp eax,1Dh
je Function1D
cmp eax,1Fh
je Function1F
cmp eax,8000001Dh
je Function04
cmp eax,80000020h
je Function10
;--- Default handling for functions without subfunctions ---
xor esi,esi           ; ESI = sub-function number for save entry
xor ecx,ecx           ; ECX = sub-function number for CPUID  
call StoreCpuId
ja OverSubFunction
AfterSubFunction:     ; Return point after sub-function specific handler
inc r9d
cmp r9d,r10d
jbe CycleCpuId        ; Cycle for CPUID standard functions
ret
OverSubFunction:
stc
ret 
;---------- CPUID function 04h = Deterministic cache parameters ---------------;
Function04:
xor esi,esi           ; ESI = Storage for sub-function number
.L0:
mov eax,r9d           ; EAX = function number
mov ecx,esi           ; ECX = subfunction number
cpuid
test al,00011111b     ; Check for subfunction list end
jz AfterSubFunction   ; Go if reach first not valid subfunction
call StoreCpuId_Entry
ja OverSubFunction    ; Go if output buffer overflow
inc esi               ; Sunfunctions number +1
jmp .L0               ; Go repeat for next subfunction
;---------- CPUID function 07h = Structured extended feature flags ------------;   
Function07:
xor esi,esi           ; ESI = Storage for sub-function number
mov ecx,esi
mov eax,r9d           ; EAX = function number (BUGGY DUPLICATED)
cpuid
mov r8d,eax           ; R8D = Maximal sub-function number
.L0:
mov eax,r9d
mov ecx,esi           ; ECX = Current sub-function number
call StoreCpuId
ja OverSubFunction    ; Go if output buffer overflow
inc esi               ; Sunfunctions number +1
cmp esi,r8d           ; 
jbe .L0               ; Go cycle if next sub-function exist
jmp AfterSubFunction
;---------- CPUID function 0Bh = Extended topology enumeration ----------------;
;---------- CPUID function 1Fh = V2 Extended topology enumeration -------------;
Function0B:
Function1F:
xor esi,esi           ; ESI = Storage for sub-function number
.L0:
mov eax,r9d           ; EAX = function number
mov ecx,esi           ; ECX = subfunction number
cpuid
test eax,eax          ; Check for subfunction list end
jz AfterSubFunction   ; Go if reach first not valid subfunction
call StoreCpuId_Entry
ja OverSubFunction    ; Go if output buffer overflow
inc esi               ; Sunfunctions number +1
jmp .L0               ; Go repeat for next subfunction
;---------- CPUID function 0Dh = Processor extended state enumeration ---------;
Function0D:
mov eax,r9d           ; EAX = function number
xor ecx,ecx           ; ECX = sub-function number
cpuid
shl rdx,32
lea r8,[rdx+rax]
xor esi,esi           ; ESI = Storage for sub-function number
.L2:
shr r8,1
jnc .L3
mov eax,r9d
mov ecx,esi           ; ECX = Sub-function number
call StoreCpuId
ja OverSubFunction    ; Go if output buffer overflow
.L3:
inc esi               ; Sunfunctions number +1
cmp esi,63            ; 
jbe .L2               ; Go cycle if next sub-function exist
jmp AfterSubFunction 
;---------- CPUID function 0Fh = Platform QoS monitoring enumeration ----------;
;---------- CPUID function 10h = L3 cache QoS enforcement enumeration (same) --;
Function0F:
Function10:
xor esi,esi           ; ESI = sub-function number for save entry 
xor ecx,ecx           ; ECX = sub-function number for CPUID 
push rax r9       
call StoreCpuId       ; Subfunction 0 of fixed list [0,1]
pop r9 rax
ja OverSubFunction    ; Go if output buffer overflow
mov esi,1
mov ecx,esi
call StoreCpuId       ; Subfunction 1 of fixed list [0,1]
ja OverSubFunction    ; Go if output buffer overflow
jmp AfterSubFunction
;---------- CPUID function 14h = Intel Processor Trace Enumeration ------------;
;---------- CPUID function 17h = System-On-Chip Vendor Attribute Enumeration --;
;---------- CPUID function 1Dh = Intel AMX Tile Information -------------------;
Function14:
Function17:
Function1D:
xor esi,esi           ; ESI = Storage for sub-function number
mov ecx,esi
mov eax,r9d           ; EAX = function number (BUGGY DUPLICATED)
cpuid
mov r8d,eax           ; R8D = Maximal sub-function number
.L0:
mov eax,r9d
mov ecx,esi           ; ECX = Current sub-function number
call StoreCpuId
ja OverSubFunction    ; Go if output buffer overflow
inc esi               ; Sunfunctions number +1
cmp esi,r8d           ; 
jbe .L0               ; Go cycle if next sub-function exist
jmp AfterSubFunction
;---------- CPUID function 18h = Deterministic Address Translation Parms. -----;
Function18:
xor esi,esi           ; ESI = Storage for sub-function number
mov ecx,esi
mov eax,r9d           ; EAX = function number (BUGGY DUPLICATED)
cpuid
mov r8d,eax           ; R8D = Maximal sub-function number
jmp .L2
.L0:
mov eax,r9d
mov ecx,esi           ; ECX = Current sub-function number
cpuid
test dl,00011111b     ; Check TLB deterministic data validity
jz .L1                ; Go skip if subfunction invalid, can be unordered
.L2:
call StoreCpuId_Entry
ja OverSubFunction    ; Go if output buffer overflow
.L1:
inc esi               ; Sunfunctions number +1
cmp esi,r8d           ; 
jbe .L0               ; Go cycle if next sub-function exist
jmp AfterSubFunction
;---------- Subroutine, one CPUID function execution --------------------------;
; INPUT:  EAX = CPUID function number
;         R9D = EAX
;         ECX = CPUID subfunction number
;         ESI = ECX
;         RDI = Pointer to memory buffer
; OUTPUT: RDI = Modified by store CPUID input parms + output parms entry
;         Flags condition code: Above (A) = means entries count limit
;------------------------------------------------------------------------------;
StoreCpuId:
cpuid
StoreCpuId_Entry:     ; Entry point for CPUID results (EAX,EBX,ECX,EDX) ready 
push rax
xor eax,eax
stosd                 ; Store tag dword[0] = Information type
mov eax,r9d
stosd                 ; Store argument dword [1] = CPUID function number 
mov eax,esi
stosd                 ; Store argument dword [2] = CPUID sub-function number
xor eax,eax
stosd                 ; Store argument dword [3] = CPUID pass number (see fn.2)
pop rax
stosd                 ; Store result dword [4] = output EAX 
xchg eax,ebx
stosd                 ; Store result dword [5] = output EBX
xchg eax,ecx
stosd                 ; Store result dword [6] = output ECX
xchg eax,edx
stosd                 ; Store result dword [7] = output EDX
inc ebp               ; Global counter +1
cmp ebp,ENTRIES_LIMIT ; Limit for number of output entries
ret

;---------- End of code imported from Java CPUID project, ---------------------;

