;------------------------------------------------------------------------------;
;                                                                              ;
;  Set of dialogue callback routines (window procedures) for application GUI:  ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Pointers to dialogues callbacks procedures, per each tab sheet ----;

ProcDialogs   DD  DialogProcSysinfo
              DD  DialogProcMemory
              DD  DialogProcMath
              DD  DialogProcOs
              DD  DialogProcNativeOs
              DD  DialogProcProcessor
              DD  DialogProcTopology
              DD  DialogProcTopologyEx
              DD  DialogProcNuma
              DD  DialogProcGroups
              DD  DialogProcAcpi
              DD  DialogProcAffCpuid
              DD  DialogProcKmd


