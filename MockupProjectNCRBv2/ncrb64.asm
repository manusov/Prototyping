;=========================================================================================================;
;                                                                                                         ;
; Project NCRB ( NUMA CPU&RAM Benchmarks v2.xx.xx ).                                                      ;
; (C)2021 Ilya Manusov.                                                                                   ;
; manusov1969@gmail.com                                                                                   ;
; Previous version v1.xx.xx                                                                               ; 
; https://github.com/manusov/NumaCpuAndRamBenchmarks                                                      ;
; This version v2.xx.xx ( UNDER CONSTRUCTION )                                                            ;
; https://github.com/manusov/Prototyping                                                                  ; 
;                                                                                                         ;
; NCRB64.ASM = source file for FASM                                                                       ; 
; NCRB64.EXE = translation result, application NCRB64.EXE main module                                     ;
; See also other components:                                                                              ;
; NCRB32.ASM, DATA.ASM, KMD32.ASM, KMD64.ASM.                                                             ;
;                                                                                                         ;
; Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 ).                                         ;
; http://flatassembler.net/                                                                               ;
;                                                                                                         ;
; Edit by FASM Editor 2.0.                                                                                ; 
; Use this editor for correct source file tabulations and format. (!)                                     ;
; https://fasmworld.ru/instrumenty/fasm-editor-2-0/                                                       ;
;                                                                                                         ;
; User mode debug by OllyDbg ( 32-bit, actual for other module NCRB32.EXE )                               ;
; http://www.ollydbg.de/version2.html                                                                     ;
;                                                                                                         ;
; User mode debug by FDBG ( 64-bit, actual for this module NCRB64.EXE )                                   ;
; https://board.flatassembler.net/topic.php?t=9689&postdays=0&postorder=asc&start=180                     ;
; ( Search for archive fdbg0025.zip )                                                                     ;
;                                                                                                         ;
; Intel Software Development Emulator ( SDE ) used for debug                                              ;
; https://software.intel.com/content/www/us/en/develop/articles/intel-software-development-emulator.html  ;
;                                                                                                         ;
; Icons from open icon library                                                                            ;
; https://sourceforge.net/projects/openiconlibrary/                                                       ;
;                                                                                                         ;
;=========================================================================================================;

;---------- Include FASM and NCRB definitions ---------------------------------;

include 'win64a.inc'               ; FASM definitions
include 'global\definitions.inc'   ; NCRB project global definitions
include 'global\registry64.inc'    ; Registry for dynamically created variables

;---------- Global definitions ------------------------------------------------;

RESOURCE_DESCRIPTION    EQU 'NCRB Win64 edition'
RESOURCE_VERSION        EQU '0.0.0.1'
RESOURCE_COMPANY        EQU 'https://github.com/manusov'
RESOURCE_COPYRIGHT      EQU '(C) 2021 Ilya Manusov'
PROGRAM_NAME            EQU 'NUMA CPU&RAM Benchmarks for Win64'
ABOUT_CAP               EQU 'Program info'
ABOUT_TEXT_1            EQU 'NUMA CPU&RAM Benchmarks'
ABOUT_TEXT_2A           EQU 'v2.00.00 for Windows x64'
ABOUT_TEXT_2B           EQU 'ENGINEERING SAMPLE #0002 for Windows x64'
ABOUT_TEXT_3            EQU RESOURCE_COPYRIGHT 

ID_EXE_ICON             = 100      ; This application icon
ID_EXE_ICONS            = 101      ; This application icon group
MSG_MEMORY_ALLOC_ERROR  = 0        ; Error messages IDs, from this file
MSG_INIT_FAILED         = 1        ; Note. Resource DLL cannot be used for 
MSG_LOAD_FAILED         = 2        ; this messages:
MSG_HANDLE_NULL         = 3        ; it must be valid before DLL loaded 
MSG_ICON_FAILED         = 4 
MSG_ICONS_POOL_FAILED   = 5
MSG_RAW_RESOURCE_FAILED = 6  
MSG_CREATE_FONT_FAILED  = 7
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

;---------- Allocate temporary buffer and GUI bind list -----------------------;

add eax,REGISTRY64_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE
mov [r15 + REGISTRY64.allocatorTempBuffer.objectStart],rax
lea rax,[r15 + REGISTRY64_MEMORY_SIZE] 
mov [r15 + REGISTRY64.allocatorTempBuffer.objectStop],rax
lea rax,[r15 + REGISTRY64_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE - BIND_BUFFER_INIT_SIZE] 
mov [r15 + REGISTRY64.allocatorBindBuffer.objectStart],rax
lea rax,[r15 + REGISTRY64_MEMORY_SIZE - TEMP_BUFFER_INIT_SIZE] 
mov [r15 + REGISTRY64.allocatorBindBuffer.objectStop],rax
add r15,REGISTRY64.appData         ; R15 = Pointer to Registry.application data

;---------- Pre-load ADVAPI32.DLL ---------------------------------------------;
; Pre-load library ADVAPI32.DLL required, because it not loaded by static
; import. Note pre-load KERNEL32.DLL is not required because it loaded by
; static import. 
; CHANGED: LOADED BY STATIC IMPORT FOR KMD SCP.

; lea rcx,[NameAdvapi32]
; call [LoadLibrary]
; mov [r15 + APPDATA.hAdvapi32],rax  ; Library handle or 0 if error

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

;---------- Get handle and address pointer to raw pools at resources DLL ------;
; Strings located at raw resources part, for compact encoding 1 byte per char,
; note standard string resource use 2 byte per char (UNICODE). 
; Binders located at raw resources part,
; note binders script used for interconnect GUI and System Information routines. 

lea rsi,[RawList]
lea rdi,[r15 + APPDATA.lockedStrings]
.loadRaw:
mov r8d,RT_RCDATA                  ; R8  = Parm#3 = Resource type
lodsw
movzx edx,ax                       ; RDX = Parm#2 = Res. name, used numeric ID
test edx,edx
jz .endRaw
mov rcx,[r15 + APPDATA.hResources] ; RCX = Parm#1 = Module handle, load from DLL
call [FindResource]                ; Find resource, get handle of block                
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .rawResourceFailed              ; Go if handle = NULL, means error
xchg rdx,rax                       ; RDX = Parm#2 = Handle of resource block
mov rcx,[r15 + APPDATA.hResources] ; RCX = Parm#1 = Module handle, load from DLL
call [LoadResource]                ; Load resource, get resource handle 
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .rawResourceFailed              ; Go if handle = NULL, means error
xchg rcx,rax                       ; RCX = Parm#1 = Resource handle
call [LockResource]                ; Lock resource, get address pointer  
test rax,rax                       ; RAX = HRSRC, handle of resource block
jz .rawResourceFailed              ; Go if handle = NULL, means error
stosq                              ; Store pointer to strings pool
jmp .loadRaw
.endRaw:

;---------- Create fonts ------------------------------------------------------;

mov rsi,[r15 + APPDATA.lockedFontList]
lea rdi,[r15 + APPDATA.hFont1]
.createFonts:
xor eax,eax
movzx ecx,word [rsi + 00]
jrcxz .doneFonts
lea rdx,word [rsi + 16]
push rdx
movzx edx,word [rsi + 14]
push rdx
movzx edx,word [rsi + 12]
push rdx
movzx edx,word [rsi + 10]
push rdx
movzx edx,word [rsi + 08]
push rdx
movzx edx,word [rsi + 06]
push rdx
push rax
push rax
push rax
movzx edx,word [rsi + 04]
push rdx
xor r9d,r9d
xor r8d,r8d
xor edx,edx
sub rsp,32
call [CreateFont]
add rsp,32+80
test rax,rax
jz .createFontFailed
stosq
add rsi,16
@@:
lodsb
cmp al,0
jne @b
jmp .createFonts
.doneFonts:

;---------- Load configuration file ncrb.inf ----------------------------------; 
; TODO.


;---------- Get system information, user mode routines ------------------------;

call SysinfoUserMode
; TODO. Error handling.

; mov ax,STR_ERROR_CPUID
; mov ax,STR_ERROR_CPUID_F1    
; mov ax,STR_ERROR_X87
; mov ax,STR_ERROR_TSC
; mov ax,STR_ERROR_TSC_FREQ
; mov ax,STR_ERROR_MEMORY_API
; mov ax,STR_ERROR_TOPOLOGY_API
; jmp .errorPlatform


;---------- Load kernel mode driver kmd64.sys ---------------------------------;
; TODO.

