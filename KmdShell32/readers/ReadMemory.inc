;---------- Operation handler -------------------------------------------------;
; INPUT:   EBX = Pointer to HardwareShell status variables block               ;
;          EDI = Pointer to HardwareShell temporary buffer                     ;
;------------------------------------------------------------------------------;
ReadMemory:
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
mov eax,dword [OptionStopAddress]
sub eax,dword [OptionStartAddress]
inc eax 
mov dword [edx + RZDriverIOQuery.BYTES + 0],eax
mov dword [edx + RZDriverIOQuery.BYTES + 4],ebp
mov [edx + RZDriverIOQuery.IODATA],ebp            ; 0 means unmap
;---------- Driver request use file I/O ---------------------------------------;
push 0
push Bytes
push sizeof.RZDriverIOQuery 
push edx
push [SCP_DATA.driver]
call [WriteFile]
;---------- Memory buffer dump ------------------------------------------------;
lea esi,[edi + KERNEL_BUFFER_OFFSET]
mov eax,dword [OptionStartAddress]
mov ebx,dword [OptionStopAddress]
sub ebx,eax
inc ebx
call DumpMemoryAbsolute
ret
