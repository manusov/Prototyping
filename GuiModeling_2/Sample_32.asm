;------------------------------------------------------------------------------;
;                                                                              ;
;               "About", "Draw" and "Progress" windows mockups                 ; 
;                        by Dialog boxes minimal sample.                       ; 
;                           ia32 variant for FASM.                             ; 
;                                                                              ;
;   https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes   ;
;   https://habr.com/ru/post/577372/                                           ;
;   http://old-dos.ru/books/7/f/5/WindowApplicationFasm.pdf                    ;
;                                                                              ;
;------------------------------------------------------------------------------;
;
; THIS ENGINEERING SAMPLE CONTAINS BUGS AND NON-OPTIMAL SOLUTIONS.
;
; TODO BEFORE INTEGRATION TO NCRB SOURCE.
;
; 1)+ OPTIMIZE CODE. INCLUDE ONE STRUCTURE FOR DRAW STRINGS AND MIN-MAX.
; 2)+ RELEASE OS HANDLES FOR FONTS, BRUSHES AND OTHER OBJECTS. BUT NOT DELETE SHARED CURSOR.
; 3)+ CHECK STATUS AFTER WINAPI FUNCTIONS (PARTIAL SUPPORT).
; 4)+ CALCULATED GEOMETRY INCLUDE X-CENTERING STRINGS 4 AND 5. 
; 5)+ BASE-INDEX ACCESS TO VARIABLES BY STRUCTURE PER "ABOUT" WINDOW. AT TEMP BUFFER.
; 6)+ MOUSE COORDINATES DETECTORS VALUES MUST BE PRE DEFINED TO 0 OR -1.
; 7)  COMMENTS AND SYMBOLIC CONSTANTS FOR SUB-STRINGS SIZES, COORDINATES AND COLORS.
;     WMPAINT constants: 44, 5, 104.
;
; TODO AFTER INTEGRATION TO NCRB SOURCE.
;
; 1)  MAKE "ABOUT" WINDOW TITLE AND OTHER STRINGS AS RAW RESOURCE, FOR USE ONE BYTE PER CHAR.
; 2)  INTERVAL BETWEEN "(C)" AND "2022".
; 3)  ABOUT STRINGS FORMAT CHANGED, UPDATE SAVE REPORT PROCEDURE.
;
; DEFERRED.
;
; 1)  OPTIMIZE COLORS COUNT AT ICONS, INCLUDE TAB ICONS, NOT "ABOUT" ICONS ONLY.
; 2)  ERROR DETAIL DECODE AFTER SHELLEXECUTE.
; 3)  VIEW DESIGN OPTIMIZATION.
; 4)  VERIFY RET / RET 16 FOR IA32 VERSION DIALOGUE CALLBACK PROCEDURES, ALL.
;
;------------------------------------------------------------------------------;

include 'win32a.inc'

CLEARTYPE_QUALITY      =  5
YADD1                  =  17
YADD2                  =  17
YBOTTOM                =  44
ICONX                  =  104
ICONY                  =  5

COUNT_ICON             =  2

IDI_EXE_ICON           =  100
IDI_BOOKS_ICON         =  101

IDI_EXE_ICONS          =  200
IDI_BOOKS_ICONS        =  201

IDD_DIALOG_P           =  300
IDD_DIALOG_D           =  301
IDD_DIALOG_A           =  302
IDD_DIALOG_1           =  303
IDD_DIALOG_2           =  304
IDD_DIALOG_3           =  305

IDB_CHILD_P            =  400
IDB_CHILD_D            =  401
IDB_CHILD_A            =  402
IDB_CHILD_1            =  403
IDB_CHILD_2            =  404
IDB_EXIT               =  405
IDB_OK                 =  406

RESOURCE_DESCRIPTION  equ 'About Box sample (ia32)'
RESOURCE_VERSION      equ '0.0.0.7'
RESOURCE_COMPANY      equ 'https://github.com/manusov'
RESOURCE_COPYRIGHT    equ '(C) 2022 Ilya Manusov'

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

