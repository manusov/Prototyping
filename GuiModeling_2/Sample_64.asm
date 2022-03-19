;------------------------------------------------------------------------------;
;                                                                              ;
;               "About", "Draw" and "Progress" windows mockups                 ; 
;                        by Dialog boxes minimal sample.                       ; 
;                           x64 variant for FASM.                              ; 
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

include 'win64a.inc'

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

RESOURCE_DESCRIPTION  equ 'Dialog Boxes sample (x64)'
RESOURCE_VERSION      equ '0.0.0.8'
RESOURCE_COMPANY      equ 'https://github.com/manusov'
RESOURCE_COPYRIGHT    equ '(C) 2022 Ilya Manusov'

format PE64 GUI
entry start
section '.code' code readable executable
start: 

sub rsp,8*5
cld

lea rsi,[lines1a]
lea rdi,[ABOUT_BOX.s1]
mov ecx,sizeof.LINECONST * 3 + 8 + sizeof.LINEVAR * 2 + 8
rep movsb

lea rcx,[appCtrl]
call [InitCommonControlsEx]
test rax,rax
jz .exit
xor ecx,ecx
call [GetModuleHandle] 
test rax,rax
jz .exit
mov [hInstance],rax 

xchg rbx,rax
mov esi,IDI_EXE_ICONS
lea rdi,[hIcons]
mov ebp,COUNT_ICON
.loadIcons:
mov edx,esi 
mov rcx,rbx  
call [LoadIcon]
test rax,rax
jz .exit
stosq
inc esi
dec ebp
jnz .loadIcons
 
xor edx,edx
push rdx rdx
lea r9,[DialogProc1]
mov r8d,HWND_DESKTOP
mov edx,IDD_DIALOG_1
mov rcx,rbx
sub rsp,32
call [CreateDialogParam]
add rsp,32 + 16
test rax,rax
jz .exit
mov [hWinMain],rax
xchg rbx,rax
; debug sample yet use ABOUT structure for all
lea rsi,[ABOUT_BOX.msg] 

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
mov rcx,[hWinDraw]
jrcxz .yetNoDraw
call [IsDialogMessage]
test eax,eax
jnz .waitMessage
.yetNoDraw:
mov rdx,rsi
mov rcx,[hWinProgress]
jrcxz .yetNoProgress
call [IsDialogMessage]
test eax,eax
jnz .waitMessage
.yetNoProgress:
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
jne .statusZero
.wmclose:
call [DestroyWindow]
xor ecx,ecx
call [PostQuitMessage]
jmp .statusZero
.wmcommand:
movzx eax,r8w
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
xor eax,eax
push rax rax
lea r9,[DialogProcProgress]
mov r8,rcx
mov edx,IDD_DIALOG_P
mov rcx,[hInstance]
sub rsp,32
call [CreateDialogParam]
add rsp,32 + 16
mov [hWinProgress],rax
jmp .statusZero
.openDraw:
xor eax,eax
push rax rax
lea r9,[DialogProcDraw]
mov r8,rcx
mov edx,IDD_DIALOG_D
mov rcx,[hInstance]
sub rsp,32
call [CreateDialogParam]
add rsp,32 + 16
mov [hWinDraw],rax
jmp .statusZero
.openAbout:
xor eax,eax
push rax rax
lea r9,[DialogProcA]
mov r8,rcx
mov edx,IDD_DIALOG_A
mov rcx,[hInstance]
sub rsp,32
call [DialogBoxParam]
add rsp,32 + 16
jmp .statusZero
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
jmp .statusZero
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
.statusZero:
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
jne .statusZero
xor edx,edx
call [EndDialog]
.statusZero:
xor eax,eax
mov rsp,rbp
pop rbp
ret

DialogProc3:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
cmp edx,WM_CLOSE
jne .statusZero
call [DestroyWindow]
.statusZero:
xor eax,eax
mov rsp,rbp
pop rbp
ret