; INT3
; call LoadKernelModeDriver
; call TryKernelModeDriver
; call UnloadKernelModeDriver

;---------- Get system information, kernel mode routines ----------------------;
; TODO.

;---------- Check dynamical import results, show missing WinAPI warning -------;
; Application can start with this non-fatal warning.

mov rsi,[Registry] 
mov rdi,[rsi + REGISTRY64.allocatorTempBuffer.objectStart]
push rdi
mov rdx,rsi
mov ax,STR_WARNING_API
call PoolStringWrite 
mov rsi,[r15 + APPDATA.lockedImportList]
add rdx,REGISTRY64.dynaImport
xor ebp,ebp
.checkImport:
cmp byte [rsi],0
je .doneCheckImport
cmp qword [rdx],0
jne .skipImport
call StringWrite
mov ax,0A0Dh
stosw
inc ebp
jmp .skippedImport
.skipImport:
lodsb
cmp al,0
jne .skipImport
.skippedImport:
cmp byte [rsi],0
jne .nextImport
inc rsi
.nextImport:
add rdx,8
jmp .checkImport
.doneCheckImport:
mov al,0
stosb
pop rdi
test ebp,ebp
jz .doneImport 
mov r9d,MB_ICONWARNING   ; R9  = Parm#4 = Message box icon type
lea r8,[ProgName]        ; R8  = Parm#3 = Pointer to caption
mov rdx,rdi              ; RDX = Parm#2 = Pointer to string
xor ecx,ecx              ; RCX = Parm#1 = Parent window handle or 0
call [MessageBoxA]
.doneImport:

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

xor r13d,r13d                      ; R13 = Exit Code, 0 means no errors
.exitResources:
mov r15,[Registry]
test r15,r15
jz .exit
mov r14,r15
add r15,REGISTRY64.appData

;---------- Delete created fonts ----------------------------------------------;

mov rsi,[r15 + APPDATA.lockedFontList]
test rsi,rsi
jz .doneDeleteFonts 
lea rdi,[r15 + APPDATA.hFont1]
.deleteFonts:
lodsw
test ax,ax
jz .doneDeleteFonts
mov rcx,[rdi]
add rdi,8
jrcxz .skipDelete
call [DeleteObject]
.skipDelete:
add rsi,14
@@:
lodsb
cmp al,0
jnz @b 
jmp .deleteFonts
.doneDeleteFonts:

;---------- Unload resource library -------------------------------------------; 

mov rcx,[r15 + APPDATA.hResources]  ; RCX = Library DATA.DLL handle
jrcxz .skipUnload                   ; Go skip unload if handle = null
call [FreeLibrary]                  ; Unload DATA.DLL
.skipUnload:

;---------- Release memory ----------------------------------------------------; 

mov r8d,MEM_RELEASE                ; R8  = Parm#3 = Memory free operation type
xor edx,edx                        ; RDX = Parm#2 = Size, 0 = by allocated
mov rcx,r14                        ; RCX = Parm#1 = Memory block base address  
call [VirtualFree]                 ; Release memory, allocated for registry

;---------- Exit --------------------------------------------------------------;

.exit:
mov ecx,r13d                       ; RCX = Parm#1 = exit code           
call [ExitProcess]

;---------- This entry points used if application start failed ----------------; 

.dialogueFailed:
mov al,MSG_DIALOGUE_FAILED     ; AL = String pool index for error name
jmp .errorProgram
.createFontFailed:
mov al,MSG_CREATE_FONT_FAILED
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

;---------- Show message box and go epplication termination -------------------;
; This procedure for application error, use message strings from exe file,
; can execute if resource DLL not loaded or load failes.

.errorProgram:
lea rsi,[MsgErrors]    ; RSI = Strings pool base, AL = String index 
mov ah,0
.errorEntry:
call IndexString       ; Return ESI = Selected string address 
mov r9d,MB_ICONERROR   ; R9  = Parm#4 = Attributes
lea r8,[ProgName]      ; R8  = Parm#3 = Pointer to title (caption) string
mov rdx,rsi            ; RDX = Parm#2 = Pointer to string: error name 
xor ecx,ecx            ; RCX = Parm#1 = Parent Window = NULL
call [MessageBox]  
mov r13d,1
jmp .exitResources

