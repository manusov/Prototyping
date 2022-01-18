;------------------------------------------------------------------------------;
;                Registers and memory dump subroutines library:                ;
;             connect include files with globally used subroutines.            ; 
;------------------------------------------------------------------------------;

include  'decprint.inc'       ; Print (to memory buffer) 32-bit decimal number
include  'hexprint.inc'       ; Print 4/8/16/32/64-bit Hexadecimal numbers
include  'doubleprint.inc'    ; Print double precision value
include  'sizeprint.inc'      ; Print memory block size as Integer.Float
include  'stringwrite.inc'    ; Copy text string terminated by 00h
include  'dumpcpu.inc'        ; Dump CPU registers and memory areas
include  'itemwrite.inc'      ; Copy text strings terminated by 00h