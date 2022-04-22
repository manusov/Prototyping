include 'win32a.inc'
ID_MAIN           = 100
ID_EXE_ICON       = 101
ID_EXE_ICONS      = 102
ID_GUI_STRINGS    = 103
ID_GUI_BINDERS    = 104
IDR_EAX_NAME      = 200
IDR_EBX_NAME      = 201
IDR_ECX_NAME      = 202
IDR_EDX_NAME      = 203
IDR_ESP_NAME      = 204
IDR_EBP_NAME      = 205
IDR_ESI_NAME      = 206
IDR_EDI_NAME      = 207
IDR_MM0_NAME      = 208
IDR_MM1_NAME      = 209
IDR_MM2_NAME      = 210
IDR_MM3_NAME      = 211
IDR_MM4_NAME      = 212
IDR_MM5_NAME      = 213
IDR_MM6_NAME      = 214
IDR_MM7_NAME      = 215
IDR_XMM0_NAME     = 300
IDR_XMM1_NAME     = 301
IDR_XMM2_NAME     = 302
IDR_XMM3_NAME     = 303
IDR_XMM4_NAME     = 304
IDR_XMM5_NAME     = 305
IDR_XMM6_NAME     = 306
IDR_XMM7_NAME     = 307
IDR_EAX_VALUE     = 400
IDR_EBX_VALUE     = 401
IDR_ECX_VALUE     = 402
IDR_EDX_VALUE     = 403
IDR_ESP_VALUE     = 404
IDR_EBP_VALUE     = 405
IDR_ESI_VALUE     = 406
IDR_EDI_VALUE     = 407
IDR_MM0_VALUE     = 408
IDR_MM1_VALUE     = 409
IDR_MM2_VALUE     = 410
IDR_MM3_VALUE     = 411
IDR_MM4_VALUE     = 412
IDR_MM5_VALUE     = 413
IDR_MM6_VALUE     = 414
IDR_MM7_VALUE     = 415
IDR_XMM0_0_VALUE  = 500
IDR_XMM0_1_VALUE  = 501
IDR_XMM1_0_VALUE  = 502
IDR_XMM1_1_VALUE  = 503
IDR_XMM2_0_VALUE  = 504
IDR_XMM2_1_VALUE  = 505
IDR_XMM3_0_VALUE  = 506
IDR_XMM3_1_VALUE  = 507
IDR_XMM4_0_VALUE  = 508
IDR_XMM4_1_VALUE  = 509
IDR_XMM5_0_VALUE  = 510
IDR_XMM5_1_VALUE  = 511
IDR_XMM6_0_VALUE  = 512
IDR_XMM6_1_VALUE  = 513
IDR_XMM7_0_VALUE  = 514
IDR_XMM7_1_VALUE  = 515
STRING_APP        = 0
STRING_EAX        = 1
STRING_EBX        = 2
STRING_ECX        = 3
STRING_EDX        = 4
STRING_ESP        = 5
STRING_EBP        = 6
STRING_ESI        = 7
STRING_EDI        = 8
STRING_MM         = 9
STRING_XMM        = 10
BUFFER_EAX        = TEXT_PER_REG * 0
BUFFER_EBX        = TEXT_PER_REG * 1
BUFFER_ECX        = TEXT_PER_REG * 2
BUFFER_EDX        = TEXT_PER_REG * 3
BUFFER_ESP        = TEXT_PER_REG * 4
BUFFER_EBP        = TEXT_PER_REG * 5
BUFFER_ESI        = TEXT_PER_REG * 6
BUFFER_EDI        = TEXT_PER_REG * 7
BUFFER_MM0        = TEXT_PER_REG * 8
BUFFER_MM1        = TEXT_PER_REG * 9
BUFFER_MM2        = TEXT_PER_REG * 10
BUFFER_MM3        = TEXT_PER_REG * 11
BUFFER_MM4        = TEXT_PER_REG * 12
BUFFER_MM5        = TEXT_PER_REG * 13
BUFFER_MM6        = TEXT_PER_REG * 14
BUFFER_MM7        = TEXT_PER_REG * 15
BUFFER_XMM0_0     = TEXT_PER_REG * 16
BUFFER_XMM0_1     = TEXT_PER_REG * 17 
BUFFER_XMM1_0     = TEXT_PER_REG * 18 
BUFFER_XMM1_1     = TEXT_PER_REG * 19 
BUFFER_XMM2_0     = TEXT_PER_REG * 20 
BUFFER_XMM2_1     = TEXT_PER_REG * 21 
BUFFER_XMM3_0     = TEXT_PER_REG * 22 
BUFFER_XMM3_1     = TEXT_PER_REG * 23 
BUFFER_XMM4_0     = TEXT_PER_REG * 24 
BUFFER_XMM4_1     = TEXT_PER_REG * 25 
BUFFER_XMM5_0     = TEXT_PER_REG * 26 
BUFFER_XMM5_1     = TEXT_PER_REG * 27 
BUFFER_XMM6_0     = TEXT_PER_REG * 28 
BUFFER_XMM6_1     = TEXT_PER_REG * 29 
BUFFER_XMM7_0     = TEXT_PER_REG * 30 
BUFFER_XMM7_1     = TEXT_PER_REG * 31 
BUFFER_N_MM0      = TEXT_PER_REG * 32
BUFFER_N_MM1      = BUFFER_N_MM0 + TEXT_PER_MM * 1 
BUFFER_N_MM2      = BUFFER_N_MM0 + TEXT_PER_MM * 2
BUFFER_N_MM3      = BUFFER_N_MM0 + TEXT_PER_MM * 3
BUFFER_N_MM4      = BUFFER_N_MM0 + TEXT_PER_MM * 4
BUFFER_N_MM5      = BUFFER_N_MM0 + TEXT_PER_MM * 5
BUFFER_N_MM6      = BUFFER_N_MM0 + TEXT_PER_MM * 6
BUFFER_N_MM7      = BUFFER_N_MM0 + TEXT_PER_MM * 7  
BUFFER_N_XMM0     = BUFFER_N_MM0 + TEXT_PER_MM * 8 
BUFFER_N_XMM1     = BUFFER_N_XMM0 + TEXT_PER_XMM * 1
BUFFER_N_XMM2     = BUFFER_N_XMM0 + TEXT_PER_XMM * 2
BUFFER_N_XMM3     = BUFFER_N_XMM0 + TEXT_PER_XMM * 3
BUFFER_N_XMM4     = BUFFER_N_XMM0 + TEXT_PER_XMM * 4
BUFFER_N_XMM5     = BUFFER_N_XMM0 + TEXT_PER_XMM * 5
BUFFER_N_XMM6     = BUFFER_N_XMM0 + TEXT_PER_XMM * 6
BUFFER_N_XMM7     = BUFFER_N_XMM0 + TEXT_PER_XMM * 7
BIND_APP_GUI      = 0
X_SIZE            = 276
Y_SIZE            = 169
COUNT_GPR         = 8
COUNT_MMX         = 8
COUNT_SSE         = 8
COUNT_VECTOR      = 2
TEXT_PER_REG      = 17
TEXT_PER_DOUBLE   = 12
TEXT_PER_MM       = 4
TEXT_PER_XMM      = 6
INFO_SIZE         = 8192
MISC_SIZE         = 8192
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

