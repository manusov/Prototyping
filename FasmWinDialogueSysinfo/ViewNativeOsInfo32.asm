; TODO.
; 1) Error handling.
; 2) Dynamical import.
; 3) Use GlobalMemoryStatus if GlobalMemoryStatusEx not supported.
; 4) Synchronize with sub-set of NCRB functionality.
; 5) Remove numbers usage, for example "82 + 50".
; 6) Optimize for resources DLL, common for NCRB32/64,
;    make some normal strings as resource raw objexts.
; 7) Required Windows-on-Windows (WoW64) info by 32-bit sample under 64-bit OS.
; 8) Optimize, remove not required PUSH/POP RAX before/after SizePrint64,
;    and other subroutines, DecimalPrint32.

include 'win32a.inc'
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
virtual at ebx 
tp TOKEN_PRIVILEGES 
end virtual

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

cld
push AppControl
call [InitCommonControlsEx]
test eax,eax
jz .guifailed
push 0
call [GetModuleHandle]
test eax,eax
jz .guifailed
mov [HandleThis],eax
push ID_EXE_ICONS
push eax 
call [LoadIcon]
test eax,eax
jz .guifailed
mov [HandleIcon],eax
push RT_RCDATA
push ID_GUI_STRINGS
push [HandleThis]
call [FindResource]                
test eax,eax
jz .guifailed 
push eax
push [HandleThis]
call [LoadResource] 
test eax,eax
jz .guifailed
push eax
call [LockResource]  
test eax,eax
jz .guifailed
mov [LockedStrings],eax
push RT_RCDATA
push ID_GUI_BINDERS
push [HandleThis]
call [FindResource]                
test eax,eax
jz .guifailed
push eax
push [HandleThis]
call [LoadResource] 
test eax,eax
jz .guifailed
push eax
call [LockResource]  
test eax,eax
jz .guifailed
mov [LockedBinders],eax

push NameKernelDll
call [GetModuleHandle]
test eax,eax
jz .libraryNotFound
mov [HandleDll],eax 
push NameFunctionMemory
mov ebx,[HandleDll]
push ebx
call [GetProcAddress]
test eax,eax
jz .functionNotFound
mov [_GlobalMemoryStatusEx],eax
push NameFunctionNative
push ebx
call [GetProcAddress]
test eax,eax
jz .nativeNotFound
mov [_GetNativeSystemInfo],eax
push NameFunctionProcessors 
push ebx
call [GetProcAddress]
mov [_GetActiveProcessorGroupCount],eax
push NameFunctionGroups
push ebx
call [GetProcAddress]
mov [_GetActiveProcessorCount],eax
push NameFunctionNuma
push ebx
call [GetProcAddress]
mov [_GetNumaHighestNodeNumber],eax
push NameFunctionLargePages
push ebx
call [GetProcAddress]
mov [_GetLargePageMinimum],eax
lea ebx,[_OpenProcessToken]
xor eax,eax
mov [ebx + 00],eax
mov [ebx + 04],eax
push NameAdvapiDll
call [LoadLibrary]
test eax,eax
jz @f
xchg ebp,eax
push NameFunctionOpen
push ebp
call [GetProcAddress]               
mov [ebx + 00],eax
push NameFunctionAdjust
push ebp
call [GetProcAddress]
mov [ebx + 04],eax
@@:
lea ecx,[MemoryStatusEx]
mov [ecx + MEMORYSTATUSEX_DEF.dwLength],sizeof.MEMORYSTATUSEX_DEF
push ecx 
call [_GlobalMemoryStatusEx]
test eax,eax
jz .statusFailed
push SystemInfo

; call [GetSystemInfo]
  call [_GetNativeSystemInfo]

mov eax,[_GetActiveProcessorGroupCount]
test eax,eax
jz @f
call eax
movzx eax,ax
@@:
mov [ActiveProcessorGroupCount],eax
mov eax,[_GetActiveProcessorCount]
test eax,eax
jz @f
push ALL_PROCESSOR_GROUPS
call eax
@@:
mov [ActiveProcessorCount],eax
lea ebx,[NumaNodesCount]
mov eax,[_GetNumaHighestNodeNumber]
test eax,eax
jz @f
push ebx
call eax
test eax,eax
jz @f
mov eax,[ebx]
inc eax
@@:
mov [ebx],eax
mov eax,[_GetLargePageMinimum]
test eax,eax
jz @f
call eax
@@:
mov [LargePageSize],eax

