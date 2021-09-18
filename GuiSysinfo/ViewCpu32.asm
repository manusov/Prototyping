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
; 11) Verify x87 FPU supported because used.
; 12) See "TODO" notes at report save procedure. 


include 'win32a.inc'
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
; IDR_BINARY         = 205
IDR_CANCEL           = 206
IDC_CPU_SELECT       = 207
STRING_APP           = 0
STRING_FONT          = 1
STRING_REPORT        = 2
; STRING_BINARY      = 3
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

FILE_PATH_BUFFER     = MISC_BUFFER
FILE_WORK_BUFFER     = MISC_BUFFER + 256
FILE_PATH_MAXIMUM    = 255
FILE_WORK_MAXIMUM    = 4096

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

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

cld
push AppControl
call [InitCommonControlsEx]
test eax,eax
jz .guiFailed
push 0
call [GetModuleHandle]
test eax,eax
jz .guiFailed
mov [HandleThis],eax
push ID_EXE_ICONS
push eax 
call [LoadIcon]
test eax,eax
jz .guiFailed
mov [HandleIcon],eax

mov edx,ID_GUI_STRINGS
call ResourceLockHelper
jz .guiFailed
mov [LockedStrings],eax
mov edx,ID_GUI_BINDERS
call ResourceLockHelper
jz .guiFailed
mov [LockedBinders],eax
mov edx,ID_CPUID_FEATURES
call ResourceLockHelper
jz .guiFailed
mov [LockedEntriesCpuid],eax
mov edx,ID_XCR0_FEATURES
call ResourceLockHelper
jz .guiFailed
mov [LockedEntriesXcr0],eax

; Check platform compatibility and WinAPI initialization
mov ebx,21
pushf
pop eax
bts eax,ebx
push eax
popf
pushf
pop eax
btr eax,ebx
jnc .noCpuid
push eax
popf
pushf
pop eax
btr eax,ebx
jc .noCpuid
xor eax,eax
cpuid
cmp eax,1
jb .noCpuidFunction

; Get processor signatures and strings information
lea edi,[MISC_BUFFER]
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
mov dword [edi-48],'n/a '
mov dword [edi],0 
mov esi,80000000h
mov eax,esi
cpuid
lea ebx,[esi + 04]
cmp eax,ebx
jb .exitName
sub edi,48
push edi
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
pop edi
mov esi,edi
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
inc edi

; Measure TSC frequency
mov eax,1
cpuid
test dl,00010000b
jz .tscAbsent
lea ebx,[OsTimer]
push ebx
call [GetSystemTimeAsFileTime]   ; this for phase correction
mov esi,[ebx + 00]
@@:
push ebx
call [GetSystemTimeAsFileTime]   ; this for measured interval start
cmp esi,[ebx + 00]
je @b
mov esi,[ebx + 00]
mov ebp,[ebx + 04]
add esi,10000000
adc ebp,0
rdtsc
mov [ebx + 08],eax
mov [ebx + 12],edx
@@:
push ebx
call [GetSystemTimeAsFileTime]
cmp ebp,[ebx + 04]
ja @b
jb @f
cmp esi,[ebx + 00]
ja @b
@@:
rdtsc
sub eax,[ebx + 08]
sbb edx,[ebx + 12] 
mov [ebx + 08],eax
mov [ebx + 12],edx 
mov dword [ebx + 00],1000000
finit
fild qword [ebx + 08]
fidiv dword [ebx + 00]
fstp qword [ebx + 00]
mov eax,[ebx + 00]
mov edx,[ebx + 04]
jmp .tscDone
.tscAbsent:
xor eax,eax
cdq
jmp .tscDone
.tscError:
mov eax,1
cdq
.tscDone:
stosd
xchg eax,edx
stosd

