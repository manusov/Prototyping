;---------- Copy file with measurement iterations -----------------------------;
;                                                                              ;
; Input:   ESI = Pointer to IPB (Input Parameters Block)                       ;
;                IPB[] = Input buffer                                          ;
;          EDI = Pointer to OPB (Output Parameters Block)                      ;
; Output:  EAX = JNI Status: 0=Error, 1=Win32 JNI OK                           ;
;                set externally from this subroutine                           ;
;                OPB[] = Output buffer                                         ;
;                                                                              ;
; Memory buffer for file I/O allocated by Java Code,                           ;
; base address of this buffer = OPB+4096                                       ;
; size of this buffer = I/O block size + 4096, for alignment reservation       ;  
; For Copy: both source and destination files parameters used                  ;
;                                                                              ;
; IPB layout (4096 bytes):                                                     ;                                                   
; DWORD [IPB+00] = Required function ID                                        ;
; DWORD [IPB+04] = Reserved for x64 layout compatibility                       ; 
; DWORD [IPB+08] = Required read/write/copy operation size, bytes              ;
; DWORD [IPB+12] = Reserved for x64 layout compatibility                       ;
; DWORD [IPB+16] = I/O block size, per OS API function call, bytes             ;
; DWORD [IPB+20] = Reserved for x64 layout compatibility                       ;
; DWORD [IPB+24] = Source file performance attributes,                         ;
;                  D0 = Read sync, D1 = Write Sync, other reserved             ;
;                  see MSDN, CreateFileA API parm#6                            ;
; DWORD [IPB+28] = Reserved for x64 layout compatibility                       ;
; QWORD [IPB+32] = Destination file performance attributes,                    ;
;                  D0 = Read sync, D1 = Write Sync, other reserved             ; 
;                  see MSDN, CreateFileA API parm#6                            ;
; DWORD [IPB+36] = Reserved for x64 layout compatibility                       ;
; DWORD [IPB+40] = Number of measurement repeats iterations                    ;
; DWORD [IPB+44] = Reserved for x64 layout compatibility                       ;
; Max. 976 BYTES [IPB+48] = Reserved                                           ; 
; 1024 BYTES [IPB+1024] = Source file path string, null terminated             ;
; 1024 BYTES [IPB+2048] = Destination file path string, null terminated        ;
; 1024 BYTES [IPB+3072] = Reserved                                             ;  
;                                                                              ;
; OPB layout (4096 bytes):                                                     ;
; DWORD [OPB+00] = Memory buffer base address, available after alignment       ;
; DWORD [OPB+04] = Reserved for x64 layout compatibility                       ;
; DWORD [OPB+08] = Memory buffer size, bytes, available after alignment        ;
; DWORD [OPB+12] = Reserved for x64 layout compatibility                       ;       
; DWORD [OPB+16] = Source file handle, available after open file               ;
; DWORD [OPB+20] = Reserved for x64 layout compatibility                       ;
; DWORD [OPB+28] = Destination file handle, available after open file          ;
; DWORD [OPB+20] = Reserved for x64 layout compatibility                       ;
; QWORD [OPB+32] = Executed read/write/copy operation size,                    ; 
;                  available after operation execute                           ;
;                  This is QWORD even for 32-bit version                       ;
; QWORD [OPB+40] = Timer delta for measurement repeats, units = 1 ns           ;
; DWORD [OPB+48] = Last called OS API status, see MSDN                         ;
; DWORD [OPB+52] = Reserved for x64 layout compatibility                       ;
; DWORD [OPB+56] = Last called OS API type:                                    ;
;    0=None,  1=Open for read,  2=Read,  3=Open for write,  4=Write,           ;
;    5=Close, 6=Delete                                                         ;
; DWORD [OPB+60] = Reserved for x64 layout compatibility                       ;
; 4024 BYTES [IPB+72] = Reserved                                               ;
;                                                                              ;
;------------------------------------------------------------------------------;
MeasureCopyFile:
push ebx esi edi ebp
;--- Clear output variables ---
call HelperClearOPB
;--- Calculate aligned base and size for memory buffer ---
call HelperDTA                 ; EBX = Work buffer base, EBP = Work buffer size
;--- Open SOURCE file for Read, note create file function used for open file ---
call HelperOpenSrcRead
test eax,eax                   ; EAX = 0 is wrong return
jz @f                          ; Go with ZF=1 if ZF=1
cmp eax,INVALID_HANDLE_VALUE   ; EAX = -1 if typical errors detected 
@@:                            ; Here flag ZF=1(Z) if error, ZF=0(NZ) if OK
mov eax,STEP_OPEN_READ 
jz ErrorFileContext 
;--- Open/Create DESTINATION file for Write ---
call HelperCreateDstWrite
test eax,eax                   ; EAX = 0 is wrong return
jz @f                          ; Go with ZF=1 if ZF=1
cmp eax,INVALID_HANDLE_VALUE   ; EAX = -1 if typical errors detected 
@@:                            ; Here flag ZF=1(Z) if error, ZF=0(NZ) if OK
mov eax,STEP_OPEN_WRITE 
jz ErrorFileContext 
;--- Start measurement cycle, get and store start timer value ---
mov eax,IPB_ITERATIONS
mov OPB_TRANSIT_REPEATS,eax
lea eax,OPB_TRANSIT_T1
push eax
call [GetSystemTimeAsFileTime]
;--- Measurement iteration ---
.CopyMeasurement:
xor eax,eax
mov OPB_TRANSIT_SIZE_L,eax
mov OPB_TRANSIT_SIZE_H,eax
.ReadWriteBlock:
;--- Read one block from source file ---
mov ebx,OPB_BUFFER_BASE
mov ebp,OPB_BUFFER_SIZE
.ReadBlock:
push 0
mov eax,esp
push 0                         ; Parm#5 = Pointer to overlapped, not used 
push eax                       ; Parm#4 = Pointer for size return 
push ebp                       ; Parm#3 = Block size 
push ebx                       ; Parm#2 = Pointer to buffer 
push OPB_SRC_HANDLE            ; Parm#1 = Target file handle  
call [ReadFile]
pop ecx                        ; ECX = Size returned
test eax,eax
mov eax,STEP_READ 
jz ErrorFileContext
test ecx,ecx
jz .ReadDone                   ; Go done if zero size returned
js .ReadDone                   ; Go done if negative size returned
;--- Cycle for blocks READ ---
add ebx,ecx
sub ebp,ecx
jnz .ReadBlock
.ReadDone: 
;--- Write one block to destination file ---
mov ebx,OPB_BUFFER_BASE
mov ebp,OPB_BUFFER_SIZE
.WriteBlock:
push 0
mov eax,esp
push 0                         ; Parm#5 = Pointer to overlapped, not used 
push eax                       ; Parm#4 = Pointer for size return 
push ebp                       ; Parm#3 = Block size 
push ebx                       ; Parm#2 = Pointer to buffer 
push OPB_DST_HANDLE            ; Parm#1 = Target file handle  
call [WriteFile]
pop ecx                        ; ECX = Size returned
test eax,eax
mov eax,STEP_WRITE 
jz ErrorFileContext
test ecx,ecx
jz .WriteDone                  ; Go done if zero size returned
js .WriteDone                  ; Go done if negative size returned
;--- Cycle for blocks WRITE ---
add OPB_OPERATION_SIZE_L,ecx
adc OPB_OPERATION_SIZE_H,0
add ebx,ecx
sub ebp,ecx
jnz .WriteBlock
.WriteDone:
;--- Cycle for blocks COPY ---
mov eax,IPB_BLOCK_SIZE
add OPB_TRANSIT_SIZE_L,eax
adc OPB_TRANSIT_SIZE_H,0
jnz .DoneReadWrite 
mov eax,IPB_REQUEST_SIZE
cmp OPB_TRANSIT_SIZE_L,eax 
jb .ReadWriteBlock  
.DoneReadWrite:
;--- Re-positioning to SOURCE file begin ---
xor eax,eax                    ; This for compact push zeroes
push eax                       ; Parm#4 = Move method = FILE_BEGIN 
push eax                       ; Parm#3 = Distance High = 0
push eax                       ; Parm#2 = Distance Low = 0
push OPB_SRC_HANDLE            ; Parm#1 = Target file handle 
call [SetFilePointer]
cmp eax,INVALID_SET_FILE_POINTER
mov eax,STEP_READ 
je ErrorFileContext
;--- Re-positioning to DESTINATION file begin ---
xor eax,eax                    ; This for compact push zeroes
push eax                       ; Parm#4 = Move method = FILE_BEGIN 
push eax                       ; Parm#3 = Distance High = 0
push eax                       ; Parm#2 = Distance Low = 0
push OPB_DST_HANDLE            ; Parm#1 = Target file handle 
call [SetFilePointer]
cmp eax,INVALID_SET_FILE_POINTER
mov eax,STEP_WRITE 
je ErrorFileContext
;--- Count measurement iterations, set total read size include meas. cycle ---
dec OPB_TRANSIT_REPEATS
jnz .CopyMeasurement
;--- Get and store stop timer value ---
lea eax,OPB_TRANSIT_T2
push eax
call [GetSystemTimeAsFileTime]
call HelperDeltaTime
;--- Close SOURCE file after READ ---
push OPB_SRC_HANDLE   ; Parm#1 = Pointer to file path
call [CloseHandle]
test eax,eax
jz @f 
;--- Close DESTINATION file after WRITE ---
push OPB_DST_HANDLE   ; Parm#1 = Pointer to file path
call [CloseHandle]
test eax,eax
@@:
mov eax,STEP_CLOSE
jz ErrorFileContext 
;--- Restore stack after alignment, done ---
jmp ExitFileContext
