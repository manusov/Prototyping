; TODO. Asymmetric cache yet not supported at summary info, include Intel Hybrid CPUs.
; TODO. Non typical cache types not detected by summary, example L1 unified.
; TODO. Print fully associative instead ways=255.
; TODO. Optimize helpers parameters, for ParameterHelper 8/16/32.
; TODO. Make U_B string as resource.
; See MSDN.
; TODO. Yet used only first member of Affinity array.
; TODO> See MSDN, not all parameters visualized.  

include 'win32a.inc'
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
STRING_GROUP        = 11
STRING_UNKNOWN_ID   = 12
STRING_UNIFIED      = 13
STRING_INSTRUCTION  = 14
STRING_DATA         = 15
STRING_TRACE        = 16
STRING_UNKNOWN_TYPE = 17
STRING_POINTS       = 18
STRING_HT           = 19
STRING_EFFICIENCY   = 20
STRING_NODE         = 21
STRING_CACHE_WAYS   = 22
STRING_CACHE_LINE   = 23
STRING_CACHE_SIZE   = 24
STRING_X            = 25
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

push NameDll
call [GetModuleHandle]
test eax,eax
jz .libraryNotFound
mov [HandleDll],eax 
push NameFunctionGet
push [HandleDll]
call [GetProcAddress]
test eax,eax
jz .functionNotFound
mov [_GetLogicalProcessorInformationEx],eax
push PAGE_READWRITE
push MEM_COMMIT + MEM_RESERVE
push LIMIT_TOTAL
push 0
call [VirtualAlloc]
test eax,eax
jz .memoryFailed
mov [AllocationBase],eax
xchg ebp,eax
lea eax,[BufferLength]
mov dword [eax],LIMIT_ENUM
push eax
lea eax,[ebp + BASE_ENUM]
push eax
xchg esi,eax
push 0FFFFh
call [_GetLogicalProcessorInformationEx]
test eax,eax
jnz .getOk
call [GetLastError]
cmp eax,ERROR_BUFFER_LIMIT
je .memoryFailed
jmp .topologyFailed 
.getOk: 

mov ecx,[BufferLength]
test ecx,ecx
jz .functionNotFound
add ecx,esi
lea edi,[ebp + BASE_TOPOLOGY]

.scanRelations:
mov edx,edi
push eax ecx
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
pop ecx eax
push edi
mov eax,[esi + 00]
cmp eax,4
ja .relationUnknown
je .relationGroup
cmp al,3
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
mov al,32
call AffinityGroupHelper
mov eax,( STRING_HT SHL 16 ) + 2908h
call ParameterHelper8
mov eax,( STRING_EFFICIENCY SHL 16 ) + 3209h
call ParameterHelper8
jmp .doneRelation
.relationNuma: 
mov al,STRING_NUMA_NODE
call RelationNameHelper 
mov al,32
call AffinityGroupHelper
mov eax,( STRING_NODE SHL 16 ) + 2908h
call ParameterHelper32
jmp .doneRelation
.relationCache:
mov al,STRING_L
call RelationNameHelper
mov al,[esi + 08]
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
mov eax,[esi + 16]
cmp eax,3
ja .typeBad
add al,STRING_UNIFIED
jmp .typeStore
.typeBad:
mov al,STRING_UNKNOWN_TYPE
.typeStore:
lea edi,[edx + 04] 
call RelationNameEntry
mov al,40
call AffinityGroupHelper
mov eax,( STRING_CACHE_WAYS SHL 16 ) + 2909h
call ParameterHelper8
mov eax,( STRING_CACHE_LINE SHL 16 ) + 320Ah
call ParameterHelper16
lea edi,[edx + 3Fh]
push ecx esi
mov eax,[esi + 0Ch]
push eax
mov ax,STRING_CACHE_SIZE
call IndexString
call StringWrite
mov bl,0
pop eax
call DecimalPrint32
pop esi ecx
jmp .doneRelation 
.relationPackage:
mov al,STRING_CPU_PACKAGE
call RelationNameHelper 
mov al,32
call AffinityGroupHelper
jmp .doneRelation
.relationGroup: 
mov al,STRING_GROUP
call RelationNameHelper 
mov al,72
call AffinityMaskHelper
.doneRelation:
pop edi
mov eax,[esi + 04]
add esi,eax
cmp esi,ecx
jb .scanRelations
mov al,0
stosb

lea edi,[ebp + BASE_CACHE]
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

lea eax,[INFO_BUFFER]
lea ecx,[ebp + BASE_TOPOLOGY]
mov [eax + BUFFER_TOPOLOGY],ecx
lea ecx,[ebp + BASE_CACHE]
mov [eax + BUFFER_CACHE],ecx

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
push MsgGuiFailed 
push 0
call [MessageBox]  
push 1           
call [ExitProcess]
.libraryNotFound:
lea ebx,[MsgLibraryNotFound]
jmp .failed
.functionNotFound:
lea ebx,[MsgFunctionNotFound]
jmp .failed
.memoryFailed:
lea ebx,[MsgMemoryFailed]
jmp .failed
.topologyFailed:
lea ebx,[MsgTopologyFailed]
.failed:
push MB_ICONERROR
push 0
push ebx
push 0
call [MessageBox]  
call ReleaseMemoryHelper
mov ecx,2           
call [ExitProcess]

RelationNameHelper:
lea edi,[edx + 01]
RelationNameEntry:
push ecx esi
mov ah,0
call IndexString
call StringWrite
pop esi ecx
ret

AffinityGroupHelper:
push esi ebp
movzx eax,al
add esi,eax
lea edi,[edx + 18]
lea ebp,[edi + 16]
mov ax,[esi + 04]
call HexPrint16
mov al,'\'
stosb
push ecx edx
mov eax,[esi + 00]
bsf ecx,eax
bsr edx,eax
cmp ecx,edx
je .modeSingle
push ecx edx
.scanMask:
bt eax,ecx
jz .endMask
inc ecx
cmp ecx,edx
jb .scanMask
.endMask:
cmp ecx,edx
pop edx ecx
je .modeInterval
.modeIndividual:
xor ecx,ecx
xor edx,edx
.cycleIndividual:
cmp edi,ebp
jae .overIndividual
shr eax,1
jnc .skipIndividual
push eax
test edx,edx
jz .firstIndividual
mov al,','
stosb
.firstIndividual:
inc edx
mov eax,ecx
mov bl,0
call DecimalPrint32
pop eax
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
pop edx ecx ebp esi
ret

AffinityMaskHelper:
push esi
movzx eax,al
add esi,eax
lea edi,[edx + 18]
mov ax,[esi + 08]
call HexPrint16
pop esi
ret

ParameterHelper8:
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,byte [esi + eax]
ParameterEntry:
push ecx esi
lea edi,[edx + edi]
push eax
xchg eax,ebx
call IndexString
call StringWrite
pop eax
mov bl,0
call DecimalPrint32
pop esi ecx
ret

ParameterHelper16:
shld ebx,eax,16
movzx edi,ah
movzx eax,al
movzx eax,word [esi + eax]
jmp ParameterEntry

ParameterHelper32:
shld ebx,eax,16
movzx edi,ah
movzx eax,al
mov eax,dword [esi + eax]
jmp ParameterEntry

CacheSummaryHelper:
lea esi,[ebp + BASE_ENUM]
push 0 0 ebp
mov ebp,esp
mov ecx,[BufferLength]
test ecx,ecx
jz .nodata
add ecx,esi
movzx ebx,ah
xchg edx,eax
.scanCaches:
cmp dword [esi + 00],2
jne .done
cmp byte [esi + 08],dl
jne .done
cmp dword [esi + 16],ebx
jne .done
mov eax,[esi + 12]
mov [ebp + 04],eax
inc dword [ebp + 08]
.done:
mov eax,[esi + 04]
add esi,eax
cmp esi,ecx
jb .scanCaches
cmp dword [ebp + 08],0
je .nodata
mov bh,dl
mov edx,edi
cld
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
push edi
lea edi,[edx + 01]
mov ax,STRING_L
call IndexString
call StringWrite
mov al,bh
or al,30h
stosb
movzx ax,bl
add al,STRING_UNIFIED
lea edi,[edx + 04] 
call RelationNameEntry
lea edi,[edx + 18]
mov bl,0FFh
push edx
mov eax,[ebp + 04]
xor edx,edx
call SizePrint64
pop edx
lea edi,[edx + 41]
mov ax,STRING_X
call IndexString
call StringWrite
mov eax,[ebp + 08]
mov bl,0
call DecimalPrint32
pop edi
.nodata:
pop ebp eax eax
ret

