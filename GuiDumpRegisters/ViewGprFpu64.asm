include 'win64a.inc'
ID_MAIN        = 100
ID_EXE_ICON    = 101
ID_EXE_ICONS   = 102
ID_GUI_STRINGS = 103
ID_GUI_BINDERS = 104
IDR_RAX_NAME   = 200
IDR_RBX_NAME   = 201
IDR_RCX_NAME   = 202
IDR_RDX_NAME   = 203
IDR_RSP_NAME   = 204
IDR_RBP_NAME   = 205
IDR_RSI_NAME   = 206
IDR_RDI_NAME   = 207
IDR_R8_NAME    = 208
IDR_R9_NAME    = 209
IDR_R10_NAME   = 210
IDR_R11_NAME   = 211
IDR_R12_NAME   = 212
IDR_R13_NAME   = 213
IDR_R14_NAME   = 214
IDR_R15_NAME   = 215
IDR_ST0_NAME   = 216
IDR_ST1_NAME   = 217
IDR_ST2_NAME   = 218
IDR_ST3_NAME   = 219
IDR_ST4_NAME   = 220
IDR_ST5_NAME   = 221
IDR_ST6_NAME   = 222
IDR_ST7_NAME   = 223
IDR_RAX_VALUE  = 300
IDR_RBX_VALUE  = 301
IDR_RCX_VALUE  = 302
IDR_RDX_VALUE  = 303
IDR_RSP_VALUE  = 304
IDR_RBP_VALUE  = 305
IDR_RSI_VALUE  = 306
IDR_RDI_VALUE  = 307
IDR_R8_VALUE   = 308
IDR_R9_VALUE   = 309
IDR_R10_VALUE  = 310
IDR_R11_VALUE  = 311
IDR_R12_VALUE  = 312
IDR_R13_VALUE  = 313
IDR_R14_VALUE  = 314
IDR_R15_VALUE  = 315
IDR_ST0_VALUE  = 316
IDR_ST1_VALUE  = 317
IDR_ST2_VALUE  = 318
IDR_ST3_VALUE  = 319
IDR_ST4_VALUE  = 320
IDR_ST5_VALUE  = 321
IDR_ST6_VALUE  = 322
IDR_ST7_VALUE  = 323
STRING_APP     = 0
STRING_RAX     = 1
STRING_RBX     = 2
STRING_RCX     = 3
STRING_RDX     = 4
STRING_RSP     = 5
STRING_RBP     = 6
STRING_RSI     = 7
STRING_RDI     = 8
STRING_R8      = 9
STRING_R9      = 10
STRING_R10     = 11
STRING_R11     = 12
STRING_R12     = 13
STRING_R13     = 14
STRING_R14     = 15
STRING_R15     = 16
STRING_ST0     = 17
STRING_ST1     = 18
STRING_ST2     = 19
STRING_ST3     = 20
STRING_ST4     = 21
STRING_ST5     = 22
STRING_ST6     = 23
STRING_ST7     = 24
BUFFER_RAX     = TEXT_PER_REG * 0
BUFFER_RBX     = TEXT_PER_REG * 1
BUFFER_RCX     = TEXT_PER_REG * 2
BUFFER_RDX     = TEXT_PER_REG * 3
BUFFER_RSP     = TEXT_PER_REG * 4
BUFFER_RBP     = TEXT_PER_REG * 5
BUFFER_RSI     = TEXT_PER_REG * 6
BUFFER_RDI     = TEXT_PER_REG * 7
BUFFER_R8      = TEXT_PER_REG * 8
BUFFER_R9      = TEXT_PER_REG * 9
BUFFER_R10     = TEXT_PER_REG * 10
BUFFER_R11     = TEXT_PER_REG * 11
BUFFER_R12     = TEXT_PER_REG * 12
BUFFER_R13     = TEXT_PER_REG * 13
BUFFER_R14     = TEXT_PER_REG * 14
BUFFER_R15     = TEXT_PER_REG * 15
BUFFER_ST0     = TEXT_PER_REG * 16
BUFFER_ST1     = TEXT_PER_REG * 17
BUFFER_ST2     = TEXT_PER_REG * 18
BUFFER_ST3     = TEXT_PER_REG * 19
BUFFER_ST4     = TEXT_PER_REG * 20
BUFFER_ST5     = TEXT_PER_REG * 21
BUFFER_ST6     = TEXT_PER_REG * 22
BUFFER_ST7     = TEXT_PER_REG * 23
BIND_APP_GUI   = 0
X_SIZE         = 129
Y_SIZE         = 242
COUNT_GPR      = 16
COUNT_FPU      = 8
TEXT_PER_REG   = 17
INFO_SIZE      = 8192
MISC_SIZE      = 8192
macro BIND_STOP
{ DB  0 }
MACRO BIND_STRING srcid, dstid
{ DD 01h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_INFO srcid, dstid
{ DD 02h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_BOOL srcid, srcbit, dstid
{ DD 03h + (srcid) SHL 9 + (srcbit) SHL 6 + (dstid) SHL 19 }
COMBO_STOP_ON  = 0
COMBO_STOP_OFF = 1
COMBO_CURRENT  = 2
COMBO_ADD      = 3
COMBO_INACTIVE = 4
MACRO BIND_COMBO srcid, dstid
{ DD 04h + (srcid) SHL 6 + (dstid) SHL 19 }

format PE64 GUI
entry start
section '.code' code readable executable
start:

sub rsp,8*5
cld
lea rcx,[AppControl]
call [InitCommonControlsEx]
test rax,rax
jz .guifailed
xor ecx,ecx
call [GetModuleHandle]
test rax,rax
jz .guifailed
mov [HandleThis],rax
mov edx,ID_EXE_ICONS
xchg rcx,rax 
call [LoadIcon]
test rax,rax
jz .guifailed
mov [HandleIcon],rax
mov r8d,RT_RCDATA
mov edx,ID_GUI_STRINGS
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .guifailed 
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .guifailed
xchg rcx,rax
call [LockResource]  
test rax,rax
jz .guifailed
mov [LockedStrings],rax
mov r8d,RT_RCDATA
mov edx,ID_GUI_BINDERS
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .guifailed
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .guifailed
xchg rcx,rax
call [LockResource]  
test rax,rax
jz .guifailed
mov [LockedBinders],rax

mov ebx,21
pushf
pop rax
bts eax,ebx
push rax
popf
pushf
pop rax
btr eax,ebx
jnc .systemFailed
push rax
popf
pushf
pop rax
btr eax,ebx
jc .systemFailed
xor eax,eax
cpuid
cmp eax,1
jb .systemFailed
mov eax,1
cpuid
test edx,1
jz .systemFailed  

mov rax,rsp
mov rbx,2
mov rcx,3
mov rdx,4
mov rbp,01111111111111111h
mov rsi,05555555555555555h
mov rdi,0AAAAAAAAAAAAAAAAh
mov r8,0123456789ABCDEFh
mov r9,0FEDCBA9876543210h
mov r10,100h
mov r11,101h
mov r12,102h
mov r13,103h
mov r14,104h
mov r15,105h 

finit
fldpi
fldz
fld1
fld1
fchs

push r15 r14 r13 r12 r11 r10 r9 r8
push rdi rsi rbp
lea r8,[rsp + 8*11] 
push r8 rdx rcx rbx rax 
lea rdi,[INFO_BUFFER]
mov ecx,COUNT_GPR
.hexDump:
pop rax
call HexPrint64
mov al,0
stosb
loop .hexDump
mov ecx,COUNT_FPU
sub rsp,64
fstp qword [rsp + 8*0]
fstp qword [rsp + 8*1]
fstp qword [rsp + 8*2]
fstp qword [rsp + 8*3]
fstp qword [rsp + 8*4]
fstp qword [rsp + 8*5]
fstp qword [rsp + 8*6]
fstp qword [rsp + 8*7]
.fpuDump:
pop rax
mov bx,0800h
push rdi
mov rsi,rdi
call DoublePrint
mov al,0
stosb
xchg rax,rdi
pop rdi
add rdi,TEXT_PER_REG - 1
sub rax,rdi
cmp rax,7
jle .stDone
mov word [rsi],0000h + '?' 
.stDone:
mov al,0
stosb
loop .fpuDump

push 0 0 
lea r9,[DialogProc]
mov r8d,HWND_DESKTOP
mov edx,ID_MAIN
mov rcx,[HandleThis]  
sub rsp,32
call [DialogBoxParam] 
add rsp,32+16
test rax,rax
jz .guifailed 
cmp rax,-1
je .guifailed
.ok:
xor ecx,ecx           
call [ExitProcess]
.guifailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgErrorGUI] 
xor ecx,ecx
call [MessageBox]  
mov ecx,1           
call [ExitProcess]
.systemFailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgErrorFPU] 
xor ecx,ecx
call [MessageBox]  
mov ecx,2           
call [ExitProcess]

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
jmp .processed
.wmcommand:
jmp .processed
.wmclose:
mov edx,1
mov rcx,[rbp + 40 + 00]
call [EndDialog]
.processed:
mov eax,1
.finish:
mov rsp,rbp
pop rdi rsi rbx rbp
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
cmp byte [rsi],0
jne .foundBinder
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
ret
BindInfo:
lea rsi,[INFO_BUFFER + rax]
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

