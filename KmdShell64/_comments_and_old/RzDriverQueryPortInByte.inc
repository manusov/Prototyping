RzDriverQueryPortInByte:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
call LoadKernelModeDriver
test eax,eax
jz .error
;----------- Blank buffer -----------------------------------------------------;
push rdi
add rdi,KERNEL_BUFFER_OFFSET
mov rcx,[OptionStopAddress]
sub rcx,[OptionStartAddress]
inc ecx 
mov al,11h
cld
push rcx
rep stosb
pop rcx rdi
;---------- Parameters for KMD request ----------------------------------------;
lea rdx,[Query]
mov [rdx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PORT_IN_BYTE
mov [rdx + RZDriverQuery.paramA],00000080h
mov [rdx + RZDriverQuery.RESULT],0
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
;---------- Memory buffer dump ------------------------------------------------;
lea rsi,[Query.RESULT]
mov ebx,8 
call DumpMemory
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret
