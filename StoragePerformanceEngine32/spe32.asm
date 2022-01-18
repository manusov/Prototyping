;==============================================================================;
;                                                                              ;
;  Template for native agent console application build. Windows ia32 version.  ;
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

format PE console
entry start

;------------------------------ Definitions -----------------------------------;
include 'win32ax.inc'                  ; FASM definitions for Win32
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
lea ebx,[Alias_Base]        ; EBX = Base for variables addressing
xor eax,eax
mov ALIAS_REPORTNAME,eax    ; Clear report file name pointer, before first out 
mov ALIAS_REPORTHANDLE,eax  ; Clear report file name handle, before first out
; Initializing console input handle
push STD_INPUT_HANDLE       ; Parm#1 = Handle ID = input device handle       
call [GetStdHandle]         ; Initializing input device handle ( keyboard )
test eax,eax
jz ExitProgram              ; Silent exit if get input handle failed
mov ALIAS_STDIN,eax         ; Store input handle
; Initializing console output handle
push STD_OUTPUT_HANDLE      ; Parm#1 = Handle ID = output device handle    
call [GetStdHandle]         ; Initializing output device handle ( display )
test eax,eax
jz ExitProgram              ; Silent exit if get output handle failed
mov ALIAS_STDOUT,eax        ; Store output handle
; Detect command line
call [GetCommandLineA]      ; Get command line
test eax,eax
jz ExitProgram              ; Silent exit if get command line failed
mov ALIAS_COMMANDLINE,eax   ; Store pointer to command line
; Title string
push TitleString
call [SetConsoleTitle]      ; Title string for console output window up
; Get console screen buffer information
push ScreenInfo             ; Parm#2 = Pointer to destination buffer
push dword ALIAS_STDOUT     ; Parm#1 = Output handle
call [GetConsoleScreenBufferInfo]
test eax,eax                ; Silent exit if get information failed, 
jz ExitProgram              ; Can replace this termination to non-color branch
; Initializing string-type options,
; for source and destination files by file I/O benchmark scenario
lea esi,[DefaultSrc]        ; ESI = Pointer to default path of source file
lea edi,[BufferSrc]         ; EDI = Pointer to buffer for source file path
call StringWrite            ; Copy default path, later can override by scenario
mov al,0
stosb                       ; Terminate path string by 0
lea esi,[DefaultDst]        ; ESI = Pointer to default path of destination file
lea edi,[BufferDst]         ; EDI = Pointer to buffer for destination file path
call StringWrite            ; Copy default path, later can override by scenario
mov al,0
stosb                       ; Terminate path string by 0
; Load scenario file: INPUT.TXT
lea ecx,[InputName]               ; Parm#1 = ECX = Pointer to scenario file name
lea edx,ALIAS_SCENARIOHANDLE      ; Parm#2 = EDX = Pointer to sc. file handle
lea esi,ALIAS_SCENARIOBASE        ; Parm#3 = ESI = Pointer to pointer to buffer
lea edi,ALIAS_SCENARIOSIZE        ; Parm#4 = EDI = Pointer to pointer to size
mov dword [esi],TEMP_BUFFER       ; Write buffer base address
mov dword [edi],TEMP_BUFFER_SIZE  ; Write buffer size limit
call ReadScenario
; Check loaded scenario file size, detect error if loaded size = buffer size
cmp dword ALIAS_SCENARIOSIZE, TEMP_BUFFER_SIZE
lea ecx,[MsgInputSize]       ; ECX = Base address for error message
jae ErrorProgramSingleParm   ; Go error if size limit 
; Interpreting input ( scenario ) file, update options values variables
lea ecx,[TEMP_BUFFER]       ; ECX = Pointer to buffer with scenario file
mov edx,ALIAS_SCENARIOSIZE
add edx,ecx                 ; EDX = Buffer limit, addr. of first not valid
lea esi,[OpDesc]            ; ESI = Pointer to options descriptors list
lea edi,ALIAS_ERROR_STATUS  ; EDI = Pointer to error status info
call ParseScenario
; Check option " display = on|off " , clear output handle if " off "
xor edx,edx
cmp [OptionDisplay],dl       ; DL = 0
jne @f
mov ALIAS_STDOUT,edx         ; EDX = 0 
@@:
; Check option " waitkey = on|off " , clear input handle if " off " 
cmp [OptionWaitkey],dl       ; DL = 0
jne @f
mov ALIAS_STDIN,edx          ; EDX = 0 
@@:
; Check parsing status, this must be after options interpreting
mov ecx,ALIAS_ERROR_P1       ; ECX = Pointer to first error description string
mov edx,ALIAS_ERROR_P2       ; EDX = Pointer to second error description string
test eax,eax
jz ErrorProgramDualParm      ; Go if input scenario file parsing error
; Start messages, only after loading options, possible " display = off "
lea ecx,[StartMsg]           ; ECX = Pointer to string for output
mov edx,ALIAS_REPORTHANDLE   ; EDX = Report file handle
mov esi,ALIAS_REPORTNAME     ; ESI = Report file name
call ConsoleWrite            ; Output first message, output = display + file
test eax,eax
jz ExitProgram               ; Silent exit if console write failed
; Initializing save output ( report ) file mechanism: OUTPUT.TXT 
cmp [OptionReport],0
je @f                        ; Go skip create report if option " report = off "
lea ecx,[OutputName]         ; ECX = Pointer to report file name
lea edx,ALIAS_REPORTHANDLE   ; EDX = Pointer to report file handle
mov ALIAS_REPORTNAME,ecx
call CreateReport
@@:
; Application name
lea ecx,[ProductID]          ; ECX = Pointer to Product ID string
mov edx,ALIAS_REPORTHANDLE   ; EDX = Output handle
mov esi,ALIAS_REPORTNAME     ; ESI = Pointer to report file name
call ConsoleWrite            ; Console output
test eax,eax
jz ExitProgram               ; Silent exit if console write failed
lea ecx,[CrLf2]              ; ECX = Pointer to 0Dh, 0Ah ( CR, LF )
mov edx,ALIAS_REPORTHANDLE   ; EDX = Output handle
mov esi,ALIAS_REPORTNAME     ; ESI = Pointer to report file name
call ConsoleWrite            ; Console output
test eax,eax
jz ExitProgram               ; Silent exit if console write failed
; Show list with options settings
lea ecx,[OpDesc]             ; ECX = Pointers to options descriptors
lea edx,[TEMP_BUFFER]        ; EDX = Pointer to buffer for build text
call ShowScenario

