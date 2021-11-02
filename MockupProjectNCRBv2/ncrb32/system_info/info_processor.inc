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
