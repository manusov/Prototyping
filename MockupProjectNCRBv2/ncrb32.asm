;=====================================================================================;
;                                                                                     ;
; Project NCRB ( NUMA CPU&RAM Benchmarks v2.xx.xx ).                                  ;
; (C)2021 Ilya Manusov.                                                               ;
; manusov1969@gmail.com                                                               ;
; Previous version v1.xx.xx                                                           ; 
; https://github.com/manusov/NumaCpuAndRamBenchmarks                                  ;
; This version v2.xx.xx ( UNDER CONSTRUCTION )                                        ;
; https://github.com/manusov/Prototyping                                              ; 
;                                                                                     ;
; NCRB32.ASM = source file for FASM                                                   ; 
; NCRB32.EXE = translation result, application NCRB32.EXE main module                 ;
; See also other components:                                                          ;
; NCRB64.ASM, DATA.ASM, KMD32.ASM, KMD64.ASM.                                         ;
;                                                                                     ;
; Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 ).                     ;
; http://flatassembler.net/                                                           ;
;                                                                                     ;
; Edit by FASM Editor 2.0.                                                            ; 
; Use this editor for correct source file tabulations and format. (!)                 ;
; https://fasmworld.ru/instrumenty/fasm-editor-2-0/                                   ;
;                                                                                     ;
; User mode debug by OllyDbg ( 32-bit, actual for this module NCRB32.EXE )            ;
; http://www.ollydbg.de/version2.html                                                 ;
;                                                                                     ;
; User mode debug by FDBG ( 64-bit, actual for other module NCRB64.EXE )              ;
; https://board.flatassembler.net/topic.php?t=9689&postdays=0&postorder=asc&start=180 ;
; ( Search for archive fdbg0025.zip )                                                 ;
;                                                                                     ;
; Icons from open icon library                                                        ;
; https://sourceforge.net/projects/openiconlibrary/                                   ;
;                                                                                     ;
;=====================================================================================;

;---------- Include FASM definitions and modules definitions ------------------;
include 'win32a.inc'               ; FASM definitions
include 'global\definitions.inc'   ; NCRB project global definitions
include 'global\registry32.inc'    ; Registry for dynamically created variables
include 'ncrb32\system_info\connect_equ.inc'
include 'ncrb32\threads_manager\connect_equ.inc'
include 'ncrb32\targets_bandwidth\connect_equ.inc'
include 'ncrb32\targets_latency\connect_equ.inc'
include 'ncrb32\targets_math\connect_equ.inc'
;---------- Global definitions ------------------------------------------------;
ID_EXE_ICON             = 100      ; This application icon
ID_EXE_ICONS            = 101      ; This application icon group
MSG_MEMORY_ALLOC_ERROR  = 0        ; Error messages IDs, from this file
MSG_INIT_FAILED         = 1        ; Note. Resource DLL cannot be used for 
MSG_LOAD_FAILED         = 2        ; this messages:
MSG_HANDLE_NULL         = 3        ; it must be valid before DLL loaded 
MSG_ICON_FAILED         = 4 
MSG_ICONS_POOL_FAILED   = 5
MSG_RAW_RESOURCE_FAILED = 6  
MSG_DIALOGUE_FAILED     = 7

;------------------------------------------------------------------------------;
;                                Code section.                                 ;        
;------------------------------------------------------------------------------;

format PE GUI 4.0
entry start
section '.code' code readable executable
start:

;---------- Application entry point, memory allocation for registry -----------;
cld
push PAGE_READWRITE                 ; Parm#4 = memory protection 
push MEM_COMMIT + MEM_RESERVE       ; Parm#3 = allocation type 
push REGISTRY32_MEMORY_SIZE         ; Parm#2 = required block size
push 0                              ; Parm#1 = fixed address, not used = 0
call [VirtualAlloc]
test eax,eax
jz .memoryAllocError                ; Go if memory allocation error
mov [Registry],eax         
mov ebx,eax                         ; EBX = Pointer to Registry
;---------- Allocate temporary buffer and GUI bind list -----------------------;
add eax,REGISTRY32_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE
mov [ebx + REGISTRY32.allocatorTempBuffer.objectStart],eax
lea eax,[ebx + REGISTRY32_MEMORY_SIZE] 
mov [ebx + REGISTRY32.allocatorTempBuffer.objectStop],eax
lea eax,[ebx + REGISTRY32_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE - BIND_BUFFER_INIT_SIZE] 
mov [ebx + REGISTRY32.allocatorBindBuffer.objectStart],eax
lea eax,[ebx + REGISTRY32_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE] 
mov [ebx + REGISTRY32.allocatorBindBuffer.objectStart],eax
add ebx,REGISTRY32.appData         ; EBX = Pointer to Registry.application data
;---------- Pre-load ADVAPI32.DLL ---------------------------------------------;
; Pre-load library ADVAPI32.DLL required, because it not loaded by static
; import. Note pre-load KERNEL32.DLL is not required because it loaded by
; static import.
push NameAdvapi32
call [LoadLibrary]
mov [ebx + APPDATA.hAdvapi32],eax   ; Library handle or 0 if error
;---------- Start GUI initialization ------------------------------------------;
push AppCtrl                        ; Parm#1 = Pointer to structure
call [InitCommonControlsEx]         ; GUI initialization
test eax,eax
jz .initFailed                      ; Go if initialization error detected
;---------- Load resources DLL ------------------------------------------------;
push LOAD_LIBRARY_AS_DATAFILE       ; Parm#3 = Load options, flags
push 0                              ; Parm#2 = Handle, reserved = 0
push NameResDll                     ; Parm#1 = Pointer to file name
call [LoadLibraryEx]                ; Load resources DLL
test eax,eax                       
jz .loadFailed                      ; Go if load resources DLL error
mov [ebx + APPDATA.hResources],eax  ; Store resources DLL handle
;---------- Get handle of this application exe file ---------------------------;
push 0                             ; Parm#1 = 0 = means this exe file
call [GetModuleHandle]             ; Get handle of this exe file
test eax,eax
jz .thisFailed                     ; Go if this module handle = NULL
mov [ebx + APPDATA.hInstance],eax  ; Store handle of current module ( exe file ) 
;---------- Get handle of this application icon -------------------------------;
push ID_EXE_ICONS                  ; Parm#2 = Resource ID
push eax                           ; Parm#1 = Module handle for resource  
call [LoadIcon]                    ; Load application icon, from this exe file
test eax,eax
jz .iconFailed                     ; Go if load error, icon handle = NULL
mov [ebx + APPDATA.hIcon],eax      ; Store handle of application icon
; Get handles and address pointers to tabs icons at resources DLL 
mov ebp,ICON_FIRST                  ; EBP = Icons identifiers
lea edi,[ebx + APPDATA.lockedIcons] ; EDI = Pointer to icons pointers list
mov esi,ICON_COUNT                  ; ESI = Number of loaded icons
;---------- Cycle for load icons from resource DLL ----------------------------;
.loadIcons:
push RT_GROUP_ICON                 ; Parm#3 = Resource type
push ebp                           ; Parm#2 = Resource name, used numeric ID
push [ebx + APPDATA.hResources]    ; Parm#1 = Module handle, this load from DLL
call [FindResource]                ; Find resource, get handle of block
test eax,eax                       ; EAX = HRSRC, handle of resource block
jz .iconsPoolFailed                ; Go if handle = NULL, means error
push eax                           ; Parm#2 = Resource name, used numeric ID
push [ebx + APPDATA.hResources]    ; Parm#1 = Module handle, this load from DLL
call [LoadResource]                ; Load resource, get resource handle  
test eax,eax                       ; EAX = HGLOBAL, handle of resource data
jz .iconsPoolFailed                ; Go if handle = NULL, means error
push eax                           ; Parm#1 = Resource handle
call [LockResource]                ; Lock resource, get address pointer
test eax,eax                       ; EAX = LPVOID, pointer to resource
jz .iconsPoolFailed                ; Go if pointer = NULL, means error
stosd                              ; Store pointer to icon                 
inc ebp                            ; EBP = Next icon
dec esi                            ; ESI = Cycle counter  
jnz .loadIcons                     ; Cycle for initializing all icons 
;---------- Get handle and address pointer to raw pools at resources DLL ------;
; Strings located at raw resources part, for compact encoding 1 byte per char,
; note standard string resource use 2 byte per char (UNICODE). 
; Binders located at raw resources part,
; note binders script used for interconnect GUI and System Information routines. 
lea esi,[RawList]
lea edi,[ebx + APPDATA.lockedStrings]
.loadRaw:
lodsw
movzx ecx,ax
jecxz .endRaw
push RT_RCDATA                     ; Parm#3 = Resource type
push ecx                           ; Parm#2 = Resource name, used numeric ID
push [ebx + APPDATA.hResources]    ; Parm#1 = Module handle, this load from DLL 
call [FindResource]                ; Find resource, get handle of block
test eax,eax                       ; EAX = HRSRC, handle of resource block
jz .rawResourceFailed              ; Go if handle = NULL, means error
push eax                           ; Parm#2 = Handle of resource block  
push [ebx + APPDATA.hResources]    ; Parm#1 = Module handle, this load from DLL
call [LoadResource]                ; Load resource, get resource handle 
test eax,eax                       ; EAX = HGLOBAL, handle of resource data
jz .rawResourceFailed              ; Go if handle = NULL, means error
push eax                           ; Parm#1 = Resource handle
call [LockResource]                ; Lock resource, get address pointer 
test eax,eax                       ; EAX = LPVOID, pointer to resource
jz .rawResourceFailed              ; Go if pointer = NULL, means error
stosd                              ; Store pointer to binders pool
jmp .loadRaw
.endRaw:


;---------- Load configuration file ncrb.inf ----------------------------------; 



;---------- Get system information, user mode routines ------------------------;
call SysinfoUserMode
; TODO. Error handling.


;---------- Load kernel mode driver kmd32.sys (Win32) or kmd64.sys (Win64) ----;



;---------- Get system information, kernel mode routines ----------------------;



