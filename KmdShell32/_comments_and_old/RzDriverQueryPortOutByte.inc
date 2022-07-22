RzDriverQueryPortOutByte:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
mov ecx,ebx
call LoadKernelModeDriver
test eax,eax
jz .error
;---------- Parameters for KMD request ----------------------------------------;
lea edx,[Query]
mov dword [edx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PORT_OUT_BYTE
mov dword [edx + RZDriverQuery.paramA],00000080h
mov dword [edx + RZDriverQuery.paramB],00000043h
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
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret

