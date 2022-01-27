; TODO. Write header: version, block length, structures count.
; TODO. Optimize text generating cycle.

include 'win32a.inc'
CLEARTYPE_QUALITY     = 5
ID_MAIN               = 100
ID_EXE_ICON           = 101
ID_EXE_ICONS          = 102
ID_GUI_STRINGS        = 103
ID_GUI_BINDERS        = 104
ID_SMBIOS_NAMES       = 105
IDR_UP_LIST           = 200
IDR_TEXT_LIST         = 201
IDR_REPORT            = 202
IDR_BINARY            = 203
IDR_CANCEL            = 204
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

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

cld
push AppControl
call [InitCommonControlsEx]
test eax,eax
jz .guiFailed
push 0
call [GetModuleHandle]
test eax,eax
jz .guiFailed
mov [HandleThis],eax

push ID_EXE_ICONS
push eax 
call [LoadIcon]
test eax,eax
jz .guiFailed
mov [HandleIcon],eax

push RT_RCDATA
push ID_GUI_STRINGS
push [HandleThis]
call [FindResource]                
test eax,eax
jz .guiFailed 
push eax
push [HandleThis]
call [LoadResource] 
test eax,eax
jz .guiFailed
push eax
call [LockResource]  
test eax,eax
jz .guiFailed
mov [LockedStrings],eax

push RT_RCDATA
push ID_GUI_BINDERS
push [HandleThis]
call [FindResource]                
test eax,eax
jz .guiFailed
push eax
push [HandleThis]
call [LoadResource] 
test eax,eax
jz .guiFailed
push eax
call [LockResource]  
test eax,eax
jz .guiFailed
mov [LockedBinders],eax

push RT_RCDATA
push ID_SMBIOS_NAMES
push [HandleThis]
call [FindResource]                
test eax,eax
jz .guiFailed
push eax
push [HandleThis]
call [LoadResource] 
test eax,eax
jz .guiFailed
push eax
call [LockResource]  
test eax,eax
jz .guiFailed
mov [LockedSmbiosNames],eax

push NameDll
call [GetModuleHandle]
test eax,eax
jz .libraryNotFound
mov [HandleDll],eax 
push NameFunctionEnum
push [HandleDll]
call [GetProcAddress]
test eax,eax
jz .functionNotFound
mov [_EnumSystemFirmwareTables],eax
push NameFunctionGet
push [HandleDll]
call [GetProcAddress]
test eax,eax
jz .functionNotFound
mov [_GetSystemFirmwareTable],eax  

push 0
push 0
mov ebp,'BMSR'
push ebp
call [_EnumSystemFirmwareTables]
test eax,eax
jz .smbiosNotFound
cmp eax,LIMIT_ENUM
ja .memoryFailed 

push PAGE_READWRITE
push MEM_COMMIT + MEM_RESERVE
push LIMIT_TOTAL
push 0
call [VirtualAlloc]
test eax,eax
jz .memoryFailed
mov [AllocationBase],eax

;---------- Start of read SMBIOS binary data procedure -------------------------;

push LIMIT_ENUM
xchg ebx,eax
push ebx
push ebp
call [_EnumSystemFirmwareTables]
test eax,eax
jz .smbiosFailed
cmp eax,LIMIT_ENUM
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

push LIMIT_TABLE
mov ebx,[AllocationBase]
lea esi,[ebx + BASE_TABLE]
push esi
push 0
push ebp
call [_GetSystemFirmwareTable]
test eax,eax
jz .smbiosFailed
cmp eax,LIMIT_TABLE
ja .memoryFailed 
mov ax,word [esi + DATA_HEADER.versionMajor]
mov word [SmbiosVersionMajor],ax
mov ecx,[esi + DATA_HEADER.dataLength]
mov [SmbiosDataLength],ecx

;---------- Start of text generation procedure --------------------------------;

mov [ScanBase],esi         ; ESI = Pointer to SMBIOS structures buffer start
lea ebp,[esi + ecx + 8]    ; EBP = Pointer to SMBIOS strucrures buffer end ( first byte after )
lea edi,[ebx + BASE_LIST]  ; EDI = Pointer to text block for viewer.
mov dword [INFO_BUFFER + BUFFER_SMBIOS_LIST],edi

mov edx,edi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push esi edi
lea edi,[edx + 01]
mov eax,esi
sub eax,[ScanBase]
call HexPrint32
mov byte [edx + 14],'-'
mov byte [edx + 21],'8'
lea edi,[edx + 29]
mov edx,esi
mov ax,STRING_SMBIOS_VERSION 
call IndexString
call StringWrite
movzx eax,byte [edx + DATA_HEADER.versionMajor]
mov bl,0
call DecimalPrint32
mov al,'.'
stosb 
movzx eax,byte [edx + DATA_HEADER.versionMinor]
call DecimalPrint32
mov ax,STRING_SMBIOS_METHOD
call IndexString
call StringWrite
movzx eax,byte [edx + DATA_HEADER.method]
call DecimalPrint32
mov ax,STRING_SMBIOS_DMI_REV
call IndexString
call StringWrite
movzx eax,byte [edx + DATA_HEADER.revisionDmi]
call DecimalPrint32
mov ax,STRING_SMBIOS_LENGTH
call IndexString
call StringWrite
mov eax,[edx + DATA_HEADER.dataLength]
call DecimalPrint32
mov ax,STRING_SMBIOS_BYTES
call IndexString
call StringWrite
pop edi esi

