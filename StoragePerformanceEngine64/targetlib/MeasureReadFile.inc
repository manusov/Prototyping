;---------- Read file with measurement iterations -----------------------------;
;                                                                              ;
; Input:   RSI = Pointer to IPB (Input Parameters Block)                       ;
;                IPB[] = Input buffer                                          ;
;          RDI = Pointer to OPB (Output Parameters Block)                      ;
; Output:  RAX = JNI Status: 0=Error, 2=Win64 JNI OK                           ;
;                set externally from this subroutine                           ;
;                OPB[] = Output buffer                                         ;
;                                                                              ;
; Memory buffer for file I/O allocated by Java Code,                           ;
; base address of this buffer = OPB+4096                                       ;
; size of this buffer = I/O block size + 4096, for alignment reservation       ;  
; For Read: source file parameters used, destination file parameters ignored   ;
;                                                                              ;
; IPB layout (4096 bytes):                                                     ;
; QWORD [IPB+00] = Required function ID                                        ;
; QWORD [IPB+08] = Required read/write/copy operation size, bytes              ;
; QWORD [IPB+16] = I/O block size, per OS API function call, bytes             ;
; QWORD [IPB+24] = Source file performance attributes,                         ;
;                  D0 = Read sync, D1 = Write Sync, other reserved             ; 
;                  see MSDN, CreateFileA API parm#6                            ;
; QWORD [IPB+32] = Destination file performance attributes,                    ;
;                  D0 = Read sync, D1 = Write Sync, other reserved             ; 
;                  see MSDN, CreateFileA API parm#6                            ;
; QWORD [IPB+40] = Number of measurement repeats iterations                    ;
; Max. 976 BYTES [IPB+48] = Reserved                                           ; 
; 1024 BYTES [IPB+1024] = Source file path string, null terminated             ;
; 1024 BYTES [IPB+2048] = Destination file path string, null terminated        ;
; 1024 BYTES [IPB+3072] = Reserved                                             ;  
;                                                                              ;
; OPB layout (4096 bytes):                                                     ;
; QWORD [OPB+00] = Memory buffer base address, available after alignment       ;
; QWORD [OPB+08] = Memory buffer size, bytes, available after alignment        ;       
; QWORD [OPB+16] = Source file handle, available after open file               ;
; QWORD [OPB+24] = Destination file handle, available after open file          ;
; QWORD [OPB+32] = Executed read/write/copy operation size,                    ; 
;                  available after operation execute                           ;
; QWORD [OPB+40] = Timer delta for measurement repeats, units = 1 ns           ;
; QWORD [OPB+48] = Last called OS API status, see MSDN                         ;
; QWORD [OPB+56] = Last called OS API type:                                    ;
;    0=None,  1=Open for read,  2=Read,  3=Open for write,  4=Write,           ;
;    5=Close, 6=Delete                                                         ;
;                                                                              ;
; DATA AREA from [OPB+64] to [OPB+128] can be used for                         ; 
; OS-scefific undocumented scratch pads                                        ;                                                                   
; Use OS-specific scratch pad at OPB[80-95], 16 bytes (for timer API)          ;
; 3968 BYTES [OPB+128] = Reserved                                              ;
;                                                                              ;
;------------------------------------------------------------------------------;
MeasureReadFile:
push rbx rsi rdi rbp r12 r13 r14 r15
;--- Clear output variables ---
call HelperClearOPB
;--- Alignment stack and reserve stack space for WinAPI, 32 bytes ---
mov rbp,rsp                   ; save RSP
and rsp,0FFFFFFFFFFFFFFF0h    ; align 16
push rbp          ; must be aligned by 16 with next operation
sub rsp,32+24     ; make parameter shadow plus >4 parameters for Win API
;--- Calculate aligned base and size for memory buffer ---
call HelperDTA                ; RBX = Work buffer base, RBP = Work buffer size
;--- Open file for Read, note create file function used for open file ---
call HelperOpenSrcRead
mov r12,rax                    ; R12 = File handle
test rax,rax                   ; RAX = 0 is wrong return
jz @f                          ; Go with ZF=1 if ZF=1
cmp rax,INVALID_HANDLE_VALUE   ; RAX = -1 if typical errors detected 
@@:                            ; Here flag ZF=1(Z) if error, ZF=0(NZ) if OK
mov eax,STEP_OPEN_READ 
jz ErrorFileContext            ; Here ZF=1(Z) if any error condition detected 
;--- Start measurement cycle, get and store start timer value ---
mov r13,IPB_ITERATIONS
xor r14,r14                    ; R14 = Accumulate size, include meas. cycle
lea rcx,OPB_TRANSIT_T1
call [GetSystemTimeAsFileTime]
;--- Measurement iteration ---
.ReadMeasurement:
xor r15,r15
mov rbx,OPB_BUFFER_BASE
;--- Read file ---
.ReadBlock:
mov rcx,r12                    ; RCX = Parm#1 = Target file handle
mov rdx,rbx                    ; RDX = Parm#2 = Pointer to buffer
mov r8,rbp                     ; R8  = Parm#3 = Block size
lea r9,PARM_6                  ; R9  = Parm#4 = Pointer for size return
mov PARM_5,0                   ; Parm#5 = Pointer to overlapped, not used
call [ReadFile]
test rax,rax
mov eax,STEP_READ 
jz ErrorFileContext
mov rax,PARM_6
test rax,rax
jz .ReadDone                   ; Go done if zero size returned
js .ReadDone                   ; Go done if negative size returned
;--- Cycle for blocks ---
add r14,rax
add r15,rax
cmp r15,IPB_REQUEST_SIZE
jb .ReadBlock
;--- End measurement cycle, get and store stop timer value ---
.ReadDone:
;--- Re-positioning to file begin ---
mov rcx,r12                    ; RCX = Parm#1 = Target file handle
xor edx,edx                    ; RDX = Parm#2 = Distance Low = 0
xor r8d,r8d                    ; R8  = Parm#3 = Distance High = 0
xor r9d,r9d                    ; R9  = Parm#4 = Move method = FILE_BEGIN
call [SetFilePointer]
cmp rax,INVALID_SET_FILE_POINTER
mov eax,STEP_READ 
je ErrorFileContext
;--- Count measurement iterations, set total read size include meas. cycle ---
dec r13
jnz .ReadMeasurement
mov OPB_OPERATION_SIZE,r14
lea rcx,OPB_TRANSIT_T2
call [GetSystemTimeAsFileTime]
call HelperDeltaTime
;--- Close file after Read ---
mov rcx,r12                    ; RCX = Parm#1 = Pointer to file path
call [CloseHandle]
test rax,rax
mov eax,STEP_CLOSE
jz ErrorFileContext 
;--- Restore stack after alignment ---
ExitFileContext:
add rsp,32+24     ; remove parameters shadow plus >4 parameters for Win API
pop rbp
mov rsp,rbp       ; restore RSP after align 16 
;--- Exit ---
pop r15 r14 r13 r12 rbp rdi rsi rbx
ret
;--- Errors handling ---
ErrorFileContext:
mov OPB_LAST_OPERATION,rax
call [GetLastError]
mov OPB_LAST_ERROR,rax
jmp ExitFileContext 
