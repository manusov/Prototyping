;---------- Operation handler -------------------------------------------------;
; INPUT:   RSI = Pointer to HardwareShell status variables block               ;
;          RDI = Pointer to HardwareShell temporary buffer                     ;
;------------------------------------------------------------------------------;
ReadMemory:
;---------- Parameters for KMD request ----------------------------------------;
lea rdx,[Query]
mov [rdx + RZDriverIOQuery.IOCODE],RZ_DRIVER_QUERY_MAP_IO_SPACE
mov rax,[OptionStartAddress]
mov [rdx + RZDriverIOQuery.SRC],rax
lea rax,[rdi + KERNEL_BUFFER_OFFSET]
mov [rdx + RZDriverIOQuery.DST],rax
mov rax,[OptionStopAddress]
sub rax,[OptionStartAddress]
inc eax 
mov [rdx + RZDriverIOQuery.BYTES],rax
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
;---------- Memory buffer dump ------------------------------------------------;
lea rsi,[rdi + KERNEL_BUFFER_OFFSET]
mov rax,[OptionStartAddress]
mov rbx,[OptionStopAddress]
sub rbx,rax
inc ebx
call DumpMemoryAbsolute
ret