;---------- Callback dialogue procedure ---------------------------------------;
;           Handler for "About" window, item in the application main menu.     ; 
;                                                                              ;
; INPUT:   RCX = Parm#1 = HWND = Dialog box handle                             ; 
;          RDX = Parm#2 = UINT = Message                                       ; 
;          R8  = Parm#3 = WPARAM, message-specific                             ;
;          R9  = Parm#4 = LPARAM, message-specific                             ;
;                                                                              ;
; OUTPUT:  RAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;                                                                              ;
;------------------------------------------------------------------------------;
DialogProcA:
push rbx rsi rdi rbp r12 r13 r14
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
xchg eax,edx
mov rbx,rcx
lea r14,[ABOUT_BOX]
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
mov rsp,rbp
pop r14 r13 r12 rbp rdi rsi rbx
ret

;------------------------------------------------------------------------------;
;                                                                              ;
;               WM_INITDIALOG handler: create "About" window.                  ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wminitdialog:
mov r9,[hIcons + 0]
mov r8d,ICON_SMALL
mov edx,WM_SETICON 
; rcx valid here
call [SendMessage]
mov edx,IDC_HAND
xor ecx,ecx
call [LoadCursor]
mov [r14 + ABOUTBOX.hCursor],rax
mov ecx,16                   ; Parm#1 = RCX = Height
xor edx,edx                  ; Parm#2 = RDX = Width
xor r8d,r8d                  ; Parm#3 = R8  = Escapment
xor r9d,r9d                  ; Parm#4 = R9  = Orientation
xor eax,eax                  ; RAX = 0 for compact push 0
push rax                     ; Parm#14 = Pointer to font typename string, here not used
push VARIABLE_PITCH          ; Parm#13 = Font pitch and family
push CLEARTYPE_QUALITY       ; Parm#12 = Output quality
push CLIP_DEFAULT_PRECIS     ; Parm#11 = Clip precision
push OUT_OUTLINE_PRECIS      ; Parm#10 = Output precision
push DEFAULT_CHARSET         ; Parm#9  = Charset
push rax                     ; Parm#8  = Strike, here=0=none
push rax                     ; Parm#7  = Underline, here=0=none
push rax                     ; Parm#6  = Italic, here=0=none
push FW_DONTCARE             ; Parm#5  = Weight of the font
sub rsp,32                   ; Create parameters shadow
call [CreateFont]
add rsp,32 + 80              ; Remove parameters shadow and 10 parameters
mov [r14 + ABOUTBOX.hFont],rax 
jmp .statusOne

;------------------------------------------------------------------------------;
;                                                                              ;
;                     WM_CLOSE handler: close window.                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmclose:
mov rcx,[r14 + ABOUTBOX.hFont]
jrcxz @f
call [DeleteObject]
@@:
mov rcx,rbx
xor edx,edx
call [EndDialog]
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;                 WM_COMMAND handler: interpreting user input.                 ; 
;                 Detect click "OK" button at "About" window.                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
.wmcommand:
cmp r8w,IDB_OK
je .wmclose
jmp .statusZero

