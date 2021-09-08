;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                   connect include files with subroutines.                    ; 
;------------------------------------------------------------------------------;

;--- Affinized CPUID application scenario -------------------------------------;
; INPUT:   ECX = Pointer to HardwareShell status variables block               ;
;          EDX = Pointer to HardwareShell temporary buffer                     ;
; OUTPUT:  EAX = Status, 0=Error, otherwise no errors                          ;
;------------------------------------------------------------------------------;
ApplicationAffinizedCpuid:
push ebx esi edi ebp 
mov ebx,ecx                 ; EBX = Pointer to error status block
xor eax,eax
mov ERROR_STRING_1,eax
mov ERROR_STRING_2,eax
mov ERROR_CODE,eax
mov [TempBufferBase],edx    ; Store Temporary buffer base
mov [MemoryBufferBase],eax  ; Memory buffer base, yet blank for skip release
mov [OriginalAffinityMask],eax
;---------- Check CPUID support by check bit RFLAGS.21 writeable --------------; 
mov ERROR_STRING_1, MsgErrorCpuid  ; Write status for step: check CPUID support
mov ebp,21               ; Start check ID bit writeable for "1"
pushf                    ; In the 64-bit mode, push RFLAGS
pop eax                  
bts eax,ebp              ; Set EAX.21=1
push eax
popf                     ; Load RFLAGS with RFLAGS.21=1
pushf                    ; Store RFLAGS
pop eax                  ; Load RFLAGS to RAX
btr eax,ebp              ; Check EAX.21=1, Set EAX.21=0
jnc .error               ; Go error branch if cannot set EFLAGS.21=1
push eax                 ; Start check ID bit writeable for "0"
popf                     ; Load RFLAGS with RFLAGS.21=0
pushf                    ; Store RFLAGS
pop eax                  ; Load RFLAGS to RAX
btr eax,ebp              ; Check EAX.21=0
jc .error                ; Go error branch if cannot set EFLAGS.21=0
;---------- Get system information and WinAPI pointers ------------------------;
push SystemInfo
call [GetSystemInfo] 
mov ERROR_STRING_1, MsgErrorWinAPI
call [GetCurrentThread]
test eax,eax
jz .errorSystem
mov [ThreadHandle],eax     ; Store current thread handle, used for affinization
push NameDll
call [GetModuleHandle]
test eax,eax
jz .errorSystem
mov ERROR_STRING_1, MsgErrorAffin
push NameFunctionAffin
push eax
call [GetProcAddress]
test eax,eax
jz .errorSystem
mov [_SetThreadAffinityMask],eax
;---------- Memory allocation for buffer --------------------------------------;
mov ERROR_STRING_1, MsgErrorAlloc
push PAGE_READWRITE
push MEM_COMMIT + MEM_RESERVE
push BUFFER_SIZE
push 0
call [VirtualAlloc]
test eax,eax
jz .errorSystem
mov [MemoryBufferBase],eax          ; Store base address of allocated block
;;---------- Initializing cycle for logical processors -------------------------;
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
push [AffinityMask]
push [ThreadHandle]
call [_SetThreadAffinityMask]
test eax,eax
jz .errorSystem
lea edx,[OriginalAffinityMask]
cmp dword [edx],0
jne .alreadySaved
mov [edx],eax
.alreadySaved:
;---------- Get CPUID binary dump, iteration for one logical CPU --------------;
mov ERROR_STRING_1, MsgErrorCpuidFailed
mov edi,[MemoryBufferBase]
call GetCPUID
mov ebp,[edi]
test ebp,ebp
jz .error
cmp ebp,ENTRIES_LIMIT
ja .error 
;---------- Output CPUID dump to console --------------------------------------;
push edi
mov edi,[TempBufferBase]
mov ax,'# '
stosw
mov eax,[ProcessorCounter]
mov bl,0
call DecimalPrint32
mov al,0
stosb
mov ecx,[TempBufferBase]
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
pop edi
lea esi,[edi + 32]
.dumpFunctions:
mov ecx,CPUID_NAME_COLOR
call SetFgColor
lea ecx,[NameCPUID]
call InternalConsoleWrite 
mov edi,[TempBufferBase]
mov eax,[esi + 04]
call HexPrint32
mov al,':'
stosb
mov ax,0000h + ' '
stosw
mov ecx,[TempBufferBase]
call InternalConsoleWrite
mov ecx,CPUID_VALUE_COLOR
call SetFgColor
mov edi,[TempBufferBase]
mov eax,[esi + 16]
call HexPrint32
mov al,'-'
stosb
mov eax,[esi + 20]
call HexPrint32
mov al,'-'
stosb
mov eax,[esi + 24]
call HexPrint32
mov al,'-'
stosb
mov eax,[esi + 28]
call HexPrint32
mov al,0
stosb
mov ecx,[TempBufferBase]
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
add esi,32                  ; Select next CPUID function (subfunction) entry
dec ebp                     ; Cycle counter for CPUID functions (subfunctions)
jnz .dumpFunctions         
call GetColor               ; Restore original color
xchg ecx,eax
call SetColor
lea ecx,[CrLf]              ; Insert empty string
call InternalConsoleWrite
;---------- Make cycle for logical processors ---------------------------------;
shl [AffinityMask],1
lea edx,[ProcessorCounter]
inc dword [edx] 
mov eax,[SystemInfo.dwNumberOfProcessors]
cmp [edx],eax
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
mov ERROR_CODE,eax 
.error:
xor eax,eax
.exit:
mov ebx,eax
xor ecx,ecx
xchg ecx,[MemoryBufferBase]
jecxz .free
push MEM_RELEASE
push 0
push ecx
call [VirtualFree]
test eax,eax
jz .errorMemory
.free:
xor edx,edx
xchg edx,[OriginalAffinityMask]
test edx,edx
jz .unchanged
push edx
push [ThreadHandle]
call [_SetThreadAffinityMask]
test eax,eax
jz .errorAffinity
.unchanged:
mov eax,ebx
pop ebp edi esi ebx
ret

