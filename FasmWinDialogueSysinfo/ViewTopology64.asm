; TODO. Asymmetric cache yet not supported at summary info, include Intel Hybrid CPUs.
; TODO. Non typical cache types not detected by summary, example L1 unified.
; TODO. Print fully associative instead ways=255.
; TODO. Optimize helpers parameters, for ParameterHelper 8/16/32.
; TODO. Make U_B string as resource.

include 'win64a.inc'
CLEARTYPE_QUALITY   = 5
ID_MAIN             = 100
ID_EXE_ICON         = 101
ID_EXE_ICONS        = 102
ID_GUI_STRINGS      = 103
ID_GUI_BINDERS      = 104
IDR_UP_TOPOLOGY     = 200
IDR_TEXT_TOPOLOGY   = 201
IDR_UP_CACHE        = 202      
IDR_TEXT_CACHE      = 203
IDR_REPORT          = 204
IDR_BINARY          = 205
IDR_CANCEL          = 206
STRING_APP          = 0
STRING_FONT         = 1
STRING_REPORT       = 2
STRING_BINARY       = 3
STRING_CANCEL       = 4
STRING_UP_TOPOLOGY  = 5
STRING_UP_CACHE     = 6
STRING_CPU_CORE     = 7
STRING_NUMA_NODE    = 8
STRING_L            = 9
STRING_CPU_PACKAGE  = 10
STRING_UNKNOWN_ID   = 11
STRING_UNIFIED      = 12
STRING_INSTRUCTION  = 13
STRING_DATA         = 14
STRING_TRACE        = 15
STRING_UNKNOWN_TYPE = 16
STRING_POINTS       = 17
STRING_HT           = 18
STRING_NODE         = 19
STRING_CACHE_WAYS   = 20
STRING_CACHE_LINE   = 21
STRING_CACHE_SIZE   = 22
STRING_X            = 23
BUFFER_TOPOLOGY     = 0
BUFFER_CACHE        = 8
BIND_APP_GUI        = 0
X_SIZE              = 360
Y_SIZE              = 240
INFO_BUFFER_SIZE    = 8192
MISC_BUFFER_SIZE    = 8192
LIMIT_TOTAL         = LIMIT_ENUM + LIMIT_TOPOLOGY + LIMIT_CACHE
LIMIT_ENUM          = 1024 * 128
LIMIT_TOPOLOGY      = 1024 * 128
LIMIT_CACHE         = 1024 * 128
BASE_ENUM           = 0
BASE_TOPOLOGY       = LIMIT_ENUM
BASE_CACHE          = LIMIT_ENUM + LIMIT_TOPOLOGY
ERROR_BUFFER_LIMIT  = 07Ah
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
mov r8d,RT_RCDATA
mov edx,ID_GUI_STRINGS
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .guiFailed 
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .guiFailed
xchg rcx,rax
call [LockResource]  
test rax,rax
jz .guiFailed
mov [LockedStrings],rax
mov r8d,RT_RCDATA
mov edx,ID_GUI_BINDERS
mov rcx,[HandleThis]
call [FindResource]                
test rax,rax
jz .guiFailed
xchg rdx,rax
mov rcx,[HandleThis]
call [LoadResource] 
test rax,rax
jz .guiFailed
xchg rcx,rax
call [LockResource]  
test rax,rax
jz .guiFailed
mov [LockedBinders],rax

lea rcx,[NameDll]
call [GetModuleHandle]
test rax,rax
jz .libraryNotFound
mov [HandleDll],rax 
lea rdx,[NameFunctionGet]
mov rcx,[HandleDll]
call [GetProcAddress]
test rax,rax
jz .functionNotFound
mov [_GetLogicalProcessorInformation],rax
mov r9d,PAGE_READWRITE
mov r8d,MEM_COMMIT + MEM_RESERVE
mov edx,LIMIT_TOTAL
xor ecx,ecx
call [VirtualAlloc]
test rax,rax
jz .memoryFailed
mov [AllocationBase],rax
xchg rbp,rax
lea rdx,[BufferLength]
mov dword [rdx],LIMIT_ENUM
lea rcx,[rbp + BASE_ENUM]
mov rsi,rcx
call [_GetLogicalProcessorInformation]
test rax,rax
jnz .getOk
call [GetLastError]
cmp rax,ERROR_BUFFER_LIMIT
je .memoryFailed
jmp .topologyFailed 
.getOk: 

mov ecx,[BufferLength]
test cl,00011111b
jnz .topologyFailed
shr ecx,5
jz .functionNotFound
lea rdi,[rbp + BASE_TOPOLOGY]
.scanRelations:
mov rdx,rdi
push rax rcx
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop rcx rax
push rdi
mov eax,[rsi + 08]
cmp eax,3
ja .relationUnknown
je .relationPackage
cmp al,2
je .relationCache
cmp al,1
je .relationNuma
cmp al,0
je .relationCore
.relationUnknown:
mov al,STRING_UNKNOWN_ID
call RelationNameHelper 
jmp .doneRelation
.relationCore:
mov al,STRING_CPU_CORE
call RelationNameHelper 
call AffinityHelper
mov eax,( STRING_HT SHL 16 ) + 2910h
call ParameterHelper8
jmp .doneRelation
.relationNuma: 
mov al,STRING_NUMA_NODE
call RelationNameHelper 
call AffinityHelper
mov eax,( STRING_NODE SHL 16 ) + 2910h
call ParameterHelper32
jmp .doneRelation
.relationCache:
mov al,STRING_L
call RelationNameHelper
mov al,[rsi + 16]
cmp al,1
jb .levelBad
cmp al,4
ja .levelBad
or al,30h
jmp .levelStore
.levelBad:
mov al,'?'
.levelStore:
mov ah,' '
stosw
mov eax,[rsi + 24]
cmp eax,3
ja .typeBad
add al,STRING_UNIFIED
jmp .typeStore
.typeBad:
mov al,STRING_UNKNOWN_TYPE
.typeStore:
lea rdi,[rdx + 04] 
call RelationNameEntry
call AffinityHelper
mov eax,( STRING_CACHE_WAYS SHL 16 ) + 2911h
call ParameterHelper8
mov eax,( STRING_CACHE_LINE SHL 16 ) + 3212h
call ParameterHelper16
lea rdi,[rdx + 3Fh]
push rcx rsi
mov eax,[rsi + 14h]
push rax
mov ax,STRING_CACHE_SIZE
call IndexString
call StringWrite
mov bl,0
pop rax
call DecimalPrint32
pop rsi rcx
jmp .doneRelation 
.relationPackage:
mov al,STRING_CPU_PACKAGE
call RelationNameHelper 
call AffinityHelper
.doneRelation:
add rsi,32
pop rdi
dec ecx
jnz .scanRelations
mov al,0
stosb

lea rdi,[rbp + BASE_CACHE]
mov ax,0101h
call CacheSummaryHelper 
mov ax,0201h
call CacheSummaryHelper 
mov ax,0002h
call CacheSummaryHelper 
mov ax,0003h
call CacheSummaryHelper 
mov ax,0004h
call CacheSummaryHelper 
mov al,0
stosb

lea rax,[INFO_BUFFER]
lea rcx,[rbp + BASE_TOPOLOGY]
mov [rax + BUFFER_TOPOLOGY],rcx
lea rcx,[rbp + BASE_CACHE]
mov [rax + BUFFER_CACHE],rcx

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
call ReleaseMemoryHelper
test rax,rax
jz .memoryFailed 
xor ecx,ecx           
call [ExitProcess]
.guiFailed:
mov r9d,MB_ICONERROR
xor r8d,r8d
lea rdx,[MsgGuiFailed]
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
mov ecx,1           
call [ExitProcess]
.libraryNotFound:
lea rbx,[MsgLibraryNotFound]
jmp .failed
.functionNotFound:
lea rbx,[MsgFunctionNotFound]
jmp .failed
.memoryFailed:
lea rbx,[MsgMemoryFailed]
jmp .failed
.topologyFailed:
lea rbx,[MsgTopologyFailed]
.failed:
mov r9d,MB_ICONERROR
xor r8d,r8d
mov rdx,rbx
xor ecx,ecx
call [MessageBox]  
call ReleaseMemoryHelper
mov ecx,2           
call [ExitProcess]

RelationNameHelper:
lea rdi,[rdx + 01]
RelationNameEntry:
push rcx rsi
mov ah,0
call IndexString
call StringWrite
pop rsi rcx
ret