;---------- Show message box and go epplication termination -------------------;
; This procedure for incompatible platform detected but application integrity
; OK, use strings from resource DLL.

.errorPlatform:       ; Input AX = Error string ID.
mov rsi,[Registry]
test rsi,rsi
jz .initFailed
mov rsi,[rsi + REGISTRY64.appData.lockedStrings]
jmp .errorEntry 

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

;---------- Find string in the pool by index ----------------------------------;
;                                                                              ;
; INPUT:   RSI = Pointer to string pool                                        ;
;          AX  = String index in the strings pool                              ;
;                                                                              ;
; OUTPUT:  RSI = Updated pointer to string, selected by index                  ;  
;                                                                              ;
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

;---------- Find string in the pool by index and write this string ------------;
;                                                                              ;
; INPUT:   AX  = String index in the application resources strings pool        ;
;                                                                              ;
; OUTPUT:  RSI = Updated pointer to string, selected by index                  ;
;          RDI = Modified by copy                                              ;  
;                                                                              ;
;------------------------------------------------------------------------------;

PoolStringWrite:
mov rcx,[Registry]
jrcxz .exit
mov rsi,[rcx + REGISTRY64.appData.lockedStrings]
test rsi,rsi
jz .exit
call IndexString
call StringWrite
.exit:
ret

;---------- Print 64-bit Hex Number -------------------------------------------;
;                                                                              ;
; INPUT:  RAX = Number                                                         ;
;         RDI = Destination Pointer                                            ;
;                                                                              ;
; OUTPUT: RDI = Modify                                                         ;
;                                                                              ;
;------------------------------------------------------------------------------;

HexPrint64:
push rax
ror rax,32
call HexPrint32
pop rax
; no RET, continue at next subroutine

;---------- Print 32-bit Hex Number -------------------------------------------;
;                                                                              ;
; INPUT:  EAX = Number                                                         ;
;         RDI = Destination Pointer                                            ;
;                                                                              ;
; OUTPUT: RDI = Modify                                                         ;
;                                                                              ;
;------------------------------------------------------------------------------;

HexPrint32:
push rax
ror eax,16
call HexPrint16
pop rax
; no RET, continue at next subroutine

;---------- Print 16-bit Hex Number -------------------------------------------;
;                                                                              ;
; INPUT:  AX  = Number                                                         ;
;         RDI = Destination Pointer                                            ;
;                                                                              ;
; OUTPUT: RDI = Modify                                                         ;
;                                                                              ;
;------------------------------------------------------------------------------;

HexPrint16:
push rax
xchg al,ah
call HexPrint8
pop rax
; no RET, continue at next subroutine

;---------- Print 8-bit Hex Number --------------------------------------------;
;                                                                              ;
; INPUT:  AL  = Number                                                         ;
;         RDI = Destination Pointer                                            ;
;                                                                              ;
; OUTPUT: RDI = Modify                                                         ;
;                                                                              ;
;------------------------------------------------------------------------------;

HexPrint8:
push rax
ror al,4
call HexPrint4
pop rax
; no RET, continue at next subroutine

;---------- Print 4-bit Hex Number --------------------------------------------;
;                                                                              ;
; INPUT:  AL  = Number (bits 0-3)                                              ;
;         RDI = Destination Pointer                                            ;
;                                                                              ;
; OUTPUT: RDI = Modify                                                         ;
;                                                                              ;
;------------------------------------------------------------------------------;

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

;---------- Print double precision value --------------------------------------;
; x87 FPU used, required x87 presence validation by CPUID before call this.    ;
;                                                                              ;
; INPUT:   RAX = Double precision number                                       ;
;          BL  = Number of digits in the INTEGER part,                         ;
;                used for add left non-signed zeroes.                          ;
;                BL=0 means not print left unsigned zeroes.                    ;
;          BH  = Number of digits in the FLOAT part,                           ;
;                used as precision control.                                    ;
;          RDI = Destination text buffer pointer                               ;
;                                                                              ;
; OUTPUT:  RDI = Modified by text string write                                 ;
;                                                                              ;
;------------------------------------------------------------------------------;

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