push ebx esi ebp
sub esp,128
mov [LargePageEnable],0
xor eax,eax
cmp [_OpenProcessToken],eax
je .exit
cmp [_AdjustTokenPrivileges],eax 
je .exit
cmp [LargePageSize],eax 
je .exit
call [GetCurrentProcess]
test eax,eax
jz .skip
lea ebp,[esp + 120]
push ebp 
push MAXIMUM_ALLOWED
push eax
call [_OpenProcessToken]
test eax,eax 
jz .skip   
xor eax,eax  
mov ebx,esp
push eax
push eax
push eax
push ebx 
push eax
push dword [ebp]
mov [tp.PrivilegeCount],1 
mov [tp.Privileges.Luid.usedpart],SE_LOCK_MEMORY_PRIVILEGE 
and [tp.Privileges.Luid.ignorehigh32bitpart],0  
mov [tp.Privileges.Attributes],SE_PRIVILEGE_ENABLED
call [_AdjustTokenPrivileges] 
mov ebx,eax 
push dword [ebp] 
call [CloseHandle] 
.skip:
test eax,eax
jz .exit
xor eax,eax
test ebx,ebx
jz .exit 
call [GetCurrentProcess]
test eax,eax
jz .exit
mov ebx,eax
push PAGE_READWRITE
push MEM_COMMIT + MEM_LARGE_PAGES
push [LargePageSize]
push 0
push ebx
call [VirtualAllocEx]
test eax,eax
jz @f
push MEM_RELEASE
push 0
push eax
push ebx
call [VirtualFreeEx]
@@:
test eax,eax
setnz al
movzx eax,al
mov [LargePageEnable],eax 
.exit:
add esp,128
pop ebp esi ebx

lea ebp,[INFO_BUFFER]
mov al,STRING_MEMORY_LOAD
call StringHelper
mov eax,[MemoryStatusEx.dwMemoryLoad]
push eax
mov bl,0
call DecimalPrint32
mov ax,' %'
stosw
pop eax
lea edi,[ebp - 82 + 62]
call HexPrint32 
mov al,STRING_TOTAL_PHYSICAL
call StringHelper
lea ecx,[MemoryStatusEx.ullTotalPhys]
call SizeHelper64
mov al,STRING_AVAIL_PHYSICAL
call StringHelper
lea ecx,[MemoryStatusEx.ullAvailPhys]
call SizeHelper64
mov al,STRING_TOTAL_PAGE_FILE 
call StringHelper
lea ecx,[MemoryStatusEx.ullTotalPageFile]
call SizeHelper64
mov al,STRING_AVAIL_PAGE_FILE
call StringHelper
lea ecx,[MemoryStatusEx.ullAvailPageFile]
call SizeHelper64
mov al,STRING_TOTAL_VIRTUAL
call StringHelper
lea ecx,[MemoryStatusEx.ullTotalVirtual]
call SizeHelper64
mov al,STRING_AVAIL_VIRTUAL
call StringHelper
lea ecx,[MemoryStatusEx.ullAvailVirtual]
call SizeHelper64
mov al,STRING_EXT_VIRTUAL
call StringHelper
mov al,'-'
stosb
mov eax,dword [MemoryStatusEx.ullAvailExtendedVirtual + 0]
mov edx,dword [MemoryStatusEx.ullAvailExtendedVirtual + 4]
lea edi,[ebp - 82 + 62]
call HexPrint64 
mov edi,ebp
mov ax,0A0Dh
stosw
mov ebp,edi 

mov al,STRING_MIN_ADDRESS
call StringHelper
lea ecx,[SystemInfo.lpMinimumApplicationAddress]
call SizeHelperAuto32
mov al,STRING_MAX_ADDRESS
call StringHelper
lea ecx,[SystemInfo.lpMaximumApplicationAddress]
call SizeHelperAuto32
mov al,STRING_PROC_MASK
call StringHelper
mov al,'-'
stosb
mov eax,[SystemInfo.dwActiveProcessorMask]
lea edi,[ebp - 82 + 62]
call HexPrint32 
mov al,STRING_PROC_TYPE
call StringHelper
lea ecx,[SystemInfo.dwProcessorType]
call NumberHelper32
mov al,STRING_ALLOC_GRAN
call StringHelper
lea ecx,[SystemInfo.dwAllocationGranularity]
call SizeHelperAuto32
mov al,STRING_PROC_LEVEL
call StringHelper
lea ecx,[SystemInfo.wProcessorLevel]
call NumberHelper16
mov al,STRING_PROC_REVISION
call StringHelper
lea ecx,[SystemInfo.wProcessorRevision]
call NumberHelper16
mov edi,ebp
mov ax,0A0Dh
stosw
mov ebp,edi 