; Get processor features list by CPUID
xor ebx,ebx
xor ebp,ebp
mov esi,[LockedEntriesCpuid]
lea edi,[CpuBitmap]
mov [edi + 00],ebx
mov [edi + 04],ebx
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
mov edx,[esi + 00]    ; EDX = function
mov ah,[esi + 04]     ; AH  = bit number, AL = tag with bits [7-2] = register 
add esi,5
call CpuidHelper
jmp .nextEntry
.withSubf:
mov edx,[esi + 00]
mov ecx,[esi + 04]
mov ah,[esi + 08] 
add esi,9
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
mov [edi + 00],ebx

; Check OS context management bitmap validity and get OS features list by XGETBV 
xor eax,eax
mov [edi + 08],eax
mov [edi + 12],eax
mov eax,1
cpuid
bt ecx,27
setc [edi + 16]
jnc .noContext
xor ecx,ecx
xgetbv
; shl rdx,32   ; TODO. EDX yet ignored for ia32 version
; or rdx,rax
xchg edx,eax   ; TODO. Yet XCR0.[31-00] only, XCR0.[63-32] yet ignored   
;---
xor ebx,ebx
xor ebp,ebp
mov esi,[LockedEntriesXcr0]
.scanOs:
lodsb
mov ah,al
and ah,00000011b
cmp ah,00000001b
jne .stopOs 
shr al,2
movzx eax,al
bt edx,eax
jnc .zeroOs
bts ebx,ebp
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
mov dword [edi + 08],ebx
mov dword [edi + 12],0
.noContext:

; Build text block for processor information list
lea esi,[MISC_BUFFER]
lea ebp,[INFO_BUFFER + BUFFER_COMMON]
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
;---
;lodsq
;test rax,rax
;jz .absentTsc
;cmp rax,1
;jz .failedTsc 
;---
lodsd
xchg edx,eax
lodsd
xchg edx,eax
test edx,edx
jnz .validTsc
test eax,eax
jz .absentTsc
cmp eax,1
je .failedTsc
;---
.validTsc:
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
mov ax,0A0Dh
stosw
mov al,0
stosb

; Build text block for processor features list
mov esi,[LockedEntriesCpuid]
lea ebp,[INFO_BUFFER + BUFFER_FEATURES]
mov ebx,dword [CpuBitmap]   ; TODO. Yet ignored high 32 bits of 64-bit bitmap, ia32 version limitations
.listCpu:
lodsb
and al,00111111b
cmp al,00000001b
je .entryLine
cmp al,00000010b
je .entryCpuid
cmp al,00000011b
jne .endListCpu
add esi,4
jmp .entryCpuid
.entryLine:
mov ax,0A0Dh
stosw
jmp .listCpu
.entryCpuid:
add esi,5
call FeaturesStringHelper
shr ebx,1
setnc al
add al,STRING_SUPPORTED
call IndexHelper
jmp .listCpu
.endListCpu:
mov ax,0A0Dh
stosw
cmp [OsBitmapValid],0
je .endListOs
mov esi,[LockedEntriesXcr0]
mov ebx,dword [OsBitmap]   ; TODO. Yet ignored high 32 bits of 64-bit bitmap, ia32 version limitations
.listOs:
lodsb
and al,00000011b
cmp al,00000001b
jne .endListOs
call FeaturesStringHelper
shr ebx,1
setnc al
add al,STRING_SUPPORTED
call IndexHelper
jmp .listOs
.endListOs:
mov ax,0A0Dh
stosw
mov al,0
stosb