;---------- Print memory block size as Integer.Float --------------------------;
;                                                                              ;
; INPUT:   RAX = Number value, units = Bytes                                   ;
;          BL  = Force units (override as smallest only)                       ;
;                FF = No force units, auto select                              ;
;                0 = Bytes, 1 = KB, 2 = MB, 3 = GB, 4 = TB                     ;
;          RDI = Destination Pointer (flat)                                    ;
;                                                                              ;
; OUTPUT:  RDI = New Destination Pointer (flat)                                ;
;                modified because string write                                 ;
;                                                                              ;
;------------------------------------------------------------------------------;

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
lea eax,[ecx + STR_UNITS_BYTES]
call PoolStringWrite  
jmp .exit
.hexMode:
call HexPrint64
mov al,'h'
stosb 
.exit:
pop rsi rdx rcx rbx rax
ret

;---------- Execute binder in the binders pool by index -----------------------;
;                                                                              ;
; INPUT:   RBX = Current window handle for get dialogue items                  ;
;          AX  = Binder index in the binders pool                              ;
;                                                                              ;
; OUTPUT:  None                                                                ;  
;                                                                              ;
;------------------------------------------------------------------------------;

Binder:
push rbx rsi rdi rbp r13 r14 r15 
cld
mov r15,[Registry]      
mov r14,r15                          ; R14 = Pointer to global registry
add r15,REGISTRY64.appData           ; R15 = Pointer to registry.appData
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
test al,00111111b
jz .stop
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
jmp .found              ; cycle for next instruction of binder
.stop:
pop r15 r14 r13 rbp rdi rsi rbx
ret

;---------- Script handler: bind indexed string from pool to GUI object -------;  

BindSetString:      ; EAX = String ID, RDX = Parm#2 = Resource ID for GUI item
mov rsi,[r15 + APPDATA.lockedStrings]
call IndexString    ; Return RSI = Pointer to selected string
BindEntry:
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rcx,rbx           ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]     ; Return handle of GUI item
test rax,rax
jz BindExit           ; Go skip if error, item not found
mov r9,rsi            ; R9  = Parm#4 = lParam = Pointer to string
xor r8d,r8d           ; R8  = Parm#3 = wParam = Not used
mov edx,WM_SETTEXT    ; RDX = Parm#2 = Msg
xchg rcx,rax          ; RCX = Parm#1 = hWnd 
call [SendMessage]    ; Set string for GUI item
BindExit:
mov rsp,rbp
ret

;---------- Script handler: bind string from temporary buffer to GUI object ---;

BindSetInfo:          ; EAX = String offset, RDX = Resource ID for GUI item
mov rsi,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
add rsi,rax
jmp BindEntry

;---------- Script handler: bind string referenced by pointer to GUI object ---;

BindSetPtr:           ; EAX = String pointer, RDX = Resource ID for GUI item
mov rsi,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
add rsi,rax
mov rsi,[rsi]
test rsi,rsi
jnz BindEntry
ret

;---------- Script handler: enable or disable GUI object by binary flag -------;

BindSetBool:       ; EAX = Variable offset:bit, RDX = Resource ID for GUI item
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov ecx,eax
shr eax,3
and ecx,0111b
mov r8,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
movzx eax,byte [r8 + rax]
bt eax,ecx
setc al
xchg esi,eax
mov rcx,rbx         ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]   ; Return handle of GUI item
test rax,rax
jz BindExit         ; Go skip if error, item not found
mov edx,esi
xchg rcx,rax 
call [EnableWindow]
jmp BindExit

;---------- Script handler: set GUI object enable and checked states ----------l
; This handler use 2-bit field: enable flag and state flag.

BindSetSwitch:      ; EAX = Variable offset:bit, RDX = Resource ID for GUI item
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov ecx,eax
shr eax,3
and ecx,0111b
mov r8,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
movzx eax,byte [r8 + rax]
xor esi,esi
bt eax,ecx
rcl esi,1
inc ecx
bt eax,ecx
rcl esi,1           ; Bits ESI.0 = value , ESI.1 = enable
mov rcx,rbx         ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]   ; Return handle of GUI item
test rax,rax
jz BindExit         ; Go skip if error, item not found
xchg rdi,rax
mov edx,esi
shr edx,1           ; RDX = Parm#2 = Data, 0 = disable , 1 = enable
mov rcx,rdi         ; RCX = Parm#1 = hWnd 
call [EnableWindow]
xor r9d,r9d         ; R9  = Parm#4 = lParam = not used
and esi,1
mov r8d,esi         ; R8  = Parm#3 = wParam: BST_CHECKED = 1, BST_UNCHECKED = 0
mov edx,BM_SETCHECK ; RDX = Parm#2 = Msg
mov rcx,rdi         ; RCX = Parm#1 = hWnd 
call [SendMessage]  ; Set string for GUI item
jmp BindExit 