mov al,STRING_PROC_CURRENT
call StringHelper
lea ecx,[SystemInfo.dwNumberOfProcessors]
call NumberHelper32
cmp [ActiveProcessorCount],0
je @f
mov al,STRING_PROC_TOTAL
call StringHelper
lea ecx,[ActiveProcessorCount]
call NumberHelper32
@@:
cmp [ActiveProcessorGroupCount],0
je @f
mov al,STRING_PROC_GROUPS
call StringHelper
lea ecx,[ActiveProcessorGroupCount]
call NumberHelper32
@@:
cmp [NumaNodesCount],0
je @f
mov al,STRING_NUMA_DOMAINS
call StringHelper
lea ecx,[NumaNodesCount]
call NumberHelper32
@@:
mov al,STRING_NORMAL_PAGE
call StringHelper
lea ecx,[SystemInfo.dwPageSize]
call SizeHelperAuto32
cmp [LargePageSize],0
je @f
mov al,STRING_LARGE_PAGE
call StringHelper
mov eax,[LargePageSize]
xor edx,edx
push eax
mov bl,0FFh
call SizePrint64
cmp [LargePageEnable],1
sete al
add al,STRING_DISABLED
mov ah,0
call IndexString
call StringWrite
pop eax
lea edi,[ebp - 82 + 62]
call HexPrint64
@@:
mov al,0
stosb

push 0 0 
push DialogProc
push HWND_DESKTOP
push ID_MAIN
push [HandleThis]  
call [DialogBoxParam] 
test eax,eax
jz .guifailed 
cmp eax,-1
je .guifailed
.ok:
push 0           
call [ExitProcess]
.guifailed:
push MB_ICONERROR
push 0
push MsgGuiFailed 
push 0
call [MessageBox]  
push 1           
call [ExitProcess]
.libraryNotFound:
lea ebx,[MsgLibraryNotFound]
jmp .failed
.functionNotFound:
lea ebx,[MsgFunctionNotFound]
jmp .failed
.nativeNotFound:
lea ebx,[MsgNativeNotFound]
jmp .failed
.statusFailed:
lea ebx,[MsgFunctionError]
.failed:
push MB_ICONERROR
push 0
push ebx
push 0
call [MessageBox]  
push 2           
call [ExitProcess]

DialogProc:
push ebp ebx esi edi
mov ebp,esp
mov eax,[ebp + 24]
cmp eax,WM_INITDIALOG
je .wminitdialog 
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_CLOSE
je .wmclose
xor eax,eax
jmp .finish
.wminitdialog:
mov ebx,[ebp + 20]
mov ax,BIND_APP_GUI
call Binder
push [HandleIcon] 
push ICON_SMALL 
push WM_SETICON 
push dword [ebp + 20]
call [SendMessage]
mov ax,STRING_APP
call IndexString
push esi
push dword [ebp + 20]
call [SetWindowText]
mov ax,STRING_FONT
call IndexString
xor eax,eax
push esi
push FIXED_PITCH
push CLEARTYPE_QUALITY
push CLIP_DEFAULT_PRECIS
push OUT_TT_ONLY_PRECIS
push DEFAULT_CHARSET
push eax
push eax
push eax
push FW_DONTCARE
push eax
push eax
push eax
push 17
call [CreateFont]
mov [HandleFont],eax
mov edx,IDR_TEXT_UP
call FontHelper
mov edx,IDR_TEXT_MAIN
call FontHelper
jmp .processed
.wmcommand:
mov eax,[ebp + 28]
cmp eax,IDR_CANCEL
je .wmclose
jmp .processed
.wmclose:
push 1
push dword [ebp + 20]
call [EndDialog]
.processed:
mov eax,1
.finish:
mov esp,ebp
pop edi esi ebx ebp
ret 16

