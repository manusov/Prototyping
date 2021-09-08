; TODO.
; 1) Error handling.
; 2) Dynamical import.
; 3) Use GlobalMemoryStatus if GlobalMemoryStatusEx not supported.
; 4) Synchronize with sub-set of NCRB functionality.
; 5) Remove numbers usage, for example "82 + 50".
; 6) Optimize for resources DLL, common for NCRB32/64,
;    make some normal strings as resource raw objects.
; 7) Optimize, remove not required PUSH/POP RAX before/after SizePrint64
;    and other subroutines, DecimalPrint32.

include 'win64a.inc'
ID_MAIN                = 100
ID_EXE_ICON            = 101
ID_EXE_ICONS           = 102
ID_GUI_STRINGS         = 103
ID_GUI_BINDERS         = 104
IDR_TEXT_UP            = 200
IDR_TEXT_MAIN          = 201
IDR_REPORT             = 204
IDR_BINARY             = 205
IDR_CANCEL             = 206
STRING_APP             = 0
STRING_FONT            = 1
STRING_REPORT          = 2
STRING_BINARY          = 3
STRING_CANCEL          = 4
STRING_TEXT_UP         = 5
STRING_MEMORY_LOAD     = 6
STRING_TOTAL_PHYSICAL  = 7
STRING_AVAIL_PHYSICAL  = 8
STRING_TOTAL_PAGE_FILE = 9 
STRING_AVAIL_PAGE_FILE = 10
STRING_TOTAL_VIRTUAL   = 11
STRING_AVAIL_VIRTUAL   = 12
STRING_EXT_VIRTUAL     = 13
STRING_MIN_ADDRESS     = 14
STRING_MAX_ADDRESS     = 15
STRING_PROC_MASK       = 16
STRING_PROC_TYPE       = 17
STRING_ALLOC_GRAN      = 18
STRING_PROC_LEVEL      = 19
STRING_PROC_REVISION   = 20
STRING_PROC_CURRENT    = 21
STRING_PROC_TOTAL      = 22
STRING_PROC_GROUPS     = 23
STRING_NUMA_DOMAINS    = 24
STRING_NORMAL_PAGE     = 25
STRING_LARGE_PAGE      = 26
STRING_DISABLED        = 27
STRING_ENABLED         = 28
BUFFER_SYSTEM_INFO     = 0
BIND_APP_GUI           = 0
X_SIZE                 = 360
Y_SIZE                 = 240
INFO_BUFFER_SIZE       = 8192
MISC_BUFFER_SIZE       = 8192

