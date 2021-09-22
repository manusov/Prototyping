; UNDER CONSTRUCTION (x64).
; Can destroy registers, volatile by Microsoft x64 calling convention.

SysinfoUserMode:
push rbx rsi rdi rbp r12 r13 r14 r15
cld
mov r15,[Registry]                               ; R15 = Registry base address
lea rdi,[r15 + REGISTRY64.cpuData.vendorString]  ; RDI = Destination pointer
mov r14,rdi                                      ; R14 = Destination base
;---------- Check CPUID instruction support, get 12-chars vendor string -------;
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
xor eax,eax    ; *** DEBUG ***
stosq          ; *** DEBUG ***
;---------- Measure TSC frequency ---------------------------------------------;
call HelperMeasureTsc
; TODO. Check for errors, CF=1
;---------- Build bind buffer -------------------------------------------------;
mov rdi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
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
mov rax,[r14 + CPUDATA.extractedMethodsBitmap]
stosq
;---------- Exit points -------------------------------------------------------;
; TODO. Error Handling.
.error:
; TODO. Error Handling.
pop r15 r14 r13 r12 rbp rdi rsi rbx
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