FontHelper:
push edx
push dword [ebp + 20]
call [GetDlgItem]
test eax,eax
jz @f
push 1
push [HandleFont]
push WM_SETFONT
push eax
call [SendMessage]
@@:
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
mov esi,[LockedStrings]
movzx ecx,ax
jecxz .stop
.cycle:
lodsb
cmp al,0
jne .cycle
loop .cycle
.stop:
ret

Binder:
cld
mov esi,[LockedBinders]
movzx ecx,ax
jecxz .foundBinder
.findBinder:
lodsb
add esi,3
test al,00111111b
jnz .findBinder
sub esi,3
loop .findBinder
.foundBinder:
cmp byte [esi],0
je .stopBinder
lodsd
mov edx,eax
mov ecx,eax
shr eax,6
and eax,00001FFFh
shr edx,6+13
and edx,00001FFFh
and ecx,00111111b
push esi
call [ProcBinders + ecx * 4 - 4]
pop esi
jmp .foundBinder
.stopBinder:
ret
BindString:
call IndexString
BindEntry:
push edx
push ebx  
call [GetDlgItem]
test eax,eax
jz BindExit
push esi
push 0
push WM_SETTEXT
push eax 
call [SendMessage]
BindExit:
ret
BindInfo:
lea esi,[INFO_BUFFER + eax]
jmp BindEntry
BindBig:
lea esi,[INFO_BUFFER + eax]
mov esi,[esi]
test esi,esi
jz BindExit
jmp BindEntry
BindBool:
mov ecx,eax
shr eax,3
and ecx,0111b
movzx eax,byte [INFO_BUFFER + eax]
bt eax,ecx
setc al
xchg esi,eax
push edx
push ebx  
call [GetDlgItem]
test eax,eax
jz .error
push esi
push eax 
call [EnableWindow]
.error:
jmp BindExit
BindCombo:
lea esi,[INFO_BUFFER + eax]
push edx 
push ebx  
call [GetDlgItem]
test eax,eax
jz .stop
xchg edi,eax
mov ebp,0FFFF0000h
.tagCombo:
lodsb 
movzx eax,al
call [ProcCombo + eax * 4]
inc ebp
jnc .tagCombo
shr ebp,16
cmp bp,0FFFFh
je .stop
push 0 
push ebp 
push CB_SETCURSEL
push edi 
call [SendMessage]
.stop:
ret
BindComboStopOn:
stc
ret
BindComboStopOff: 
stc
ret
BindComboCurrent:
call HelperBindCombo
shl ebp,16
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
push esi
movzx eax,ax
call IndexString
push esi
push 0
push CB_ADDSTRING
push edi 
call [SendMessage]
pop esi
ret

DecimalPrint32:
cld
push eax ebx ecx edx
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
push edx
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax 
pop eax
inc bh
test ecx,ecx
jnz .mainCycle 
pop edx ecx ebx eax
ret

HexPrint64:
xchg eax,edx
call HexPrint32
xchg eax,edx
HexPrint32:
push eax
ror eax,16
call HexPrint16
pop eax
HexPrint16:
push eax
xchg al,ah
call HexPrint8
pop eax
HexPrint8:
push eax
ror al,4
call HexPrint4
pop eax
HexPrint4:
cld
push eax
and al,0Fh
add al,90h
daa
adc al,40h
daa
stosb
pop eax
ret

SizePrint64:
pushad
cld
xor ecx,ecx
test eax,eax
jnz .unitsAutoCycle
test edx,edx
jz .decimalMode
xor ebp,ebp
xor esi,esi
.unitsAutoCycle:
mov ebp,eax
shrd eax,edx,10
shr edx,10
jnz .above32bit 
cmp cl,bl
je .modNonZero
xor esi,esi
shrd esi,ebp,10
shr esi,22
cmp bl,0FFh
jne .above32bit 
test esi,esi
jnz .modNonZero
.above32bit:                
inc ecx
jmp .unitsAutoCycle
.modNonZero:
cmp ecx,4
ja .hexMode
mov eax,ebp
.decimalMode:
push ebx
mov bl,0
call DecimalPrint32
pop ebx
jecxz .afterNumber
cmp bl,0FFh
je .afterNumber
mov al,'.'
stosb
xchg eax,esi
xor edx,edx
mov ebx,102
div ebx
cmp eax,9
jbe .limitDecimal
mov eax,9
.limitDecimal:
mov bl,0
call DecimalPrint32
.afterNumber:
mov al,' '
stosb
lea esi,[U_B]
mov al,cl
mov ah,4
call StringWriteSelected
jmp .exit
.hexMode:
call HexPrint64
mov al,'h'
stosb 
.exit:
mov [esp],edi
popad
ret