mov ebx,21
pushf
pop eax
bts eax,ebx
push eax
popf
pushf
pop eax
btr eax,ebx
jnc .systemFailed
push eax
popf
pushf
pop eax
btr eax,ebx
jc .systemFailed
xor eax,eax
cpuid
cmp eax,1
jb .systemFailed
mov eax,1
cpuid
mov eax,06800001h
and edx,eax
cmp edx,eax
jne .systemFailed 

finit
fldpi
fld1
fldpi
fchs
fld1
fchs
sub esp,32
fstp qword [esp + 00]
fstp qword [esp + 08]
fstp qword [esp + 16]
fstp qword [esp + 24]
movupd xmm7,[esp + 00]
movupd xmm0,[esp + 16]
add esp,32
lea eax,[.X0]
movupd xmm4,[eax + 00]
movupd xmm5,[eax + 16] 
jmp .skipData
.X0  DQ  1234567891.234
     DQ  12345678912.345    
     DQ  123456712345.89
     DQ  1234567891239.154 
.skipData:

mov eax,esp
mov ebx,2
mov ecx,3
mov edx,4
mov ebp,011111111h
mov esi,055555555h
mov edi,0AAAAAAAAh

movd mm0,esi
psllq mm0,32
movd mm1,edi
por mm0,mm1
movd mm7,ebp

