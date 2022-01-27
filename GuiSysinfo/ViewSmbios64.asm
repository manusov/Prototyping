; TODO. Write header: version, block length, structures count.
; TODO. Optimize text generating cycle.

include 'win64a.inc'
CLEARTYPE_QUALITY     = 5
ID_MAIN               = 100
ID_EXE_ICON           = 101
ID_EXE_ICONS          = 102
ID_GUI_STRINGS        = 103
ID_GUI_BINDERS        = 104
ID_SMBIOS_NAMES       = 105
IDR_UP_LIST           = 200
IDR_TEXT_LIST         = 201
IDR_TEXT_SUMMARY      = 202
IDR_REPORT            = 203
IDR_BINARY            = 204
IDR_CANCEL            = 205
STRING_APP            = 0
STRING_FONT           = 1
STRING_REPORT         = 2
STRING_BINARY         = 3
STRING_CANCEL         = 4
STRING_UP_LIST        = 5
STRING_SMBIOS_VERSION = 6 
STRING_SMBIOS_METHOD  = 7
STRING_SMBIOS_DMI_REV = 8
STRING_SMBIOS_LENGTH  = 9
STRING_SMBIOS_BYTES   = 10
BUFFER_SMBIOS_LIST    = 0
BUFFER_SMBIOS_SUMMARY = 8
BIND_APP_GUI          = 0
X_SIZE                = 360
Y_SIZE                = 240
INFO_BUFFER_SIZE      = 8192
MISC_BUFFER_SIZE      = 8192
LIMIT_TOTAL           = LIMIT_ENUM + LIMIT_TABLE + LIMIT_LIST
LIMIT_ENUM            = 1024 * 128
LIMIT_TABLE           = 1024 * 1024
LIMIT_LIST            = 1024 * 256
BASE_ENUM             = 0
BASE_TABLE            = LIMIT_ENUM 
BASE_LIST             = LIMIT_ENUM + LIMIT_TABLE

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

struct DATA_HEADER
method       db 0
versionMajor db 0
versionMinor db 0
revisionDmi  db 0
dataLength   dd 0
ends

struct OBJECT_HEADER
type   db 0
length db 0
handle dw 0
ends

SMBIOS_TYPE_0       = 0
SMBIOS_UNKNOWN_TYPE = 45

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

mov r8d,RT_RCDATA
mov edx,ID_SMBIOS_NAMES
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
mov [LockedSmbiosNames],rax

lea rcx,[NameDll]
call [GetModuleHandle]
test rax,rax
jz .libraryNotFound
mov [HandleDll],rax 
lea rdx,[NameFunctionEnum]
mov rcx,[HandleDll]
call [GetProcAddress]
test rax,rax
jz .functionNotFound
mov [_EnumSystemFirmwareTables],rax
lea rdx,[NameFunctionGet]
mov rcx,[HandleDll]
call [GetProcAddress]
test rax,rax
jz .functionNotFound
mov [_GetSystemFirmwareTable],rax  

xor r8d,r8d
xor edx,edx
mov ebp,'BMSR'
mov ecx,ebp
call [_EnumSystemFirmwareTables]
test rax,rax
jz .smbiosNotFound
cmp rax,LIMIT_ENUM
ja .memoryFailed 

mov r9d,PAGE_READWRITE
mov r8d,MEM_COMMIT + MEM_RESERVE
mov edx,LIMIT_TOTAL
xor ecx,ecx
call [VirtualAlloc]
test rax,rax
jz .memoryFailed
mov [AllocationBase],rax

;---------- Start of read SMBIOS binary data procedure -------------------------;

mov r8d,LIMIT_ENUM
mov rdx,rax
mov ecx,ebp
call [_EnumSystemFirmwareTables]
test rax,rax
jz .smbiosFailed
cmp rax,LIMIT_ENUM
ja .memoryFailed 
test al,00000011b
jnz .smbiosFailed 
shr eax,2
jz .smbiosFailed
mov [ScanCounter],eax

