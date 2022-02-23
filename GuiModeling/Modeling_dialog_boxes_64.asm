;------------------------------------------------------------------------------;
;                                                                              ;
;             Dialog boxes minimal sample. x64 variant for FASM.               ; 
;                                                                              ;
;   https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes   ;
;   https://habr.com/ru/post/577372/                                           ;
;   http://old-dos.ru/books/7/f/5/WindowApplicationFasm.pdf                    ;
;                                                                              ;
;------------------------------------------------------------------------------;

include 'win64a.inc'
IDI_EXE_ICON  = 100
IDI_EXE_ICONS = 101
IDD_DIALOG_1  = 102
IDD_DIALOG_2  = 103
IDD_DIALOG_3  = 104
IDB_CHILD_1   = 105
IDB_CHILD_2   = 106
IDB_EXIT      = 107
RESOURCE_DESCRIPTION  EQU 'Dialog Boxes sample (x64)'
RESOURCE_VERSION      EQU '0.0.0.0'
RESOURCE_COMPANY      EQU 'https://github.com/manusov'
RESOURCE_COPYRIGHT    EQU '(C) 2022 Ilya Manusov'

format PE64 GUI
entry start
section '.code' code readable executable
start: 

sub rsp,8*5
cld
lea rcx,[appCtrl]
call [InitCommonControlsEx]
test rax,rax
jz .exit
xor ecx,ecx
call [GetModuleHandle] 
test rax,rax
jz .exit
mov [hInstance],rax 
 
xor edx,edx
push rdx rdx
lea r9,[DialogProc1]
mov r8d,HWND_DESKTOP
mov edx,IDD_DIALOG_1
xchg rcx,rax
sub rsp,32
call [CreateDialogParam]
add rsp,32 + 16
test rax,rax
jz .exit
mov [hWinMain],rax
xchg rbx,rax

lea rsi,[msg]
.waitMessage:
xor r9d,r9d
xor r8d,r8d
xor edx,edx
mov rcx,rsi
call [GetMessage]
test eax,eax
jz .exit
mov rdx,rsi
mov rcx,[hWinChild]
jrcxz .yetNoChild
call [IsDialogMessage]
test eax,eax
jnz .waitMessage
.yetNoChild:
mov rdx,rsi
mov rcx,rbx
call [IsDialogMessage]
jmp .waitMessage

.exit:
xor ecx,ecx           
call [ExitProcess]

DialogProc1:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
cmp edx,WM_COMMAND
je .wmcommand
cmp edx,WM_CLOSE
jne statusZero
.wmclose:
call [DestroyWindow]
xor ecx,ecx
call [PostQuitMessage]
jmp statusZero
.wmcommand:
movzx eax,r8w
cmp eax,IDB_EXIT
je .wmclose
cmp eax,IDB_CHILD_1
je .openModal
cmp eax,IDB_CHILD_2
je .openModeless
jmp statusZero
.openModal:
xor eax,eax
push rax rax
lea r9,[DialogProc2]
mov r8,rcx
mov edx,IDD_DIALOG_2
mov rcx,[hInstance]
sub rsp,32
call [DialogBoxParam]
add rsp,32 + 16
jmp statusZero
.openModeless:
xor eax,eax
push rax rax
lea r9,[DialogProc3]
mov r8,rcx
mov edx,IDD_DIALOG_3
mov rcx,[hInstance]
sub rsp,32
call [CreateDialogParam]
add rsp,32 + 16
mov [hWinChild],rax
statusZero:
xor eax,eax
mov rsp,rbp
pop rbp
ret

DialogProc2:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
cmp edx,WM_CLOSE
jne statusZero
xor edx,edx
call [EndDialog]
jmp statusZero

DialogProc3:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
cmp edx,WM_CLOSE
jne statusZero
call [DestroyWindow]
jmp statusZero

section '.data' data readable writeable
appCtrl  INITCOMMONCONTROLSEX  8, 0
msg      MSG                   ?
align 8
hInstance  DQ  ?
hWinMain   DQ  ?
hWinChild  DQ  ?

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
dialog mainDialog, 'Application main window (x64)', 100, 100, 200, 150, \
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
icon exegicon, exeicon, 'images\fasm64.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="Hello Dialog Boxes"'
db '    processorArchitecture="amd64"'
db '    version="1.0.0.0"'
db '    type="win32"/>'
db '<description>Hello Dialog Boxes</description>'
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
resource version, 1, LANG_NEUTRAL, version_info
versioninfo version_info, \ 
            VOS__WINDOWS32, VFT_DLL, VFT2_UNKNOWN, LANG_NEUTRAL, 0, \
'FileDescription' , RESOURCE_DESCRIPTION , \
'FileVersion'     , RESOURCE_VERSION     , \
'CompanyName'     , RESOURCE_COMPANY     , \
'LegalCopyright'  , RESOURCE_COPYRIGHT