push edi esi ebp
lea ebp,[esp + 4*3] 
push ebp edx ecx ebx eax 
lea edi,[INFO_BUFFER]
mov ecx,COUNT_GPR
.hexDump:
pop eax
call HexPrint32
mov al,0
stosb
add edi,TEXT_PER_REG - 9
loop .hexDump
mov ecx,COUNT_MMX
sub esp,64
movq qword [esp + 8*0],mm0
movq qword [esp + 8*1],mm1
movq qword [esp + 8*2],mm2
movq qword [esp + 8*3],mm3
movq qword [esp + 8*4],mm4
movq qword [esp + 8*5],mm5
movq qword [esp + 8*6],mm6
movq qword [esp + 8*7],mm7
.mmxDump:
pop eax edx
call HexPrint64
mov al,0
stosb
loop .mmxDump
mov cl,COUNT_SSE
sub esp,128
movupd [esp + 16*00],xmm0
movupd [esp + 16*01],xmm1
movupd [esp + 16*02],xmm2
movupd [esp + 16*03],xmm3
movupd [esp + 16*04],xmm4
movupd [esp + 16*05],xmm5
movupd [esp + 16*06],xmm6
movupd [esp + 16*07],xmm7
.sseDump:
mov bp,COUNT_VECTOR
.vectorDump:
pop eax edx
mov bx,0800h
push edi
mov esi,edi
call DoublePrint
mov al,0
stosb
xchg eax,edi
pop edi
sub eax,edi
add edi,TEXT_PER_REG - 1
cmp eax,TEXT_PER_DOUBLE
jle .doubleDone
cmp eax,TEXT_PER_DOUBLE + 9
jle .doubleCorrection
mov word [esi + 00],0000h + '?' 
.doubleCorrection:
mov byte [esi + TEXT_PER_DOUBLE],0
.doubleDone:
mov al,0
stosb
dec bp
jnz .vectorDump
loop .sseDump
mov ax,STRING_MM
call IndexString
xor ecx,ecx
.mmxNames:
push esi
call StringWrite
pop esi
movzx ax,cl
or al,30h
stosw
inc ecx
cmp ecx,COUNT_MMX
jb .mmxNames
mov ax,STRING_XMM
call IndexString
xor ecx,ecx
.sseNames:
push esi edi
call StringWrite
movzx eax,cl
mov bl,0
call DecimalPrint32
mov al,0
stosb
pop edi esi
add edi,TEXT_PER_XMM
inc ecx
cmp ecx,COUNT_SSE
jb .sseNames
emms

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
push MsgErrorGUI 
push 0
call [MessageBox]  
push 1           
call [ExitProcess]
.systemFailed:
push MB_ICONERROR
push 0
push MsgErrorCPU 
push 0
call [MessageBox]  
push 2           
call [ExitProcess]

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
jmp .processed
.wmcommand:
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
cmp byte [esi],0
jne .foundBinder
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

DoublePrint:
pushad
cld
test eax,eax
jnz @f
mov ecx,07FFFFFFFh
and ecx,edx
jz .fp64_Zero
cmp ecx,07FF80000h
je .fp64_QNAN
cmp ecx,07FF00000h
je .fp64_INF
ja .fp64_NAN
@@:
finit
push edx eax
push eax
fstcw [esp]
pop eax
or ax,0C00h
push eax
fldcw [esp]
pop eax
fld qword [esp]
pop eax edx
fld st0
frndint
fxch
fsub st0,st1
mov eax,1
movzx ecx,bh
jecxz .divisorDone
@@:
imul eax,eax,10
loop @b
.divisorDone:
push eax
fimul dword [esp]
pop eax
sub esp,32
fbstp [esp + 00]
fbstp [esp + 16]
test byte [esp + 16 + 09],80h
setnz dl
test byte [esp + 00 + 09],80h
setnz dh
test dx,dx
jz @f
mov al,'-'
stosb
@@:
mov cx,20 
mov edx,[esp + 16 + 06]  
mov esi,[esp + 16 + 02] 
mov ebp,[esp + 16 + 00] 
shl ebp,16
and edx,07FFFFFFFh
.cycleInteger:
mov eax,edx
shr eax,28
cmp cl,1
je .store
cmp cl,bl
jbe .store
test ch,ch
jnz .store
test al,al
jz .position 
.store:
mov ch,1
or al,30h
stosb
.position:
shld edx,esi,4
shld esi,ebp,4
shl ebp,4
dec cl
jnz .cycleInteger
test bh,bh
jz .exit
mov al,'.'
stosb
std 
movzx ecx,bh     
lea edi,[edi + ecx]
mov edx,[esp + 00 + 00]  
mov esi,[esp + 00 + 04] 
mov ebp,[esp + 00 + 00] 
push edi
dec edi
.cycleFloat:
mov al,dl
and al,0Fh
or al,30h
stosb
shrd edx,esi,4
shrd esi,ebp,4
shr ebp,4
loop .cycleFloat
pop edi
cld
add esp,32
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
.Error:
mov al,'?'
stosb
.exit:
finit
mov [esp],edi
popad
ret

