;==============================================================================;
;                                                                              ;
;                        HARDWARE SHELL PROJECT.                               ;
;                                                                              ;
;                  Template for console debug. Win32 edition.                  ; 
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
; FASM definitions for Win32
include 'win32a.inc'
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
DB  XEND   ; ID = 0 = Terminator for list of options descriptors
}
; ID = 1 = means option is list of keywords
macro OPTION_KEYS  x1, x2, x3, x4
{
DB  XKEY   ; ID = 1 = means option is list of keywords
DD  x1     ; Pointer to option long name string, 0-terminated
DD  x2     ; Pointer to option value = byte 
DD  x3     ; Pointer to option single word short name string, for detection
DD  x4     ; Pointer to list of 0-terminated keywords, 0,0 means end of list 
}
; ID = 2 = means 32-bit unsigned value, interpreted as decimal
macro OPTION_DECIMAL_32  x1, x2, x3
{
DB  XDEC   ; ID = 2 = means 32-bit unsigned value, interpreted as decimal
DD  x1     ; Pointer to option long name string, 0-terminated
DD  x2     ; Pointer to option value = dword
DD  x3     ; Pointer to option single word short name string, for detection 
}
; ID = 3 = means 64-bit unsigned value, interpreted as hex
macro OPTION_HEX_64  x1, x2, x3
{
DB  XHEX   ; ID = 3 = means 64-bit unsigned value, interpreted as hex
DD  x1     ; Pointer to option long name string, 0-terminated
DD  x2     ; Pointer to option value = qword
DD  x3     ; Pointer to option single word short name string, for detection 
}
; ID = 3 = means 64-bit unsigned value, interpreted as hex
macro OPTION_SIZE_64  x1, x2, x3
{
DB  XSIZE  ; ID = 3 = means 64-bit unsigned value, interpreted as hex
DD  x1     ; Pointer to option long name string, 0-terminated
DD  x2     ; Pointer to option value = qword
DD  x3     ; Pointer to option single word short name string, for detection 
}
; ID = 5 = means pointer to pointer to string
macro OPTION_STRING  x1, x2, x3
{
DB  XSTR   ; ID = 5 = means pointer to pointer to string
DD  x1     ; Pointer to option long name string, 0-terminated
DD  x2     ; Pointer to option value = pointer to string, 0-terminated
DD  x3     ; Pointer to option single word short name string, for detection 
}
; Support strings formatting and options strings save
OPTION_NAME_FORMAT    EQU  29    ; Formatted output left part before " = " size  
PATH_BUFFER_SIZE      EQU  256   ; Limit for buffers with paths, include last 0
; Aliases for compact access to variables
; Update this required if change variables layout at connect_var.inc
ALIAS_STDIN           EQU  [ebx + 4*00]
ALIAS_STDOUT          EQU  [ebx + 4*01]
ALIAS_REPORTNAME      EQU  [ebx + 4*02]
ALIAS_REPORTHANDLE    EQU  [ebx + 4*03]
ALIAS_SCENARIOHANDLE  EQU  [ebx + 4*04] 
ALIAS_SCENARIOBASE    EQU  [ebx + 4*05]
ALIAS_SCENARIOSIZE    EQU  [ebx + 4*06] 
ALIAS_COMMANDLINE     EQU  [ebx + 4*07]
; This 3 variables must be continuous for return status from subroutines
ALIAS_ERROR_STATUS    EQU  [ebx + 4*08]    
ALIAS_ERROR_P1        EQU  [ebx + 4*08]  ; alias of previous
ALIAS_ERROR_P2        EQU  [ebx + 4*09]
ALIAS_ERROR_C         EQU  [ebx + 4*10]
; Registers and memory dump subroutines support: global used data definitions.
REGISTER_NAME_COLOR   EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY
REGISTER_VALUE_COLOR  EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
DUMP_ADDRESS_COLOR    EQU  FOREGROUND_GREEN + FOREGROUND_INTENSITY
DUMP_DATA_COLOR       EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY  
; Constants for keyboard input check
BLANK_KEY             EQU 00h
ENTER_KEY             EQU 0Dh 
;------------------------------------------------------------------------------;
;                 Source definitions for fragment under debug.                 ;
;------------------------------------------------------------------------------;
include 'include\connect_equ.inc'
;------------------------------------------------------------------------------;
;                               Code section.                                  ;
;------------------------------------------------------------------------------;
format PE console
entry start
section '.text' code readable executable
start:
;------------------------------------------------------------------------------;
;                           Template service code.                             ;
;------------------------------------------------------------------------------;
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
; Start message, only after loading options, possible " display = off "
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
; Verify and correct (if required) start and stop address,
mov eax,dword [OptionStartAddress]
mov ecx,dword [OptionStopAddress]
cmp eax,ecx
jb .skipSwap
xchg eax,ecx
.skipSwap:
mov edx,ecx
sub edx,eax
jz .setLimit
cmp edx,KERNEL_BLOCK_LIMIT
jb .skipLimit
.setLimit:
lea ecx,[eax + KERNEL_BLOCK_LIMIT - 1]
.skipLimit:
mov dword [OptionStartAddress + 0],eax
mov dword [OptionStartAddress + 4],0
mov dword [OptionStopAddress + 0],ecx
mov dword [OptionStopAddress + 4],0
; Show list with options settings
lea ecx,[OpDesc]             ; ECX = Pointers to options descriptors
lea edx,[TEMP_BUFFER]        ; EDX = Pointer to buffer for build text
call ShowScenario
;------------------------------------------------------------------------------;
;                        Code fragment under debug.                            ; 
;------------------------------------------------------------------------------;
lea ecx,ALIAS_ERROR_STATUS      ; Pointer to status variables block
lea edx,[TEMP_BUFFER]           ; Pointer to temporary buffer
call ApplicationKmdShell
test eax,eax                    ; EAX = Status: 0=Error, otherwise no errors
mov ecx,ALIAS_ERROR_P1          ; ECX = Status variables block [0]
mov edx,ALIAS_ERROR_P2          ; EDX = Status variables block [1]
mov eax,ALIAS_ERROR_C           ; EAX  = Status variables block [2]
jz ErrorProgramTripleParm       ; Go if error returned 
;------------------------------------------------------------------------------;
; End of code fragment under debug, continue service code with console output. ; 
;------------------------------------------------------------------------------;
lea ebx,[Alias_Base]            ; EBX = Restore base for variables addressing
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
lea esi,[TEMP_BUFFER]      ; ESI = Non volatile pointer to buffer for char
.waitKey:
mov byte [esi],BLANK_KEY
mov ecx,esi                ; Parm#1 = ECX = Pointer to buffer for char
call ConsoleRead           ; Console input
test eax,eax
jz .skipKey                ; Go skip if input error
cmp byte [esi],ENTER_KEY
jne .waitKey               ; Go repeat if not ENTER key 
.skipKey:
lea ecx,[CrLf2]                 ; ECX = Pointer to 0Dh, 0Ah ( CR, LF )
mov edx,ALIAS_REPORTHANDLE      ; EDX = Output handle
mov esi,ALIAS_REPORTNAME        ; ESI = Pointer to report file name
call ConsoleWrite               ; Console output
;------------------------------------------------------------------------------;
;               Exit application, this point used if no errors.                ;
;------------------------------------------------------------------------------;
ExitProgram:               ; Common entry point for exit to OS
push 0                     ; Parm#1 = Exit code = 0 (no errors)
call [ExitProcess]         ; No return from this function
;------------------------------------------------------------------------------;
;               Error handling and exit application.                           ;
;------------------------------------------------------------------------------;
ErrorProgramSingleParm:    ; Here valid Parm#1 = ECX = Pointer to first string
xor edx,edx                ; Parm#2 = EDX = Pointer to second string, not used 
ErrorProgramDualParm:      ; Here used 2 params: ECX, EDX
xor eax,eax                ; Parm#3 = EAX  = WinAPI error code, not used 
ErrorProgramTripleParm:    ; Here used all 3 params: ECX, EDX, EAX
lea edi,[TEMP_BUFFER]      ; Parm#4 = Pointer to work buffer
call ShowError             ; Show error message
push 1                     ; Parm#1 = Exit code = 1 (error detected)
call [ExitProcess]         ; No return from this function
;------------------------------------------------------------------------------;
;               Helpers subroutines for template service code.                 ;
;------------------------------------------------------------------------------;
;---------- Copy selected text string terminated by 00h -------;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   ESI = Source address                                ;
;          EDI = Destination address                           ;
;          AL  = Selector                                      ;
;          AH  = Limit  (if Selector>Limit, set Selector=0)    ; 
; OUTPUT:  ESI = Modified by copy                              ;
;          EDI = Modified by copy                              ;
;          Memory at [Input EDI] modified                      ;
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
; INPUT:   ESI = Source address                                ;
;          EDI = Destination address                           ;
; OUTPUT:  ESI = Modified by copy                              ;
;          EDI = Modified by copy                              ;
;          Memory at [Input EDI] modified                      ;
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
; INPUT:  EDX:EAX = Number, EDX=High32, EAX=Low32              ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint64:
xchg eax,edx
call HexPrint32
xchg eax,edx
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------------------------;
; INPUT:  EAX = Number                                         ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint32:
push eax
ror eax,16
call HexPrint16
pop eax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------------------------;
; INPUT:  AX  = Number                                         ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
HexPrint16:
push eax
xchg al,ah
call HexPrint8
pop eax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------------------------;
; INPUT:  AL  = Number                                         ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify	                                       ;
;--------------------------------------------------------------;
HexPrint8:
push eax
ror al,4
call HexPrint4
pop eax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------------------------;
; INPUT:  AL  = Number (bits 0-3)                              ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
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
;---------- Print 32-bit Decimal Number -----------------------;
; INPUT:   EAX = Number value                                  ;
;          BL  = Template size, chars. 0=No template           ;
;          EDI = Destination Pointer (flat)                    ;
; OUTPUT:  EDI = New Destination Pointer (flat)                ;
;                modified because string write                 ;
;--------------------------------------------------------------;
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
; Detect special cases for DOUBLE format, yet unsigned indication
test eax,eax
jnz @f                       ; Go if low 32 bits not zero, not a special case
mov ecx,07FFFFFFFh
and ecx,edx                  ; This mask clear sign bit ECX.31 = All number.63
jz .fp64_Zero                ; Go if special cases = 0.0  or  -0.0
cmp ecx,07FF80000h
je .fp64_QNAN                ; Go if special case = QNAN (Quiet Not a Number)
cmp ecx,07FF00000h
je .fp64_INF                 ; Go if special case = INF (Infinity)
ja .fp64_NAN                 ; Go if special case = NAN (Not a Number)
@@:
; Initializing FPU x87
finit
; Change rounding mode from default (nearest) to truncate  
push edx eax   ; save input value
push eax       ; reserve space
fstcw [esp]
pop eax
or ax,0C00h    ; correct Rounding Control, RC = FPU CW bits [11-10]
push eax
fldcw [esp]
pop eax
; Load input value, note rounding mode already changed
fld qword [esp]
pop eax edx
; Separate integer and float parts 
fld st0         ; st0 = value   , st1 = value copy
frndint         ; st0 = integer , st1 = value copy
fxch            ; st0 = value copy , st1 = integer
fsub st0,st1    ; st0 = float , st1 = integer
; Build divisor = f(precision selected) 
mov eax,1
movzx ecx,bh    ; BH = count digits after "."
jecxz .divisorDone
@@:
imul eax,eax,10
loop @b
.divisorDone:
; Build float part as integer number 
push eax
fimul dword [esp]
pop eax
; Extract signed Binary Coded Decimal (BCD) to [esp+00] float part .X 
sub esp,32       ; Make frame for stack variable, used for x87 write data
fbstp [esp+00]   ; Store BCD integer and pop, destination is 80 bit = 10 bytes
; Extract signed Binary Coded Decimal (BCD) to [esp+16], integer part X.
fbstp [esp+16]   ; Store BCD integer and pop, destination is 80 bit = 10 bytes
; Check sign of integer and float part 
test byte [esp+16+09],80h   ; Test bit 79 of 80-bit x87 operand (integer part)
setnz dl                    ; DL = Sign of integer part
test byte [esp+00+09],80h   ; Test bit 79 of 80-bit x87 operand (floating part)
setnz dh                    ; DH = Sign of floating part
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
; Write INTEGER part, note chars # 18,19 not printed 
                         ; CH = 0  = flag "minimum one digit always printed"
