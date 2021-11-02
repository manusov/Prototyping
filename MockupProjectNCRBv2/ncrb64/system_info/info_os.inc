;---------- Get Operating System information ----------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoOperatingSystem:
sub rsp,32 + 8

lea rsi,[DYNA_IMPORT]
lea rdi,[OS_DATA]
mov rax,[rsi + DYNAIMPORT._GlobalMemoryStatusEx]
test rax,rax
jz .error
lea rcx,[rdi + OSDATA.memoryStatusEx]
mov [rcx + MEMORYSTATUSEX_DEF.dwLength],sizeof.MEMORYSTATUSEX_DEF
call rax 
test rax,rax
jz .error
lea rcx,[rdi + OSDATA.systemInfo]
call [GetSystemInfo]
mov rax,[rsi + DYNAIMPORT._GetActiveProcessorGroupCount]
test rax,rax
jz @f
call rax
movzx eax,ax
@@:
mov [rdi + OSDATA.activeProcessorGroupCount],eax
mov rax,[rsi + DYNAIMPORT._GetActiveProcessorCount]
test rax,rax
jz @f
mov ecx,ALL_PROCESSOR_GROUPS
call rax
@@:
mov [rdi + OSDATA.activeProcessorCount],eax
mov rax,[rsi + DYNAIMPORT._GetNumaHighestNodeNumber]
test rax,rax
jz @f
lea rbx,[rdi + OSDATA.numaNodeCount]
mov rcx,rbx
call rax
test rax,rax
jz @f
mov eax,[rbx]
inc eax
@@:               ; If no data, EAX=0 at this point, means no nodes found
mov [rbx],eax
;---------- Get large page size -----------------------------------------------;
mov rax,[rsi + DYNAIMPORT._GetLargePageMinimum]
test rax,rax
jz @f
call rax
@@:
mov [rdi + OSDATA.largePageSize],rax
;---------- Get large page information: try allocation ------------------------;
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,128 + 32
xor eax,eax
mov [rdi + OSDATA.largePageEnable],eax
cmp [rsi + DYNAIMPORT._OpenProcessToken],rax
je .exit
cmp [rsi + DYNAIMPORT._AdjustTokenPrivileges],rax 
je .exit
cmp [rdi + OSDATA.largePageSize],rax 
je .exit
call [GetCurrentProcess]
test rax,rax
jz .skip
mov rcx,rax
mov edx,MAXIMUM_ALLOWED 
lea r8,[rsp + 120 - 32]
call [rsi + DYNAIMPORT._OpenProcessToken] 
test rax,rax 
jz .skip   
mov rcx,[rsp + 120 - 32]  
xor edx,edx
lea r8,[rsp + 48]
xor r9d,r9d
mov [rsp + 32],r9
mov [rsp + 40],r9
mov [tp.PrivilegeCount],1 
mov [tp.Privileges.Luid.usedpart],SE_LOCK_MEMORY_PRIVILEGE 
and [tp.Privileges.Luid.ignorehigh32bitpart],0  
mov [tp.Privileges.Attributes],SE_PRIVILEGE_ENABLED
call [rsi + DYNAIMPORT._AdjustTokenPrivileges] 
mov rbx,rax 
mov rcx,[rsp + 120 - 32] 
call [CloseHandle] 
.skip:
test rax,rax
jz .exit
xor eax,eax
test rbx,rbx
jz .exit 
call [GetCurrentProcess]
test rax,rax
jz .exit
mov rbx,rax
mov rcx,rax 
xor edx,edx
mov r8,[rdi + OSDATA.largePageSize]
mov r9d,MEM_COMMIT + MEM_LARGE_PAGES
push 0
push PAGE_READWRITE
sub rsp,32
call [VirtualAllocEx]
add rsp,32 + 16
test rax,rax
jz @f
mov rcx,rbx
mov rdx,rax
xor r8d,r8d
mov r9d,MEM_RELEASE
call [VirtualFreeEx]
@@:
test rax,rax
setnz al
movzx eax,al
mov [rdi + OSDATA.largePageEnable],eax 
.exit:
mov rsp,rbp


add rsp,32 + 8
ret


.error:
add rsp,32 + 8
stc
ret