;---------- Create and show main dialogue window ------------------------------;
push 0                          ; Parm#5 = Pass value
push DialogProcMain             ; Parm#4 = Pointer to dialogue procedure
push HWND_DESKTOP               ; Parm#3 = Owner window handle
push IDD_MAIN                   ; Parm#2 = Resource ( template ) id
push [ebx + APPDATA.hResources] ; Parm#1 = Handle of resource module
call [DialogBoxParam]           ; Create modal dialogue 
test eax,eax
jz .dialogueFailed              ; Go if create dialogue return error 
cmp eax,-1                     
je .dialogueFailed              ; Go if create dialogue return error
;---------- Application exit point with release resource ----------------------; 
xor ebp,ebp                     ; EBP = Exit Code, 0 means no errors
.exitResources:
mov ebx,[Registry]
test ebx,ebx
jz .exit
mov esi,ebx
add ebx,REGISTRY32.appData
mov ecx,[ebx + APPDATA.hAdvapi32]  ; ECX = Library ADVAPI32.DLL handle
jecxz .skipUnload                  ; Go skip unload if handle = null
push ecx
call [FreeLibrary]              ; Unload ADVAPI32.DLL
.skipUnload:
push MEM_RELEASE                ; Parm#3 = Memory free operation type
push 0                          ; Parm#2 = Size, 0 = by allocated
push esi                        ; Parm#1 = Memory block base address  
call [VirtualFree]              ; Release memory, allocated for registry
.exit:                          
push ebp                        ; Parm#1 = exit code           
call [ExitProcess]
;---------- This entry points used if application start failed ----------------;
.dialogueFailed:
mov al,MSG_DIALOGUE_FAILED       ; AL = String pool index for error name
jmp .errorProgram
.rawResourceFailed:
mov al,MSG_RAW_RESOURCE_FAILED
jmp .errorProgram
.iconsPoolFailed:
mov al,MSG_ICONS_POOL_FAILED
jmp .errorProgram
.iconFailed:
mov al,MSG_ICON_FAILED
jmp .errorProgram
.thisFailed:
mov al,MSG_HANDLE_NULL
jmp .errorProgram
.loadFailed:
mov al,MSG_LOAD_FAILED
jmp .errorProgram 
.initFailed:
mov al,MSG_INIT_FAILED
jmp .errorProgram
.memoryAllocError:
mov al,MSG_MEMORY_ALLOC_ERROR
.errorProgram:
;---------- Show message box and go epplication termination -------------------;
lea esi,[MsgErrors]    ; ESI = Strings pool base, AL = String index 
mov ah,0
call IndexString       ; Return ESI = Selected string address 
push MB_ICONERROR      ; Parm#4 = Attributes
push ProgName          ; Parm#3 = Pointer to title (caption) string
push esi               ; Parm#2 = Pointer to string: error name 
push 0                 ; Parm#1 = Parent Window = NULL
call [MessageBox]  
mov ebp,1              ; EBP = Exit Code, 1 means error occurred
jmp .exitResources

;---------- Callback dialogue procedure for main window -----------------------;
; Note. This ESP values at procedure entry, +4 because EIP pushed.             ;
;                                                                              ;
; INPUT:   [esp + 04] = Parm#1 = HWND = Dialog box handle                      ; 
;          [esp + 08] = Parm#2 = UINT = Message                                ; 
;          [esp + 12] = Parm#3 = WPARAM, message-specific                      ;
;          [esp + 16] = Parm#4 = LPARAM, message-specific                      ;
; OUTPUT:  EAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;------------------------------------------------------------------------------;
PARM_HWNDDLG  EQU  dword [ebp + 08]  
PARM_MSG      EQU  dword [ebp + 12]
PARM_WPARAM   EQU  dword [ebp + 16]
PARM_LPARAM   EQU  dword [ebp + 20]
;---------- Entry -------------------------------------------------------------;
DialogProcMain:
push ebp
mov ebp,esp
push ebx esi edi
mov edi,[Registry]                   ; EDI = Pointer to global registry
add edi,REGISTRY32.appData           ; EDI = Pointer to registry.appData
lea ebx,[edi + APPDATA.tabCtrlItem]  ; EBX = Pointer to tab item structure
lea esi,[edi + APPDATA.hTabDlg]      ; ESI = Pointer to sheets handles array
;---------- Detect message type -----------------------------------------------;
mov eax,PARM_MSG
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_COMMAND
je .wmcommand
cmp eax,WM_CLOSE
je .wmclose
cmp eax,WM_NOTIFY
je .tabproc
.skip:
xor eax,eax
jmp .finish

