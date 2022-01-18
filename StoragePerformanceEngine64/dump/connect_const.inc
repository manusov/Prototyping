;------------------------------------------------------------------------------;
;                Registers and memory dump subroutines library:                ;
;     globally used data constants and variables with pre-defined state.       ; 
;------------------------------------------------------------------------------;

;------------------------------------------------------------------------------;
; CPU registers names.
;------------------------------------------------------------------------------;

NamesGPR64:
DB  'RAX' , 0 
DB  'RBX' , 0
DB  'RCX' , 0
DB  'RDX' , 0
DB  'RSI' , 0
DB  'RDI' , 0
DB  'RBP' , 0
DB  'RSP' , 0
DB  'R8 ' , 0
DB  'R9 ' , 0
DB  'R10' , 0
DB  'R11' , 0
DB  'R12' , 0
DB  'R13' , 0
DB  'R14' , 0
DB  'R15' , 0

NamesSelectors:
DB  'CS' , 0
DB  'DS' , 0
DB  'ES' , 0
DB  'SS' , 0
DB  'FS' , 0
DB  'GS' , 0

NamesFPU:
DB  'ST0' , 0
DB  'ST1' , 0
DB  'ST2' , 0
DB  'ST3' , 0
DB  'ST4' , 0
DB  'ST5' , 0
DB  'ST6' , 0
DB  'ST7' , 0

NamesMMX:
DB  'MM0' , 0
DB  'MM1' , 0
DB  'MM2' , 0
DB  'MM3' , 0
DB  'MM4' , 0
DB  'MM5' , 0
DB  'MM6' , 0
DB  'MM7' , 0

NameSSE:
DB  'XMM' , 0

NameAVX256:
DB  'YMM' , 0

NameAVX512:
DB  'ZMM' , 0

IntervalAVX512:
DB  0Dh, 0Ah, '      ' , 0

;------------------------------------------------------------------------------;
; Memory size and speed units.                                                 ;
;------------------------------------------------------------------------------;

U_B            DB  'Bytes',0
U_KB           DB  'KB',0
U_MB           DB  'MB',0
U_GB           DB  'GB',0
U_TB           DB  'TB',0
U_MBPS         DB  'MBPS',0
U_NS           DB  'nanoseconds',0
