include 'win32a.inc'
CLEARTYPE_QUALITY   = 5
ID_MAIN             = 100
ID_EXE_ICON         = 101
ID_EXE_ICONS        = 102
ID_GUI_STRINGS      = 103
ID_GUI_BINDERS      = 104
IDR_UP_LIST         = 200
IDR_TEXT_LIST       = 201
IDR_UP_SUMMARY      = 202      
IDR_TEXT_SUMMARY    = 203
IDR_REPORT          = 204
IDR_BINARY          = 205
IDR_CANCEL          = 206
STRING_APP          = 0
STRING_FONT         = 1
STRING_REPORT       = 2
STRING_BINARY       = 3
STRING_CANCEL       = 4
STRING_UP_LIST      = 5
STRING_UP_SUMMARY   = 6
STRING_UNKNOWN      = 7
STRING_ACPI_DATA    = 8
BUFFER_ACPI_LIST    = 0
BUFFER_ACPI_SUMMARY = 8
BIND_APP_GUI        = 0
X_SIZE              = 360
Y_SIZE              = 240
INFO_BUFFER_SIZE    = 8192
MISC_BUFFER_SIZE    = 8192
LIMIT_TOTAL         = LIMIT_ENUM + LIMIT_TABLE + LIMIT_LIST + LIMIT_SUMMARY
LIMIT_ENUM          = 1024 * 128
LIMIT_TABLE         = 1024 * 1024
LIMIT_LIST          = 1024 * 256
LIMIT_SUMMARY       = 1024 * 128
BASE_ENUM           = 0
BASE_TABLE          = LIMIT_ENUM 
BASE_LIST           = LIMIT_ENUM + LIMIT_TABLE
BASE_SUMMARY        = LIMIT_ENUM + LIMIT_TABLE + LIMIT_LIST
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
push 'IPCA'
call [_EnumSystemFirmwareTables]
test eax,eax
jz .acpiNotFound
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

push LIMIT_TABLE
mov ebp,[AllocationBase]
push ebp
push 'IPCA'
call [_EnumSystemFirmwareTables]
test eax,eax
jz .acpiFailed
cmp eax,LIMIT_ENUM
ja .memoryFailed 
test al,00000011b
jnz .acpiFailed 
shr eax,2
jz .acpiFailed
mov [ScanCounter],eax
lea esi,[ebp + BASE_ENUM]
lea edi,[ebp + BASE_LIST]
lea ebx,[ebp + BASE_SUMMARY] 
lea eax,[INFO_BUFFER]
mov [eax + BUFFER_ACPI_LIST],edi
mov [eax + BUFFER_ACPI_SUMMARY],ebx
.scanAcpiTables:
lodsd
push LIMIT_TABLE
lea edx,[ebp + BASE_TABLE]
push edx
push eax
push 'IPCA'
call [_GetSystemFirmwareTable]
test eax,eax
jz .acpiFailed
cmp eax,LIMIT_TABLE
ja .memoryFailed 
mov edx,edi
lea eax,[ebp + BASE_LIST + LIMIT_LIST - 80]
cmp edi,eax
jae .memoryFailed
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push esi edi
mov eax,00040100h
call StringCopyHelper
mov eax,0006080Ah
call StringCopyHelper
mov eax,00081110h
call StringCopyHelper
mov eax,0004201Ch
call StringCopyHelper
mov ax,2D18h
call StringHexHelper
mov ax,3920h
call StringHexHelper
mov ax,4708h
call StringDecimalHelper
mov edi,ebx
mov edx,ebx
lea eax,[ebp + BASE_SUMMARY + LIMIT_SUMMARY - 80]
cmp edi,eax
jae .memoryFailed
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
mov ebx,edi
mov eax,00040100h
call StringCopyHelper
mov ax,', '
stosw
mov ax,STRING_ACPI_DATA
call IndexString
lea edx,[ebp + BASE_TABLE]
mov edx,[edx] 
.findAcpiBase:
cmp byte [esi],0
je .endAcpiBase
add esi,4
cmp [esi - 4],edx
je .foundAcpiBase
.skipAcpiBase:
lodsb
cmp al,0
jne .skipAcpiBase
jmp .findAcpiBase
.endAcpiBase:
mov ax,STRING_UNKNOWN
call IndexString
.foundAcpiBase:
call StringWrite
mov al,'.'
stosb
pop edi esi
dec [ScanCounter]
jnz .scanAcpiTables