;---------- Script handlers: set decimal and hex number edit field ------------;

BindSetDec32:
mov cl,0
jmp BindSetNumberEntry
BindSetHex32:
mov cl,1
jmp BindSetNumberEntry
BindSetHex64:
mov cl,2
BindSetNumberEntry:
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rsi,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
mov rdi,[r14 + REGISTRY64.allocatorTempBuffer.objectStart]
add rsi,rax
push rbx rdi
dec cl
jz .printHex32
dec cl
jz .printHex64
lodsd
mov bl,0
call DecimalPrint32
jmp .printDone
.printHex32:
lodsd
call HexPrint32
jmp .printDone
.printHex64:
lodsq
call HexPrint64
.printDone:
mov al,0
stosb
pop rdi rbx
mov rcx,rbx         ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]   ; Return handle of GUI item
test rax,rax
jz BindExit         ; Go skip if error, item not found
mov r9,rdi
xor r8d,r8d
mov edx,WM_SETTEXT
xchg rcx,rax
call [SendMessage]
jmp BindExit

;---------- Script handler: operations with combo box -------------------------; 

BindSetCombo:     ; EAX = Combo offset, RDX = Parm#2 = Resource ID for GUI item
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rsi,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
add rsi,rax           ; RSI = Pointer to combo description list
mov rcx,rbx           ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]     ; Return handle of GUI item
test rax,rax          ; RAX = Handle of combo box
jz .exit              ; Go skip if error, item not found
xchg rdi,rax          ; RDI = Handle of combo box
mov r13d,0FFFF0000h   ; R13D = Store:Counter for selected item
.scan:
lodsb                       ; AL = Tag from combo description list 
movzx rax,al
call [ProcCombo + rax * 8]  ; Call handler = F(tag)
inc r13d
jnc .scan                   ; Continue if end tag yet not found
shr r13d,16
cmp r13w,0FFFFh
je .exit
xor r9d,r9d           ; R9  = Parm#4 = lParam = Not used 
mov r8d,r13d          ; R8  =  Parm#3 = wParam = Selected item, 0-based 
mov edx,CB_SETCURSEL  ; RDX = Parm#2 = Msg
mov rcx,rdi           ; RCX = Parm#1 = hWnd 
call [SendMessage]    ; Set string for GUI item
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
shl r13d,16
clc
ret
BindComboAdd:                     ; Add item to list
call HelperBindCombo
clc
ret
BindComboInactive:                ; Add item to list as inactive (gray)
clc
ret

;---------- Script handler: bind font from registry to GUI object -------------;

BindSetFont:          ; EAX = Font number, RDX = Resource ID for GUI item
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
lea rsi,[r15 + APPDATA.hFont1]
mov rsi,[rsi + rax * 8]
mov rcx,rbx            ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]      ; Return handle of GUI item
test rax,rax
jz BindExit            ; Go skip if error, item not found
mov r9d,1              ; R9  = Parm#4 = lParam = redraw flag
mov r8,rsi             ; R8  = Parm#3 = wParam = handle
mov edx,WM_SETFONT     ; RDX = Parm#2 = Msg
xchg rcx,rax           ; RCX = Parm#1 = hWnd
call [SendMessage]
jmp BindExit

;--- Script handler: get state of GUI object and write to bit at buffer -------;

BindGetSwitch:        ; EAX = Variable offs:bit, RDX = Resource ID for GUI item
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
xchg esi,eax          ; ESI = Destination variable address and bit position
mov rcx,rbx           ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]     ; Return handle of GUI item
test rax,rax
jz BindExit           ; Go skip if error, item not found
xchg rcx,rax          ; RCX = Parm#1 = Handle                     
mov edx,BM_GETCHECK   ; RDX = Parm#2 = Message
xor r8d,r8d           ; R8  = Parm#3 = Not used = 0
xor r9d,r9d           ; R9  = Parm#4 = Not used = 0
call [SendMessage]    ; Return RAX = widget state
cmp rax,1             ; Compare with BST_CHECKED = 1, set ZF = 1 if this
pushf
mov ecx,esi
shr esi,3
and ecx,0111b
add rsi,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
popf
je .setBit
mov al,11111110b
rol al,cl
and [rsi],al 
jmp .done
.setBit:
mov al,00000001b
rol al,cl
or [rsi],al 
.done:
jmp BindExit

