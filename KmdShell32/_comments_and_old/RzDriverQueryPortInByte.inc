RzDriverQueryPortInByte:
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
lea edx,[Query]
mov dword [edx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PORT_IN_BYTE
mov dword [edx + RZDriverQuery.paramA],00000080h
mov dword [edx + RZDriverQuery.RESULT],0
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
lea esi,[Query.RESULT]
mov ebx,4 
call DumpMemory
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret

