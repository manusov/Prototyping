;------------------------------------------------------------------------------;
;                         Console support library:                             ;
;                       global used data definitions.                          ; 
;------------------------------------------------------------------------------;

; Color masks

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

; Console data structures definition
; Char coordinates

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

; Options descriptors

XEND    EQU  0      ; Types of descriptors
XKEY    EQU  1 
XDEC    EQU  2
XHEX    EQU  3
XSIZE   EQU  4
XSTR    EQU  5
XLAST   EQU  5

X0      EQU  0      ; Offsets of descriptors fields
X1      EQU  1
X2      EQU  5
X3      EQU  9
X4      EQU  13

XBIG    EQU  17     ; Addends for addressing descriptors sequence
XSMALL  EQU  13
XDELTA  EQU  XBIG - XSMALL

macro OPTION_END
{
DB  XEND         ; ID = 0 = Terminator for list of options descriptors
}

macro OPTION_KEYS  x1, x2, x3, x4
{
DB  XKEY         ; ID = 1 = means option is list of keywords
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = byte 
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection
DD  x4 - OpDesc  ; Pointer to list of 0-terminated keywords, 0,0 means end of list 
}

macro OPTION_DECIMAL_32  x1, x2, x3
{
DB  XDEC         ; ID = 2 = means 32-bit unsigned value, interpreted as decimal
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = dword
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}

macro OPTION_HEX_64  x1, x2, x3
{
DB  XHEX         ; ID = 3 = means 64-bit unsigned value, interpreted as hex
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = qword
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}

macro OPTION_SIZE_64  x1, x2, x3
{
DB  XSIZE        ; ID = 3 = means 64-bit unsigned value, interpreted as hex
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = qword
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}

macro OPTION_STRING  x1, x2, x3
{
DB  XSTR         ; ID = 5 = means pointer to pointer to string
DD  x1 - OpDesc  ; Pointer to option long name string, 0-terminated
DD  x2 - OpDesc  ; Pointer to option value = pointer to string, 0-terminated
DD  x3 - OpDesc  ; Pointer to option single word short name string, for detection 
}

; Support strings formatting and options strings save

OPTION_NAME_FORMAT  EQU  29    ; Formatted output left part before " = " size  
PATH_BUFFER_SIZE    EQU  256   ; Limit for buffers with paths, include last 0

; Aliases for compact access to variables
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