mov cx,20                ; CL = 20 = maximum number of digits in the integer part 
mov edx,[esp + 16 + 06]  ; EDX = Integer part BCD , bytes [06-09] = chars [12-19]  
mov esi,[esp + 16 + 02]  ; ESI = bytes [02-05] = chars [04-11] 
mov ebp,[esp + 16 + 00]  ; EBP = bytes [00-01] = chars [00-03] 
shl ebp,16
and edx,07FFFFFFFh   ; clear sign bit
.cycleInteger:   ; Cycle for digits in the INTEGER part
mov eax,edx
shr eax,28       ; AL = current digit, can be 00h-07h for positive only context
cmp cl,1
je .store        ; Go print if last pass, otherwise .X instead 0.X
cmp cl,bl
jbe .store       ; Go print if required by formatting option, BL=count
test ch,ch
jnz .store       ; Go print, if digits sequence already beginned
test al,al
jz .position     ; Otherwise, can go skip print if digit = 0 
.store:
mov ch,1
or al,30h
stosb            ; Write current ASCII digit
.position:
shld edx,esi,4   ; Positioning digits sequence at EBP:ESI:EDX group
shld esi,ebp,4
shl ebp,4
dec cl
jnz .cycleInteger  ; Cycle for digits in the INTEGER part
; Write decimal point
test bh,bh
jz .exit           ; Skip if not print float part
mov al,'.'
stosb
; Write FLOATING part, note chars # 18-23 not printed
std                  ; Write from right to left 
movzx ecx,bh         ; ECX = digits count     
lea edi,[edi+ecx]    ; EDI = After last digit (char) position
mov edx,[esp+00+00]  ; EDX = Floating part BCD , bytes [00-03] = chars [00-07]  
mov esi,[esp+00+04]  ; ESI = bytes [04-07] = chars [08-15] 
mov ebp,[esp+00+00]  ; EBP = bytes [08-11] = chars [16-23] 
push edi
dec edi
.cycleFloat:         ; Cycle for digits in the FLOATING part
mov al,dl
and al,0Fh
or al,30h
stosb
shrd edx,esi,4       ; Positioning digits sequence at EBP:ESI:EDX group
shrd esi,ebp,4
shr ebp,4
loop .cycleFloat     ; Cycle for digits in the FLOATING part
pop edi
cld                  ; Restore strings increment mode
; Go exit subroutine
add esp,32
jmp .exit
; Write strings for different errors types
.fp64_Zero:					; Zero
mov eax,'0.0 '
jmp .fp64special
.fp64_INF:          ; "INF" = Infinity, yet unsigned infinity indicated
mov eax,'INF '
jmp .fp64special
.fp64_NAN:
mov eax,'NAN '      ; "NAN" = (Signaled) Not a number
jmp .fp64special
.fp64_QNAN:
mov eax,'QNAN'      ; "QNAN" = Quiet not a number
.fp64special:
stosd
jmp .exit
.Error:
mov al,'?'
stosb
.exit:
; Exit with re-initialize x87 FPU 
finit
mov [esp],edi
popad
ret
;---------- Print memory block size as Integer.Float -------------------;
; Float part is 1 char, use P1-version of Floating Print                ;
; If rounding precision impossible, print as hex                        ;
; Only x.5 floating values supported, otherwise as hex                  ;
;                                                                       ;
; INPUT:   EDX:EAX = Number value, units = Bytes, EDX=High32, EAX=Low32 ;
;          BL  = Force units (override as smallest only)                ;
;                FF = No force units, auto select                       ;
;                0 = Bytes, 1 = KB, 2 = MB, 3 = GB, 4 = TB              ;
;          EDI = Destination Pointer (flat)                             ;
; OUTPUT:  EDI = New Destination Pointer (flat)                         ;
;                modified because string write                          ;
;-----------------------------------------------------------------------;
; If BL=FFh, auto-select units ( Bytes, KB, MB, GB, TB ) by
; size and mod=0 criteria
; Otherwise print with selected units with 1 digit floating part
SizePrint64:
pushad
cld
; Cycle for units selection
xor ecx,ecx          ; ECX = Units selector
test eax,eax
jnz .unitsAutoCycle
test edx,edx
jz .decimalMode      ; Go if value  = 0
xor ebp,ebp
xor esi,esi
.unitsAutoCycle:
mov ebp,eax          ; EBP = Save previous value
shrd eax,edx,10
shr edx,10
jnz .above32bit      ; Go execute next division if value > 32-bit 
cmp cl,bl
je .modNonZero       ; Go print if override units match
xor esi,esi
shrd esi,ebp,10
shr esi,22           ; ESI = mod
cmp bl,0FFh
jne .above32bit      ; Go skip mod logic if override units mode 
test esi,esi
jnz .modNonZero      ; Go print if mod non-zero
.above32bit:                
inc ecx              ; Units selector + 1
jmp .unitsAutoCycle  ; Make cycle for select optimal units
; Check overflow
.modNonZero:
cmp ecx,4
ja .hexMode          ; Go print hex if units too big
; Print value integer part
mov eax,ebp
.decimalMode:
push ebx
mov bl,0
call DecimalPrint32  ; Print value, integer part
pop ebx
; Pring floating part if override units mode
jecxz .afterNumber   ; Go skip float part if units = bytes
cmp bl,0FFh
je .afterNumber      ; Go skip float part if units = auto
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
call DecimalPrint32         ; Print value, floating part
; Print units
.afterNumber:
mov al,' '
stosb
lea esi,[U_B]
mov al,cl
mov ah,4
call StringWriteSelected    ; Print units
jmp .exit
; Entry point for print as HEX if value too big
.hexMode:
call HexPrint64             ; Print 64-bit hex integer: number of Bytes
mov al,'h'
stosb 
.exit:
mov [esp],edi
popad
ret
;---------- Get console color, saved at start-------------------------------;
; INPUT:  None                                                              ;
; OUTPUT: EAX = Color code                                                  ;
;---------------------------------------------------------------------------;
GetColor:
mov eax,[ScreenInfo.wAttributes]
ret
;---------- Set console color ----------------------------------------------;
; INPUT:   ECX = New color code                                             ;
;          Use global variable [StdOut]                                     ;
; OUTPUT:  EAX = OS Status                                                  ;
;---------------------------------------------------------------------------;
SetColor:
push ecx             ; Parm#2 = Color for set
push [StdOut]        ; Parm#1 = Handle for console output
call [SetConsoleTextAttribute]
ret
;--- Set console foreground color, background color as saved at start ------;
;                                                                           ;
; INPUT:   ECX = New foreground color code                                  ;
;          Use global variable [StdOut]                                     ;
; OUTPUT:  EAX = OS Status                                                  ;
;---------------------------------------------------------------------------;
SetFgColor:
call GetColor         ; Return EAX = default color
and eax,CLEAR_FOREGROUND
and ecx,CLEAR_BACKGROUND
add eax,ecx
push eax              ; Parm#2 = Color for set
push [StdOut]         ; Parm#1 = Handle for console output
call [SetConsoleTextAttribute]
ret
;---------------------- Win32 console functions notes -------------------------;
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
; Echo and edit string mode disabled                                        ;
; Used simplified variant of [ReadConsole], Number of chars to Read = 1     ;
;                                                                           ;
; INPUT:  ECX = Pointer to output buffer, for single char in this variant   ;
;                                                                           ;
; OUTPUT: EAX = Status                                                      ;
;         Buffer at [input ECX] updated.                                    ;
;---------------------------------------------------------------------------;
ConsoleRead:
push ebx esi ebp eax eax      ; EAX = For variables storage
mov ebp,esp                   ; EBP = Pointer to stack frame
mov ebx,[StdIn]               ; EBX = Storage for input device handle
mov esi,ecx                   ; RSI = Non volatile copy of pointer
; Exit with status = OK if input handle = 0, wait key disabled by options
mov eax,1                     ; EAX = Status = OK, if wait key disabled
test ebx,ebx                  ; EBX = Input handle
jz .exit                      ; Skip keyboard input if handle = 0
; Get current console mode
push ebp                      ; Parm#2 = Pointer to output variable 
push ebx                      ; Parm#1 = Input device handle
call [GetConsoleMode]         ; Get current console mode
test eax,eax                  ; EAX = Status, 0 if error
jz .exit                      ; Go exit function if error
; Change current console mode
mov eax,[ebp]                 ; EAX = Console mode
and al,DISABLE_ECHO_ALL       ; Disable echo and string in. (ret. after 1 char)
push eax                      ; Parm#2 = Console mode 
push ebx                      ; Parm#1 = Input device handle
call [SetConsoleMode]         ; Get current console mode
test eax,eax                  ; EAX = Status, 0 if error
jz .exit                      ; Go exit function if error
; Read console ( wait only without echo )
push 0                        ; Parm#5 = InputControl
lea eax,[ebp + 8]
push eax                      ; Parm#4 = Pointer to output var., chars count
push 1                        ; Parm#3 = Number of chars to Read
push esi                      ; Parm#2 = Pointer to input buffer
push ebx                      ; Parm#1 = Input device handle
call [ReadConsole]            ; Keyboard input
; Restore current console mode, use parameters shadow created at entry subr.
push dword [ebp]              ; Parm#2 = Console mode
push ebx                      ; Parm#1 = Input device handle
xchg ebx,eax                  ; EBX = Save error code after input char
call [SetConsoleMode]         ; Set current console mode
; Error code = F( restore, input )
test ebx,ebx                  ; Check status after console input 
setnz bl                      ; BL=0 if input error, BL=1 if input OK
test eax,eax                  ; Check status after restore console mode
setnz al                      ; AL=0 if mode error, AL=1 if mode OK
and al,bl                     ; AL=1 only if both operations status OK
and eax,1                     ; Bit EAX.0 = Valid, bits EAX.[31-1] = 0
; Exit point, EAX = Status actual here
.exit:
pop ecx ecx ebp esi ebx
ret
;---------------------- Win32 console functions notes -------------------------;
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
;---------- String write in ASCII ------------------------------------------;
;                                                                           ;
; INPUT:   ECX = Pointer to 0-terminated ASCII string, string output        ; 
;                to console and optional to report file (if EDX non zero)   ;
;          EDX = Report file handle, used as report validity flag only,     ;
;                report file must be re-opened before write                 ;
;          ESI = Pointer to report file name and path,                      ;
;                0-terminated ASCII string                                  ;
;                                                                           ;
; OUTPUT:  EAX = OS Status                                                  ;
;---------------------------------------------------------------------------;
; This special entry point not required input EDX, ESI
InternalConsoleWrite:
push edx esi
mov edx,[ReportHandle]
mov esi,[ReportName]
call ConsoleWrite
pop esi edx
ret
; This normal entry point required input EDX, ESI
ConsoleWrite:
push ebx esi edi ebp 0        ; push 0 for variable
mov ebp,esp                   ; EBP = Pointer to stack frame
mov esi,ecx                   ; ESI = Non volatile copy of buffer pointer
mov ebx,edx                   ; EBX = Non volatile copy of report handle
; Calculate string length
xor edi,edi                   ; EDI = Number of chars ( length )
@@:
cmp byte [esi + edi],0        ; Check current char from string
je @f                         ; Exit cycle if terminator (byte=0) found
inc edi                       ; Chars counter + 1
jmp @b                        ; Go next iteration
@@:
; Write console - optional
mov eax,1                     ; EAX = Status = OK, if display output disabled
mov ecx,[StdOut]              ; ECX = Parm#1 = Input device handle
jecxz @f                      ; Skip console output if handle = 0
push 0                        ; Parm#5 = Overlapped, not used
push ebp                      ; Parm#4 = Pointer to output variable, count
push edi                      ; Parm#3 = Number of chars ( length )
push esi                      ; Parm#2 = Pointer to string ( buffer )
push ecx                      ; Parm#1 = Input device handle
call [WriteFile]
@@:
; Check criteria for write report file - optional
mov eax,1                     ; EAX = Status = OK, if report save disabled
test ebx,ebx                  ; EBX = Report temp. handle used as flag
jz .exit                      ; Skip file output if handle = 0
cmp ebx,INVALID_HANDLE_VALUE
je .exit                      ; Skip file output if handle = Invalid = -1
mov ecx,[ebp + 12]            ; ECX = Pointer to name string
jecxz .exit                   ; Skip file output if name pointer = 0
; Open
xor eax,eax
push eax                      ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL    ; Parm #6 = File attributes
push OPEN_EXISTING            ; Parm #5 = Creation disposition
push eax                      ; Parm #4 = Security attributes, not used
push eax                      ; Parm #3 = Share mode, not used
push GENERIC_WRITE            ; Parm #2 = Desired access
push ecx                      ; Parm #1 = Pointer to name string
call [CreateFileA]
test eax,eax
jz .exit                      ; Go if open file error
mov ebx,eax                   ; EBX = Save file handle
; Positioning pointer to end of file
push FILE_END                 ; Parm #4 = Move method
push 0                        ; Parm #3 = Position, high dword
push 0                        ; Parm #2 = Position, low dword
push eax                      ; Parm #1 = File handle
call [SetFilePointer]
; Write
.write:
push 0                   ; Parm#5 = Overlapped, not used
push ebp                 ; Parm#4 = Pointer to output variable, count
push edi                 ; Parm#3 = Number of chars ( length )
push esi                 ; Parm#2 = Pointer to string ( buffer )
push ebx                 ; Parm#1 = File handle
call [WriteFile]
mov ecx,[ebp]            ; ECX = Returned size
test eax,eax             ; EAX = status, 0 means error
jz .close                ; Go exit if error
jecxz .close             ; Go exit if returned size = 0
add esi,ecx              ; ESI = advance read pointer by returned size
sub edi,ecx              ; EDI = subtract current read size from size limit
ja .write                ; Repeat read if return size > 0 and limit not reached 
; Close
.close:
test ebx,ebx
jz .exit
push ebx                 ; Parm#1 = Handle
call [CloseHandle]       ; Close report file after write
; Exit point, EAX = Status actual here
.exit:
pop ebp ebp edi esi ebx
ret
;---------- Create report file ---------------------------------------------;
; After this function successfully call, function ConsoleWrite              ;
; starts save output information to report file                             ;
;                                                                           ;
; INPUT:  ECX = Pointer to report file name, 0-terminated ASCII string      ;
;         EDX = Pointer to report file handle, return handle = 0 if error   ;
;                                                                           ;
; OUTPUT: EAX = Status code                                                 ;
;               Variable report handle at [input ECX] =                     ;
;               Temporary handle, used as flag for write report file enable ;
;---------------------------------------------------------------------------;
CreateReport:
push ebx
mov ebx,edx                   ; EBX = Non volatile copy of handle pointer 
; Create file, input parameter RCX = Pointer to file name
xor eax,eax                   ; EAX = 0 for store result = 0 if ReportName = 0
jecxz @f
push eax                      ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL    ; Parm #6 = File attributes
push CREATE_ALWAYS            ; Parm #5 = Creation disposition
push eax                      ; Parm #4 = Security attributes, not used
push eax                      ; Parm #3 = Share mode, not used
push GENERIC_WRITE            ; Parm #2 = Desired access
push ecx                      ; Parm #1 = Pointer to file name
call [CreateFileA]
@@:
; Store result
mov [ebx],eax                 ; EAX = Returned handle
; Close file
test eax,eax
jz @f
push eax                      ; Parm#1 = Handle
call [CloseHandle]
@@:
pop ebx
ret
;---------- Read scenario file ---------------------------------------------;
;                                                                           ;
; INPUT: ECX = Pointer to scenario file path and name,                      ;
;              0-terminated ASCII string                                    ;
;        EDX = Pointer to scenario handle                                   ;
;        ESI = Pointer to loaded scenario base address variable,            ; 
;              this variable is buffer base address for file read           ;
;        EDI = Pointer to scenario size variable,                           ; 
;              this variable is size limit for this buffer                  ;   
;                                                                           ;
; OUTPUT: EAX = OS API last operation status code                           ;
;         Variable scenario handle at [input EDX] = updated by file open    ;
;         Variable scenario size at [input EDI] = Read size, 0 if error     ;
;---------------------------------------------------------------------------;
READ_SIZE   EQU  dword [ebp + 0]
READ_BASE   EQU  dword [ebp + 4]
SIZE_LIMIT  EQU  dword [ebp + 8] 
ReadScenario:
push ebx ebp ebp ebp ebp     ; 3 last pushes for variables
mov ebp,esp
mov ebx,edx                  ; EBX = non volatile pointer to scenario handle
; Open file, by input parameters: ECX = Pointer to file name 
xor eax,eax                  ; EAX = 0 for store result = 0 if ScenarioName = 0
jecxz .error                 ; Skip operation if file name pointer = 0
push eax                     ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL   ; Parm #6 = File attributes
push OPEN_EXISTING           ; Parm #5 = Creation/Open disposition
push eax                     ; Parm #4 = Security attributes, not used
push eax                     ; Parm #3 = Share mode, not used
push GENERIC_READ            ; Parm #2 = Desired access
push ecx                     ; Parm #1 = Pointer to file name
call [CreateFileA]
mov [ebx],eax                ; Save scenario file handle
; Initializing for read file
mov READ_SIZE,0          ; READ_SIZE  = 0, clear read size counter
mov eax,[esi]
mov READ_BASE,eax        ; READ_BASE  = Base address of memory buffer
mov eax,[edi]
mov SIZE_LIMIT,eax       ; SIZE_LIMIT = Size limit of memory buffer
; Read file
.read:
mov ecx,[ebx]            ; ECX = File handle
jecxz .error             ; Skip read and close if handle = 0 (if open error)
xor eax,eax
push eax                 ; This push = space for output variable = dword
mov edx,esp
push eax                 ; Parm #5 = Pointer to overlapped str., not used
push edx                 ; Parm #4 = Pointer to output size
push SIZE_LIMIT          ; R8  = Parm #3 = Buffer size limit
push READ_BASE           ; Parm #2 = Buffer base address for read
push ecx                 ; Parm #1 = File handle
call [ReadFile]
pop ecx                  ; ECX = Output size, EAX = Output status
; Analusing read results
test eax,eax
jz .error                ; Go error if OS status = 0
jecxz .result            ; Go normal read termination if returned size = 0
test ecx,ecx
js .error                ; Go error if size negative, note for 32-bit only
add READ_SIZE,ecx        ; Accumulate read size
add READ_BASE,ecx        ; Advance read pointer by returned size
sub SIZE_LIMIT,ecx       ; Subtract current read size from size limit 
ja .read                 ; Repeat read if return size > 0 and limit not reached 
jb .error   ; Error if read size > size limit, if SIZE_LIMIT = 0, means read OK
; Write result size
.result:
mov eax,READ_SIZE
mov [edi],eax            ; Write scenario size = file size if read OK 
jmp .close
.error:
mov dword [edi],0        ; Write scenario size = 0 if read error
; Close file
.close:
mov ecx,[ebx]            ; ECX = File handle
jecxz .exit
push ecx                 ; Parm #1 = File handle
call [CloseHandle]
.exit:
pop ebp ebp ebp ebp ebx
ret
;---------- Parse scenario file and update options variables ---------------;
;                                                                           ;
; INPUT:   ECX = Pointer to buffer with loaded scenario file                ;  
;          EDX = Limit for this buffer, address of first not-valid byte     ;          
;          ESI = Pointer to options descriptors list                        ;
;          EDI = Pointer to error status variables, for error reporting:    ;
;                3 DWORDS, 2 pointers to strings + 1 OS API error code      ;         
;                                                                           ;
; OUTPUT:  RAX = Status, 0 = error, error status variables valid            ;
;                        1 = no errors, error status variables not used     ;
;          Update options values variables, addressed by descriptors at R8  ;
;          Update status variables, addressed by R9, if error               ;
;                                                                           ;         
;---------------------------------------------------------------------------;
SCENARIO_POINTER  EQU  dword [ebp + 00]  ; Pointer to buffer with scenario file ( R8 at x64 )
SCENARIO_LIMIT    EQU  dword [ebp + 04]  ; Buffer limit, addr. of first not valid byte ( R9 at x64 ) 
ERROR_POINTER     EQU  dword [ebp + 12]  ; Pointer to error status variables, 3 qwords ( R12 at x64 )
OPTIONS_LIST      EQU  dword [ebp + 16]  ; Pointer to options descriptors list ( R11 at x64 )
                                         ; EBX = Dynamical copy of this options pointer ( R10 at x64 )