;---------- WM_INITDIALOG handler: create main window -------------------------;
.wminitdialog:                   ; Build main window 
mov eax,PARM_HWNDDLG
mov [edi + APPDATA.hMain],eax
push IDC_TAB                         ; Parm#2 = Item identifier
push PARM_HWNDDLG                    ; Parm#1 = Handle to dialog box
call [GetDlgItem]                    ; Get handle
mov [edi + APPDATA.hTab],eax         ; Store sheets container handle
;---------- Initializing sheet structure --------------------------------------;
xor eax,eax
mov [ebx + TC_ITEM.mask],TCIF_TEXT + TCIF_IMAGE
mov [ebx + TC_ITEM.lpReserved1],eax
mov [ebx + TC_ITEM.lpReserved2],eax
mov [ebx + TC_ITEM.lParam],eax
mov [ebx + TC_ITEM.cchTextMax],64  ; Maximum text size
;---------- Create image list for icons ---------------------------------------;
push 0                           ; Parm#5 = cGrow = not used
push ICON_COUNT                  ; Parm#4 = Images count 
push ILC_COLOR32 + ILC_MASK      ; Parm#3 = Images flags
push 16                          ; Parm#2 = Y size
push 16                          ; Parm#1 = X size 
call [ImageList_Create]
mov [edi + APPDATA.hImageList],eax  ; Store image list handle 
;---------- Initialize cycle for create icons from resource -------------------;
push esi edi
lea esi,[edi + APPDATA.lockedIcons]
mov edi,ICON_COUNT
;---------- Cycle for create icons from resource ------------------------------;
.createIcons:
lodsd
push LR_DEFAULTCOLOR             ; Parm#7 = Flags
push 16                          ; Parm#6 = cyDesired
push 16                          ; Parm#5 = cxDesired
push 30000h                      ; Parm#4 = Version of icon format
push TRUE                        ; Parm#3 = Icon/Cursor, True means Icon
push 468h                        ; Parm#2 = dwResSize, bytes 
push eax                         ; Parm#1 = Pointer to resource bits
call [CreateIconFromResourceEx]  ; Create icon, return handle
push eax                         ; Parm#2 = Handle to icon
mov ecx,[Registry]
push [ecx + REGISTRY32.appData.hImageList]  ; Parm#1 = Handle to image list
call [ImageList_AddIcon]
dec edi
jnz .createIcons
pop edi esi
push [edi + APPDATA.hImageList]  ; Parm#4 = LPARAM = image list handle
push 0                           ; Parm#3 = WPARAM = not used = 0
push TCM_SETIMAGELIST            ; Parm#2 = Message
push [edi + APPDATA.hTab]        ; Parm#1 = Container window handle
call [SendMessage]               ; Link image list with control
;---------- Initialize cycle for insert items with strings and icons ----------;
push esi edi
mov esi,[edi + APPDATA.lockedStrings]
xor edi,edi
;---------- Cycle for insert items to tabbed panel ----------------------------;
.insertSheets:
mov ecx,[Registry]
mov eax,esi
mov [ebx + TC_ITEM.pszText],eax
mov [ebx + TC_ITEM.iImage],edi
push ebx                              ; Parm#4 = LPARAM = pointer to TCITEM
push ITEM_COUNT - 1                   ; Parm#3 = WPARAM = index for new tab
push TCM_INSERTITEM                   ; Parm#2 = Message
push [ecx + REGISTRY32.appData.hTab]  ; Parm#1 = Container window handle
call [SendMessage]                    ; Add third sheet to tabbed panel
cld
.skipString:
lodsb
cmp al,0
jne .skipString                  ; Cycle for skip string
inc edi
cmp edi,ITEM_COUNT
jb .insertSheets                 ; Cycle for insert all sheets
pop edi esi
;---------- Set item size for container ---------------------------------------;
push ( 27 shl 16 + 97 )          ; Parm#4 = LPARAM = [Ysize][Xsize]
push 0                           ; Parm#3 = WPARAM = not used = 0
push TCM_SETITEMSIZE             ; Parm#2 = Message
push [edi + APPDATA.hTab]        ; Parm#1 = Container window handle
call [SendMessage]               ; Set sheets size 
;---------- Initializing cycle for create dialogues per sheets ----------------;
push esi edi
mov edi,esi
mov esi,IDD_FIRST
lea ebx,[ProcDialogs]
mov ecx,ITEM_COUNT
;---------- Cycle for create dialogues per sheets -----------------------------;
.createDialogues:
push ecx
mov ecx,[Registry]
push 0                            ; Parm#5 = Passed parameter = not used = 0
push dword [ebx]                  ; Parm#4 = Pointer to callback procedure
push PARM_HWNDDLG                 ; Parm#3 = Container window handle
push esi                          ; Parm#2 = Dialog box resource id
push [ecx + REGISTRY32.appData.hResources]   ; Parm#1 = Resource module handle
call [CreateDialogParam]          ; Set dialogue with handler for third sheet
stosd                             ; Store third sheet handle
inc esi
pop ecx
add ebx,4
loop .createDialogues             ; Create dialogues cycle for all sheets
pop edi esi
;---------- Cycle for find and select active sheet dialogue window ------------;
mov ecx,ITEM_COUNT
.findActive:
push ecx
mov eax,SW_HIDE                    ; This for all sheets exclude first
cmp ecx,ITEM_COUNT
jne .showThis
mov eax,SW_SHOWDEFAULT             ; This for first sheet, activate it
.showThis:
push eax                           ; Parm#2 = Window activity mode
lodsd
push eax                           ; Parm#1 = Window handle
call [ShowWindow]
pop ecx
loop .findActive
;---------- Select active sheet at container ----------------------------------;
xor eax,eax                          ; EAX = 0 for compact PUSH 0
mov [edi + APPDATA.selectedTab],eax  ; Active sheet = 0  
push eax                             ; Parm#4 = LPARAM = not used = 0 
push eax                             ; Parm#3 = WPARAM = index = [selectedTab]
push TCM_SETCURSEL                   ; Parm#2 = Message
push [edi + APPDATA.hTab]            ; Parm#1 = Container window handle
call [SendMessage]                   ; Set current selected sheet
;---------- Main window icon and text title -----------------------------------;
push [edi + APPDATA.hIcon]        ; Parm#4 = LPARAM = Icon handle
push ICON_SMALL                   ; Parm#3 = WPARAM = Icon type
push WM_SETICON                   ; Parm#2 = Message 
push PARM_HWNDDLG                 ; Parm#1 = Window handle
call [SendMessage]                ; Set main window icon
push ProgName                     ; Parm#2 = Pointer to text string
push PARM_HWNDDLG                 ; Parm#1 = Window handle
call [SetWindowText]              ; Set main window text title
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------;
.wmcommand:                       ; User input: cancel button or close window
;---------- Detect click "About" item in the main menu ------------------------;
mov eax,PARM_WPARAM
cmp ax,IDM_ABOUT
jne .noabout
;---------- "About" message box and wait user input ---------------------------;
push MB_ICONINFORMATION   ; Parm#4 = Message box icon type = Info
push AboutCap             ; Parm#3 = Pointer to caption
push AboutText            ; Parm#2 = Pointer to "About" string
push PARM_HWNDDLG         ; Parm#1 = Parent window handle
call [MessageBoxA]
jmp .processed
.noabout:
;---------- Detect click "Exit" item in the main menu -------------------------; 
cmp ax,IDM_EXIT
je .wmclose
jmp .processed