DoublePrint:
push rax rbx rcx rdx r8 r9 r10 r11
cld
mov rdx,07FFFFFFFFFFFFFFFh
and rdx,rax
jz .fp64_Zero
mov rcx,07FF8000000000000h
cmp rdx,rcx
je .fp64_QNAN
mov rcx,07FF0000000000000h
cmp rdx,rcx
je .fp64_INF
ja .fp64_NAN
finit
push rax
push rax
fstcw [rsp]
pop rax
or ax,0C00h
push rax
fldcw [rsp]
pop rax
fld qword [rsp]
pop rax
fld st0
frndint
fxch
fsub st0,st1
mov eax,1
movzx ecx,bh
jrcxz .orderDetected
@@:
imul rax,rax,10
loop @b
.orderDetected:
push rax
fimul dword [rsp]
pop rax
push rax rax
fbstp [rsp]
pop r8 r9
push rax rax
fbstp [rsp]
pop r10 r11
bt r11,15
setc dl
bt r9,15
setc dh
test dx,dx
jz @f
mov al,'-'
stosb
@@:
mov dl,0
mov ecx,18 
.cycleInteger:
mov al,r11l
shr al,4
cmp cl,1
je .store
cmp cl,bl
jbe .store
test dl,dl
jnz .store
test al,al
jz .position 
.store:
mov dl,1
or al,30h
stosb
.position:
shld r11,r10,4
shl r10,4
loop .cycleInteger
test bh,bh
jz .exit
mov al,'.'
stosb
std 
movzx ecx,bh     
lea rdi,[rdi + rcx]
push rdi
dec rdi
.cycleFloat:
mov al,r8l
and al,0Fh
or al,30h
stosb
shrd r8,r9,4
shr r9,4
loop .cycleFloat
pop rdi
cld
jmp .exit
.fp64_Zero:
mov eax,'0.0 '
jmp .fp64special
.fp64_INF:
mov eax,'INF '
jmp .fp64special
.fp64_NAN:
mov eax,'NAN '
jmp .fp64special
.fp64_QNAN:
mov eax,'QNAN'
.fp64special:
stosd
jmp .exit
.error:
mov al,'?'
stosb
.exit:
finit
pop r11 r10 r9 r8 rdx rcx rbx rax
ret

