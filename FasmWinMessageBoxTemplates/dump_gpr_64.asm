;==============================================================================;
;                                                                              ;
;     Debug sample: dump x86-64 general-purpose registers. Win64 edition.      ; 
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

include 'win64a.inc'

;------------------------------------------------------------------------------;
;                  Definitions for fragment under debug.                       ;
;------------------------------------------------------------------------------;

; ...

;------------------------------------------------------------------------------;
;                                Code section.                                 ;
;------------------------------------------------------------------------------;

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:

;------------------------------------------------------------------------------;
;                          Template routines code.                             ;
;------------------------------------------------------------------------------;

sub rsp,8*5        ; Create and align stack frame for WinAPI parameters shadow


; Place here call to code for debug instead of this fragment. 
; Set registers values.
mov rax,rsp ; -1
mov rbx,2
mov rcx,3
mov rdx,4
mov rbp,01111111111111111h
mov rsi,05555555555555555h
mov rdi,0AAAAAAAAAAAAAAAAh
mov r8,0123456789ABCDEFh
mov r9,0FEDCBA9876543210h
mov r10,100h
mov r11,101h
mov r12,102h
mov r13,103h
mov r14,104h
mov r15,105h 
; End of fragment under debug

; Visual registers
call VisualDump
; Exit from application
xor ecx,ecx              ; Parm#1 = RCX = exit code
call [ExitProcess]

;----- Subroutine show GUI box with registers dump ------------;
; INPUT:  General Purpose Registers (GPR) for dump:            ;  
;         RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP               ;
;         R8, R9, R10, R11, R12, R13, R14, R15                 ;
; OUTPUT: None                                                 ;         
;--------------------------------------------------------------;
VisualDump:
push rbx rsi rdi rbp r12 r13 r14 r15        ; Save non-volatile registers
push rsi rdi
push r15 r14 r13 r12 r11 r10 r9 r8
push rdi rsi rbp
lea r8,[rsp + 8*22] 
push r8 rdx rcx rbx rax 
; Build text block with registers dump
cld
mov ecx,16
lea rsi,[WinMessage]
lea rdi,[DataBuffer]
.dumpRegs:
movsw
movsb
mov eax,' =  '
stosd
pop rax
call HexPrint64
mov ax,0D0Ah
stosw
loop .dumpRegs  ; Cycle for 16 registers
mov al,0
stosb
; Show GUI box with text block
push rbp                    ; Save RBP
mov rbp,rsp                 ; Save RSP 
sub rsp,32                  ; Create parameters shadow
and rsp,0FFFFFFFFFFFFFFF0h  ; Align RSP required for API Call
xor ecx,ecx                 ; RCX = Parm #1 = Parent window
lea rdx,[DataBuffer]        ; RDX = Parm #2 = Message
lea r8,[WinCaption]         ; R8  = Parm #3 = Caption (upper message)
xor r9,r9                   ; R9  = Parm #4 = Message flags
call [MessageBoxA]          ; Call target function - show window
mov rsp,rbp                 ; Restore RSP
pop rbp                     ; Restore RBP
; Done
pop rdi rsi
mov eax,10                  ; Return code = 10
pop r15 r14 r13 r12 rbp rdi rsi rbx   ; Restore after save for callback expe.
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

WinCaption  DB '  GPR64 dump',0
WinMessage  DB 'RAXRBXRCXRDXRSPRBPRSIRDI'
            DB 'R8 R9 R10R11R12R13R14R15'

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
