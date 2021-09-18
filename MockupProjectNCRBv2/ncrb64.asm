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
; NCRB64.ASM = source file for FASM                                                   ; 
; NCRB64.EXE = translation result, application NCRB64.EXE main module                 ;
; See also other components:                                                          ;
; NCRB32.ASM, DATA.ASM, KMD32.ASM, KMD64.ASM.                                         ;
;                                                                                     ;
; Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 ).                     ;
; http://flatassembler.net/                                                           ;
;                                                                                     ;
; Edit by FASM Editor 2.0.                                                            ; 
; Use this editor for correct source file tabulations and format. (!)                 ;
; https://fasmworld.ru/instrumenty/fasm-editor-2-0/                                   ;
;                                                                                     ;
; User mode debug by OllyDbg ( 32-bit, actual for other module NCRB32.EXE )           ;
; http://www.ollydbg.de/version2.html                                                 ;
;                                                                                     ;
; User mode debug by FDBG ( 64-bit, actual for this module NCRB64.EXE )               ;
; https://board.flatassembler.net/topic.php?t=9689&postdays=0&postorder=asc&start=180 ;
; ( Search for archive fdbg0025.zip )                                                 ;
;                                                                                     ;
; Icons from open icon library                                                        ;
; https://sourceforge.net/projects/openiconlibrary/                                   ;
;                                                                                     ;
;=====================================================================================;

;---------- Global definitions ------------------------------------------------;
include 'win64a.inc'               ; FASM definitions
include 'global\definitions.inc'   ; NCRB project global definitions
include 'global\registry64.inc'    ; Registry for dynamically created variables
ID_EXE_ICON             = 100      ; This application icon
ID_EXE_ICONS            = 101      ; This application icon group
MSG_MEMORY_ALLOC_ERROR  = 0        ; Error messages IDs, from this file
MSG_INIT_FAILED         = 1        ; Note. Resource DLL cannot be used for this 
MSG_LOAD_FAILED         = 2        ; messages:
MSG_HANDLE_NULL         = 3        ; it must be valid before DLL loaded 
MSG_ICON_FAILED         = 4 
MSG_STRINGS_POOL_FAILED = 5
MSG_ICONS_POOL_FAILED   = 6
MSG_CONTROL_POOL_FAILED = 7
MSG_DIALOGUE_FAILED     = 8

;------------------------------------------------------------------------------;
;                                Code section.                                 ;        
;------------------------------------------------------------------------------;

format PE64 GUI 5.0
entry start
section '.code' code readable executable
start:

;---------- Application entry point, memory allocation for registry -----------;
sub rsp,8 + 32                     ; Stack alignment + Parameters shadow
cld
mov r9d,PAGE_READWRITE             ; R9  = Parm#4 = memory protection 
mov r8d,MEM_COMMIT + MEM_RESERVE   ; R8  = Parm#3 = allocation type 
mov edx,REGISTRY64_MEMORY_SIZE     ; RDX = Parm#2 = required block size
xor ecx,ecx                        ; RCX = Parm#1 = fixed address, not used = 0
call [VirtualAlloc]
test rax,rax
jz .memoryAllocError               ; Go if memory allocation error
mov [Registry],rax
mov r15,rax                        ; R15 = Pointer to Registry
;---------- Allocate temporary buffer -----------------------------------------;
add eax,REGISTRY64_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE
mov [r15 + REGISTRY64.allocatorTempBuffer.objectStart],rax
lea rax,[r15 + REGISTRY64_MEMORY_SIZE] 
mov [r15 + REGISTRY64.allocatorTempBuffer.objectStop],rax
;---------- Pre-load ADVAPI32.DLL ---------------------------------------------;
; Pre-load library ADVAPI32.DLL required, because it not loaded by static
; import. Note pre-load KERNEL32.DLL is not required because it loaded by
; static import.
lea rcx,[NameAdvapi32]
call [LoadLibrary]
mov [r15 + APPDATA.hAdvapi32],rax  ; Library handle or 0 if error
;---------- Start GUI initialization ------------------------------------------;
lea rcx,[AppCtrl]                  ; RCX = Parm#1 = Pointer to structure
call [InitCommonControlsEx]        ; GUI controls initialization
test rax,rax
jz .initFailed                     ; Go if initialization error detected
;---------- Load resources DLL, same data DLL for ia32 and x64 ----------------;
mov r8d,LOAD_LIBRARY_AS_DATAFILE   ; R8  = Parm#3 = Load options, flags
xor edx,edx                        ; RDX = Parm#2 = Handle, reserved = 0 
lea rcx,[NameResDll]               ; RCX = Parm#1 = Pointer to file name
call [LoadLibraryEx]               ; Load resources DLL
test rax,rax
jz .loadFailed                     ; Go if load resources DLL error
mov [r15 + APPDATA.hResources],rax ; Store resources DLL handle
;---------- Get handle of this application exe file ---------------------------;
xor ecx,ecx                        ; RCX = Parm#1 = 0 = means this exe file 
call [GetModuleHandle]             ; Get handle of this exe file
test rax,rax
jz .thisFailed                     ; Go if this module handle = NULL 
mov [r15 + APPDATA.hInstance],rax  ; Store handle of current module ( exe file ) 
;---------- Get handle of this application icon -------------------------------;
mov edx,ID_EXE_ICONS               ; RDX = Parm#2 = Resource ID
xchg rcx,rax                       ; RCX = Parm#1 = Module handle for resource 
call [LoadIcon]                    ; Load application icon, from this exe file
test rax,rax
jz .iconFailed                     ; Go if load error, icon handle = NULL
mov [r15 + APPDATA.hIcon],rax      ; Store handle of application icon
;---------- Get handle and address pointer to strings pool at resources DLL ---;
mov r8d,RT_RCDATA                  ; R8  = Parm#3 = Resource type
mov edx,IDS_STRINGS_POOL           ; RDX = Parm#2 = Res. name, used numeric ID
mov rcx,[r15 + APPDATA.hResources] ; RCX = Parm#1 = Module handle, load from DLL
call [FindResource]                ; Find resource, get handle of block                
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .stringsPoolFailed              ; Go if handle = NULL, means error
xchg rdx,rax                       ; RDX = Parm#2 = Handle of resource block
mov rcx,[r15 + APPDATA.hResources] ; RCX = Parm#1 = Module handle, load from DLL
call [LoadResource]                ; Load resource, get resource handle 
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .stringsPoolFailed              ; Go if handle = NULL, means error
xchg rcx,rax                       ; RCX = Parm#1 = Resource handle
call [LockResource]                ; Lock resource, get address pointer  
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .stringsPoolFailed              ; Go if handle = NULL, means error
mov [r15 + APPDATA.lockedStrings],rax  ; Store pointer to strings pool
;---------- Get handles and address pointers to tabs icons at resources DLL ---;
mov ebx,ICON_FIRST                   ; EBX = Icons identifiers
lea rdi,[r15 + APPDATA.lockedIcons]  ; RDI = Pointer to icons pointers list
mov esi,ICON_COUNT                   ; ESI = Number of loaded icons
;---------- Cycle for load icons from resource DLL ----------------------------;
.loadIcons:
mov r8d,RT_GROUP_ICON
mov edx,ebx
mov rcx,[r15 + APPDATA.hResources]
call [FindResource]
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .iconsPoolFailed                ; Go if handle = NULL, means error
xchg rdx,rax
mov rcx,[r15 + APPDATA.hResources]
call [LoadResource] 
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .iconsPoolFailed                ; Go if handle = NULL, means error
xchg rcx,rax
call [LockResource] 
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .iconsPoolFailed                ; Go if pointer = NULL, means error
stosq                              ; Store pointer to icon                 
inc ebx                            ; EBX = Next icon
dec esi                            ; ESI = Cycle counter  
jnz .loadIcons                     ; Cycle for initializing all icons 
;---------- Get handle and address pointers to raw resource at resources DLL --;
; Binders script used for interconnect GUI and System Information routines 
mov r8d,RT_RCDATA                  ; R8  = Parm#3 = Resource type
mov edx,IDS_BINDERS_POOL           ; RDX = Parm#2 = Res. name, used numeric ID
mov rcx,[r15 + APPDATA.hResources] ; RCX = Parm#1 = Module handle, load from DLL
call [FindResource]                ; Find resource, get handle of block                
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .controlPoolFailed              ; Go if handle = NULL, means error
xchg rdx,rax                       ; RDX = Parm#2 = Handle of resource block
mov rcx,[r15 + APPDATA.hResources] ; RCX = Parm#1 = Module handle, load from DLL
call [LoadResource]                ; Load resource, get resource handle 
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .controlPoolFailed              ; Go if handle = NULL, means error
xchg rcx,rax                       ; RCX = Parm#1 = Resource handle
call [LockResource]                ; Lock resource, get address pointer  
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .controlPoolFailed              ; Go if handle = NULL, means error
mov [r15 + APPDATA.lockedBinders],rax  ; Store pointer to binders pool
;---------- Create and show main dialogue window ------------------------------; 
push 0 0                       ; Parm#5 = Pass value, plus alignment qword 
lea r9,[DialogProcMain]        ; R9  = Parm#4 = Pointer to dialogue proced.
mov r8d,HWND_DESKTOP           ; R8  = Parm#3 = Owner window handle
mov edx,IDD_MAIN               ; RDX = Parm#2 = Resource ( template ) id
mov rcx,[r15 + APPDATA.hResources]  ; RCX = Parm#1 = Handle of resource module  
sub rsp,32                     ; Allocate parameters shadow
call [DialogBoxParam]          ; Create modal dialogue 
add rsp,32 + 16                ; Remove parameters shadow and 2 parameters
test rax,rax
jz .dialogueFailed             ; Go if create dialogue return error 
cmp rax,-1
je .dialogueFailed             ; Go if create dialogue return error
;---------- Application exit point with release resource ----------------------; 
xor r14d,r14d                      ; R14 = Exit Code, 0 means no errors
.exitResources:
mov r15,[Registry]
test r15,r15
jz .exit
mov rcx,[r15 + APPDATA.hAdvapi32]  ; RCX = Library ADVAPI32.DLL handle
jrcxz .skipUnload                  ; Go skip unload if handle = null
call [FreeLibrary]                 ; Unload ADVAPI32.DLL
.skipUnload:
mov r8d,MEM_RELEASE                ; R8  = Parm#3 = Memory free operation type
xor edx,edx                        ; RDX = Parm#2 = Size, 0 = by allocated
mov rcx,r15                        ; RCX = Parm#1 = Memory block base address  
call [VirtualFree]                 ; Release memory, allocated for registry
.exit:
mov ecx,r14d                       ; RCX = Parm#1 = exit code           
call [ExitProcess]
;---------- This entry points used if application start failed ----------------; 
.dialogueFailed:
mov al,MSG_DIALOGUE_FAILED     ; AL = String pool index for error name
jmp .errorProgram
.controlPoolFailed:
mov al,MSG_CONTROL_POOL_FAILED
jmp .errorProgram
.iconsPoolFailed:
mov al,MSG_ICONS_POOL_FAILED
jmp .errorProgram
.stringsPoolFailed:
mov al,MSG_STRINGS_POOL_FAILED
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
lea rsi,[MsgErrors]    ; RSI = Strings pool base, AL = String index 
mov ah,0
call IndexString       ; Return ESI = Selected string address 
mov r9d,MB_ICONERROR   ; R9  = Parm#4 = Attributes
lea r8,[ProgName]      ; R8  = Parm#3 = Pointer to title (caption) string
mov rdx,rsi            ; RDX = Parm#2 = Pointer to string: error name 
xor ecx,ecx            ; RCX = Parm#1 = Parent Window = NULL
call [MessageBox]  
mov r14d,1
jmp .exitResources

