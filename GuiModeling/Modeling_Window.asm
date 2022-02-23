;------------------------------------------------------------------------------;
;                           GUI window example.                                ; 
;         FASM x64 version of C++ Listing 3.7 ( partial functionality )        ;
;                          from Litvinenko book.                               ;
;                         ISBN 978-5-9775-0600-7.                              ;
;                                                                              ;
; Actual for add functionality:                                                ;
; https://board.flatassembler.net/topic.php?p=207751                           ;
; https://flatassembler.net/examples.php                                       ;
;------------------------------------------------------------------------------;

include 'win64w.inc'     ; FASM definitions for UNICODE application type
MAX_LOADSTRING   = 100   ; Limit for load strings resources, unicode chars  
ID_STRINGS       = 1     ; String resources required special numeration, why ?       
IDS_APP_TITLE    = 2     ; String resource for application name
IDS_CLASS_NAME   = 3     ; String resource for GUI window class name 
IDI_FILE_ICON    = 100   ; Icon resource for EXE file
IDI_MAIN_ICON    = 101   ; Icon resource for main window 
IDI_HELP_ICON    = 102   ; Icon resource for help window 
IDI_COLORS_ICON  = 103   ; Icon resource for colors selection window 
IDI_FILE_GICON   = 104   ; Icon resource group for EXE file
IDI_MAIN_GICON   = 105   ; Icon resource group for main window
IDI_HELP_GICON   = 106   ; Icon resource group for help window 
IDI_COLORS_GICON = 107   ; Icon resource group for colors selection window 
IDR_MENU         = 200   ; Application main menu resource
IDM_EXIT         = 201   ; ID for menuitem Exit
IDM_COLOR        = 202   ; ID for menuitem Color
IDM_ABOUT        = 203   ; ID for menuitem About
IDD_ABOUT_BOX    = 300   ; Dialogue resource for About window 
IDD_COLORS_BOX   = 301   ; Dialogue resource for Colors selection window 
IDC_ABOUT_TEXT   = 400   ; Text element resource for text in the About window   
IDB_ABOUT_OK     = 301   ; Button resource for OK button in the About window
IDB_COLORS_OK    = 302   ; Button resource for OK button in the Colors window 
IDA_MAIN         = 500   ; Accelerators key combinations resource for main menu

;---------- Code section ------------------------------------------------------;
format PE64 GUI
entry start
section '.code' code readable executable
start: 

; Application initialization
sub rsp,8*5
cld
lea rcx,[appCtrl]
call [InitCommonControlsEx]   ; Initializing for GUI application
test rax,rax
jz .exit                      ; Silent exit if initialization errors
xor ecx,ecx
call [GetModuleHandle]        ; Get handle of this exe module 
test rax,rax
jz .exit
mov [hInstance],rax           ; Save handle of this exe module 
mov rbx,rax                   ; RBX = Handle of this exe module 

; Load strings
mov rcx,rbx
mov edx,IDS_APP_TITLE
lea r8,[nameTitle]
mov r9d,MAX_LOADSTRING
call [LoadString]             ; Load name string for application main window 
test rax,rax
jz .exit
mov rcx,rbx
mov edx,IDS_CLASS_NAME
lea r8,[nameWindowClass]
mov r9d,MAX_LOADSTRING
call [LoadString]             ; Load name string for main window class
test rax,rax
jz .exit

; Load icons
mov rcx,rbx
mov edx,IDI_FILE_GICON
call [LoadIcon]               ; Load icon (group) for this exe module
test rax,rax
jz .exit
xchg rsi,rax                  ; RSI = This exe module icon handle
mov rcx,rbx
mov edx,IDI_MAIN_GICON
call [LoadIcon]               ; Load icon (group) for main window
test rax,rax
jz .exit
xchg rdi,rax                  ; RDI = main window icon handle
mov rcx,rbx
mov edx,IDI_HELP_GICON
call [LoadIcon]               ; Load icon (group) for Help window
test rax,rax
jz .exit
mov [hIconHelp] ,rax          ; Store Help window icon handle
mov rcx,rbx
mov edx,IDI_COLORS_GICON
call [LoadIcon]               ; Load icon (group) for Colors selection window
test rax,rax
jz .exit
mov [hIconColors] ,rax        ; Store Colors selection window icon handle

; Load cursor
xor ecx,ecx
mov edx,IDC_ARROW             ; ID = one of OS cursors, not from application
call [LoadCursor]             ; Load cursor for GUI window 
test rax,rax
jz .exit
xchg rbp,rax                  ; RBP = Cursor handle

; Register class for GUI window
lea rcx,[wcex]                ; RCX = Pointer to Windows Class structure
xor eax,eax                   ; RAX = 0 , for compact zero assign
mov [rcx + WNDCLASSEX.cbSize]        , sizeof.WNDCLASSEX
mov [rcx + WNDCLASSEX.style]         , CS_HREDRAW + CS_VREDRAW 
mov [rcx + WNDCLASSEX.lpfnWndProc]   , WndProc_Main
mov [rcx + WNDCLASSEX.cbClsExtra]    , eax
mov [rcx + WNDCLASSEX.cbWndExtra]    , eax
mov [rcx + WNDCLASSEX.hInstance]     , rbx
mov [rcx + WNDCLASSEX.hIcon]         , rsi
mov [rcx + WNDCLASSEX.hCursor]       , rbp
mov [rcx + WNDCLASSEX.hbrBackground] , COLOR_WINDOW + 1
mov [rcx + WNDCLASSEX.lpszMenuName]  , IDR_MENU
mov [rcx + WNDCLASSEX.lpszClassName] , nameWindowClass
mov [rcx + WNDCLASSEX.hIconSm]       , rdi
call [RegisterClassEx]        ; Class registration for GUI window
test rax,rax
jz .exit

X_BASE = 100  ; X position for main window left up point 
Y_BASE = 100  ; Y position for main window left up point
X_SIZE = 700  ; X size (width) for main window
Y_SIZE = 500  ; X size (height) for main window

; Create window
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
mov r9d,WS_VISIBLE + WS_DLGFRAME + WS_SYSMENU  ; Original = WS_OVERLAPPEDWINDOW
lea r8,[nameTitle]
mov rdx,[rcx + WNDCLASSEX.lpszClassName]
xor ecx,ecx
sub rsp,32
call [CreateWindowEx]         ; Create GUI window
add rsp,32 + 8 * 8
test rax,rax
jz .exit

; Show window
mov rcx,rbx
mov edx,SW_NORMAL
call [ShowWindow]             ; Set GUI window show state
mov rcx,rbx
call [UpdateWindow]           ; Send WM_PAINT message to GUI window, update it

; Load accelerators resource for keys combinations
mov rcx,[hInstance]
mov edx,IDA_MAIN
call [LoadAccelerators]       ; Load accelerator resource for keys combinations
test rax,rax
jz .exit

; Cycle for get and handling messages
xchg rsi,rax                  ; RSI = hAccelTable, save accelerators handle 
lea rdi,[msg]                 ; RDI = Pointer to message structure
.waitMessage:                 ; Cycle for get messages
mov rcx,rdi
xor edx,edx
xor r8d,r8d
xor r9d,r9d
call [GetMessage]
cmp rax,1		     
jb .exitMessage               ; RAX = 0 means exit application
jne .waitMessage              ; 0 < RAX < 2 means get next message 

mov rcx,[rdi + MSG.hwnd]      ; RAX = 1 means message handling
mov rdx,rsi
mov r8,rdi
call [TranslateAccelerator]   ; Detect accelerator keys  
test rax,rax
jnz .waitMessage 

mov rcx,[hDlgColor]
mov rdx,rdi
call [IsDialogMessage]
test rax,rax
jnz .waitMessage

mov rcx,rdi
call [TranslateMessage]       ; Translate virtual key messages to characters
mov rcx,rdi
call [DispatchMessage]        ; Dispatch message to window callback procedure
jmp .waitMessage              ; Go for wait next message
.exitMessage:

; Exit application
.exit:                        ; Exit application
xor ecx,ecx                   ; Exit code = 0           
call [ExitProcess]

; Equations for stack frame access, parameters shadow, created by caller.
; See Microsoft x64 calling convention for details.
; RBP = Pointer to frame
; +32 because PUSH RBX RSI RDI RBP
; +08 because RIP saved by CALL instruction
; +00 ... +16 is offsets of parameters
PARM_HWNDDLG EQU qword [rbp + 32 + 08 + 00]
PARM_MSG     EQU qword [rbp + 32 + 08 + 08]
PARM_WPARAM  EQU qword [rbp + 32 + 08 + 16]
PARM_LPARAM  EQU qword [rbp + 32 + 08 + 24]
LOW_WPARAM   EQU dword [rbp + 32 + 08 + 16]

