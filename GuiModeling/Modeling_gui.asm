; TODO.
; Search for mismatches with original C++ application at Litvinenko Project.
; Optimize by registers usage instead memory variables.
; Optimize by remove redundant registers reloads.
; Question about resources:
;  - How create modeless (non modal) window by resources ?
;  - Member WNDCLASSEX.lpszClassName can be 16-bit resource id ? 

include 'win64w.inc'
MAX_LOADSTRING    = 100 
X_BASE            = 300
Y_BASE            = 200
X_SIZE            = 350
Y_SIZE            = 300
ID_STRINGS        = 1
IDS_APP_TITLE     = 2
IDS_CLASS_NAME    = 3
IDI_EXE_ICON      = 100
IDI_SMALL_ICON    = 101
IDI_EXE_GICON     = 102
IDI_SMALL_GICON   = 103
IDR_MENU          = 200
IDM_SAVE_REPORT   = 201
IDM_SAVE_IMAGE    = 202
IDM_LOAD_REPORT   = 203
IDM_EXIT          = 204
IDM_ABOUT         = 205
IDD_ABOUTBOX      = 300 
IDC_ABOUT_NAME    = 301
IDB_ABOUT_OK      = 302
IDA_MAIN          = 400

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
mov [hInst],rax
mov rbx,rax
mov rcx,rbx
mov edx,IDS_APP_TITLE
lea r8,[nameTitle]
mov r9d,MAX_LOADSTRING
call [LoadString]
test rax,rax
jz .exit
mov rcx,rbx
mov edx,IDS_CLASS_NAME
lea r8,[nameWindowClass]
mov r9d,MAX_LOADSTRING
call [LoadString]
test rax,rax
jz .exit
mov rcx,rbx
mov edx,IDI_EXE_GICON
call [LoadIcon]
test rax,rax
jz .exit
xchg rsi,rax
mov rcx,rbx
mov edx,IDI_SMALL_GICON
call [LoadIcon]
test rax,rax
jz .exit
xchg rdi,rax
xor ecx,ecx
mov edx,IDC_ARROW
call [LoadCursor]
test rax,rax
jz .exit
xchg rbp,rax

xor eax,eax
lea rcx,[wcex]
mov [rcx + WNDCLASSEX.cbSize]        , sizeof.WNDCLASSEX
mov [rcx + WNDCLASSEX.style]         , CS_HREDRAW + CS_VREDRAW 
mov [rcx + WNDCLASSEX.lpfnWndProc]   , WndProc_Main
mov [rcx + WNDCLASSEX.cbClsExtra]    , eax
mov [rcx + WNDCLASSEX.cbWndExtra]    , eax
mov [rcx + WNDCLASSEX.hInstance]     , rbx
mov [rcx + WNDCLASSEX.hIcon]         , rsi
mov [rcx + WNDCLASSEX.hCursor]       , rbp
mov [rcx + WNDCLASSEX.hbrBackground] , COLOR_WINDOW + 1
mov [rcx + WNDCLASSEX.lpszMenuName]  , IDR_MENU  ; rax ; IDS_CLASS_NAME
mov [rcx + WNDCLASSEX.lpszClassName] , nameWindowClass
mov [rcx + WNDCLASSEX.hIconSm]       , rdi
call [RegisterClassEx]
test rax,rax
jz .exit

lea rcx,[wcex]
xor eax,eax
push rax
push rbx
push rax
push rax
push Y_SIZE
push X_SIZE
push Y_BASE
push X_BASE
mov r9d,WS_VISIBLE + WS_DLGFRAME + WS_SYSMENU    ; WS_OVERLAPPEDWINDOW
lea r8,[nameTitle]
mov rdx,[rcx + WNDCLASSEX.lpszClassName]
xor ecx,ecx
sub rsp,32
call [CreateWindowEx]
add rsp,32 + 8 * 8
test rax,rax
jz .exit

