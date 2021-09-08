;==============================================================================;
;                                                                              ;
;                 Template for Windows x64 GUI application.                    ;
;                                                                              ;
;        Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 )        ;
;           Visit http://flatassembler.net/ for more information.              ;
;                                                                              ;
;       Edit by FASM Editor 2.0, use this editor for correct tabulations.      ;
;              https://fasmworld.ru/instrumenty/fasm-editor-2-0/               ;
;                                                                              ;
;==============================================================================;

include 'win64a.inc'

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:

; ... place here code for debug ...

xor ecx,ecx         ; Parameter #1 = RCX = Exit code
call [ExitProcess]  ; Terminate application

section '.idata' import data readable writeable
library kernel32 , 'KERNEL32.DLL' , user32 , 'USER32.DLL'
include 'api\kernel32.inc'
include 'api\user32.inc'