; Callback procedure for Main window.
; Some of parameters backups is redundant for current procedure version.
; Some of PUSHes and parameters reloads is redundant for current procedure version.
; LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
WndProc_Main:
cld
push rbx rsi rdi rbp         ; This registers can be used but must non volatile
mov rbp,rsp                  ; RBP = Backup for stack pointer (RSP)
and rsp,0FFFFFFFFFFFFFFF0h   ; Stack alignment by x64 calling convention
sub rsp,32                   ; Parameters shadow by x64 calling convention 
mov PARM_HWNDDLG,rcx         ; Backup parameter #1 = HWND hWnd   
mov PARM_MSG,rdx             ; Backup parameter #2 = UINT message
mov PARM_WPARAM,r8           ; Backup parameter #3 = WPARAM wParam
mov PARM_LPARAM,r9           ; Backup parameter #4 = LPARAM lParam
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
.wmcommand:             ; Entry to WM_COMMAND message handler
movzx eax,r8w           ; EAX = int wmId = LOWORD(wParam)
cmp eax,IDM_EXIT
je .menuExit
cmp eax,IDM_ABOUT
je .menuAbout
cmp eax,IDM_COLOR
je .menuColors
jmp returnDefault

.menuAbout:             ; "About" item in the main menu 
push 0 0
lea r9,[WndProc_About]
mov r8,PARM_HWNDDLG
mov edx,IDD_ABOUT_BOX
mov rcx,[hInstance]
sub rsp,32
call [DialogBoxParam]
add rsp,32 + 8 * 2
jmp returnZero

.menuColors:             ; "Colors" item in the main menu
xor eax,eax
push rax
push rax
lea r9,[WndProc_Colors]
mov r8,PARM_HWNDDLG
mov edx,IDD_COLORS_BOX
mov rcx,[hInstance]
sub rsp,32
call [CreateDialogParam]
add rsp,32 + 16
jmp returnZero

.menuExit:              ; "Exit" item in the main menu
mov rcx,PARM_HWNDDLG
call [DestroyWindow]
jmp returnZero
.wmpaint:               ; Entry to WM_PAINT message handler 
mov rcx,PARM_HWNDDLG
lea rdx,[ps]
call [BeginPaint]
test rax,rax
jz @f
mov rbx,rax
push 0
mov eax,[testStringSize]
push rax                ; Size of string, chars
lea r9,[testString]     ; Pointer to string
mov r8d,10              ; Y position, original = 5
mov edx,10              ; X position, original = 5 
mov rcx,rbx             ; Handle of display context
sub rsp,32              ; Rebuild parameter shadow because >4 input parameters
call [TextOut]          ; Draw string
add rsp,32 + 8 * 2
mov rcx,PARM_HWNDDLG
lea rdx,[ps]
call [EndPaint]
@@:
jmp returnZero
.wmdestroy:             ; Entry to WM_DESTROY message handler 
xor ecx,ecx 
call [PostQuitMessage]  ; Destroy window when application exit
returnZero:             ; Return with status = 0 
xor eax,eax
jmp returnStatus
returnDefault:          ; Call default routine and return with it status
mov rcx,PARM_HWNDDLG 
mov rdx,PARM_MSG
mov r8,PARM_WPARAM
mov r9,PARM_LPARAM
call [DefWindowProc]
returnStatus:           ; Return: restore RSP and registers, status = RAX
mov rsp,rbp
pop rbp rdi rsi rbx
ret

