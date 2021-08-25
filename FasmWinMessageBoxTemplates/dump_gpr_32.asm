;==============================================================================;
;                                                                              ;
;    Debug sample: dump 32-bit x86 general-purpose registers. Win32 edition.   ; 
;                                                                              ;
;        Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 )        ;
;                         http://flatassembler.net/                            ;
;                                                                              ;
;       Edit by FASM Editor 2.0, use this editor for correct tabulations.      ;
;              https://fasmworld.ru/instrumenty/fasm-editor-2-0/               ;
;                                                                              ;
;==============================================================================;

;------------------------------------------------------------------------------;
;                         Definitions for template.                            ;
;------------------------------------------------------------------------------;

include 'win32a.inc'

;------------------------------------------------------------------------------;
;                  Definitions for fragment under debug.                       ;
;------------------------------------------------------------------------------;

; ...

;------------------------------------------------------------------------------;
;                                Code section.                                 ;
;------------------------------------------------------------------------------;

format PE GUI 4.0
entry start
section '.text' code readable executable
start:

;------------------------------------------------------------------------------;
;                          Template routines code.                             ;
;------------------------------------------------------------------------------;

; Place here call to code for debug instead of this fragment. 
; Set registers values.
mov eax,esp ; 1
mov ebx,2
mov ecx,22h
mov edx,33h
mov ebp,3
mov esi,15h
mov edi,25h
; End of fragment under debug

; Visual registers
call VisualDump
; Exit from application
push 0              ; Parm#1 = exit code
call [ExitProcess]

;----- Subroutine show GUI box with registers dump ------------;
; INPUT:  General Purpose Registers (GPR) for dump:            ;  
;         EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP               ;
; OUTPUT: None                                                 ;         
;--------------------------------------------------------------;
VisualDump:
push eax ebx ecx edx esp ebp esi edi
; Build text block with registers dump 
cld
mov ecx,8
lea esi,[WinMessage]
lea edi,[DataBuffer]
lea ebp,[esp+7*4]
.dumpRegs:
movsw
movsb
mov eax,' =  '
stosd
mov eax,[ebp]
sub ebp,4 
cmp cl,4      ; Detect iteration with ESP value
jne @f
add eax,5*4   ; Correct ESP value for dump
@@:
call HexPrint32
mov ax,0D0Ah
stosw
loop .dumpRegs  ; Cycle for 8 registers
mov al,0
stosb
; Show GUI box with text block
xor eax,eax                 ; EAX = 0 for make compact PUSH 0
push eax                    ; Parm #4 = Message flags
push dword WinCaption       ; Parm #3 = Caption ( upper message )
push dword DataBuffer       ; Parm #2 = Message
push eax                    ; Parm #1 = Parent window
call [MessageBoxA]          ; Call target function - show window
; Done
pop edi esi ebp eax edx ecx ebx eax
ret
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
; OUTPUT: EDI = Modify                                         ;
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
; Note about short variant with DAA, but for 32-bit mode only. ;
;--------------------------------------------------------------;
HexPrint4:
cld
push eax
and al,0Fh
cmp al,9
ja .modify
add al,'0'
jmp .store
.modify:
add al,'A'-10
.store:
stosb
pop eax
ret

;------------------------------------------------------------------------------;
;              Code fragment under debug, subroutines for call.                ; 
;------------------------------------------------------------------------------;

; ...

;------------------------------------------------------------------------------;
;                              Data section.                                   ;
;          Note remember about error if data section exist but empty.          ;     
;------------------------------------------------------------------------------;

section '.data' data readable writeable

;------------------------------------------------------------------------------;
;                Constants located at exe file, part of template.              ;
;------------------------------------------------------------------------------;

WinCaption  DB '  GPR32 dump',0
WinMessage  DB 'EAXEBXECXEDXESPEBPESIEDI'

;------------------------------------------------------------------------------;
;           Constants located at exe file, part of code under debug.           ;
;------------------------------------------------------------------------------;

; ...

;------------------------------------------------------------------------------;
;       Variables not requires space in the exe file, part of template.        ;
;------------------------------------------------------------------------------;

DataBuffer  DB 1024 DUP (?)

;------------------------------------------------------------------------------;
;    Variables not requires space in the exe file, part of code under debug.   ;
;------------------------------------------------------------------------------;

; ...

;------------------------------------------------------------------------------;
;                              Import section.                                 ;
;------------------------------------------------------------------------------;

section '.idata' import data readable writeable
library kernel32, 'KERNEL32.DLL', user32,'USER32.DLL'    
include 'api\user32.inc'       ; USER32.DLL required because MessageBoxA used
include 'api\kernel32.inc'     ; KERNEL32.DLL required for System API


