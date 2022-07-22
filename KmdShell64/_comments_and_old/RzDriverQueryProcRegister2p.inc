RzDriverQueryProcRegister2p:
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
mov [rdx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PROC_REGISTER_2P
lea rax,[callbackTwoParm]
mov qword [rdx + RZDriverQuery.PROC],rax
lea rax,[rdi + KERNEL_BUFFER_OFFSET + 24]
mov [rdx + RZDriverQuery.paramA],rax
mov rax,0123456789ABCDEFh
mov [rdx + RZDriverQuery.paramB],rax
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
lea rsi,[rdi + KERNEL_BUFFER_OFFSET]
mov rbx,[OptionStopAddress]
sub rbx,[OptionStartAddress]
inc rbx 
call DumpMemory
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret

;---------- Kernel mode callback subroutine -----------------------------------;

callbackTwoParm:
mov [rax],rdx
ret