;---------- Callback dialogue procedure for main window -----------------------;
; INPUT:   RCX = Parm#1 = HWND = Dialog box handle                             ; 
;          RDX = Parm#2 = UINT = Message                                       ; 
;          R8  = Parm#3 = WPARAM, message-specific                             ;
;          R9  = Parm#4 = LPARAM, message-specific                             ;
; OUTPUT:  RAX = status, TRUE = message recognized and processed               ;
;                        FALSE = not recognized, must be processed by OS,      ;
;                        see MSDN for status exceptions and details            ;  
;------------------------------------------------------------------------------;
PARM_HWNDDLG  EQU  qword [rbp + 40 + 08 + 00]  
PARM_MSG      EQU  qword [rbp + 40 + 08 + 08]
PARM_WPARAM   EQU  qword [rbp + 40 + 08 + 16]
PARM_LPARAM   EQU  qword [rbp + 40 + 08 + 24]
LOW_WPARAM    EQU  dword [rbp + 40 + 08 + 16]
;---------- Entry -------------------------------------------------------------;
DialogProcMain:
cld
push rbp rbx rsi rdi r15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h           ; Stack alignment
sub rsp,32                           ; Make parameters shadow for next calls
mov PARM_HWNDDLG,rcx                 ; Save input parameters to shadow 
mov PARM_MSG,rdx
mov PARM_WPARAM,r8
mov PARM_LPARAM,r9
mov r15,[Registry]                   ; R15 = Pointer to global registry
lea rbx,[r15 + APPDATA.tabCtrlItem]  ; RBX = Pointer to tab item structure
lea rsi,[r15 + APPDATA.hTabDlg]      ; RSI = Pointer to sheets handles array
;---------- Detect message type -----------------------------------------------;
cmp rdx,0000FFFFh
jae .skip
xchg eax,edx                   ; Use EAX for compact CMP
cmp eax,WM_INITDIALOG
je .wminitdialog               ; Go if dialogue initialization message 
cmp eax,WM_COMMAND
je .wmcommand                  ; Go if command message
cmp eax,WM_CLOSE
je .wmclose                    ; Go if window close message
cmp eax,WM_NOTIFY
je .tabproc                    ; Go if notification message from child window
.skip:
xor eax,eax
jmp .finish                    ; Go exit if unknown event