section '.data' data readable writeable
MsgErrorGUI    DB  'GUI initialization failed.',0
MsgErrorFPU    DB  'x87 FPU absent or locked.',0
AppControl     INITCOMMONCONTROLSEX  8, 0
ProcBinders    DQ  BindString
               DQ  BindInfo
               DQ  BindBool
               DQ  BindCombo
ProcCombo      DQ  BindComboStopOn
               DQ  BindComboStopOff 
               DQ  BindComboCurrent
               DQ  BindComboAdd
               DQ  BindComboInactive
HandleThis     DQ  ? 
HandleIcon     DQ  ?
LockedStrings  DQ  ?
LockedBinders  DQ  ?
align 4096 
INFO_BUFFER    DB  INFO_SIZE DUP (?)
MISC_BUFFER    DB  MISC_SIZE DUP (?)  

section '.idata' import data readable writeable
library kernel32 , 'kernel32.dll', \
        user32   , 'user32.dll'  , \
        comctl32 , 'comctl32.dll'
include 'api\kernel32.inc'
include 'api\user32.inc'
include 'api\comctl32.inc'

section '.rsrc' resource data readable
directory RT_DIALOG     , dialogs   , \
          RT_RCDATA     , raws      , \ 
          RT_ICON       , icons     , \
          RT_GROUP_ICON , gicons    , \
          RT_MANIFEST   , manifests