ReleaseMemoryHelper:
mov ecx,[AllocationBase]
jecxz @f
push MEM_RELEASE
push 0
push ecx
call [VirtualFree]
@@:
ret

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
mov eax,[ebp + 28]
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

SizePrint64:
pushad
cld
xor ecx,ecx
test eax,eax
jnz .unitsAutoCycle
test edx,edx
jz .decimalMode
xor ebp,ebp
xor esi,esi
.unitsAutoCycle:
mov ebp,eax
shrd eax,edx,10
shr edx,10
jnz .above32bit 
cmp cl,bl
je .modNonZero
xor esi,esi
shrd esi,ebp,10
shr esi,22
cmp bl,0FFh
jne .above32bit 
test esi,esi
jnz .modNonZero
.above32bit:                
inc ecx
jmp .unitsAutoCycle
.modNonZero:
cmp ecx,4
ja .hexMode
mov eax,ebp
.decimalMode:
push ebx
mov bl,0
call DecimalPrint32
pop ebx
jecxz .afterNumber
cmp bl,0FFh
je .afterNumber
mov al,'.'
stosb
xchg eax,esi
xor edx,edx
mov ebx,102
div ebx
cmp eax,9
jbe .limitDecimal
mov eax,9
.limitDecimal:
mov bl,0
call DecimalPrint32
.afterNumber:
mov al,' '
stosb
lea esi,[U_B]
mov al,cl
mov ah,4
call StringWriteSelected
jmp .exit
.hexMode:
call HexPrint64
mov al,'h'
stosb 
.exit:
mov [esp],edi
popad
ret

section '.data' data readable writeable
MsgGuiFailed                       DB  'GUI initialization failed.'        , 0
MsgLibraryNotFound                 DB  'OS API initialization failed.'     , 0 
MsgFunctionNotFound                DB  'OS not supports topology list.'    , 0
MsgMemoryFailed                    DB  'Memory allocation error.'          , 0 
MsgTopologyFailed                  DB  'Scan topology failed.'             , 0
NameDll                            DB  'KERNEL32.DLL'                      , 0
NameFunctionGet                    DB  'GetLogicalProcessorInformationEx'  , 0
U_B                                DB  'Bytes',0
U_KB                               DB  'KB',0
U_MB                               DB  'MB',0
U_GB                               DB  'GB',0
U_TB                               DB  'TB',0
U_MBPS                             DB  'MBPS',0
U_NS                               DB  'nanoseconds',0
AppControl                         INITCOMMONCONTROLSEX  8, 0
ProcBinders                        DD  BindString
                                   DD  BindInfo
                                   DD  BindBig
                                   DD  BindBool
                                   DD  BindCombo
ProcCombo                          DD  BindComboStopOn
                                   DD  BindComboStopOff 
                                   DD  BindComboCurrent
                                   DD  BindComboAdd
                                   DD  BindComboInactive
AllocationBase                     DD  0
BufferLength                       DD  0
HandleThis                         DD  ? 
HandleIcon                         DD  ?
LockedStrings                      DD  ?
LockedBinders                      DD  ?
HandleFont                         DD  ?
HandleDll                          DD  ?
_GetLogicalProcessorInformationEx  DD  ?  
align 4096 
INFO_BUFFER                        DB  INFO_BUFFER_SIZE DUP (?)
NISC_BUFFER                        DB  MISC_BUFFER_SIZE DUP (?)  

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
DB  'SMP extended topology list (ia32 v0.0)' , 0
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
DB  'Processor group'    , 0
DB  'Unknown ID'         , 0
DB  'Unified'            , 0
DB  'Instruction'        , 0
DB  'Data'               , 0
DB  'Trace'              , 0
DB  'Unknown'            , 0
DB  ' ...'               , 0
DB  'ht='                , 0
DB  'efficiency='        , 0
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
icon exegicon, exeicon, 'images\fasmicon32.ico'
resource manifests, 1, LANG_NEUTRAL, manifest
resdata manifest
db '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
db '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">'
db '<assemblyIdentity'
db '    name="Extended topology viewer"'
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