AffinityHelper:
lea rdi,[rdx + 18]
mov rax,[rsi + 00]
lea r8,[rdi + 16]
push rcx rdx rsi
bsf rcx,rax
bsr rdx,rax
cmp ecx,edx
je .modeSingle
push rcx rdx
.scanMask:
bt eax,ecx
jz .endMask
inc ecx
cmp ecx,edx
jb .scanMask
.endMask:
cmp ecx,edx
pop rdx rcx
je .modeInterval
.modeIndividual:
xor ecx,ecx
xor edx,edx
.cycleIndividual:
cmp rdi,r8
jae .overIndividual
shr rax,1
jnc .skipIndividual
push rax
test edx,edx
jz .firstIndividual
mov al,','
stosb
.firstIndividual:
inc edx
mov eax,ecx
mov bl,0
call DecimalPrint32
pop rax
.skipIndividual:
inc ecx
cmp cl,64
jb .cycleIndividual
jmp .done
.overIndividual:
mov ax,STRING_POINTS
call IndexString
call StringWrite
jmp .done
.modeInterval:
xchg eax,ecx
mov bl,0
call DecimalPrint32 
mov al,'-'
stosb
.modeSingle:
xchg eax,edx
call DecimalPrint32
.done:
pop rsi rdx rcx
ret

ParameterHelper8:
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,byte [rsi + rax]
ParameterEntry:
push rcx rsi
lea rdi,[rdx + rdi]
push rax
xchg eax,ebx
call IndexString
call StringWrite
pop rax
mov bl,0
call DecimalPrint32
pop rsi rcx
ret

ParameterHelper16:
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,word [rsi + rax]
jmp ParameterEntry

ParameterHelper32:
shld ebx,eax,16
movzx edi,ah
movzx eax,al
mov eax,dword [rsi + rax]
jmp ParameterEntry

CacheSummaryHelper:
lea rsi,[rbp + BASE_ENUM]
mov ecx,[BufferLength]
shr ecx,5
jz .nodata
movzx ebx,ah
xchg edx,eax
xor r8d,r8d
xor r9d,r9d
.scanCaches:
cmp dword [rsi + 08],2
jne .done
cmp byte [rsi + 16],dl
jne .done
cmp dword [rsi + 24],ebx
jne .done
mov r8d,[rsi + 20]
inc r9d
.done:
add rsi,32
loop .scanCaches
test r8d,r8d
jz .nodata
mov bh,dl
mov rdx,rdi
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
push rdi
lea rdi,[rdx + 01]
mov ax,STRING_L
call IndexString
call StringWrite
mov al,bh
or al,30h
stosb
movzx ax,bl
add al,STRING_UNIFIED
lea rdi,[rdx + 04] 
call RelationNameEntry
lea rdi,[rdx + 18]
mov bl,0FFh
mov eax,r8d
call SizePrint64
lea rdi,[rdx + 41]
mov ax,STRING_X
call IndexString
call StringWrite
mov eax,r9d
mov bl,0
call DecimalPrint32
pop rdi
.nodata:
ret

ReleaseMemoryHelper:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov r8d,MEM_RELEASE
xor edx,edx
mov rcx,[AllocationBase]
jrcxz @f
call [VirtualFree]
@@:
mov rsp,rbp
pop rbp
ret

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
mov edx,IDR_UP_TOPOLOGY
call FontHelper
mov edx,IDR_TEXT_TOPOLOGY
call FontHelper
mov edx,IDR_UP_CACHE
call FontHelper
mov edx,IDR_TEXT_CACHE
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

StringWriteSelected:
test al,al
jz StringWrite
cmp al,ah
ja StringWrite  
mov ah,al
cld
@@:
lodsb
cmp al,0
jne @b
dec ah
jnz @b
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

SizePrint64:
push rax rbx rcx rdx rsi
cld
cmp bl,0FFh
je .autoUnits
mov esi,1
movzx ecx,bl
jrcxz .unitsAdjusted
.unitsCycle:
shl rsi,10
loop .unitsCycle
.unitsAdjusted:
mov cl,bl
xor edx,edx
div rsi
mov bl,0
call DecimalPrint32
imul eax,edx,10
div rsi
cmp cl,0
je .afterNumber
push rax
mov al,'.'
stosb
pop rax
jmp .decimalMode
.autoUnits:
xor ecx,ecx
test rax,rax
jz .decimalMode
.unitsAutoCycle:
mov rbx,rax
xor edx,edx
mov esi,1024                           
div rsi
mov esi,0FFFFFFFFh
cmp rbx,rsi
ja .above32bit
test rdx,rdx
jnz .modNonZero
.above32bit:
inc ecx
jmp .unitsAutoCycle
.modNonZero:
cmp ecx,4
ja .hexMode
mov eax,ebx
.decimalMode:
mov bl,0
call DecimalPrint32
.afterNumber:
mov al,' '
stosb
lea rsi,[U_B]
mov al,cl
mov ah,4
call StringWriteSelected
jmp .exit
.hexMode:
call HexPrint64
mov al,'h'
stosb 
.exit:
pop rsi rdx rcx rbx rax
ret

