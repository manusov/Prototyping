; ADD THIS STRING TO FILE UNDER DEBUG:
; include '..\modeling_dumps.inc'

; Processor #0 
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; Processor #1
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; Processor #2
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; Processor #3
DB  000h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  008h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 data cache, 48KB, affinity for processor #0 
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 0C0h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 instruction cache, 32KB, affinity for processor #0
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 080h, 000h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L2 unified cache, 512KB, affinity for processor #0  
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 008h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L3 unified cache, 16MB, affinity for processor #0 (really for all CPUs, but Oracle emulate L3 per each CPU) 
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 003h, 010h, 040h, 000h, 000h, 000h, 000h, 001h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 data cache, 48KB, affinity for processor #1
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 0C0h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h 

; L1 instruction cache, 32KB, affinity for processor #1
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 080h, 000h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L2 unified cache, 512KB, affinity for processor #1
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 008h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L3 unified cache, 16MB, affinity for processor #1 (really for all CPUs, but Oracle emulate L3 per each CPU)
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 003h, 010h, 040h, 000h, 000h, 000h, 000h, 001h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 data cache, 48KB, affinity for processor #2
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 0C0h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 instruction cache, 32KB, affinity for processor #2
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 080h, 000h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L2 unified cache, 512KB, affinity for processor #2
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 008h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 

; L3 unified cache, 16MB, affinity for processor #2 (really for all CPUs, but Oracle emulate L3 per each CPU)
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 003h, 010h, 040h, 000h, 000h, 000h, 000h, 001h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 data cache, 48KB, affinity for processor #3
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 00Ch, 040h, 000h, 000h, 0C0h, 000h, 000h
DB  002h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 008h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L1 instruction cache, 32KB, affinity for processor #3
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 001h, 008h, 040h, 000h, 000h, 080h, 000h, 000h
DB  001h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 008h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; L2 unified cache, 512KB, affinity for processor #3
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 002h, 008h, 040h, 000h, 000h, 000h, 008h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 008h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h 

; L3 unified cache, 16MB, affinity for processor #3 (really for all CPUs, but Oracle emulate L3 per each CPU)
DB  002h, 000h, 000h, 000h, 038h, 000h, 000h, 000h, 003h, 010h, 040h, 000h, 000h, 000h, 000h, 001h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 008h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; Processor package
DB  003h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 001h, 000h
DB  00Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; NUMA node
DB  001h, 000h, 000h, 000h, 030h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  00Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

; Processor group
DB  004h, 000h, 000h, 000h, 050h, 000h, 000h, 000h, 001h, 000h, 001h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  004h, 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
DB  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 00Fh, 000h, 000h, 000h, 000h, 000h, 000h, 000h