; TODO. Check for incompatible SMBIOS configuration. EAX = 4, DWORD [EBX] = 0.
; TODO. Review limits, addends of LIMIT_TOTAL.
; TODO. Change registers for optimizing.
; TODO. BASE_ENUM not used.

mov r9d,LIMIT_TABLE
mov rbx,[AllocationBase]
lea rsi,[rbx + BASE_TABLE]
mov r8,rsi
xor edx,edx
mov ecx,ebp
call [_GetSystemFirmwareTable]
test rax,rax
jz .smbiosFailed
cmp rax,LIMIT_TABLE
ja .memoryFailed 
mov ax,word [rsi + DATA_HEADER.versionMajor]
mov word [SmbiosVersionMajor],ax
mov ecx,[rsi + DATA_HEADER.dataLength]
mov [SmbiosDataLength],ecx

;---------- Start of text generation procedure --------------------------------;

mov [ScanBase],rsi         ; RSI = Pointer to SMBIOS structures buffer start
lea rbp,[rsi + rcx + 8]    ; RBP = Pointer to SMBIOS strucrures buffer end ( first byte after )
lea rdi,[rbx + BASE_LIST]  ; RDI = Pointer to text block for viewer.
mov qword [INFO_BUFFER + BUFFER_SMBIOS_LIST],rdi

mov rdx,rdi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push rsi rdi
lea rdi,[rdx + 01]
mov rax,rsi
sub rax,[ScanBase]
call HexPrint32
mov byte [rdx + 14],'-'
mov byte [rdx + 21],'8'
lea rdi,[rdx + 29]
mov rdx,rsi
mov ax,STRING_SMBIOS_VERSION 
call IndexString
call StringWrite
movzx eax,byte [rdx + DATA_HEADER.versionMajor]
mov bl,0
call DecimalPrint32
mov al,'.'
stosb 
movzx eax,byte [rdx + DATA_HEADER.versionMinor]
call DecimalPrint32
mov ax,STRING_SMBIOS_METHOD
call IndexString
call StringWrite
movzx eax,byte [rdx + DATA_HEADER.method]
call DecimalPrint32
mov ax,STRING_SMBIOS_DMI_REV
call IndexString
call StringWrite
movzx eax,byte [rdx + DATA_HEADER.revisionDmi]
call DecimalPrint32
mov ax,STRING_SMBIOS_LENGTH
call IndexString
call StringWrite
mov eax,[rdx + DATA_HEADER.dataLength]
call DecimalPrint32
mov ax,STRING_SMBIOS_BYTES
call IndexString
call StringWrite
pop rdi rsi

add rsi,8
mov ax,0A0Dh
stosw

.smbiosList:
mov rbx,rsi
cmp byte [rsi],127  ; TODO. Check code 127 terminator or not, use size limit only ?
je .endSmbiosList 
push rbx
mov rdx,rdi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push rdi
lea rdi,[rdx + 01]
mov rax,rsi
sub rax,[ScanBase]
call HexPrint32
lea rdi,[rdx + 14]
movzx eax,byte [rsi + 00]
mov bl,0
mov bh,al
call DecimalPrint32
lea rdi,[rdx + 21]
movzx eax,byte [rsi + 01]
push rax
call DecimalPrint32
pop rax
add rsi,rax
lea rdi,[rdx + 29]
movzx ax,bh
push rsi
cmp ax,SMBIOS_UNKNOWN_TYPE
jbe @f
mov ax,SMBIOS_UNKNOWN_TYPE
@@:
call IndexStringSmbios
call StringWrite 
pop rsi rdi rbx

.stringsWrite:
cmp byte [rsi],0
jne .stringNonEmpty
inc rsi
jmp .stringDone
.stringNonEmpty:
mov rdx,rdi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push rdi
lea rdi,[rdx + 29]
call StringWriteSmbios
pop rdi
.stringDone:
cmp byte [rsi],0 
jne .stringsWrite 

mov ax,0A0Dh
stosw