mov ax,0A0Dh
stosw
mov al,0
stosb
mov edi,ebx
mov ax,0A0Dh
stosw
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
.memoryFailed:
lea ebx,[MsgMemoryFailed]
jmp .failed
.acpiFailed:
lea ebx,[MsgAcpiFailed]
jmp .failed
.acpiNotFound:
lea ebx,[MsgAcpiNotFound]
.failed:
push MB_ICONERROR
push 0
push ebx
push 0
call [MessageBox]  
call ReleaseMemoryHelper
push 2           
call [ExitProcess]

StringCopyHelper:
movzx ecx,al
lea esi,[ebp + BASE_TABLE + ecx]
movzx ecx,ah
lea edi,[edx + ecx]
xor ecx,ecx
shld ecx,eax,16
.copy:
lodsb
cmp al,0
je .space
cmp al,' '
jb .change
cmp al,'z'
jbe .store 
.change:
mov al,'.'
jmp .store
.space:
mov al,' '
.store:
stosb
loop .copy
ret

StringDecimalHelper:
push ebx
movzx ecx,ah
lea edi,[edx + ecx]
movzx ecx,al
movzx eax,byte [ebp + BASE_TABLE + ecx]
mov bl,0
call DecimalPrint32 
pop ebx
ret

StringHexHelper:
movzx ecx,ah
lea edi,[edx + ecx]
movzx ecx,al
mov eax,[ebp + BASE_TABLE + ecx]
call HexPrint32
mov al,'h'
stosb 
ret

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
mov edx,IDR_UP_SUMMARY
call FontHelper
mov edx,IDR_TEXT_SUMMARY
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