cld

lea esi,[lines1a]
lea edi,[ABOUT_BOX.s1]
mov ecx,sizeof.LINECONST * 3 + 4 + sizeof.LINEVAR * 2 + 4
rep movsb

push appCtrl
call [InitCommonControlsEx]
test eax,eax
jz .exit
push 0
call [GetModuleHandle] 
test eax,eax
jz .exit
mov [hInstance],eax 

xchg ebx,eax
mov esi,IDI_EXE_ICONS
lea edi,[hIcons]
mov ebp,COUNT_ICON
.loadIcons:
push esi 
push ebx  
call [LoadIcon]
test eax,eax
jz .exit
stosd
inc esi
dec ebp
jnz .loadIcons

push 0
push DialogProc1
push HWND_DESKTOP
push IDD_DIALOG_1
push ebx
call [CreateDialogParam]
test eax,eax
jz .exit
mov [hWinMain],eax
xchg ebx,eax
; debug sample yet use ABOUT structure for all
lea esi,[ABOUT_BOX.msg]

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
mov ecx,[hWinDraw]
jecxz .yetNoDraw
push esi
push ecx
call [IsDialogMessage]
test eax,eax
jnz .waitMessage
.yetNoDraw:
mov ecx,[hWinProgress]
jecxz .yetNoProgress
push esi
push ecx
call [IsDialogMessage]
test eax,eax
jnz .waitMessage
.yetNoProgress:
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
jne .statusZero
.wmclose:
push dword [esp + 04]
call [DestroyWindow]
push 0
call [PostQuitMessage]
jmp .statusZero
.wmcommand:
movzx eax,word [esp + 12]
cmp eax,IDB_EXIT
je .wmclose
cmp eax,IDB_CHILD_P
je .openProgress
cmp eax,IDB_CHILD_D
je .openDraw
cmp eax,IDB_CHILD_A
je .openAbout
cmp eax,IDB_CHILD_1
je .openModal
cmp eax,IDB_CHILD_2
je .openModeless
jmp .statusZero
.openProgress:
push 0
push DialogProcProgress
push dword [esp + 12]
push IDD_DIALOG_P
push [hInstance]
call [CreateDialogParam]
mov [hWinProgress],eax
jmp .statusZero
.openDraw:
push 0
push DialogProcDraw
push dword [esp + 12]
push IDD_DIALOG_D
push [hInstance]
call [CreateDialogParam]
mov [hWinDraw],eax
jmp .statusZero
.openAbout:
push 0
push DialogProcA
push dword [esp + 12]
push IDD_DIALOG_A
push [hInstance]
call [DialogBoxParam]
jmp .statusZero
.openModal:
push 0
push DialogProc2
push dword [esp + 12]
push IDD_DIALOG_2
push [hInstance]
call [DialogBoxParam]
jmp .statusZero
.openModeless:
push 0
push DialogProc3
push dword [esp + 12]
push IDD_DIALOG_3
push [hInstance]
call [CreateDialogParam]
mov [hWinChild],eax
.statusZero:
xor eax,eax
ret 16

DialogProc2:
cmp dword [esp + 08],WM_CLOSE
jne .statusZero
push 0
push dword [esp + 08]
call [EndDialog]
.statusZero:
xor eax,eax
ret 16

DialogProc3:
cmp dword [esp + 08],WM_CLOSE
jne .statusZero
push dword [esp + 04]
call [DestroyWindow]
.statusZero:
xor eax,eax
ret 16


