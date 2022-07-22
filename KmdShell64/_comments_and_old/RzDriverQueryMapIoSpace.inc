RzDriverQueryMapIoSpace:
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
mov [rdx + RZDriverIOQuery.IOCODE],RZ_DRIVER_QUERY_MAP_IO_SPACE
mov rax,[OptionStartAddress]
mov [rdx + RZDriverIOQuery.SRC],rax
lea rax,[rdi + KERNEL_BUFFER_OFFSET]
mov [rdx + RZDriverIOQuery.DST],rax
mov [rdx + RZDriverIOQuery.BYTES],rcx
mov [rdx + RZDriverIOQuery.IODATA],0   ; 0 means unmap
;---------- Driver request use file I/O ---------------------------------------;
mov rbp,rsp                   ; RBP = Storage for RSP 
and rsp,0FFFFFFFFFFFFFFF0h    ; Align stack
push 0 0
sub rsp,32
lea r9,[Bytes] 
mov r8d,sizeof.RZDriverIOQuery
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