add esi,8
mov ax,0A0Dh
stosw

.smbiosList:
mov ebx,esi
cmp byte [esi],127  ; TODO. Check code 127 terminator or not, use size limit only ?
je .endSmbiosList 
push ebx
mov edx,edi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push edi
lea edi,[edx + 01]
mov eax,esi
sub eax,[ScanBase]
call HexPrint32
lea edi,[edx + 14]
movzx eax,byte [esi + 00]
mov bl,0
mov bh,al
call DecimalPrint32
lea edi,[edx + 21]
movzx eax,byte [esi + 01]
push eax
call DecimalPrint32
pop eax
add esi,eax
lea edi,[edx + 29]
movzx ax,bh
push esi
cmp ax,SMBIOS_UNKNOWN_TYPE
jbe @f
mov ax,SMBIOS_UNKNOWN_TYPE
@@:
call IndexStringSmbios
call StringWrite 
pop esi edi ebx

.stringsWrite:
cmp byte [esi],0
jne .stringNonEmpty
inc esi
jmp .stringDone
.stringNonEmpty:
mov edx,edi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push edi
lea edi,[edx + 29]
call StringWriteSmbios
pop edi
.stringDone:
cmp byte [esi],0 
jne .stringsWrite 

mov ax,0A0Dh
stosw

.dumpWrite:
cmp ebx,esi
ja .endLine
mov edx,edi
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push edi
lea edi,[edx + 29]
mov ecx,16
.dumpLine:
cmp ebx,esi
ja .endByte
mov al,[ebx]
inc ebx
call HexPrint8
mov al,' '
stosb
loop .dumpLine
stc
.endByte:
pop edi
.endLine:
jb .dumpWrite

mov ax,0A0Dh
stosw
inc esi
cmp esi,ebp
jb .smbiosList 
.endSmbiosList:
mov al,0
stosb

;---------- End of text generation procedure ----------------------------------;

push 0 0 
push DialogProc
push HWND_DESKTOP
push ID_MAIN
push [HandleThis]  
call [DialogBoxParam] 
test eax,eax
jz .guiFailed 
cmp eax,-1
je .guiFailed
.ok:
push 0           
call [ExitProcess]
.guiFailed:
push MB_ICONERROR
push 0
push MsgguiFailed 
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
.memoryFailed:
lea ebx,[MsgMemoryFailed]
jmp .failed
.smbiosFailed:
lea ebx,[MsgSmbiosFailed]
jmp .failed
.smbiosNotFound:
lea ebx,[MsgSmbiosNotFound]
.failed:
push MB_ICONERROR
push 0
push ebx
push 0
call [MessageBox]  
call ReleaseMemoryHelper
push 2           
call [ExitProcess]

ReleaseMemoryHelper:
mov ecx,[AllocationBase]
jecxz @f
push MEM_RELEASE
push 0
push ecx
call [VirtualFree]
@@:
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
mov edx,IDR_UP_LIST
call FontHelper
mov edx,IDR_TEXT_LIST
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
mov esi,[LockedStrings]
IndexStringEntry:
cld
movzx ecx,ax
jecxz .stop
.cycle:
lodsb
cmp al,0
jne .cycle
loop .cycle
.stop:
ret

IndexStringSmbios:
mov esi,[LockedSmbiosNames]
jmp IndexStringEntry

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

section '.data' data readable writeable
MsgguiFailed               DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound         DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound        DB  'OS not supports firmware access.'  , 0
MsgSmbiosNotFound          DB  'SMBIOS not found.'                 , 0
MsgMemoryFailed            DB  'Memory allocation error.'          , 0 
MsgSmbiosFailed            DB  'SMBIOS error.'                     , 0
NameDll                    DB  'KERNEL32.DLL'                      , 0
NameFunctionEnum           DB  'EnumSystemFirmwareTables'          , 0
NameFunctionGet            DB  'GetSystemFirmwareTable'            , 0
AppControl                 INITCOMMONCONTROLSEX  8, 0
ProcBinders                DD  BindString
                           DD  BindInfo
                           DD  BindBig
                           DD  BindBool
                           DD  BindCombo
ProcCombo                  DD  BindComboStopOn
                           DD  BindComboStopOff 
                           DD  BindComboCurrent
                           DD  BindComboAdd
                           DD  BindComboInactive
AllocationBase             DD  0
HandleThis                 DD  ? 
HandleIcon                 DD  ?
LockedStrings              DD  ?
LockedBinders              DD  ?
LockedSmbiosNames          DD  ?
HandleFont                 DD  ?
HandleDll                  DD  ?
_EnumSystemFirmwareTables  DD  ?
_GetSystemFirmwareTable    DD  ?
ScanBase                   DD  ?
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
DB  'SMBIOS structures list (ia32 v0.01)'   , 0
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
icon exegicon, exeicon, 'images\fasmicon32.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="SMBIOS structures viewer"'
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