;---------- Callback dialogue procedure ---------------------------------------;
;           Handler for "About" window, item in the application main menu.     ; 
;                                                                              ;
; INPUT:   [esp + 04] = Parm#1 = HWND = Dialog box handle                      ; 
;          [esp + 08] = Parm#2 = UINT = Message                                ; 
;          [esp + 12] = Parm#3 = WPARAM, message-specific                      ;
;          [esp + 16] = Parm#4 = LPARAM, message-specific                      ;
;                                                                              ;
; OUTPUT:  EAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;                                                                              ;
;------------------------------------------------------------------------------;
DialogProcA:
push ebx esi edi ebp
mov eax,[esp + 08 + 16]
mov ebx,[esp + 04 + 16]
;---------- Detect message type -----------------------------------------------;
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_CLOSE
je .wmclose
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_LBUTTONDOWN
je .wmlbuttondown
cmp eax,WM_RBUTTONDOWN
je .wmrbuttondown
cmp eax,WM_LBUTTONUP
je .wmlbuttonup
cmp eax,WM_RBUTTONUP
je .wmrbuttonup
cmp eax,WM_MOUSEMOVE
je .wmmousemove
cmp eax,WM_PAINT
je .wmpaint
.statusZero:
xor eax,eax
jmp .status
.statusOne:
mov eax,1
.status:
pop ebp edi esi ebx
ret