; Callback procedure for About window.
; Some of parameters backups is redundant for current procedure version.
; Some of PUSHes and parameters reloads is redundant for current procedure version.
; INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
WndProc_About:
cld
push rbx rsi rdi rbp         ; This registers can be used but must non volatile
mov rbp,rsp                  ; RBP = Backup for stack pointer (RSP)
and rsp,0FFFFFFFFFFFFFFF0h   ; Stack alignment by x64 calling convention
sub rsp,32                   ; Parameters shadow by x64 calling convention
mov PARM_HWNDDLG,rcx         ; Backup parameter #1 = HWND hDlg  
mov PARM_MSG,rdx             ; Backup parameter #2 = UINT message
mov PARM_WPARAM,r8           ; Backup parameter #3 = WPARAM wParam
mov PARM_LPARAM,r9           ; Backup parameter #4 = LPARAM lParam
cmp rdx,WM_INITDIALOG
je .wminitdialog
cmp rdx,WM_COMMAND
je .wmcommand
jmp returnZero
.wminitdialog:               ; Entry to WM_INITDIALOG message handler
mov r9,[hIconHelp] 
mov r8d,ICON_SMALL 
mov edx,WM_SETICON 
mov rcx,PARM_HWNDDLG
call [SendMessage]
mov eax,1
jmp returnStatus
.wmcommand:                  ; Entry to WM_COMMAND message handler
movzx eax,r8w
cmp eax,IDB_ABOUT_OK
je .stopAbout                ; Go close "About" window if OK button 
cmp eax,IDCANCEL
je .stopAbout                ; Go close "About" window if [x]
jmp returnZero
.stopAbout:
mov rcx,PARM_HWNDDLG
mov rdx,r8
call [EndDialog]             ; Close "About" window
mov eax,1                    ; Status = TRUE
jmp returnStatus


; Callback procedure for Colors Selection window.
; Some of parameters backups is redundant for current procedure version.
; Some of PUSHes and parameters reloads is redundant for current procedure version.
; INT_PTR CALLBACK Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
WndProc_Colors:
cld
push rbx rsi rdi rbp         ; This registers can be used but must non volatile
mov rbp,rsp                  ; RBP = Backup for stack pointer (RSP)
and rsp,0FFFFFFFFFFFFFFF0h   ; Stack alignment by x64 calling convention
sub rsp,32                   ; Parameters shadow by x64 calling convention
mov PARM_HWNDDLG,rcx         ; Backup parameter #1 = HWND hDlg  
mov PARM_MSG,rdx             ; Backup parameter #2 = UINT message
mov PARM_WPARAM,r8           ; Backup parameter #3 = WPARAM wParam
mov PARM_LPARAM,r9           ; Backup parameter #4 = LPARAM lParam
cmp rdx,WM_INITDIALOG
je .wminitdialog
cmp rdx,WM_COMMAND
je .wmcommand
jmp returnZero
.wminitdialog:               ; Entry to WM_INITDIALOG message handler
;
; ...
;
mov r9,[hIconColors] 
mov r8d,ICON_SMALL 
mov edx,WM_SETICON 
mov rcx,PARM_HWNDDLG
call [SendMessage]
mov eax,1
jmp returnStatus
.wmcommand:                  ; Entry to WM_COMMAND message handler
movzx eax,r8w
cmp eax,IDB_COLORS_OK
je .stopColors               ; Go close "Colors selection" window if OK button 
cmp eax,IDCANCEL
je .stopColors               ; Go close "Colors selection" window if [x]
jmp returnZero
.stopColors:
mov rcx,PARM_HWNDDLG
call [DestroyWindow]         ; Close "Colors selection" window
mov eax,1                    ; Status = TRUE
jmp returnStatus
  
;---------- Data section ------------------------------------------------------;
; Locate first fields with pre-defined values, for file space minimization.
section '.data' data readable writeable
appCtrl          INITCOMMONCONTROLSEX  8, 0
testString       DU                    'This string show by WM_PAINT...', 0
testStringSize   DD                    31                     
nameTitle        DU  MAX_LOADSTRING    DUP (?)  ; Buffer for window name
nameWindowClass  DU  MAX_LOADSTRING    DUP (?)  ; Buffer for class name
align 8
hInstance        DQ            ?       ; This module handle
hDlgColor        DQ            ?       ; Handle for Colors selection dialogue
hIconHelp        DQ            ?       ; Icon handle for Help window
hIconColors      DQ            ?       ; Icon handle for Color selection window 
wcex             WNDCLASSEX    ?       ; Structure for window class
msg              MSG           ?       ; Structure for message 
ps               PAINTSTRUCT   ?       ; Structure for paint

;---------- Import section ----------------------------------------------------;
; Some of listed libraries is redundant for current application version.
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