mov rcx,rbx
mov edx,SW_NORMAL
call [ShowWindow]
mov rcx,rbx
call [UpdateWindow] 

mov rcx,[hInst]
mov edx,IDA_MAIN
call [LoadAccelerators]
test rax,rax
jz .exit
mov [hAccelTable],rax 

lea rsi,[msg]
.waitMessage:
mov rcx,rsi
xor edx,edx
xor r8d,r8d
xor r9d,r9d
call [GetMessage]
cmp rax,1		     
jb .exitMessage
jne .waitMessage

mov rcx,[rsi + MSG.hwnd]
mov rdx,[hAccelTable]
mov r8,rsi
call [TranslateAccelerator]  
test rax,rax
jnz .waitMessage 

mov rcx,rsi
call [TranslateMessage]
mov rcx,rsi
call [DispatchMessage]
jmp .waitMessage
.exitMessage:

.exit:
xor ecx,ecx           
call [ExitProcess]

PARM_HWNDDLG  EQU  qword [rbp + 32 + 08 + 00]
PARM_MSG      EQU  qword [rbp + 32 + 08 + 08]
PARM_WPARAM   EQU  qword [rbp + 32 + 08 + 16]
PARM_LPARAM   EQU  qword [rbp + 32 + 08 + 24]
LOW_WPARAM    EQU  dword [rbp + 32 + 08 + 16]

WndProc_Main:
cld
push rbx rsi rdi rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov PARM_HWNDDLG,rcx 
mov PARM_MSG,rdx
mov PARM_WPARAM,r8
mov PARM_LPARAM,r9
cmp rdx,0000FFFFh
ja returnZero
xchg eax,edx
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_PAINT
je .wmpaint
cmp eax,WM_DESTROY
je .wmdestroy
jmp returnDefault
.wmcommand:
movzx eax,r8w
cmp eax,IDM_EXIT
je .menuExit
cmp eax,IDM_ABOUT
je .menuAbout
jmp returnDefault
.menuAbout:
push 0 0
lea r9,[WndProc_About]
mov r8,PARM_HWNDDLG
mov edx,IDD_ABOUTBOX
mov rcx,[hInst]
sub rsp,32
call [DialogBoxParam]
add rsp,32 + 8 * 2
jmp returnZero
.menuExit:
mov rcx,PARM_HWNDDLG
call [DestroyWindow]
jmp returnZero
.wmpaint:
mov rcx,PARM_HWNDDLG
lea rdx,[ps]
call [BeginPaint]
test rax,rax
jz @f
mov [hdc],rax
push 0
mov eax,[testStringSize]
push rax
lea r9,[testString]
mov r8d,10 ; 5
mov edx,10 ; 5
mov rcx,[hdc]
sub rsp,32
call [TextOut]
add rsp,32 + 8 * 2
mov rcx,PARM_HWNDDLG
lea rdx,[ps]
call [EndPaint]
@@:
jmp returnZero
.wmdestroy:
xor ecx,ecx 
call [PostQuitMessage]
returnZero:
xor eax,eax
jmp returnStatus
returnDefault:
mov rcx,PARM_HWNDDLG 
mov rdx,PARM_MSG
mov r8,PARM_WPARAM
mov r9,PARM_LPARAM
call [DefWindowProc]
returnStatus:
mov rsp,rbp
pop rbp rdi rsi rbx
ret

WndProc_About:
cld
push rbx rsi rdi rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov PARM_HWNDDLG,rcx 
mov PARM_MSG,rdx
mov PARM_WPARAM,r8
mov PARM_LPARAM,r9
cmp rdx,WM_INITDIALOG
je .wminitdialog
cmp rdx,WM_COMMAND
je .wmcommand
jmp returnZero
.wminitdialog:
mov r9,[wcex.hIcon]  ; hIconSm 
mov r8d,ICON_SMALL 
mov edx,WM_SETICON 
mov rcx,PARM_HWNDDLG
call [SendMessage]
mov eax,1
jmp returnStatus
.wmcommand:
movzx eax,r8w
cmp eax,IDB_ABOUT_OK
je .stopAbout 
cmp eax,IDCANCEL
je .stopAbout
jmp returnZero
.stopAbout:
mov rcx,PARM_HWNDDLG
mov rdx,r8
call [EndDialog]
mov eax,1
jmp returnStatus
  

