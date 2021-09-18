include 'win32a.inc'
CLEARTYPE_QUALITY      = 5
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

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

cld
xor eax,eax
mov [AllocatedBuffer],eax
mov [OriginalAffinityMask],eax
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

;---------- Start target code -------------------------------------------------;

BUFFER_SIZE       EQU  4 * 1024 * 1024
OFFSET_TEXT       EQU  16384  
PROCESSORS_LIMIT  EQU  32

;---------- Check CPUID support by check bit RFLAGS.21 writeable --------------;
mov ebx,21               ; Start check ID bit writeable for "1"
pushf                    ; In the 64-bit mode, push RFLAGS
pop eax                  
bts eax,ebx              ; Set EAX.21=1
push eax
popf                     ; Load RFLAGS with RFLAGS.21=1
pushf                    ; Store RFLAGS
pop eax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=1, Set EAX.21=0
jnc .errorCpuid          ; Go error branch if cannot set EFLAGS.21=1
push eax                 ; Start check ID bit writeable for "0"
popf                     ; Load RFLAGS with RFLAGS.21=0
pushf                    ; Store RFLAGS
pop eax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=0
jc .errorCpuid           ; Go error branch if cannot set EFLAGS.21=0

;---------- Get system information and WinAPI pointers ------------------------;
push SystemInfo
call [GetSystemInfo] 
call [GetCurrentThread]
test eax,eax
jz .errorWinAPI
mov [ThreadHandle],eax   ; Current thread handle, used for affinization
push NameKernelDll
call [GetModuleHandle]
test eax,eax
jz .errorWinAPI
push NameFunctionAffinity
push eax
call [GetProcAddress]
test eax,eax
jz .errorAffin
mov [_SetThreadAffinityMask],eax

;---------- Initializing cycle for logical processors -------------------------;
mov eax,[SystemInfo.dwNumberOfProcessors]
test eax,eax
jz .errorWinAPI
cmp eax,PROCESSORS_LIMIT
ja .errorCpuLimit
xor ebp,ebp                  ; EBP = Processor counter
mov [AffinityMask],1         ; Current thread affinity mask

;---------- Memory allocation for buffer --------------------------------------;
push PAGE_READWRITE
push MEM_COMMIT + MEM_RESERVE
push BUFFER_SIZE
push 0
call [VirtualAlloc]
test eax,eax
jz .errorAlloc
mov [AllocatedBuffer],eax    ; Base address of allocated block
add eax,OFFSET_TEXT
mov [TextPointer],eax        ; Base address of text block

;---------- Begin cycle for logical processors, thread affinization -----------;
.dumpProcessors:
push [AffinityMask]
push [ThreadHandle]
call [_SetThreadAffinityMask]
test eax,eax
jz .errorAffinFailed
lea edx,[OriginalAffinityMask]
cmp dword [edx],0
jnz .alreadySaved
mov [edx],eax
.alreadySaved:

;---------- Get CPUID binary dump, iteration for one logical CPU --------------;
mov edi,[AllocatedBuffer]
call GetCPUID
mov ebx,[edi]
test ebx,ebx
jz .errorCpuidFailed
cmp ebx,ENTRIES_LIMIT
ja .errorCpuidFailed 

;---------- Output CPUID dump to console --------------------------------------;
mov esi,[AllocatedBuffer]
add esi,32
mov edi,[TextPointer]
.dumpFunctions:
cmp edi,[TextPointer]
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
jne .noFirstLine
push ebx edi
sub edi,82 - 1
mov eax,ebp
mov bl,0
call DecimalPrint32
pop edi ebx
.noFirstLine:
push edi
mov eax,[esi + 04]
sub edi,82 - 11
call HexPrint32
pop edi
push edi
mov ecx,4
add esi,16
sub edi,82 - 24
.registers:
lodsd
call HexPrint32
add edi,3
loop .registers
pop edi
dec ebx                     ; Cycle counter for CPUID functions (subfunctions)
jnz .dumpFunctions         
mov [TextPointer],edi

;---------- Make cycle for logical processors ---------------------------------;
shl [AffinityMask],1
inc ebp 
cmp ebp,[SystemInfo.dwNumberOfProcessors]
jnb @f
mov ax,0A0Dh
stosw
mov [TextPointer],edi
@@:
jb .dumpProcessors

;---------- Close text block --------------------------------------------------;
mov edi,[TextPointer]
mov al,0
stosb
mov eax,[AllocatedBuffer]
add eax,OFFSET_TEXT
mov dword [INFO_BUFFER],eax

