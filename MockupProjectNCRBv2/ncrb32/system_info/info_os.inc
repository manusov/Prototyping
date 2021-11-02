;---------- Get Operating System information ----------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoOperatingSystem:
lea ebx,[DYNA_IMPORT]
lea edi,[OS_DATA]
mov eax,[ebx + DYNAIMPORT._GlobalMemoryStatusEx]
test eax,eax
jz .error
lea ecx,[OS_DATA.memoryStatusEx]
mov [ecx + MEMORYSTATUSEX_DEF.dwLength],sizeof.MEMORYSTATUSEX_DEF
push ecx
call eax 
test eax,eax
jz .error
lea ecx,[OS_DATA.systemInfo]
push ecx
call [GetSystemInfo]
mov eax,[ebx + DYNAIMPORT._GetActiveProcessorGroupCount]
test eax,eax
jz @f
call eax
movzx eax,ax
@@:
mov [OS_DATA.activeProcessorGroupCount],eax
mov eax,[ebx + DYNAIMPORT._GetActiveProcessorCount]
test eax,eax
jz @f
push ALL_PROCESSOR_GROUPS
call eax
@@:
mov [OS_DATA.activeProcessorCount],eax
mov eax,[ebx + DYNAIMPORT._GetNumaHighestNodeNumber]
test eax,eax
jz @f
lea ecx,[OS_DATA.numaNodeCount]
push ecx
call eax
test eax,eax
jz @f
mov eax,[OS_DATA.numaNodeCount]
inc eax
@@:               ; If no data, EAX=0 at this point, means no nodes found
mov [OS_DATA.numaNodeCount],eax
;---------- Get large page size -----------------------------------------------;
mov eax,[ebx + DYNAIMPORT._GetLargePageMinimum]
test eax,eax
jz @f
call eax
@@:
mov [OS_DATA.largePageSize],eax
;---------- Get large page information: try allocation ------------------------;
push esi
sub esp,128
xor eax,eax
mov [OS_DATA.largePageEnable],eax
cmp [ebx + DYNAIMPORT._OpenProcessToken],eax
je .exit
cmp [ebx + DYNAIMPORT._AdjustTokenPrivileges],eax 
je .exit
cmp [OS_DATA.largePageSize],eax 
je .exit
call [GetCurrentProcess]
test eax,eax
jz .skip
lea ebp,[esp + 120]
push ebp 
push MAXIMUM_ALLOWED
push eax
call [ebx + DYNAIMPORT._OpenProcessToken]
test eax,eax 
jz .skip   
xor eax,eax  
mov esi,esp
push eax
push eax
push eax
push esi 
push eax
push dword [ebp]
mov [tp.PrivilegeCount],1 
mov [tp.Privileges.Luid.usedpart],SE_LOCK_MEMORY_PRIVILEGE 
and [tp.Privileges.Luid.ignorehigh32bitpart],0  
mov [tp.Privileges.Attributes],SE_PRIVILEGE_ENABLED
call [ebx + DYNAIMPORT._AdjustTokenPrivileges] 
mov esi,eax 
push dword [ebp] 
call [CloseHandle] 
.skip:
test eax,eax
jz .exit
xor eax,eax
test esi,esi
jz .exit 
call [GetCurrentProcess]
test eax,eax
jz .exit
mov esi,eax
push PAGE_READWRITE
push MEM_COMMIT + MEM_LARGE_PAGES
push [OS_DATA.largePageSize]
push 0
push esi
call [VirtualAllocEx]
test eax,eax
jz @f
push MEM_RELEASE
push 0
push eax
push esi
call [VirtualFreeEx]
@@:
test eax,eax
setnz al
movzx eax,al
mov [OS_DATA.largePageEnable],eax 
.exit:
add esp,128
pop esi
ret
.error:
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
mov [DYNA_PTR.textOs.objectStart],ebp
mov ax,STR_OS_MEMORY_LOAD
call HelperOsInfoString                          ; Memory load percentage
mov eax,[OS_DATA.memoryStatusEx.dwMemoryLoad]
push eax
mov bl,0
call DecimalPrint32
mov ax,' %'
stosw
pop eax
mov edi,ecx
call HexPrint32 
mov ax,STR_OS_TOTAL_PHYSICAL
call HelperOsInfoString                          ; Total physical memory
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
mov edi,ebp
mov ax,0A0Dh                                     ; Interval
stosw
mov ebp,edi 
mov bh,0                                         ; BH = 0 means system info
call HelperSystemInfo
mov edi,ebp
mov ax,0A0Dh                                     ; Interval
stosw
mov ebp,edi 
mov ax,STR_OS_PROC_CURRENT                       ; Processors at currebt group
call HelperOsInfoString
mov al,SYSTEM_INFO.dwNumberOfProcessors
call HelperOsInfoNumber32
mov ecx,[OS_DATA.activeProcessorCount]      ; Processors total
jecxz @f
push ecx
mov ax,STR_OS_PROC_TOTAL
call HelperOsInfoString
pop eax
call HelperOsInfoValue32
@@:
mov ecx,[OS_DATA.activeProcessorGroupCount] ; Processors groups
jecxz @f
push ecx
mov ax,STR_OS_PROC_GROUPS
call HelperOsInfoString
pop eax
call HelperOsInfoValue32
@@:
mov ecx,[OS_DATA.numaNodeCount]  ; NUMA domains
jecxz @f
push ecx
mov ax,STR_OS_NUMA_DOMAINS
call HelperOsInfoString
pop eax
call HelperOsInfoValue32
@@:
mov ax,STR_OS_NORMAL_PAGE                        ; Normal page size
call HelperOsInfoString
mov al,SYSTEM_INFO.dwPageSize
call HelperOsInfoSizeAuto32
mov ecx,[OS_DATA.largePageSize]  ; Large page size and status
jecxz .skipLargePages
push ecx
mov ax,STR_OS_LARGE_PAGE
call HelperOsInfoString
pop eax
push eax ecx esi
mov bl,0FFh
xor edx,edx
call SizePrint64
cmp [OS_DATA.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
call PoolStringWrite
pop esi edi eax
xor edx,edx
call HexPrint64
.skipLargePages:
mov al,0
stosb
mov ebp,edi
mov [DYNA_PTR.textOs.objectStop],ebp
;---------- Additions for screen 0 information --------------------------------;
lea ecx,[BIND_LIST]
add ecx,BINDLIST.bindSys
lea edi,[ecx + BINDSYS.procTotal]
mov eax,[OS_DATA.activeProcessorCount]      ; Processors total
mov bl,0
call DecimalPrint32
mov al,0
stosb
lea edi,[ecx + BINDSYS.groups]
mov eax,[OS_DATA.activeProcessorGroupCount] ; Processors groups
call DecimalPrint32
mov al,0
stosb
lea edi,[ecx + BINDSYS.procCur]
mov eax,[OS_DATA.systemInfo.dwNumberOfProcessors]
call DecimalPrint32
mov al,0
stosb
lea edi,[ecx + BINDSYS.numaNodes]
mov eax,[OS_DATA.numaNodeCount]  ; NUMA domains
call DecimalPrint32
mov al,0
stosb
lea edi,[ecx + BINDSYS.memPhys]
mov eax,dword [OS_DATA.memoryStatusEx.ullTotalPhys + 0]
mov edx,dword [OS_DATA.memoryStatusEx.ullTotalPhys + 4]
mov bl,2
call SizePrint64
mov al,0
stosb
lea edi,[ecx + BINDSYS.memAvail]
mov eax,dword [OS_DATA.memoryStatusEx.ullAvailPhys + 0]
mov edx,dword [OS_DATA.memoryStatusEx.ullAvailPhys + 4]
call SizePrint64
mov al,0
stosb
lea edi,[ecx + BINDSYS.largePage]
mov eax,[OS_DATA.largePageSize]
xor edx,edx
mov bl,0FFh
call SizePrint64
mov al,0
stosb
lea edi,[ecx + BINDSYS.largeEnable]
cmp [OS_DATA.largePageEnable],0
mov ax,STR_OS_DISABLED
je @f
mov ax,STR_OS_ENABLED
@@:
push esi
call PoolStringWrite
pop esi
mov al,0
stosb
ret

