;---------- Get ACPI information ----------------------------------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
InfoAcpi:
sub rsp,32 + 8
push rsi rdi
lea rcx,[BIND_LIST]
lea rdx,[rcx + BINDLIST.bindMadt]
mov r8d,2
.writeNotAvl:  ; TODO. USE BINDERS FOR THIS OPERATION.
mov ax,STR_N_A
lea rdi,[rdx + BINDACPI.oem]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDACPI.manufact]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDACPI.oemRev]
push rax
call PoolStringWrite
pop rax
lea rdi,[rdx + BINDACPI.comment]
call PoolStringWrite
add rdx,sizeof.BINDACPI
dec r8d
jnz .writeNotAvl
pop rdi rsi
xor eax,eax
cmp [rsi + DYNAIMPORT._EnumSystemFirmwareTables],rax
je .skipAcpiList
cmp [rsi + DYNAIMPORT._GetSystemFirmwareTable],rax
je .skipAcpiList
call HelperAlignment
mov r8d,VALUE_BUFFER_LIMIT  ; R8  = Parm#3 = Buffer size limit
lea rdx,[rbp + 16]          ; RDX = Parm#2 = Pointer to buffer
mov ecx,'IPCA'              ; RCX = Parm#1 = Signature, select data type = ACPI
call [rsi + DYNAIMPORT._EnumSystemFirmwareTables]
test rax,rax
jz .error
cmp rax,VALUE_BUFFER_LIMIT
ja .error
mov rbx,rbp
lea r14,[rbp + 16 + rax]    ; R14 = Pointer to 36-byte entries list
mov [r13],r14               ; [R13] = Limit of 4-byte entries  
mov [rbx],r14
add rbx,16                  ; RBX = Pointer to 4-byte tables signatures list
imul rax,rax,9
lea r12,[r14 + rax]         ; R12 = Pointer to buffer for read table
test r12l,00001111b
jz .scanAcpiList
and r12l,11110000b
add r12,10h 
.scanAcpiList:
mov r9,VALUE_BUFFER_LIMIT   ; R9  = Parm#4 = Buffer size limit  
mov r8,r12                  ; R8  = Parm#3 = Buffer pointer
mov edx,[rbx]               ; RDX = Parm#2 = Table signature
mov ecx,'IPCA'              ; RCX = Parm#1 = Table provider signature
call [rsi + DYNAIMPORT._GetSystemFirmwareTable]
test rax,rax
jz .skipOneTable
cmp rax,VALUE_BUFFER_LIMIT
ja .skipOneTable 
push rsi rdi
cld
mov ecx,9
mov rsi,r12
mov rdi,r14
rep movsd
mov r14,rdi
;--- start of addition for show ACPI tables MADT and SRAT at screen 0 ---------;
lea rcx,[BIND_LIST]
lea rdx,[rcx + BINDLIST.acpiEnable]
or byte [rdx],00000001b
mov r8d,[r12]
cmp r8d,'APIC'
je .madtFound
cmp r8d,'SRAT'
je .sratFound
jmp .skipTable
.madtFound:
or byte [rdx],00000010b
lea rdx,[rcx + BINDLIST.bindMadt]
jmp .copyAcpi
.sratFound:
or byte [rdx],00000100b
lea rdx,[rcx + BINDLIST.bindSrat]
.copyAcpi:
lea rsi,[r12 + 10]
lea rdi,[rdx + BINDACPI.oem]
mov ecx,6
call HelperCopyAcpi
lea rsi,[r12 + 16]
lea rdi,[rdx + BINDACPI.manufact]
mov ecx,8
call HelperCopyAcpi
lea rdi,[rdx + BINDACPI.oemRev]
mov ax,STR_OEM_REV
call PoolStringWrite
mov eax,[r12 + 24]
call HexPrint32
mov ax,0000 + 'h'
stosw 
cmp r8d,'SRAT'
je .sratParse
.madtParse:        ; Interpreting ACPI MADT table, get number of APICs
lea rsi,[r12 + 44] ; RSI = Pointer to APIC list in the MADT
mov ecx,[r12 + 4]
add rcx,r12        ; RCX = Table limit
xor r8d,r8d        ; R8D = Counter of Local APICs (MADT), start clear R8D = 0
xor r9d,r9d        ; R9D = Counter of I/O APICs (MADT), start clear R9D = 0
.madtScanApics:    ; This code for parse ACPI MADT, signature = "APIC"
mov al,[rsi]
cmp al,0           ; 0 = Local APIC
jne @f             ; Go skip if not a Local APIC
test byte [rsi + 4],00000001b
jz @f              ; Go skip if Local APIC disabled
inc r8d            ; counter+1 if local APIC found
@@:
cmp al,1			     ; 1 = I/O APIC
jne @f				     ; Go skip if not a I/O APIC
inc r9d 			     ; +1 if found
@@:
movzx rax,byte [rsi + 1] ; RAX = Size of structure, for skip it and access next
add rsi,rax
cmp rsi,rcx
jb .madtScanApics  ; Cycle for scan structures in the MADT
push rbx
lea rdi,[rdx + BINDACPI.comment]
mov ax,STR_LOCAL_APICS
call PoolStringWrite 
mov bl,0
xchg eax,r8d
call DecimalPrint32
mov ax,', '
stosw
mov ax,STR_IO_APICS
call PoolStringWrite 
xchg eax,r9d
call DecimalPrint32
mov al,0
stosb
pop rbx
jmp .skipTable
.sratParse:
push rbx
; UNDER CONSTRUCTION.
lea rdi,[rdx + BINDACPI.comment]
mov ax,STR_DOMAINS
call PoolStringWrite 
mov bl,0
xchg eax,r8d
call DecimalPrint32
mov ax,', '
stosw
mov ax,STR_CPUS
call PoolStringWrite 
xchg eax,r9d
call DecimalPrint32
mov ax,', '
stosw
mov ax,STR_RAMS
call PoolStringWrite 
xchg eax,r10d
call DecimalPrint32
mov al,0
stosb
pop rbx
.skipTable:
;--- end of addition for show ACPI tables MADT and SRAT at screen 0 -----------;
pop rdi rsi
.skipOneTable:
add rbx,4
cmp rbx,[r13]
jb .scanAcpiList 
mov [DYNA_PTR.listAcpi.objectStart],rbp
mov [DYNA_PTR.listAcpi.objectStop],r14
mov rbp,r14
.skipAcpiList:
add rsp,32 + 8
ret
.error:
add rsp,32 + 8
stc
ret
;--- Build ACPI information for text report and GUI widgets -------------------;
;                                                                              ;
; INPUT:   UNDER CONSTRUCTION                                                  ;
;                                                                              ;
; OUTPUT:  UNDER CONSTRUCTION                                                  ;
;                                                                              ;
;------------------------------------------------------------------------------;
TextAcpi:
;---------- Build text block for ACPI screen, first block = tables list -------;
mov rsi,[DYNA_PTR.listAcpi.objectStart]
mov rcx,[DYNA_PTR.listAcpi.objectStop]
test rcx,rcx
jz .noAcpi
mov rcx,[rsi]
mov rbx,rcx
sub rcx,rsi
sub ecx,16
shr ecx,2
jrcxz .noAcpi
mov [DYNA_PTR.textAcpi1.objectStart],rbp
.acpiScan:
mov rdi,rbp
push rcx
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov eax,00040100h
call HelperCopyString
mov eax,0006080Ah
call HelperCopyString 
mov eax,00081110h
call HelperCopyString 
mov eax,0004201Ch
call HelperCopyString 
mov ax,2D18h
call HelperHexString
mov ax,3920h
call HelperHexString 
mov ax,4708h
call HelperDecimalString
add rbx,36
add rbp,82
pop rcx
loop .acpiScan 
.acpiDone:
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textAcpi1.objectStop],rbp
.noAcpi: 
;--- Build text block for ACPI screen, second block = tables detection --------;
mov r14,[APP_DATA.lockedDataAcpi]
test r14,r14
jz .noAcpiData 
mov rsi,[DYNA_PTR.listAcpi.objectStart]
mov rcx,[DYNA_PTR.listAcpi.objectStop]
jrcxz .noAcpiData
mov rcx,[rsi]
sub rcx,rsi
sub ecx,16
shr ecx,2
jrcxz .noAcpiData
lea rbx,[rsi + 16]
mov [DYNA_PTR.textAcpi2.objectStart],rbp
.acpiDataScan:
mov rdi,rbp
push rcx
mov ecx,80
mov al,' '
rep stosb
mov ax,0A0Dh
stosw
mov eax,00040100h
call HelperCopyString
mov ax,', '
stosw
mov rsi,r14
mov edx,[rbx]
.findAcpi:
cmp byte [rsi],0
je .notFoundAcpi
cmp edx,[rsi]
je .foundAcpi
.skipAcpi:
lodsb
cmp al,0
jne .skipAcpi
jmp .findAcpi
.notFoundAcpi:
mov ax,STR_ACPI_UNKNOWN
call PoolStringWrite
jmp .doneAcpi
.foundAcpi:
add rsi,4
call StringWrite
.doneAcpi:
add rbx,4
add rbp,82
pop rcx
loop .acpiDataScan
.acpiDataDone:
mov al,0
stosb
mov rbp,rdi
mov [DYNA_PTR.textAcpi2.objectStop],rbp
.noAcpiData:
ret