;---------- WM_NOTIFY handler: events from child GUI objects ------------------;
.tabproc:                         ; Change sheet selection
cmp PARM_WPARAM,IDC_TAB
jne .skip
mov eax,PARM_LPARAM
cmp [eax + NMHDR.code],TCN_SELCHANGE
jne .skip                            ; Skip if other event, no sheet change
mov eax,[edi + APPDATA.selectedTab]
push SW_HIDE                         ; Parm#2 = Window activity mode 
push dword [esi + eax*4]             ; Parm#1 = Window handle 
call [ShowWindow]                    ; Hide current sheet
push 0                               ; Parm#4 = LPARAM = not used = 0
push 0                               ; Parm#3 = WPARAM = not used = 0
push TCM_GETCURSEL                   ; Parm#2 = Message
push [edi + APPDATA.hTab]            ; Parm#1 = Container window handle
call [SendMessage]                   ; Get current selected sheet number 
mov [edi + APPDATA.selectedTab],eax  ; Update current selected sheet number 
push SW_SHOWDEFAULT
push dword [esi + eax*4]
call [ShowWindow]                    ; Show current selected sheet
jmp .processed

;---------- WM_CLOSE handler: close window ------------------------------------;
.wmclose:
push 1                            ; Parm#2 = Result for return
push PARM_HWNDDLG                 ; Parm#1 = Window handle
call [EndDialog]

;---------- Exit points -------------------------------------------------------;
.processed:
mov eax,1
.finish:
pop edi esi ebx ebp
ret 16

;---------- Callback dialogue procedures for tab sheets window ----------------;
; Note. This ESP values at procedure entry, +4 because EIP pushed.             ;
;                                                                              ;
; INPUT:   [esp + 04] = Parm#1 = HWND = Dialog box handle                      ; 
;          [esp + 08] = Parm#2 = UINT = Message                                ; 
;          [esp + 12] = Parm#3 = WPARAM, message-specific                      ;
;          [esp + 16] = Parm#4 = LPARAM, message-specific                      ;
; OUTPUT:  EAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;------------------------------------------------------------------------------;
DialogProcSysinfo:
mov al,BINDER_SYSINFO
jmp DialogProcEntry
DialogProcMemory:
mov al,BINDER_MEMORY
jmp DialogProcEntry
DialogProcMath:
mov al,BINDER_MATH
jmp DialogProcEntry
DialogProcOs:
mov al,BINDER_OS
jmp DialogProcEntry
DialogProcNativeOs:
mov al,BINDER_NATIVE_OS
jmp DialogProcEntry
DialogProcProcessor:
mov al,BINDER_PROCESSOR
jmp DialogProcEntry
DialogProcTopology:
mov al,BINDER_TOPOLOGY
jmp DialogProcEntry
DialogProcTopologyEx:
mov al,BINDER_TOPOLOGY_EX
jmp DialogProcEntry
DialogProcNuma:
mov al,BINDER_NUMA
jmp DialogProcEntry
DialogProcGroups:
mov al,BINDER_P_GROUPS
jmp DialogProcEntry
DialogProcAcpi:
mov al,BINDER_ACPI
jmp DialogProcEntry
DialogProcAffCpuid:
mov al,BINDER_AFF_CPUID
jmp DialogProcEntry
DialogProcKmd:
mov al,BINDER_KMD

;---------- Entry point with AL = Binder ID for required child window ---------;
DialogProcEntry:
push ebp
mov ebp,esp
push ebx esi edi
movzx esi,al              ; ESI = Binder ID for this child window
;---------- Detect message type -----------------------------------------------;
mov eax,PARM_MSG
cmp eax,WM_INITDIALOG
je .wminitdialog
cmp eax,WM_COMMAND
je .wmcommand
xor eax,eax
jmp .finish