;---------- Call target debug fragment and visual results ---------------------;
; Task = Measure mass storage performance,
; console output ( optional ) and write to report file ( optional )
; executed in this subroutine 
lea ecx,[TEMP_BUFFER]        ; ECX = Pointer to transit buffer
lea edx,ALIAS_ERROR_STATUS   ; EDX = Pointer to error status info
call StoragePerformanceContext
test eax,eax
mov ecx,ALIAS_ERROR_P1       ; ECX = Pointer to first error description string
mov edx,ALIAS_ERROR_P2       ; EDX = Pointer to second error description string
mov eax,ALIAS_ERROR_C        ; EAX = WinAPI error code
jz ErrorProgramTripleParm    ; Go if input scenario file parsing error

;---------- End of target debug fragment, continue console output -------------;
; This for "Press ENTER ..." not add to text report
xor eax,eax
mov ALIAS_REPORTNAME,eax        ; Clear report file name pointer 
mov ALIAS_REPORTHANDLE,eax      ; Clear report file name handle
; Restore original color
call GetColor                   ; Return EAX = Original ( OS ) console color
xchg ecx,eax
call SetColor                   ; Set color by input ECX
; Done message, write to console ( optional ) and report file ( optional )
lea ecx,[DoneMsgNoWait]         ; ECX = Pointer to message 1
cmp [OptionWaitkey],0
je  @f
lea ecx,[DoneMsgWait]           ; ECX = Pointer to message 2
@@:
mov edx,ALIAS_REPORTHANDLE      ; EDX = Output handle
mov esi,ALIAS_REPORTNAME        ; ESI = Pointer to report file name
call ConsoleWrite 
; Wait key press
lea ecx,[TEMP_BUFFER]           ; Parm#1 = Pointer to buffer for char
call ConsoleRead                ; Console input
lea ecx,[CrLf2]                 ; ECX = Pointer to 0Dh, 0Ah ( CR, LF )
mov edx,ALIAS_REPORTHANDLE      ; EDX = Output handle
mov esi,ALIAS_REPORTNAME        ; ESI = Pointer to report file name
call ConsoleWrite               ; Console output

;---------- Exit application, this point used if no errors --------------------;
ExitProgram:               ; Common entry point for exit to OS
push 0                     ; Parm#1 = Exit code = 0 (no errors)
call [ExitProcess]         ; No return from this function

;---------- Error handling and exit application -------------------------------;
ErrorProgramSingleParm:    ; Here valid Parm#1 = ECX = Pointer to first string
xor edx,edx                ; Parm#2 = EDX = Pointer to second string, not used 
ErrorProgramDualParm:      ; Here used 2 params: ECX, EDX
xor eax,eax                ; Parm#3 = EAX  = WinAPI error code, not used 
ErrorProgramTripleParm:    ; Here used all 3 params: ECX, EDX, EAX
lea edi,[TEMP_BUFFER]      ; Parm#4 = Pointer to work buffer
call ShowError             ; Show error message
push 1                     ; Parm#1 = RCX = Exit code = 1 (error detected)
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