push 0 0 
push DialogProc
push HWND_DESKTOP
push ID_MAIN
push [HandleThis]  
call [DialogBoxParam] 
test eax,eax
jz .guiFailed 
cmp eax,-1
je .guiFailed
.ok:
push 0           
call [ExitProcess]
.guiFailed:
push MB_ICONERROR
push 0
push MsgGuiFailed 
push 0
call [MessageBox]  
push 1           
call [ExitProcess]
.libraryNotFound:
lea ebx,[MsgLibraryNotFound]
jmp .failed
.noCpuid:
lea ebx,[MsgCpuid]
jmp .failed
.noCpuidFunction:
lea ebx,[MsgCpuidFunction]
.failed:
push MB_ICONERROR
push 0
push ebx
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
mov ax,STRING_FONT
call IndexString
xor eax,eax
push esi
push FIXED_PITCH
push CLEARTYPE_QUALITY
push CLIP_DEFAULT_PRECIS
push OUT_TT_ONLY_PRECIS
push DEFAULT_CHARSET
push eax
push eax
push eax
push FW_DONTCARE
push eax
push eax
push eax
push 17
call [CreateFont]
mov [HandleFont],eax
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
mov eax,[ebp + 28]
cmp eax,IDR_REPORT
je .wmreport
cmp eax,IDR_CANCEL
je .wmclose
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

.wmreport:
; Copy file name to buffer
lea ebx,[FileOpen]
lea esi,[ReportName]
lea edi,[FILE_PATH_BUFFER]
call StringWrite
xor eax,eax
stosb
; Show dialogue box
; TODO. Optimize, clear by rep stosb and set used fields only,
; don't use one MOV per each zeroed field
; TODO. Check exception under Windows 7 under debuggers, example OllyDbg.
; TODO. Not added ".txt" extension if file name edit.
mov [ebx + OPENFILENAME.lStructSize],sizeof.OPENFILENAME
mov [ebx + OPENFILENAME.hwndOwner],eax
mov [ebx + OPENFILENAME.hInstance],eax
mov [ebx + OPENFILENAME.lpstrFilter],ReportFilter
mov [ebx + OPENFILENAME.lpstrCustomFilter],eax
mov [ebx + OPENFILENAME.nMaxCustFilter],eax
mov [ebx + OPENFILENAME.nFilterIndex],eax
mov [ebx + OPENFILENAME.lpstrFile],FILE_PATH_BUFFER
mov [ebx + OPENFILENAME.nMaxFile],FILE_PATH_MAXIMUM
mov [ebx + OPENFILENAME.lpstrFileTitle],eax
mov [ebx + OPENFILENAME.nMaxFileTitle],eax
mov [ebx + OPENFILENAME.lpstrInitialDir],eax
mov [ebx + OPENFILENAME.lpstrTitle],eax
mov [ebx + OPENFILENAME.Flags],OFN_FILEMUSTEXIST
mov [ebx + OPENFILENAME.nFileOffset],ax
mov [ebx + OPENFILENAME.nFileExtension],ax
mov [ebx + OPENFILENAME.lpstrDefExt],eax
mov [ebx + OPENFILENAME.lCustData],eax
mov [ebx + OPENFILENAME.lpfnHook],eax
mov [ebx + OPENFILENAME.lpTemplateName],eax
push ebx
call [GetSaveFileName]
test eax,eax
jz .noSelections
; Create report file 
mov ecx,[ebx + OPENFILENAME.lpstrFile]
xor ebx,ebx
test ecx,ecx
jz .reportError 
xor eax,eax                   ; EAX = 0 for compact push
push eax                      ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL    ; Parm #6 = File attributes
push CREATE_ALWAYS            ; Parm #5 = Creation disposition
push eax                      ; Parm #4 = Security attributes, not used
push eax                      ; Parm #3 = Share mode, not used
push GENERIC_WRITE            ; Parm #2 = Desired access
push ecx                      ; Parm #1 = Pointer to file name
call [CreateFileA]
test eax,eax
jz .reportError
cmp eax,INVALID_HANDLE_VALUE
je .reportError 
xchg ebx,eax
; Save application name
xor eax,eax
call IndexString
lea edi,[FILE_WORK_BUFFER]
push edi
call StringWrite
mov eax,000A0D00h + '.'
stosd
pop esi
call FileStringWrite
; Save table up for first text block
call LineHelper
call FileStringWrite
mov ax,STRING_UP_COMMON
call IndexString
call FileStringWrite
call LineHelper
call FileStringWrite
; Save first text block
lea esi,[INFO_BUFFER + BUFFER_COMMON]
call FileStringWrite
; Save table up for second text block
call LineHelper
call FileStringWrite
mov ax,STRING_UP_FEATURES
call IndexString
call FileStringWrite
call LineHelper
call FileStringWrite
; Save second text block
lea esi,[INFO_BUFFER + BUFFER_FEATURES]
call FileStringWrite
; Show status box
lea esi,[MsgReportOK]
lea edi,[FILE_WORK_BUFFER]
push edi
call StringWrite
lea esi,[FILE_PATH_BUFFER]
call StringWrite
mov al,0
stosb
pop edx
push edx     ; TODO. Remove extra pop-push
xor eax,eax
call IndexString
pop edx
mov ecx,esi
mov eax,MB_OK
jmp .reportStatus
.reportError:
mov eax,MB_ICONERROR
lea edx,[MsgReportFailed]
xor ecx,ecx
.reportStatus:
push eax
push ecx
push edx
push 0
call [MessageBox]  
; Close file
test ebx,ebx
jz .noSelections
push ebx
xor ebx,ebx
call [CloseHandle]
test eax,eax
jz .reportError 
; Done
; TODO. Decode error code and write details about file I/O error.
; TODO. Add "." after message string.
.noSelections:
jmp .processed 

