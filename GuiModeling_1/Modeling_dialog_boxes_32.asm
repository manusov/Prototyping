;------------------------------------------------------------------------------;
;                                                                              ;
;             Dialog boxes minimal sample. ia32 variant for FASM.              ; 
;                                                                              ;
;   https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes   ;
;   https://habr.com/ru/post/577372/                                           ;
;   http://old-dos.ru/books/7/f/5/WindowApplicationFasm.pdf                    ;
;                                                                              ;
;------------------------------------------------------------------------------;

include 'win32a.inc'
IDI_EXE_ICON  = 100
IDI_EXE_ICONS = 101
IDD_DIALOG_1  = 102
IDD_DIALOG_2  = 103
IDD_DIALOG_3  = 104
IDB_CHILD_1   = 105
IDB_CHILD_2   = 106
IDB_EXIT      = 107
RESOURCE_DESCRIPTION  EQU 'Dialog Boxes sample (ia32)'
RESOURCE_VERSION      EQU '0.0.0.0'
RESOURCE_COMPANY      EQU 'https://github.com/manusov'
RESOURCE_COPYRIGHT    EQU '(C) 2022 Ilya Manusov'

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

cld
push appCtrl
call [InitCommonControlsEx]
test eax,eax
jz .exit
push 0
call [GetModuleHandle] 
test eax,eax
jz .exit
mov [hInstance],eax 

push 0
push DialogProc1
push HWND_DESKTOP
push IDD_DIALOG_1
push eax
call [CreateDialogParam]
test eax,eax
jz .exit
mov [hWinMain],eax
xchg ebx,eax

lea esi,[msg]
.waitMessage:
push 0
push 0
push 0
push esi
call [GetMessage]
test eax,eax
jz .exit
mov ecx,[hWinChild]
jecxz .yetNoChild
push esi
push ecx
call [IsDialogMessage]
test eax,eax
jnz .waitMessage
.yetNoChild:
push esi
push ebx
call [IsDialogMessage]
jmp .waitMessage

.exit:
push 0           
call [ExitProcess]

DialogProc1:
mov eax,[esp + 08]
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_CLOSE
jne statusZero
.wmclose:
push dword [esp + 04]
call [DestroyWindow]
push 0
call [PostQuitMessage]
jmp statusZero
.wmcommand:
movzx eax,word [esp + 12]
cmp eax,IDB_EXIT
je .wmclose
cmp eax,IDB_CHILD_1
je .openModal
cmp eax,IDB_CHILD_2
je .openModeless
jmp statusZero
.openModal:
push 0
push DialogProc2
push dword [esp + 12]
push IDD_DIALOG_2
push [hInstance]
call [DialogBoxParam]
jmp statusZero
.openModeless:
push 0
push DialogProc3
push dword [esp + 12]
push IDD_DIALOG_3
push [hInstance]
call [CreateDialogParam]
mov [hWinChild],eax
statusZero:
xor eax,eax
ret

DialogProc2:
cmp dword [esp + 08],WM_CLOSE
jne statusZero
push 0
push dword [esp + 08]
call [EndDialog]
jmp statusZero

DialogProc3:
cmp dword [esp + 08],WM_CLOSE
jne statusZero
push dword [esp + 04]
call [DestroyWindow]
jmp statusZero

section '.data' data readable writeable
appCtrl  INITCOMMONCONTROLSEX  8, 0
msg      MSG                   ?
align 8
hInstance  DD  ?
hWinMain   DD  ?
hWinChild  DD  ?

section '.idata' import data readable writeable
library \ 
kernel32 , 'kernel32.dll' , \
advapi32 , 'advapi32.dll' , \
user32   , 'user32.dll'   , \
comctl32 , 'comctl32.dll' , \
comdlg32 , 'comdlg32.dll' , \
gdi32    , 'gdi32.dll' 
include  'api\kernel32.inc'
include  'api\advapi32.inc'
include  'api\user32.inc'
include  'api\comctl32.inc'
include  'api\comdlg32.inc'
include  'api\gdi32.inc'

section '.rsrc' resource data readable
directory RT_DIALOG     , dialogs   , \
          RT_ICON       , icons     , \
          RT_GROUP_ICON , gicons    , \
          RT_MANIFEST   , manifests , \
          RT_VERSION    , version
resource  dialogs, \
IDD_DIALOG_1 , LANG_ENGLISH + SUBLANG_DEFAULT, mainDialog  , \
IDD_DIALOG_2 , LANG_ENGLISH + SUBLANG_DEFAULT, firstChild  , \
IDD_DIALOG_3 , LANG_ENGLISH + SUBLANG_DEFAULT, secondChild
dialog mainDialog, 'Application main window (ia32)', 100, 100, 200, 150, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem 'BUTTON', 'Open modal'   , IDB_CHILD_1, 120, 102, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open modeless', IDB_CHILD_2, 120, 117, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Exit'         , IDB_EXIT   , 120, 132, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 
dialog firstChild, 'First child window (modal)', 100, 100, 150, 100, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
enddialog 
dialog secondChild, 'Second child window (modeless)', 100, 100, 150, 100, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
enddialog 
resource icons  , IDI_EXE_ICON  , LANG_NEUTRAL , exeicon
resource gicons , IDI_EXE_ICONS , LANG_NEUTRAL , exegicon
icon exegicon, exeicon, 'images\fasm32.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="Hello Dialog Boxes"'
db '    processorArchitecture="x86"'
db '    version="1.0.0.0"'
db '    type="win32"/>'
db '<description>Hello Dialog Boxes</description>'
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
resource version, 1, LANG_NEUTRAL, version_info
versioninfo version_info, \ 
            VOS__WINDOWS32, VFT_DLL, VFT2_UNKNOWN, LANG_NEUTRAL, 0, \
'FileDescription' , RESOURCE_DESCRIPTION , \
'FileVersion'     , RESOURCE_VERSION     , \
'CompanyName'     , RESOURCE_COMPANY     , \
'LegalCopyright'  , RESOURCE_COPYRIGHT