;------------------------------------------------------------------------------;
;                                                                              ;
;               WM_INITDIALOG handler: create "About" window.                  ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wminitdialog:
push [hIcons + 0]
push ICON_SMALL
push WM_SETICON 
push dword [esp + 16 + 16]
call [SendMessage]
push IDC_HAND
push 0
call [LoadCursor]
mov [ABOUT_BOX.hCursor],eax
xor eax,eax                      ; EAX = 0 for compact push 0
push eax                         ; Parm#14 = Pointer to font typename string, here not used
push VARIABLE_PITCH              ; Parm#13 = Font pitch and family
push CLEARTYPE_QUALITY           ; Parm#12 = Output quality
push CLIP_DEFAULT_PRECIS         ; Parm#11 = Clip precision
push OUT_OUTLINE_PRECIS          ; Parm#10 = Output precision
push DEFAULT_CHARSET             ; Parm#9  = Charset
push eax                         ; Parm#8  = Strike, here=0=none
push eax                         ; Parm#7  = Underline, here=0=none
push eax                         ; Parm#6  = Italic, here=0=none
push FW_DONTCARE                 ; Parm#5  = Weight of the font
push eax                         ; Parm#4 = Orientation
push eax                         ; Parm#3 = Escapment
push eax                         ; Parm#2 = Width
push 16                          ; Parm#1 = Height
call [CreateFont]
mov [ABOUT_BOX.hFont],eax 
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
mov ecx,[ABOUT_BOX.hFont]
jecxz @f
push ecx
call [DeleteObject]
@@:
push 0
push dword [esp + 08 + 16]
call [EndDialog]
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_COMMAND handler: interpreting user input.                 ; 
;                 Detect click "OK" button at "About" window.                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmcommand:
cmp word [esp + 12 + 16],IDB_OK
je .wmclose
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;        WM_LBUTTONDOWN handler: interpreting mouse left button click.         ; 
;          Provide mouse cursor consistency and go web link if click.          ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmlbuttondown:
mov eax,[esp + 16 + 16]
call HelperCheckLine4
lea ecx,[linkGitHub]
jc .clicked 
call HelperCheckLine5
lea ecx,[linkFasm]
jnc .statusZero
.clicked:
push ecx
mov ecx,[ABOUT_BOX.hCursor]
jecxz @f
push ecx
call [SetCursor]
@@:
pop ecx
xor eax,eax
push SW_NORMAL
push eax
push eax
push ecx
push eax
push eax
call [ShellExecute]
cmp eax,32
ja @f
push MB_ICONERROR   ; Parm#4 = Attributes
push 0              ; Parm#3 = Pointer to title (caption) string
push msgShell       ; Parm#2 = Pointer to string: error name 
push 0              ; Parm#1 = Parent Window or NULL
call [MessageBox]  
@@:
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;              WM_RBUTTONDOWN, WM_LBUTTONUP, WM_RBUTTONUP handler.             ; 
;    Provide mouse cursor consistency when mouse buttons click and release.    ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmrbuttondown:
.wmlbuttonup:
.wmrbuttonup:
mov eax,[esp + 16 + 16]
call HelperCheckLine4
jc @f
call HelperCheckLine5
jnc .statusZero
@@:
.change:
mov ecx,[ABOUT_BOX.hCursor]
jecxz @f
push ecx
call [SetCursor]
@@:
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;              WM_RBUTTONDOWN, WM_LBUTTONUP, WM_RBUTTONUP handler.             ; 
;               Provide mouse cursor consistency when mouse move.              ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmmousemove:
mov eax,[esp + 16 + 16]
call HelperCheckLine4
jc .change
call HelperCheckLine5
jc .change
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_PAINT handler. Draw GUI window content.                   ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmpaint:
lea esi,[ABOUT_BOX.ps]
push esi
push ebx
call [BeginPaint]
test eax,eax
jz .statusZero
push 0
xchg ebp,eax
lea edi,[ABOUT_BOX.rect]
push esi edi
add esi,PAINTSTRUCT.rcPaint + RECT.left
mov ecx,3
cld
rep movsd
lodsd
sub eax,YBOTTOM
stosd
pop edi esi
push COLOR_WINDOW + 1
push edi
push ebp
call [FillRect] 
push [hIcons + 04]
push ICONY
push ICONX
push ebp
call [DrawIcon]
xor eax,eax
mov ecx,[ABOUT_BOX.hFont]
jecxz @f
push ecx
push ebp
call [SelectObject]
@@:
mov [ABOUT_BOX.hFontBack],eax
push ABOUT_BOX.tm
push ebp
call [GetTextMetrics]
;---------- Strings 1-3, without clickable elements ---------------------------;
push esi
cld
lea esi,[ABOUT_BOX.s1]
.next1:
lodsd
xchg ecx,eax
jecxz .stop1
lodsb
movzx eax,al
mov [edi + RECT.top],eax
add eax,YADD1
mov [edi + RECT.bottom],eax
call HelperCenterText
jmp .next1
.stop1:
pop esi
;---------- Strings 4-5, with clickable elements ------------------------------;
push ebx
lea ebx,[ABOUT_BOX.s4]
.next2:
mov edx,[ebx + LINEVAR.pointer]
test edx,edx
jz .stop2
movzx eax,byte [ebx + LINEVAR.vertical]
mov [ebx + LINEVAR.ymin],eax
add eax,[ABOUT_BOX.tm + TEXTMETRIC.tmHeight]
mov [ebx + LINEVAR.ymax],eax
movzx ecx,byte [ebx + LINEVAR.xs1]
call HelperTextSize
mov edx,[esi + PAINTSTRUCT.rcPaint + RECT.right]
sub edx,[esi + PAINTSTRUCT.rcPaint + RECT.left]
sub edx,eax
shr edx,1
mov [esp + 04],edx
mov [edi + RECT.left],edx
movzx eax,byte [ebx + LINEVAR.vertical]
mov [edi + RECT.top],eax
add eax,YADD2
mov [edi + RECT.bottom],eax
movzx edx,byte [ebx + LINEVAR.xs2]
mov ecx,[ebx + LINEVAR.pointer]
call HelperLeftText
movzx ecx,byte [ebx + LINEVAR.xs2]
mov edx,[ebx + LINEVAR.pointer]
call HelperTextSize
add eax,[esp + 04]
mov [edi + RECT.left],eax
mov [ebx + LINEVAR.xmin],eax
push 00FF0000h
push ebp
call [SetTextColor]
push eax
movzx edx,byte [ebx + LINEVAR.xs3]
movzx ecx,byte [ebx + LINEVAR.xs2]
add ecx,[ebx + LINEVAR.pointer]
call HelperLeftText
push ebp
call [SetTextColor]
movzx ecx,byte [ebx + LINEVAR.xs2]
add cl,[ebx + LINEVAR.xs3]
mov edx,[ebx + LINEVAR.pointer]
call HelperTextSize
add eax,[esp + 04]
mov [edi + RECT.left],eax
mov [ebx + LINEVAR.xmax],eax
mov edx,-1
movzx ecx,byte [ebx + LINEVAR.xs2]
add cl,[ebx + LINEVAR.xs3]
add ecx,[ebx + LINEVAR.pointer]
call HelperLeftText
add ebx,sizeof.LINEVAR
jmp .next2 
.stop2:
pop ebx eax
;---------- Paint window elements done ----------------------------------------; 
mov ecx,[ABOUT_BOX.hFontBack]
jecxz @f
push ecx
push ebp
call [SelectObject]
@@:
push esi
push ebx
call [EndPaint]
jmp .statusZero