;---------- WM_INITDIALOG handler: create main window -------------------------; 
.wminitdialog: 
mov rax,PARM_HWNDDLG
mov [r15 + APPDATA.hMain],rax
mov edx,IDC_TAB                ; RDX = Parm#2 = Item identifier 
mov rcx,PARM_HWNDDLG           ; RCX = Parm#1 = Handle to dialog box
call [GetDlgItem]              ; Get handle
mov [r15 + APPDATA.hTab],rax   ; Store window handle = sheets container handle
;---------- Initializing sheet structure --------------------------------------;
xor eax,eax
mov [rbx + TC_ITEM.mask],TCIF_TEXT + TCIF_IMAGE
mov [rbx + TC_ITEM.lpReserved1],eax
mov [rbx + TC_ITEM.lpReserved2],eax
mov [rbx + TC_ITEM.lParam],eax
mov [rbx + TC_ITEM.cchTextMax],64  ; Maximum text size
;---------- Create image list for icons ---------------------------------------;
push 0 0                         ; Alignment + Parm#5 = cGrow = not used
mov r9d,ICON_COUNT               ; R9  = Parm#4 = Images count
mov r8d,ILC_COLOR32 + ILC_MASK   ; R8  = Parm#3 = Images flags
mov edx,16                       ; RDX = Parm#2 = Y size
mov ecx,16                       ; RCX = Parm#1 = X size
sub rsp,32
call [ImageList_Create]
add rsp,32 + 16
mov [r15 + APPDATA.hImageList],rax   ; Store image list handle 
;---------- Initialize cycle for create icons from resource -------------------;
push rsi rdi
mov edi,ICON_COUNT
lea rsi,[r15 + APPDATA.lockedIcons]
;---------- Cycle for create icons from resource ------------------------------;
.createIcons:
lodsq
push 0                           ; This for stack alignment
push LR_DEFAULTCOLOR             ; Parm#7 = Flags
push 16                          ; Parm#6 = cyDesired
push 16                          ; Parm#5 = cxDesired
mov r9d,30000h                   ; R9  = Parm#4 = Version of icon format
mov r8d,TRUE                     ; R8  = Parm#3 = Icon/Cursor, True means Icon
mov edx,468h                     ; RDX = Parm#2 = dwResSize, bytes 
xchg rcx,rax                     ; RCX = Parm#1 = Pointer to resource bits 
sub rsp,32
call [CreateIconFromResourceEx]     ; Create icon, return handle
add rsp,32 + 32
xchg rdx,rax                        ; RDX = Parm#2 = Handle to icon
mov rcx,[r15 + APPDATA.hImageList]  ; RCX = Parm#1 = Handle to image list 
call [ImageList_AddIcon] 
dec edi
jnz .createIcons
pop rdi rsi
mov r9,[r15 + APPDATA.hImageList]   ; R9  = Parm#4 = LPARAM = image list handle     
xor r8d,r8d                         ; R8  = Parm#3 = WPARAM = not used = 0                 
mov edx,TCM_SETIMAGELIST            ; RDX = Parm#2 = Message
mov rcx,[r15 + APPDATA.hTab]        ; RCX = Parm#1 = Container window handle 
call [SendMessage]                  ; Link image list with control
;---------- Initialize cycle for insert items with strings and icons ----------;
push rsi
sub rsp,32 + 8                      ; Stack alignment and parameters shadow
mov rsi,[r15 + APPDATA.lockedStrings]
xor edi,edi
;---------- Cycle for insert items to tabbed panel ----------------------------;
.insertSheets:
mov rax,rsi
mov [rbx + TC_ITEM.pszText],rax
mov [rbx + TC_ITEM.iImage],edi
mov r9,rbx                       ; R9  = Parm#4 = LPARAM = pointer to TCITEM
mov r8d,ITEM_COUNT - 1           ; R8  = Parm#3 = WPARAM = index for new tab 
mov edx,TCM_INSERTITEM           ; RDX = Parm#2 = Message
mov rcx,[r15 + APPDATA.hTab]     ; RCX = Parm#1 = Container window handle
call [SendMessage]               ; Add this sheet to tabbed panel
.skipString:
lodsb
cmp al,0
jne .skipString                  ; Cycle for skip string
inc edi
cmp edi,ITEM_COUNT
jb .insertSheets                 ; Cycle for insert all sheets
add rsp,32 + 8
pop rsi
;---------- Set item size for container ---------------------------------------;
mov r9d,( 27 shl 16 + 97 )       ; R9  = Parm#4 = LPARAM = [Ysize][Xsize]
xor r8d,r8d                      ; R8  = Parm#3 = WPARAM = not used = 0
mov edx,TCM_SETITEMSIZE          ; RDX = Parm#2 = Message
mov rcx,[r15 + APPDATA.hTab]     ; RCX = Parm#1 = Container window handle
call [SendMessage]               ; Set sheets size 
;---------- Initializing cycle for create dialogues per sheets ----------------; 
push rsi rsi                     ; Second push for stack alignment
mov rdi,rsi
mov esi,IDD_FIRST
lea rbx,[ProcDialogs]
mov ecx,ITEM_COUNT
;---------- Cycle for create dialogues per sheets -----------------------------;
.createDialogues:
push rcx                         ; RCX saved and stack alignment
push 0                           ; Parm#5 = Passed parameter = not used = 0
mov r9,[rbx]                     ; R9  = Parm#4 = Pointer to callback proc.
mov r8,PARM_HWNDDLG              ; R8  = Parm#3 = Container window handle
mov edx,esi                      ; RDX = Parm#2 = Dialog box resource id 
mov rcx,[r15 + APPDATA.hResources]    ; RCX = Parm#1 = Resource module handle
sub rsp,32
call [CreateDialogParam]         ; Set dialogue with handler for sheet
add rsp,32 + 8
stosq                            ; Store this sheet handle
inc esi
pop rcx
add rbx,8
loop .createDialogues            ; Create dialogues cycle for all sheets 
pop rsi rsi                      ; Second pop for stack alignment
;---------- Cycle for find and select active sheet dialogue window ------------;
mov ecx,ITEM_COUNT
.findActive:
push rcx
mov edx,SW_HIDE                      ; This for all sheets exclude first
cmp ecx,ITEM_COUNT 
jne .showThis
mov edx,SW_SHOWDEFAULT               ; This for first sheet, activate it
.showThis:                           ; RDX = Parm#2 = Window activity mode
lodsq
xchg rcx,rax                         ; RCX = Parm#1 = Window handle 
sub rsp,32 + 8
call [ShowWindow]
add rsp,32 + 8
pop rcx
loop .findActive
;---------- Select active sheet at container ----------------------------------;  
xor r9d,r9d                          ; R9  = Parm#4 = LPARAM = not used = 0 
xor r8d,r8d                          ; R8  = Parm#3 = WPARAM = index
mov [r15 + APPDATA.selectedTab],r8d  ; Active sheet = 0
mov edx,TCM_SETCURSEL                ; RDX = Parm#2 = Message
mov rcx,[r15 + APPDATA.hTab]         ; RCX = Parm#1 = Container window handle 
call [SendMessage]                   ; Set current selected sheet
;---------- Main window icon and text title -----------------------------------; 
mov r9,[r15 + APPDATA.hIcon]         ; R9  = Parm#4 = LPARAM = Icon handle 
mov r8d,ICON_SMALL                   ; R8  = Parm#3 = WPARAM = Icon type 
mov edx,WM_SETICON                   ; RDX = Parm#2 = Message 
mov rcx,PARM_HWNDDLG                 ; RCX = Parm#1 = Window handle
call [SendMessage]                   ; Set main window icon
lea rdx,[ProgName]                   ; RDX = Parm#2 = Pointer to text string
mov rcx,PARM_HWNDDLG                 ; RCX = Parm#1 = Window handle
call [SetWindowText]                 ; Set main window text title
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------; 
.wmcommand:                       ; User input: cancel button or close window
;---------- Detect click "About" item in the main menu ------------------------;
mov eax,LOW_WPARAM
cmp ax,IDM_ABOUT
jne .noabout
;---------- "About" message box and wait user input ---------------------------;
mov r9d,MB_ICONINFORMATION   ; R9  = Parm#4 = Message box icon type = Info
lea r8,[AboutCap]            ; R8  = Parm#3 = Pointer to caption
lea rdx,[AboutText]          ; RDX = Parm#2 = Pointer to "About" string
mov rcx,PARM_HWNDDLG         ; RCX = Parm#1 = Parent window handle
call [MessageBoxA]
jmp .processed
.noabout:
;---------- Detect click "Exit" item in the main menu -------------------------; 
cmp ax,IDM_EXIT
je .wmclose
jmp .processed