;--- Build Operating System information for text report and GUI widgets -------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextOperatingSystem:
mov r14,rdi
mov [DYNA_PTR.textOs.objectStart],rbp
mov ax,STR_OS_MEMORY_LOAD                        ; Memory load percentage
call HelperOsInfoString
mov eax,[r14 + OSDATA.memoryStatusEx.dwMemoryLoad]
push rax
mov bl,0
call DecimalPrint32
mov ax,' %'
stosw
pop rax
mov rdi,r8
call HexPrint32 
mov ax,STR_OS_TOTAL_PHYSICAL                     ; Total physical memory
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullTotalPhys
call HelperOsInfoSize
mov ax,STR_OS_AVAIL_PHYSICAL                     ; Available physical memory
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullAvailPhys
call HelperOsInfoSize
mov ax,STR_OS_TOTAL_PAGE_FILE                    ; Total page file 
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullTotalPageFile
call HelperOsInfoSize
mov ax,STR_OS_AVAIL_PAGE_FILE                    ; Available page file
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullAvailPageFile
call HelperOsInfoSize
mov ax,STR_OS_TOTAL_VIRTUAL                      ; Total virtual user space
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullTotalVirtual
call HelperOsInfoSize
mov ax,STR_OS_AVAIL_VIRTUAL                      ; Available virtual user space
call HelperOsInfoString
mov al,MEMORYSTATUSEX_DEF.ullAvailVirtual
call HelperOsInfoSize
mov ax,STR_OS_EXT_VIRTUAL                        ; Extended virtual
call HelperOsInfoString
mov al,'-'
stosb
mov al,MEMORYSTATUSEX_DEF.ullAvailExtendedVirtual
call HelperOsInfoHex
mov rdi,rbp
mov ax,0A0Dh                                     ; Interval
stosw
mov rbp,rdi 
mov bh,0                                         ; BH = 0 means system info
call HelperSystemInfo
mov rdi,rbp
mov ax,0A0Dh                                     ; Interval
stosw
mov rbp,rdi 
mov ax,STR_OS_PROC_CURRENT                       ; Processors at currebt group
call HelperOsInfoString
mov al,SYSTEM_INFO.dwNumberOfProcessors
call HelperOsInfoNumber32
mov ecx,[OS_DATA.activeProcessorCount] ; [r15 + REGISTRY64.osData.activeProcessorCount]      ; Processors total
jecxz @f
push rcx
mov ax,STR_OS_PROC_TOTAL
call HelperOsInfoString
pop rax
call HelperOsInfoValue32
@@:
mov ecx,[OS_DATA.activeProcessorGroupCount]   ; [r15 + REGISTRY64.osData.activeProcessorGroupCount] ; Processors groups
jecxz @f
push rcx
mov ax,STR_OS_PROC_GROUPS
call HelperOsInfoString
pop rax
call HelperOsInfoValue32
@@:
mov ecx,[OS_DATA.numaNodeCount] ; [r15 + REGISTRY64.osData.numaNodeCount]  ; NUMA domains
jecxz @f
push rcx
mov ax,STR_OS_NUMA_DOMAINS
call HelperOsInfoString
pop rax
call HelperOsInfoValue32
@@:
mov ax,STR_OS_NORMAL_PAGE                        ; Normal page size
call HelperOsInfoString
mov al,SYSTEM_INFO.dwPageSize
call HelperOsInfoSizeAuto32
mov rcx,[OS_DATA.largePageSize] ; [r15 + REGISTRY64.osData.largePageSize]  ; Large page size and status
jrcxz .skipLargePages
push rcx
mov ax,STR_OS_LARGE_PAGE
call HelperOsInfoString
pop rax
push rax
mov bl,0FFh
call SizePrint64
cmp [OS_DATA.largePageEnable],0 ; [r15 + REGISTRY64.osData.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
call PoolStringWrite
pop rax
mov rdi,r8
call HexPrint64
.skipLargePages:
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textOs.objectStop],rbp
;---------- Additions for screen 0 information --------------------------------;
lea rdx,[BIND_LIST]
add rdx,BINDLIST.bindSys
lea rdi,[rdx + BINDSYS.procTotal]
mov eax,[OS_DATA.activeProcessorCount]      ; Processors total
mov bl,0
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.groups]
mov eax,[OS_DATA.activeProcessorGroupCount] ; Processors groups
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.procCur]
mov eax,[OS_DATA.systemInfo.dwNumberOfProcessors]
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.numaNodes]
mov eax,[OS_DATA.numaNodeCount]  ; NUMA domains
call DecimalPrint32
mov al,0
stosb
lea rdi,[rdx + BINDSYS.memPhys]
mov rax,[OS_DATA.memoryStatusEx.ullTotalPhys]
mov bl,2
call SizePrint64
mov al,0
stosb
lea rdi,[rdx + BINDSYS.memAvail]
mov rax,[OS_DATA.memoryStatusEx.ullAvailPhys]
call SizePrint64
mov al,0
stosb
lea rdi,[rdx + BINDSYS.largePage]
mov rax,[OS_DATA.largePageSize]
mov bl,0FFh
call SizePrint64
mov al,0
stosb
lea rdi,[rdx + BINDSYS.largeEnable]
cmp [OS_DATA.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
call PoolStringWrite
mov al,0
stosb
ret