;------------------------------------------------------------------------------;
;                                                                              ;
;        WM_LBUTTONDOWN handler: interpreting mouse left button click.         ; 
;          Provide mouse cursor consistency and go web link if click.          ; 
;                                                                              ;
;------------------------------------------------------------------------------;
.wmlbuttondown:
call HelperCheckLine4
lea r8,[linkGitHub]
jc .clicked 
call HelperCheckLine5
lea r8,[linkFasm]
jnc .statusZero
.clicked:
mov rsi,r8
mov rcx,[r14 + ABOUTBOX.hCursor]
jrcxz @f
call [SetCursor]
@@:
mov r8,rsi
push SW_NORMAL
push 0
xor r9d,r9d
xor edx,edx
xor ecx,ecx
sub rsp,32
call [ShellExecute]
add rsp,32 + 16
cmp rax,32
ja @f
mov r9d,MB_ICONERROR   ; Parm#4 = Attributes
xor r8d,r8d            ; Parm#3 = Pointer to title (caption) string
lea rdx,[msgShell]     ; Parm#2 = Pointer to string: error name 
xor ecx,ecx            ; Parm#1 = Parent Window or NULL
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
call HelperCheckLine4
jc @f
call HelperCheckLine5
jnc .statusZero
@@:
.change:
mov rcx,[r14 + ABOUTBOX.hCursor]
jrcxz @f
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
lea rsi,[r14 + ABOUTBOX.ps]
mov rdx,rsi
; rcx valid here
call [BeginPaint]
test rax,rax
jz .statusZero
xchg r13,rax
lea rdi,[r14 + ABOUTBOX.rect]
push rsi rdi
add rsi,PAINTSTRUCT.rcPaint + RECT.left
mov ecx,3
cld
rep movsd
lodsd
sub eax,YBOTTOM
stosd
pop rdi rsi
mov r8d,COLOR_WINDOW + 1
mov rdx,rdi
mov rcx,r13
call [FillRect] 
mov r9,[hIcons + 08]
mov r8d,ICONY
mov edx,ICONX
mov rcx,r13
call [DrawIcon]
xor eax,eax
mov rdx,[r14 + ABOUTBOX.hFont]
test rdx,rdx
jz @f
mov rcx,r13
call [SelectObject]
@@:
mov [r14 + ABOUTBOX.hFontBack],rax
lea rdx,[r14 + ABOUTBOX.tm]
mov rcx,r13
call [GetTextMetrics]
;---------- Strings 1-3, without clickable elements ---------------------------;
push rsi
cld
lea rsi,[r14 + ABOUTBOX.s1]
.next1:
lodsq
xchg rdx,rax
test rdx,rdx
jz .stop1
lodsb
movzx eax,al
mov [rdi + RECT.top],eax
add eax,YADD1
mov [rdi + RECT.bottom],eax
call HelperCenterText
jmp .next1
.stop1:
pop rsi
;---------- Strings 4-5, with clickable elements ------------------------------;
push rbx
sub rsp,32 + 8
lea rbx,[r14 + ABOUTBOX.s4]
.next2:
mov rdx,[rbx + LINEVAR.pointer]
test rdx,rdx
jz .stop2
movzx eax,byte [rbx + LINEVAR.vertical]
mov [rbx + LINEVAR.ymin],eax
add eax,[r14 + ABOUTBOX.tm + TEXTMETRIC.tmHeight]
mov [rbx + LINEVAR.ymax],eax
movzx r8d,byte [rbx + LINEVAR.xs1]
call HelperTextSize
mov edx,[rsi + PAINTSTRUCT.rcPaint + RECT.right]
sub edx,[rsi + PAINTSTRUCT.rcPaint + RECT.left]
sub edx,eax
shr edx,1
mov r12d,edx
mov [rdi + RECT.left],edx
movzx eax,byte [rbx + LINEVAR.vertical]
mov [rdi + RECT.top],eax
add eax,YADD2
mov [rdi + RECT.bottom],eax
movzx r8d,byte [rbx + LINEVAR.xs2]
mov rdx,[rbx + LINEVAR.pointer]
call HelperLeftText
movzx r8d,byte [rbx + LINEVAR.xs2]
mov rdx,[rbx + LINEVAR.pointer]
call HelperTextSize
add eax,r12d
mov [rdi + RECT.left],eax
mov [rbx + LINEVAR.xmin],eax
mov edx,00FF0000h
mov rcx,r13
call [SetTextColor]
push rax
movzx r8d,byte [rbx + LINEVAR.xs3]
movzx edx,byte [rbx + LINEVAR.xs2]
add rdx,[rbx + LINEVAR.pointer]
call HelperLeftText
pop rdx
mov rcx,r13
call [SetTextColor]
movzx r8d,byte [rbx + LINEVAR.xs2]
add r8b,[rbx + LINEVAR.xs3]
mov rdx,[rbx + LINEVAR.pointer]
call HelperTextSize
add eax,r12d
mov [rdi + RECT.left],eax
mov [rbx + LINEVAR.xmax],eax
mov r8,-1
movzx edx,byte [rbx + LINEVAR.xs2]
add dl,[rbx + LINEVAR.xs3]
add rdx,[rbx + LINEVAR.pointer]
call HelperLeftText
add rbx,sizeof.LINEVAR
jmp .next2 
.stop2:
add rsp,32 + 8
pop rbx
;---------- Paint window elements done ----------------------------------------;
mov rdx,[r14 + ABOUTBOX.hFontBack]
test rdx,rdx
jz @f
mov rcx,r13
call [SelectObject]
@@:
mov rdx,rsi
mov rcx,rbx
call [EndPaint]
jmp .statusZero

