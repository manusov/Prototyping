RzDriverQueryProcNoargs:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
mov ecx,ebx
call LoadKernelModeDriver
test eax,eax
jz .error
;---------- Parameters for KMD request ----------------------------------------;
lea edx,[Query]
mov [edx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PROC_NOARGS
lea eax,[beepCallback]
mov dword [edx + RZDriverQuery.PROC + 0],eax
mov dword [edx + RZDriverQuery.PROC + 4],0
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

;---------- Kernel mode callback subroutine -----------------------------------;
beepCallback:
mov al,0B6h
out 43h,al
mov al,0058h
out 42h,al
mov al,0002h
out 42h, al
in al,61h
or al,00000011b
out 61h, al
mov ecx,1500000000
@@:
loop @b
in al,61h
and al,11111100b
out 61h,al
ret