;---------- WM_INITDIALOG handler: create sheet window ------------------------;
.wminitdialog:
mov ebx,PARM_HWNDDLG
mov eax,esi
call Binder
xchg eax,esi
cmp al,BINDER_MEMORY
jne @f
inc eax
call Binder
@@:
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------;
.wmcommand:
mov eax,PARM_WPARAM
cmp ax,IDB_SYSINFO_CANCEL    ; Detect click "Exit" button in the child window
je .wmclose
jmp .processed

;---------- WM_CLOSE handler: close window ------------------------------------;
.wmclose:
mov ecx,[Registry]
mov ecx,[ecx + REGISTRY32.appData + APPDATA.hMain]
jecxz .processed
push 0
push 0
push WM_CLOSE
push ecx
call [SendMessage]

;---------- Exit points -------------------------------------------------------;
.processed:
mov eax,1
.finish:
pop edi esi ebx ebp
ret 16

;---------- Copy text string terminated by 00h --------------------------------;
; Note last byte 00h not copied.                                               ;
;                                                                              ;
; INPUT:   ESI = Source address                                                ;
;          EDI = Destination address                                           ;
; OUTPUT:  ESI = Modified by copy                                              ;
;          EDI = Modified by copy                                              ;
;          Memory at [Input EDI] modified                                      ;
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
;---------- Find string in the pool by index ----------------------------------;
; INPUT:   ESI = Pointer to string pool                                        ;
;          AX  = Index                                                         ;
; OUTPUT:  ESI = Updated pointer to string, selected by index                  ;  
;------------------------------------------------------------------------------;
IndexString:
cld
movzx ecx,ax
jecxz .stop
.cycle:
lodsb
cmp al,0
jne .cycle
loop .cycle
.stop:
ret
;---------- Find string in the pool by index and write this string ------------;
; INPUT:   AX  = String index in the application resources strings pool        ;
; OUTPUT:  ESI = Updated pointer to string, selected by index                  ;
;          EDI = Modified by copy                                              ;  
;------------------------------------------------------------------------------;
PoolStringWrite:
mov ecx,[Registry]
jecxz .exit
mov esi,[ecx + REGISTRY32.appData.lockedStrings]
test esi,esi
jz .exit
call IndexString
call StringWrite
.exit:
ret
;---------- Print 64-bit Hex Number -------------------------------------------;
; INPUT:  EDX:EAX = Number, EDX=High32, EAX=Low32                              ;
;         EDI = Destination Pointer                                            ;
; OUTPUT: EDI = Modify                                                         ;
;------------------------------------------------------------------------------;
HexPrint64:
xchg eax,edx
call HexPrint32
xchg eax,edx
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number -------------------------------------------;
; INPUT:  EAX = Number                                                         ;
;         EDI = Destination Pointer                                            ;
; OUTPUT: EDI = Modify                                                         ;
;------------------------------------------------------------------------------;
HexPrint32:
push eax
ror eax,16
call HexPrint16
pop eax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number -------------------------------------------;
; INPUT:  AX  = Number                                                         ;
;         EDI = Destination Pointer                                            ;
; OUTPUT: EDI = Modify                                                         ;
;------------------------------------------------------------------------------;
HexPrint16:
push eax
xchg al,ah
call HexPrint8
pop eax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number --------------------------------------------;
; INPUT:  AL  = Number                                                         ;
;         EDI = Destination Pointer                                            ;
; OUTPUT: EDI = Modify	                                                       ;
;------------------------------------------------------------------------------;
HexPrint8:
push eax
ror al,4
call HexPrint4
pop eax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number --------------------------------------------;
; INPUT:  AL  = Number (bits 0-3)                                              ;
;         EDI = Destination Pointer                                            ;
; OUTPUT: EDI = Modify                                                         ;
;------------------------------------------------------------------------------;
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
;---------- Print 32-bit Decimal Number ---------------------------------------;
; INPUT:   EAX = Number value                                                  ;
;          BL  = Template size, chars. 0=No template                           ;
;          EDI = Destination Pointer (flat)                                    ;
; OUTPUT:  EDI = New Destination Pointer (flat)                                ;
;                modified because string write                                 ;
;------------------------------------------------------------------------------;
DecimalPrint32:
cld
push eax ebx ecx edx
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
pop edx ecx ebx eax
ret
;---------- Print double precision value --------------------------------------;
; x87 FPU used, required x87 presence validation by CPUID before call this.    ;
;                                                                              ;
; INPUT:   EDX:EAX = Double precision number, EDX=High32, EAX=Low32            ;
;          BL  = Number of digits in the INTEGER part,                         ;
;                used for add left non-signed zeroes.                          ;
;                BL=0 means not print left unsigned zeroes.                    ;
;          BH  = Number of digits in the FLOAT part,                           ;
;                used as precision control.                                    ;
;          EDI = Destination text buffer pointer                               ;
;                                                                              ;
; OUTPUT:  EDI = Modified by text string write                                 ;
;------------------------------------------------------------------------------;
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
;---------- Execute binder in the binders pool by index -----------------------;
; INPUT:   EBX = Current window handle for get dialogue items                  ;
;          AX  = Binder index in the binders pool                              ;
; OUTPUT:  None                                                                ;  
;------------------------------------------------------------------------------;
Binder:
push ebx esi edi ebp
cld
mov edi,[Registry]
mov esi,[edi + REGISTRY32.appData + APPDATA.lockedBinders]
movzx ecx,ax
jecxz .found          ; Go if selected first binder, index = 0
.find:
lodsb
add esi,3             ; 3 bytes can not exist, skip (not read by LODSD) it
test al,00111111b
jnz .find             ; Go  continue scan if list terminator not found
sub esi,3             ; Terminator opcode is single-byte, return pointer back
loop .find            ; Cycle for skip required number of binders
.found:               ; Start execute selected binder
lodsd
mov edx,eax
mov ecx,eax
shr eax,6
and eax,00001FFFh     ; EAX = first 13-bit parameter
shr edx,6+13
and edx,00001FFFh     ; EDX = second 13-bit parameter
and ecx,00111111b
push esi edi
call [ProcBinders + ecx * 4 - 4]  ; call by ECX = Binder index
pop edi esi
cmp byte [esi],0
jne .found            ; cycle for next instruction of binder
pop ebp edi esi ebx
ret
;---------- Script handler: bind indexed string from pool to GUI object -------;
BindString:
mov esi,[edi + REGISTRY32.appData + APPDATA.lockedStrings]
call IndexString      ; Return ESI = Pointer to selected string
BindEntry:
push edx              ; Parm#2 = Resource ID for GUI item 
push ebx              ; Parm#1 = Parent window handle  
call [GetDlgItem]     ; Return handle of GUI item
test eax,eax
jz .exit              ; Go skip if error, item not found
push esi              ; Parm#4 = lParam = Pointer to string
push 0                ; Parm#3 = wParam = Not used
push WM_SETTEXT       ; Parm#2 = Msg
push eax              ; Parm#1 = hWnd 
call [SendMessage]    ; Set string for GUI item
.exit:
ret
;---------- Script handler: bind string from temporary buffer to GUI object ---;
BindInfo:
mov esi,[edi + REGISTRY32.allocatorBindBuffer.objectStart]
add esi,eax
jmp BindEntry
;---------- Script handler: bind string referenced by pointer to GUI object ---;
BindBig:
mov esi,[edi + REGISTRY32.allocatorBindBuffer.objectStart]
add esi,eax
mov esi,[esi]
test esi,esi
jnz BindEntry
ret
;---------- Script handler: enable or disable GUI object by binary flag -------;
BindBool:
mov ecx,eax
shr eax,3
and ecx,0111b
mov esi,[edi + REGISTRY32.allocatorBindBuffer.objectStart]
movzx eax,byte [esi + eax]
bt eax,ecx
setc al
xchg esi,eax
push edx              ; Parm#2 = Resource ID for GUI item 
push ebx              ; Parm#1 = Parent window handle  
call [GetDlgItem]     ; Return handle of GUI item
test eax,eax
jz .exit              ; Go skip if error, item not found
push esi
push eax 
call [EnableWindow]
.exit:
ret
;---------- Script handler: operations with combo box -------------------------;
BindCombo:
mov esi,[edi + REGISTRY32.allocatorBindBuffer.objectStart]
add esi,eax                   ; ESI = Pointer to combo description list
push edx                      ; Parm#2 = Resource ID for GUI item 
push ebx                      ; Parm#1 = Parent window handle  
call [GetDlgItem]             ; Return handle of GUI item
test eax,eax                  ; EAX = Handle of combo box
jz .exit                      ; Go skip if error, item not found
xchg edi,eax                  ; EDI = Handle of combo box
mov ebp,0FFFF0000h            ; EBP = Store:Counter for selected item
.scan:
lodsb                         ; AL = Tag from combo description list 
movzx eax,al
call [ProcCombo + eax * 4]    ; Call handler = F(tag)
inc ebp
jnc .scan                     ; Continue if end tag yet not found
shr ebp,16
cmp bp,0FFFFh
je .exit
push 0                        ; Parm#4 = lParam = Not used 
push ebp                      ; Parm#3 = wParam = Selected item, 0-based 
push CB_SETCURSEL             ; Parm#2 = Msg
push edi                      ; Parm#1 = hWnd 
call [SendMessage]            ; Set string for GUI item
.exit:
ret
BindComboStopOn:              ; End of list, combo box enabled
stc
ret
BindComboStopOff:             ; End of list, combo box disabled (gray) 
stc
ret
BindComboCurrent:             ; Add item to list as current selected
call HelperBindCombo
shl ebp,16
clc
ret
BindComboAdd:                 ; Add item to list
call HelperBindCombo
clc
ret
BindComboInactive:            ; Add item to list as inactive (gray)
clc
ret

