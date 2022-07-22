ReadSpdAtSmbus:
ret


;---------- User mode caller --------------------------------------------------;

READ_SPD_CONTROL_OFFSET  EQU  0
READ_SPD_DATA_OFFSET     EQU  256
READ_SPD_BUFFER_SIZE     EQU  512

DebugReadSpdAtSmbus:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
call LoadKernelModeDriver
test eax,eax
jz .error
;----------- Blank buffer -----------------------------------------------------;
push rdi
add rdi,KERNEL_BUFFER_OFFSET + READ_SPD_CONTROL_OFFSET
mov ecx,READ_SPD_BUFFER_SIZE 
mov al,11h
cld
push rcx
rep stosb
pop rcx rdi
;---------- Parameters for KMD request ----------------------------------------;
lea rdx,[Query]
mov [rdx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PROC_REGISTER_2P
lea rax,[callbackReadSpdAtSmbus]
mov qword [rdx + RZDriverQuery.PROC],rax
lea rax,[rdi + KERNEL_BUFFER_OFFSET + READ_SPD_CONTROL_OFFSET]
mov [rdx + RZDriverQuery.paramA],rax
lea rax,[rdi + KERNEL_BUFFER_OFFSET + READ_SPD_DATA_OFFSET]
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
lea rsi,[rdi + KERNEL_BUFFER_OFFSET + READ_SPD_CONTROL_OFFSET]
mov ebx,READ_SPD_BUFFER_SIZE 
call DumpMemory
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret

;---------- Kernel mode callback ----------------------------------------------;

; Debug SPD Read.
; RAX = Pointer to control and status structure:
;       DW  SMBus base I/O
;       DB  SPD byte address
;       DB  SPD EPROM address
;       DB  Status byte
; RDX = Pointer to destination buffer for store one byte 

callbackReadSpdAtSmbus:

; DEBUG
mov dword [rax],055555555h
mov dword [rdx],0AAAAAAAAh
; DEBUG

ret

