;==============================================================================;
;                                                                              ;
;                        HARDWARE SHELL PROJECT.                               ;
;                                                                              ;
;                  Template for console debug. Win64 edition.                  ;
;                                                                              ;
;                    Customized for kernel mode driver test.                   ;
;                                                                              ;
;         Translation by Flat Assembler version 1.73.30 (Feb 21, 2022).        ;
;                         http://flatassembler.net/                            ;
;                                                                              ;
;       Edit by FASM Editor 2.0, use this editor for correct tabulations.      ;
;              https://fasmworld.ru/instrumenty/fasm-editor-2-0/               ;
;                                                                              ;
;              Special thanks to @L.CHEMIST ( Andrey A. Meshkov )              ;
; for Kernel Mode Driver ( KMD ) and Service Control Program ( SCP ) examples  ;
;                         http://maalchemist.narod.ru                          ;
;                                                                              ;
;==============================================================================;

;------------------------------------------------------------------------------;
;                      Source definitions for template.                        ;
;------------------------------------------------------------------------------;
; FASM definitions for Win64
include 'win64a.inc'
; Required 48 Kilobytes miscellaneous buffer
TEMP_BUFFER_SIZE            EQU  48 * 1024
; Definitions for color console support, color masks
CLEAR_FOREGROUND            EQU  0FFFFFFF0h
CLEAR_BACKGROUND            EQU  0FFFFFF0Fh
SELECT_FOREGROUND           EQU  00000000Fh
SELECT_BACKGROUND           EQU  0000000F0h
; Color values
FOREGROUND_BLUE             EQU  01h
FOREGROUND_GREEN            EQU  02h
FOREGROUND_RED              EQU  04h
FOREGROUND_INTENSITY        EQU  08h
BACKGROUND_BLUE             EQU  010h
BACKGROUND_GREEN            EQU  020h
BACKGROUND_RED              EQU  040h
BACKGROUND_INTENSITY        EQU  080h
COMMON_LVB_LEADING_BYTE     EQU  0100h
COMMON_LVB_TRAILING_BYTE    EQU  0200h
COMMON_LVB_GRID_HORIZONTAL  EQU  0400h
COMMON_LVB_GRID_LVERTICAL   EQU  0800h
COMMON_LVB_GRID_RVERTICAL   EQU  01000h
COMMON_LVB_REVERSE_VIDEO    EQU  04000h
COMMON_LVB_UNDERSCORE       EQU  08000h
; Console data structures definition, char coordinates
struct COORD
x dw  ?
y dw  ?
ends
; Rectangle corners coordinates
struct SMALL_RECT
Left   dw  ?
Top    dw  ?
Right  dw  ?
Bottom dw  ?
ends
; Console screen buffer information
struct CONSOLE_SCREEN_BUFFER_INFO
dwSize               COORD       ; buffer size at chars rows and columns
dwCursorPosition     COORD       ; coordinates (position) of cursor at buffer
wAttributes          dd  ?       ; attributes of chars
srWindow             SMALL_RECT  ; coordinates up-left and down-right corner of buffers
dwMaximumWindowSize  COORD       ; maximum sizes of console window
ends
; Definitions for configuration file support, options descriptors, types of descriptors 
XEND    EQU  0
XKEY    EQU  1 
XDEC    EQU  2
XHEX    EQU  3
XSIZE   EQU  4
XSTR    EQU  5
XLAST   EQU  5
; Offsets of descriptors fields
X0      EQU  0
X1      EQU  1
X2      EQU  5
X3      EQU  9
X4      EQU  13
; Addends for addressing descriptors sequence
XBIG    EQU  17
XSMALL  EQU  13
XDELTA  EQU  XBIG - XSMALL
; ID = 0 = Terminator for list of options descriptors
macro OPTION_END
{
DB  XEND         ; ID = 0 = Terminator for list of options descriptors
}
; ID = 1 = means option is list of keywords
macro OPTION_KEYS  x1, x2, x3, x4
{
DB  XKEY         ; ID = 1 = means option is list of keywords
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = byte 
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection
DD  x4 - OpDesc  ; Pointer to list of 0-terminated keywords, 0,0 means end of list 
}
; ID = 2 = means 32-bit unsigned value, interpreted as decimal
macro OPTION_DECIMAL_32  x1, x2, x3
{
DB  XDEC         ; ID = 2 = means 32-bit unsigned value, interpreted as decimal
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = dword
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}
; ID = 3 = means 64-bit unsigned value, interpreted as hex
macro OPTION_HEX_64  x1, x2, x3
{
DB  XHEX         ; ID = 3 = means 64-bit unsigned value, interpreted as hex
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = qword
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}
; ID = 3 = means 64-bit unsigned value, interpreted as hex
macro OPTION_SIZE_64  x1, x2, x3
{
DB  XSIZE        ; ID = 3 = means 64-bit unsigned value, interpreted as hex
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = qword
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}
; ID = 5 = means pointer to pointer to string
macro OPTION_STRING  x1, x2, x3
{
DB  XSTR         ; ID = 5 = means pointer to pointer to string
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = pointer to string, 0-terminated
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}
; Support strings formatting and options strings save
OPTION_NAME_FORMAT    EQU  29    ; Formatted output left part before " = " size  
PATH_BUFFER_SIZE      EQU  256   ; Limit for buffers with paths, include last 0
; Global aliases for compact access to variables
; Update this required if change variables layout at connect_var.inc
ALIAS_STDIN           EQU  [rbx + 8*00]
ALIAS_STDOUT          EQU  [rbx + 8*01]
ALIAS_REPORTNAME      EQU  [rbx + 8*02]
ALIAS_REPORTHANDLE    EQU  [rbx + 8*03]
ALIAS_SCENARIOHANDLE  EQU  [rbx + 8*04] 
ALIAS_SCENARIOBASE    EQU  [rbx + 8*05]
ALIAS_SCENARIOSIZE    EQU  [rbx + 8*06] 
ALIAS_COMMANDLINE     EQU  [rbx + 8*07]
; This 3 variables must be continuous for return status from subroutines
ALIAS_ERROR_STATUS    EQU  [rbx + 8*08]    
ALIAS_ERROR_P1        EQU  [rbx + 8*08]  ; alias of previous
ALIAS_ERROR_P2        EQU  [rbx + 8*09]
ALIAS_ERROR_C         EQU  [rbx + 8*10]
; Registers and memory dump subroutines support: global used data definitions.
REGISTER_NAME_COLOR   EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY
REGISTER_VALUE_COLOR  EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
DUMP_ADDRESS_COLOR    EQU  FOREGROUND_GREEN + FOREGROUND_INTENSITY
DUMP_DATA_COLOR       EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
; Constants for keyboard input check
BLANK_KEY             EQU  00h
ENTER_KEY             EQU  0Dh 
;------------------------------------------------------------------------------;
;                 Source definitions for fragment under debug.                 ;
;------------------------------------------------------------------------------;
include 'connect_equ.inc'
;------------------------------------------------------------------------------;
;                               Code section.                                  ;
;------------------------------------------------------------------------------;
format PE64 console
entry start
section '.text' code readable executable
start:
;------------------------------------------------------------------------------;
;                           Template service code.                             ;
;------------------------------------------------------------------------------;
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
; Start message, only after loading options, possible " display = off "
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
; Verify and correct (if required) start and stop address,
; yet maximum size = 4 KB 
BLOCK_SIZE_LIMIT = 4096 - 1
mov rax,[OptionStartAddress]
mov rcx,[OptionStopAddress]
cmp rax,rcx
jb .skipSwap
xchg rax,rcx
.skipSwap:
mov rdx,rcx
sub rdx,rax
jz .setLimit
cmp rdx,BLOCK_SIZE_LIMIT
jb .skipLimit
.setLimit:
lea rcx,[rax + BLOCK_SIZE_LIMIT]
.skipLimit:
mov [OptionStartAddress],rax
mov [OptionStopAddress],rcx
; Show list with options settings
lea rcx,[OpDesc]             ; Parm#1 = RCX = Pointers to options descriptors
lea rdx,[TEMP_BUFFER]        ; Parm#2 = RDX = Pointer to buffer for build text
call ShowScenario
;------------------------------------------------------------------------------;
;                        Code fragment under debug.                            ; 
;------------------------------------------------------------------------------;
lea rcx,ALIAS_ERROR_STATUS      ; RCX = Pointer to status variables block
lea rdx,[TEMP_BUFFER]           ; RDX = Pointer to temporary buffer
call ApplicationKmdShell
mov rcx,ALIAS_ERROR_P1          ; RCX = Status variables block [0]
mov rdx,ALIAS_ERROR_P2          ; RDX = Status variables block [1]
mov r8,ALIAS_ERROR_C            ; R8  = Status variables block [2]
test rax,rax                    ; RAX = Status: 0=Error, otherwise no errors
jz ErrorProgramTripleParm       ; Go if error returned 
;------------------------------------------------------------------------------;
; End of code fragment under debug, continue service code with console output. ; 
;------------------------------------------------------------------------------;
lea rbx,[Alias_Base]       ; RBX = Restore base for variables addressing
; This for "Press ENTER ..." not add to text report
xor eax,eax
mov ALIAS_REPORTNAME,rax   ; Clear report file name pointer 
mov ALIAS_REPORTHANDLE,rax ; Clear report file name handle
; Restore original color
call GetColor              ; Return EAX = Original ( OS ) console color
xchg ecx,eax
call SetColor              ; Set color by input ECX
; Done message, write to console ( optional ) and report file ( optional )
lea rcx,[DoneMsgNoWait]    ; Parm#1 = RCX = Pointer to message
cmp [OptionWaitkey],0
je  @f
lea rcx,[DoneMsgWait]      ; Parm#1 = RCX = Pointer to message
@@:
mov rdx,ALIAS_REPORTHANDLE ; Parm#2 = RDX = Output handle
mov r8,ALIAS_REPORTNAME    ; Parm#3 = R8  = Pointer to report file name
call ConsoleWrite 
; Wait key press
lea rsi,[TEMP_BUFFER]      ; RSI = Non volatile pointer to buffer for char
.waitKey:
mov byte [rsi],BLANK_KEY
mov rcx,rsi                ; Parm#1 = RCX = Pointer to buffer for char
call ConsoleRead           ; Console input
test rax,rax
jz .skipKey                ; Go skip if input error
cmp byte [rsi],ENTER_KEY
jne .waitKey               ; Go repeat if not ENTER key 
.skipKey:
lea rcx,[CrLf2]            ; Parm#1 = RCX = Pointer to 0Dh, 0Ah ( CR, LF )
mov rdx,ALIAS_REPORTHANDLE ; Parm#2 = RDX = Output handle
mov r8,ALIAS_REPORTNAME    ; Parm#3 = R8  = Pointer to report file name
call ConsoleWrite          ; Console output
;------------------------------------------------------------------------------;
;               Exit application, this point used if no errors.                ;
;------------------------------------------------------------------------------;
ExitProgram:               ; Common entry point for exit to OS
xor ecx,ecx                ; Parm#1 = RCX = Exit code = 0 (no errors)
call [ExitProcess]         ; No return from this function
;------------------------------------------------------------------------------;
;               Error handling and exit application.                           ;
;------------------------------------------------------------------------------;
ErrorProgramSingleParm:    ; Here valid Parm#1 = RCX = Pointer to string
xor edx,edx                ; Parm#2 = RDX = Pointer to second string, not used 
ErrorProgramDualParm:      ; Here used 2 params: RCX, RDX
xor r8,r8                  ; Parm#3 = R8  = WinAPI error code, not used 
ErrorProgramTripleParm:    ; Here used all 3 params: RCX, RDX, R8
lea r9,[TEMP_BUFFER]       ; Parm#4 = R9 = Pointer to work buffer
call ShowError             ; Show error message
mov ecx,1                  ; Parm#1 = RCX = Exit code = 1 (error detected)
call [ExitProcess]         ; No return from this function
;------------------------------------------------------------------------------;
;               Helpers subroutines for template service code.                 ;
;------------------------------------------------------------------------------;
;---------- Copy selected text string terminated by 00h -------;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   RSI = Source address                                ;
;          RDI = Destination address                           ;
;          AL  = Selector                                      ;
;          AH  = Limit  (if Selector>Limit, set Selector=0)    ; 
; OUTPUT:  RSI = Modified by copy                              ;
;          RDI = Modified by copy                              ;
;          Memory at [Input RDI] modified                      ; 
;--------------------------------------------------------------;
StringWriteSelected:
test al,al
jz StringWrite    ; Direct write for first string entry
cmp al,ah
ja StringWrite    ; Set limit if selector value > limit  
mov ah,al
; Skip AH strings
cld
@@:
lodsb
cmp al,0
jne @b
dec ah
jnz @b
; No RET continue in the next subroutine
;---------- Copy text string terminated by 00h ----------------;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   RSI = Source address                                ;
;          RDI = Destination address                           ;
; OUTPUT:  RSI = Modified by copy                              ;
;          RDI = Modified by copy                              ;
;          Memory at [Input RDI] modified                      ; 
;--------------------------------------------------------------;
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
;---------- Print 64-bit Hex Number ---------------------------;
; INPUT:  RAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint64:
push rax
ror rax,32
call HexPrint32
pop rax
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------------------------;
; INPUT:  EAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint32:
push rax
ror eax,16
call HexPrint16
pop rax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------------------------;
; INPUT:  AX  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint16:
push rax
xchg al,ah
call HexPrint8
pop rax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------------------------;
; INPUT:  AL  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint8:
push rax
ror al,4
call HexPrint4
pop rax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------------------------;
; INPUT:  AL  = Number (bits 0-3)                              ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
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
;---------- Print 32-bit Decimal Number -----------------------;
; INPUT:   EAX = Number value                                  ;
;          BL  = Template size, chars. 0=No template           ;
;          RDI = Destination Pointer (flat)                    ;
; OUTPUT:  RDI = New Destination Pointer (flat)                ;
;                modified because string write                 ;
;--------------------------------------------------------------;
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
;---------- Print double precision value ----------------------;
; x87 FPU used,                                                ; 
; required x87 presence validation by CPUID before call this.  ;
;                                                              ;
; INPUT:   RAX = Double precision number                       ;
;          BL  = Number of digits in the INTEGER part,         ;
;                used for add left non-signed zeroes.          ; 
;                BL=0 means not print left unsigned zeroes.    ;
;          BH  = Number of digits in the FLOAT part,           ;
;                used as precision control.                    ;
;          RDI = Destination text buffer pointer               ;
;                                                              ;
; OUTPUT:  RDI = Modified by text string write                 ;  
;--------------------------------------------------------------;
DoublePrint:
push rax rbx rcx rdx r8 r9 r10 r11
cld
; Detect special cases for DOUBLE format, yet unsigned indication
mov rdx,07FFFFFFFFFFFFFFFh
and rdx,rax
jz .fp64_Zero             ; Go if special cases = 0.0  or  -0.0
mov rcx,07FF8000000000000h
cmp rdx,rcx
je .fp64_QNAN             ; Go if special case = QNAN (Quiet Not a Number)
mov rcx,07FF0000000000000h
cmp rdx,rcx
je .fp64_INF              ; Go if special case = INF (Infinity)
ja .fp64_NAN              ; Go if special case = NAN (Not a Number)
; Initializing FPU x87
finit
; Change rounding mode from default (nearest) to truncate  
push rax     ; save input value
push rax     ; reserve space
fstcw [rsp]
pop rax
or ax,0C00h  ; correct Rounding Control, RC = FPU CW bits [11-10]
push rax
fldcw [rsp]
pop rax
; Load input value, note rounding mode already changed
fld qword [rsp]
pop rax
; Separate integer and float parts 
fld st0         ; st0 = value   , st1 = value copy
frndint         ; st0 = integer , st1 = value copy
fxch            ; st0 = value copy , st1 = integer
fsub st0,st1    ; st0 = float , st1 = integer
; Build divisor = f(precision selected) 
mov eax,1
movzx ecx,bh    ; BH = count digits after "."
jrcxz .divisorDone
@@:
imul rax,rax,10
loop @b
.divisorDone:
; Build float part as integer number 
push rax
fimul dword [rsp]
pop rax
; Extract signed Binary Coded Decimal (BCD) to R9:R8, float part .X
push rax rax  ; Make frame for stack variable, used for x87 write data
fbstp [rsp]   ; Store BCD integer and pop, destination is 80 bit = 10 bytes
pop r8 r9     ; R9:R8 = data from x87 write
; Extract signed Binary Coded Decimal (BCD) to R11:R10, integer part X.
push rax rax  ; Make frame for stack variable, used for x87 write data
fbstp [rsp]   ; Store BCD integer and pop, destination is 80 bit = 10 bytes
pop r10 r11     ; R11:R10 = data from x87 write
; Check sign of integer and float part 
bt r11,15     ; R11 bit 15 is bit 79 of 80-bit x87 operand (integer part)
setc dl       ; DL = Sign of integer part
bt r9,15      ; R9 bit 15 is bit 79 of 80-bit x87 operand (floating part)
setc dh       ; DH = Sign of floating part
; Go error if sign of integer and float part mismatch
; This comparision and error branching rejected 
; because bug with -1.0 "-" AND "+", CHECK IF SIGN SAVED ?
; cmp dx,0100h
; je .Error
; cmp dx,0001h
; je .Error
; Write "-" if one of signs "-".
test dx,dx
jz @f            ; Go skip write "-" if both integer/floating signs "+"
; Write "-" if negative value
mov al,'-'
stosb
@@:
; Write INTEGER part 
mov dl,0         ; DL = flag "minimum one digit always printed"
mov ecx,18       ; RCX = maximum number of digits in the integer part 
.cycleInteger:   ; Cycle for digits in the INTEGER part
mov al,r11l
shr al,4         ; AL = current digit
cmp cl,1
je .store        ; Go print if last pass, otherwise .X instead 0.X
cmp cl,bl
jbe .store       ; Go print if required by formatting option, BL=count
test dl,dl
jnz .store       ; Go print, if digits sequence already beginned
test al,al
jz .position     ; Otherwise, can go skip print if digit = 0 
.store:
mov dl,1
or al,30h
stosb            ; Write current ASCII digit
.position:
shld r11,r10,4   ; Positioning digits sequence at R11:R10 pair
shl r10,4
loop .cycleInteger         ; Cycle for digits in the INTEGER part
; Write decimal point
test bh,bh
jz .exit         ; Skip if not print float part
mov al,'.'
stosb
; Write FLOATING part
std               ; Write from right to left 
movzx ecx,bh      ; RCX = digits count     
lea rdi,[rdi+rcx] ; RDI = After last digit (char) position
push rdi
dec rdi
.cycleFloat:     ; Cycle for digits in the FLOATING part
mov al,r8l
and al,0Fh
or al,30h
stosb
shrd r8,r9,4     ; Positioning digits sequence at R9:R8 pair
shr r9,4
loop .cycleFloat ; Cycle for digits in the FLOATING part
pop rdi
cld              ; Restore strings increment mode
; Go exit subroutine
jmp .exit
; Write strings for different errors types
.fp64_Zero:			 ; Zero
mov eax,'0.0 '
jmp .fp64special
.fp64_INF:       ; "INF" = Infinity, yet unsigned infinity indicated
mov eax,'INF '
jmp .fp64special
.fp64_NAN:
mov eax,'NAN '   ; "NAN" = (Signaled) Not a number
jmp .fp64special
.fp64_QNAN:
mov eax,'QNAN'   ; "QNAN" = Quiet not a number
.fp64special:
stosd
jmp .exit
.error:
mov al,'?'
stosb
.exit:
; Exit with re-initialize x87 FPU 
finit
pop r11 r10 r9 r8 rdx rcx rbx rax
ret
;---------- Print memory block size as Integer.Float ----------;
; Float part is 1 char, use P1-version of Floating Print       ;
; If rounding precision impossible, print as hex               ;
; Only x.5 floating values supported, otherwise as hex         ;
;                                                              ;
; INPUT:   RAX = Number value, units = Bytes                   ;
;          BL  = Force units (override as smallest only)       ;
;                FF = No force units, auto select              ;
;                0 = Bytes, 1 = KB, 2 = MB, 3 = GB, 4 = TB     ;
;          RDI = Destination Pointer (flat)                    ;
; OUTPUT:  RDI = New Destination Pointer (flat)                ;
;                modified because string write                 ;
;--------------------------------------------------------------;
SizePrint64:
push rax rbx rcx rdx rsi
cld
cmp bl,0FFh
je .autoUnits
; Adjust to requested units ( Bytes, KB, MB, GB, TB )
mov esi,1
movzx ecx,bl
jrcxz .unitsAdjusted
.unitsCycle:
shl rsi,10
loop .unitsCycle
.unitsAdjusted:
mov cl,bl
xor edx,edx
div rsi          ; EAX = Integer part, note overflows ignored if explicit units
mov bl,0
call DecimalPrint32
imul eax,edx,10
div rsi          ; EAX = Float part
cmp cl,0
je .afterNumber
push rax
mov al,'.'
stosb
pop rax
jmp .decimalMode
; Auto-select units ( Bytes, KB, MB, GB, TB ) by mod=0 criteria
.autoUnits:
xor ecx,ecx                 ; ECX = Units selector
test rax,rax
jz .decimalMode             ; Go if value  = 0
.unitsAutoCycle:
mov rbx,rax                 ; RBX = Save previous value
xor edx,edx                 ; RDX = Dividend bits [127-64] = 0
mov esi,1024                ; RSI = Divisor                           
div rsi
mov esi,0FFFFFFFFh
cmp rbx,rsi
ja .above32bit              ; Go execute next division if value > 32-bit
test rdx,rdx
jnz .modNonZero             ; Go print if mod non-zero
.above32bit:
inc ecx                     ; Units selector + 1
jmp .unitsAutoCycle         ; Make cycle for select optimal units
; Check overflow
.modNonZero:
cmp ecx,4
ja .hexMode                 ; Go print hex if units too big
; Print value and units
mov eax,ebx
.decimalMode:
mov bl,0
call DecimalPrint32         ; Print value
.afterNumber:
mov al,' '
stosb
lea rsi,[U_B]
mov al,cl
mov ah,4
call StringWriteSelected    ; Print units
jmp .exit
; Entry point for print as HEX if value too big
.hexMode:
call HexPrint64             ; Print 64-bit hex integer: number of Bytes
mov al,'h'
stosb 
; Exit
.exit:
pop rsi rdx rcx rbx rax
ret
;---------- Get console color, saved at start-------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:  None                                                              ;
;                                                                           ;
; OUTPUT: EAX = Color code                                                  ;
;---------------------------------------------------------------------------;
GetColor:
mov eax,[ScreenInfo.wAttributes]
ret
;---------- Set console color ----------------------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:   ECX = New color code                                             ;
;          Use global variable [StdOut]                                     ;  
;                                                                           ;
; OUTPUT:  RAX = OS Status                                                  ;
;---------------------------------------------------------------------------;
SetColor:
push rbp
mov rbp,rsp                    ; RBP = storage for RSP and pointer to frame
and rsp,0FFFFFFFFFFFFFFF0h     ; Align stack
; Set console color
mov edx,ecx                    ; EDX = Color for set
mov rcx,[StdOut]               ; RCX = Handle for console output
sub rsp,32                     ; Parameters shadow
call [SetConsoleTextAttribute]
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop rbp
ret
;--- Set console foreground color, background color as saved at start ------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:   ECX = New foreground color code                                  ;
;          Use global variable [StdOut]                                     ;
;                                                                           ;
; OUTPUT:  RAX = OS Status                                                  ;
;---------------------------------------------------------------------------;
SetFgColor:
push rbp
mov rbp,rsp                 ; RBP = storage for RSP and pointer to frame
and rsp,0FFFFFFFFFFFFFFF0h  ; Align stack for call WinAPI by convention
; Set console color
call GetColor               ; Return EAX = default color
and eax,CLEAR_FOREGROUND
and ecx,CLEAR_BACKGROUND
lea rdx,[rax + rcx]         ; EDX = Color for set
mov rcx,[StdOut]            ; RCX = Handle for console output
sub rsp,32                  ; Parameters shadow
call [SetConsoleTextAttribute]
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop rbp
ret
;---------------------- Win64 console functions notes -------------------------;
; Used functions:
;
; GetStdHandle
; Input:  Parm#1 = Handle type code for retrieve
; Output: EAX = Handle, if error 0 or INVALID_HANDLE_VALUE 
;
; WriteConsole
; Input:  Parm#1 = Handle of output device
;         Parm#2 = Pointer to buffer
;         Parm#3 = Number of characters to write
;         Parm#4 = Pointer to returned number of successfully chars write
;         Parm#5 = Reserved parameters must be 0 (NULL)
; Output: Status, Nonzero=OK, 0=Error 
;
; ReadConsole
; Input:  Parm#1 = Handle of input device
;         Parm#2 = Pointer to buffer
;         Parm#3 = Number of chars to read (limit, but not for edit)
;         Parm#4 = Pointer to returned number of cars read (before ENTER)
;         Parm#5 = Pointer to CONSOLE_READCONSOLE_CONTROL structure, 0=None
; Output: Status, Nonzero=OK, 0=Error
;
; ExitProcess
; Input:  Parm#1 = Exit code for parent process
; No output, because not return control to caller
;
;------------------------------------------------------------------------------;
DISABLE_ECHO_ALL = 0F9h
;---------- Wait for press any key -----------------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
; Echo and edit string mode disabled                                        ;
; Used simplified variant of [ReadConsole], Number of chars to Read = 1     ;
;                                                                           ;
; INPUT:  RCX = Pointer to output buffer, for single char in this variant   ;
;                                                                           ;
; OUTPUT: RAX = Status                                                      ;
;         Buffer at [input RCX] updated.                                    ;
;---------------------------------------------------------------------------;
ConsoleRead:
push rbx rsi rbp rax rax      ; RBX, RSI, RBP = non-volatile, RAX = for storage
mov rbp,rsp                   ; RBP = storage for RSP and pointer to frame
and rsp,0FFFFFFFFFFFFFFF0h    ; Align stack for call WinAPI by convention
sub rsp,32                    ; Create parameters shadow
mov rbx,[StdIn]               ; RBX = Storage for input device handle
mov rsi,rcx                   ; RSI = Non volatile copy of pointer
; Exit with status = OK if input handle = 0, wait key disabled by options
mov eax,1                     ; RAX = Status = OK, if wait key disabled
test rbx,rbx                  ; RBX = Input handle
jz .exit                      ; Skip keyboard input if handle = 0
; Get current console mode
mov rcx,rbx                   ; RCX = Parm#1 = Input device handle
mov rdx,rbp                   ; RDX = Parm#2 = Pointer to output variable 
call [GetConsoleMode]         ; Get current console mode
test rax,rax                  ; RAX = Status, 0 if error
jz .exit                      ; Go exit function if error
; Change current console mode
mov rcx,rbx                   ; RCX = Parm#1 = Input device handle
mov edx,[rbp]                 ; RDX = Parm#2 = Console mode
and dl,DISABLE_ECHO_ALL       ; Disable echo and string in. (ret. after 1 char)
call [SetConsoleMode]         ; Get current console mode
test rax,rax                  ; RAX = Status, 0 if error
jz .exit                      ; Go exit function if error
; Read console (wait only without echo)
mov rcx,rbx                   ; RCX = Parm#1 = Input device handle
mov rdx,rsi                   ; RDX = Parm#2 = Pointer to in. buffer
mov r8d,1                     ; R8  = Parm#3 = Number of chars to Read
lea r9d,[rbp+8]               ; R9  = Parm#4 = Pointer to out. var., chars count
xor eax,eax                   ; EAX = 0
push rax rax                  ; Align stack + Parm#5 = InputControl
sub rsp,32                    ; Create parameters shadow
call [ReadConsole]            ; Keyboard input
add rsp,32+16                 ; Remove parameters shadow, parm#5, stack align
; Restore current console mode, use parameters shadow created at entry subr.
mov rcx,rbx                   ; RCX = Parm#1 = Input device handle
xchg rbx,rax                  ; RBX = Save error code after input char
mov edx,[rbp]                 ; RDX = Parm#2 = Console mode
call [SetConsoleMode]         ; Set current console mode
; Error code = F( restore, input )
test rbx,rbx                  ; Check status after console input 
setnz bl                      ; BL=0 if input error, BL=1 if input OK
test rax,rax                  ; Check status after restore console mode
setnz al                      ; AL=0 if mode error, AL=1 if mode OK
and al,bl                     ; AL=1 only if both operations status OK
and eax,1                     ; Bit RAX.0=Valid, bits RAX.[63-1]=0
; Exit point, RAX = Status actual here
.exit:
mov rsp,rbp
pop rcx rcx rbp rsi rbx
ret
;---------------------- Win64 console functions notes -------------------------;
; Used functions:
;
; GetStdHandle
; Input:  Parm#1 = Handle type code for retrieve
; Output: EAX = Handle, if error 0 or INVALID_HANDLE_VALUE 
;
; WriteConsole
; Input:  Parm#1 = Handle of output device
;         Parm#2 = Pointer to buffer
;         Parm#3 = Number of characters to write
;         Parm#4 = Pointer to returned number of successfully chars write
;         Parm#5 = Reserved parameters must be 0 (NULL)
; Output: Status, Nonzero=OK, 0=Error 
;
; ReadConsole
; Input:  Parm#1 = Handle of input device
;         Parm#2 = Pointer to buffer
;         Parm#3 = Number of chars to read (limit, but not for edit)
;         Parm#4 = Pointer to returned number of cars read (before ENTER)
;         Parm#5 = Pointer to CONSOLE_READCONSOLE_CONTROL structure, 0=None
; Output: Status, Nonzero=OK, 0=Error
;
; ExitProcess
; Input:  Parm#1 = Exit code for parent process
; No output, because not return control to caller
;
;------------------------------------------------------------------------------;
;---------- ASCII string write to console ----------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:   RCX = Pointer to 0-terminated ASCII string, string output        ; 
;                to console and optional to report file (if RDX non zero)   ;
;          RDX = Report file handle, used as report validity flag only,     ;
;                report file must be re-opened before write                 ;
;          R8  = Pointer to report file name and path,                      ;
;                0-terminated ASCII string                                  ;
;                                                                           ;
; OUTPUT:  RAX = OS Status                                                  ;
;---------------------------------------------------------------------------;
InternalConsoleWrite:    ; This special entry point not required input RDX, R8
mov rdx,[ReportHandle]
mov r8,[ReportName]
ConsoleWrite:            ; This normal entry point required input RDX, R8
; Entry
push rbx rsi rdi rbp r12
mov rbp,rsp                   ; RBP = storage for RSP and pointer to frame
push 0                        ; Scratch pad for output parameter =  write size
and rsp,0FFFFFFFFFFFFFFF0h    ; Align stack for call WinAPI by convention
mov rbx,rdx                   ; RBX = Non volatile copy of report handle
mov r12,r8                    ; R12 = Non volatile copy of report path pointer
; Calculate string length
mov rdx,rcx                   ; RDX = Parm#2 = Pointer to string ( buffer )
xor r8d,r8d                   ; R8  = Parm#3 = Number of chars ( length )
@@:
cmp byte [rcx+r8],0           ; Check current char from string
je @f                         ; Exit cycle if terminator (byte=0) found
inc r8d                       ; Chars counter + 1
jmp @b                        ; Go next iteration
@@:
; Save input parameters for usage for console and file both
mov rsi,rdx                   ; RSI = Non volatile copy of buffer pointer 
mov rdi,r8                    ; RDI = Non volatile copy if length
; Write console - optional
mov eax,1                     ; RAX = Status = OK, if display output disabled
mov rcx,[StdOut]              ; RCX = Parm#1 = Input device handle
jrcxz @f                      ; Skip console output if handle = 0
lea r9,[rbp-8]                ; R9  = Parm#4 = Pointer to out. variable, count
xor eax,eax                   ; RAX = 0
push rax rax                  ; Align stack + Parm#5 (exist = null) = Reserved
sub rsp,32                    ; Create parameters shadow
call [WriteFile]
add rsp,32+16                 ; Remove parameters shadow, parm#5, stack align
@@:
; Check criteria for write report file - optional
mov eax,1                     ; RAX = Status = OK, if report save disabled
test rbx,rbx                  ; RBX = Report temp. handle used as flag
jz .exit                      ; Skip file output if handle = 0
cmp rbx,INVALID_HANDLE_VALUE
je .exit                      ; Skip file output if handle = Invalid = -1
mov rcx,r12                   ; RCX = Parm #1 = Pointer to name string
test rcx,rcx
jz .exit                      ; Skip file output if name pointer = 0
; Open
mov edx,GENERIC_WRITE         ; RDX = Parm #2 = Desired access 
xor r8d,r8d                   ; R8  = Parm #3 = Share mode, not used
xor r9d,r9d                   ; R9  = Parm #4 = Security attributes, not used
xor eax,eax
push rax                      ; This push for stack alignment
push rax                      ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL    ; Parm #6 = File attributes
push OPEN_EXISTING            ; Parm #5 = Creation disposition
sub rsp,32                    ; Create parameters shadow
call [CreateFileA]
add rsp,32+32                 ; Remove parameters shadow and parameters
test rax,rax
jz .exit                      ; Go if open file error
mov rbx,rax
; Positioning pointer to end of file
xchg rcx,rax                  ; RCX = Parm #1 = File handle
xor edx,edx                   ; RDX = Parm #2 = Position, low dword
xor r8d,r8d                   ; R8  = Parm #3 = Position, high dword
mov r9d,FILE_END              ; R9  = Parm #4 = Move method
sub rsp,32
call [SetFilePointer]
add rsp,32
; Write
.write:
mov rcx,rbx              ; RCX = Parm#1 = File handle
mov rdx,rsi              ; RDX = Parm#2 = Pointer to string ( buffer ) 
mov r8,rdi               ; R8  = Parm#3 = Number of chars ( length ) 
xor eax,eax              ; RAX = 0
push rax                 ; This space for output variable plus stack align
mov r9,rsp               ; R9  = Parm#4 = Pointer to out. variable, count
push rax                 ; Parm#5 (exist = null) = Reserved
sub rsp,32               ; Create parameters shadow
call [WriteFile]
add rsp,32+8             ; Remove parameters shadow, parm#5
pop rcx                  ; RCX = Returned size
test rax,rax             ; RAX = status, 0 means error
jz .close                ; Go exit if error
jrcxz .close             ; Go exit if returned size = 0
add rsi,rcx              ; RSI = advance read pointer by returned size
sub rdi,rcx              ; RDI = subtract current read size from size limit
ja .write                ; Repeat read if return size > 0 and limit not reached 
; Close
.close:
mov rcx,rbx
jrcxz .exit
sub rsp,32
call [CloseHandle]       ; Close report file after write
; Exit point, RAX = Status actual here
.exit:
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop r12 rbp rdi rsi rbx
ret
;---------- Create report file ---------------------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; After this function successfully call, function ConsoleWrite              ;
; starts save output information to report file                             ;
;                                                                           ;
; INPUT:  RCX = Pointer to report file name, 0-terminated ASCII string      ;
;         RDX = Pointer to report file handle, return handle = 0 if error   ;  
;                                                                           ;
; OUTPUT: RAX = Status code                                                 ;
;               Variable report handle at [input RCX] =                     ; 
;               Temporary handle, used as flag for write report file enable ;
;---------------------------------------------------------------------------;
CreateReport:
push rbx rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h    ; Align stack for call WinAPI by convention
mov rbx,rdx                   ; RBX = Non volatile copy of handle pointer 
; Create file, input parameter RCX = Parm #1 = Pointer to file name
xor eax,eax                   ; RAX = 0 for store result = 0 if ReportName = 0
jrcxz @f
mov edx,GENERIC_WRITE         ; RDX = Parm #2 = Desired access 
xor r8d,r8d                   ; R8  = Parm #3 = Share mode, not used
xor r9d,r9d                   ; R9  = Parm #4 = Security attributes, not used
xor eax,eax
push rax                      ; This push for stack alignment
push rax                      ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL    ; Parm #6 = File attributes
push CREATE_ALWAYS            ; Parm #5 = Creation disposition
sub rsp,32                    ; Create parameters shadow
call [CreateFileA]
add rsp,32+32                 ; Remove parameters shadow and parameters
@@:
; Store result
mov [rbx],rax                 ; RAX = Returned handle
; Close file
xchg rcx,rax
jrcxz @f
sub rsp,32
call [CloseHandle]
@@:
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop rbp rbx
ret
;---------- Read scenario file ---------------------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT: RCX = Pointer to scenario file path and name,                      ;
;              0-terminated ASCII string                                    ;
;        RDX = Pointer to scenario handle                                   ;
;        R8  = Pointer to loaded scenario base address variable,            ; 
;              this variable is buffer base address for file read           ;
;        R9  = Pointer to scenario size variable,                           ; 
;              this variable is size limit for this buffer                  ;   
;                                                                           ;
; OUTPUT: RAX = OS API last operation status code                           ;
;         Variable scenario handle at [input RDX] = updated by file open    ;
;         Variable scenario size at [input R9] = Read size, 0 if error      ;
;---------------------------------------------------------------------------;
ReadScenario:
push rbx rsi rdi rbp r12 r13 r14
mov rbx,rdx                  ; RBX = non volatile pointer to scenario handle
mov rsi,r8                   ; RSI = non volatile pointer to loaded scenario 
mov rdi,r9                   ; RDI = non volatile pointer to scenario size 
mov rbp,rsp                  ; RBP = Storage for RSP before alignment
and rsp,0FFFFFFFFFFFFFFF0h   ; Align stack for call WinAPI by convention
; Open file, by input parameters: RCX = Parm #1 = Pointer to file name 
xor eax,eax                  ; RAX = 0 for store result = 0 if ScenarioName = 0
jrcxz .error                 ; Skip operation if file name pointer = 0
mov edx,GENERIC_READ         ; RDX = Parm #2 = Desired access 
xor r8d,r8d                  ; R8  = Parm #3 = Share mode, not used
xor r9d,r9d                  ; R9  = Parm #4 = Security attributes, not used
xor eax,eax
push rax                     ; This push for stack alignment
push rax                     ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL   ; Parm #6 = File attributes
push OPEN_EXISTING           ; Parm #5 = Creation/Open disposition
sub rsp,32                   ; Create parameters shadow
call [CreateFileA]
add rsp,32+32                ; Remove parameters shadow and parameters
mov [rbx],rax                ; Save scenario file handle
; Initializing for read file
xor r12,r12                  ; R12 = 0, clear read size counter
mov r13,[rsi]                ; R13 = Base address of memory buffer
mov r14,[rdi]                ; R14 = Size limit of memory buffer
; Read file
.read:
mov rcx,[rbx]            ; RCX = Parm #1 = File handle
jrcxz .error             ; Skip read and close if handle = 0 (if open error)
mov rdx,r13              ; RDX = Parm #2 = Buffer base address for read
mov r8,r14               ; R8  = Parm #3 = Buffer size limit
xor eax,eax
push rax                 ; This space for output variable plus stack align
mov r9,rsp               ; R9  = Parm #4 = Pointer to output size
push rax                 ; Parm #5 = Pointer to overlapped str., not used
sub rsp,32
call [ReadFile]
add rsp,32+8
pop rcx                  ; RCX = Output size, RAX = Output status
; Analusing read results
test rax,rax
jz .error                ; Go error if OS status = 0
jrcxz .result            ; Go normal read termination if returned size = 0
test ecx,ecx             ; Note bits RCX.63-32 cleared at PUSH RAX=0
js .error                ; Go error if size negative, note for 32-bit only
add r12,rcx              ; R12 = accumulate read size
add r13,rcx              ; R13 = advance read pointer by returned size
sub r14,rcx              ; R14 = subtract current read size from size limit 
ja .read                 ; Repeat read if return size > 0 and limit not reached 
jb .error          ; Error if read size > size limit, if R14 = 0, means read OK 
; Write result size
.result:
mov [rdi],r12            ; Write scenario size = file size if read OK 
jmp .close
.error:
mov qword [rdi],0        ; Write scenario size = 0 if read error
; Close file
.close:
mov rcx,[rbx]            ; RCX = Parm #1 = File handle
jrcxz .exit
sub rsp,32
call [CloseHandle]
.exit:
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop r14 r13 r12 rbp rdi rsi rbx
ret
;---------- Parse scenario file and update options variables ---------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:   RCX = Pointer to buffer with loaded scenario file                ;  
;          RDX = Limit for this buffer, address of first not-valid byte     ;          
;          R8  = Pointer to options descriptors list                        ;
;          R9  = Pointer to error status variables, for error reporting:    ;
;                3 QWORDS, 2 pointers to strings + 1 OS API error code      ;         
;                                                                           ;
; OUTPUT:  RAX = Status, 0 = error, error status variables valid            ;
;                        1 = no errors, error status variables not used     ;
;          Update options values variables, addressed by descriptors at R8  ;
;          Update status variables, addressed by R9, if error               ;
;                                                                           ;         
;---------------------------------------------------------------------------;
ParseScenario:
cld
push rbx rsi rdi rbp r12
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h  ; Align stack for call WinAPI by convention
; This cycle for input scenario file strings
mov r12,r9           ; R12 = Pointer to error status variables, 3 qwords 
mov r11,r8           ; R11 = Pointer to options descriptors list 
mov r8,rcx           ; R8  = Pointer to buffer with scenario file
mov r9,rdx           ; R9  = Buffer limit, addr. of first not valid byte
; Pre-clear status
xor eax,eax
mov [r12 + 00],rax
mov [r12 + 08],rax
mov [r12 + 16],rax
; This cycle for strings in the scenario
StringsCycle:
mov [r12 + 08],r8    ; R8 = Pointer to parsed error cause string
mov r10,r11          ; R10 = Reload pointer to options descriptors list 
; This cycle for options descriptors
.optionsCycle:
mov al,[r10 + X0]    ; AL = Option type from option descriptor
cmp al,XEND
je .parseError1       ; Go error if option not found at list, unknown option
cmp al,XLAST
ja .parseError1       ; Go error if option not found at list, unknown option
mov rsi,r8           ; RSI = Pointer to text file buffer 
mov edi,[r10 + X3]   ; RDI = Pointer to option keyword
add rdi,OpDesc
; This cycle for option name word compare
.detectName:
cmp rsi,r9              ; RSI = Pointer to scenario data, R9 = Pointer limit
jae .parseExitOK         ; Go if scenario done  
mov ah,[rdi]            ; AH = Current char from option descriptor, keyword 
inc rdi
test ah,ah              ; AH = 0 means keyword done, means keyword match 
jz .detectedThisOption  ; Go if keyword match detected 
lodsb                   ; AL = Current char from scenario file
cmp al,0Ah
je .detectNextString    ; Go if LF(Line Feed), keyword done, try next
cmp al,0Dh
je .detectNextString    ; Go if CR(Carriage Return), keyword done, try next
cmp al,';'
je .detectTailString    ; Go if comments, keyword done, try next
cmp al,' '
je .detectNextOption    ; Go if SPACE, keyword done, try next
cmp al,09h
je .detectNextOption    ; Go if TAB, keyword done, try next 
cmp al,'='
je .detectNextOption    ; Go if EQUAL, keyword done, try next 
cmp al,'0'
jb .parseError2         ; Go error if unexpected char
cmp al,'z'
ja .parseError2         ; Go error if unexpected char
cmp al,'A'
jb @f                   ; Skip if not a text char
cmp al,'z'
ja @f                   ; Skip if not a text char 
and ax,0DFDFh           ; If text char, convert both compare chars to UPPER CASE 
@@:
cmp al,ah               ; Compare chars from keyword pattern and scenario
je .detectName          ; Continue compare if this chars match 
; Option not detected, select next element of options descriptors list
.detectNextOption:
mov al,[r10 + X0]       ; AL = Option type from option descriptor
add r10,XSMALL
cmp al,XKEY
jne .optionsCycle
add r10,XDELTA
jmp .optionsCycle
; Option detected, select and run option handler
.detectedThisOption:
mov al,[r10 + X0]       ; AL = Option type from option descriptor 
cmp al,XKEY
je .handlerOptionKeys
cmp al,XDEC
je .handlerOptionDecimal32
cmp al,XHEX
je .handlerOptionHex64
cmp al,XSIZE
je .handlerOptionSize64
; Handler for option type 5 = string
call OptionString 
jmp .parseDone
; Handler for option type 1 = one of keywords
.handlerOptionKeys:
call OptionKeys
jmp .parseDone
; Handler for option type 2 = 32-bit value as decimal
.handlerOptionDecimal32:
call OptionDecimal32
jmp .parseDone
; Handler for option type 3 = 64-bit value as hex
.handlerOptionHex64: 
call OptionHex64
jmp .parseDone
; Handler for option type 4 = 64-bit value as size
.handlerOptionSize64:
call OptionSize64 
.parseDone:
jc .parseError2            ; Go if error detected
; Detect tail, non-informative part of string
.detectTailString:
cmp rsi,r9
jae .parseExitOK            ; Go if scenario done
lodsb                      ; Read current char, scan for end of this string
cmp al,0Ah
je .detectNextString
cmp al,0Dh
jne .detectTailString   
; Step to next string of scenario file 
.detectNextString:
cmp rsi,r9
jae .parseExitOK            ; Go if scenario done
lodsb                      ; Read current char, scan for start next string
cmp al,0Ah
je .detectNextString 
cmp al,0Dh
je .detectNextString 
lea r8,[rsi-1]             ; R8 = Address of first char of next string
jmp StringsCycle
; Error branches
.parseError1:              ; This handler for unknown option keyword
lea rax,[MsgUnknownOption]
jmp .entryError
.parseError2:              ; This handler for errors in option string
lea rax,[MsgOption]
.entryError:
mov [r12 + 00],rax         ; RAX = Pointer to error comments string
mov r8,[r12 + 08]          ; R8 = Pointer to parsed error cause string
; Terminate error caused string for prevent show all scenario file
mov rsi,r8                 ; Start scanning end of error cause string
lea rdi,[r9 - 1]
.scancrlf:
cmp rsi,rdi                ; RDI = Loaded scenario file limit in the buffer
jae .limitcrlf             ; Go if scenario file done 
lodsb
cmp al,0Ah
je .foundlf
cmp al,0Dh
.foundlf:
jne .scancrlf
.limitcrlf:
mov byte [rsi],0           ; Mark end of string for output error cause string
; Exit points
xor eax,eax   ; Status = error
jmp .parseExit 
.parseExitOK:
mov eax,1     ; Status = no errors
.parseExit:
mov rsp,rbp   ; This for restore after alignment and also instead ADD RSP,32
pop r12 rbp rdi rsi rbx
ret
;---------- Local subroutine: OPTION_KEYS handler -----------------------------;
; INPUT:    RSI = Pointer to scenario file current parse fragment              ;
;           R9  = Limit for RSI, address of first not valid byte               ;
;           R10 = Pointer to this detected option descriptor                   ;
; OUTPUT:   R8  = Updated pointer to current scenario                          ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionKeys:
call SkipEqual        ; Skip " = " fragment
jc ParseSpecialCase   ; Go if scenario done or parsing error
mov edi,[r10 + X4]    ; RDI = Patterns pointer , RSI = Scenario pointer
add rdi,OpDesc
xor ecx,ecx          ; ECX = Possible keywords pointer
mov r8,rsi           ; R8 = Pointer to keyword in the file
.nextKeywordCycle:   ; This cycle for select next possible keyword 
mov rsi,r8           ; RSI = Restore pointer to keyword in the file
.keywordCycle:       ; This cycle for compare option current keyword
cmp rsi,r9           ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; AL = current char from scenario file, Pointer + 1
mov ah,[rdi]         ; AH = current char from comparision pattern 
inc rdi              ; Pattern pointer + 1
test ah,ah
jz .keywordMatch     ; Go if possible keyword done, keyword match
cmp al,'0'
jb .error            ; Go if wrong char in the keyword, next line
cmp al,'z'
ja .error            ; Go if wrong char in the keyword, next line
cmp al,'A'
jb .skipConvert      ; Go if convert to upper case not required
cmp al,'z'
ja .skipConvert      ; Go if convert to upper case not required
and ax,0DFDFh        ; Convert both compared chars to upper case
.skipConvert:
cmp al,ah
jne .scanZero        ; Go to next possible keyword comparision if mismatch
test ah,ah
jz .keywordMatch     ; Go if keyword match, zero reached at pattern
jmp .keywordCycle
.error:              ; Go this if wrong char detected after "="
jmp ParseError
.scanZero:           ; Go this if next possible keyword compare
mov al,[rdi]
inc rdi
cmp al,0
jne .scanZero
cmp byte [rdi],0
je .error                ; Go error if list done but keyword not detected
inc ecx                  ; ECX = counter for option value
jmp .nextKeywordCycle    ; Otherwise, go compare with next possible keyword
.keywordMatch:           ; Go this if keyword match
mov edx,[r10 + X2]       ; RDX = Pointer to option value
add rdx,OpDesc
mov [rdx],cl       ; Write option value, one byte selector
; Global exit points with global-visible labels
ParseOK:             ; Next, return and skip remaining string part
clc
ParseSpecialCase:  ; Return with CF, ZF valid
ret                ; Return with CF=0 means normal status 
EndOfScenario:
or al,1
stc
ret                ; Return with CF=1, ZF=0 means end of scenario file 
ParseError:
xor al,al
stc
ret                ; Return with CF=1, ZF=1 means parse error: unexpected char
;---------- Local subroutine: OPTION_DECIMAL_32 handler -----------------------;
; INPUT:    RSI = Pointer to scenario file current parse fragment              ;
;           R9  = Limit for RSI, address of first not valid byte               ;
;           R10 = Pointer to this detected option descriptor                   ;
; OUTPUT:   R8  = Updated pointer to current scenario                          ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionDecimal32:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; ECX = Numeric value for extract
.cycleDecimal:       ; Cycle for interpreting decimal numeric string
cmp rsi,r9           ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb
cmp al,'0'
jb .stopDecimal      ; Go if not a decimal digit char '0'...'9'
cmp al,'9'
ja .stopDecimal      ; Go if not a decimal digit char '0'...'9'
and eax,0Fh          ; Mask for convert '0'...'9' to 0...9
imul rcx,rcx,10      ; Update value, use 64-bit RCX because unsigned required
add ecx,eax          ; Add current value
jmp .cycleDecimal    ; Continue cycle for interpreting decimal numeric string
.stopDecimal:        ; This point for first non-decimal char detected
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
jne ParseError       ; Go error if wrong char after digit
.normalTerm:         ; Otherwise normal termination 
mov edx,[r10 + X2]   ; RDX = Pointer to option value
add rdx,OpDesc
mov [rdx],ecx        ; Write option value, dword, extracted as decimal 
clc                  ; Next, return and skip remaining string part
ret
;---------- Local subroutine: OPTION_HEX_64 handler ---------------------------;
; INPUT:    RSI = Pointer to scenario file current parse fragment              ;
;           R9  = Limit for RSI, address of first not valid byte               ;
;           R10 = Pointer to this detected option descriptor                   ;
; OUTPUT:   R8  = Updated pointer to current scenario                          ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionHex64:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; RCX = Numeric value for extract
.cycleHex:           ; Cycle for interpreting hex numeric string
cmp rsi,r9           ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read char from scenario
cmp al,'0'
jb .nodecimal        ; Go if not a decimal digit char '0'...'9'
cmp al,'9'
jna .decimal         ; Go if decimal digit char '0'...'9'
.nodecimal:
mov ah,al
and ah,0DFh          ; Make uppercase for convert 'a'...'f' to 'A'...'F'
cmp ah,'A'
jb .stopHex          ; Go if not a hex digit char 'A'...'F' 
cmp ah,'F'
ja .stopHex          ; Go if not a hex digit char 'A'...'F'
mov al,ah
sub al,'A'-10        ; Convert 'A'...'F' to 10...15
.decimal:
and eax,0Fh          ; Convert to 00h...0Fh values
shl rcx,4            ; Shift previous extracted value
or ecx,eax           ; Add current char
jmp .cycleHex        ; Continue cycle for interpreting hex numeric string
.stopHex:            ; This point for first non-hexadecimal char detected
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
jne ParseError       ; Go error if wrong char after digit
.normalTerm:         ; Otherwise normal termination, store extracted value 
mov edx,[r10 + X2]   ; RDX = Pointer to option value
add rdx,OpDesc
mov [rdx],rcx        ; Write option value, qword, extracted as decimal 
clc                  ; Next, return and skip remaining string part
ret
;---------- Local subroutine: OPTION_SIZE_64 handler --------------------------;
; INPUT:    RSI = Pointer to scenario file current parse fragment              ;
;           R9  = Limit for RSI, address of first not valid byte               ;
;           R10 = Pointer to this detected option descriptor                   ;
; OUTPUT:   R8  = Updated pointer to current scenario                          ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionSize64:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; RCX = Numeric value for extract
.cycleNumStr:        ; Cycle for interpreting numeric string
cmp rsi,r9           ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read char from scenario
cmp al,'0'
jb .notadigit        ; Go if not a digit
cmp al,'9'
ja .notadigit        ; Go if not a digit
and eax,0Fh          ; Mask digit, '0'...'9' converted to 0...9
imul rcx,rcx,10      ; Update value, use 64-bit RCX because unsigned required
add ecx,eax          ; Add current digit to extracted value
jmp .cycleNumStr
.notadigit:         ; First non-numeric char detected, also cycle for this part
cmp al,0Ah
je .normalTerm      ; Go string end if 0Ah (LF)
cmp al,0Dh
je .normalTerm      ; Go string end if 0Dh (CR)
cmp al,';'
je .normalTerm      ; Go string end if comments
cmp al,09h
je .nextChar
cmp al,' '
je .nextChar
cmp al,'M'          ; Detect M = megabytes
je .megabytes
cmp al,'m'
je .megabytes
cmp al,'G'          ; Detect G = gigabytes
je .gigabytes
cmp al,'g'
je .gigabytes
cmp al,'K'          ; Detect K = kilobytes
je .kilobytes
cmp al,'k'
jne ParseError
.kilobytes:
imul rcx,rcx,1024            ; Make kilobytes from accumulated value
jmp .nextChar
.megabytes:
imul rcx,rcx,1024*1024       ; Make megabytes from accumulated value
jmp .nextChar
.gigabytes:
imul rcx,rcx,1024*1024*1024  ; Make gigabytes from accumulated value 
.nextChar:
lodsb                ; Get next char after numeric value
cmp rsi,r9           ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
jmp .notadigit
.normalTerm:         ; Otherwise normal termination, store extracted value 
mov edx,[r10 + X2]   ; RDX = Pointer to option value
add rdx,OpDesc
mov [rdx],rcx        ; Write option value, qword, extracted as decimal 
clc                  ; Next, return and skip remaining string part
ret
;---------- Local subroutine: OPTION_STRING handler ---------------------------;
; INPUT:    RSI = Pointer to scenario file current parse fragment              ;
;           R9  = Limit for RSI, address of first not valid byte               ;
;           R10 = Pointer to this detected option descriptor                   ;
; OUTPUT:   R8  = Updated pointer to current scenario                          ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionString:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error 
mov edi,[r10 + X2]   ; RDI = Pointer to pointer to string
add rdi,OpDesc
mov rdi,[rdi]        ; RDI = Pointer to string
mov ecx,PATH_BUFFER_SIZE - 1   ; Limit for string buffer, exclude last 0
.cycle:              ; Cycle for string copy from scenario to buffer
cmp rsi,r9           ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read current char
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
je .stop             ; Go string end if one of this chars
stosb                ; Store char in the destination buffer
loop .cycle          ; Cycle for string copy, limited by buffer size (RCX) 
jmp ParseError       ; Go error if too long string
.stop:               ; End of informative part of string
mov al,0
stosb                ; Write 0-terminator
clc                  ; Next, return and skip remaining string part
ret
;---------- Check line continue by compare current char -----------------------; 
; INPUT:    AL = Char for comparision                                          ;
; OUTPUT    ZF flag, 1(Z) if char detected, 0(NZ) if char not detected         ;
;           char detected if match one of:                                     ;
;           0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)                     ;
;------------------------------------------------------------------------------;
CheckLineChar:
cmp al,0Ah
je @f          ; Go with ZF=1 if LF (Line Feed)
cmp al,0Dh
je @f          ; Go with ZF=1 if CR (Carriage Return)
cmp al,' '
je @f          ; Go with ZF=1 if SPACE
cmp al,09h     
je @f          ; Go with ZF=1 if TAB
cmp al,';'
@@:
ret
;---------- Local subroutine for skip " = " -----------------------------------;
; INPUT:   RSI = Pointer to scenario file current parse fragment               ;
;          R9  = Limit for RSI, address of first not valid byte                ;
; OUTPUT:  RSI = Updated by skip fragment " = "                                ;
;          CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF   ;
;          ZF flag = special case type, valid if CF = 1                        ;
;          ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached          ;
;------------------------------------------------------------------------------;
SkipEqual:
cmp rsi,r9      ; Check end of file
jae .normal     ; Go exit if end of file
lodsb           ; AL = current char
cmp al,' '
je SkipEqual    ; Continue skip if SPACE
cmp al,09h
je SkipEqual    ; Continue skip if TAB
cmp al,'='
jne .error 
.cycle:
cmp rsi,r9      ; Check end of file
jae .normal     ; Go exit if end of file
lodsb           ; AL = current char
cmp al,' '
je .cycle       ; Continue skip if SPACE
cmp al,09h
je .cycle       ; Continue skip if TAB
dec rsi         ; RSI = Pointer to first char after " = " sequence
clc
ret             ; Return with CF=0 means normal status 
.normal:
or al,1
stc
ret             ; Return with CF=1, ZF=0 means end of scenario file 
.error:
xor al,al
stc
ret             ; Return with CF=1, ZF=1 means parse error: unexpected char
;---------- Show scenario options settings ---------------------------------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:  RCX = Pointer to options descriptors list                         ;
;         RDX = Pointer to work buffer for prepare text data                ;
;               no limits provided, caller must associate buffer size and   ;
;               text output size, typically additional space available      ;
;                                                                           ;
; OUTPUT: None                                                              ;
;         Use memory at [input RDX]                                         ;
;                                                                           ;         
;---------------------------------------------------------------------------;
ShowScenario:
cld
push rbx rsi rdi rbp 
; Initializing cycle for show options 
mov rbx,rcx            ; RBX = Pointer to options descriptors list 
mov rdi,rdx            ; RDI = Pointer to work buffer for prepare text data 
push rdi
; Start cycle for show options, build text block in the buffer
.opInterpreting:
mov al,[rbx + X0]      ; AL = Option type from descriptor
cmp al,XEND               
je .opDone             ; Go exit cycle if terminator detected
cmp al,XLAST
ja .opDone             ; Go exit cycle if unknown option code
; Write option name
push rax
mov rdx,rdi
mov ecx,OPTION_NAME_FORMAT
mov al,' '
rep stosb
xchg rdi,rdx
mov esi,[rbx + X1]
add rsi,OpDesc
call StringWrite       ; Write option name, left part of string
mov rdi,rdx
mov ax,'= '
stosw                  ; Write "= " between left and right parts of string 
pop rax                ; Restore option type, AL = Type
mov esi,[rbx + X2]     ; RSI = Pointer to option value, size is option-specific
add rsi,OpDesc
; Detect option type = AL
cmp al,XKEY
je .opKeys
cmp al,XDEC
je .opDecimal32
cmp al,XHEX
je .opHex64
cmp al,XSIZE
je .opSize64
; Option handler = string
.opString:
mov rsi,[rsi]              ; RSI = Pointer to raw string
call StringWrite           ; Write option value after " = ", raw string
.opInterpretingP25:
add rbx,XSMALL             ; RBX = Pointer, go to next option descriptor
mov ax,0A0Dh
stosw                      ; Make next string, write CR, LF 
jmp .opInterpreting
; Option handler = keys
.opKeys:
mov al,[rsi]               ; AL = Index for sequence of 0-terminated strings
mov ah,0FFh
mov esi,[rbx + X4]
add rsi,OpDesc
call StringWriteSelected   ; Write option value after " = ", selected keyword
add rbx,XBIG               ; RBX = Pointer, go to next option descriptor
mov ax,0A0Dh
stosw                      ; Make next string, write CR, LF 
jmp .opInterpreting
; Option handler = decimal 32
.opDecimal32:
mov eax,[rsi]              ; EAX = Value for visual as 32-bit decimal number
push rbx
mov bl,0                   ; BL = Template for print
call DecimalPrint32        ; Write option value after " = ", decimal number
pop rbx
jmp .opInterpretingP25
; Option handler = hex 64
.opHex64:
mov rax,[rsi]              ; RAX = Value for visual as 64-bit hex number
call HexPrint64            ; Write option value after " = ", hex number
mov al,'h'
stosb
jmp .opInterpretingP25
; Option handler = size 64
.opSize64:
mov rax,[rsi]           ; RAX = Value for visual as 64-bit size, can use K/M/G
push rbx
mov bl,0FFh
call SizePrint64        ; Write option value after " = ", size
pop rbx
jmp .opInterpretingP25
; Termination
.opDone:
mov ax,0A0Dh
stosw                   ; Make next string, write CR, LF 
mov al,0
stosb                   ; Terminate all sequence of strings, write 0 byte
pop rcx
; Read data from prepared buffer and display to console, 
; optionally save to report file
call InternalConsoleWrite
pop rbp rdi rsi rbx 
ret
;---------- Show details about detected error and wait key press -----------;
; Input / Output parameters and Volatile / Non volatile registers           ;
; compatible with Microsoft x64 calling convention                          ;
;                                                                           ;
; INPUT:  RCX = Pointer to error description first string, 0 means skip     ;
;         RDX = Pointer to error description second string, 0 means skip    ;
;         R8D = Windows style error code for decoding by WinAPI and         ;
;               show string "<Error name> (code)", 0 means skip             ;
;         R9  = Pointer to work (transit) buffer for prepare text data      ;
;               no limits provided, caller must associate buffer size and   ;
;               text output size, typically additional space available      ;
;                                                                           ;
; OUTPUT: None                                                              ;
;         Use memory at [input R9]                                          ;
;                                                                           ;         
;---------------------------------------------------------------------------;
ShowError:
cld
push rbx rsi rdi rbp 
mov rbx,rcx                     ; RBX = String #1, non volatile
mov rsi,rdx                     ; RSI = String #2, non volatile
mov edi,r8d                     ; EDI = WinAPI error code, non volatile
mov rbp,r9                      ; RBP = Buffer pointer, non volatile
; Set color and write "Error: " message part
mov ecx,FOREGROUND_RED + FOREGROUND_INTENSITY
call SetFgColor                 ; Color for "Error: " message part
lea rcx,[MsgError]
call InternalConsoleWrite
; Set color and conditionally write first string
mov ecx, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
call SetFgColor
mov rcx,rbx
jrcxz @f                        ; Go skip if string pointer = 0
call InternalConsoleWrite       ; First string about error
@@:
; Conditionally write second string with alignment for "Error: " part
test rsi,rsi
jz @f                           ; Go skip if string pointer = 0
lea rcx,[CrLf]
call InternalConsoleWrite       ; Next string
lea rcx,[MsgErrorTab]
call InternalConsoleWrite       ; Tabulation for alignment for "Error: " part
mov rcx,rsi
call InternalConsoleWrite       ; Second string about error
@@:
; Conditionally write third string with alignment for "Error: " part
test rdi,rdi
jz @f                           ; Go skip if error code = 0
lea rcx,[CrLf]
call InternalConsoleWrite       ; Next string
lea rcx,[MsgErrorTab]
call InternalConsoleWrite       ; Tabulation for alignment for "Error: " part 
push rdi
lea rsi,[MsgErrorOS]            ; RSI = Pointer to string
mov rdi,rbp                     ; RDI = Pointer to buffer
call StringWrite                ; Write "OS error" to buffer
pop rax                         ; EAX = Error code
mov bl,0                        ; BL  = numeric template code
mov esi,eax                     ; ESI = Error code, backup
call DecimalPrint32             ; Write error code decimal number to buffer
mov ax,' ='
stosw
stosb
xchg eax,esi               ; EAX = WinAPI error code
call DecodeError           ; Write OS error description string to buffer
mov al,0
stosb
mov rcx,rbp
call InternalConsoleWrite  ; Write from buffer to console 
@@:
; Restore console color, skip string and write done message "Press ENTER..."
call GetColor
xchg ecx,eax
call SetColor              ; Restore original color
lea rcx,[CrLf2]
call InternalConsoleWrite
lea rcx,[DoneMsgNoWait]    ; Parm#1 = RCX = Pointer to message
cmp [OptionWaitkey],0
je  @f
lea rcx,[DoneMsgWait]      ; Parm#1 = RCX = Pointer to message
@@:
call InternalConsoleWrite
; Wait key press, after key pressed skip string
lea rsi,[TEMP_BUFFER]      ; RSI = Non volatile pointer to buffer for char
.waitKey:
mov byte [rsi],BLANK_KEY
mov rcx,rsi                ; Parm#1 = RCX = Pointer to buffer for char
call ConsoleRead           ; Console input
test rax,rax
jz .skipKey                ; Go skip if input error
cmp byte [rsi],ENTER_KEY
jne .waitKey               ; Go repeat if not ENTER key 
.skipKey:
lea rcx,[CrLf2]
call InternalConsoleWrite
pop rbp rdi rsi rbx 
ret
;---------- Translation error code to error name string -------;
;                                                              ;
; INPUT:   RAX = Error code for translation                    ;
;          RDI = Destination address for build text string     ;
;                                                              ;
; OUTPUT:  RDI = Modified by string write                      ;
;          Memory at [Input RDI] = output string               ;
;                                  not 0-terminated            ;
;--------------------------------------------------------------;
DecodeError:
push rsi rbp
mov rbp,rsp                  ; RBP = storage for RSP and pointer to frame
and rsp,0FFFFFFFFFFFFFFF0h   ; Align stack
; Get text string from OS
xor ecx,ecx
push rcx                     ; Pointer to dynamically allocated buffer
mov rdx,rsp
push rcx                     ; Parm #7 = Arguments, parameter ignored
push rcx                     ; Parm #6 = Size, parameter ignored
push rdx                     ; Parm #5 = Pointer to pointer to allocated buffer
mov ecx,FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM  ; Parm #1 = RCX = Flags
xor edx,edx                  ; Parm #2 = RDX = Message source, ignored
mov r8,rax                   ; Parm #3 = R8  = Message ID, code for translation  
mov r9d,LANG_NEUTRAL         ; Parm #4 = R9  = Language ID
sub rsp,32
call [FormatMessage]
add rsp,32+24
pop rsi                      ; RSI = Updated pointer to allocated buffer
; End of get text string from OS, copy string
mov rcx,rsi
jrcxz .unknown               ; Skip string copy if buffer pointer = null 
test rax,rax
jz .unknown                  ; Skip string copy if output size = 0 
call StringWrite
jmp .release
.unknown:
mov al,'?'
stosb                        ; Write "?" if cannot get string
; Release buffer
.release:
jrcxz .exit                  ; Skip memory release if buffer pointer = null 
sub rsp,32
call [LocalFree]             ; RCX = Pointer to memory block for release
add rsp,32
.exit:
mov rsp,rbp
pop rbp rsi
ret
;------------------------------------------------------------------------------;
;                Registers and memory dump subroutines library:                ;
;                     dump CPU registers and memory areas.                     ;
;------------------------------------------------------------------------------;
;--- Dump 16 64-bit general purpose registers ----------;
; INPUT:   GPR registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpGPR64:
; Save registers for non-volatile and for dump
push rax rbx rcx rdx rsi rdi rbp
lea rax,[rsp + 7*8 + 8]
push rax
push r8 r9 r10 r11 r12 r13 r14 r15
; Initializing dump cycle
cld
mov ebx,16
lea rsi,[NamesGPR64]
lea rbp,[rsp + 15*8 ]
; Dump cycle with 16 Read instructions
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
mov rax,[rbp]
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
sub rbp,8           ; Select next register at stack frame
add rsi,4           ; Select next text string for register name
dec ebx             ; Cycle counter for 16 general-purpose registers
jnz .cycle
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea rcx,[CrLf]
call InternalConsoleWrite
; Restore registers and return
pop r15 r14 r13 r12 r11 r10 r9 r8
pop rax rbp rdi rsi rdx rcx rbx rax
ret
;--- Dump 6 16-bit segment selectors registers ---------;
; INPUT:   Segment selectors registers values for dump  ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpSelectors:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Push 6 selectors
xor eax,eax
mov ax,gs
push rax      ; PUSH #1
mov ax,fs
push rax      ; PUSH #2
mov ax,ss
push rax      ; PUSH #3
mov ax,es
push rax      ; PUSH #4
mov ax,ds
push rax      ; PUSH #5
mov ax,cs
push rax      ; PUSH #6
; Initializing dump cycle
cld
mov ebx,6
lea rsi,[NamesSelectors]
; Dump cycle with pop 6 selectors
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[6-1] 
call HexPrint16
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,3           ; Select next text string for register name
dec ebx             ; Cycle counter for 6 segment selectors registers
jnz .cycle
; Global entry point for return
DumpReturn:
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea rcx,[CrLf]
call InternalConsoleWrite
; Restore registers and return
pop r11 r10 r9 r8 rbp rdi rsi rdx rcx rbx rax
ret
;--- Dump 8 x87 FPU registers --------------------------;
; INPUT:   FPU registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpFPU:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 8 registers
sub rsp,64
fstp qword [rsp + 8*0]
fstp qword [rsp + 8*1]
fstp qword [rsp + 8*2]
fstp qword [rsp + 8*3]
fstp qword [rsp + 8*4]
fstp qword [rsp + 8*5]
fstp qword [rsp + 8*6]
fstp qword [rsp + 8*7]
; Initializing dump cycle
cld
mov ebp,8
lea rsi,[NamesFPU]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[8-1] 
mov bx,0700h
call DoublePrint
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,4           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 FPU selectors registers
jnz .cycle
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 MMX registers ------------------------------;
; INPUT:   MMX registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpMMX:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 8 registers
sub rsp,64
movq [rsp + 8*0],mm0
movq [rsp + 8*1],mm1
movq [rsp + 8*2],mm2
movq [rsp + 8*3],mm3
movq [rsp + 8*4],mm4
movq [rsp + 8*5],mm5
movq [rsp + 8*6],mm6
movq [rsp + 8*7],mm7
; Initializing dump cycle
cld
mov ebp,8
lea rsi,[NamesMMX]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[8-1] 
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,4           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .cycle
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 predicate registers ( AVX512 K0-K7 ) ----------------;
; Variant for minimum AVX512F functionality, 16-bit predicates   ;
; INPUT:   K0-K7 predicate registers values for dump             ;
; OUTPUT:  None                                                  ;
;----------------------------------------------------------------;
DumpPredicates16:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 8 registers
sub rsp,64
kmovw [rsp + 8*0],k0
kmovw [rsp + 8*1],k1
kmovw [rsp + 8*2],k2
kmovw [rsp + 8*3],k3
kmovw [rsp + 8*4],k4
kmovw [rsp + 8*5],k5
kmovw [rsp + 8*6],k6
kmovw [rsp + 8*7],k7
; Initializing dump cycle
cld
mov ebp,8
lea rsi,[NamesK]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[8-1] 
call HexPrint16
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,3           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .cycle
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 predicate registers ( AVX512 K0-K7 ) ----------------;
; Variant for full AVX512BW functionality, 64-bit predicates     ;
; INPUT:   K0-K7 predicate registers values for dump             ;
; OUTPUT:  None                                                  ;
;----------------------------------------------------------------;
DumpPredicates64:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 8 registers
sub rsp,64
kmovq [rsp + 8*0],k0
kmovq [rsp + 8*1],k1
kmovq [rsp + 8*2],k2
kmovq [rsp + 8*3],k3
kmovq [rsp + 8*4],k4
kmovq [rsp + 8*5],k5
kmovq [rsp + 8*6],k6
kmovq [rsp + 8*7],k7
; Initializing dump cycle
cld
mov ebp,8
lea rsi,[NamesK]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov rcx,rsi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
pop rax             ; POP #[8-1] 
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea rcx,[CrLf]
call InternalConsoleWrite
add rsi,3           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .cycle
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 16 SSE registers as hex ----------------------;
; INPUT:   SSE registers values for dump                ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpSSE:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,256
movups [rsp + 16*00],xmm0
movups [rsp + 16*01],xmm1
movups [rsp + 16*02],xmm2
movups [rsp + 16*03],xmm3
movups [rsp + 16*04],xmm4
movups [rsp + 16*05],xmm5
movups [rsp + 16*06],xmm6
movups [rsp + 16*07],xmm7
movups [rsp + 16*08],xmm8
movups [rsp + 16*09],xmm9
movups [rsp + 16*10],xmm10
movups [rsp + 16*11],xmm11
movups [rsp + 16*12],xmm12
movups [rsp + 16*13],xmm13
movups [rsp + 16*14],xmm14
movups [rsp + 16*15],xmm15
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.cycleVectors:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameSSE]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, XMM0-XMM15
cmp ebp,9
ja .formatText
mov al,' '
stosb
.formatText:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
; XMM[i] data frame start 
mov rax,[rsp+00]
call HexPrint64  ; first 64-bit scalar as hex
mov al,' '
stosb
mov rax,[rsp+08] 
call HexPrint64  ; second 64-bit scalar as hex
add rsp,16
; XMM[i] data frame start
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,16
jnz .cycleVectors     ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 16 AVX256 registers as hex -------------------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpAVX256:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,512
vmovupd [rsp + 32*00],ymm0
vmovupd [rsp + 32*01],ymm1
vmovupd [rsp + 32*02],ymm2
vmovupd [rsp + 32*03],ymm3
vmovupd [rsp + 32*04],ymm4
vmovupd [rsp + 32*05],ymm5
vmovupd [rsp + 32*06],ymm6
vmovupd [rsp + 32*07],ymm7
vmovupd [rsp + 32*08],ymm8
vmovupd [rsp + 32*09],ymm9
vmovupd [rsp + 32*10],ymm10
vmovupd [rsp + 32*11],ymm11
vmovupd [rsp + 32*12],ymm12
vmovupd [rsp + 32*13],ymm13
vmovupd [rsp + 32*14],ymm14
vmovupd [rsp + 32*15],ymm15
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX256]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .formatText
mov al,' '
stosb
.formatText:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
; YMM[i] data frame start 
mov rax,[rsp + 24]
call HexPrint64  ; first 64-bit scalar as hex
mov al,' '
stosb
mov rax,[rsp + 16] 
call HexPrint64  ; second 64-bit scalar as hex
mov al,' '
stosb
mov rax,[rsp + 08] 
call HexPrint64  ; third 64-bit scalar as hex
mov al,' '
stosb
mov rax,[rsp + 00] 
call HexPrint64  ; forth 64-bit scalar as hex
add rsp,32
; YMM[i] data frame end
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,16
jnz .cycleVector    ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 16 AVX256 registers as double numbers --------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpAVX256asDouble:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,512
vmovupd [rsp + 32*00],ymm0
vmovupd [rsp + 32*01],ymm1
vmovupd [rsp + 32*02],ymm2
vmovupd [rsp + 32*03],ymm3
vmovupd [rsp + 32*04],ymm4
vmovupd [rsp + 32*05],ymm5
vmovupd [rsp + 32*06],ymm6
vmovupd [rsp + 32*07],ymm7
vmovupd [rsp + 32*08],ymm8
vmovupd [rsp + 32*09],ymm9
vmovupd [rsp + 32*10],ymm10
vmovupd [rsp + 32*11],ymm11
vmovupd [rsp + 32*12],ymm12
vmovupd [rsp + 32*13],ymm13
vmovupd [rsp + 32*14],ymm14
vmovupd [rsp + 32*15],ymm15
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX256]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .formatText
mov al,' '
stosb
.formatText:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov al,' '
stosb
; YMM[i] data frame start 
mov rax,[rsp + 24]
call HelperDoubleDump
mov rax,[rsp + 16] 
call HelperDoubleDump
mov rax,[rsp + 08] 
call HelperDoubleDump
mov rax,[rsp + 00] 
call HelperDoubleDump
add rsp,32
; YMM[i] data frame end
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,16
jnz .cycleVector   ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Helper subroutine for dump ----------------;
; INPUT:   RDI = Pointer to destination buffer  ; 
;          RAX = 64-bit doube number            ;
; OUTPUT:  RDI updated by text data write       ; 
;-----------------------------------------------;
HelperDoubleDump:
push rdi rax
mov ecx,8
mov al,' '
rep stosb
pop rax rdi
push rdi
add rdi,2
mov bx,0200h
push rax
test rax,rax
js .sign
mov al,'+'
stosb
.sign:
pop rax
call DoublePrint
pop rdi
add rdi,7
mov al,' '
cmp byte [rdi],' '
je .exit
mov al,'\'
.exit:
stosb
ret
;--- Dump 32 AVX512 registers as hex -------------------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpAVX512:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,2048
vmovupd [rsp + 64*00],zmm0
vmovupd [rsp + 64*01],zmm1
vmovupd [rsp + 64*02],zmm2
vmovupd [rsp + 64*03],zmm3
vmovupd [rsp + 64*04],zmm4
vmovupd [rsp + 64*05],zmm5
vmovupd [rsp + 64*06],zmm6
vmovupd [rsp + 64*07],zmm7
vmovupd [rsp + 64*08],zmm8
vmovupd [rsp + 64*09],zmm9
vmovupd [rsp + 64*10],zmm10
vmovupd [rsp + 64*11],zmm11
vmovupd [rsp + 64*12],zmm12
vmovupd [rsp + 64*13],zmm13
vmovupd [rsp + 64*14],zmm14
vmovupd [rsp + 64*15],zmm15
vmovupd [rsp + 64*16],zmm16
vmovupd [rsp + 64*17],zmm17
vmovupd [rsp + 64*18],zmm18
vmovupd [rsp + 64*19],zmm19
vmovupd [rsp + 64*20],zmm20
vmovupd [rsp + 64*21],zmm21
vmovupd [rsp + 64*22],zmm22
vmovupd [rsp + 64*23],zmm23
vmovupd [rsp + 64*24],zmm24
vmovupd [rsp + 64*25],zmm25
vmovupd [rsp + 64*26],zmm26
vmovupd [rsp + 64*27],zmm27
vmovupd [rsp + 64*28],zmm28
vmovupd [rsp + 64*29],zmm29
vmovupd [rsp + 64*30],zmm30
vmovupd [rsp + 64*31],zmm31
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX512]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .formatText
mov al,' '
stosb
.formatText:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov rcx,rdi
mov al,' '
stosb
; ZMM[i] data frame start 
mov rax,[rsp + 56]
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 48] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 40] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 32] 
call HexPrint64
lea rsi,[IntervalAVX512]
call StringWrite
mov rax,[rsp + 24]
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 16] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 08] 
call HexPrint64
mov al,' '
stosb
mov rax,[rsp + 00] 
call HexPrint64
add rsp,64
; ZMM[i] data frame end
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,32
jnz .cycleVector   ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 32 AVX512 registers as double numbers --------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpAVX512asDouble:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11
; Store 16 registers
sub rsp,2048
vmovupd [rsp + 64*00],zmm0
vmovupd [rsp + 64*01],zmm1
vmovupd [rsp + 64*02],zmm2
vmovupd [rsp + 64*03],zmm3
vmovupd [rsp + 64*04],zmm4
vmovupd [rsp + 64*05],zmm5
vmovupd [rsp + 64*06],zmm6
vmovupd [rsp + 64*07],zmm7
vmovupd [rsp + 64*08],zmm8
vmovupd [rsp + 64*09],zmm9
vmovupd [rsp + 64*10],zmm10
vmovupd [rsp + 64*11],zmm11
vmovupd [rsp + 64*12],zmm12
vmovupd [rsp + 64*13],zmm13
vmovupd [rsp + 64*14],zmm14
vmovupd [rsp + 64*15],zmm15
vmovupd [rsp + 64*16],zmm16
vmovupd [rsp + 64*17],zmm17
vmovupd [rsp + 64*18],zmm18
vmovupd [rsp + 64*19],zmm19
vmovupd [rsp + 64*20],zmm20
vmovupd [rsp + 64*21],zmm21
vmovupd [rsp + 64*22],zmm22
vmovupd [rsp + 64*23],zmm23
vmovupd [rsp + 64*24],zmm24
vmovupd [rsp + 64*25],zmm25
vmovupd [rsp + 64*26],zmm26
vmovupd [rsp + 64*27],zmm27
vmovupd [rsp + 64*28],zmm28
vmovupd [rsp + 64*29],zmm29
vmovupd [rsp + 64*30],zmm30
vmovupd [rsp + 64*31],zmm31
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea rsi,[NameAVX512]
lea rdi,[TEMP_BUFFER]
call StringWrite
mov eax,ebp
mov bl,0
call DecimalPrint32   ; This number at register name, YMM0-YMM15
cmp ebp,9
ja .formatText
mov al,' '
stosb
.formatText:
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov al,' '
stosb
; ZMM[i] data frame start 
mov rax,[rsp + 56]
call HelperDoubleDump
mov rax,[rsp + 48] 
call HelperDoubleDump
mov rax,[rsp + 40] 
call HelperDoubleDump
mov rax,[rsp + 32] 
call HelperDoubleDump
mov rax,[rsp + 24]
call HelperDoubleDump
mov rax,[rsp + 16] 
call HelperDoubleDump
mov rax,[rsp + 08] 
call HelperDoubleDump
mov rax,[rsp + 00] 
call HelperDoubleDump
add rsp,64
; ZMM[i] data frame end
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,32
jnz .cycleVector             ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump memory region --------------------------------;
; INPUT:   RSI = Pointer to region for dump             ;
;          EBX = Region length, bytes                   ;
;          Memory [RSI] = data for dump                 ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpMemory:
; Push registers include volatile for API
push rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11 r15
xor r15,r15
.cycleDump:
test r15,0Fh
jnz .skipAddressPrint
mov ecx,DUMP_ADDRESS_COLOR
call SetFgColor
lea rdi,[TEMP_BUFFER]
mov eax,r15d
call HexPrint32
mov ax,'  '
stosw
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
.skipAddressPrint:
mov ecx,DUMP_DATA_COLOR
call SetFgColor
mov ecx,16
mov ebp,ebx
push rcx r15
lea rdi,[TEMP_BUFFER]
.cycle16bytes:
dec ebp
js .lineStop
mov al,[rsi+r15]
call HexPrint8
mov al,' '
stosb
jmp .lineDone
.lineStop:
mov ax,'  '
stosw
stosb
.lineDone:
inc r15
loop .cycle16bytes
mov al,' '
stosb
pop r15 rcx
cmp ecx,ebx
jbe .lengthLimited
mov ecx,ebx
.lengthLimited:
.cycleAscii:
mov al,[rsi+r15]
cmp al,' '
jb .belowSpace
cmp al,'z'
jbe .charLimited
.belowSpace:
mov al,'.'
.charLimited:
stosb
inc r15
loop .cycleAscii
mov al,0
stosb
lea rcx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea rcx,[CrLf]
call InternalConsoleWrite
sub ebx,16
ja .cycleDump 
; Go to restore original color, insert empty string, restore registers
pop r15
jmp DumpReturn
;---------- Copy text string terminated by 00h ----------------;
; CR, LF added before string                                   ;
; Spaces added after string                                    ;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   RSI = Source address                                ;
;          RDI = Destination address                           ;
; OUTPUT:  RSI = Modified by copy                              ;
;          RDI = Modified by copy                              ;
;          Memory at [Input RDI] modified                      ;
;--------------------------------------------------------------;
ItemWrite_CRLF:
push rax
cld
mov ax,0A0Dh
stosw             ; CR, LF before string
pop rax
ItemWrite:
push rax
cld
@@:
movsb
cmp byte [rsi],0
jne @b            ; Cycle for copy null-terminated string
inc rsi
mov ax,' ='
stosw             ; " = " after string
stosb
pop rax
ret
;------------------------------------------------------------------------------;
;                  Subroutines for fragment under debug.                       ;
;------------------------------------------------------------------------------;
include 'connect_code.inc'
;------------------------------------------------------------------------------;
;                              Data section.                                   ;
;          Note remember about error if data section exist but empty.          ;     
;------------------------------------------------------------------------------;
section '.data' data readable writeable
;------------------------------------------------------------------------------;
;                Constants located at exe file, part of template.              ;
;           Located before variables for EXE file space minimization.          ;
;------------------------------------------------------------------------------;
; Strings for console output
StartMsg           DB  0Dh, 0Ah, 'Starting...', 0Dh, 0Ah, 0Dh, 0Ah, 0
DoneMsgWait        DB  'Done, press ENTER...', 0
DoneMsgNoWait      DB  'Done.', 0
TitleString        DB  'Hardware Shell v0.22 (x64)', 0
CrLf2              DB  0Dh, 0Ah
CrLf               DB  0Dh, 0Ah, 0
; Names for scenario file and report file
InputName          DB  'input.txt',0
OutputName         DB  'output.txt',0
; Error messages
MsgError           DB  'ERROR: ', 0
MsgErrorTab        DB  '       ', 0
MsgErrorOS         DB  'OS error ', 0
MsgUnknownOption   DB  'Unknown option.', 0 
MsgOption          DB  'Bad option string.', 0
MsgInputSize       DB  'Input scenario file size too big.', 0 
; Options descriptors, for values, controlled by scenario file INPUT.TXT 
OpDesc:
OPTION_KEYS        NameDisplay      , OptionDisplay      , WordDisplay      , KeyOnOff
OPTION_KEYS        NameReport       , OptionReport       , WordReport       , KeyOnOff
OPTION_KEYS        NameWaitkey      , OptionWaitkey      , WordWaitkey      , KeyOnOff
OPTION_HEX_64      NameStartAddress , OptionStartAddress , WordStartAddress
OPTION_HEX_64      NameStopAddress  , OptionStopAddress  , WordStopAddress
OPTION_END
; Options values, controlled by scenario file INPUT.TXT
; Located at constants part, because this variables has pre-defined values
OptionDisplay       DB  1    ; on = console output enabled, off = disabled
OptionReport        DB  1    ; on = save report to file output.txt enabled, off = disabled
OptionWaitkey       DB  1    ; on = wait "Press ENTER" after operation, off = skip this waiting
OptionStartAddress  DQ  00000000FFFFFF00h   ; start address default value
OptionStopAddress   DQ  00000000FFFFFFFFh   ; stop address default value (inclusive)
; Long names for options, used for display and save report with parameters list
NameDisplay         DB  'Display console messages' , 0  
NameReport          DB  'Generate report file'     , 0
NameWaitkey         DB  'Wait key press from user' , 0
NameStartAddress    DB  'Start physical address'   , 0
NameStopAddress     DB  'Stop physical address'    , 0
; Short single word names for options, used for parsing
WordDisplay         DB  'display' , 0
WordReport          DB  'report'  , 0
WordWaitkey         DB  'waitkey' , 0
WordStartAddress    DB  'start'   , 0
WordStopAddress     DB  'stop'    , 0
; Keywords for options
KeyOnOff            DB  'off', 0, 'on', 0, 0
; Memory size and speed units.
U_B                 DB  'Bytes',0
U_KB                DB  'KB',0
U_MB                DB  'MB',0
U_GB                DB  'GB',0
U_TB                DB  'TB',0
U_MBPS              DB  'MBPS',0
U_NS                DB  'nanoseconds',0
; CPU registers names.
NamesGPR64:
DB  'RAX' , 0 
DB  'RBX' , 0
DB  'RCX' , 0
DB  'RDX' , 0
DB  'RSI' , 0
DB  'RDI' , 0
DB  'RBP' , 0
DB  'RSP' , 0
DB  'R8 ' , 0
DB  'R9 ' , 0
DB  'R10' , 0
DB  'R11' , 0
DB  'R12' , 0
DB  'R13' , 0
DB  'R14' , 0
DB  'R15' , 0
NamesSelectors:
DB  'CS' , 0
DB  'DS' , 0
DB  'ES' , 0
DB  'SS' , 0
DB  'FS' , 0
DB  'GS' , 0
NamesFPU:
DB  'ST0' , 0
DB  'ST1' , 0
DB  'ST2' , 0
DB  'ST3' , 0
DB  'ST4' , 0
DB  'ST5' , 0
DB  'ST6' , 0
DB  'ST7' , 0
NamesMMX:
DB  'MM0' , 0
DB  'MM1' , 0
DB  'MM2' , 0
DB  'MM3' , 0
DB  'MM4' , 0
DB  'MM5' , 0
DB  'MM6' , 0
DB  'MM7' , 0
NamesK:
DB  'K0' , 0
DB  'K1' , 0
DB  'K2' , 0
DB  'K3' , 0
DB  'K4' , 0
DB  'K5' , 0
DB  'K6' , 0
DB  'K7' , 0
NameSSE:
DB  'XMM' , 0
NameAVX256:
DB  'YMM' , 0
NameAVX512:
DB  'ZMM' , 0
IntervalAVX512:
DB  0Dh, 0Ah, '      ' , 0
;------------------------------------------------------------------------------;
;           Constants located at exe file, part of code under debug.           ;
;           Located before variables for EXE file space minimization.          ;
;------------------------------------------------------------------------------;
include 'connect_const.inc'
;------------------------------------------------------------------------------;
;       Variables not requires space in the exe file, part of template.        ;
;           Located after constants for EXE file space minimization.           ;
;------------------------------------------------------------------------------;
; Console input, output, report file, scenario file control variables
; IMPORTANT. If change this values layout, update aliases at this file top
Alias_Base:             ; This label used as base point at access aliases 
StdIn           DQ  ?   ; Handle for Input Device ( example = keyboard )
StdOut          DQ  ?   ; Handle for Output Device ( example = display )
ReportName      DQ  ?   ; Pointer to report file name ( example = output.txt )
ReportHandle    DQ  ?   ; Report file dynamically re-created handle, 0=None
ScenarioHandle  DQ  ?   ; Scenario file handle 
ScenarioBase    DQ  ?   ; Scenario file loading base address, 0 = None
ScenarioSize    DQ  ?   ; Scenario file loading size, 0 = None (load error) 
CommandLine     DQ  ?   ; Pointer to command line string
; This 3 variables must be continuous for return status from subroutines 
ErrorPointer1   DQ  ?   ; Pointer to first error description string, 0=none
ErrorPointer2   DQ  ?   ; Pointer to second error description string, 0=none
ErrorCode       DQ  ?   ; WinAPI error code, 0=none    
; Console output support
ScreenInfo  CONSOLE_SCREEN_BUFFER_INFO     ; Console output control structure
; Multifunctional buffer.
align 4096      ; Align by page, actual for Vector brief test 
TEMP_BUFFER     DB  TEMP_BUFFER_SIZE DUP (?)
;------------------------------------------------------------------------------;
;    Variables not requires space in the exe file, part of code under debug.   ;
;           Located after constants for EXE file space minimization.           ;
;------------------------------------------------------------------------------;
include 'connect_var.inc'
;------------------------------------------------------------------------------;
;                              Import section.                                 ;
;------------------------------------------------------------------------------;
section '.idata' import data readable writeable
library user32   , 'USER32.DLL'   , \ 
        kernel32 , 'KERNEL32.DLL' , \
        advapi32 , 'ADVAPI32.DLL'
include 'api\user32.inc'    ; Win API, user interface
include 'api\kernel32.inc'  ; Win API, OS standard functions
include 'api\advapi32.inc'  ; Win API, this used for Kernel Mode Driver load
 