; TODO.
; 1)  Add features requires PREFIX_CPUID, PREFIX_CPUID_S.
; 2)  Add logical CPU selection by combo box.
; 3)  Add string ", OS enabled" or ", OS disabled" to CPU features
; 4)  Add cache information by processor-specific methods.
; 5)  Add core(s) clock(s) at format: clocks | tsc=... , core=...
; 6)  Add virtual CPUID signature.
; 7)  Add Intel AMX CPUID and XCR0 detection.
; 8)  Verify extended CPUID support detection, EAX.31. Coppermine problem.
; 9)  Verify under Intel SDE emulator.
; 10) Regularize last empty string write. 0A0Dh value.


include 'win64a.inc'
CLEARTYPE_QUALITY    = 5
ID_MAIN              = 100
ID_EXE_ICON          = 101
ID_EXE_ICONS         = 102
ID_GUI_STRINGS       = 103
ID_GUI_BINDERS       = 104
ID_CPUID_FEATURES    = 105
ID_XCR0_FEATURES     = 106
IDR_UP_COMMON        = 200
IDR_TEXT_COMMON      = 201
IDR_UP_FEATURES      = 202      
IDR_TEXT_FEATURES    = 203
IDR_REPORT           = 204
IDR_BINARY           = 205
IDR_CANCEL           = 206
IDC_CPU_SELECT       = 207
STRING_APP           = 0
STRING_FONT          = 1
STRING_REPORT        = 2
STRING_BINARY        = 3
STRING_CANCEL        = 4
STRING_UP_COMMON     = 5
STRING_UP_FEATURES   = 6
STRING_NONE          = 7
STRING_FAILED        = 8
STRING_SIGNATURE     = 9
STRING_NAME          = 10
STRING_CLOCK         = 11
STRING_TFMS          = 12
STRING_TSC           = 13
STRING_MHZ           = 14
STRING_SUPPORTED     = 15
STRING_NOT_SUPPORTED = 16
STRING_OS_ENABLED    = 17
STRING_OS_DISABLED   = 18
BUFFER_COMMON        = 0
BUFFER_FEATURES      = 4096
BIND_APP_GUI         = 0
X_SIZE               = 360
Y_SIZE               = 240
INFO_BUFFER_SIZE     = 8192
MISC_BUFFER_SIZE     = 8192

macro BIND_STOP
{ DB 0 }
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

R_EAX = 0
R_EBX = 1
R_ECX = 2
R_EDX = 3
MACRO ENTRY_STOP
{
DB 00h
}
MACRO ENTRY_LINE
{
DB 01h
}
MACRO ENTRY_CPUID name, mnemonic, function, register, bit 
{
DB 02h + ( register SHL 6 )
DD function
DB bit
DB name, 0 , mnemonic , 0
}
MACRO ENTRY_CPUID_S name, mnemonic, function, subfunction, register, bit 
{
DB 03h + ( register SHL 6 )
DD function, subfunction
DB bit
DB name, 0 , mnemonic , 0
}
MACRO PREFIX_CPUID function, register, bit 
{
DB 04h + ( register SHL 6 )
DD function
DB bit
}
MACRO PREFIX_CPUID_S function, subfunction, register, bit 
{
DB 05h + ( register SHL 6 )
DD function, subfunction
DB bit
}
MACRO ENTRY_XCR0 name, mnemonic, bit
{
DB  01h + ( bit SHL 2 )
DB name, 0 , mnemonic , 0
}

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
mov edx,ID_GUI_STRINGS
call ResourceLockHelper
jz .guiFailed
mov [LockedStrings],rax
mov edx,ID_GUI_BINDERS
call ResourceLockHelper
jz .guiFailed
mov [LockedBinders],rax
mov edx,ID_CPUID_FEATURES
call ResourceLockHelper
jz .guiFailed
mov [LockedEntriesCpuid],rax
mov edx,ID_XCR0_FEATURES
call ResourceLockHelper
jz .guiFailed
mov [LockedEntriesXcr0],rax

