;---------- Read scenario file ---------------------------------------------;
;                                                                           ;
; INPUT: ECX = Pointer to scenario file path and name,                      ;
;              0-terminated ASCII string                                    ;
;        EDX = Pointer to scenario handle                                   ;
;        ESI = Pointer to loaded scenario base address variable,            ; 
;              this variable is buffer base address for file read           ;
;        EDI = Pointer to scenario size variable,                           ; 
;              this variable is size limit for this buffer                  ;   
;                                                                           ;
; OUTPUT: EAX = OS API last operation status code                           ;
;         Variable scenario handle at [input EDX] = updated by file open    ;
;         Variable scenario size at [input EDI] = Read size, 0 if error     ;
;---------------------------------------------------------------------------;

READ_SIZE   EQU  dword [ebp + 0]
READ_BASE   EQU  dword [ebp + 4]
SIZE_LIMIT  EQU  dword [ebp + 8] 

ReadScenario:

; Entry
push ebx ebp ebp ebp ebp     ; 3 last pushes for variables
mov ebp,esp
mov ebx,edx                  ; EBX = non volatile pointer to scenario handle

; Open file, by input parameters: ECX = Pointer to file name 
xor eax,eax                  ; EAX = 0 for store result = 0 if ScenarioName = 0
jecxz .L2                    ; Skip operation if file name pointer = 0

push eax                     ; Parm #7 = Template file, not used
push FILE_ATTRIBUTE_NORMAL   ; Parm #6 = File attributes
push OPEN_EXISTING           ; Parm #5 = Creation/Open disposition
push eax                     ; Parm #4 = Security attributes, not used
push eax                     ; Parm #3 = Share mode, not used
push GENERIC_READ            ; Parm #2 = Desired access
push ecx                     ; Parm #1 = Pointer to file name
call [CreateFileA]
mov [ebx],eax                ; Save scenario file handle

; Initializing for read file
mov READ_SIZE,0          ; READ_SIZE  = 0, clear read size counter
mov eax,[esi]
mov READ_BASE,eax        ; READ_BASE  = Base address of memory buffer
mov eax,[edi]
mov SIZE_LIMIT,eax       ; SIZE_LIMIT = Size limit of memory buffer

; Read file
.L10:
mov ecx,[ebx]            ; ECX = File handle
jecxz .L2                ; Skip read and close if handle = 0 (if open error)
xor eax,eax
push eax                 ; This push = space for output variable = dword
mov edx,esp
push eax                 ; Parm #5 = Pointer to overlapped str., not used
push edx                 ; Parm #4 = Pointer to output size
push SIZE_LIMIT          ; R8  = Parm #3 = Buffer size limit
push READ_BASE           ; Parm #2 = Buffer base address for read
push ecx                 ; Parm #1 = File handle
call [ReadFile]
pop ecx                  ; ECX = Output size, EAX = Output status

; Analusing read results
test eax,eax
jz .L2                   ; Go error if OS status = 0
jecxz .L3                ; Go normal read termination if returned size = 0
test ecx,ecx
js .L2                   ; Go error if size negative, note for 32-bit only
add READ_SIZE,ecx        ; Accumulate read size
add READ_BASE,ecx        ; Advance read pointer by returned size
sub SIZE_LIMIT,ecx       ; Subtract current read size from size limit 
ja .L10                  ; Repeat read if return size > 0 and limit not reached 
jb .L2                   ; Error if read size > size limit
                         ; If SIZE_LIMIT = 0, means read OK
; Write result size
.L3:
mov eax,READ_SIZE
mov [edi],eax            ; Write scenario size = file size if read OK 
jmp .L1
.L2:
mov dword [edi],0        ; Write scenario size = 0 if read error
.L1:

; Close file
mov ecx,[ebx]            ; ECX = File handle
jecxz .L0
push ecx                 ; Parm #1 = File handle
call [CloseHandle]
.L0:

; Exit
pop ebp ebp ebp ebp ebx
ret