;---------- This code imported from Java CPUID project, -----------------------;
;---------- from JNI native library for Windows x64, with some changes --------;

;---------- Get CPUID dump ----------------------------------------------------;
; Parm#1 = EDI = Pointer to buffer for status and dump data                    ;
; Output = EAX = Status: 0=Error, Non-Zero=OK, set external. at transit caller ;
;          Buffer DWORD[0] = Number of entries returned                        ;
;          Bytes [4-31] = Reserved for alignment                               ;
;          Bytes [32-16383] = Buffer, maximum (16384-32)/32 = 511 entries ret. ;  
;------------------------------------------------------------------------------;
GetCPUID:
push edi                 ; (+1)
cld
mov ecx,8
xor eax,eax
rep stosd                ; blank first 8 * 4 = 32 bytes
push edi                 ; Parm#1
call Internal_GetCPUID
pop edi                  ; (-1)
mov [edi],eax
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
; INPUT:  Parameter#1 = [esp+4] = Pointer to output buffer
; OUTPUT: EAX = Number of output entries
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
;---
Internal_GetCPUID:
;---------- Initializing ------------------------------------------------------;

temp_r8   EQU  dword [ebp+00]  ; this for porting from x64 code
temp_r9   EQU  dword [ebp+04]
temp_r10  EQU  dword [ebp+08]
temp_ebp  EQU  dword [ebp+12]

cld
;--- Store registers ---
push ebx ebp esi edi
;--- Variables pool ---
xor eax,eax
push eax eax eax eax
mov ebp,esp
;--- Start ---
mov edi,[esp+32+4]
mov temp_ebp,0            ; xor ebp,ebp ; EBP = Global output entries counter
;
; This skipped, because CPUID support verified in the caller at
; HardwareShell project.
;
; call CheckCPUID           ; Return CF=Error flag, EAX=Maximum standard function
; jc NoCpuId
;
;---
;---------- Get standard CPUID results ----------------------------------------;
mov temp_r9,0             ; xor r9d,r9d  ; R9D  = standard functions start
;--- added for HardwareShell variant ---
mov eax,temp_r9           ; EAX = Function number for CPUID instruction
xor ecx,ecx               ; ECX = Subfunction, here redundant
cpuid
;--- end of added for HardwareShell variant ---
cmp eax,STANDARD_LIMIT    ; EAX = maximum supported standard function number
ja ErrorCpuId             ; Go if invalid limit
call SequenceCpuId
jc ErrorCpuId             ; Exit if output buffer overflow at subfunction