;---------- Script handlers: get decimal and hex number edit field ------------;

BindGetDec32:
xor r13d,r13d
jmp BindGetNumberEntry
BindGetHex32:
mov r13b,1
jmp BindGetNumberEntry
BindGetHex64:
mov r13b,2
BindGetNumberEntry:
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rsi,[r14 + REGISTRY64.allocatorTempBuffer.objectStart]
mov rdi,[r14 + REGISTRY64.allocatorBindBuffer.objectStart]
add rdi,rax
mov rcx,rbx        ; RCX = Parm#1 = Parent window handle, RDX = Parm#2 = ID  
call [GetDlgItem]  ; Return handle of GUI item
test rax,rax
jz .exit           ; Go skip if error, item not found
mov r9,rsi
mov r8d,17
mov edx,WM_GETTEXT
xchg rcx,rax
call [SendMessage]
test rax,rax
jz .exit           ; Go skip if error, can't read string
test r13b,r13b
jz .parseDec
xor ecx,ecx
.parseHex:
xor eax,eax
lodsb
cmp al,'0'
jb .parseDone
cmp al,'9'
ja .tryHex 
and al,0Fh 
jmp .tryDone
.tryHex:
and al,0DFh
sub al,'A' - 10
cmp al,10
jb .parseDone 
.tryDone:
rol rcx,4
or rcx,rax
jmp .parseHex
.parseDec:
xor eax,eax
lodsb
sub al,'0'
jb .parseDone
cmp al,9
ja .parseDone
imul ecx,ecx,10
add ecx,eax
.parseDone:
xchg rax,rcx
cmp r13b,2
jb .store32
stosq
jmp .exit
.store32:
stosd
.exit:
jmp BindExit

;---------- Helper for add string to combo box list ---------------------------;
;                                                                              ;
; INPUT:   RSI = Pointer to binder script                                      ;
;          RDI = Parent window handle                                          ;
;          R15 = Pointer to application registry for global variables access   ; 
;                                                                              ;
; OUTPUT:  None                                                                ;
;                                                                              ;
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

;---------- Include subroutines from modules ----------------------------------;

include 'ncrb64\dialogs\connect_code.inc'
include 'ncrb64\system_info\connect_code.inc'
include 'ncrb64\threads_manager\connect_code.inc'
include 'ncrb64\memory_bandwidth_temporal\connect_code.inc'
include 'ncrb64\memory_bandwidth_non_temporal\connect_code.inc'
include 'ncrb64\memory_bandwidth_partial\connect_code.inc'
include 'ncrb64\memory_latency\connect_code.inc'
include 'ncrb64\math_bandwidth\connect_code.inc'
include 'ncrb64\math_latency\connect_code.inc'

;------------------------------------------------------------------------------;
;                              Data section.                                   ;        
;------------------------------------------------------------------------------;

section '.data' data readable writeable

;---------- Common data for application ---------------------------------------;

Registry      DQ  0                        ; Must be 0 for conditional release
AppCtrl       INITCOMMONCONTROLSEX  8, 0   ; Structure for initialization

;---------- Pointers to procedures of GUI bind scripts interpreter ------------;

ProcBinders   DQ  BindSetString
              DQ  BindSetInfo
              DQ  BindSetPtr
              DQ  BindSetBool
              DQ  BindSetSwitch
              DQ  BindSetDec32
              DQ  BindSetHex32
              DQ  BindSetHex64              
              DQ  BindSetCombo
              DQ  BindSetFont
              DQ  BindGetSwitch
              DQ  BindGetDec32
              DQ  BindGetHex32
              DQ  BindGetHex64              

ProcCombo     DQ  BindComboStopOn     ; End of list, combo box enabled
              DQ  BindComboStopOff    ; End of list, combo box disabled (gray) 
              DQ  BindComboCurrent    ; Add item to list as current selected
              DQ  BindComboAdd        ; Add item to list
              DQ  BindComboInactive   ; Add item to list as inactive (gray)

