;------------------------------------------------------------------------------;
;                  Target fragment for debug, this part is:                    ;
;              globally used variables without pre-defined values.             ;     
;------------------------------------------------------------------------------;

SystemInfo              SYSTEM_INFO  ?
_SetThreadAffinityMask  DD           ?
ProcessorCounter        DD           ?
AffinityMask            DD           ?
OriginalAffinityMask    DD           ?

TempBufferBase          DD           ?
MemoryBufferBase        DD           ?
ThreadHandle            DD           ?