macro BIND_STOP
{ DB  0 }
MACRO BIND_STRING srcid, dstid
{ DD 01h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_INFO srcid, dstid
{ DD 02h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_BIG srcid, dstid
{ DD 03h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_BOOL srcid, srcbit, dstid
{ DD 04h + (srcid) SHL 9 + (srcbit) SHL 6 + (dstid) SHL 19 }
COMBO_STOP_ON  = 0
COMBO_STOP_OFF = 1
COMBO_CURRENT  = 2
COMBO_ADD      = 3
COMBO_INACTIVE = 4
MACRO BIND_COMBO srcid, dstid
{ DD 05h + (srcid) SHL 6 + (dstid) SHL 19 }

CLEARTYPE_QUALITY      = 5
ALL_PROCESSOR_GROUPS   = 0000FFFFh 
struct MEMORYSTATUSEX_DEF
dwLength                 dd ?
dwMemoryLoad             dd ?
ullTotalPhys             dq ?
ullAvailPhys             dq ?
ullTotalPageFile         dq ?
ullAvailPageFile         dq ?
ullTotalVirtual          dq ?
ullAvailVirtual          dq ?
ullAvailExtendedVirtual  dq ?
ends

MEM_LARGE_PAGES          =  020000000h 
SE_PRIVILEGE_ENABLED     = 2 
SE_LOCK_MEMORY_PRIVILEGE = 4
struct LUID 
usedpart             dd  ?   
ignorehigh32bitpart  dd  ? 
ends 
struct LUID_AND_ATTRIBUTES 
Luid        LUID 
Attributes  dd  ?  
ends 
struct TOKEN_PRIVILEGES 
PrivilegeCount  dd  ? 
Privileges      LUID_AND_ATTRIBUTES 
ends 
virtual at r8 
tp  TOKEN_PRIVILEGES 
end virtual

format PE64 GUI 5.0
entry start
section '.code' code readable executable
start:

sub rsp,8*5
cld
lea rcx,[AppControl]
call [InitCommonControlsEx]
test rax,rax
jz .guiFailed
xor ecx,ecx
call [GetModuleHandle]
test rax,rax
jz .guiFailed
mov [HandleThis],rax
mov edx,ID_EXE_ICONS
xchg rcx,rax 
call [LoadIcon]
test rax,rax
jz .guiFailed
mov [HandleIcon],rax
mov r8d,RT_RCDATA
mov edx,ID_GUI_STRINGS
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .guiFailed 
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .guiFailed
xchg rcx,rax
call [LockResource]  
test rax,rax
jz .guiFailed
mov [LockedStrings],rax
mov r8d,RT_RCDATA
mov edx,ID_GUI_BINDERS
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .guiFailed
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .guiFailed
xchg rcx,rax
call [LockResource]  
test rax,rax
jz .guiFailed
mov [LockedBinders],rax

lea rcx,[NameKernelDll]
call [GetModuleHandle]
test rax,rax
jz .libraryNotFound
mov [HandleDll],rax 
lea rdx,[NameFunctionMemory]
mov rcx,[HandleDll]
mov rbx,rcx
call [GetProcAddress]
test rax,rax
jz .functionNotFound
mov [_GlobalMemoryStatusEx],rax
lea rdx,[NameFunctionProcessors] 
mov rcx,rbx
call [GetProcAddress]
mov [_GetActiveProcessorGroupCount],rax
lea rdx,[NameFunctionGroups]
mov rcx,rbx
call [GetProcAddress]
mov [_GetActiveProcessorCount],rax
lea rdx,[NameFunctionNuma]
mov rcx,rbx
call [GetProcAddress]
mov [_GetNumaHighestNodeNumber],rax
lea rdx,[NameFunctionLargePages]
mov rcx,rbx
call [GetProcAddress]
mov [_GetLargePageMinimum],rax
lea rbx,[_OpenProcessToken]
xor eax,eax
mov [rbx + 00],rax
mov [rbx + 08],rax
lea rcx,[NameAdvapiDll]
call [LoadLibrary]
test rax,rax
jz @f
xchg rbp,rax
lea rdx,[NameFunctionOpen]
mov rcx,rbp
call [GetProcAddress]               
mov [rbx + 00],rax
lea rdx,[NameFunctionAdjust]
mov rcx,rbp
call [GetProcAddress]
mov [rbx + 08],rax
@@:

lea rcx,[MemoryStatusEx]
mov r12,rcx
mov [r12 + MEMORYSTATUSEX_DEF.dwLength],sizeof.MEMORYSTATUSEX_DEF 
call [_GlobalMemoryStatusEx]
test rax,rax
jz .statusFailed
lea rcx,[SystemInfo]
mov r13,rcx
call [GetSystemInfo]
mov rax,[_GetActiveProcessorGroupCount]
test rax,rax
jz @f
call rax
movzx eax,ax
@@:
mov [ActiveProcessorGroupCount],eax
mov rax,[_GetActiveProcessorCount]
test rax,rax
jz @f
mov ecx,ALL_PROCESSOR_GROUPS
call rax
@@:
mov [ActiveProcessorCount],eax
lea rbx,[NumaNodesCount]
mov rax,[_GetNumaHighestNodeNumber]
test rax,rax
jz @f
mov rcx,rbx
call rax
test rax,rax
jz @f
mov eax,[rbx]
inc eax
@@:
mov [rbx],eax
mov rax,[_GetLargePageMinimum]
test rax,rax
jz @f
call rax
@@:
mov [LargePageSize],rax

push rbx rsi rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,128 + 32
mov [LargePageEnable],0
xor eax,eax
cmp [_OpenProcessToken],rax
je .exit
cmp [_AdjustTokenPrivileges],rax 
je .exit
cmp [LargePageSize],rax 
je .exit
call [GetCurrentProcess]
test rax,rax
jz .skip
mov rcx,rax
mov edx,MAXIMUM_ALLOWED 
lea r8,[rsp + 120 - 32]
call [_OpenProcessToken] 
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
call [_AdjustTokenPrivileges] 
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
mov r8,[LargePageSize]
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
mov [LargePageEnable],eax 
.exit:
mov rsp,rbp
pop rbp rsi rbx

lea rbp,[INFO_BUFFER]
mov al,STRING_MEMORY_LOAD
call StringHelper
mov eax,[r12 + MEMORYSTATUSEX_DEF.dwMemoryLoad]
push rax
mov bl,0
call DecimalPrint32
mov ax,' %'
stosw
pop rax
lea rdi,[rbp - 82 + 62]
call HexPrint32 
mov al,STRING_TOTAL_PHYSICAL
call StringHelper
mov al,MEMORYSTATUSEX_DEF.ullTotalPhys
call SizeHelper64
mov al,STRING_AVAIL_PHYSICAL
call StringHelper
mov al,MEMORYSTATUSEX_DEF.ullAvailPhys
call SizeHelper64
mov al,STRING_TOTAL_PAGE_FILE 
call StringHelper
mov al,MEMORYSTATUSEX_DEF.ullTotalPageFile
call SizeHelper64
mov al,STRING_AVAIL_PAGE_FILE
call StringHelper
mov al,MEMORYSTATUSEX_DEF.ullAvailPageFile
call SizeHelper64
mov al,STRING_TOTAL_VIRTUAL
call StringHelper
mov al,MEMORYSTATUSEX_DEF.ullTotalVirtual
call SizeHelper64
mov al,STRING_AVAIL_VIRTUAL
call StringHelper
mov al,MEMORYSTATUSEX_DEF.ullAvailVirtual
call SizeHelper64
mov al,STRING_EXT_VIRTUAL
call StringHelper
mov al,'-'
stosb
mov rax,[r12 + MEMORYSTATUSEX_DEF.ullAvailExtendedVirtual]
lea rdi,[rbp - 82 + 62]
call HexPrint64 
mov rdi,rbp
mov ax,0A0Dh
stosw
mov rbp,rdi 

mov r12,r13
mov al,STRING_MIN_ADDRESS
call StringHelper
mov al,SYSTEM_INFO.lpMinimumApplicationAddress
call SizeHelperAuto64
mov al,STRING_MAX_ADDRESS
call StringHelper
mov al,SYSTEM_INFO.lpMaximumApplicationAddress
call SizeHelperAuto64
mov al,STRING_PROC_MASK
call StringHelper
mov al,'-'
stosb
mov rax,[r12 + SYSTEM_INFO.dwActiveProcessorMask]
lea rdi,[rbp - 82 + 62]
call HexPrint64 
mov al,STRING_PROC_TYPE
call StringHelper
mov al,SYSTEM_INFO.dwProcessorType
call NumberHelper32
mov al,STRING_ALLOC_GRAN
call StringHelper
mov al,SYSTEM_INFO.dwAllocationGranularity
call SizeHelper32
mov al,STRING_PROC_LEVEL
call StringHelper
mov al,SYSTEM_INFO.wProcessorLevel
call NumberHelper16
mov al,STRING_PROC_REVISION
call StringHelper
mov al,SYSTEM_INFO.wProcessorRevision
call NumberHelper16
mov rdi,rbp
mov ax,0A0Dh
stosw
mov rbp,rdi 

mov al,STRING_PROC_CURRENT
call StringHelper
mov al,SYSTEM_INFO.dwNumberOfProcessors
call NumberHelper32
lea r12,[ActiveProcessorGroupCount]
cmp dword [r12 + 04],0
je @f
mov al,STRING_PROC_TOTAL
call StringHelper
mov al,4
call NumberHelper32
@@:
cmp dword [r12 + 00],0
je @f
mov al,STRING_PROC_GROUPS
call StringHelper
mov al,0
call NumberHelper32
@@:
cmp dword [r12 + 08],0
je @f
mov al,STRING_NUMA_DOMAINS
call StringHelper
mov al,8
call NumberHelper32
@@:
mov r12,r13
mov al,STRING_NORMAL_PAGE
call StringHelper
mov al,SYSTEM_INFO.dwPageSize
call SizeHelper32
lea r12,[LargePageSize]
cmp qword [r12 + 0],0
je @f
mov al,STRING_LARGE_PAGE
call StringHelper
mov rax,[r12 + 00]
push rax
mov bl,0FFh
call SizePrint64
cmp dword [r12 + 08],1
sete al
add al,STRING_DISABLED
mov ah,0
call IndexString
call StringWrite
pop rax
lea rdi,[rbp - 82 + 62]
call HexPrint64
@@:
mov al,0
stosb

push 0 0 
lea r9,[DialogProc]
mov r8d,HWND_DESKTOP
mov edx,ID_MAIN
mov rcx,[HandleThis]  
sub rsp,32
call [DialogBoxParam] 
add rsp,32+16
test rax,rax
jz .guiFailed 
cmp rax,-1
je .guiFailed
.ok:
xor ecx,ecx           
call [ExitProcess]
.guiFailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgGuiFailed]
xor ecx,ecx
call [MessageBox]  
mov ecx,1           
call [ExitProcess]
.libraryNotFound:
lea rbx,[MsgLibraryNotFound]
jmp .failed
.functionNotFound:
lea rbx,[MsgFunctionNotFound]
jmp .failed
.statusFailed:
lea rbx,[MsgFunctionError]
.failed:
mov r9d,MB_ICONERROR
xor r8d,r8d
mov rdx,rbx
xor ecx,ecx
call [MessageBox]  
mov ecx,2           
call [ExitProcess]

DialogProc:
push rbp rbx rsi rdi
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov [rbp + 40 + 00],rcx 
mov [rbp + 40 + 08],rdx
mov [rbp + 40 + 16],r8
mov [rbp + 40 + 24],r9
xchg rax,rdx
cmp rax,WM_INITDIALOG
je .wminitdialog 
cmp rax,WM_COMMAND
je .wmcommand
cmp rax,WM_CLOSE
je .wmclose
xor eax,eax
jmp .finish
.wminitdialog:
mov rbx,[rbp + 40 + 00]
mov ax,BIND_APP_GUI
call Binder
mov r9,[HandleIcon] 
mov r8d,ICON_SMALL 
mov edx,WM_SETICON 
mov rcx,[rbp + 40 + 00]
call [SendMessage]
mov ax,STRING_APP
call IndexString
mov rdx,rsi
mov rcx,[rbp + 40 + 00]
call [SetWindowText]
mov ax,STRING_FONT
call IndexString
xor eax,eax
push rsi
push FIXED_PITCH
push CLEARTYPE_QUALITY
push CLIP_DEFAULT_PRECIS
push OUT_TT_ONLY_PRECIS
push DEFAULT_CHARSET
push rax
push rax
push rax
push FW_DONTCARE
xor r9d,r9d
xor r8d,r8d
xor edx,edx
mov ecx,17
sub rsp,32
call [CreateFont]
add rsp,32+80
mov [HandleFont],rax
mov edx,IDR_TEXT_UP
call FontHelper
mov edx,IDR_TEXT_MAIN
call FontHelper
jmp .processed
.wmcommand:
mov eax,[rbp + 40 + 16]
cmp eax,IDR_CANCEL
je .wmclose
jmp .processed
.wmclose:
mov rcx,[HandleFont]
jrcxz @f
call [DeleteObject]
@@:
mov edx,1
mov rcx,[rbp + 40 + 00]
call [EndDialog]
.processed:
mov eax,1
.finish:
mov rsp,rbp
pop rdi rsi rbx rbp
ret

FontHelper:
mov rcx,[rbp + 40 + 00]
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
call [GetDlgItem]
test rax,rax
jz @f
mov r9d,1
mov r8,[HandleFont]
mov edx,WM_SETFONT
xchg rcx,rax
call [SendMessage]
@@:
mov rsp,rbp
pop rbp
ret

StringWriteSelected:
test al,al
jz StringWrite
cmp al,ah
ja StringWrite  
mov ah,al
cld
@@:
lodsb
cmp al,0
jne @b
dec ah
jnz @b
StringWrite:
cld
@@:
lodsb
cmp al,0
je @f
stosb
jmp @b
@@:
ret

IndexString:
cld
mov rsi,[LockedStrings]
movzx rcx,ax
jrcxz .stop
.cycle:
lodsb
cmp al,0
jne .cycle
loop .cycle
.stop:
ret

Binder:
cld
mov rsi,[LockedBinders]
movzx rcx,ax
jrcxz .foundBinder
.findBinder:
lodsb
add rsi,3
test al,00111111b
jnz .findBinder
sub rsi,3
loop .findBinder
.foundBinder:
cmp byte [rsi],0
je .stopBinder
lodsd
mov edx,eax
mov ecx,eax
shr eax,6
and eax,00001FFFh
shr edx,6+13
and edx,00001FFFh
and ecx,00111111b
push rsi
call [ProcBinders + rcx * 8 - 8]
pop rsi
jmp .foundBinder
.stopBinder:
ret
BindString:
call IndexString
BindEntry:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rcx,rbx  
call [GetDlgItem]
test rax,rax
jz BindExit
mov r9,rsi
xor r8d,r8d
mov edx,WM_SETTEXT
xchg rcx,rax 
call [SendMessage]
BindExit:
mov rsp,rbp
pop rbp
BindRet:
ret
BindInfo:
lea rsi,[INFO_BUFFER + rax]
jmp BindEntry
BindBig:
lea rsi,[INFO_BUFFER + rax]
mov rsi,[rsi]
test rsi,rsi
jz BindRet
jmp BindEntry
BindBool:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov ecx,eax
shr eax,3
and ecx,0111b
movzx eax,byte [INFO_BUFFER + rax]
bt eax,ecx
setc al
xchg esi,eax
mov rcx,rbx  
call [GetDlgItem]
test rax,rax
jz .error
mov edx,esi
xchg rcx,rax 
call [EnableWindow]
.error:
jmp BindExit
BindCombo:
push r15 rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
lea rsi,[INFO_BUFFER + rax]
mov rcx,rbx  
call [GetDlgItem]
test rax,rax
jz .stop
xchg rdi,rax
mov r15d,0FFFF0000h
.tagCombo:
lodsb 
movzx rax,al
call [ProcCombo + rax * 8]
inc r15d
jnc .tagCombo
shr r15d,16
cmp r15w,0FFFFh
je .stop
xor r9d,r9d 
mov r8d,r15d 
mov edx,CB_SETCURSEL
mov rcx,rdi 
call [SendMessage]
.stop:
mov rsp,rbp
pop rbp r15
ret
BindComboStopOn:
stc
ret
BindComboStopOff: 
stc
ret
BindComboCurrent:
call HelperBindCombo
shl r15d,16
clc
ret
BindComboAdd:
call HelperBindCombo
clc
ret
BindComboInactive:
clc
ret
HelperBindCombo:
lodsw
push rsi rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
movzx eax,ax
call IndexString
mov r9,rsi
xor r8d,r8d
mov edx,CB_ADDSTRING
mov rcx,rdi 
call [SendMessage]
mov rsp,rbp
pop rbp rsi
ret

DecimalPrint32:
cld
push rax rbx rcx rdx
mov bh,80h-10
add bh,bl
mov ecx,1000000000
.mainCycle:
xor edx,edx
div ecx
and al,0Fh
test bh,bh
js .firstZero
cmp ecx,1
je .firstZero
cmp al,0
jz .skipZero
.firstZero:
mov bh,80h
or al,30h
stosb
.skipZero:
push rdx
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax 
pop rax
inc bh
test ecx,ecx
jnz .mainCycle 
pop rdx rcx rbx rax
ret

HexPrint64:
push rax
ror rax,32
call HexPrint32
pop rax
HexPrint32:
push rax
ror eax,16
call HexPrint16
pop rax
HexPrint16:
push rax
xchg al,ah
call HexPrint8
pop rax
HexPrint8:
push rax
ror al,4
call HexPrint4
pop rax
HexPrint4:
cld
push rax
and al,0Fh
cmp al,9
ja .modify
add al,'0'
jmp .store
.modify:
add al,'A'-10
.store:
stosb
pop rax
ret

SizePrint64:
push rax rbx rcx rdx rsi
cld
cmp bl,0FFh
je .autoUnits
mov esi,1
movzx ecx,bl
jrcxz .unitsAdjusted
.unitsCycle:
shl rsi,10
loop .unitsCycle
.unitsAdjusted:
mov cl,bl
xor edx,edx
div rsi
mov bl,0
call DecimalPrint32
imul eax,edx,10
div rsi
cmp cl,0
je .afterNumber
push rax
mov al,'.'
stosb
pop rax
jmp .decimalMode
.autoUnits:
xor ecx,ecx
test rax,rax
jz .decimalMode
.unitsAutoCycle:
mov rbx,rax
xor edx,edx
mov esi,1024                           
div rsi
mov esi,0FFFFFFFFh
cmp rbx,rsi
ja .above32bit
test rdx,rdx
jnz .modNonZero
.above32bit:
inc ecx
jmp .unitsAutoCycle
.modNonZero:
cmp ecx,4
ja .hexMode
mov eax,ebx
.decimalMode:
mov bl,0
call DecimalPrint32
.afterNumber:
mov al,' '
stosb
lea rsi,[U_B]
mov al,cl
mov ah,4
call StringWriteSelected
jmp .exit
.hexMode:
call HexPrint64
mov al,'h'
stosb 
.exit:
pop rsi rdx rcx rbx rax
ret

StringHelper:
mov rdi,rbp
push rax
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop rax
mov rbp,rdi
mov ah,0
call IndexString
lea rdi,[rbp - 82 + 01]
call StringWrite
lea rdi,[rbp - 82 + 33]
ret

SizeHelper32:
mov bl,0FFh
movzx eax,al
mov eax,dword [r12 + rax]
push rax
call SizePrint64
pop rax
lea rdi,[rbp - 82 + 62]
jmp HexPrint32

SizeHelper64:
mov bl,2
SizeHelperEntry:
movzx eax,al
mov rax,[r12 + rax]
push rax
call SizePrint64
pop rax
lea rdi,[rbp - 82 + 62]
jmp HexPrint64

SizeHelperAuto64:
mov bl,0FFh
jmp SizeHelperEntry 

NumberHelper32:
movzx eax,al
mov eax,[r12 + rax]
push rax
mov bl,0
call DecimalPrint32
pop rax
lea rdi,[rbp - 82 + 62]
jmp HexPrint32

NumberHelper16:
movzx eax,al
movzx eax,word [r12 + rax]
push rax
mov bl,0
call DecimalPrint32
pop rax
lea rdi,[rbp - 82 + 62]
jmp HexPrint16

section '.data' data readable writeable
MsgGuiFailed                   DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound             DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound            DB  'OS not supports memory status.'    , 0
MsgFunctionError               DB  'OS memory status failed.'          , 0
NameKernelDll                  DB  'KERNEL32.DLL'                      , 0
NameAdvapiDll                  DB  'ADVAPI32.DLL'                      , 0
NameFunctionMemory             DB  'GlobalMemoryStatusEx'              , 0
NameFunctionProcessors         DB  'GetActiveProcessorGroupCount'      , 0 
NameFunctionGroups             DB  'GetActiveProcessorCount'           , 0
NameFunctionNuma               DB  'GetNumaHighestNodeNumber'          , 0
NameFunctionLargePages         DB  'GetLargePageMinimum'               , 0
NameFunctionOpen               DB  'OpenProcessToken'                  , 0
NameFunctionAdjust             DB  'AdjustTokenPrivileges'             , 0
AppControl                     INITCOMMONCONTROLSEX  8, 0
U_B                            DB  'Bytes',0
U_KB                           DB  'KB',0
U_MB                           DB  'MB',0
U_GB                           DB  'GB',0
U_TB                           DB  'TB',0
U_MBPS                         DB  'MBPS',0
U_NS                           DB  'nanoseconds',0
ProcBinders                    DQ  BindString
                               DQ  BindInfo
                               DQ  BindBig
                               DQ  BindBool
                               DQ  BindCombo
ProcCombo                      DQ  BindComboStopOn
                               DQ  BindComboStopOff 
                               DQ  BindComboCurrent
                               DQ  BindComboAdd
                               DQ  BindComboInactive
HandleThis                     DQ  ? 
HandleIcon                     DQ  ?
LockedStrings                  DQ  ?
LockedBinders                  DQ  ?
HandleFont                     DQ  ?
HandleDll                      DQ  ?
_GlobalMemoryStatusEx          DQ  ?
_GetActiveProcessorGroupCount  DQ  ?
_GetActiveProcessorCount       DQ  ?
_GetNumaHighestNodeNumber      DQ  ?
_GetLargePageMinimum           DQ  ?
_OpenProcessToken              DQ  ?
_AdjustTokenPrivileges         DQ  ?
MemoryStatusEx                 MEMORYSTATUSEX_DEF  ?
SystemInfo                     SYSTEM_INFO         ?
ActiveProcessorGroupCount      DD  ?
ActiveProcessorCount           DD  ?
NumaNodesCount                 DD  ?
LargePageSize                  DQ  ?
LargePageEnable                DD  ?
align 4096 
INFO_BUFFER                    DB  INFO_BUFFER_SIZE DUP (?)
MISC_BUFFER                    DB  MISC_BUFFER_SIZE DUP (?)  

section '.idata' import data readable writeable
library kernel32 , 'kernel32.dll' , \
        user32   , 'user32.dll'   , \
        comctl32 , 'comctl32.dll' , \
        gdi32    , 'gdi32.dll'
include 'api\kernel32.inc'
include 'api\user32.inc'
include 'api\comctl32.inc'
include 'api\gdi32.inc'

section '.rsrc' resource data readable
directory RT_DIALOG     , dialogs   , \
          RT_RCDATA     , raws      , \ 
          RT_ICON       , icons     , \
          RT_GROUP_ICON , gicons    , \
          RT_MANIFEST   , manifests
resource dialogs, ID_MAIN, LANG_ENGLISH + SUBLANG_DEFAULT, mydialog
dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU + WS_SIZEBOX, 0, 0, 'System monospace', 18
dialogitem 'BUTTON', '', IDR_REPORT       , 241, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_BINARY       , 280, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_CANCEL       , 319, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'EDIT'  , '', IDR_TEXT_UP      ,  3,    5, 354,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem 'EDIT'  , '', IDR_TEXT_MAIN    ,  3,   18, 354, 200, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
enddialog
resource raws, ID_GUI_STRINGS, LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS, LANG_ENGLISH + SUBLANG_DEFAULT, guibinders
resdata guistrings
DB  'OS information (x64 v0.0)' , 0
DB  'System monospace bold'     , 0
DB  'Report', 0
DB  'Binary', 0
DB  'Cancel', 0
DB  ' Parameter                     | Value                      | Hex' , 0
DB  'Memory load'                  , 0
DB  'Total physical memory'        , 0
DB  'Available physical memory'    , 0
DB  'Total page file'              , 0
DB  'Available page file'          , 0
DB  'Total virtual user space'     , 0
DB  'Available virtual user space' , 0
DB  'Extended virtual'             , 0
DB  'Application minimum address'  , 0
DB  'Application maximum address'  , 0
DB  'Active processor mask'        , 0
DB  'Processor type'               , 0
DB  'Allocation granularity'       , 0
DB  'Processor level'              , 0
DB  'Processor revision'           , 0
DB  'Processors at current group'  , 0
DB  'Processors total'             , 0
DB  'Processor groups'             , 0
DB  'NUMA domains'                 , 0
DB  'Normal page size'             , 0
DB  'Minimum large page size'      , 0
DB  ' ( DISABLED )'                , 0
DB  ' ( ENABLED )'                 , 0
endres
resdata guibinders
BIND_STRING  STRING_REPORT       , IDR_REPORT
BIND_STRING  STRING_BINARY       , IDR_BINARY
BIND_STRING  STRING_CANCEL       , IDR_CANCEL
BIND_STRING  STRING_TEXT_UP      , IDR_TEXT_UP
BIND_INFO    BUFFER_SYSTEM_INFO  , IDR_TEXT_MAIN
BIND_STOP
endres
resource icons, ID_EXE_ICON, LANG_NEUTRAL, exeicon
resource gicons, ID_EXE_ICONS, LANG_NEUTRAL, exegicon
icon exegicon, exeicon, 'images\fasmicon64.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="OS information viewer"'
db '    processorArchitecture="amd64"'
db '    version="1.0.0.0"'
db '    type="win32"/>'
db '<description>Shield Icon Demo</description>'
db '<dependency>'
db '    <dependentAssembly>'
db '        <assemblyIdentity'
db '           type="win32"'
db '           name="Microsoft.Windows.Common-Controls"'
db '           version="6.0.0.0"'
db '           processorArchitecture="amd64"'
db '           publicKeyToken="6595b64144ccf1df"'
db '           language="*"'
db '        />'
db '     </dependentAssembly>'
db '  </dependency>'
db '</assembly>'
endres