StringHelper:
mov edi,ebp
push eax
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop eax
mov ebp,edi
mov ah,0
call IndexString
lea edi,[ebp - 82 + 01]
call StringWrite
lea edi,[ebp - 82 + 33]
ret

SizeHelperAuto32:
mov bl,0FFh
jmp SjzeHelperEntry

SizeHelper32:
mov bl,2
SjzeHelperEntry:
mov eax,dword [ecx + 00]
xor edx,edx
push eax
call SizePrint64
pop eax
lea edi,[ebp - 82 + 62]
jmp HexPrint32

SizeHelper64:
mov bl,2
mov eax,[ecx + 00]
mov edx,[ecx + 04]
push eax edx
call SizePrint64
pop edx eax
lea edi,[ebp - 82 + 62]
jmp HexPrint64

NumberHelper32:
mov eax,[ecx]
push eax
mov bl,0
call DecimalPrint32
pop eax
lea edi,[ebp - 82 + 62]
jmp HexPrint32

NumberHelper16:
movzx eax,word [ecx]
push eax
mov bl,0
call DecimalPrint32
pop eax
lea edi,[ebp - 82 + 62]
jmp HexPrint16

section '.data' data readable writeable
MsgGuiFailed                   DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound             DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound            DB  'OS not supports memory status.'    , 0
MsgNativeNotFound              DB  'WoW64 not found.'                  , 0
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
NameFunctionNative             DB  'GetNativeSystemInfo'               , 0
AppControl                     INITCOMMONCONTROLSEX  8, 0
U_B                            DB  'Bytes',0
U_KB                           DB  'KB',0
U_MB                           DB  'MB',0
U_GB                           DB  'GB',0
U_TB                           DB  'TB',0
U_MBPS                         DB  'MBPS',0
U_NS                           DB  'nanoseconds',0
ProcBinders                    DD  BindString
                               DD  BindInfo
                               DD  BindBig
                               DD  BindBool
                               DD  BindCombo
ProcCombo                      DD  BindComboStopOn
                               DD  BindComboStopOff 
                               DD  BindComboCurrent
                               DD  BindComboAdd
                               DD  BindComboInactive
HandleThis                     DD  ? 
HandleIcon                     DD  ?
LockedStrings                  DD  ?
LockedBinders                  DD  ?
HandleFont                     DD  ?
HandleDll                      DD  ?
_GlobalMemoryStatusEx          DD  ?
_GetActiveProcessorGroupCount  DD  ?
_GetActiveProcessorCount       DD  ?
_GetNumaHighestNodeNumber      DD  ?
_GetLargePageMinimum           DD  ?
_OpenProcessToken              DD  ?
_AdjustTokenPrivileges         DD  ?
_GetNativeSystemInfo           DD  ?
MemoryStatusEx                 MEMORYSTATUSEX_DEF  ?
SystemInfo                     SYSTEM_INFO         ?
ActiveProcessorGroupCount      DD  ?
ActiveProcessorCount           DD  ?
NumaNodesCount                 DD  ?
LargePageSize                  DD  ?
LargePageEnable                DD  ?
align 4096 
INFO_BUFFER                    DB  INFO_BUFFER_SIZE DUP (?)
NISC_BUFFER                    DB  MISC_BUFFER_SIZE DUP (?)  

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
DB  'Native OS information (ia32 v0.0)' , 0
DB  'System monospace bold'             , 0
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
icon exegicon, exeicon, 'images\fasmicon32.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="OS information viewer"'
db '    processorArchitecture="x86"'
db '    version="1.0.0.0"'
db '    type="win32"/>'
db '<description>Shield Icon Demo</description>'
db '<dependency>'
db '    <dependentAssembly>'
db '        <assemblyIdentity'
db '           type="win32"'
db '           name="Microsoft.Windows.Common-Controls"'
db '           version="6.0.0.0"'
db '           processorArchitecture="x86"'
db '           publicKeyToken="6595b64144ccf1df"'
db '           language="*"'
db '        />'
db '     </dependentAssembly>'
db '  </dependency>'
db '</assembly>'
endres

