;------------------------------------------------------------------------------;
;                    ADD THIS STRING TO FILE UNDER DEBUG:                      ;
;                      include '..\EMULATOR_32_1.inc'                          ;
;------------------------------------------------------------------------------;

InfoExtendedTopology:
push esi edi
mov esi,Emulation_Dump_Start
mov edi,ebp
mov ecx,Emulation_Dump_End - Emulation_Dump_Start
mov [DYNA_PTR.listTopologyEx.objectStart],ebp
add ebp,ecx
mov [DYNA_PTR.listTopologyEx.objectStop],ebp
cld
rep movsb
pop edi esi
clc
ret

Emulation_Dump_Start:
; Core #0, Performance, HT 
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h,  001h, 001h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  003h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; Core #1, Performance, HT
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h,  001h, 001h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  00Ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; Core #2, Efficient
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  010h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; Core #3, Efficient
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  020h, 000h, 000h, 000h, 000h, 000h, 000h, 000h,  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L1 data cache, 48KB, affinity for core #0 
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 0C0h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 003h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L1 instruction cache, 32KB, affinity for core #0
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 080h, 000h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 003h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L2 unified cache, 512KB, affinity for core #0  
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 008h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 003h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L3 unified cache, 16MB, affinity for all cores #[0-5] 
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 003h, 010h, 040h, 000h, 000h, 000h, 000h, 001h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 03Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 data cache, 48KB, affinity for core #1
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 0C0h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00Ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h 
; L1 instruction cache, 32KB, affinity for core #1
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 080h, 000h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00Ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L2 unified cache, 512KB, affinity for core #1
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 008h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00Ch, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 data cache, 48KB, affinity for core #2
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 060h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L1 instruction cache, 32KB, affinity for core #2
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 000h, 001h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L2 unified cache, 512KB, affinity for core #2
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 004h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 010h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h 

; L1 data cache, 48KB, affinity for core #3
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 060h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L1 instruction cache, 32KB, affinity for core #3
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 000h, 001h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; L2 unified cache, 512KB, affinity for core #3
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 004h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 020h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h 

; Processor package
DB  003h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  03Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; NUMA node
DB  001h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  03Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
; Processor group
DB  004h, 000h, 000h, 000h, 050h, 000h, 000h, 000h, 001h, 000h, 001h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  004h, 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h

Emulation_Dump_End:

