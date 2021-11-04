;---------- Get processor information -----------------------------------------;
; Use this call when start.                                                    ;
; Repeat this call before vector brief, memory run, memory draw.               ;
; Move local helpers to this file or eliminate helpers, optimize structure.    ;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoProcessor:
;---------- Check CPUID instruction support, get 12-chars vendor string -------;
lea edi,[CPU_DATA.vendorString]
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
mov esi,[APP_DATA.lockedDataCpuCommon]
test esi,esi
jz @f
call HelperBuildBitmap
@@:
stosd
mov eax,80000000h
stosd
;------- Build CPU AVX512 features bitmap, for system information screen ------;
xor eax,eax
mov esi,[APP_DATA.lockedDataCpuAvx512]
test esi,esi
jz @f
call HelperBuildBitmap
@@:
stosd
xor eax,eax
stosd
;------- Build OS context features bitmap, for system information screen ------;
xor eax,eax
mov esi,[APP_DATA.lockedDataOsContext]
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
; xor eax,eax    ; *** DEBUG ***
; stosd          ; *** DEBUG ***
; stosd          ; *** DEBUG ***
pop esi
;---------- Measure TSC frequency ---------------------------------------------;
push esi
call HelperMeasureTsc
pop esi
; TODO. Check for errors, CF=1
push esi
; EDI = Pointer for bind buffer addressing.
; EBP = Pointer for CPU data addressing in the application registry
lea edi,[BIND_LIST]
lea ebp,[CPU_DATA.vendorString]
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
pop esi
clc
ret
.error:
stc
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