;---------- Helper for add string to combo box list ---------------------------;
; INPUT:   ESI = Pointer to binder script                                      ;
;          EDI = Parent window handle                                          ;
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperBindCombo:
lodsw
push esi
movzx eax,ax
mov esi,[Registry]
mov esi,[esi + REGISTRY32.appData + APPDATA.lockedStrings]
call IndexString      ; Return ESI = Pointer to selected string
push esi              ; Parm#4 = lParam = Pointer to string
push 0                ; Parm#3 = wParam = Not used
push CB_ADDSTRING     ; Parm#2 = Msg
push edi              ; Parm#1 = hWnd 
call [SendMessage]    ; Set string for GUI item
pop esi
ret

;---------- Include subroutines from modules ----------------------------------;
include 'ncrb32\system_info\connect_code.inc'
include 'ncrb32\threads_manager\connect_code.inc'
include 'ncrb32\targets_bandwidth\connect_code.inc'
include 'ncrb32\targets_latency\connect_code.inc'
include 'ncrb32\targets_math\connect_code.inc'

;------------------------------------------------------------------------------;
;                              Data section.                                   ;        
;------------------------------------------------------------------------------;

section '.data' data readable writeable
;---------- Common data for application ---------------------------------------;
Registry      DD  0                        ; Must be 0 for conditional release
AppCtrl       INITCOMMONCONTROLSEX  8, 0   ; Structure for initialization
;---------- Pointers to dialogues callbacks procedures, per each tab sheet ----;
ProcDialogs   DD  DialogProcSysinfo
              DD  DialogProcMemory
              DD  DialogProcMath
              DD  DialogProcOs
              DD  DialogProcNativeOs
              DD  DialogProcProcessor
              DD  DialogProcTopology
              DD  DialogProcTopologyEx
              DD  DialogProcNuma
              DD  DialogProcGroups
              DD  DialogProcAcpi
              DD  DialogProcAffCpuid
              DD  DialogProcKmd