;---------- Helper for detect mouse cursor position at clickable text ---------;
;                                                                              ;
; INPUT  :  EAX = Mouse cursor window position, bits[31-16]=Y, bits[15-0]=X    ;
;           Memory structure (LINEVAR) used                                    ; 
;                                                                              ;
; OUTPUT :  CF = result, 0 = Mouse cursor outside, 1 = Indide                  ;
;                                                                              ;
;------------------------------------------------------------------------------; 
HelperCheckLine4:
lea ecx,[ABOUT_BOX.s4 + LINEVAR.xmin]  ; Pointer for clickable string 1
jmp helperCheckLine
HelperCheckLine5:
lea ecx,[ABOUT_BOX.s5 + LINEVAR.xmin]  ; Pointer for clickable string 2
helperCheckLine:
push eax
mov edx,eax
shr edx,16
movzx eax,ax
cmp eax,[ecx + 00]
jb @f
cmp eax,[ecx + 04]
ja @f
cmp edx,[ecx + 08]
jb @f
cmp edx,[ecx + 12]
ja @f
pop eax
stc
ret
@@:
pop eax
clc
ret

;---------- Helper for draw text string ---------------------------------------;
;                                                                              ;
; INPUT  : EBP = Handle Device Context (HDC)                                   ;
;          ECX = Pointer to text string                                        ;
;          EDX = Length of text string or -1 if 0-terminated string            ;
;          EDI = Pointer to RECT structure for string positioning at window    ;
;                                                                              ;
; OUTPUT : None                                                                ;
;                                                                              ;
;------------------------------------------------------------------------------; 
HelperLeftText:
mov eax,DT_LEFT + DT_NOPREFIX    ; Entry point for Left-aligned, EDX=Length
jmp helperText
HelperCenterText:
mov eax,DT_CENTER + DT_NOPREFIX  ; Entry point for Center-aligned, 0-terminated
mov edx,-1
helperText:
push eax 
push edi
push edx
push ecx
push ebp
call [DrawText]
ret

;---------- Helper for get text string pixel width ----------------------------;
;                                                                              ;
; INPUT  : EBP = Handle Device Context (HDC)                                   ;
;          EDX = Pointer to text string, string without 0 termination          ;
;          ECX = Length of text string                                         ;
;                                                                              ;
; OUTPUT : EAX = pixel width                                                   ;
;          Memory structure (sz) updated                                       ;
;                                                                              ;
;------------------------------------------------------------------------------; 
HelperTextSize:
push ABOUT_BOX.sz
push ecx
push edx
push ebp
call [GetTextExtentPoint32]
mov eax,[ABOUT_BOX.sz + SIZE.cx]
ret