;---------- WM_NOTIFY handler: events from child GUI objects ------------------;
.tabproc:                         ; Change sheet selection
cmp LOW_WPARAM,IDC_TAB
jne .skip
mov rax,PARM_LPARAM
cmp [rax + NMHDR.code],TCN_SELCHANGE
jne .skip                         ; Skip if other event, no sheet change
mov eax,[r15 + APPDATA.selectedTab]
mov edx,SW_HIDE                   ; RDX = Parm#2 = Window activity mode  
mov rcx,[rsi + rax*8]             ; RCX = Parm#1 = Window handle 
call [ShowWindow]                 ; Hide current sheet
xor r9d,r9d                       ; R9  = Parm#4 = LPARAM = not used = 0
xor r8d,r8d                       ; R8  = Parm#3 = WPARAM = not used = 0
mov edx,TCM_GETCURSEL             ; RDX = Parm#2 = Message
mov rcx,[r15 + APPDATA.hTab]      ; RCX = Parm#1 = Container window handle
call [SendMessage]                ; Get current selected sheet number 
mov [r15 + APPDATA.selectedTab],eax     ; Update current selected sheet number 
mov edx,SW_SHOWDEFAULT            ; RDX = Parm#2 = Window activity mode
mov rcx,[rsi + rax*8]             ; RCX = Parm#1 = Window handle
call [ShowWindow]                 ; Show current selected sheet
jmp .processed

