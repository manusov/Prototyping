;==============================================================================;
;                                                                              ;
;    Debug sample: Read and dump CPU privileged objects from user mode.        ; 
;                              Win64 edition.                                  ;
;  This code checked at Intel Haswell, but can cause faults if CR4.UMIP = 1.   ; 
;                                                                              ;
;        Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 )        ;
;                         http://flatassembler.net/                            ;
;                                                                              ;
;       Edit by FASM Editor 2.0, use this editor for correct tabulations.      ;
;              https://fasmworld.ru/instrumenty/fasm-editor-2-0/               ;
;                                                                              ;
;==============================================================================;

;------------------------------------------------------------------------------;
;                                Definitions.                                  ;
;------------------------------------------------------------------------------;

include 'win64a.inc'

;------------------------------------------------------------------------------;
;                                Code section.                                 ;
;------------------------------------------------------------------------------;

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
sub rsp,8*5        ; Create and align stack frame for WinAPI parameters shadow
; Set buffer address, blank buffer by 11h
cld
lea rbx,[BinaryData]
mov rdi,rbx
mov ecx,1024
mov al,11h
rep stosb
; Store system priveleged registers to buffer
sgdt [rbx+00h]
sidt [rbx+10h]
sldt [rbx+20h]
str  [rbx+30h]
smsw [rbx+40h] 
; Build text block with registers dump
cld
lea rsi,[WinMessage]
lea rdi,[TextData]
lea rbx,[BinaryData]
; GDTR base
call StringWrite
lea rcx,[rbx+02h]
call ValuePrint64
; GDTR limit
call StringWrite
lea rcx,[rbx+00h]
call ValuePrint16
; IDTR base
call StringWrite
lea rcx,[rbx+12h]
call ValuePrint64
; IDTR limit
call StringWrite
lea rcx,[rbx+10h]
call ValuePrint16
; LDTR selector
call StringWrite
lea rcx,[rbx+20h]
call ValuePrint16
; TR selector
call StringWrite
lea rcx,[rbx+30h]
call ValuePrint16
; MSW
call StringWrite
lea rcx,[rbx+40h]
call ValuePrint16
mov al,0
stosb
; Show GUI box with text block
mov rbp,rsp                 ; Save RSP 
sub rsp,32                  ; Create parameters shadow
and rsp,0FFFFFFFFFFFFFFF0h  ; Align RSP required for API Call
xor ecx,ecx                 ; RCX = Parm #1 = Parent window
lea rdx,[TextData]          ; RDX = Parm #2 = Message
lea r8,[WinCaption]         ; R8  = Parm #3 = Caption (upper message)
xor r9,r9                   ; R9  = Parm #4 = Message flags
call [MessageBoxA]          ; Call target function - show window
mov rsp,rbp                 ; Restore RSP
; Exit from application
xor ecx,ecx                 ; Parm#1 = RCX = exit code
call [ExitProcess]

; Helpers subroutines
;---------- String copy ---------------------------------------;
; INPUT:   RSI = source address                                ;
;          RDI = destination address                           ;
; OUTPUT:  RSI = Modify                                        ;
;          RDI = Modify                                        ;
;--------------------------------------------------------------; 
StringWrite:
@@:
lodsb
cmp al,0
je @f
stosb
jmp @b
@@:
ret
;--- Read and print 64-bit Hex Number with "=", "h", LF, CR ---;
; INPUT:  RCX = Pointer to number                              ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
ValuePrint64:
mov eax,' =  '
stosd
mov rax,[rcx]
call HexPrint64
PrintEntry:
mov al,'h'
stosb
mov ax,0D0Ah
stosw
ret
;--- Read and print 16-bit Hex Number with "=", "h", LF, CR ---;
; INPUT:  RCX = Pointer to number                              ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
ValuePrint16:
mov eax,' =  '
stosd
mov ax,[rcx]
call HexPrint16
jmp PrintEntry
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
;                              Data section.                                   ;
;          Note remember about error if data section exist but empty.          ;     
;------------------------------------------------------------------------------;

section '.data' data readable writeable
; Constants located at exe file
WinCaption  DB '  Win64 CPU privileged objects dump',0
WinMessage  DB 'GDTR base',0
            DB 'GDTR limit',0
            DB 'IDTR base',0
            DB 'IDTR limit',0
            DB 'LDTR selector',0
            DB 'TR selector',0
            DB 'MSW',0
; Variables not requires space in the exe file
TextData    DB 1024 DUP (?)
BinaryData  DB 1024 DUP (?)

;------------------------------------------------------------------------------;
;                              Import section.                                 ;
;------------------------------------------------------------------------------;

section '.idata' import data readable writeable
library kernel32, 'KERNEL32.DLL', user32,'USER32.DLL'    
include 'api\user32.inc'       ; USER32.DLL required because MessageBoxA used
include 'api\kernel32.inc'     ; KERNEL32.DLL required for System API