;---------- Print 32-bit Decimal Number ---------------------------------------;
;                                                                              ;
; INPUT:   EAX = Number value                                                  ;
;          BL  = Template size, chars. 0=No template                           ;
;          EDI = Destination Pointer (flat)                                    ;
;                                                                              ;
; OUTPUT:  EDI = New Destination Pointer (flat)                                ;
;                modified because string write                                 ;
;                                                                              ;
;------------------------------------------------------------------------------;
DecimalPrint32:
pushad
cld
mov bh,80h-10         ; Bit BH.7 = print zeroes flag
add bh,bl
mov ecx,1000000000    ; ECX = service divisor
.mainCycle:
xor edx,edx
div ecx               ; Produce current digit, EDX:EAX / ECX
and al,0Fh
test bh,bh
js .firstZero
cmp ecx,1
je .firstZero
cmp al,0              ; Not actual left zero ?
jz .skipZero
.firstZero:
mov bh,80h            ; Flag = 1
or al,30h
stosb                 ; Store char
.skipZero:
push edx              ; Push remainder
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax          ; ECX = Quotient, used as divisor and cycle condition 
pop eax              ; EAX = remainder
inc bh
test ecx,ecx
jnz .mainCycle       ; Cycle if (unsigned) quotient still > 0 
mov [esp],edi
popad
ret

; Connect modules under debug.
include 'Draw_32.inc'
include 'Progress_32.inc'


section '.data' data readable writeable
; constants
appCtrl     INITCOMMONCONTROLSEX  8, 0
line1       DB  'NUMA CPU&RAM Benchmarks.'                    , 0
line2       DB  'v2.02.02 for Windows ia32.'                  , 0
line3       DB  '(C) 2022 Ilya Manusov.'                      , 0
line4       DB  'More at GitHub.'                             , 0
line5       DB  'Developed with Flat Assembler.'              , 0
linkGitHub  DB  'https://github.com/manusov?tab=repositories' , 0
linkFasm    DB  'https://flatassembler.net/'                  , 0
msgShell    DB  'Shell error.'                                , 0

struct LINECONST
pointer   dd ?
vertical  db ?
ends

struct LINEVAR
pointer   dd  ?
vertical  db  ?
xs1       db  ?
xs2       db  ?
xs3       db  ?
xmin      dd  ?
xmax      dd  ?
ymin      dd  ?
ymax      dd  ?
ends

lines1a:
s1a  LINECONST  line1, 45 
s2a  LINECONST  line2, 62
s3a  LINECONST  line3, 79
e1a  dd    0

lines2a:
s4a  LINEVAR  line4, 106, 15,  8,  6, -1, -1, -1, -1 
s5a  LINEVAR  line5, 123, 30, 15, 14, -1, -1, -1, -1
e2a  dd    0

; variables
align 4
hInstance     DD  ?
hWinMain      DD  ?
hWinChild     DD  ?
hWinDraw      DD  ?
hWinProgress  DD  ?
hIcons        DD  COUNT_ICON  DUP (?) 

struct ABOUTBOX
hCursor      dd          ?
hFont        dd          ?
hFontBack    dd          ?
msg          MSG         ?
ps           PAINTSTRUCT ?
rect         RECT        ?
sz           SIZE        ?
tm           TEXTMETRIC  ?
s1           LINECONST   ?
s2           LINECONST   ?
s3           LINECONST   ?
e1           dd          ?
s4           LINEVAR     ?
s5           LINEVAR     ?
e2           dd          ?
ends
align 8
ABOUT_BOX ABOUTBOX ?

struct DRAWBOX
hCursor      dd          ?
hFont        dd          ?
hFontBack    dd          ?
msg          MSG         ?
ps           PAINTSTRUCT ?
rect         RECT        ?
sz           SIZE        ?
tm           TEXTMETRIC  ?
ends
align 8
DRAW_BOX DRAWBOX ?

XCOUNT = 700
struct DRAWPOINTS
pointsX      dq XCOUNT dup (?)
pointsY      dq XCOUNT dup (?)
scaleX       dq ?
scaleY       dq ?
gridOffsetX  dq ?
gridOffsetY  dq ?
gridStepX    dq ?
gridStepY    dq ?
valueStepX   dq ?
valueStepY   dq ?
ends
align 8
DRAW_POINTS DRAWPOINTS ?