FileStringWrite:
cld
push esi edi ebp 0
mov ebp,esp
mov edi,esi
mov ecx,FILE_WORK_MAXIMUM
mov al,0
repne scasb
sub edi,esi
dec edi
.write:
push 0                 ; Parm#5 = Overlapped, not used
push ebp               ; Parm#4 = Pointer to output variable, count
push edi               ; Parm#3 = Number of chars ( length )
push esi               ; Parm#2 = Pointer to string ( buffer )
push ebx               ; Parm#1 = File handle
call [WriteFile]
mov ecx,[ebp]          ; ECX = Returned size
test eax,eax           ; EAX = status, 0 means error
jz .stop               ; Go exit if error
jecxz .stop            ; Go exit if returned size = 0
add esi,ecx            ; ESI = advance read pointer by returned size
sub edi,ecx            ; EDI = subtract current read size from size limit
ja .write              ; Repeat write if return size > 0 and limit not reached 
.stop:
pop ebp ebp edi esi
ret

LineHelper:
cld
lea edi,[FILE_WORK_BUFFER]
mov esi,edi
mov ax,0A0Dh
stosw
mov ecx,79
push eax
mov al,'-'
rep stosb
pop eax
stosw
mov al,0
stosb
ret

CpuidHelper:
push esi edi ebp ebx
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
pop ebx ebp edi esi
jnc @f
bts ebx,ebp
@@:
ret

ResourceLockHelper:
push RT_RCDATA
push edx
push [HandleThis]
call [FindResource]                
test eax,eax
jz .exit 
push eax
push [HandleThis]
call [LoadResource] 
test eax,eax
jz .exit
push eax
call [LockResource]  
test eax,eax
.exit:
ret

FontHelper:
push edx
push dword [ebp + 20]
call [GetDlgItem]
test eax,eax
jz @f
push 1
push [HandleFont]
push WM_SETFONT
push eax
call [SendMessage]
@@:
ret

CommonStringHelper:
push esi
mov ah,0
call IndexString
mov edi,ebp
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov ebp,edi
lea edi,[ebp - 82 + 01]
call StringWrite
lea edi,[ebp - 82 + 18]
pop esi
ret

FeaturesStringHelper:
mov edi,ebp
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov ebp,edi
lea edi,[ebp - 82 + 01]
call StringWrite
lea edi,[ebp - 82 + 43]
call StringWrite
lea edi,[ebp - 82 + 65]
ret