;---------- WM_CLOSE handler: close window ------------------------------------;
.wmclose:
mov edx,1  ; xor edx,edx          ; RDX = Parm#2 = Result for return
mov rcx,PARM_HWNDDLG              ; RCX = Parm#1 = Window handle
call [EndDialog]

;---------- Exit points -------------------------------------------------------;
.processed:
mov eax,1
.finish:
mov rsp,rbp
pop r15 rdi rsi rbx rbp
ret

;---------- Callback dialogue procedures for tab sheets child windows ---------;
; INPUT:   RCX = Parm#1 = HWND = Dialog box handle                             ; 
;          RDX = Parm#2 = UINT = Message                                       ; 
;          R8  = Parm#3 = WPARAM, message-specific                             ;
;          R9  = Parm#4 = LPARAM, message-specific                             ;
; OUTPUT:  RAX = status, TRUE = message recognized and processed               ;
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
cld
push rbp rbx rsi rdi r15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov PARM_HWNDDLG,rcx           ; Save input parameters to shadow 
mov PARM_MSG,rdx
mov PARM_WPARAM,r8
mov PARM_LPARAM,r9
mov r15,[Registry]             ; R15 = Pointer to global registry
movzx esi,al                   ; ESI = Binder ID for this child window
;---------- Detect message type -----------------------------------------------;
cmp rdx,0000FFFFh
jae .skip
xchg eax,edx                   ; Use EAX for compact CMP
cmp eax,WM_INITDIALOG
je .wminitdialog               ; Go if dialogue initialization message 
cmp eax,WM_COMMAND
je .wmcommand                  ; Go if command message
cmp eax,WM_CLOSE
je .wmclose                    ; Go if window close message
.skip:
xor eax,eax
jmp .finish                    ; Go exit if unknown event

;---------- WM_INITDIALOG handler: create sheet window ------------------------;
.wminitdialog:
mov rbx,rcx
mov eax,esi
call Binder
xchg eax,esi
cmp al,BINDER_MEMORY
jne @f
inc eax
inc eax
call Binder
@@:
jmp .processed

;---------- WM_COMMAND handler: interpreting user input -----------------------;
.wmcommand:
mov eax,r8d
cmp ax,IDB_SYSINFO_CANCEL    ; Detect click "Exit" button in the child window
je .wmclose
jmp .processed

;---------- WM_CLOSE handler: close window ------------------------------------;
.wmclose:
mov rcx,[r15 + APPDATA.hMain]
jrcxz .processed
xor r9d,r9d
xor r8d,r8d
mov edx,WM_CLOSE
call [SendMessage]

;---------- Exit points -------------------------------------------------------;
.processed:
mov eax,1
.finish:
mov rsp,rbp
pop r15 rdi rsi rbx rbp
ret