;---------- End target code, show dialogue window -----------------------------;

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
call ReleaseMemoryHelper
jz .errorRelease
call RestoreAffinityHelper
jz .errorAffinFailed
push 0           
call [ExitProcess]
.guifailed:
push MB_ICONERROR
push 0
push MsgGuiFailed 
push 0
call [MessageBox]  
call ReleaseMemoryHelper
call RestoreAffinityHelper
push 1           
call [ExitProcess]
.errorCpuid:
lea ebx,[MsgErrorCpuid]
jmp .failed
.errorAlloc:
lea ebx,[MsgErrorAlloc]
jmp .failed
.errorRelease:
lea ebx,[MsgErrorRelease]
jmp .failed
.errorWinAPI:
lea ebx,[MsgErrorWinAPI]
jmp .failed
.errorAffin:
lea ebx,[MsgErrorAffin]
jmp .failed
.errorAffinFailed:
lea ebx,[MsgErrorAffinFailed]
jmp .failed
.errorCpuLimit:
lea ebx,[MsgErrorCpuLimit]
jmp .failed
.errorCpuidFailed:
lea ebx,[MsgErrorCpuidFailed]
.failed:
push MB_ICONERROR
push 0
push ebx
push 0
call [MessageBox]  
call ReleaseMemoryHelper
call RestoreAffinityHelper
push 2           
call [ExitProcess]

ReleaseMemoryHelper:
mov ecx,[AllocatedBuffer]
jecxz .free
push MEM_RELEASE
push 0
push ecx
call [VirtualFree]
test eax,eax
.free:
ret

RestoreAffinityHelper:
mov edx,[OriginalAffinityMask]
test edx,edx
jz .unchanged
push edx
push [ThreadHandle]
call [_SetThreadAffinityMask]
test eax,eax
.unchanged:
ret

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

include  'cpuid_ia32.inc'

section '.data' data readable writeable
MsgGuiFailed            DB  'GUI initialization failed.'                 , 0
MsgErrorCpuid           DB  'CPUID instruction not supported or locked.' , 0
MsgErrorAlloc           DB  'Memory allocation error.'                   , 0
MsgErrorRelease         DB  'Memory release error.'                      , 0
MsgErrorWinAPI          DB  'WinAPI initialization failed.'              , 0
MsgErrorAffin           DB  'Affinization WinAPI not found.'             , 0
MsgErrorAffinFailed     DB  'Affinization failed.'                       , 0
MsgErrorCpuLimit        DB  'Too many logical processors detected.'      , 0  
MsgErrorCpuidFailed     DB  'CPUID failed.'                              , 0
NameKernelDll           DB  'KERNEL32.DLL'                               , 0
NameFunctionAffinity    DB  'SetThreadAffinityMask'                      , 0
AppControl              INITCOMMONCONTROLSEX  8, 0
U_B                     DB  'Bytes',0
U_KB                    DB  'KB',0
U_MB                    DB  'MB',0
U_GB                    DB  'GB',0
U_TB                    DB  'TB',0
U_MBPS                  DB  'MBPS',0
U_NS                    DB  'nanoseconds',0
ProcBinders             DD  BindString
                        DD  BindInfo
                        DD  BindBig
                        DD  BindBool
                        DD  BindCombo
ProcCombo               DD  BindComboStopOn
                        DD  BindComboStopOff 
                        DD  BindComboCurrent
                        DD  BindComboAdd
                        DD  BindComboInactive
HandleThis              DD  ? 
HandleIcon              DD  ?
LockedStrings           DD  ?
LockedBinders           DD  ?
HandleFont              DD  ?
_SetThreadAffinityMask  DD  ?
SystemInfo              SYSTEM_INFO  ?
AllocatedBuffer         DD  ?
OriginalAffinityMask    DD  ?
ThreadHandle            DD  ?
AffinityMask            DD  ?
TextPointer             DD  ?
align 4096 
INFO_BUFFER             DB  INFO_BUFFER_SIZE DUP (?)
MISC_BUFFER             DB  MISC_BUFFER_SIZE DUP (?)  

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
; dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU + WS_SIZEBOX, 0, 0, 'System monospace', 18
dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU, 0, 0, 'System monospace', 18
dialogitem 'BUTTON', '', IDR_REPORT       , 241, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_BINARY       , 280, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_CANCEL       , 319, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'EDIT'  , '', IDR_TEXT_UP      ,  3,    5, 354,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem 'EDIT'  , '', IDR_TEXT_MAIN    ,  3,   18, 354, 200, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
enddialog
resource raws, ID_GUI_STRINGS, LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS, LANG_ENGLISH + SUBLANG_DEFAULT, guibinders
resdata guistrings
DB  'Affinized CPUID information (ia32 v0.02)' , 0
DB  'System monospace bold' , 0
DB  'Report', 0
DB  'Binary', 0
DB  'Cancel', 0
DB  ' Thread  | Function   | EAX      | EBX      | ECX      | EDX' , 0
endres
resdata guibinders
BIND_STRING  STRING_REPORT  , IDR_REPORT
BIND_STRING  STRING_BINARY  , IDR_BINARY
BIND_STRING  STRING_CANCEL  , IDR_CANCEL
BIND_STRING  STRING_TEXT_UP , IDR_TEXT_UP
BIND_BIG     0              , IDR_TEXT_MAIN
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
db '    name="Affinized CPUID information viewer"'
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