;---------- Helper for detect mouse cursor position at clickable text ---------;
;                                                                              ;
; INPUT  :  R9D = Mouse cursor window position, bits[31-16]=Y, bits[15-0]=X    ;
;           R14 = Pointer to ABOUT_BOX data structure                          ;
;           Memory structure (LINEVAR) used                                    ; 
;                                                                              ;
; OUTPUT :  CF = result, 0 = Mouse cursor outside, 1 = Indide                  ;
;                                                                              ;
;------------------------------------------------------------------------------; 
HelperCheckLine4:
lea rcx,[r14 + ABOUTBOX.s4 + LINEVAR.xmin]  ; Pointer for clickable string 1
jmp helperCheckLine
HelperCheckLine5:
lea rcx,[r14 + ABOUTBOX.s5 + LINEVAR.xmin]  ; Pointer for clickable string 2
helperCheckLine:
movzx eax,r9w
mov edx,r9d
shr edx,16
cmp eax,[rcx + 00]
jb @f
cmp eax,[rcx + 04]
ja @f
cmp edx,[rcx + 08]
jb @f
cmp edx,[rcx + 12]
ja @f
stc
ret
@@:
clc
ret

;---------- Helper for draw text string ---------------------------------------;
;                                                                              ;
; INPUT  : R13 = Handle Device Context (HDC)                                   ;
;          RDX = Pointer to text string                                        ;
;          R8  = Length of text string or -1 if 0-terminated string            ;
;          RDI = Pointer to RECT structure for string positioning at window    ;
;                                                                              ;
; OUTPUT : None                                                                ;
;                                                                              ;
;------------------------------------------------------------------------------; 
HelperLeftText:
mov eax,DT_LEFT + DT_NOPREFIX    ; Entry point for Left-aligned, R8=Length
jmp helperText
HelperCenterText:
mov eax,DT_CENTER + DT_NOPREFIX  ; Entry point for Center-aligned, 0-terminated
mov r8d,-1
helperText:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
push 0
push rax 
sub rsp,32
mov r9,rdi
mov rcx,r13
call [DrawText]
ExitRbp:
mov rsp,rbp
pop rbp
ret

;---------- Helper for get text string pixel width ----------------------------;
;                                                                              ;
; INPUT  : R13 = Handle Device Context (HDC)                                   ;
;          RDX = Pointer to text string, string without 0 termination          ;
;          R8  = Length of text string                                         ;
;          R14 = Pointer to ABOUT_BOX data structure                           ;
;                                                                              ;
; OUTPUT : EAX = pixel width                                                   ;
;          Memory structure (sz) updated                                       ;
;                                                                              ;
;------------------------------------------------------------------------------; 
HelperTextSize:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
lea r9,[r14 + ABOUTBOX.sz]
mov rcx,r13
call [GetTextExtentPoint32]
mov eax,[r14 + ABOUTBOX.sz + SIZE.cx]
jmp ExitRbp

;---------- Print 32-bit Decimal Number ---------------------------------------;
;                                                                              ;
; INPUT:   EAX = Number value                                                  ;
;          BL  = Template size, chars. 0=No template                           ;
;          RDI = Destination Pointer (flat)                                    ;
;                                                                              ;
; OUTPUT:  RDI = New Destination Pointer (flat)                                ;
;                modified because string write                                 ;
;                                                                              ;
;------------------------------------------------------------------------------;
DecimalPrint32:
cld
push rax rbx rcx rdx
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
push rdx              ; Push remainder
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax          ; ECX = Quotient, used as divisor and cycle condition 
pop rax              ; EAX = remainder
inc bh
test ecx,ecx
jnz .mainCycle       ; Cycle if (unsigned) quotient still > 0 
pop rdx rcx rbx rax
ret

;---------- Copy text string terminated by 00h --------------------------------;
; Note last byte 00h not copied.                                               ;
;                                                                              ;
; INPUT:   RSI = Source address                                                ;
;          RDI = Destination address                                           ;
;                                                                              ;
; OUTPUT:  RSI = Modified by copy                                              ;
;          RDI = Modified by copy                                              ;
;          Memory at [Input RDI] modified                                      ;
;                                                                              ;
;------------------------------------------------------------------------------;
StringWrite:
cld
.cycle:
lodsb
cmp al,0
je .exit
stosb
jmp .cycle
.exit:
ret

