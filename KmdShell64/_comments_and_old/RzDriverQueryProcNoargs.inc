RzDriverQueryProcNoargs:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
call LoadKernelModeDriver
test eax,eax
jz .error
;---------- Parameters for KMD request ----------------------------------------;
lea rdx,[Query]
mov [rdx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PROC_NOARGS
lea rax,[beepCallback]
mov [rdx + RZDriverQuery.PROC],rax
;---------- Driver request use file I/O ---------------------------------------;
mov rbp,rsp                   ; RBP = Storage for RSP 
and rsp,0FFFFFFFFFFFFFFF0h    ; Align stack
push 0 0
sub rsp,32
lea r9,[Bytes] 
mov r8d,sizeof.RZDriverQuery
mov rcx,[SCP_DATA.driver]
call [WriteFile]
mov rsp,rbp
;---------- Unload kernel mode driver kmd64.sys -------------------------------;
mov rcx,rsi
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