;---------- Find string in the pool by index ----------------------------------;
; INPUT:   RSI = Pointer to string pool                                        ;
;          AX  = String index in the strings pool                              ;
; OUTPUT:  RSI = Updated pointer to string, selected by index                  ;  
;------------------------------------------------------------------------------;
IndexString:
cld
movzx rcx,ax
jrcxz .stop
.cycle:
lodsb
cmp al,0
jne .cycle
loop .cycle
.stop:
ret

;---------- Execute binder in the binders pool by index -----------------------;
; INPUT:   RBX = Current window handle for get dialogue items                  ;
;          AX  = Binder index in the binders pool                              ;
; OUTPUT:  None                                                                ;  
;------------------------------------------------------------------------------;
Binder:
push rbx rsi rdi rbp r14 r15 
cld
mov r15,[Registry]      ; R15 = Pointer to global registry
mov rsi,[r15 + APPDATA.lockedBinders]
movzx rcx,ax
jrcxz .found            ; Go if selected first binder, index = 0
.find:
lodsb
add rsi,3               ; 3 bytes can not exist, skip (not read by LODSD) it
test al,00111111b
jnz .find               ; Go  continue scan if list terminator not found
sub rsi,3               ; Terminator opcode is single-byte, return pointer back
loop .find              ; Cycle for skip required number of binders
.found:                 ; Start execute selected binder
lodsd
mov edx,eax
mov ecx,eax
shr eax,6
and eax,00001FFFh       ; EAX = first 13-bit parameter
shr edx,6+13
and edx,00001FFFh       ; EDX = second 13-bit parameter
and ecx,00111111b
push rsi
call [ProcBinders + rcx * 8 - 8]  ; call by ECX = Binder index
pop rsi
cmp byte [rsi],0
jne .found                   ; cycle for next instruction of binder
pop r15 r14 rbp rdi rsi rbx
ret
;---------- Script handler: bind indexed string from pool to GUI object -------;  
BindString:                  ; RDX = Parm#2 = Resource ID for GUI item
mov rsi,[r15 + APPDATA.lockedStrings]
call IndexString             ; Return RSI = Pointer to selected string
BindEntry:
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rcx,rbx                  ; RCX = Parm#1 = Parent window handle  
call [GetDlgItem]            ; Return handle of GUI item
test rax,rax
jz BindExit                  ; Go skip if error, item not found
mov r9,rsi                   ; R9  = Parm#4 = lParam = Pointer to string
xor r8d,r8d                  ; R8  = Parm#3 = wParam = Not used
mov edx,WM_SETTEXT           ; RDX = Parm#2 = Msg
xchg rcx,rax                 ; RCX = Parm#1 = hWnd 
call [SendMessage]           ; Set string for GUI item
BindExit:
mov rsp,rbp
ret
;---------- Script handler: bind string from temporary buffer to GUI object ---;
BindInfo:
mov rsi,[r15 + REGISTRY64.allocatorTempBuffer.objectStart]
add rsi,rax
jmp BindEntry
;---------- Script handler: enable or disable GUI object by binary flag -------;
BindBool:
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov ecx,eax
shr eax,3
and ecx,0111b
mov r8,[r15 + REGISTRY64.allocatorTempBuffer.objectStart]
movzx eax,byte [r8 + rax]
bt eax,ecx
setc al
xchg esi,eax
mov rcx,rbx                       ; RCX = Parm#1 = Parent window handle  
call [GetDlgItem]                 ; Return handle of GUI item
test rax,rax
jz BindExit                       ; Go skip if error, item not found
mov edx,esi
xchg rcx,rax 
call [EnableWindow]
jmp BindExit
;---------- Script handler: operations with combo box -------------------------; 
BindCombo:                        ; RDX = Parm#2 = Resource ID for GUI item
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rsi,[r15 + REGISTRY64.allocatorTempBuffer.objectStart]
add rsi,rax                       ; RSI = Pointer to combo description list
mov rcx,rbx                       ; RCX = Parm#1 = Parent window handle  
call [GetDlgItem]                 ; Return handle of GUI item
test rax,rax                      ; RAX = Handle of combo box
jz .exit                          ; Go skip if error, item not found
xchg rdi,rax                      ; RDI = Handle of combo box
mov r14d,0FFFF0000h               ; R14D = Store:Counter for selected item
.scan:
lodsb                             ; AL = Tag from combo description list 
movzx rax,al
call [ProcCombo + rax * 8]        ; Call handler = F(tag)
inc r14d
jnc .scan                         ; Continue if end tag yet not found
shr r14d,16
cmp r14w,0FFFFh
je .exit
xor r9d,r9d                   ; R9  = Parm#4 = lParam = Not used 
mov r8d,r14d                  ; R8 =  Parm#3 = wParam = Selected item, 0-based 
mov edx,CB_SETCURSEL          ; RDX = Parm#2 = Msg
mov rcx,rdi                   ; RCX = Parm#1 = hWnd 
call [SendMessage]            ; Set string for GUI item
.exit:
mov rsp,rbp
ret
BindComboStopOn:                  ; End of list, combo box enabled
stc
ret
BindComboStopOff:                 ; End of list, combo box disabled (gray) 
stc
ret
BindComboCurrent:                 ; Add item to list as current selected
call HelperBindCombo
shl r14d,16
clc
ret
BindComboAdd:                     ; Add item to list
call HelperBindCombo
clc
ret
BindComboInactive:                ; Add item to list as inactive (gray)
clc
ret
;---------- Helper for add string to combo box list ---------------------------;
; INPUT:   RSI = Pointer to binder script                                      ;
;          RDI = Parent window handle                                          ;
;          R15 = Pointer to application registry for global variables access   ; 
; OUTPUT:  None                                                                ;
;------------------------------------------------------------------------------;
HelperBindCombo:
lodsw
push rsi rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rsi,[r15 + APPDATA.lockedStrings]
call IndexString                  ; Return RSI = Pointer to selected string
mov r9,rsi                        ; R9  = Parm#4 = lParam = Pointer to string
xor r8d,r8d                       ; R8  = Parm#3 = wParam = Not used
mov edx,CB_ADDSTRING              ; RDX = Parm#2 = Msg
mov rcx,rdi                       ; RCX = Parm#1 = hWnd 
call [SendMessage]                ; Set string for GUI item
mov rsp,rbp
pop rbp rsi
ret