section '.data' data readable writeable
MsgGuiFailed                     DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound               DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound              DB  'OS not supports topology list.'    , 0
MsgMemoryFailed                  DB  'Memory allocation error.'          , 0 
MsgTopologyFailed                DB  'Scan topology failed.'             , 0
NameDll                          DB  'KERNEL32.DLL'                      , 0
NameFunctionGet                  DB  'GetLogicalProcessorInformation'    , 0
U_B                              DB  'Bytes',0
U_KB                             DB  'KB',0
U_MB                             DB  'MB',0
U_GB                             DB  'GB',0
U_TB                             DB  'TB',0
U_MBPS                           DB  'MBPS',0
U_NS                             DB  'nanoseconds',0
AppControl                       INITCOMMONCONTROLSEX  8, 0
ProcBinders                      DQ  BindString
                                 DQ  BindInfo
                                 DQ  BindBig
                                 DQ  BindBool
                                 DQ  BindCombo
ProcCombo                        DQ  BindComboStopOn
                                 DQ  BindComboStopOff 
                                 DQ  BindComboCurrent
                                 DQ  BindComboAdd
                                 DQ  BindComboInactive
AllocationBase                   DQ  0
BufferLength                     DD  0
HandleThis                       DQ  ? 
HandleIcon                       DQ  ?
LockedStrings                    DQ  ?
LockedBinders                    DQ  ?
HandleFont                       DQ  ?
HandleDll                        DQ  ?
_GetLogicalProcessorInformation  DQ  ?  
align 4096 
INFO_BUFFER                      DB  INFO_BUFFER_SIZE DUP (?)
MISC_BUFFER                      DB  MISC_BUFFER_SIZE DUP (?)  

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
dialogitem 'BUTTON', '', IDR_REPORT        , 241, 224,  38, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_BINARY        , 280, 224,  38, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT + WS_DISABLED
dialogitem 'BUTTON', '', IDR_CANCEL        , 319, 224,  38, 13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem 'EDIT'  , '', IDR_UP_TOPOLOGY   ,  3,    5, 354, 10, WS_VISIBLE + WS_BORDER + ES_READONLY 
dialogitem 'EDIT'  , '', IDR_TEXT_TOPOLOGY ,  3,   18, 354, 90, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem 'EDIT'  , '', IDR_UP_CACHE      ,  3,  114, 354, 10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem 'EDIT'  , '', IDR_TEXT_CACHE    ,  3,  127, 354, 90, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
enddialog
resource raws, ID_GUI_STRINGS, LANG_ENGLISH + SUBLANG_DEFAULT, guistrings, \
               ID_GUI_BINDERS, LANG_ENGLISH + SUBLANG_DEFAULT, guibinders
resdata guistrings
DB  'SMP topology list (x64 v0.0)' , 0
DB  'System monospace bold'        , 0
DB  'Report', 0
DB  'Binary', 0
DB  'Cancel', 0
DB  ' Topology unit  | Logical CPU affinity | Comments' , 0
DB  ' Cache          | Size                 | Count' , 0
DB  'CPU core'           , 0
DB  'NUMA node'          , 0
DB  'L'                  , 0
DB  'CPU package'        , 0
DB  'Unknown ID'         , 0
DB  'Unified'            , 0
DB  'Instruction'        , 0
DB  'Data'               , 0
DB  'Trace'              , 0
DB  'Unknown'            , 0
DB  ' ...'               , 0
DB  'ht='                , 0
DB  'node='              , 0
DB  'ways='              , 0
DB  'line='              , 0
DB  'size='              , 0
DB  'x '                 , 0
endres
resdata guibinders
BIND_STRING  STRING_REPORT       , IDR_REPORT
BIND_STRING  STRING_BINARY       , IDR_BINARY
BIND_STRING  STRING_CANCEL       , IDR_CANCEL
BIND_STRING  STRING_UP_TOPOLOGY  , IDR_UP_TOPOLOGY
BIND_STRING  STRING_UP_CACHE     , IDR_UP_CACHE
BIND_BIG     BUFFER_TOPOLOGY     , IDR_TEXT_TOPOLOGY
BIND_BIG     BUFFER_CACHE        , IDR_TEXT_CACHE
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
db '    name="Topology viewer"'
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