;---------- Resources section -------------------------------------------------;
; String resources required special numeration, why ?
section '.rsrc' data readable resource
; Root directory of resources
directory \
RT_STRING      , dir_strings      , \
RT_ICON        , dir_icons        , \
RT_GROUP_ICON  , dir_gicons       , \
RT_MENU        , dir_menus        , \
RT_DIALOG      , dir_dialogs      , \
RT_ACCELERATOR , dir_accelerators 
; Resource directory for strings
resource dir_strings, ID_STRINGS, LANG_NEUTRAL + SUBLANG_DEFAULT, res_strings  
; Resource data for strings
resdata res_strings
DU 0                                    ; id = 0 
DU 0                                    ; id = 1  
DU 28 , 'Sample with non-modal window'  ; id = 2 
DU 7  , 'FASMGUI'                       ; id = 3 
endres
; Resource directory for icons
resource dir_icons , \
IDI_FILE_ICON    , LANG_NEUTRAL , res_fileicon    , \
IDI_MAIN_ICON    , LANG_NEUTRAL , res_mainicon    , \
IDI_HELP_ICON    , LANG_NEUTRAL , res_helpicon    , \
IDI_COLORS_ICON  , LANG_NEUTRAL , res_colorsicon
; Resource directory for group icons
resource dir_gicons , \
IDI_FILE_GICON   , LANG_NEUTRAL , res_filegicon   , \
IDI_MAIN_GICON   , LANG_NEUTRAL , res_maingicon   , \
IDI_HELP_GICON   , LANG_NEUTRAL , res_helpgicon   , \
IDI_COLORS_GICON , LANG_NEUTRAL , res_colorsgicon
; Resource data for icons and group icons
icon res_filegicon   , res_fileicon   , 'images\fasm64.ico'
icon res_maingicon   , res_mainicon   , 'images\sysinfo.ico'
icon res_helpgicon   , res_helpicon   , 'images\help.ico'
icon res_colorsgicon , res_colorsicon , 'images\colors.ico'
; Resource directory for main menu
resource dir_menus, IDR_MENU, LANG_ENGLISH + SUBLANG_DEFAULT, mainMenu
; Resource data for main menu
menu mainMenu
menuitem '&File'     , 0         , MFR_POPUP
menuitem 'E&xit'     , IDM_EXIT  , MFR_END
menuitem '&Dialog'   , 0         , MFR_POPUP
menuitem 'Co&lor'    , IDM_COLOR , MFR_END
menuitem '&Help'     , 0         , MFR_POPUP + MFR_END
menuitem '&About...' , IDM_ABOUT , MFR_END
; Resource directory for "About" window (this window created by resources)
resource dir_dialogs, \
IDD_ABOUT_BOX  , LANG_ENGLISH + SUBLANG_DEFAULT, aboutBox , \
IDD_COLORS_BOX , LANG_ENGLISH + SUBLANG_DEFAULT, colorsBox
; Resource data for "About" window
dialog      aboutBox, 'Program info' , 20, 20, 100, 50 , WS_CAPTION + WS_SYSMENU, 0, 0, 'Verdana', 10
dialogitem  'STATIC', 'Non-modal sample v0.0.'  , IDC_ABOUT_TEXT,   7,  10, 170,  10 , WS_VISIBLE
dialogitem  'BUTTON', 'OK'                      , IDB_ABOUT_OK  ,  30,  30,  38,  13 , WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 
; Resource data for "Colors selection" window
; Note WS_VISIBLE set for dialog
dialog      colorsBox, 'Colors selection' , 100, 100, 250, 150 , WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'BUTTON', 'OK'                , IDB_COLORS_OK , 205, 130,  38,  13 , WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 
; Resource directory for accelerators key combinations description
resource dir_accelerators, IDA_MAIN, LANG_ENGLISH + SUBLANG_DEFAULT, hot_keys
; Resource data for accelerators key combinations description
; In this example:
; ALT-X     = File\Exit
; ALT-L     = Dialog\Color
; ALT-A     = Help\About 
accelerator hot_keys,\
FVIRTKEY + FNOINVERT + FALT , 'X' , IDM_EXIT  , \
FVIRTKEY + FNOINVERT + FALT , 'L' , IDM_COLOR , \
FVIRTKEY + FNOINVERT + FALT , 'A' , IDM_ABOUT
