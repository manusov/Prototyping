; UNDER CONSTRUCTION.
; MOVE BINARY GET ROUTINE AND TEXTS GENERATORS FOR SCREEN 0 AND DETAILS SCREEN
; TO ONE FILE PER SYSTEM INFORMATION OBJECT,
; INSTEAD ALL BINARY + ALL TEXTS.

; Use this call when start.
; Repeat this call before vector brief, memory run, memory draw.
; Move local helpers to this file or eliminate helpers, optimize structure.

InfoProcessor:

;---------- Check CPUID instruction support, get 12-chars vendor string -------;

lea rdi,[CPU_DATA.vendorString]  ; RDI = Destination pointer
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

; mov rsi,[r15 + REGISTRY64.appData.lockedDataCpuCommon]
mov rsi,[APP_DATA.lockedDataCpuCommon]

test rsi,rsi
jz @f
call HelperBuildBitmap
@@:
bts rax,63
stosq

;------- Build CPU AVX512 features bitmap, for system information screen ------;

xor eax,eax

; mov rsi,[r15 + REGISTRY64.appData.lockedDataCpuAvx512]
mov rsi,[APP_DATA.lockedDataCpuAvx512]

test rsi,rsi
jz @f
call HelperBuildBitmap
@@:
stosq

;------- Build OS context features bitmap, for system information screen ------;

xor eax,eax

; mov rsi,[r15 + REGISTRY64.appData.lockedDataOsContext]
mov rsi,[APP_DATA.lockedDataOsContext]

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

; xor eax,eax    ; *** DEBUG ***
; stosq          ; *** DEBUG ***

;---------- Measure TSC frequency ---------------------------------------------;

call HelperMeasureTsc
; TODO. Check for errors, CF=1



; RDI = Pointer for bind buffer addressing.
; R14 = Pointer for CPU data addressing in the application registry 

; mov rdi,[r15 + REGISTRY64.allocatorBindBuffer.objectStart]
lea rdi,[BIND_LIST]

lea r14,[CPU_DATA.vendorString]

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





clc
ret

.error:
stc
ret