;---------- Get virtual CPUID results -----------------------------------------;
mov temp_r9,040000000h    ; R9D = virtual functions start number
mov eax,temp_r9           ; EAX = Function number for CPUID instruction
xor ecx,ecx               ; ECX = Subfunction, here redundant
cpuid
mov ecx,eax
and ecx,0FFFFFF00h        ; ECX = Pattern bits for check virtual CPUID support
cmp ecx,temp_r9           ; Compare pattern bits
jne NoVirtual             ; Skip virtual CPUID if not supported
cmp eax,40000000h + VIRTUAL_LIMIT  ; EAX = maximum extended function number
ja ErrorCpuId             ; Go if invalid limit, too big function number
call SequenceCpuId
jc ErrorCpuId             ; Exit if output buffer overflow at subfunction
NoVirtual:                ; This label for skip virtual functions

;---------- Get extended CPUID results ----------------------------------------;
mov temp_r9,80000000h     ; mov r9d,80000000h ; R9D  = extended functions start
mov eax,temp_r9           ; r9d
cpuid
test eax,eax
jns NoExtended            ; Go skip extended functions if bit EAX.31 = 0
cmp eax,80000000h + EXTENDED_LIMIT  ; EAX = maximum extended function number
ja ErrorCpuId             ; Go if invalid limit
call SequenceCpuId
jc ErrorCpuId             ; Exit if output buffer overflow
NoExtended:

;---------- Return points -----------------------------------------------------;
mov eax,temp_ebp          ; Normal exit point, return EAX = number of entries
ExitCpuId:
add esp,16
pop edi esi ebp ebx
ret 4
NoCpuId:                  ; Exit for CPUID not supported, EAX=0  
xor eax,eax
jmp ExitCpuId
ErrorCpuId:               ; Exit for CPUID error, EAX=-1=FFFFFFFFFFFFFFFFh
mov eax,-1
jmp ExitCpuId 

;---------- Subroutine, sequence of CPUID functions ---------------------------;
; INPUT:  R9D = Start CPUID function number
;         EAX = Limit CPUID function number (inclusive)
;         EDI = Pointer to memory buffer
; OUTPUT: EDI = Modified by store CPUID input parms + output parms entry
;         Flags condition code: Carry (C) = means entries count limit
;------------------------------------------------------------------------------;
SequenceCpuId:
mov temp_r10,eax   ; mov r10d,eax ; R10D = standard or extended functions limit 
CycleCpuId:
;--- Specific handling for functions with subfunctions ---
mov eax,temp_r9    ; r9d ; EAX = function number, input at R9D
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
xor esi,esi               ; ESI = sub-function number for save entry
xor ecx,ecx               ; ECX = sub-function number for CPUID  
call StoreCpuId
ja OverSubFunction
AfterSubFunction:         ; Return point after sub-function specific handler
mov eax,temp_r9
inc eax
mov temp_r9,eax
cmp eax,temp_r10
jbe CycleCpuId            ; Cycle for CPUID standard functions
ret
OverSubFunction:
stc
ret 