; Connect modules under debug.
include 'Draw_64.inc'
; include 'Progress_64_1.inc'
; include 'Progress_64_2.inc'
  include 'Progress_64_3.inc'


section '.data' data readable writeable
; constants
appCtrl       INITCOMMONCONTROLSEX  8, 0
line1         DB  'NUMA CPU&RAM Benchmarks.'                    , 0
line2         DB  'v2.02.02 for Windows x64.'                   , 0
line3         DB  '(C) 2022 Ilya Manusov.'                      , 0
line4         DB  'More at GitHub.'                             , 0
line5         DB  'Developed with Flat Assembler.'              , 0
linkGitHub    DB  'https://github.com/manusov?tab=repositories' , 0
linkFasm      DB  'https://flatassembler.net/'                  , 0
msgShell      DB  'Shell error.'                                , 0

lineProgress  DB  'Please wait...'                              , 0

struct LINECONST
pointer   dq ?
vertical  db ?
ends

struct LINEVAR
pointer   dq  ?
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
e1a  dq    0

lines2a:
s4a  LINEVAR  line4, 106, 15,  8,  6, -1, -1, -1, -1 
s5a  LINEVAR  line5, 123, 30, 15, 14, -1, -1, -1, -1
e2a  dq    0

; variables
align 8
hInstance     DQ  ?
hWinMain      DQ  ?
hWinChild     DQ  ?
hWinDraw      DQ  ?
hWinProgress  DQ  ?
hIcons        DQ  COUNT_ICON  DUP (?) 

struct ABOUTBOX
hCursor      dq          ?
hFont        dq          ?
hFontBack    dq          ?
msg          MSG         ?
ps           PAINTSTRUCT ?
rect         RECT        ?
sz           SIZE        ?
tm           TEXTMETRIC  ?
s1           LINECONST   ?
s2           LINECONST   ?
s3           LINECONST   ?
e1           dq          ?
s4           LINEVAR     ?
s5           LINEVAR     ?
e2           dq          ?
ends
align 8
ABOUT_BOX ABOUTBOX ?

struct DRAWBOX
hCursor      dq          ?
hFont        dq          ?
hFontBack    dq          ?
msg          MSG         ?
ps           PAINTSTRUCT ?
rect         RECT        ?
sz           SIZE        ?
tm           TEXTMETRIC  ?
ends
align 8
DRAW_BOX     DRAWBOX ?

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

; TODO. Inspect unused fields in this structure.
struct PROGRESSBOX
hCursor      dq          ?
hFont        dq          ?
hFontBack    dq          ?

hFont1       dq          ?
hFont2       dq          ?

msg          MSG         ?
ps           PAINTSTRUCT ?
rect         RECT        ?
sz           SIZE        ?
tm           TEXTMETRIC  ?
; additions for progress window
hPen                dq  ?            ; pen for progress percentage cycle
progressPercentage  dd  ?            ; percentage integer value, 0-100

; TODO. Can be same field for progressString and operationString, 40 bytes.

progressString      db  5  dup (?)   ; maximum string length = 5 , if "100%",0
operationString     db  40 dup (?)  

oldPoint         POINT  ?
alpha               dq  ?
delta               dq  ?
a                   dd  ?
b                   dd  ?
c                   dd  ?

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
dialog mainDialog, 'Application main window (x64)', 100, 100, 200, 150, \
       WS_CAPTION + WS_SYSMENU + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem 'BUTTON', 'Open progress' , IDB_CHILD_P, 120,  57, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open draw'     , IDB_CHILD_D, 120,  72, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open about'    , IDB_CHILD_A, 120,  87, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open modal'    , IDB_CHILD_1, 120, 102, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Open modeless' , IDB_CHILD_2, 120, 117, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'BUTTON', 'Exit'          , IDB_EXIT   , 120, 132, 75, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog 
dialog progressChild, '', 220, 120, 80,  80, \
       WS_BORDER + WS_POPUP + WS_VISIBLE, 0, 0, 'Verdana', 10
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
icon exegicon   , exeicon   , 'images\fasm64.ico'
icon booksgicon , booksicon , 'images\books.ico'
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