section '.data' data readable writeable
MsgGuiFailed               DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound         DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound        DB  'OS not supports firmware access.'  , 0
MsgAcpiNotFound            DB  'ACPI not found, check CMOS setup.' , 0
MsgMemoryFailed            DB  'Memory allocation error.'          , 0 
MsgAcpiFailed              DB  'ACPI error.'                       , 0
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
HandleFont                 DD  ?
HandleDll                  DD  ?
_EnumSystemFirmwareTables  DD  ?
_GetSystemFirmwareTable    DD  ?
ScanCounter                DD  ?  
align 4096 
INFO_BUFFER                DB  INFO_BUFFER_SIZE DUP (?)
NISC_BUFFER                DB  MISC_BUFFER_SIZE DUP (?)  

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
dialogitem 'BUTTON', '', IDR_REPORT       , 241, 224,  38, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_BINARY       , 280, 224,  38, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_CANCEL       , 319, 224,  38, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'EDIT'  , '', IDR_UP_LIST      ,  3,    5, 354, 10, WS_VISIBLE + WS_BORDER + ES_READONLY 
dialogitem 'EDIT'  , '', IDR_TEXT_LIST    ,  3,   18, 354, 90, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem 'EDIT'  , '', IDR_UP_SUMMARY   ,  3,  114, 354, 10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem 'EDIT'  , '', IDR_TEXT_SUMMARY ,  3,  127, 354, 90, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
enddialog
resource raws, ID_GUI_STRINGS, LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS, LANG_ENGLISH + SUBLANG_DEFAULT, guibinders
resdata guistrings
DB  'ACPI tables list (ia32 v0.0)' , 0
DB  'System monospace bold'             , 0
DB  'Report', 0
DB  'Binary', 0
DB  'Cancel', 0
DB  ' Sign | OEM ID | OEM Table ID | Creator ID | OEM Rev   | Creator Rev | Rev' , 0
DB  ' Summary',0
DB  'UNKNOWN table signature'     , 0
DB  'AEST' , 'Arm Error Source'                                 , 0
DB  'APIC' , 'Multiple APIC Description'                        , 0
DB  'BDAT' , 'BIOS Data ACPI'                                   , 0
DB  'BERT' , 'Boot Error Record'                                , 0
DB  'BGRT' , 'Boot Graphics Resource'                           , 0
DB  'BOOT' , 'Simple Boot Flag'                                 , 0
DB  'CDIT' , 'Component Distance Information'                   , 0
DB  'CEDT' , 'CXL Early Discovery'                              , 0
DB  'CPEP' , 'Corrected Platform Error Polling'                 , 0
DB  'CRAT' , 'Component Resource Attribute'                     , 0
DB  'CSRT' , 'Core System Resource'                             , 0
DB  'DBGP' , 'Debug Port'                                       , 0
DB  'DMAR' , 'DMA Remapping'                                    , 0
DB  'DSDT' , 'Differentiated System Description'                , 0
DB  'DPPT' , 'DMA Protection Policy'                            , 0
DB  'DRTM' , 'Dynamic Root of Trust for Measurement'            , 0
DB  'ECDT' , 'Embedded Controller Boot Resources'               , 0
DB  'EINJ' , 'Error Injection'                                  , 0
DB  'ERST' , 'Error Record Serialization'                       , 0
DB  'ETDT' , 'Event Timer Description'                          , 0
DB  'FACP' , 'Fixed ACPI Description'                           , 0
DB  'FACS' , 'Firmware ACPI Control Structure'                  , 0
DB  'FPDT' , 'Firmware Performance Data'                        , 0
DB  'GTDT' , 'Generic Timer Description'                        , 0
DB  'HEST' , 'Hardware Error Source'                            , 0
DB  'HPET' , 'High Precision Event Timer'                       , 0
DB  'IBFT' , 'iSCSI Boot Firmware'                              , 0
DB  'IORT' , 'I/O Remapping'                                    , 0
DB  'IVRS' , 'I/O Virtualization Reporting'                     , 0
DB  'LPIT' , 'Low Power Idle'                                   , 0
DB  'MCFG' , 'Memory Mapped Configuration'                      , 0
DB  'MCHI' , 'Management Controller Host Interface'             , 0
DB  'MPAM' , 'Arm Memory Partitioning and Monitoring'           , 0
DB  'MSDM' , 'Microsoft Data Management'                        , 0
DB  'MSCT' , 'Maximum System Characteristics'                   , 0
DB  'MPST' , 'Memory Power State'                               , 0
DB  'NFIT' , 'NVDIMM Firmware Interface'                        , 0
DB  'OEMx' , 'OEM Specific Information'                         , 0
DB  'PHAT' , 'Platform Health Assessment'                       , 0
DB  'PCCT' , 'Platform Communications Channel'                  , 0
DB  'PMTT' , 'Platform Memory Topology'                         , 0
DB  'PSDT' , 'Persistent System Description'                    , 0
DB  'PRMT' , 'Platform Runtime Mechanism Table'                 , 0
DB  'RASF' , 'ACPI RAS Feature'                                 , 0
DB  'RGRT' , 'Regulatory Graphics Resource Table'               , 0
DB  'RSDT' , 'Root System Description'                          , 0
DB  'SBST' , 'Smart Battery Specification'                      , 0
DB  'SDEI' , 'Software Delegated Exceptions Interface'          , 0
DB  'SDEV' , 'Secure Devices Table'                             , 0
DB  'SLIC' , 'Microsoft Software Licensing'                     , 0
DB  'SLIT' , 'System Locality Distance Information'             , 0
DB  'SRAT' , 'Static/System Resource Affinity'                  , 0
DB  'SSDT' , 'Secondary System Description'                     , 0
DB  'SPCR' , 'Serial Port Console Redirection'                  , 0
DB  'SPMI' , 'Server Platform Management Interface'             , 0
DB  'STAO' , '_STA Override'                                    , 0
DB  'SVKL' , 'Storage Volume Key Data'                          , 0
DB  'TCPA' , 'Trusted Computing Platform Alliance Capabilities' , 0
DB  'TPM2' , 'Trusted Platform Module 2'                        , 0
DB  'UEFI' , 'Unified Extensible Firmware Interface'            , 0
DB  'WAET' , 'Windows ACPI Emulated Devices'                    , 0
DB  'WDAT' , 'Watch Dog Action Table'                           , 0
DB  'WDRT' , 'Watch Dog Resource Table'                         , 0
DB  'WPBT' , 'Windows Platform Binary'                          , 0
DB  'WSMT' , 'Windows Security Mitigations'                     , 0
DB  'XENV' , 'Xen Project'                                      , 0
DB  'XSDT' , 'Extended System Description'                      , 0
DB  0
endres
resdata guibinders
BIND_STRING  STRING_REPORT       , IDR_REPORT
BIND_STRING  STRING_BINARY       , IDR_BINARY
BIND_STRING  STRING_CANCEL       , IDR_CANCEL
BIND_STRING  STRING_UP_LIST      , IDR_UP_LIST
BIND_STRING  STRING_UP_SUMMARY   , IDR_UP_SUMMARY
BIND_BIG     BUFFER_ACPI_LIST    , IDR_TEXT_LIST
BIND_BIG     BUFFER_ACPI_SUMMARY , IDR_TEXT_SUMMARY
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
db '    name="ACPI tables viewer"'
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

