; UNDER CONSTRUCTION

;------------------------------------------------------------------------------;
;                                                                              ;
; Part of mass storage measurement scenario: file I/O bandwidth.               ;
; Measure bandwidth in MBPS ( megabytes per second ),                          ;
; use WinAPI for file I/O.                                                     ;
; Variant for SINGLE FILE scenario.                                            ;
; Note X (for example X.1) means major level item at storage_context.inc.      ;
;                                                                              ;
; INPUT:                                                                       ;
;                                                                              ;
; OUTPUT:                                                                      ;
;                                                                              ;
;------------------------------------------------------------------------------;

MeasureSingleFileMBPS:

;--- (X.1) --- Build source file path, name and extension ---------------------;

;--- (X.2) --- Build destination file path, name and extension ----------------;

;--- (X.3) --- File I/O -------------------------------------------------------;

;--- (X.4) --- Data size sum calculation --------------------------------------;

;--- (X.5) ---Work time sum calculation ---------------------------------------;

;--- (X.6) ---Cycle for series ------------------------------------------------;

ret