resource dialogs, ID_MAIN, LANG_ENGLISH + SUBLANG_DEFAULT, mydialog
dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU + WS_SIZEBOX, 0, 0, 'Verdana monospace', 8
dialogitem 'STATIC', '', IDR_RAX_NAME  ,  5,   9,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RBX_NAME  ,  5,  18,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RCX_NAME  ,  5,  27,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RDX_NAME  ,  5,  36,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RSP_NAME  ,  5,  45,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RBP_NAME  ,  5,  54,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RSI_NAME  ,  5,  63,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RDI_NAME  ,  5,  72,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R8_NAME   ,  5,  81,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R9_NAME   ,  5,  90,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R10_NAME  ,  5,  99,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R11_NAME  ,  5, 108,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R12_NAME  ,  5, 117,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R13_NAME  ,  5, 126,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R14_NAME  ,  5, 135,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R15_NAME  ,  5, 144,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST0_NAME  ,  5, 158,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST1_NAME  ,  5, 167,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST2_NAME  ,  5, 176,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST3_NAME  ,  5, 185,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST4_NAME  ,  5, 194,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST5_NAME  ,  5, 203,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST6_NAME  ,  5, 212,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST7_NAME  ,  5, 221,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RAX_VALUE , 34,   9,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RBX_VALUE , 34,  18,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RCX_VALUE , 34,  27,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RDX_VALUE , 34,  36,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RSP_VALUE , 34,  45,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RBP_VALUE , 34,  54,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RSI_VALUE , 34,  63,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_RDI_VALUE , 34,  72,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R8_VALUE  , 34,  81,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R9_VALUE  , 34,  90,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R10_VALUE , 34,  99,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R11_VALUE , 34, 108,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R12_VALUE , 34, 117,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R13_VALUE , 34, 126,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R14_VALUE , 34, 135,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_R15_VALUE , 34, 144,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST0_VALUE , 34, 158,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST1_VALUE , 34, 167,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST2_VALUE , 34, 176,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST3_VALUE , 34, 185,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST4_VALUE , 34, 194,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST5_VALUE , 34, 203,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST6_VALUE , 34, 212,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ST7_VALUE , 34, 221,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
enddialog
resource raws, ID_GUI_STRINGS, LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS, LANG_ENGLISH + SUBLANG_DEFAULT, guibinders
resdata guistrings
DB  'GPR64 and FPU (x64 v0.01)', 0
DB  'RAX', 0
DB  'RBX', 0
DB  'RCX', 0
DB  'RDX', 0
DB  'RSP', 0
DB  'RBP', 0
DB  'RSI', 0
DB  'RDI', 0
DB  'R8' , 0
DB  'R9' , 0
DB  'R10', 0
DB  'R11', 0
DB  'R12', 0
DB  'R13', 0
DB  'R14', 0
DB  'R15', 0
DB  'ST0', 0
DB  'ST1', 0
DB  'ST2', 0
DB  'ST3', 0
DB  'ST4', 0
DB  'ST5', 0
DB  'ST6', 0
DB  'ST7', 0
endres
resdata guibinders
BIND_STRING  STRING_RAX , IDR_RAX_NAME
BIND_STRING  STRING_RBX , IDR_RBX_NAME
BIND_STRING  STRING_RCX , IDR_RCX_NAME
BIND_STRING  STRING_RDX , IDR_RDX_NAME
BIND_STRING  STRING_RSP , IDR_RSP_NAME
BIND_STRING  STRING_RBP , IDR_RBP_NAME
BIND_STRING  STRING_RSI , IDR_RSI_NAME
BIND_STRING  STRING_RDI , IDR_RDI_NAME
BIND_STRING  STRING_R8  , IDR_R8_NAME
BIND_STRING  STRING_R9  , IDR_R9_NAME
BIND_STRING  STRING_R10 , IDR_R10_NAME
BIND_STRING  STRING_R11 , IDR_R11_NAME
BIND_STRING  STRING_R12 , IDR_R12_NAME
BIND_STRING  STRING_R13 , IDR_R13_NAME
BIND_STRING  STRING_R14 , IDR_R14_NAME
BIND_STRING  STRING_R15 , IDR_R15_NAME
BIND_STRING  STRING_ST0 , IDR_ST0_NAME
BIND_STRING  STRING_ST1 , IDR_ST1_NAME
BIND_STRING  STRING_ST2 , IDR_ST2_NAME
BIND_STRING  STRING_ST3 , IDR_ST3_NAME
BIND_STRING  STRING_ST4 , IDR_ST4_NAME
BIND_STRING  STRING_ST5 , IDR_ST5_NAME
BIND_STRING  STRING_ST6 , IDR_ST6_NAME
BIND_STRING  STRING_ST7 , IDR_ST7_NAME
BIND_INFO    BUFFER_RAX , IDR_RAX_VALUE
BIND_INFO    BUFFER_RBX , IDR_RBX_VALUE
BIND_INFO    BUFFER_RCX , IDR_RCX_VALUE
BIND_INFO    BUFFER_RDX , IDR_RDX_VALUE
BIND_INFO    BUFFER_RSP , IDR_RSP_VALUE
BIND_INFO    BUFFER_RBP , IDR_RBP_VALUE
BIND_INFO    BUFFER_RSI , IDR_RSI_VALUE
BIND_INFO    BUFFER_RDI , IDR_RDI_VALUE
BIND_INFO    BUFFER_R8  , IDR_R8_VALUE
BIND_INFO    BUFFER_R9  , IDR_R9_VALUE
BIND_INFO    BUFFER_R10 , IDR_R10_VALUE
BIND_INFO    BUFFER_R11 , IDR_R11_VALUE
BIND_INFO    BUFFER_R12 , IDR_R12_VALUE
BIND_INFO    BUFFER_R13 , IDR_R13_VALUE
BIND_INFO    BUFFER_R14 , IDR_R14_VALUE
BIND_INFO    BUFFER_R15 , IDR_R15_VALUE
BIND_INFO    BUFFER_ST0 , IDR_ST0_VALUE
BIND_INFO    BUFFER_ST1 , IDR_ST1_VALUE
BIND_INFO    BUFFER_ST2 , IDR_ST2_VALUE
BIND_INFO    BUFFER_ST3 , IDR_ST3_VALUE
BIND_INFO    BUFFER_ST4 , IDR_ST4_VALUE
BIND_INFO    BUFFER_ST5 , IDR_ST5_VALUE
BIND_INFO    BUFFER_ST6 , IDR_ST6_VALUE
BIND_INFO    BUFFER_ST7 , IDR_ST7_VALUE
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
db '    name="Shield Icon Demo"'
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