;---------- Pointers to procedures of GUI bind scripts interpreter ------------;
ProcBinders   DD  BindString
              DD  BindInfo
              DD  BindBig
              DD  BindBool
              DD  BindCombo
ProcCombo     DD  BindComboStopOn     ; End of list, combo box enabled
              DD  BindComboStopOff    ; End of list, combo box disabled (gray) 
              DD  BindComboCurrent    ; Add item to list as current selected
              DD  BindComboAdd        ; Add item to list
              DD  BindComboInactive   ; Add item to list as inactive (gray)
;---------- List for load raw resources ---------------------------------------;
RawList       DW  IDS_STRINGS_POOL
              DW  IDS_BINDERS_POOL
              DW  IDS_CPU_COMMON_POOL
              DW  IDS_CPU_AVX512_POOL
              DW  IDS_OS_CONTEXT_POOL
              DW  IDS_CPU_METHOD_POOL
              DW  IDS_ACPI_DATA_POOL
              DW  0
;---------- Libraries for dynamical import ------------------------------------;
NameAdvapi32  DB  'ADVAPI32.DLL' , 0
NameResDll    DB  'DATA.DLL'     , 0
;---------- Text strings about application ------------------------------------;
ProgName      DB  'NUMA CPU&RAM Benchmarks for Win32',0
AboutCap      DB  'Program info',0
AboutText     DB  'NUMA CPU&RAM Benchmark'    , 0Dh,0Ah
              DB  'v2.00.00 for Windows ia32' , 0Dh,0Ah
              DB  '(C)2021 Ilya Manusov'      , 0Dh,0Ah,0
;---------- Errors messages strings -------------------------------------------;
MsgErrors     DB  'Memory allocation error.'                 , 0      
              DB  'Initialization failed.'                   , 0
              DB  'Load resource library failed.'            , 0
              DB  'Handle of this module is NULL.'           , 0
              DB  'Load application icon failed.'            , 0
              DB  'Load icon data from resource DLL failed.' , 0
              DB  'Load raw data from resource DLL failed.'  , 0
              DB  'Create dialogue window failed.'           , 0
;---------- Include constants and pre-defined variables from modules ----------;
include 'ncrb32\system_info\connect_const.inc'
include 'ncrb32\threads_manager\connect_const.inc'
include 'ncrb32\targets_bandwidth\connect_const.inc'
include 'ncrb32\targets_latency\connect_const.inc'
include 'ncrb32\targets_math\connect_const.inc'
;---------- Include non pre-defined variables from modules --------------------;
include 'ncrb32\system_info\connect_var.inc'
include 'ncrb32\threads_manager\connect_var.inc'
include 'ncrb32\targets_bandwidth\connect_var.inc'
include 'ncrb32\targets_latency\connect_var.inc'
include 'ncrb32\targets_math\connect_var.inc'

;------------------------------------------------------------------------------;
;                              Import section.                                 ;        
;------------------------------------------------------------------------------;

section '.idata' import data readable writeable
library kernel32 , 'kernel32.dll', \
        user32   , 'user32.dll'  , \
        comctl32 , 'comctl32.dll'
include 'api\kernel32.inc'
include 'api\user32.inc'
include 'api\comctl32.inc'

;------------------------------------------------------------------------------;
;                            Resources section.                                ;        
;------------------------------------------------------------------------------;

section '.rsrc' resource data readable
directory RT_ICON       , icons,   \
          RT_GROUP_ICON , gicons,  \
          RT_MANIFEST   , manifests
resource icons  , ID_EXE_ICON  , LANG_NEUTRAL , exeicon
resource gicons , ID_EXE_ICONS , LANG_NEUTRAL , exegicon
icon exegicon, exeicon, 'images\fasm32.ico'
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


