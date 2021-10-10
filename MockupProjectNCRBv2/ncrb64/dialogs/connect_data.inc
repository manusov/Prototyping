;------------------------------------------------------------------------------;
;                                                                              ;
;  Set of dialogue callback routines (window procedures) for application GUI:  ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Pointers to dialogues callbacks procedures, per each tab sheet ----;

ProcDialogs   DQ  DialogProcSysinfo
              DQ  DialogProcMemory
              DQ  DialogProcMath
              DQ  DialogProcOs
              DQ  DialogProcNativeOs
              DQ  DialogProcProcessor
              DQ  DialogProcTopology
              DQ  DialogProcTopologyEx
              DQ  DialogProcNuma
              DQ  DialogProcGroups
              DQ  DialogProcAcpi
              DQ  DialogProcAffCpuid
              DQ  DialogProcKmd

