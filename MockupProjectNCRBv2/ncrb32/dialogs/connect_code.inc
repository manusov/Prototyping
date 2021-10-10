;------------------------------------------------------------------------------;
;                                                                              ;
;  Set of dialogue callback routines (window procedures) for application GUI:  ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

PARM_HWNDDLG  EQU  dword [ebp + 08]  
PARM_MSG      EQU  dword [ebp + 12]
PARM_WPARAM   EQU  dword [ebp + 16]
PARM_LPARAM   EQU  dword [ebp + 20]

include 'main_window.inc'
include 'main_window_tabs.inc'
include 'memory_run.inc'
include 'memory_draw.inc'
include 'math_run.inc'
include 'math_draw.inc'
include 'vector_brief.inc'