section '.data' data readable writeable
appCtrl          INITCOMMONCONTROLSEX  8, 0
testString       DU                    'Paint this string...', 0
testStringSize   DD                    20                     
hInst            DQ                    ?
nameTitle        DU  MAX_LOADSTRING    DUP (?)
nameWindowClass  DU  MAX_LOADSTRING    DUP (?)
wcex             WNDCLASSEX            ?
msg              MSG                   ? 
ps               PAINTSTRUCT           ?
hdc              DQ                    ?
hAccelTable      DQ                    ?

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

section '.rsrc' data readable resource

directory \
RT_STRING      , dir_strings      , \
RT_ICON        , dir_icons        , \
RT_GROUP_ICON  , dir_gicons       , \
RT_MENU        , dir_menus        , \
RT_DIALOG      , dir_dialogs      , \
RT_ACCELERATOR , dir_accelerators 

resource dir_strings, ID_STRINGS, LANG_NEUTRAL + SUBLANG_DEFAULT, res_strings  
resdata res_strings
DU 0                                   ; id = 0 
DU 0                                   ; id = 1  
DU 27 , 'Hello World GUI application'  ; id = 2 
DU 7  , 'FASMGUI'                      ; id = 3 
endres

resource dir_icons , \
IDI_EXE_ICON    , LANG_NEUTRAL , res_exeicon   , \
IDI_SMALL_ICON  , LANG_NEUTRAL , res_smallicon

resource dir_gicons , \
IDI_EXE_GICON   , LANG_NEUTRAL , res_exegicon  , \
IDI_SMALL_GICON , LANG_NEUTRAL , res_smallgicon
icon res_exegicon   , res_exeicon   , 'images\fasm64.ico'
icon res_smallgicon , res_smallicon , 'images\sysinfo.ico'

resource dir_menus, IDR_MENU, LANG_ENGLISH + SUBLANG_DEFAULT, mainMenu
menu mainMenu
menuitem '&File'        , 0 , MFR_POPUP
menuitem '&Save report' , IDM_SAVE_REPORT
menuitem 'S&ave image'  , IDM_SAVE_IMAGE  , 0 , MFS_DISABLED
menuseparator
menuitem '&Load report' , IDM_LOAD_REPORT , 0 , MFS_DISABLED
menuseparator
menuitem 'E&xit'        , IDM_EXIT, MFR_END
menuitem '&Help'        , 0 , MFR_POPUP + MFR_END
menuitem '&About...'    , IDM_ABOUT, MFR_END

resource dir_dialogs,\
IDD_ABOUTBOX , LANG_ENGLISH + SUBLANG_DEFAULT, aboutBox
dialog      aboutBox, 'Program info' ,                  20,  20,  75,  50 , WS_CAPTION + WS_SYSMENU, 0, 0, 'Verdana', 10
dialogitem  'STATIC', 'GUI sample.'  , IDC_ABOUT_NAME,   7,  10, 170,  10 , WS_VISIBLE
dialogitem  'BUTTON', 'OK'           , IDB_ABOUT_OK  ,  30,  30,  38,  13 , WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 

resource dir_accelerators, IDA_MAIN, LANG_ENGLISH + SUBLANG_DEFAULT, hot_keys
accelerator hot_keys,\
FVIRTKEY + FNOINVERT + FCONTROL  , 'N' , IDM_EXIT  , \
FVIRTKEY + FNOINVERT + FALT      , 'X' , IDM_EXIT  , \
FVIRTKEY + FNOINVERT + FSHIFT    , 'H' , IDM_ABOUT