;---------- CPUID function 04h = Deterministic cache parameters ---------------;
Function04:
xor esi,esi           ; ESI = Storage for sub-function number
.L0:
mov eax,temp_r9       ; r9d ; EAX = function number
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
mov eax,temp_r9       ; r9d ; EAX = function number (BUGGY DUPLICATED)
cpuid
mov temp_r8,eax       ; r8d,eax ; R8D = Maximal sub-function number
.L0:
mov eax,temp_r9       ; r9d
mov ecx,esi           ; ECX = Current sub-function number
call StoreCpuId
ja OverSubFunction    ; Go if output buffer overflow
inc esi               ; Sunfunctions number +1
cmp esi,temp_r8       ; r8d 
jbe .L0               ; Go cycle if next sub-function exist
jmp AfterSubFunction
;---------- CPUID function 0Bh = Extended topology enumeration ----------------;
;---------- CPUID function 1Fh = V2 Extended topology enumeration -------------;
Function0B:
Function1F:
xor esi,esi           ; ESI = Storage for sub-function number
.L0:
mov eax,temp_r9       ; r9d ; EAX = function number
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
mov eax,temp_r9       ; r9d ; EAX = function number
xor ecx,ecx           ; ECX = sub-function number
cpuid
xor esi,esi           ; ESI = Storage for sub-function number
.L2:
rcr edx,1
rcr eax,1
jnc .L3
push eax edx
mov eax,temp_r9       ; r9d
mov ecx,esi           ; ECX = Sub-function number
call StoreCpuId
pop edx eax
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
push eax temp_r9      ; r9       
call StoreCpuId       ; Subfunction 0 of fixed list [0,1]
pop temp_r9 eax       ; r9
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
mov eax,temp_r9       ; r9d ; EAX = function number (BUGGY DUPLICATED)
cpuid
mov temp_r8,eax       ; r8d,eax ; R8D = Maximal sub-function number
.L0:
mov eax,temp_r9       ; r9d
mov ecx,esi           ; ECX = Current sub-function number
call StoreCpuId
ja OverSubFunction    ; Go if output buffer overflow
inc esi               ; Sunfunctions number +1
cmp esi,temp_r8       ; r8d 
jbe .L0               ; Go cycle if next sub-function exist
jmp AfterSubFunction
;---------- CPUID function 18h = Deterministic Address Translation Parms. -----;
Function18:
xor esi,esi           ; ESI = Storage for sub-function number
mov ecx,esi
mov eax,temp_r9       ; r9d ; EAX = function number (BUGGY DUPLICATED)
cpuid
mov temp_r8,eax       ; r8d,eax ; R8D = Maximal sub-function number
jmp .L2
.L0:
mov eax,temp_r9       ; r9d
mov ecx,esi           ; ECX = Current sub-function number
cpuid
test dl,00011111b     ; Check TLB deterministic data validity
jz .L1                ; Go skip if subfunction invalid, can be unordered
.L2:
call StoreCpuId_Entry
ja OverSubFunction    ; Go if output buffer overflow
.L1:
inc esi               ; Sunfunctions number +1
cmp esi,temp_r8       ; r8d 
jbe .L0               ; Go cycle if next sub-function exist
jmp AfterSubFunction

;---------- Subroutine, one CPUID function execution --------------------------;
; INPUT:  EAX = CPUID function number
;         R9D = EAX ( R8-R15 emulated in memory, because port from x64 )
;         ECX = CPUID subfunction number
;         ESI = ECX
;         EDI = Pointer to memory buffer
; OUTPUT: EDI = Modified by store CPUID input parms + output parms entry
;         Flags condition code: Above (A) = means entries count limit
;------------------------------------------------------------------------------;
StoreCpuId:
cpuid
StoreCpuId_Entry:     ; Entry point for CPUID results (EAX,EBX,ECX,EDX) ready 
push eax
xor eax,eax
stosd                 ; Store tag dword[0] = Information type
mov eax,temp_r9       ; r9d
stosd                 ; Store argument dword [1] = CPUID function number 
mov eax,esi
stosd                 ; Store argument dword [2] = CPUID sub-function number
xor eax,eax
stosd                 ; Store argument dword [3] = CPUID pass number (see fn.2)
pop eax
stosd                 ; Store result dword [4] = output EAX 
xchg eax,ebx
stosd                 ; Store result dword [5] = output EBX
xchg eax,ecx
stosd                 ; Store result dword [6] = output ECX
xchg eax,edx
stosd                 ; Store result dword [7] = output EDX
inc temp_ebp          ; ebp ; Global counter +1
cmp temp_ebp,ENTRIES_LIMIT  ; ebp ; Limit for number of output entries
ret

;---------- End of code imported from Java CPUID project, ---------------------;

