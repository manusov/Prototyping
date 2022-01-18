;------------------------------------------------------------------------------;
;              Set of target fragments for debug, this part is:                ;
;                      variables without pre-defined values.                   ;     
;------------------------------------------------------------------------------;

;--- Results summary ---
TotalReadSizeSum           DQ  ?       ; Sum of all Read operations sizes
TotalWriteSizeSum          DQ  ?       ; Sum of all Write operations sizes
TotalCopySizeSum           DQ  ?       ; Sum of all Copy operations sizes
TotalReadTimeSum           DQ  ?       ; Sum of all Read operations times
TotalWriteTimeSum          DQ  ?       ; Sum of all Write operations times 
TotalCopyTimeSum           DQ  ?       ; Sum of all Copy operations times 

;--- System information variables ---
TscFrequencyHz             DQ  ?       ; CPU TSC Frequency, integer, Hz
TscFrequencyMHz            DQ  ?       ; CPU TSC Frequency, double prec., MHz 
TscPeriodPs                DQ  ?       ; CPU TSC Period, double prec., picoseco.
MemorySize                 DQ  ?       ; Size of allocated memory block
MemoryBase                 DQ  ?       ; This changed by alignment requirements

;--- OS API Functions pointers ---
PVirtualAllocExNuma        DQ  ?       ; Pointer to detectable WinAPI function
PSetThreadAffinityMask     DQ  ?       ; Pointer to detectable WinAPI function

;--- Benchmarks results variables, floating point double precision values ---
ReadMBPS                   DQ  ?       ; Read speed, Megabytes per Second
WriteMBPS                  DQ  ?       ; Write speed, Megabytes per Second
CopyMBPS                   DQ  ?       ; Copy speed, Megabytes per Second

;--- API control structures pointers ---
PIPB                       DQ  ?       ; Pointer to Input Parameters Block
POPB                       DQ  ?       ; Pointer to Output Parameters Block

;--- Files paths components for JNI/IPB/OPB style library ---  
ReadPrefix                 DB  PATH_BUFFER_SIZE  DUP (?)
ReadPostfix                DB  PATH_BUFFER_SIZE  DUP (?)
WritePrefix                DB  PATH_BUFFER_SIZE  DUP (?)
WritePostfix               DB  PATH_BUFFER_SIZE  DUP (?)