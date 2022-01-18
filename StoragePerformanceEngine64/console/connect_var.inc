;------------------------------------------------------------------------------;
;                         Console support library:                             ;
;                      variables without pre-defined values.                   ;
;------------------------------------------------------------------------------;

; Console input, output, report file, scenario file control variables
; IMPORTANT. If change this values layout, update aliases at connect_equ.inc

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

ScreenInfo  CONSOLE_SCREEN_BUFFER_INFO     ; Console output control structure

BufferSrc   DB  PATH_BUFFER_SIZE  DUP (?)  ; Buffer with source path
BufferDst   DB  PATH_BUFFER_SIZE  DUP (?)  ; Buffer with destination path