; Check platform compatibility and WinAPI initialization
mov ebx,21
pushf
pop rax
bts eax,ebx
push rax
popf
pushf
pop rax
btr eax,ebx
jnc .noCpuid
push rax
popf
pushf
pop rax
btr eax,ebx
jc .noCpuid
xor eax,eax
cpuid
cmp eax,1
jb .noCpuidFunction

; Get processor information
lea rdi,[MISC_BUFFER]
xor eax,eax
cpuid
xchg eax,ebx
stosd
xchg eax,edx
stosd
xchg eax,ecx
stosd
mov al,0
stosb
mov eax,1
cpuid
stosd
mov ecx,48
mov al,' '
rep stosb
mov dword [rdi-48],'n/a '
mov dword [rdi],0 
mov esi,80000000h
mov eax,esi
cpuid
lea ebx,[esi + 04]
cmp eax,ebx
jb .exitName
sub rdi,48
push rdi
.storeName:
lea eax,[esi + 02]
cpuid
stosd
xchg eax,ebx
stosd
xchg eax,ecx
stosd
xchg eax,edx
stosd
inc esi
cmp si,4 - 2
jbe .storeName
pop rdi
mov rsi,rdi
mov ecx,48
mov ebx,ecx
.scanName:
lodsb                      
dec ebx
cmp al,0
je .endName
cmp al,' '
loope .scanName
mov cl,48
je .endName
inc ebx
dec esi
.copyName:
lodsb
cmp al,0
je .endName
stosb
dec ecx
dec ebx
jnz .copyName
.endName:
mov al,' '
rep stosb
.exitName:
inc rdi
mov eax,1
cpuid
test dl,00010000b
jz .tscAbsent
lea rbx,[OsTimer]
mov rcx,rbx
call [GetSystemTimeAsFileTime]
mov rsi,[rbx]
@@:
mov rcx,rbx
call [GetSystemTimeAsFileTime]
cmp rsi,[rbx]
je @b
mov rsi,[rbx]
add rsi,10000000
rdtsc
shl rdx,32
lea rbp,[rax + rdx]
@@:
mov rcx,rbx
call [GetSystemTimeAsFileTime]
cmp rsi,[rbx]
ja @b
rdtsc
shl rdx,32
or rax,rdx
sub rax,rbp
jbe .tscError
push 1000000 rax
finit
fild qword [rsp + 00]
fidiv dword [rsp + 08]
fstp qword [rsp + 08]
pop rax rax
jmp .tscDone
.tscAbsent:
xor eax,eax
jmp .tscDone
.tscError:
mov eax,1
.tscDone:
stosq

; Get processor features list by CPUID
lea rdi,[CpuBitmap]
mov qword [rdi + 00],0
xor ebx,ebx
xor ebp,ebp
mov rsi,[LockedEntriesCpuid]
.scanCpu:
lodsb
mov ah,al
and ah,00111111b
cmp ah,00000001b
je .scanCpu
cmp ah,00000010b
je .withoutSubf
cmp ah,00000011b
je .withSubf
jmp .stopCpu
.withoutSubf:
xor ecx,ecx           ; ECX = subfunction
mov edx,[rsi + 00]    ; EDX = function
mov ah,[rsi + 04]     ; AH  = bit number, AL = tag with bits [7-2] = register 
add rsi,5
call CpuidHelper
jmp .nextEntry
.withSubf:
mov edx,[rsi + 00]
mov ecx,[rsi + 04]
mov ah,[rsi + 08] 
add rsi,9
call CpuidHelper
.nextEntry:
mov ecx,2
.skipCpu:
lodsb
cmp al,0
je .skippedCpu
jmp .skipCpu
.skippedCpu:
loop .skipCpu 
inc ebp
jmp .scanCpu
.stopCpu:
mov [rdi + 00],rbx

