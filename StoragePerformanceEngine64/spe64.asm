;==============================================================================;
;                                                                              ;
;  Template for native agent console application build. Windows x64 version.   ;
;  This file is main module: translation object, interconnecting all modules.  ;
;                                                                              ;
;  Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 ).             ;
;  http://flatassembler.net/                                                   ;
;                                                                              ;
;  Edit by FASM Editor 2.0.                                                    ;
;  Use this editor for correct source file tabulations and format. (!)         ;
;  https://fasmworld.ru/instrumenty/fasm-editor-2-0/                           ;
;                                                                              ;
;==============================================================================;

format PE64 console
entry start

;------------------------------ Definitions -----------------------------------;
include 'win64a.inc'                   ; FASM definitions for Win64
include 'console\connect_equ.inc'      ; Equations for color console support
include 'dump\connect_equ.inc'         ; Equations for CPU regs. and mem. dump
include 'system\connect_equ.inc'       ; Equations for OS and hardware support
include 'target\connect_equ.inc'       ; Equations for disk I/O
include 'targetlib\BaseEquations.inc'  ; Equations for JNI-style library
TEMP_BUFFER_ALIGNMENT  EQU  4096       ; Temporary buffer alignment = page
TEMP_BUFFER_SIZE       EQU  32768      ; Temporary buffer size = 32 KB 
IPB_CONST              EQU  4096       ; Input Parameters Block size, bytes
OPB_CONST              EQU  4096       ; Output Parameters Block size, bytes

