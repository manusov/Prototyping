RzDriverQueryMapIoSpace:
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
mov [edx + RZDriverIOQuery.IOCODE],RZ_DRIVER_QUERY_MAP_IO_SPACE
mov eax,dword [OptionStartAddress]
mov dword [edx + RZDriverIOQuery.SRC + 0],eax
mov dword [edx + RZDriverIOQuery.SRC + 4],ebp
lea eax,[edi + KERNEL_BUFFER_OFFSET]
mov dword [edx + RZDriverIOQuery.DST + 0],eax
mov dword [edx + RZDriverIOQuery.DST + 4],ebp
mov dword [edx + RZDriverIOQuery.BYTES + 0],ecx
mov dword [edx + RZDriverIOQuery.BYTES + 4],ebp
mov [edx + RZDriverIOQuery.IODATA],ebp            ; 0 means unmap
;---------- Driver request use file I/O ---------------------------------------;
push 0
push Bytes
push sizeof.RZDriverIOQuery 
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

