RzDriverQueryProcRegister2p:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
mov ecx,ebx
call LoadKernelModeDriver
test eax,eax
jz .error
;----------- Blank buffer -----------------------------------------------------;
push edi
add edi,KERNEL_BUFFER_OFFSET
mov ecx,dword [OptionStopAddress]
sub ecx,dword [OptionStartAddress]
inc ecx 
mov al,11h
cld
push ecx
rep stosb
pop ecx edi
;---------- Parameters for KMD request ----------------------------------------;
mov edx,Query
xor ebp,ebp
mov [edx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PROC_REGISTER_2P
mov eax,callbackTwoParm
mov dword [edx + RZDriverQuery.PROC + 0],eax
mov dword [edx + RZDriverQuery.PROC + 4],ebp
lea eax,[edi + KERNEL_BUFFER_OFFSET + 8]
mov dword [edx + RZDriverQuery.paramA + 0],eax
mov dword [edx + RZDriverQuery.paramA + 4],ebp
mov eax,01234567h
mov dword [edx + RZDriverQuery.paramB + 0],eax
mov dword [edx + RZDriverQuery.paramB + 4],ebp
;---------- Driver request use file I/O ---------------------------------------;
push 0
push Bytes
push sizeof.RZDriverQuery 
push edx
push [SCP_DATA.driver]
call [WriteFile]
;---------- Unload kernel mode driver kmd64.sys -------------------------------;
mov ecx,ebx
call UnloadKernelModeDriver
test eax,eax
jz .error
;---------- Memory buffer dump ------------------------------------------------;
lea esi,[edi + KERNEL_BUFFER_OFFSET]
mov ebx,dword [OptionStopAddress]
sub ebx,dword [OptionStartAddress]
inc ebx 
call DumpMemory
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret

;---------- Kernel mode callback subroutine -----------------------------------;

callbackTwoParm:
mov [eax],edx
ret