.dumpWrite:
cmp rbx,rsi
ja .endLine
mov rdx,rdi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push rdi
lea rdi,[rdx + 29]
mov ecx,16
.dumpLine:
cmp rbx,rsi
ja .endByte
mov al,[rbx]
inc rbx
call HexPrint8
mov al,' '
stosb
loop .dumpLine
stc
.endByte:
pop rdi
.endLine:
jb .dumpWrite

mov ax,0A0Dh
stosw
inc rsi
cmp rsi,rbp
jb .smbiosList 
.endSmbiosList:
mov al,0
stosb

;---------- End of text generation procedure ----------------------------------;

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
test rax,rax
jz .memoryFailed 
xor ecx,ecx           
call [ExitProcess]
.guiFailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgGuiFailed]
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
mov ecx,1           
call [ExitProcess]
.libraryNotFound:
lea rbx,[MsgLibraryNotFound]
jmp .failed
.functionNotFound:
lea rbx,[MsgFunctionNotFound]
jmp .failed
.memoryFailed:
lea rbx,[MsgMemoryFailed]
jmp .failed
.smbiosFailed:
lea rbx,[MsgSmbiosFailed]
jmp .failed
.smbiosNotFound:
lea rbx,[MsgSmbiosNotFound]
.failed:
mov r9d,MB_ICONERROR
xor r8d,r8d
mov rdx,rbx
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
mov ecx,2           
call [ExitProcess]

ReleaseMemoryHelper:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov r8d,MEM_RELEASE
xor edx,edx
mov rcx,[AllocationBase]
jrcxz @f
call [VirtualFree]
@@:
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
mov edx,IDR_UP_LIST
call FontHelper
mov edx,IDR_TEXT_LIST
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

StringWriteSmbios:
cld
.copy:
lodsb
cmp al,0
je .exit
cmp al,' '
jb .change
cmp al,'z'
jbe .store 
.change:
mov al,'.'
.store:
stosb
jmp .copy
.exit:
ret

IndexString:
mov rsi,[LockedStrings]
IndexStringEntry:
cld
movzx rcx,ax
jrcxz .stop
.cycle:
lodsb
cmp al,0
jne .cycle
loop .cycle
.stop:
ret

IndexStringSmbios:
mov rsi,[LockedSmbiosNames]
jmp IndexStringEntry

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

section '.data' data readable writeable
MsgGuiFailed               DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound         DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound        DB  'OS not supports firmware access.'  , 0
MsgSmbiosNotFound          DB  'SMBIOS not found.'                 , 0
MsgMemoryFailed            DB  'Memory allocation error.'          , 0 
MsgSmbiosFailed            DB  'SMBIOS error.'                     , 0
NameDll                    DB  'KERNEL32.DLL'                      , 0
NameFunctionEnum           DB  'EnumSystemFirmwareTables'          , 0
NameFunctionGet            DB  'GetSystemFirmwareTable'            , 0
AppControl                 INITCOMMONCONTROLSEX  8, 0
ProcBinders                DQ  BindString
                           DQ  BindInfo
                           DQ  BindBig
                           DQ  BindBool
                           DQ  BindCombo
ProcCombo                  DQ  BindComboStopOn
                           DQ  BindComboStopOff 
                           DQ  BindComboCurrent
                           DQ  BindComboAdd
                           DQ  BindComboInactive
AllocationBase             DQ  0
HandleThis                 DQ  ? 
HandleIcon                 DQ  ?
LockedStrings              DQ  ?
LockedBinders              DQ  ?
LockedSmbiosNames          DQ  ?
HandleFont                 DQ  ?
HandleDll                  DQ  ?
_EnumSystemFirmwareTables  DQ  ?
_GetSystemFirmwareTable    DQ  ?  
ScanBase                   DQ  ?
ScanCounter                DD  ?
SmbiosDataLength           DD  ?
SmbiosVersionMajor         DB  ?
SmbiosVersionMinor         DB  ?
align 4096 
INFO_BUFFER                DB  INFO_BUFFER_SIZE DUP (?)
MISC_BUFFER                DB  MISC_BUFFER_SIZE DUP (?)  

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
dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU, 0, 0, 'System monospace', 18
dialogitem 'BUTTON', '', IDR_REPORT       , 241, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_BINARY       , 280, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_CANCEL       , 319, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'EDIT'  , '', IDR_UP_LIST      ,  3,    5, 354,  10, WS_VISIBLE + WS_BORDER + ES_READONLY 
dialogitem 'EDIT'  , '', IDR_TEXT_LIST    ,  3,   18, 354, 200, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
enddialog
resource raws, ID_GUI_STRINGS  , LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS  , LANG_ENGLISH + SUBLANG_DEFAULT, guibinders, \
               ID_SMBIOS_NAMES , LANG_ENGLISH + SUBLANG_DEFAULT, smbiosnames