ParseScenario:
cld
push ebx esi edi ebp edx ecx
mov ebp,esp
; Pre-clear status
xor eax,eax
mov [edi + 00],eax
mov [edi + 04],eax
mov [edi + 08],eax
; This cycle for strings in the scenario
.stringsCycle:
mov ecx,ERROR_POINTER
mov edx,SCENARIO_POINTER
mov [ecx + 04],edx       ; EDX = Pointer to parsed error cause string
mov ebx,OPTIONS_LIST     ; EBX = Reload pointer to options descriptors list 
; This cycle for options descriptors
.optionsCycle:
mov al,[ebx + X0]         ; AL = Option type from option descriptor
cmp al,XEND
je .parseError1           ; Go error if option not found at list, unknown option
cmp al,XLAST
ja .parseError1           ; Go error if option not found at list, unknown option
mov esi,SCENARIO_POINTER  ; ESI = Pointer to text file buffer 
mov edi,[ebx + X3]        ; EDI = Pointer to option keyword
; This cycle for option name word compare
.detectName:
cmp esi,SCENARIO_LIMIT  ; ESI = Pointer to scenario data, compare with limit
jae .parseExitOK        ; Go if scenario done  
mov ah,[edi]            ; AH = Current char from option descriptor, keyword 
inc edi
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
mov al,[ebx + X0]       ; AL = Option type from option descriptor
add ebx,XSMALL
cmp al,XKEY
jne .optionsCycle
add ebx,XDELTA
jmp .optionsCycle
; Option detected, select and run option handler
.detectedThisOption:
mov al,[ebx + X0]       ; AL = Option type from option descriptor 
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
jc .parseError2           ; Go if error detected
; Detect tail, non-informative part of string
.detectTailString:
cmp esi,SCENARIO_LIMIT
jae .parseExitOK         ; Go if scenario done
lodsb                    ; Read current char, scan for end of this string
cmp al,0Ah
je .detectNextString
cmp al,0Dh
jne .detectTailString   
; Step to next string of scenario file 
.detectNextString:
cmp esi,SCENARIO_LIMIT
jae .parseExitOK            ; Go if scenario done
lodsb                       ; Read current char, scan for start next string
cmp al,0Ah
je .detectNextString 
cmp al,0Dh
je .detectNextString 
dec esi
mov SCENARIO_POINTER,esi   ; Address of first char of next string
jmp .stringsCycle
; Error branches
.parseError1:              ; This handler for unknown option keyword
lea eax,[MsgUnknownOption]
jmp .entryError
.parseError2:              ; This handler for errors in option string
lea eax,[MsgOption]
.entryError:
mov ecx,ERROR_POINTER
mov [ecx + 00],eax         ; EAX = Pointer to error comments string
mov edx,[ecx + 04]         ; EDX = Pointer to parsed error cause string
; Terminate error caused string for prevent show all scenario file
mov esi,edx                ; Start scanning end of error cause string
mov edi,SCENARIO_LIMIT
dec edi
.scancrlf:
cmp esi,edi                ; EDI = Loaded scenario file limit in the buffer
jae .limitcrlf             ; Go if scenario file done 
lodsb
cmp al,0Ah
je .foundlf
cmp al,0Dh
.foundlf:
jne .scancrlf
.limitcrlf:
mov byte [esi],0           ; Mark end of string for output error cause string
; Exit points
xor eax,eax   ; Status = error
jmp .parseExit 
.parseExitOK:
mov eax,1     ; Status = no errors
.parseExit:
pop ebp ebp ebp edi esi ebx
ret
;---------- Local subroutine: OPTION_KEYS handler -----------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT = Limit for RSI, address of first not valid byte    ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionKeys:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
mov edi,[ebx + X4]   ; EDI = Patterns pointer , ESI = Scenario pointer
xor ecx,ecx          ; ECX = Possible keywords pointer
mov SCENARIO_POINTER,esi  ; Pointer to keyword in the file
.cycleDecimal:        ; This cycle for select next possible keyword 
mov esi,SCENARIO_POINTER   ; ESI = Restore pointer to keyword in the file
.continueDecimal:    ; This cycle for compare option current keyword
cmp esi,SCENARIO_LIMIT     ;  Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; AL = current char from scenario file, Pointer + 1
mov ah,[edi]         ; AH = current char from comparision pattern 
inc edi              ; Pattern pointer + 1
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
jmp .continueDecimal
.error:              ; Go this if wrong char detected after "="
jmp ParseError
.scanZero:           ; Go this if next possible keyword compare
mov al,[edi]
inc edi
cmp al,0
jne .scanZero
cmp byte [edi],0
je .error            ; Go error if list done but keyword not detected
inc ecx              ; ECX = counter for option value
jmp .cycleDecimal    ; Otherwise, go compare with next possible keyword
.keywordMatch:       ; Go this if keyword match
mov edx,[ebx + X2]   ; EDX = Pointer to option value
mov [edx],cl         ; Write option value, one byte selector
; Global exit points with global-visible labels
ParseOK:           ; Next, return and skip remaining string part
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
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT = Limit for RSI, address of first not valid byte    ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionDecimal32:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; ECX = Numeric value for extract
.cycleDecimal:       ; Cycle for interpreting decimal numeric string
cmp esi,SCENARIO_LIMIT     ; Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb
cmp al,'0'
jb .stopDecimal      ; Go if not a decimal digit char '0'...'9'
cmp al,'9'
ja .stopDecimal      ; Go if not a decimal digit char '0'...'9'
and eax,0Fh          ; Mask for convert '0'...'9' to 0...9
imul ecx,ecx,10      ; Update value, use 64-bit RCX because unsigned required
add ecx,eax          ; Add current value
jmp .cycleDecimal    ; Continue cycle for interpreting decimal numeric string
.stopDecimal:        ; This point for first non-decimal char detected
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
jne ParseError       ; Go error if wrong char after digit
.normalTerm:         ; Otherwise normal termination 
mov edx,[ebx + X2]   ; EDX = Pointer to option value
mov [edx],ecx        ; Write option value, dword, extracted as decimal 
clc                  ; Next, return and skip remaining string part
ret
;---------- Local subroutine: OPTION_HEX_64 handler ---------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT = Limit for RSI, address of first not valid byte    ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionHex64:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; EDI:ECX = Numeric value for extract
xor edi,edi
.cycleHex:           ; Cycle for interpreting hex numeric string
cmp esi,SCENARIO_LIMIT   ; Loaded scenario file limit in the buffer
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
shld edi,ecx,4       ; Shift previous extracted 64-bit value at EDI:ECX
shl ecx,4
or ecx,eax           ; Add current char
jmp .cycleHex        ; Continue cycle for interpreting hex numeric string
.stopHex:            ; This point for first non-hexadecimal char detected
call CheckLineChar   ; Detect 0Ah(LF), 0Dh(CR), 3Bh(';'), 20h(' '), 09h(TAB)
jne ParseError       ; Go error if wrong char after digit
.normalTerm:         ; Otherwise normal termination, store extracted value 
mov edx,[ebx + X2]   ; EDX = Pointer to option value
mov [edx + 0],ecx    ; Write option value, qword, extracted as decimal
mov [edx + 4],edi 
clc                  ; Next, return and skip remaining string part
ret
;---------- Local subroutine: OPTION_SIZE_64 handler --------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT  = Limit for RSI, address of first not valid byte   ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionSize64:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error
xor ecx,ecx          ; EDI:ECX = Numeric value for extract
xor edi,edi
.cycleNumStr:        ; Cycle for interpreting numeric string
cmp esi,SCENARIO_LIMIT        ;  Loaded scenario file limit in the buffer
jae EndOfScenario    ; Go exit if scenario file done 
lodsb                ; Read char from scenario
cmp al,'0'
jb .notadigit        ; Go if not a digit
cmp al,'9'
ja .notadigit        ; Go if not a digit
and eax,0Fh          ; Mask digit, '0'...'9' converted to 0...9
push eax
mov edx,10
call LocalMultiply64
pop eax 
add ecx,eax         ; Add current digit to extracted value
adc edi,0           ; High dword of qwoed
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
mov edx,1024            ; Make kilobytes from accumulated value
jmp .goMultiply
.megabytes:
mov edx,1024*1024       ; Make megabytes from accumulated value
jmp .goMultiply
.gigabytes:
mov edx,1024*1024*1024  ; Make gigabytes from accumulated value 
.goMultiply:
call LocalMultiply64
.nextChar:
lodsb                  ; Get next char after numeric value
cmp esi,SCENARIO_LIMIT      ; R9 = Loaded scenario file limit in the buffer
jae EndOfScenario      ; Go exit if scenario file done 
jmp .notadigit
.normalTerm:           ; Otherwise normal termination, store extracted value 
mov edx,[ebx + X2]     ; EDX = Pointer to option value
mov [edx + 0],ecx      ; Write option value, qword, extracted as decimal
mov [edx + 4],edi      ; High dword of qword 
clc                    ; Next, return and skip remaining string part
ret
;--- Helper for 64-bit multiply ---------------------;
; INPUT:   EDI:ECX = 64-bit value for multiply       ;
;          EDX = Multiplier                          ;
; OUTPUT:  EDI:ECX = Multiplied by EAX               ;
;          EAX, EDX destroyed                        ;
;----------------------------------------------------;
LocalMultiply64:
push edx
xchg eax,edi
mul edx
xchg edi,eax
pop edx
xchg eax,ecx
mul edx
xchg ecx,eax
add edi,edx
ret
;---------- Local subroutine: OPTION_STRING handler ---------------------------;
; INPUT:    ESI = Pointer to scenario file current parse fragment              ;
;           EBX = Pointer to this detected option descriptor                   ;
;           SCENARIO_LIMIT  = Limit for RSI, address of first not valid byte   ;
;           EBP = Pointer to stack frame variables                             ;
; OUTPUT:   SCENARIO_POINTER = Updated pointer to current scenario             ;        
;           CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF  ;
;           ZF flag = special case type, valid if CF = 1                       ;
;           ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached         ;
;------------------------------------------------------------------------------;
OptionString:
call SkipEqual       ; Skip " = " fragment
jc ParseSpecialCase  ; Go if scenario done or parsing error 
mov edi,[ebx + X2]   ; EDI = Pointer to pointer to string
mov edi,[edi]        ; EDI = Pointer to string
mov ecx,PATH_BUFFER_SIZE - 1   ; Limit for string buffer, exclude last 0
.cycle:              ; Cycle for string copy from scenario to buffer
cmp esi,SCENARIO_LIMIT        ; Loaded scenario file limit in the buffer
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
; INPUT:   ESI = Pointer to scenario file current parse fragment               ;
;          SCENARIO_LIMIT = Limit for RSI, address of first not valid byte     ;
;          EBP = Pointer to stack frame variables                              ;
; OUTPUT:  ESI = Updated by skip fragment " = "                                ;
;          CF flag = status, CF=0(NC)=skipped OK, CF=1(C)=spec. case, see ZF   ;
;          ZF flag = special case type, valid if CF = 1                        ;
;          ZF=1(Z)=parse error, ZF=0(NZ)=end of scenario file reached          ;
;------------------------------------------------------------------------------;
SkipEqual:
cmp esi,SCENARIO_LIMIT   ; Check end of file
jae .normal              ; Go exit if end of file
lodsb                    ; AL = current char
cmp al,' '
je SkipEqual    ; Continue skip if SPACE
cmp al,09h
je SkipEqual    ; Continue skip if TAB
cmp al,'='
jne .error 
.cycle:
cmp esi,SCENARIO_LIMIT   ; Check end of file
jae .normal     ; Go exit if end of file
lodsb           ; AL = current char
cmp al,' '
je .cycle       ; Continue skip if SPACE
cmp al,09h
je .cycle       ; Continue skip if TAB
dec esi         ; ESI = Pointer to first char after " = " sequence
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
;                                                                           ;
; INPUT:  ECX = Pointer to options descriptors list                         ;
;         EDX = Pointer to work buffer for prepare text data                ;
;               no limits provided, caller must associate buffer size and   ;
;               text output size, typically additional space available      ;
;                                                                           ;
; OUTPUT: None                                                              ;
;         Use memory at [input EDX]                                         ;
;                                                                           ;         
;---------------------------------------------------------------------------;
ShowScenario:
cld
push ebx esi edi ebp 
; Initializing cycle for show options 
mov ebx,ecx            ; EBX = Pointer to options descriptors list 
mov edi,edx            ; EDI = Pointer to work buffer for prepare text data 
push edi
; Start cycle for show options, build text block in the buffer
.opInterpreting:
mov al,[ebx + X0]      ; AL = Option type from descriptor
cmp al,XEND               
je .opDone             ; Go exit cycle if terminator detected
cmp al,XLAST
ja .opDone             ; Go exit cycle if unknown option code
; Write option name
push eax
mov edx,edi
mov ecx,OPTION_NAME_FORMAT
mov al,' '
rep stosb
xchg edi,edx
mov esi,[ebx + X1]
call StringWrite       ; Write option name, left part of string
mov edi,edx
mov ax,'= '
stosw                  ; Write "= " between left and right parts of string 
pop eax                ; Restore option type, AL = Type
mov esi,[ebx + X2]     ; RSI = Pointer to option value, size is option-specific
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
mov esi,[esi]              ; ESI = Pointer to raw string
call StringWrite           ; Write option value after " = ", raw string
.opInterpretingP25:
add ebx,XSMALL             ; RBX = Pointer, go to next option descriptor
mov ax,0A0Dh
stosw                      ; Make next string, write CR, LF 
jmp .opInterpreting
; Option handler = keys
.opKeys:
mov al,[esi]               ; AL = Index for sequence of 0-terminated strings
mov ah,0FFh
mov esi,[ebx + X4]
call StringWriteSelected   ; Write option value after " = ", selected keyword
add ebx,XBIG               ; RBX = Pointer, go to next option descriptor
mov ax,0A0Dh
stosw                      ; Make next string, write CR, LF 
jmp .opInterpreting
; Option handler = decimal 32
.opDecimal32:
mov eax,[esi]              ; EAX = Value for visual as 32-bit decimal number
push ebx
mov bl,0                   ; BL = Template for print
call DecimalPrint32        ; Write option value after " = ", decimal number
pop ebx
jmp .opInterpretingP25
; Option handler = hex 64
.opHex64:
mov eax,[esi + 0]    ; EAX = Value for visual as 64-bit hex number
mov edx,[esi + 4]
call HexPrint64      ; Write option value after " = ", hex number
mov al,'h'
stosb
jmp .opInterpretingP25
; Option handler = size 64
.opSize64:
mov eax,[esi + 0]    ; EDX:EAX = Value for visual as 64-bit size, can use K/M/G
mov edx,[esi + 4]
push ebx
mov bl,0FFh
call SizePrint64     ; Write option value after " = ", size
pop ebx
jmp .opInterpretingP25
; Termination
.opDone:
mov ax,0A0Dh
stosw                ; Make next string, write CR, LF 
mov al,0
stosb                ; Terminate all sequence of strings, write 0 byte
pop ecx
; Read data from prepared buffer and display to console, 
; optionally save to report file
call InternalConsoleWrite
pop ebp edi esi ebx 
ret
;---------- Show details about detected error and wait key press -----------;
;                                                                           ;
; INPUT:  ECX = Pointer to error description first string, 0 means skip     ;
;         EDX = Pointer to error description second string, 0 means skip    ;
;         EAX = Windows style error code for decoding by WinAPI and         ;
;               show string "<Error name> (code)", 0 means skip             ;
;         EDI = Pointer to work ( transit ) buffer for prepare text data    ;
;               no limits provided, caller must associate buffer size and   ;
;               text output size, typically additional space available      ;
;                                                                           ;
; OUTPUT: None                                                              ;
;         Use memory at [input EDI]                                         ;
;                                                                           ;         
;---------------------------------------------------------------------------;
ShowError:
cld
push ebx esi edi ebp eax edx ecx
mov ebp,esp 
; Set color and write "Error: " message part
mov ecx,FOREGROUND_RED + FOREGROUND_INTENSITY
call SetFgColor                 ; Color for "Error: " message part
lea ecx,[MsgError]
call InternalConsoleWrite
; Set color and conditionally write first string
mov ecx, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY
call SetFgColor
mov ecx,[ebp + 00]              ; ECX = Input ECX = string 1
jecxz @f                        ; Go skip if string pointer = 0
call InternalConsoleWrite       ; First string about error
@@:
; Conditionally write second string with alignment for "Error: " part
mov ebx,[ebp + 04]              ; EBX = Input EDX = string 2
test ebx,ebx
jz @f                           ; Go skip if string pointer = 0
lea ecx,[CrLf]
call InternalConsoleWrite       ; Next string
lea ecx,[MsgErrorTab]
call InternalConsoleWrite       ; Tabulation for alignment for "Error: " part
mov ecx,ebx
call InternalConsoleWrite       ; Second string about error
@@:
; Conditionally write third string with alignment for "Error: " part
mov ebx,[ebp + 08]              ; EBX = Input EAX = WinAPI error code
test ebx,ebx
jz @f                           ; Go skip if error code = 0
lea ecx,[CrLf]
call InternalConsoleWrite       ; Next string
lea ecx,[MsgErrorTab]
call InternalConsoleWrite       ; Tabulation for alignment for "Error: " part 
lea esi,[MsgErrorOS]            ; ESI = Pointer to string, EDI = To buffer
call StringWrite                ; Write "OS error" to buffer
xchg eax,ebx                    ; EAX = WinAPI error code
mov bl,0                        ; BL  = Numeric template control
call DecimalPrint32             ; Write error code decimal number to buffer
mov ax,' ='
stosw
stosb
mov eax,[ebp + 08]              ; EBX = Input EAX = WinAPI error code
call DecodeError                ; Write OS error description string to buffer
mov al,0
stosb
mov ecx,[ebp + 16]              ; ECX = Input EDI = buffer pointer
call InternalConsoleWrite       ; Write from buffer to console 
@@:
; Restore console color, skip string and write done message "Press ENTER..."
call GetColor
xchg ecx,eax
call SetColor                    ; Restore original color
lea ecx,[CrLf2]
call InternalConsoleWrite
lea ecx,[DoneMsgNoWait]          ; ECX = Pointer to message 1
cmp [OptionWaitkey],0
je  @f
lea ecx,[DoneMsgWait]            ; ECX = Pointer to message 2
@@:
call InternalConsoleWrite
; Wait key press, after key pressed skip string
lea esi,[TEMP_BUFFER]      ; ESI = Non volatile pointer to buffer for char
.waitKey:
mov byte [esi],BLANK_KEY
mov ecx,esi                ; Parm#1 = ECX = Pointer to buffer for char
call ConsoleRead           ; Console input
test eax,eax
jz .skipKey                ; Go skip if input error
cmp byte [esi],ENTER_KEY
jne .waitKey               ; Go repeat if not ENTER key 
.skipKey:
lea ecx,[CrLf2]
call InternalConsoleWrite
pop ebp ebp ebp ebp edi esi ebx 
ret
;---------- Translation error code to error name string -------;
;                                                              ;
; INPUT:   EAX = Error code for translation                    ;
;          EDI = Destination address for build text string     ;
;                                                              ;
; OUTPUT:  EDI = Modified by string write                      ;
;          Memory at [Input EDI] = output string               ;
;                                  not 0-terminated            ;
;--------------------------------------------------------------;
DecodeError:
push esi
; Get text string from OS
xor ecx,ecx
push ecx               ; Pointer to dynamically allocated buffer
mov edx,esp
push ecx               ; Parm #7 = Arguments, parameter ignored
push ecx               ; Parm #6 = Size, parameter ignored
push edx               ; Parm #5 = Pointer to pointer to allocated buffer
push LANG_NEUTRAL      ; Parm #4 = Language ID
push eax               ; Parm #3 = Message ID, code for translation
push ecx               ; Parm #2 = Message source, ignored
push FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM  ; Parm #1 = Flags
call [FormatMessage]
pop esi                ; ESI = Updated pointer to allocated buffer
; End of get text string from OS, copy string
mov ecx,esi
jecxz .unknown         ; Skip string copy if buffer pointer = null 
test eax,eax
jz .unknown            ; Skip string copy if output size = 0 
call StringWrite
jmp .release
.unknown:
mov al,'?'
stosb                  ; Write "?" if cannot get string
; Release buffer
.release:
jecxz .exit            ; Skip memory release if buffer pointer = null
push ecx               ; Parm#1 = Pointer to memory block for release 
call [LocalFree]
.exit:
pop esi
ret
;------------------------------------------------------------------------------;
;                Registers and memory dump subroutines library:                ;
;             connect include files with globally used subroutines.            ; 
;------------------------------------------------------------------------------;
;--- Dump 8 32-bit general purpose registers ----;
; INPUT:   GPR registers values for dump         ;
; OUTPUT:  None                                  ; 
;------------------------------------------------;
DumpGPR32:
; Save registers for non-volatile and for dump
push eax ebx ecx edx esi edi ebp
lea eax,[esp + 7*4 + 4]
push eax
; Initializing dump cycle
cld
mov ebx,8
lea esi,[NamesGPR32]
lea ebp,[esp + 7*4]
; Dump cycle with 8 Read instructions
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
mov eax,[ebp]
call HexPrint32
mov al,0
stosb
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
sub ebp,4           ; Select next register at stack frame
add esi,4           ; Select next text string for register name
dec ebx             ; Cycle counter for 16 general-purpose registers
jnz .cycle
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea ecx,[CrLf]
call InternalConsoleWrite
; Restore registers and return
pop eax ebp edi esi edx ecx ebx eax
ret
;--- Dump 6 16-bit segment selectors registers ---------;
; INPUT:   Segment selectors registers values for dump  ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpSelectors:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Push 6 selectors
xor eax,eax
mov ax,gs
push eax      ; PUSH #1
mov ax,fs
push eax      ; PUSH #2
mov ax,ss
push eax      ; PUSH #3
mov ax,es
push eax      ; PUSH #4
mov ax,ds
push eax      ; PUSH #5
mov ax,cs
push eax      ; PUSH #6
; Initializing dump cycle
cld
mov ebx,6
lea esi,[NamesSelectors]
; Dump cycle with pop 6 selectors
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax             ; POP #[6-1] 
call HexPrint16
mov al,0
stosb
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
add esi,3           ; Select next text string for register name
dec ebx             ; Cycle counter for 6 segment selectors registers
jnz .cycle
; Entry point for return
DumpReturn:
; Restore original color
call GetColor
xchg ecx,eax
call SetColor
; Insert empty string
lea ecx,[CrLf]
call InternalConsoleWrite
; Restore registers and return
pop ebp edi esi edx ecx ebx eax
ret
;--- Dump 8 x87 FPU registers --------------------------;
; INPUT:   FPU registers values for dump                ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpFPU:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 8 registers
sub esp,64
fstp qword [esp + 8*0]
fstp qword [esp + 8*1]
fstp qword [esp + 8*2]
fstp qword [esp + 8*3]
fstp qword [esp + 8*4]
fstp qword [esp + 8*5]
fstp qword [esp + 8*6]
fstp qword [esp + 8*7]
; Initializing dump cycle
cld
mov ebp,8
lea esi,[NamesFPU]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax edx         ; POP #[8-1] 
mov bx,0700h
call DoublePrint
mov al,0
stosb
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
add esi,4           ; Select next text string for register name
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
push eax ebx ecx edx esi edi ebp
; Store 8 registers
sub esp,64
movq [esp + 8*0],mm0
movq [esp + 8*1],mm1
movq [esp + 8*2],mm2
movq [esp + 8*3],mm3
movq [esp + 8*4],mm4
movq [esp + 8*5],mm5
movq [esp + 8*6],mm6
movq [esp + 8*7],mm7
; Initializing dump cycle
cld
mov ebp,8
lea esi,[NamesMMX]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax edx         ; POP #[8-1] 
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
add esi,4           ; Select next text string for register name
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
push eax ebx ecx edx esi edi ebp
; Store 8 registers
sub esp,32
kmovw [esp + 4*0],k0
kmovw [esp + 4*1],k1
kmovw [esp + 4*2],k2
kmovw [esp + 4*3],k3
kmovw [esp + 4*4],k4
kmovw [esp + 4*5],k5
kmovw [esp + 4*6],k6
kmovw [esp + 4*7],k7
; Initializing dump cycle
cld
mov ebp,8
lea esi,[NamesK]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax             ; POP #[8-1] 
call HexPrint16
mov al,0
stosb
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
add esi,3           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .cycle
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 predicate registers ( AVX512 K0-K7 ) ----------------;
; Variant for full AVX512BW functionality, 54-bit predicates     ;
; INPUT:   K0-K7 predicate registers values for dump             ;
; OUTPUT:  None                                                  ;
;----------------------------------------------------------------;
DumpPredicates64:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 8 registers
sub esp,64
kmovq [esp + 8*0],k0
kmovq [esp + 8*1],k1
kmovq [esp + 8*2],k2
kmovq [esp + 8*3],k3
kmovq [esp + 8*4],k4
kmovq [esp + 8*5],k5
kmovq [esp + 8*6],k6
kmovq [esp + 8*7],k7
; Initializing dump cycle
cld
mov ebp,8
lea esi,[NamesK]
; Dump cycle with pop 8 registers
.cycle:
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
mov ecx,esi
call InternalConsoleWrite 
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
pop eax edx         ; POP #[8-1] 
call HexPrint64
mov al,0
stosb
call InternalConsoleWrite
lea ecx,[CrLf]
call InternalConsoleWrite
add esi,3           ; Select next text string for register name
dec ebp             ; Cycle counter for 8 MMX registers
jnz .cycle
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 SSE registers as hex -----------------------;
; INPUT:   SSE registers values for dump                ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpSSE:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,128
movups [esp + 16*00],xmm0
movups [esp + 16*01],xmm1
movups [esp + 16*02],xmm2
movups [esp + 16*03],xmm3
movups [esp + 16*04],xmm4
movups [esp + 16*05],xmm5
movups [esp + 16*06],xmm6
movups [esp + 16*07],xmm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameSSE]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
; XMM[i] data frame start 
mov eax,[esp+00]
mov edx,[esp+04]
call HexPrint64
mov al,' '
stosb
mov eax,[esp+08]
mov edx,[esp+12] 
call HexPrint64
add esp,16
; XMM[i] data frame start
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea ecx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,8
jnz .cycleVector   ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 AVX256 registers as hex --------------------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpAVX256:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,256
vmovupd [esp + 32*00],ymm0
vmovupd [esp + 32*01],ymm1
vmovupd [esp + 32*02],ymm2
vmovupd [esp + 32*03],ymm3
vmovupd [esp + 32*04],ymm4
vmovupd [esp + 32*05],ymm5
vmovupd [esp + 32*06],ymm6
vmovupd [esp + 32*07],ymm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX256]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
; YMM[i] data frame start 
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 16]
mov edx,[esp + 16 + 4] 
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 08]
mov edx,[esp + 08 + 4] 
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 00]
mov edx,[esp + 00 + 4] 
call HexPrint64
add esp,32
; YMM[i] data frame end
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea ecx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,8
jnz .cycleVector    ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 AVX256 registers as double numbers ---------;
; INPUT:   AVX256 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpAVX256asDouble:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,256
vmovupd [esp + 32*00],ymm0
vmovupd [esp + 32*01],ymm1
vmovupd [esp + 32*02],ymm2
vmovupd [esp + 32*03],ymm3
vmovupd [esp + 32*04],ymm4
vmovupd [esp + 32*05],ymm5
vmovupd [esp + 32*06],ymm6
vmovupd [esp + 32*07],ymm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 16 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX256]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov al,' '
stosb
; YMM[i] data frame start 
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HelperDoubleDump
mov eax,[esp + 16]
mov edx,[esp + 16 + 4] 
call HelperDoubleDump
mov eax,[esp + 08]
mov edx,[esp + 08 + 4] 
call HelperDoubleDump
mov eax,[esp + 00]
mov edx,[esp + 00 + 4] 
call HelperDoubleDump
add esp,32
; YMM[i] data frame end
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea ecx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,8
jnz .cycleVector   ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Helper subroutine for dump --------------------;
; INPUT:   EDI = Pointer to destination buffer      ; 
;          EDX:EAX = 64-bit doube number            ;
; OUTPUT:  EDI updated by text data write           ; 
;---------------------------------------------------;
HelperDoubleDump:
push edi eax
mov ecx,8
mov al,' '
rep stosb
pop eax edi
push edi
add edi,2
mov bx,0200h
push eax
test edx,edx
js .sign
mov al,'+'
stosb
.sign:
pop eax
call DoublePrint
pop edi
add edi,7
mov al,' '
cmp byte [edi],' '
je .exit
mov al,'\'
.exit:
stosb
ret
;--- Dump 8 AVX512 registers as hex --------------------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpAVX512:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,512
vmovupd [esp + 64*00],zmm0
vmovupd [esp + 64*01],zmm1
vmovupd [esp + 64*02],zmm2
vmovupd [esp + 64*03],zmm3
vmovupd [esp + 64*04],zmm4
vmovupd [esp + 64*05],zmm5
vmovupd [esp + 64*06],zmm6
vmovupd [esp + 64*07],zmm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX512]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov ecx,edi
mov al,' '
stosb
; ZMM[i] data frame start 
mov eax,[esp + 56]
mov edx,[esp + 56 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 48]
mov edx,[esp + 48 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 40]
mov edx,[esp + 40 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 32]
mov edx,[esp + 32 + 4]
call HexPrint64
lea esi,[IntervalAVX512]
call StringWrite
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 16]
mov edx,[esp + 16 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 08]
mov edx,[esp + 08 + 4]
call HexPrint64
mov al,' '
stosb
mov eax,[esp + 00]
mov edx,[esp + 00 + 4]
call HexPrint64
add esp,64
; ZMM[i] data frame end
mov al,0
stosb
call InternalConsoleWrite
; Cycle
lea ecx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,8
jnz .cycleVector    ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump 8 AVX512 registers as double numbers ---------;
; INPUT:   AVX512 registers values for dump             ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpAVX512asDouble:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
; Store 16 registers
sub esp,512
vmovupd [esp + 64*00],zmm0
vmovupd [esp + 64*01],zmm1
vmovupd [esp + 64*02],zmm2
vmovupd [esp + 64*03],zmm3
vmovupd [esp + 64*04],zmm4
vmovupd [esp + 64*05],zmm5
vmovupd [esp + 64*06],zmm6
vmovupd [esp + 64*07],zmm7
; Initializing dump cycle
cld
xor ebp,ebp
; Dump cycle with pop 32 registers
.cycleVector:
; Register name
mov ecx,REGISTER_NAME_COLOR
call SetFgColor
lea esi,[NameAVX512]
lea edi,[TEMP_BUFFER]
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
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Register value
mov ecx,REGISTER_VALUE_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
mov al,' '
stosb
; ZMM[i] data frame start 
mov eax,[esp + 56]
mov edx,[esp + 56 + 4]
call HelperDoubleDump
mov eax,[esp + 48]
mov edx,[esp + 48 + 4]
call HelperDoubleDump
mov eax,[esp + 40]
mov edx,[esp + 40 + 4]
call HelperDoubleDump
mov eax,[esp + 32]
mov edx,[esp + 32 + 4]
call HelperDoubleDump
mov eax,[esp + 24]
mov edx,[esp + 24 + 4]
call HelperDoubleDump
mov eax,[esp + 16]
mov edx,[esp + 16 + 4]
call HelperDoubleDump
mov eax,[esp + 08]
mov edx,[esp + 08 + 4]
call HelperDoubleDump
mov eax,[esp + 00]
mov edx,[esp + 00 + 4]
call HelperDoubleDump
add esp,64
; ZMM[i] data frame end
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea ecx,[CrLf]
call InternalConsoleWrite
inc ebp
cmp ebp,8
jnz .cycleVector      ; Cycle counter for 16 SSE registers
; Go to restore original color, insert empty string, restore registers
jmp DumpReturn
;--- Dump memory region, show absolute 32-bit address --;
; INPUT:   ESI = Pointer to region for dump             ;
;          EBX = Region length, bytes                   ;
;          EAX = Value for print block absolute address ;
;          Memory [ESI] = data for dump                 ;
; OUTPUT:  None                                         ;
;-------------------------------------------------------;
DumpMemoryAbsolute:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
push 1 eax
jmp DumpMemoryEntry
;--- Dump memory region --------------------------------;
; INPUT:   ESI = Pointer to region for dump             ;
;          EBX = Region length, bytes                   ;
;          Memory [ESI] = data for dump                 ;
; OUTPUT:  None                                         ; 
;-------------------------------------------------------;
DumpMemory:
; Push registers include volatile for API
push eax ebx ecx edx esi edi ebp
push 0 0
DumpMemoryEntry:
push 0
.cycleDump:
test dword [esp],0Fh
jnz .skipAddressPrint
mov ecx,DUMP_ADDRESS_COLOR
call SetFgColor
lea edi,[TEMP_BUFFER]
;--- address print ---
cmp dword [esp + 8],0
jne .mem32
mov eax,[esp]
call HexPrint32
jmp .memDone
.mem32:
mov eax,[esp + 04]
call HexPrint32
.memDone:
;--- address print done ---
mov ax,'  '
stosw
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
.skipAddressPrint:
mov ecx,DUMP_DATA_COLOR
call SetFgColor
mov ecx,16
mov ebp,ebx
push dword [esp] ecx 
lea edi,[TEMP_BUFFER]
.cycle16bytes:
dec ebp
js .lineStop
mov eax,dword [esp + 4]
mov al,[esi + eax]
call HexPrint8
mov al,' '
stosb
jmp .lineDone
.lineStop:
mov ax,'  '
stosw
stosb
.lineDone:
inc dword [esp + 4]
loop .cycle16bytes
mov al,' '
stosb
pop ecx eax
cmp ecx,ebx
jbe .lengthLimited
mov ecx,ebx
.lengthLimited:
.cycleAscii:
mov eax,dword [esp]
mov al,[esi + eax]
cmp al,' '
jb .belowSpace
cmp al,'z'
jbe .charLimited
.belowSpace:
mov al,'.'
.charLimited:
stosb
inc dword [esp]
inc dword [esp + 4]
loop .cycleAscii
mov al,0
stosb
lea ecx,[TEMP_BUFFER]
call InternalConsoleWrite
; Cycle
lea ecx,[CrLf]
call InternalConsoleWrite
sub ebx,16
ja .cycleDump 
; Go to restore original color, insert empty string, restore registers
pop eax eax eax
jmp DumpReturn
;---------- Copy text string terminated by 00h ----------------;
; CR, LF added before string                                   ;
; Spaces added after string                                    ;
; Note last byte 00h not copied                                ;
;                                                              ;
; INPUT:   ESI = Source address                                ;
;          EDI = Destination address                           ;
; OUTPUT:  ESI = Modified by copy                              ;
;          EDI = Modified by copy                              ;
;          Memory at [Input EDI] modified                      ;
;--------------------------------------------------------------;
ItemWrite_CRLF:
push eax
cld
mov ax,0A0Dh
stosw             ; CR, LF before string
pop eax
ItemWrite:
push eax
cld
@@:
movsb
cmp byte [esi],0
jne @b            ; Cycle for copy null-terminated string
inc esi
mov ax,' ='
stosw             ; " = " after string
stosb
pop eax
ret
;------------------------------------------------------------------------------;
;                  Subroutines for fragment under debug.                       ;
;------------------------------------------------------------------------------;
include 'include\connect_code.inc'
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
TitleString        DB  'Ring 0 test. v0.24 (ia32)', 0
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
OPTION_KEYS        NameAction       , OptionAction       , WordAction       , KeyAction
OPTION_HEX_64      NameStartAddress , OptionStartAddress , WordStartAddress
OPTION_HEX_64      NameStopAddress  , OptionStopAddress  , WordStopAddress
OPTION_END
; Options values, controlled by scenario file INPUT.TXT
; Located at constants part, because this variables has pre-defined values
OptionDisplay       DB  1    ; on = console output enabled, off = disabled
OptionReport        DB  1    ; on = save report to file output.txt enabled, off = disabled
OptionWaitkey       DB  1    ; on = wait "Press ENTER" after operation, off = skip this waiting
OptionAction        DB  0    ; operation select, system object for read
OptionStartAddress  DQ  00000000FFFFFF00h   ; start address default value
OptionStopAddress   DQ  00000000FFFFFFFFh   ; stop address default value (inclusive)
; Long names for options, used for display and save report with parameters list
NameDisplay         DB  'Display console messages' , 0  
NameReport          DB  'Generate report file'     , 0
NameWaitkey         DB  'Wait key press from user' , 0
NameAction          DB  'Action'                   , 0
NameStartAddress    DB  'Start physical address'   , 0
NameStopAddress     DB  'Stop physical address'    , 0
; Short single word names for options, used for parsing
WordDisplay         DB  'display' , 0
WordReport          DB  'report'  , 0
WordWaitkey         DB  'waitkey' , 0
WordAction          DB  'action'  , 0
WordStartAddress    DB  'start'   , 0
WordStopAddress     DB  'stop'    , 0
; Keywords for options
KeyOnOff            DB  'off', 0, 'on', 0, 0
KeyAction           DB  'memory' , 0 , 'io'   , 0 , 'pci'  , 0 , 'pcimcfg', 0
                    DB  'crmsr'  , 0 , 'cmos' , 0 , 'apic' , 0
                    DB  'spdsmbus' , 0 , 'clksmbus' , 0, 0  
