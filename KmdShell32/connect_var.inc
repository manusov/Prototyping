;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;              globally used variables without pre-defined values.             ;     
;------------------------------------------------------------------------------;

SCP_DATA  SCPDATA         ?   ; Service control program data
isWoW64   DD              ?   ; Detect 32-bit application under Win64
Query     RZDriverQuery   ?   ; Request structure for driver
Bytes     DQ              ?   ; Bytes counter for driver request