resdata guistrings
DB  'SMBIOS structures list (x64 v0.01)'    , 0
DB  'System monospace bold'                 , 0
DB  'Report'                                , 0
DB  'Binary'                                , 0
DB  'Cancel'                                , 0
DB  ' Offset(h) | Type | Length | Details'  , 0 
DB  'Version '                              , 0
DB  ', method='                             , 0
DB  ', DMIrev='                             , 0
DB  ', Length='                             , 0
DB  ' bytes'                                , 0
DB  0
endres
resdata guibinders
BIND_STRING  STRING_REPORT         , IDR_REPORT
BIND_STRING  STRING_BINARY         , IDR_BINARY
BIND_STRING  STRING_CANCEL         , IDR_CANCEL
BIND_STRING  STRING_UP_LIST        , IDR_UP_LIST
BIND_BIG     BUFFER_SMBIOS_LIST    , IDR_TEXT_LIST
BIND_STOP
endres

resdata smbiosnames
DB  'BIOS Information'                          , 0
DB  'System Info'                               , 0
DB  'Baseboard'                                 , 0
DB  'Chassis'                                   , 0
DB  'Processor'                                 , 0
DB  'Memory Controller'                         , 0
DB  'Memory Module'                             , 0
DB  'Cache Information'                         , 0
DB  'Port Connector'                            , 0
DB  'System Slots'                              , 0
DB  'On Board Devices'                          , 0
DB  'OEM Strings'                               , 0
DB  'System Configuration Options'              , 0
DB  'BIOS Language'                             , 0
DB  'Group Associations'                        , 0
DB  'System Event Log'                          , 0
DB  'Physical Memory Array'                     , 0
DB  'Memory Device'                             , 0
DB  '32-Bit Memory Error'                       , 0
DB  'Memory Array Mapped Address'               , 0
DB  'Memory Device Mapped Address'              , 0
DB  'Built-in Pointing Device'                  , 0
DB  'Portable Battery'                          , 0
DB  'System Reset'                              , 0
DB  'Hardware Security'                         , 0
DB  'System Power Controls'                     , 0
DB  'Voltage Probe'                             , 0
DB  'Cooling Device'                            , 0
DB  'Temperature Probe'                         , 0
DB  'Electrical Current Probe'                  , 0
DB  'Out-of-Band Remote Access'                 , 0
DB  'Boot Integrity Services (BIS) Entry Point' , 0
DB  'System Boot Information'                   , 0
DB  '64-Bit Memory Error'                       , 0
DB  'Management Device'                         , 0
DB  'Management Device Component'               , 0
DB  'Management Device Threshold Data'          , 0
DB  'Memory Channel'                            , 0
DB  'IPMI Device Information'                   , 0
DB  'System Power Supply'                       , 0
DB  'Additional Information'                    , 0
DB  'Onboard Devices Extended'                  , 0
DB  'Management Controller Host Interface'      , 0
DB  'TPM Device'                                , 0
DB  'Processor Additional Information'          , 0
DB  'UNKNOWN structure type'                    , 0
endres

resource icons, ID_EXE_ICON, LANG_NEUTRAL, exeicon
resource gicons, ID_EXE_ICONS, LANG_NEUTRAL, exegicon
icon exegicon, exeicon, 'images\fasmicon64.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="SMBIOS structures viewer"'
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