; Check OS context management bitmap validity and get OS features list by XGETBV 
mov qword [rdi + 08],0
mov eax,1
cpuid
bt ecx,27
setc [rdi + 16]
jnc .noContext
xor ecx,ecx
xgetbv
shl rdx,32
or rdx,rax
xor ebx,ebx
xor ebp,ebp
mov rsi,[LockedEntriesXcr0]
.scanOs:
lodsb
mov ah,al
and ah,00000011b
cmp ah,00000001b
jne .stopOs 
shr al,2
movzx eax,al
bt rdx,rax
jnc .zeroOs
bts rbx,rbp
.zeroOs:
mov ecx,2
.skipOs:
lodsb
cmp al,0
je .skippedOs
jmp .skipOs
.skippedOs:
loop .skipOs 
inc ebp
jmp .scanOs
.stopOs:
mov [rdi + 08],rbx
.noContext:

; Build text block for processor information list
lea rsi,[MISC_BUFFER]
lea rbp,[INFO_BUFFER + BUFFER_COMMON]
mov al,STRING_SIGNATURE
call CommonStringHelper
call StringWrite
mov al,STRING_TFMS
call IndexHelper
lodsd
call HexPrint32
mov al,'h'
stosb
mov al,STRING_NAME
call CommonStringHelper
call StringWrite
mov al,STRING_CLOCK
call CommonStringHelper
mov al,STRING_TSC
call IndexHelper
lodsq
test rax,rax
jz .absentTsc
cmp rax,1
jz .failedTsc 
mov bx,0200h
call DoublePrint
mov al,STRING_MHZ
call IndexHelper
jmp .doneTsc
.absentTsc:
mov al,STRING_NONE
jmp .entryTsc
.failedTsc:
mov al,STRING_FAILED
.entryTsc:
call IndexHelper
.doneTsc:
mov al,0
stosb

; Build text block for processor features list
mov rsi,[LockedEntriesCpuid]
lea rbp,[INFO_BUFFER + BUFFER_FEATURES]
mov rbx,[CpuBitmap]
.listCpu:
lodsb
and al,00111111b
cmp al,00000001b
je .entryLine
cmp al,00000010b
je .entryCpuid
cmp al,00000011b
jne .endListCpu
add rsi,4
jmp .entryCpuid
.entryLine:
mov ax,0A0Dh
stosw
jmp .listCpu
.entryCpuid:
add rsi,5
call FeaturesStringHelper
shr rbx,1
setnc al
add al,STRING_SUPPORTED
call IndexHelper
jmp .listCpu
.endListCpu:
mov ax,0A0Dh
stosw
cmp [OsBitmapValid],0
je .endListOs
mov rsi,[LockedEntriesXcr0]
mov rbx,[OsBitmap]
.listOs:
lodsb
and al,00000011b
cmp al,00000001b
jne .endListOs
call FeaturesStringHelper
shr rbx,1
setnc al
add al,STRING_SUPPORTED
call IndexHelper
jmp .listOs
.endListOs:
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
xor ecx,ecx           
call [ExitProcess]
.guiFailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgGuiFailed]
xor ecx,ecx
call [MessageBox]  
mov ecx,1           
call [ExitProcess]
.libraryNotFound:
lea rbx,[MsgLibraryNotFound]
jmp .failed
.noCpuid:
lea rbx,[MsgCpuid]
jmp .failed
.noCpuidFunction:
lea rbx,[MsgCpuidFunction]
.failed:
mov r9d,MB_ICONERROR
xor r8d,r8d
mov rdx,rbx
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
mov edx,IDR_UP_COMMON
call FontHelper
mov edx,IDR_TEXT_COMMON
call FontHelper
mov edx,IDR_UP_FEATURES
call FontHelper
mov edx,IDR_TEXT_FEATURES
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