;------------------------------ Code section ----------------------------------;
section '.code' code readable executable
start:
; Start application
sub rsp,8*5                 ; Make parameters shadow and stack alignment
lea rbx,[Alias_Base]        ; RBX = Base for variables addressing
xor eax,eax
mov ALIAS_REPORTNAME,rax    ; Clear report file name pointer, before first out 
mov ALIAS_REPORTHANDLE,rax  ; Clear report file name handle, before first out
; Initializing console input handle
mov ecx,STD_INPUT_HANDLE    ; Parm#1 = RCX = Handle ID = input device handle       
call [GetStdHandle]         ; Initializing input device handle ( keyboard )
test rax,rax
jz ExitProgram              ; Silent exit if get input handle failed
mov ALIAS_STDIN,rax         ; Store input handle
; Initializing console output handle
mov ecx,STD_OUTPUT_HANDLE   ; Parm#1 = RCX = Handle ID = output device handle    
call [GetStdHandle]         ; Initializing output device handle ( display )
test rax,rax
jz ExitProgram              ; Silent exit if get output handle failed
mov ALIAS_STDOUT,rax        ; Store output handle
; Detect command line
call [GetCommandLineA]      ; Get command line
test rax,rax
jz ExitProgram              ; Silent exit if get command line failed
mov ALIAS_COMMANDLINE,rax   ; Store pointer to command line
; Title string
lea rcx,[TitleString]
call [SetConsoleTitle]      ; Title string for console output window up
; Get console screen buffer information
mov rcx,ALIAS_STDOUT        ; Parm#1 = RCX = Output handle
lea rdx,[ScreenInfo]        ; Parm#2 = RDX = Pointer to destination buffer 
call [GetConsoleScreenBufferInfo]
test rax,rax                ; Silent exit if get information failed, 
jz ExitProgram              ; Can replace this termination to non-color branch
; Initializing string-type options,
; for source and destination files by file I/O benchmark scenario
lea rsi,[DefaultSrc]        ; RSI = Pointer to default path of source file
lea rdi,[BufferSrc]         ; RDI = Pointer to buffer for source file path
call StringWrite            ; Copy default path, later can override by scenario
mov al,0
stosb                       ; Terminate path string by 0
lea rsi,[DefaultDst]        ; RSI = Pointer to default path of destination file
lea rdi,[BufferDst]         ; RDI = Pointer to buffer for destination file path
call StringWrite            ; Copy default path, later can override by scenario
mov al,0
stosb                       ; Terminate path string by 0
; Load scenario file: INPUT.TXT
lea rcx,[InputName]              ; Parm#1 = RCX = Pointer to scenario file name
lea rdx,ALIAS_SCENARIOHANDLE     ; Parm#2 = RDX = Pointer to sc. file handle 
lea r8,ALIAS_SCENARIOBASE        ; Parm#3 = R8  = Pointer to pointer to buffer
lea r9,ALIAS_SCENARIOSIZE        ; Parm#4 = R9  = Pointer to pointer to size
lea rax,[TEMP_BUFFER]            ; RAX = Buffer base, 64-bit address required
mov [r8],rax                     ; Write buffer base address
mov qword [r9],TEMP_BUFFER_SIZE  ; Write buffer size limit
call ReadScenario
; Check loaded scenario file size, detect error if loaded size = buffer size
cmp qword ALIAS_SCENARIOSIZE, TEMP_BUFFER_SIZE
lea rcx,[MsgInputSize]       ; RCX = Base address for error message
jae ErrorProgramSingleParm   ; Go error if size limit 
; Interpreting input ( scenario ) file, update options values variables
lea rcx,[TEMP_BUFFER]        ; RCX = Pointer to buffer with scenario file
mov rdx,ALIAS_SCENARIOSIZE
add rdx,rcx                  ; RDX = Buffer limit, addr. of first not valid byte
lea r8,[OpDesc]              ; R8 = Pointer to options descriptors list
lea r9,ALIAS_ERROR_STATUS    ; R9 = Pointer to error status info
call ParseScenario
; Check option " display = on|off " , clear output handle if " off "
xor edx,edx
cmp [OptionDisplay],dl       ; DL = 0
jne @f
mov ALIAS_STDOUT,rdx         ; RDX = 0 
@@:
; Check option " waitkey = on|off " , clear input handle if " off " 
cmp [OptionWaitkey],dl       ; DL = 0
jne @f
mov ALIAS_STDIN,rdx          ; RDX = 0 
@@:
; Check parsing status, this must be after options interpreting
mov rcx,ALIAS_ERROR_P1       ; RCX = Pointer to first error description string
mov rdx,ALIAS_ERROR_P2       ; RDX = Pointer to second error description string
test rax,rax
jz ErrorProgramDualParm      ; Go if input scenario file parsing error
; Start messages, only after loading options, possible " display = off "
lea rcx,[StartMsg]           ; Parm#1 = RCX = Pointer to string for output         
mov rdx,ALIAS_REPORTHANDLE   ; Parm#2 = RDX = Report file handle
mov r8,ALIAS_REPORTNAME      ; Parm#3 = R8  = Report file name
call ConsoleWrite            ; Output first message, output = display + file
test rax,rax
jz ExitProgram               ; Silent exit if console write failed
; Initializing save output ( report ) file mechanism: OUTPUT.TXT 
cmp [OptionReport],0
je @f                        ; Go skip create report if option " report = off "
lea rcx,[OutputName]         ; Parm#1 = RCX = Pointer to report file name 
mov ALIAS_REPORTNAME,rcx
lea rdx,ALIAS_REPORTHANDLE   ; Parm#2 = RDX = Pointer to report file handle 
call CreateReport
@@:
; Application name
lea rcx,[ProductID]          ; Parm#1 = RCX = Pointer to Product ID string
mov rdx,ALIAS_REPORTHANDLE   ; Parm#2 = RDX = Output handle
mov r8,ALIAS_REPORTNAME      ; Parm#3 = R8  = Pointer to report file name
call ConsoleWrite            ; Console output
test rax,rax
jz ExitProgram               ; Silent exit if console write failed
lea rcx,[CrLf2]              ; Parm#1 = RCX = Pointer to 0Dh, 0Ah ( CR, LF )
mov rdx,ALIAS_REPORTHANDLE   ; Parm#2 = RDX = Output handle
mov r8,ALIAS_REPORTNAME      ; Parm#3 = R8  = Pointer to report file name
call ConsoleWrite            ; Console output
test rax,rax
jz ExitProgram               ; Silent exit if console write failed
; Show list with options settings
lea rcx,[OpDesc]             ; Parm#1 = RCX = Pointers to options descriptors
lea rdx,[TEMP_BUFFER]        ; Parm#2 = RDX = Pointer to buffer for build text
call ShowScenario

;---------- Call target debug fragment and visual results ---------------------;
; Task = Measure mass storage performance,
; console output ( optional ) and write to report file ( optional )
; executed in this subroutine 
lea rcx,[TEMP_BUFFER]
lea rdx,ALIAS_ERROR_STATUS       ; RDX = Pointer to error status info
call StoragePerformanceContext
mov rcx,ALIAS_ERROR_P1       ; RCX = Pointer to first error description string
mov rdx,ALIAS_ERROR_P2       ; RDX = Pointer to second error description string
mov r8,ALIAS_ERROR_C         ; R8  = WinAPI error code
test rax,rax
jz ErrorProgramTripleParm    ; Go if input scenario file parsing error