struct PROGRESSBOX
hCursor      dd          ?
hFont        dd          ?
hFontBack    dd          ?
msg          MSG         ?
ps           PAINTSTRUCT ?
rect         RECT        ?
sz           SIZE        ?
tm           TEXTMETRIC  ?
; additions for progress window
hPen                dd  ?          ; pen for progress percentage cycle
progressPercentage  dd  ?          ; percentage integer value, 0-100
progressString      db  5 dup (?)  ; maximum string length = 5 , if "100%",0
oldPoint         POINT  ?
alpha               dq  ?
delta               dq  ?
a                   dd  ?
b                   dd  ?
c                   dd  ?
; additions for porting from x64 code
; TODO. Remove this temporary data by optimization: registers or stack.
vr12d               dd  ?
vr15d               dd  ?
vrbp                dd  ?
ends
align 8
PROGRESS_BOX PROGRESSBOX ?


section '.idata' import data readable writeable
library \ 
kernel32 , 'kernel32.dll' , \
advapi32 , 'advapi32.dll' , \
user32   , 'user32.dll'   , \
comctl32 , 'comctl32.dll' , \
comdlg32 , 'comdlg32.dll' , \
gdi32    , 'gdi32.dll'    , \
shell32  , 'shell32.dll' 
include  'api\kernel32.inc'
include  'api\advapi32.inc'
include  'api\user32.inc'
include  'api\comctl32.inc'
include  'api\comdlg32.inc'
include  'api\gdi32.inc'
include  'api\shell32.inc'


section '.rsrc' resource data readable
directory RT_DIALOG     , dialogs   , \
          RT_ICON       , icons     , \
          RT_GROUP_ICON , gicons    , \
          RT_MANIFEST   , manifests , \
          RT_VERSION    , version
resource  dialogs, \
IDD_DIALOG_1 , LANG_ENGLISH + SUBLANG_DEFAULT, mainDialog    , \
IDD_DIALOG_P , LANG_ENGLISH + SUBLANG_DEFAULT, progressChild , \
IDD_DIALOG_D , LANG_ENGLISH + SUBLANG_DEFAULT, drawChild     , \
IDD_DIALOG_A , LANG_ENGLISH + SUBLANG_DEFAULT, aboutChild    , \
IDD_DIALOG_2 , LANG_ENGLISH + SUBLANG_DEFAULT, firstChild    , \
IDD_DIALOG_3 , LANG_ENGLISH + SUBLANG_DEFAULT, secondChild
dialog mainDialog, 'Application main window (ia32)', 100, 100, 200, 150, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem 'BUTTON', 'Open progress' , IDB_CHILD_P, 120,  57, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open draw'     , IDB_CHILD_D, 120,  72, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open about'    , IDB_CHILD_A, 120,  87, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open modal'    , IDB_CHILD_1, 120, 102, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open modeless' , IDB_CHILD_2, 120, 117, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Exit'          , IDB_EXIT   , 120, 132, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 
dialog progressChild, 'Please wait...', 100, 100, 90,  85, \
       WS_VISIBLE, 0, 0, 'Verdana', 10
enddialog 
dialog drawChild, 'Draw', 100, 100, 387, 278, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
enddialog 
dialog aboutChild, 'Program info', 100, 100, 120, 101, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem 'BUTTON', 'OK' , IDB_OK , 47 , 83, 32, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 
dialog firstChild, 'First child window (modal)', 100, 100, 150, 100, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
enddialog 
dialog secondChild, 'Second child window (modeless)', 100, 100, 150, 100, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
enddialog 
resource icons , \
IDI_EXE_ICON   , LANG_NEUTRAL , exeicon , \
IDI_BOOKS_ICON , LANG_NEUTRAL , booksicon
resource gicons , \
IDI_EXE_ICONS   , LANG_NEUTRAL , exegicon , \
IDI_BOOKS_ICONS , LANG_NEUTRAL , booksgicon
icon exegicon   , exeicon   , 'images\fasm32.ico'
icon booksgicon , booksicon , 'images\books.ico'
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
