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

; PARM_HWNDDLG_DRAW  EQU  qword [rbp + 64 + 08 + 00]
; PARM_MSG_DRAW      EQU  qword [rbp + 64 + 08 + 08]
; PARM_WPARAM_DRAW   EQU  qword [rbp + 64 + 08 + 16]
; PARM_LPARAM_DRAW   EQU  qword [rbp + 64 + 08 + 24]
; LOW_WPARAM_DRAW    EQU  dword [rbp + 64 + 08 + 16]


include 'dialog_main.inc'
include 'dialog_main_tabs.inc'
include 'dialog_memory_run.inc'
include 'dialog_memory_draw.inc'
include 'dialog_vector_brief.inc'

include 'scenario_memory_run.inc'
include 'scenario_memory_draw.inc'
include 'scenario_vector_brief.inc'
