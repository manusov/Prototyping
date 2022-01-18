;------- Part of mass storage measurement scenario: file I/O bandwidth --------;
;                                                                              ;
; Measure bandwidth in MBPS ( megabytes per second ),                          ; 
; use WinAPI for file I/O.                                                     ;
; Variant for FILE GROUP scenario.                                             ;
; Note X (for example X.1) means major level item at storage_context.inc.      ; 
;                                                                              ;
; INPUT:    RSI = Pointer to Input Parameters Block (IPB)                      ;
;           RDI = Pointer to Output Parameters Block (OPB)                     ;
;                 Note for IPB, OPB layout descriptions,                       ;
;                 see functions at "targetlib" firectory.                      ;
;                                                                              ;
; OUTPUT:   OPB fields updated by executed file I/O operation,                 ;
;           caller can detect errors and get timings info by OPB fields.       ;
;                                                                              ;
;------------------------------------------------------------------------------;

MeasureGroupFileMBPS:

push rbx rsi rdi rbp r12 r13 r14 r15

;--- (X.1) --- Create and Write files group -----------------------------------;

lea r15,[Step07_01_Write]    ; R15 = Pointer to phase name for error report
call MessageStepName

xor r13,r13             ; R13 = Total Write data size
xor r14,r14             ; R14 = Total Write operation time
xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"
.WriteGroup:
lea rcx,[ReadPrefix]    ; RCX = Pointer to file name and path, example "src"
lea rdx,[ReadPostfix]   ; RDX = Pointer to extension, example ".bin" 
lea rax,IPB_SRC_PATH    ; RAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureWriteFile   ; Write file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error
add r13,OPB_OPERATION_SIZE
add r14,OPB_TIMER_DELTA
inc ebx
cmp ebx,[OptionFilecount]
jb .WriteGroup          ; Cycle for all files in the group
mov [TotalWriteSizeSum],r13
mov [TotalWriteTimeSum],r14

call MessageStepDone

;--- (X.2) --- Copy files group -----------------------------------------------;

lea r15,[Step07_02_Copy]    ; R15 = Pointer to phase name for error report
call MessageStepName

xor r13,r13             ; R13 = Total Copy data size
xor r14,r14             ; R14 = Total Copy operation time
xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"
.CopyGroup:
lea rcx,[ReadPrefix]    ; RCX = Pointer to file name and path, example "src"
lea rdx,[ReadPostfix]   ; RDX = Pointer to extension, example ".bin" 
lea rax,IPB_SRC_PATH    ; RAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
lea rcx,[WritePrefix]   ; RCX = Pointer to file name and path, example "dst"
lea rdx,[WritePostfix]  ; RDX = Pointer to extension, example ".bin" 
lea rax,IPB_DST_PATH    ; RAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureCopyFile    ; Copy file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error
add r13,OPB_OPERATION_SIZE
add r14,OPB_TIMER_DELTA
inc ebx
cmp ebx,[OptionFilecount]
jb .CopyGroup           ; Cycle for all files in the group
mov [TotalCopySizeSum],r13
mov [TotalCopyTimeSum],r14

call MessageStepDone

;--- (X.3) --- Read files group -----------------------------------------------;

lea r15,[Step07_03_Read]    ; R15 = Pointer to phase name for error report
call MessageStepName

xor r13,r13             ; R13 = Total Read data size
xor r14,r14             ; R14 = Total Read operation time
xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"
.ReadGroup:
lea rcx,[ReadPrefix]    ; RCX = Pointer to file name and path, example "src"
lea rdx,[ReadPostfix]   ; RDX = Pointer to extension, example ".bin" 
lea rax,IPB_SRC_PATH    ; RAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureReadFile    ; Read file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error
add r13,OPB_OPERATION_SIZE
add r14,OPB_TIMER_DELTA
inc ebx
cmp ebx,[OptionFilecount]
jb .ReadGroup           ; Cycle for all files in the group
mov [TotalReadSizeSum],r13
mov [TotalReadTimeSum],r14

call MessageStepDone

;--- (X.4) --- Delete source and destination files group ----------------------;

lea r15,[Step07_04_Delete]    ; R15 = Pointer to phase name for error report
call MessageStepName

xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"
.DeleteGroup:
lea rcx,[ReadPrefix]    ; RCX = Pointer to file name and path, example "src"
lea rdx,[ReadPostfix]   ; RDX = Pointer to extension, example ".bin" 
lea rax,IPB_SRC_PATH    ; RAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureDeleteFile  ; Delete source file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error
lea rcx,[WritePrefix]   ; RCX = Pointer to file name and path, example "dst"
lea rdx,[WritePostfix]  ; RDX = Pointer to extension, example ".bin" 
lea rax,IPB_SRC_PATH    ; RAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureDeleteFile  ; Delete destination file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error
inc ebx
cmp ebx,[OptionFilecount]
jb .DeleteGroup         ; Cycle for all files in the group

call MessageStepDone

;--- Done ---


BreakSeries:

pop r15 r14 r13 r12 rbp rdi rsi rbx
ret