section '.data' data readable writeable
MsgErrorGUI    DB  'GUI initialization failed.',0
MsgErrorCPU    DB  'CPU feature not supported or locked:', 0Dh, 0Ah
               DB  'required x87, MMX and SSE2.', 0
AppControl     INITCOMMONCONTROLSEX  8, 0
ProcBinders    DD  BindString
               DD  BindInfo
               DD  BindBool
               DD  BindCombo
ProcCombo      DD  BindComboStopOn
               DD  BindComboStopOff 
               DD  BindComboCurrent
               DD  BindComboAdd
               DD  BindComboInactive
HandleThis     DD  ? 
HandleIcon     DD  ?
LockedStrings  DD  ?
LockedBinders  DD  ?
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
dialogitem 'STATIC', '', IDR_EAX_NAME      ,  5,   9,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EBX_NAME      ,  5,  18,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ECX_NAME      ,  5,  27,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EDX_NAME      ,  5,  36,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ESP_NAME      ,  5,  45,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EBP_NAME      ,  5,  54,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ESI_NAME      ,  5,  63,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EDI_NAME      ,  5,  72,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM0_NAME      ,  5,  86,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM1_NAME      ,  5,  95,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM2_NAME      ,  5, 104,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM3_NAME      ,  5, 113,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM4_NAME      ,  5, 122,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM5_NAME      ,  5, 131,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM6_NAME      ,  5, 140,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM7_NAME      ,  5, 149,  28,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EAX_VALUE     , 34,   9,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EBX_VALUE     , 34,  18,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ECX_VALUE     , 34,  27,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EDX_VALUE     , 34,  36,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ESP_VALUE     , 34,  45,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EBP_VALUE     , 34,  54,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_ESI_VALUE     , 34,  63,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_EDI_VALUE     , 34,  72,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM0_VALUE     , 34,  86,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM1_VALUE     , 34,  95,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM2_VALUE     , 34, 104,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM3_VALUE     , 34, 113,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM4_VALUE     , 34, 122,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM5_VALUE     , 34, 131,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM6_VALUE     , 34, 140,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_MM7_VALUE     , 34, 149,  82,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM0_NAME     , 124,   9,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM1_NAME     , 124,  18,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM2_NAME     , 124,  27,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM3_NAME     , 124,  36,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM4_NAME     , 124,  45,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM5_NAME     , 124,  54,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM6_NAME     , 124,  63,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM7_NAME     , 124,  72,  36,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM0_0_VALUE  , 214,   9,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM0_1_VALUE  , 161,   9,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM1_0_VALUE  , 214,  18,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM1_1_VALUE  , 161,  18,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM2_0_VALUE  , 214,  27,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM2_1_VALUE  , 161,  27,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM3_0_VALUE  , 214,  36,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM3_1_VALUE  , 161,  36,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM4_0_VALUE  , 214,  45,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM4_1_VALUE  , 161,  45,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM5_0_VALUE  , 214,  54,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM5_1_VALUE  , 161,  54,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM6_0_VALUE  , 214,  63,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM6_1_VALUE  , 161,  63,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM7_0_VALUE  , 214,  72,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem 'STATIC', '', IDR_XMM7_1_VALUE  , 161,  72,  52,  8, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
enddialog
resource raws, ID_GUI_STRINGS, LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS, LANG_ENGLISH + SUBLANG_DEFAULT, guibinders
resdata guistrings
DB  'GPR32, MMX and SSE (ia32 v0.01)', 0
DB  'EAX', 0
DB  'EBX', 0
DB  'ECX', 0
DB  'EDX', 0
DB  'ESP', 0
DB  'EBP', 0
DB  'ESI', 0
DB  'EDI', 0
DB  'MM' , 0
DB  'XMM', 0
endres
resdata guibinders
BIND_STRING  STRING_EAX     , IDR_EAX_NAME
BIND_STRING  STRING_EBX     , IDR_EBX_NAME
BIND_STRING  STRING_ECX     , IDR_ECX_NAME
BIND_STRING  STRING_EDX     , IDR_EDX_NAME
BIND_STRING  STRING_ESP     , IDR_ESP_NAME
BIND_STRING  STRING_EBP     , IDR_EBP_NAME
BIND_STRING  STRING_ESI     , IDR_ESI_NAME
BIND_STRING  STRING_EDI     , IDR_EDI_NAME
BIND_INFO    BUFFER_N_MM0   , IDR_MM0_NAME
BIND_INFO    BUFFER_N_MM1   , IDR_MM1_NAME
BIND_INFO    BUFFER_N_MM2   , IDR_MM2_NAME
BIND_INFO    BUFFER_N_MM3   , IDR_MM3_NAME
BIND_INFO    BUFFER_N_MM4   , IDR_MM4_NAME
BIND_INFO    BUFFER_N_MM5   , IDR_MM5_NAME
BIND_INFO    BUFFER_N_MM6   , IDR_MM6_NAME
BIND_INFO    BUFFER_N_MM7   , IDR_MM7_NAME
BIND_INFO    BUFFER_N_XMM0  , IDR_XMM0_NAME
BIND_INFO    BUFFER_N_XMM1  , IDR_XMM1_NAME
BIND_INFO    BUFFER_N_XMM2  , IDR_XMM2_NAME
BIND_INFO    BUFFER_N_XMM3  , IDR_XMM3_NAME
BIND_INFO    BUFFER_N_XMM4  , IDR_XMM4_NAME
BIND_INFO    BUFFER_N_XMM5  , IDR_XMM5_NAME
BIND_INFO    BUFFER_N_XMM6  , IDR_XMM6_NAME
BIND_INFO    BUFFER_N_XMM7  , IDR_XMM7_NAME
BIND_INFO    BUFFER_EAX     , IDR_EAX_VALUE
BIND_INFO    BUFFER_EBX     , IDR_EBX_VALUE
BIND_INFO    BUFFER_ECX     , IDR_ECX_VALUE
BIND_INFO    BUFFER_EDX     , IDR_EDX_VALUE
BIND_INFO    BUFFER_ESP     , IDR_ESP_VALUE
BIND_INFO    BUFFER_EBP     , IDR_EBP_VALUE
BIND_INFO    BUFFER_ESI     , IDR_ESI_VALUE
BIND_INFO    BUFFER_EDI     , IDR_EDI_VALUE
BIND_INFO    BUFFER_MM0     , IDR_MM0_VALUE
BIND_INFO    BUFFER_MM1     , IDR_MM1_VALUE
BIND_INFO    BUFFER_MM2     , IDR_MM2_VALUE
BIND_INFO    BUFFER_MM3     , IDR_MM3_VALUE
BIND_INFO    BUFFER_MM4     , IDR_MM4_VALUE
BIND_INFO    BUFFER_MM5     , IDR_MM5_VALUE
BIND_INFO    BUFFER_MM6     , IDR_MM6_VALUE
BIND_INFO    BUFFER_MM7     , IDR_MM7_VALUE
BIND_INFO    BUFFER_XMM0_0  , IDR_XMM0_0_VALUE
BIND_INFO    BUFFER_XMM0_1  , IDR_XMM0_1_VALUE
BIND_INFO    BUFFER_XMM1_0  , IDR_XMM1_0_VALUE
BIND_INFO    BUFFER_XMM1_1  , IDR_XMM1_1_VALUE
BIND_INFO    BUFFER_XMM2_0  , IDR_XMM2_0_VALUE
BIND_INFO    BUFFER_XMM2_1  , IDR_XMM2_1_VALUE
BIND_INFO    BUFFER_XMM3_0  , IDR_XMM3_0_VALUE
BIND_INFO    BUFFER_XMM3_1  , IDR_XMM3_1_VALUE
BIND_INFO    BUFFER_XMM4_0  , IDR_XMM4_0_VALUE
BIND_INFO    BUFFER_XMM4_1  , IDR_XMM4_1_VALUE
BIND_INFO    BUFFER_XMM5_0  , IDR_XMM5_0_VALUE
BIND_INFO    BUFFER_XMM5_1  , IDR_XMM5_1_VALUE
BIND_INFO    BUFFER_XMM6_0  , IDR_XMM6_0_VALUE
BIND_INFO    BUFFER_XMM6_1  , IDR_XMM6_1_VALUE
BIND_INFO    BUFFER_XMM7_0  , IDR_XMM7_0_VALUE
BIND_INFO    BUFFER_XMM7_1  , IDR_XMM7_1_VALUE
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
db '    name="Shield Icon Demo"'
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