IndexHelper:
push esi
mov ah,0
call IndexString
call StringWrite
pop esi
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
cmp byte [esi],0
je .stopBinder
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
jmp .foundBinder
.stopBinder:
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
BindBig:
lea esi,[INFO_BUFFER + eax]
mov esi,[esi]
test esi,esi
jz BindExit
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
fbstp [esp+00]
fbstp [esp+16]
test byte [esp+16+09],80h
setnz dl
test byte [esp+00+09],80h
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
MsgGuiFailed               DB  'GUI initialization failed.'                , 0
MsgLibraryNotFound         DB  'OS API initialization failed.'             , 0
MsgCpuid                   DB  'CPUID not supported or locked.'            , 0
MsgCpuidFunction           DB  'CPUID function 1 not supported or locked.' , 0   
NameDll                    DB  'KERNEL32.DLL'                              , 0
ReportName                 DB  'report.txt',0
ReportFilter               DB  'Text files ', 0, '*.txt',0,0
MsgReportFailed            DB  'Save report failed.'                       , 0
MsgReportOK                DB  'Report saved: '                            , 0 
AppControl                 INITCOMMONCONTROLSEX  8, 0
ProcBinders                DD  BindString
                           DD  BindInfo
                           DD  BindBig
                           DD  BindBool
                           DD  BindCombo
ProcCombo                  DD  BindComboStopOn
                           DD  BindComboStopOff 
                           DD  BindComboCurrent
                           DD  BindComboAdd
                           DD  BindComboInactive
AllocationBase             DD  0
HandleThis                 DD  ? 
HandleIcon                 DD  ?
LockedStrings              DD  ?
LockedBinders              DD  ?
LockedEntriesCpuid         DD  ?
LockedEntriesXcr0          DD  ?
HandleFont                 DD  ?
HandleDll                  DD  ?
CpuBitmap                  DQ  ?  ; only low 32 bits used at ia32 version
OsBitmap                   DQ  ?  ; only low 32 bits used at ia32 version
OsBitmapValid              DB  ?
FileOpen                   OPENFILENAME  ?
align 64
OsTimer                    DQ  ?
StartTsc                   DQ  ?
align 4096 
INFO_BUFFER                DB  INFO_BUFFER_SIZE DUP (?)
MISC_BUFFER                DB  MISC_BUFFER_SIZE DUP (?)  

section '.idata' import data readable writeable
library kernel32 , 'kernel32.dll' , \
        user32   , 'user32.dll'   , \
        comctl32 , 'comctl32.dll' , \
        comdlg32 , 'comdlg32.dll' , \
        gdi32    , 'gdi32.dll'
include 'api\kernel32.inc'
include 'api\user32.inc'
include 'api\comctl32.inc'
include 'api\comdlg32.inc'
include 'api\gdi32.inc'

section '.rsrc' resource data readable
directory RT_DIALOG     , dialogs   , \
          RT_RCDATA     , raws      , \ 
          RT_ICON       , icons     , \
          RT_GROUP_ICON , gicons    , \
          RT_MANIFEST   , manifests
resource dialogs, ID_MAIN, LANG_ENGLISH + SUBLANG_DEFAULT, mydialog
; dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU + WS_SIZEBOX, 0, 0, 'System monospace', 18
dialog mydialog, '', 0, 0, X_SIZE, Y_SIZE, DS_CENTER + WS_CAPTION + WS_SYSMENU, 0, 0, 'System monospace', 18
; TODO.
; dialogitem 'COMBOBOX' , '', IDC_CPU_SELECT    , 110, 224, 129, 100, WS_VISIBLE + CBS_DROPDOWNLIST + CBS_HASSTRINGS + WS_VSCROLL
; dialogitem 'BUTTON'   , '', IDR_REPORT        , 241, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
; dialogitem 'BUTTON'   , '', IDR_BINARY        , 280, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
;
dialogitem 'BUTTON'   , '', IDR_REPORT        , 280, 224,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
;
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
DB  'CPU information (ia32 v0.03)' , 0
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
; BIND_STRING  STRING_BINARY      , IDR_BINARY
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
icon exegicon, exeicon, 'images\fasmicon32.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="CPU information viewer"'
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

