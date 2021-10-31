;------------------------------------------------------------------------------;
;                                                                              ;
;                   This performance patterns used in the                      ; 
;                     "Run", "Draw" benchmark scenarios.                       ;
;           Library of performance patterns for latency measurement:           ;
;                    connect include files with subroutines.                   ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Get pointer to performance pattern subroutines by ID --------------;
;                                                                              ;
; INPUT:   AL  = Subroutine ID                                                 ;
;                                                                              ;
; OUTPUT:  EBX = Subroutine #1 entry point, walk list builder                  ;
;          EAX = Subroutine #2 entry point, walker                             ;
;                                                                              ;
;------------------------------------------------------------------------------;
GetLatencyPattern:
movzx eax,al                         ; This clears bits EAX[31-8]
imul eax,eax,6                       ; 6 = sizeof.LATENCY_ENTRY
push eax
movzx eax,word [TABLE_LATENCY + eax]  ; Read 16-bit compact offset
lea ebx,[ROUTINES_LATENCY + eax]      ; Calculate 32-bit absolute address
pop eax
cmp al,2
ja .x2
mov al,2
jmp .xdone
.x2:
mov al,5
.xdone:
movzx eax,word [TABLE_LATENCY + eax]  ; Read 16-bit compact offset
add eax,ROUTINES_LATENCY              ; Calculate 32-bit absolute address
ret
;---------- Get pointer to performance pattern cycle body for dump by ID ------;
;    Also returns used instruction operand width, bits                         ;
;                                                                              ;
; INPUT:   AL  = Subroutine ID                                                 ;
;                                                                              ;
; OUTPUT:  EAX = Dump fragment base address                                    ;
;          EDX = Dump fragment length, bytes                                   ;
;          ECX = Used instruction operand width, bits                          ;
;                                                                              ;
;------------------------------------------------------------------------------;
GetLatencyDump:
cmp al,2
ja .x2
mov al,2
jmp .xdone
.x2:
mov al,5
.xdone:
movzx eax,al
imul eax,eax,6   ; sizeof.LATENCY_ENTRY
movzx ecx,byte [TABLE_LATENCY + eax + 5]
movzx edx,byte [TABLE_LATENCY + eax + 4]
movzx eax,word [TABLE_LATENCY + eax + 2]
add eax,ROUTINES_LATENCY
ret
;--- Include files connect for performance patterns: build list, walk list ----;
ROUTINES_LATENCY:  ; Pointer for compact 16-bit relative offsets
include 'latency_lcm_32.inc'       ; Build walk list, use linear congruental method, for one 32-bit load
include 'latency_rdrand_32.inc'    ; Build walk list, use RDRAND instruction, for one 32-bit load
include 'latency_walk_32.inc'      ; Walk on pre-builded walk list, one 32-bit load
include 'latency_lcm_32x2.inc'     ; Build walk list, use linear congruental method, for two 32-bit loads match x64 version
include 'latency_rdrand_32x2.inc'  ; Build walk list, use RDRAND instruction, for two 32-bit loads match x64 version
include 'latency_walk_32x2.inc'    ; Walk on pre-builded walk list, two 32-bit loads match x64 version

