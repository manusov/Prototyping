;------- Part of mass storage measurement scenario: file I/O bandwidth --------;
;                                                                              ;
; Measure bandwidth in MBPS ( megabytes per second ),                          ; 
; use WinAPI for file I/O.                                                     ;
; Variant for FILE GROUP scenario.                                             ;
; Note X (for example X.1) means major level item at storage_context.inc.      ; 
;                                                                              ;
; INPUT:    ESI = Pointer to Input Parameters Block (IPB)                      ;
;           EDI = Pointer to Output Parameters Block (OPB)                     ;
;                 Note for IPB, OPB layout descriptions,                       ;
;                 see functions at "targetlib" firectory.                      ;
;                                                                              ;
; OUTPUT:   OPB fields updated by executed file I/O operation,                 ;
;           caller can detect errors and get timings info by OPB fields.       ;
;                                                                              ;
;------------------------------------------------------------------------------;

MeasureGroupFileMBPS:

push ebx esi edi ebp

;--- (X.1) --- Create and Write files group -----------------------------------;

lea eax,[Step07_01_Write]    ; EAX = Pointer to phase name for error report
call MessageStepName

; FILE I/O WIN API ERROR REPORTING UNDER CONSTRUCTION
; xor r13,r13           ; R13 = Total Write data size
; xor r14,r14           ; R14 = Total Write operation time

xor eax,eax
mov dword [TotalWriteTimeSum + 0],eax
mov dword [TotalWriteTimeSum + 4],eax

xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"

.WriteGroup:
lea ecx,[ReadPrefix]    ; ECX = Pointer to file name and path, example "src"
lea edx,[ReadPostfix]   ; EDX = Pointer to extension, example ".bin" 
lea eax,IPB_SRC_PATH    ; EAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureWriteFile   ; Write file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error

; add r13,OPB_OPERATION_SIZE
; add r14,OPB_TIMER_DELTA

mov eax,OPB_TIMER_DELTA_L
mov edx,OPB_TIMER_DELTA_H
add dword [TotalWriteTimeSum + 0],eax
adc dword [TotalWriteTimeSum + 4],edx

inc ebx
cmp ebx,[OptionFilecount]
jb .WriteGroup          ; Cycle for all files in the group

; mov [TotalWriteSizeSum],r13
; mov [TotalWriteTimeSum],r14

call MessageStepDone

;--- (X.2) --- Copy files group -----------------------------------------------;

lea eax,[Step07_02_Copy]    ; EAX = Pointer to phase name for error report
call MessageStepName

; FILE I/O WIN API ERROR REPORTING UNDER CONSTRUCTION
; xor r13,r13           ; R13 = Total Copy data size
; xor r14,r14           ; R14 = Total Copy operation time

xor eax,eax
mov dword [TotalCopyTimeSum + 0],eax
mov dword [TotalCopyTimeSum + 4],eax

xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"

.CopyGroup:
lea ecx,[ReadPrefix]    ; ECX = Pointer to file name and path, example "src"
lea edx,[ReadPostfix]   ; EDX = Pointer to extension, example ".bin" 
lea eax,IPB_SRC_PATH    ; EAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
lea ecx,[WritePrefix]   ; ECX = Pointer to file name and path, example "dst"
lea edx,[WritePostfix]  ; EDX = Pointer to extension, example ".bin" 
lea eax,IPB_DST_PATH    ; EAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureCopyFile    ; Copy file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error

; add r13,OPB_OPERATION_SIZE
; add r14,OPB_TIMER_DELTA

mov eax,OPB_TIMER_DELTA_L
mov edx,OPB_TIMER_DELTA_H
add dword [TotalCopyTimeSum + 0],eax
adc dword [TotalCopyTimeSum + 4],edx

inc ebx
cmp ebx,[OptionFilecount]
jb .CopyGroup           ; Cycle for all files in the group

; mov [TotalCopySizeSum],r13
; mov [TotalCopyTimeSum],r14

call MessageStepDone

;--- (X.3) --- Read files group -----------------------------------------------;

lea eax,[Step07_03_Read]    ; EAX = Pointer to phase name for error report
call MessageStepName

; FILE I/O WIN API ERROR REPORTING UNDER CONSTRUCTION
; xor r13,r13             ; R13 = Total Read data size
; xor r14,r14             ; R14 = Total Read operation time

xor eax,eax
mov dword [TotalReadTimeSum + 0],eax
mov dword [TotalReadTimeSum + 4],eax

xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"

.ReadGroup:
lea ecx,[ReadPrefix]    ; ECX = Pointer to file name and path, example "src"
lea edx,[ReadPostfix]   ; EDX = Pointer to extension, example ".bin" 
lea eax,IPB_SRC_PATH    ; EAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureReadFile    ; Read file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error

; add r13,OPB_OPERATION_SIZE
; add r14,OPB_TIMER_DELTA

mov eax,OPB_TIMER_DELTA_L
mov edx,OPB_TIMER_DELTA_H
add dword [TotalReadTimeSum + 0],eax
adc dword [TotalReadTimeSum + 4],edx

inc ebx
cmp ebx,[OptionFilecount]
jb .ReadGroup           ; Cycle for all files in the group

; mov [TotalReadSizeSum],r13
; mov [TotalReadTimeSum],r14

call MessageStepDone

;--- (X.4) --- Delete source and destination files group ----------------------;

lea eax,[Step07_04_Delete]    ; EAX = Pointer to phase name for error report
call MessageStepName

xor ebx,ebx             ; EBX = Files counter, for example 0 means "src0.bin"
.DeleteGroup:
lea ecx,[ReadPrefix]    ; ECX = Pointer to file name and path, example "src"
lea edx,[ReadPostfix]   ; EDX = Pointer to extension, example ".bin" 
lea eax,IPB_SRC_PATH    ; EAX = Pointer to destination buffer
call PathHelper         ; Build file path and name string, EBX = number 
call MeasureDeleteFile  ; Delete source file selected by EBX
cmp OPB_LAST_ERROR, 0
jne BreakSeries         ; Go if error
cmp OPB_LAST_OPERATION, STEP_NONE
jne BreakSeries         ; Go if error
lea ecx,[WritePrefix]   ; ECX = Pointer to file name and path, example "dst"
lea edx,[WritePostfix]  ; EDX = Pointer to extension, example ".bin" 
lea eax,IPB_SRC_PATH    ; EAX = Pointer to destination buffer
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

pop ebp edi esi ebx
ret
