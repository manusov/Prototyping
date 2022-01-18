; UNDER CONSTRUCTION

;------------------------------------------------------------------------------;
;                                                                              ;
; Part of mass storage measurement scenario: file I/O performance.             ;
; Measure performance in IOPS ( I/O operations per second ),                   ;
; use WinAPI for file I/O.                                                     ;
; Note X (for example X.1) means major level item at storage_context.inc.      ;
;                                                                              ;
; INPUT:                                                                       ;
;                                                                              ;
; OUTPUT:                                                                      ;
;                                                                              ;
;------------------------------------------------------------------------------;

MeasureFileIOPS:

;--- (X.1) --- Build selected blocks list for Read and Write ------------------;

;--- (X.2) --- Create and fill series of files --------------------------------;

;--- (X.3) --- Execute Write IOPS measurement ---------------------------------;

;--- (X.4) --- Execute Read IOPS measurement ----------------------------------;

;--- (X.5) --- Execute Mixed IOPS measurement ---------------------------------;

;--- (X.6) --- Close and delete file group ------------------------------------;

ret
