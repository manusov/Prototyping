include 'win64a.inc'
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
mov ecx,'IPCA'
call [_EnumSystemFirmwareTables]
test rax,rax
jz .acpiNotFound
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

mov r8d,LIMIT_TABLE
mov rdx,[AllocationBase]
mov rbp,rdx
mov ecx,'IPCA'
call [_EnumSystemFirmwareTables]
test rax,rax
jz .acpiFailed
cmp rax,LIMIT_ENUM
ja .memoryFailed 
test al,00000011b
jnz .acpiFailed 
shr rax,2
jz .acpiFailed
xchg r12,rax
lea rsi,[rbp + BASE_ENUM]
lea rdi,[rbp + BASE_LIST]
lea rbx,[rbp + BASE_SUMMARY] 
lea rax,[INFO_BUFFER]
mov [rax + BUFFER_ACPI_LIST],rdi
mov [rax + BUFFER_ACPI_SUMMARY],rbx
.scanAcpiTables:
lodsd
mov r9,LIMIT_TABLE
lea r8,[rbp + BASE_TABLE]
mov edx,eax
mov ecx,'IPCA'
call [_GetSystemFirmwareTable]
test rax,rax
jz .acpiFailed
cmp rax,LIMIT_TABLE
ja .memoryFailed 
mov rdx,rdi
lea rax,[rbp + BASE_LIST + LIMIT_LIST - 80]
cmp rdi,rax
jae .memoryFailed
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
push rsi rdi
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
mov rdi,rbx
mov rdx,rbx
lea rax,[rbp + BASE_SUMMARY + LIMIT_SUMMARY - 80]
cmp rdi,rax
jae .memoryFailed
mov ecx,80
mov al,' '
cld
rep stosb
mov ax,0A0Dh
stosw
mov rbx,rdi
mov eax,00040100h
call StringCopyHelper
mov ax,', '
stosw
mov ax,STRING_ACPI_DATA
call IndexString
lea rdx,[rbp + BASE_TABLE]
mov edx,[rdx] 
.findAcpiBase:
cmp byte [rsi],0
je .endAcpiBase
add rsi,4
cmp [rsi - 4],edx
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
pop rdi rsi
dec r12
jnz .scanAcpiTables
mov ax,0A0Dh
stosw
mov al,0
stosb
mov rdi,rbx
mov ax,0A0Dh
stosw
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
.acpiFailed:
lea rbx,[MsgAcpiFailed]
jmp .failed
.acpiNotFound:
lea rbx,[MsgAcpiNotFound]
.failed:
mov r9d,MB_ICONERROR
xor r8d,r8d
mov rdx,rbx
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
mov ecx,2           
call [ExitProcess]

StringCopyHelper:
movzx ecx,al
lea rsi,[rbp + BASE_TABLE + rcx]
movzx ecx,ah
lea rdi,[rdx + rcx]
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
push rbx
movzx ecx,ah
lea rdi,[rdx + rcx]
movzx ecx,al
movzx eax,byte [rbp + BASE_TABLE + rcx]
mov bl,0
call DecimalPrint32 
pop rbx
ret

StringHexHelper:
movzx ecx,ah
lea rdi,[rdx + rcx]
movzx ecx,al
mov eax,[rbp + BASE_TABLE + rcx]
call HexPrint32
mov al,'h'
stosb 
ret

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
mov edx,IDR_UP_SUMMARY
call FontHelper
mov edx,IDR_TEXT_SUMMARY
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
HandleFont                 DQ  ?
HandleDll                  DQ  ?
_EnumSystemFirmwareTables  DQ  ?
_GetSystemFirmwareTable    DQ  ?  
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
DB  'ACPI tables list (x64 v0.0)' , 0
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
icon exegicon, exeicon, 'images\fasmicon64.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="ACPI tables viewer"'
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