; Memory size and speed units.
U_B                 DB  'Bytes',0
U_KB                DB  'KB',0
U_MB                DB  'MB',0
U_GB                DB  'GB',0
U_TB                DB  'TB',0
U_MBPS              DB  'MBPS',0
U_NS                DB  'nanoseconds',0
; CPU registers names.
NamesGPR32:
DB  'EAX' , 0 
DB  'EBX' , 0
DB  'ECX' , 0
DB  'EDX' , 0
DB  'ESI' , 0
DB  'EDI' , 0
DB  'EBP' , 0
DB  'ESP' , 0
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
include 'include\connect_const.inc'
;------------------------------------------------------------------------------;
;       Variables not requires space in the exe file, part of template.        ;
;           Located after constants for EXE file space minimization.           ;
;------------------------------------------------------------------------------;
; Console input, output, report file, scenario file control variables
; IMPORTANT. If change this values layout, update aliases at this file top
Alias_Base:             ; This label used as base point at access aliases 
StdIn           DD  ?   ; Handle for Input Device ( example = keyboard )
StdOut          DD  ?   ; Handle for Output Device ( example = display )
ReportName      DD  ?   ; Pointer to report file name ( example = output.txt )
ReportHandle    DD  ?   ; Report file dynamically re-created handle, 0=None
ScenarioHandle  DD  ?   ; Scenario file handle 
ScenarioBase    DD  ?   ; Scenario file loading base address, 0 = None
ScenarioSize    DD  ?   ; Scenario file loading size, 0 = None (load error) 
CommandLine     DD  ?   ; Pointer to command line string
; This 3 variables must be continuous for return status from subroutines 
ErrorPointer1   DD  ?   ; Pointer to first error description string, 0=none
ErrorPointer2   DD  ?   ; Pointer to second error description string, 0=none
ErrorCode       DD  ?   ; WinAPI error code, 0=none    
; Console output support
ScreenInfo  CONSOLE_SCREEN_BUFFER_INFO     ; Console output control structure
; Multifunctional buffer.
align 4096      ; Align by page, actual for Vector brief test 
TEMP_BUFFER     DB  TEMP_BUFFER_SIZE DUP (?)
;------------------------------------------------------------------------------;
;    Variables not requires space in the exe file, part of code under debug.   ;
;           Located after constants for EXE file space minimization.           ;
;------------------------------------------------------------------------------;
include 'include\connect_var.inc'
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