CpuidHelper:
push rsi rdi rbp rbx
mov esi,edx         ; ESI = function
mov edi,ecx         ; EDI = subfunction 
mov ebp,eax         ; EBP = bit number : register id
mov eax,esi
and eax,80000000h
cpuid
cmp eax,esi
jb .bitNo
xchg eax,esi
mov ecx,edi
cpuid
mov edi,ebp
shr edi,8
and ebp,00FFh
shr ebp,6
jz .regEax
dec ebp
jz .regEbx
dec ebp
jz .regEcx
.regEdx:
bt edx,edi
jmp .bitDone
.regEcx:
bt ecx,edi
jmp .bitDone
.regEbx:
bt ebx,edi
jmp .bitDone
.regEax:
bt eax,edi
jmp .bitDone
.bitNo:
clc
.bitDone:
pop rbx rbp rdi rsi
jnc @f
bts rbx,rbp
@@:
ret


ResourceLockHelper:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov r8d,RT_RCDATA
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .exit 
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .exit
xchg rcx,rax
call [LockResource]  
test rax,rax
.exit:
mov rsp,rbp
pop rbp
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

CommonStringHelper:
push rsi
mov ah,0
call IndexString
mov rdi,rbp
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov rbp,rdi
lea rdi,[rbp - 82 + 01]
call StringWrite
lea rdi,[rbp - 82 + 18]
pop rsi
ret

FeaturesStringHelper:
mov rdi,rbp
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov rbp,rdi
lea rdi,[rbp - 82 + 01]
call StringWrite
lea rdi,[rbp - 82 + 43]
call StringWrite
lea rdi,[rbp - 82 + 65]
ret

IndexHelper:
push rsi
mov ah,0
call IndexString
call StringWrite
pop rsi
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
MsgGuiFailed               DB  'GUI initialization failed.'                , 0
MsgLibraryNotFound         DB  'OS API initialization failed.'             , 0
MsgCpuid                   DB  'CPUID not supported or locked.'            , 0
MsgCpuidFunction           DB  'CPUID function 1 not supported or locked.' , 0   
NameDll                    DB  'KERNEL32.DLL'                              , 0
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
LockedEntriesCpuid         DQ  ?
LockedEntriesXcr0          DQ  ?
HandleFont                 DQ  ?
HandleDll                  DQ  ?
CpuBitmap                  DQ  ?
OsBitmap                   DQ  ?
OsBitmapValid              DB  ?
align 64
OsTimer                    DQ  ?
align 4096 
INFO_BUFFER                DB  INFO_BUFFER_SIZE DUP (?)
MISC_BUFFER                DB  MISC_BUFFER_SIZE DUP (?)

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
; TODO.
; dialogitem 'COMBOBOX' , '', IDC_CPU_SELECT    , 110, 224, 129, 100, WS_VISIBLE + CBS_DROPDOWNLIST + CBS_HASSTRINGS + WS_VSCROLL
dialogitem 'BUTTON'   , '', IDR_REPORT        , 241, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON'   , '', IDR_BINARY        , 280, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON'   , '', IDR_CANCEL        , 319, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'EDIT'     , '', IDR_UP_COMMON     ,   3,   5, 354,  10, WS_VISIBLE + WS_BORDER + ES_READONLY 
dialogitem 'EDIT'     , '', IDR_TEXT_COMMON   ,   3,  18, 354,  90, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem 'EDIT'     , '', IDR_UP_FEATURES   ,   3, 114, 354,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem 'EDIT'     , '', IDR_TEXT_FEATURES ,   3, 127, 354,  90, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
enddialog
resource raws, ID_GUI_STRINGS    , LANG_ENGLISH + SUBLANG_DEFAULT, guistrings,  \
               ID_GUI_BINDERS    , LANG_ENGLISH + SUBLANG_DEFAULT, guibinders,  \
               ID_CPUID_FEATURES , LANG_ENGLISH + SUBLANG_DEFAULT, cpufeatures, \ 
               ID_XCR0_FEATURES  , LANG_ENGLISH + SUBLANG_DEFAULT, xcr0features
