;------------------------------------------------------------------------------;
;                                                                              ;
;  Set of dialogue callback routines (window procedures) for application GUI:  ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

PARM_HWNDDLG  EQU  qword [rbp + 40 + 08 + 00]
PARM_MSG      EQU  qword [rbp + 40 + 08 + 08]
PARM_WPARAM   EQU  qword [rbp + 40 + 08 + 16]
PARM_LPARAM   EQU  qword [rbp + 40 + 08 + 24]
LOW_WPARAM    EQU  dword [rbp + 40 + 08 + 16]

include 'main_window.inc'
include 'main_window_tabs.inc'
include 'memory_run.inc'
include 'memory_draw.inc'
include 'math_run.inc'
include 'math_draw.inc'
include 'vector_brief.inc'