;---------- End of target debug fragment, continue console output -------------;
; This for "Press ENTER ..." not add to text report
xor eax,eax
mov ALIAS_REPORTNAME,rax        ; Clear report file name pointer 
mov ALIAS_REPORTHANDLE,rax      ; Clear report file name handle
; Restore original color
call GetColor                   ; Return EAX = Original ( OS ) console color
xchg ecx,eax
call SetColor                   ; Set color by input ECX
; Done message, write to console ( optional ) and report file ( optional )
lea rcx,[DoneMsgNoWait]         ; Parm#1 = RCX = Pointer to message
cmp [OptionWaitkey],0
je  @f
lea rcx,[DoneMsgWait]           ; Parm#1 = RCX = Pointer to message
@@:
mov rdx,ALIAS_REPORTHANDLE      ; Parm#2 = RDX = Output handle
mov r8,ALIAS_REPORTNAME         ; Parm#3 = R8  = Pointer to report file name
call ConsoleWrite 
; Wait key press
lea rcx,[TEMP_BUFFER]           ; Parm#1 = RCX = Pointer to buffer for char
call ConsoleRead                ; Console input
lea rcx,[CrLf2]                 ; Parm#1 = RCX = Pointer to 0Dh, 0Ah ( CR, LF )
mov rdx,ALIAS_REPORTHANDLE      ; Parm#2 = RDX = Output handle
mov r8,ALIAS_REPORTNAME         ; Parm#3 = R8  = Pointer to report file name
call ConsoleWrite               ; Console output

;---------- Exit application, this point used if no errors --------------------;
ExitProgram:               ; Common entry point for exit to OS
xor ecx,ecx                ; Parm#1 = RCX = Exit code = 0 (no errors)
call [ExitProcess]         ; No return from this function

;---------- Error handling and exit application -------------------------------;
ErrorProgramSingleParm:    ; Here valid Parm#1 = RCX = Pointer to string
xor edx,edx                ; Parm#2 = RDX = Pointer to second string, not used 
ErrorProgramDualParm:      ; Here used 2 params: RCX, RDX
xor r8,r8                  ; Parm#3 = R8  = WinAPI error code, not used 
ErrorProgramTripleParm:    ; Here used all 3 params: RCX, RDX, R8
lea r9,[TEMP_BUFFER]       ; Parm#4 = R9 = Pointer to work buffer
call ShowError             ; Show error message
mov ecx,1                  ; Parm#1 = RCX = Exit code = 1 (error detected)
call [ExitProcess]         ; No return from this function

;------------------------------ Libraries -------------------------------------;
include 'console\connect_code.inc'   ; Connect library subroutines
include 'dump\connect_code.inc'
include 'system\connect_code.inc'
include 'target\connect_code.inc'    ; Connect target functionality subroutines
; File I/O library, legacy JNI/IPB/OPB style
include 'targetlib\BaseRoutines.inc'       ; Helpers library
include 'targetlib\GetRandomData.inc'      ; Get random data array, use RDRAND
include 'targetlib\MeasureReadFile.inc'    ; Read file with measur. iterations 
include 'targetlib\MeasureWriteFile.inc'   ; Write file with measur. iterations
include 'targetlib\MeasureCopyFile.inc'    ; Copy file with measur. iterations
include 'targetlib\MeasureDeleteFile.inc'  ; Delete file
include 'targetlib\PrecisionLinear.inc'    ; W/C/R sequence with measur. iter.
include 'targetlib\PrecisionMixed.inc'     ; W/C/R ... random, mixed

;------------------------------- Data section ---------------------------------;
section '.data' data readable writeable
; Constants, located before variables for EXE file space minimization
align 8
include 'console\connect_const.inc'  ; Library constants
include 'dump\connect_const.inc'
include 'system\connect_const.inc'
include 'target\connect_const.inc'   ; Target functionality constants 
; Variables, located at top of file for EXE file space minimization
align 8
include 'console\connect_var.inc'    ; Library variables
include 'dump\connect_var.inc'
include 'system\connect_var.inc'
include 'target\connect_var.inc'     ; Target functionality variables
; Multifunctional buffer
align  TEMP_BUFFER_ALIGNMENT 
TEMP_BUFFER  DB  TEMP_BUFFER_SIZE DUP (?)

;------------------------------ Import section --------------------------------;
section '.idata' import data readable writeable
library kernel32, 'KERNEL32.DLL'
include 'api\kernel32.inc'             ; Win API, OS standard kernel functions