;------------------------------------------------------------------------------;
;                              Data section.                                   ;        
;------------------------------------------------------------------------------;

section '.data' data readable writeable
;---------- Common data for application ---------------------------------------;
Registry      DQ  0                        ; Must be 0 for conditional release
AppCtrl       INITCOMMONCONTROLSEX  8, 0   ; Structure for initialization
;---------- Pointers to dialogues callbacks procedures, per each tab sheet ----;
ProcDialogs   DQ  DialogProcSysinfo
              DQ  DialogProcMemory
              DQ  DialogProcMath
              DQ  DialogProcOs
              DQ  DialogProcNativeOs
              DQ  DialogProcProcessor
              DQ  DialogProcTopology
              DQ  DialogProcTopologyEx
              DQ  DialogProcNuma
              DQ  DialogProcGroups
              DQ  DialogProcAcpi
              DQ  DialogProcAffCpuid
              DQ  DialogProcKmd
;---------- Pointers to procedures of GUI bind scripts interpreter ------------;
ProcBinders   DQ  BindString
              DQ  BindInfo
              DQ  BindBool
              DQ  BindCombo
ProcCombo     DQ  BindComboStopOn     ; End of list, combo box enabled
              DQ  BindComboStopOff    ; End of list, combo box disabled (gray) 
              DQ  BindComboCurrent    ; Add item to list as current selected
              DQ  BindComboAdd        ; Add item to list
              DQ  BindComboInactive   ; Add item to list as inactive (gray)
;---------- Libraries for dynamical import ------------------------------------;
NameAdvapi32  DB  'ADVAPI32.DLL' , 0
NameResDll    DB  'DATA.DLL'     , 0
;---------- Text strings about application ------------------------------------;
ProgName      DB  'NUMA CPU&RAM Benchmarks for Win64',0
AboutCap      DB  'Program info',0
AboutText     DB  'NUMA CPU&RAM Benchmark'   , 0Dh,0Ah
              DB  'v2.00.00 for Windows x64' , 0Dh,0Ah
              DB  '(C)2021 Ilya Manusov'     , 0Dh,0Ah,0
;---------- Errors messages strings -------------------------------------------;
MsgErrors     DB  'Memory allocation error.'                 , 0      
              DB  'Initialization failed.'                   , 0
              DB  'Load resource library failed.'            , 0
              DB  'Handle of this module is NULL.'           , 0
              DB  'Load application icon failed.'            , 0
              DB  'Resource DLL strings pool access failed.' , 0
              DB  'Resource DLL icons pool access failed.'   , 0
              DB  'Resource DLL control pool access failed.' , 0
              DB  'Create dialogue window failed.'           , 0

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
icon exegicon, exeicon, 'images\fasm64.ico'
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