resdata guistrings
DB  'CPU information (x64 v0.0)' , 0
DB  'System monospace bold'      , 0
DB  'Report', 0
DB  'Binary', 0
DB  'Cancel', 0
DB  ' Parameter      | Value' , 0
DB  ' CPUID or XCRO feature                   | Mnemonic            | Support' , 0
DB  'n/a'           , 0
DB  'failed'        , 0
DB  'Signature'     , 0
DB  'Name'          , 0
DB  'Clock'         , 0
DB  ', TFMS = '     , 0
DB  'TSC = '        , 0
DB  ' MHz'          , 0
DB  'supported'     , 0
DB  'not supported' , 0
DB  ', OS enabled'  , 0
DB  ', OS disabled' , 0 
endres
resdata guibinders
BIND_STRING    STRING_REPORT      , IDR_REPORT
BIND_STRING    STRING_BINARY      , IDR_BINARY
BIND_STRING    STRING_CANCEL      , IDR_CANCEL
BIND_STRING    STRING_UP_COMMON   , IDR_UP_COMMON
BIND_STRING    STRING_UP_FEATURES , IDR_UP_FEATURES
BIND_INFO      BUFFER_COMMON      , IDR_TEXT_COMMON
BIND_INFO      BUFFER_FEATURES    , IDR_TEXT_FEATURES
BIND_STOP
endres
resdata cpufeatures
ENTRY_CPUID    'Multimedia extension'                    , 'MMX'                 , 00000001h , R_EDX , 23
ENTRY_CPUID    'Streaming SIMD extension'                , 'SSE'                 , 00000001h , R_EDX , 25  
ENTRY_CPUID    'Streaming SIMD extension 2'              , 'SSE2'                , 00000001h , R_EDX , 26
ENTRY_CPUID    'Streaming SIMD extension 3'              , 'SSE3'                , 00000001h , R_ECX , 01
ENTRY_CPUID    'Supplemental SSE3'                       , 'SSSE3'               , 00000001h , R_ECX , 09
ENTRY_CPUID    'Streaming SIMD extension 4.1'            , 'SSE4.1'              , 00000001h , R_ECX , 19
ENTRY_CPUID    'Streaming SIMD extension 4.2'            , 'SSE4.2'              , 00000001h , R_ECX , 20
ENTRY_CPUID    'Advanced vector extension'               , 'AVX'                 , 00000001h , R_ECX , 28
ENTRY_CPUID    'Advanced vector extension 2'             , 'AVX2'                , 00000007h , R_EBX , 05
ENTRY_CPUID    'Random number generator'                 , 'RDRAND'              , 00000001h , R_ECX , 30
ENTRY_CPUID    'Intel virtual machine extension'         , 'VMX'                 , 00000001h , R_ECX , 05
ENTRY_CPUID    'AMD secure virtual machine'              , 'SVM'                 , 80000001h , R_ECX , 02
ENTRY_CPUID    '64-bit architecture'                     , 'x86-64'              , 80000001h , R_EDX , 29
ENTRY_LINE
ENTRY_CPUID_S  'AVX512 foundation'                       , 'AVX512F'             , 00000007h , 00000000h , R_EBX , 16
ENTRY_CPUID_S  'AVX512 conflict detection'               , 'AVX512CD'            , 00000007h , 00000000h , R_EBX , 28
ENTRY_CPUID_S  'AVX512 prefetch'                         , 'AVX512PF'            , 00000007h , 00000000h , R_EBX , 26
ENTRY_CPUID_S  'AVX512 exponential and reciprocal'       , 'AVX512ER'            , 00000007h , 00000000h , R_EBX , 27
ENTRY_CPUID_S  'AVX512 vector length control'            , 'AVX512VL'            , 00000007h , 00000000h , R_EBX , 31
ENTRY_CPUID_S  'AVX512 bytes and words'                  , 'AVX512BW'            , 00000007h , 00000000h , R_EBX , 30
ENTRY_CPUID_S  'AVX512 doublewords and quadwords'        , 'AVX512DQ'            , 00000007h , 00000000h , R_EBX , 17
ENTRY_CPUID_S  'AVX512 integer fused multiply and add'   , 'AVX512_IFMA'         , 00000007h , 00000000h , R_EBX , 21
ENTRY_CPUID_S  'AVX512 vector byte manipulation'         , 'AVX512_VBMI'         , 00000007h , 00000000h , R_ECX , 01
ENTRY_CPUID_S  'AVX512 vector byte manipulation 2'       , 'AVX512_VBMI2'        , 00000007h , 00000000h , R_ECX , 06
ENTRY_CPUID_S  'AVX512 VNNI BFLOAT16 format'             , 'AVX512_BF16'         , 00000007h , 00000000h , R_EAX , 05
ENTRY_CPUID_S  'AVX512 vector neural network'            , 'AVX512_VNNI'         , 00000007h , 00000000h , R_ECX , 11
ENTRY_CPUID_S  'AVX512 bit algorithms'                   , 'AVX512_BITALG'       , 00000007h , 00000000h , R_ECX , 12
ENTRY_CPUID_S  'AVX512 4-iteration FMA single precision' , 'AVX512_4FMAPS'       , 00000007h , 00000000h , R_EDX , 03
ENTRY_CPUID_S  'AVX512 4-iteration VNNI word mode'       , 'AVX512_4VNNIW'       , 00000007h , 00000000h , R_EDX , 02
ENTRY_CPUID_S  'AVX512 count number of set bits'         , 'AVX512_VPOPCNTDQ'    , 00000007h , 00000000h , R_ECX , 14
ENTRY_CPUID_S  'AVX512 compute intersection'             , 'AVX512_VP2INTERSECT' , 00000007h , 00000000h , R_EDX , 08
ENTRY_CPUID_S  'AVX512 floating point 16-bit format'     , 'AVX512_FP16'         , 00000007h , 00000000h , R_EDX , 23
; TODO.
; ENTRY_CPUID_S  'AVX512 vector advanced encryption'       , 'AVX512+VAES'         , 00000007h , 00000000h , R_ECX , 09
; ENTRY_CPUID_S  'AVX512 Galois field numeric'             , 'AVX512+GFNI'         , 00000007h , 00000000h , R_ECX , 08
; ENTRY_CPUID_S  'AVX512 carry less multiplication'        , 'AVX512+VPCLMULQDQ'   , 00000007h , 00000000h , R_ECX , 10
ENTRY_STOP
endres
resdata xcr0features
ENTRY_XCR0     'SSE128 context'                          , 'XMM[0-15]'  , 01  
ENTRY_XCR0     'AVX256 context'                          , 'YMM[0-15]'  , 02 
ENTRY_XCR0     'AVX512 context, low 16 registers'        , 'ZMM[0-15]'  , 06
ENTRY_XCR0     'AVX512 context, high 16 registers'       , 'ZMM[16-31]' , 07
ENTRY_XCR0     'AVX512 context, predicate registers'     , 'K[0-7]'     , 05
ENTRY_XCR0     'MPX context, bounds registers'           , 'BNDREGS'    , 03
ENTRY_XCR0     'MPX context, configuration and status'   , 'BNDCSR'     , 04
ENTRY_STOP
endres
resource icons, ID_EXE_ICON, LANG_NEUTRAL, exeicon
resource gicons, ID_EXE_ICONS, LANG_NEUTRAL, exegicon
icon exegicon, exeicon, 'images\fasmicon64.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="CPU information viewer"'
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

