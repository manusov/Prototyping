;==============================================================================;
;                                                                              ;
;                              Debug sample:                                   ; 
; Read and dump CPU privileged objects and segments selectors from user mode.  ; 
;                              Win32 edition.                                  ;
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

include 'win32a.inc'

;------------------------------------------------------------------------------;
;                                Code section.                                 ;
;------------------------------------------------------------------------------;

format PE GUI 4.0
entry start
section '.text' code readable executable
start:
; Set buffer address, blank buffer by 11h
cld
lea ebx,[BinaryData]
mov edi,ebx
mov ecx,1024
mov al,11h
rep stosb
; Store system priveleged registers to buffer
sgdt [ebx+00h]
sidt [ebx+10h]
sldt [ebx+20h]
str  [ebx+30h]
smsw [ebx+40h]
; Store segment registers to buffer
mov [ebx+50h],cs
mov [ebx+52h],ss
mov [ebx+54h],ds
mov [ebx+56h],es
mov [ebx+58h],fs
mov [ebx+5Ah],gs 
; Build text block with registers dump
cld
lea esi,[WinMessage]
lea edi,[TextData]
lea ebx,[BinaryData]
; GDTR base
call StringWrite
lea ecx,[ebx+02h]
call ValuePrint32
; GDTR limit
call StringWrite
lea ecx,[ebx+00h]
call ValuePrint16
; IDTR base
call StringWrite
lea ecx,[ebx+12h]
call ValuePrint32
; IDTR limit
call StringWrite
lea ecx,[ebx+10h]
call ValuePrint16
; LDTR selector
call StringWrite
lea ecx,[ebx+20h]
call ValuePrint16
; TR selector
call StringWrite
lea ecx,[ebx+30h]
call ValuePrint16
; MSW
call StringWrite
lea ecx,[ebx+40h]
call ValuePrint16
; Skip string and start build text with segment reisters
mov ax,0D0Ah
stosw
; CS
call StringWrite
lea ecx,[ebx+50h]
call ValuePrint16
; SS
call StringWrite
lea ecx,[ebx+52h]
call ValuePrint16
; DS
call StringWrite
lea ecx,[ebx+54h]
call ValuePrint16
; ES
call StringWrite
lea ecx,[ebx+56h]
call ValuePrint16
; FS
call StringWrite
lea ecx,[ebx+58h]
call ValuePrint16
; GS
call StringWrite
lea ecx,[ebx+5Ah]
call ValuePrint16
mov al,0
stosb
; Show GUI box with text block
xor eax,eax                 ; EAX = 0 for make compact PUSH 0
push eax                    ; Parm #4 = Message flags
push dword WinCaption       ; Parm #3 = Caption (upper message)
push dword TextData         ; Parm #2 = Message
push eax                    ; Parm #1 = Parent window
call [MessageBoxA]          ; Call target function - show window
; Exit from application
push 0                      ; Parm#1 = exit code
call [ExitProcess]

; Helpers subroutines
;---------- String copy ---------------------------------------;
; INPUT:   ESI = source address                                ;
;          EDI = destination address                           ;
; OUTPUT:  ESI = Modify                                        ;
;          EDI = Modify                                        ;
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
; INPUT:  ECX = Pointer to number                              ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
ValuePrint32:
mov eax,' =  '
stosd
mov eax,[ecx]
call HexPrint32
PrintEntry:
mov al,'h'
stosb
mov ax,0D0Ah
stosw
ret
;--- Read and print 16-bit Hex Number with "=", "h", LF, CR ---;
; INPUT:  ECX = Pointer to number                              ;
;         EDI = Destination Pointer                            ;
; OUTPUT: EDI = Modify                                         ;
;--------------------------------------------------------------;
ValuePrint16:
mov eax,' =  '
stosd
mov ax,[ecx]
call HexPrint16
jmp PrintEntry
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
;                              Data section.                                   ;
;          Note remember about error if data section exist but empty.          ;     
;------------------------------------------------------------------------------;

; Remember about error if data section empty
section '.data' data readable writeable
; Constants located at exe file
WinCaption  DB '  Win32 CPU privileged objects dump',0
WinMessage  DB 'GDTR base',0
            DB 'GDTR limit',0
            DB 'IDTR base',0
            DB 'IDTR limit',0
            DB 'LDTR selector',0
            DB 'TR selector',0
            DB 'MSW',0
            DB 'CS',0
            DB 'SS',0
            DB 'DS',0
            DB 'ES',0
            DB 'FS',0
            DB 'GS',0
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