;---------- List for load raw resources ---------------------------------------;

RawList       DW  IDS_STRINGS_POOL
              DW  IDS_BINDERS_POOL
              DW  IDS_CPU_COMMON_POOL
              DW  IDS_CPU_AVX512_POOL
              DW  IDS_OS_CONTEXT_POOL
              DW  IDS_CPU_METHOD_POOL
              DW  IDS_ACPI_DATA_POOL
              DW  IDS_IMPORT_POOL
              DW  IDS_FONTS_POOL
              DW  0

;---------- Libraries for dynamical import ------------------------------------;

NameKernel32  DB  'KERNEL32.DLL' , 0      ; Must be sequental list of WinAPI
NameAdvapi32  DB  'ADVAPI32.DLL' , 0 , 0  ; Two zeroes means end of list
NameResDll    DB  'DATA.DLL'     , 0

;---------- Text strings about application ------------------------------------;

ProgName      DB  PROGRAM_NAME   , 0
AboutCap      DB  ABOUT_CAP      , 0
AboutText     DB  ABOUT_TEXT_1   , 0Dh,0Ah
              DB  ABOUT_TEXT_2B  , 0Dh,0Ah
              DB  ABOUT_TEXT_3   , 0Dh,0Ah, 0

;---------- Errors messages strings -------------------------------------------;

MsgErrors     DB  'Memory allocation error.'                 , 0      
              DB  'Initialization failed.'                   , 0
              DB  'Load resource library failed.'            , 0
              DB  'Handle of this module is NULL.'           , 0
              DB  'Load application icon failed.'            , 0
              DB  'Load icon data from resource DLL failed.' , 0
              DB  'Load raw data from resource DLL failed.'  , 0
              DB  'Create font failed.'                      , 0
              DB  'Create dialogue window failed.'           , 0

;---------- Include constants and pre-defined variables from modules ----------;

include 'ncrb64\dialogs\connect_data.inc'
include 'ncrb64\system_info\connect_data.inc'
include 'ncrb64\threads_manager\connect_data.inc'
include 'ncrb64\memory_bandwidth_temporal\connect_data.inc'
include 'ncrb64\memory_bandwidth_non_temporal\connect_data.inc'
include 'ncrb64\memory_bandwidth_partial\connect_data.inc'
include 'ncrb64\memory_latency\connect_data.inc'
include 'ncrb64\math_bandwidth\connect_data.inc'
include 'ncrb64\math_latency\connect_data.inc'

;------------------------------------------------------------------------------;
;                              Import section.                                 ;        
;------------------------------------------------------------------------------;

section '.idata' import data readable writeable
library kernel32 , 'kernel32.dll' , \
        advapi32 , 'advapi32.dll' , \
        user32   , 'user32.dll'   , \
        comctl32 , 'comctl32.dll' , \
        gdi32    , 'gdi32.dll' 
include 'api\kernel32.inc'
include 'api\advapi32.inc'
include 'api\user32.inc'
include 'api\comctl32.inc'
include 'api\gdi32.inc'

;------------------------------------------------------------------------------;
;                            Resources section.                                ;        
;------------------------------------------------------------------------------;

section '.rsrc' resource data readable
directory RT_ICON       , icons     , \
          RT_GROUP_ICON , gicons    , \
          RT_MANIFEST   , manifests , \
          RT_VERSION    , version

;---------- Icons resource ----------------------------------------------------;

resource icons  , ID_EXE_ICON  , LANG_NEUTRAL , exeicon
resource gicons , ID_EXE_ICONS , LANG_NEUTRAL , exegicon
icon exegicon, exeicon, 'images\fasm64.ico'

;---------- Manifest resource -------------------------------------------------;

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

;---------- Version resource --------------------------------------------------;

resource     version, 1, LANG_NEUTRAL, version_info
versioninfo  version_info, \ 
             VOS__WINDOWS32, VFT_DLL, VFT2_UNKNOWN, LANG_NEUTRAL, 0, \
'FileDescription' , RESOURCE_DESCRIPTION ,\
'FileVersion'     , RESOURCE_VERSION     ,\
'CompanyName'     , RESOURCE_COMPANY     ,\
'LegalCopyright'  , RESOURCE_COPYRIGHT

