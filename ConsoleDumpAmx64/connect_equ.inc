;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;                     globally used source definitions.                        ; 
;------------------------------------------------------------------------------;

ERROR_STRING_1  EQU  qword [r15 + 00]
ERROR_STRING_2  EQU  qword [r15 + 08]
ERROR_CODE      EQU  qword [r15 + 16]

CONTEXT_BASE    EQU  TEMP_BUFFER + 4096
CONTEXT_SIZE    EQU  64
TMM_BASE        EQU  CONTEXT_BASE + CONTEXT_SIZE  
TMM_COUNT       EQU  8
ALL_TMM_SIZE    EQU  TMM_COUNT * 1024

ONE_TMM_SIZE    EQU  1024
TMM0_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 0  
TMM1_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 1
TMM2_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 2
TMM3_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 3
TMM4_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 4
TMM5_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 5
TMM6_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 6
TMM7_BASE       EQU  TMM_BASE + ONE_TMM_SIZE * 7

TILE_SIZE       EQU  64
TILE_COUNT      EQU  16

NAME_COLOR      EQU  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY
 

