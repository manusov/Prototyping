include 'win64a.inc'
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

format PE64 GUI 5.0
entry start
section '.code' code readable executable
start:

sub rsp,8*5
cld
xor eax,eax
mov [AllocatedBuffer],rax
mov [OriginalAffinityMask],rax
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

;---------- Start target code -------------------------------------------------;

BUFFER_SIZE       EQU  4 * 1024 * 1024
OFFSET_TEXT       EQU  16384  
PROCESSORS_LIMIT  EQU  64

;---------- Check CPUID support by check bit RFLAGS.21 writeable --------------;
mov ebx,21               ; Start check ID bit writeable for "1"
pushf                    ; In the 64-bit mode, push RFLAGS
pop rax                  
bts eax,ebx              ; Set EAX.21=1
push rax
popf                     ; Load RFLAGS with RFLAGS.21=1
pushf                    ; Store RFLAGS
pop rax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=1, Set EAX.21=0
jnc .errorCpuid          ; Go error branch if cannot set EFLAGS.21=1
push rax                 ; Start check ID bit writeable for "0"
popf                     ; Load RFLAGS with RFLAGS.21=0
pushf                    ; Store RFLAGS
pop rax                  ; Load RFLAGS to RAX
btr eax,ebx              ; Check EAX.21=0
jc .errorCpuid           ; Go error branch if cannot set EFLAGS.21=0

;---------- Get system information and WinAPI pointers ------------------------;
lea rcx,[SystemInfo]
call [GetSystemInfo] 
call [GetCurrentThread]
test rax,rax
jz .errorWinAPI
xchg r12,rax            ; R12 = Current thread handle, used for affinization
lea rcx,[NameKernelDll]
call [GetModuleHandle]
test rax,rax
jz .errorWinAPI
lea rdx,[NameFunctionAffinity]
xchg rcx,rax
call [GetProcAddress]
test rax,rax
jz .errorAffin
mov [_SetThreadAffinityMask],rax

;---------- Initializing cycle for logical processors -------------------------;
mov eax,[SystemInfo.dwNumberOfProcessors]
test eax,eax
jz .errorWinAPI
cmp eax,PROCESSORS_LIMIT
ja .errorCpuLimit
xor ebp,ebp                  ; EBP = Processor counter
mov r15d,1                   ; R15 = Current thread affinity mask

;---------- Memory allocation for buffer --------------------------------------;
xor ecx,ecx
mov edx,BUFFER_SIZE
mov r8d,MEM_COMMIT + MEM_RESERVE
mov r9d,PAGE_READWRITE
call [VirtualAlloc]
test rax,rax
jz .errorAlloc
mov [AllocatedBuffer],rax    ; Base address of allocated block
lea r13,[rax + OFFSET_TEXT]  ; R13 = Base address of text block

;---------- Begin cycle for logical processors, thread affinization -----------;
.dumpProcessors:
mov rcx,r12
mov rdx,r15
call [_SetThreadAffinityMask]
test rax,rax
jz .errorAffinFailed
lea rdx,[OriginalAffinityMask]
cmp qword [rdx],0            ; Original affinity mask
jne .alreadySaved
mov [rdx],rax
.alreadySaved:

;---------- Get CPUID binary dump, iteration for one logical CPU --------------;
mov rdi,[AllocatedBuffer]
call GetCPUID
mov ebx,[rdi]
test ebx,ebx
jz .errorCpuidFailed
cmp ebx,ENTRIES_LIMIT
ja .errorCpuidFailed 

;---------- Output CPUID dump to console --------------------------------------;
mov rsi,[AllocatedBuffer]
add rsi,32
mov rdi,r13
.dumpFunctions:
cmp rdi,r13
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
jne .noFirstLine
push rbx rdi
sub rdi,82 - 1
mov eax,ebp
mov bl,0
call DecimalPrint32
pop rdi rbx
.noFirstLine:
push rdi
mov eax,[rsi + 04]
sub rdi,82 - 11
call HexPrint32
pop rdi
push rdi
mov ecx,4
add rsi,16
sub rdi,82 - 24
.registers:
lodsd
call HexPrint32
add rdi,3
loop .registers
pop rdi
dec ebx                     ; Cycle counter for CPUID functions (subfunctions)
jnz .dumpFunctions         
mov r13,rdi

;---------- Make cycle for logical processors ---------------------------------;
shl r15,1
inc ebp 
cmp ebp,[SystemInfo.dwNumberOfProcessors]
jnb @f
mov ax,0A0Dh
stosw
mov r13,rdi
@@:
jb .dumpProcessors

;---------- Close text block --------------------------------------------------;
mov rdi,r13
mov al,0
stosb
mov rax,[AllocatedBuffer]
add rax,OFFSET_TEXT
mov qword [INFO_BUFFER],rax

;---------- End target code, show dialogue window -----------------------------;

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
call ReleaseMemoryHelper
jz .errorRelease
call RestoreAffinityHelper
jz .errorAffinFailed
xor ecx,ecx           
call [ExitProcess]
.guiFailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgGuiFailed]
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
call RestoreAffinityHelper
mov ecx,1           
call [ExitProcess]
.errorCpuid:
lea rbx,[MsgErrorCpuid]
jmp .failed
.errorAlloc:
lea rbx,[MsgErrorAlloc]
jmp .failed
.errorRelease:
lea rbx,[MsgErrorRelease]
jmp .failed
.errorWinAPI:
lea rbx,[MsgErrorWinAPI]
jmp .failed
.errorAffin:
lea rbx,[MsgErrorAffin]
jmp .failed
.errorAffinFailed:
lea rbx,[MsgErrorAffinFailed]
jmp .failed
.errorCpuLimit:
lea rbx,[MsgErrorCpuLimit]
jmp .failed
.errorCpuidFailed:
lea rbx,[MsgErrorCpuidFailed]
.failed:
mov r9d,MB_ICONERROR
xor r8d,r8d
mov rdx,rbx
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
call RestoreAffinityHelper
mov ecx,2           
call [ExitProcess]

ReleaseMemoryHelper:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rcx,[AllocatedBuffer]
jrcxz .free
xor edx,edx
mov r8d,MEM_RELEASE
call [VirtualFree]
test rax,rax
.free:
mov rsp,rbp
pop rbp
ret

RestoreAffinityHelper:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rcx,r12
xor rdx,rdx
mov rdx,[OriginalAffinityMask]
test rdx,rdx
jz .unchanged
call [_SetThreadAffinityMask]
test rax,rax
.unchanged:
mov rsp,rbp
pop rbp
ret

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

include  'cpuid_x64.inc'

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
ProcBinders             DQ  BindString
                        DQ  BindInfo
                        DQ  BindBig
                        DQ  BindBool
                        DQ  BindCombo
ProcCombo               DQ  BindComboStopOn
                        DQ  BindComboStopOff 
                        DQ  BindComboCurrent
                        DQ  BindComboAdd
                        DQ  BindComboInactive
HandleThis              DQ  ? 
HandleIcon              DQ  ?
LockedStrings           DQ  ?
LockedBinders           DQ  ?
HandleFont              DQ  ?
_SetThreadAffinityMask  DQ  ?
SystemInfo              SYSTEM_INFO  ?
AllocatedBuffer         DQ  ?
OriginalAffinityMask    DQ  ?
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
DB  'Affinized CPUID information (x64 v0.02)' , 0
DB  'System monospace bold'          , 0
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
icon exegicon, exeicon, 'images\fasmicon64.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="Affinized CPUID information viewer"'
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

