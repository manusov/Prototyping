ReadSpdAtSmbus:
ret


;---------- User mode caller --------------------------------------------------;

READ_SPD_CONTROL_OFFSET  EQU  0
READ_SPD_DATA_OFFSET     EQU  256
READ_SPD_BUFFER_SIZE     EQU  512
SMBUS_BASE_IO            EQU  0400h  ; This for given platform only.

DebugReadSpdAtSmbus:
;---------- Try load kernel mode driver kmd64.sys -----------------------------;
mov ecx,ebx
call LoadKernelModeDriver
test eax,eax
jz .error
;----------- Blank buffer -----------------------------------------------------;
push edi
add edi,KERNEL_BUFFER_OFFSET + READ_SPD_CONTROL_OFFSET 
mov ecx,READ_SPD_BUFFER_SIZE 
mov al,11h
cld
push ecx
rep stosb
pop ecx edi
;---------- Parameters for KMD request ----------------------------------------;
mov edx,Query
xor ebp,ebp
mov [edx + RZDriverQuery.IOCODE],RZ_DRIVER_QUERY_PROC_REGISTER_2P
mov eax,callbackReadSpdAtSmbus
mov dword [edx + RZDriverQuery.PROC + 0],eax
mov dword [edx + RZDriverQuery.PROC + 4],ebp

lea eax,[edi + KERNEL_BUFFER_OFFSET + READ_SPD_CONTROL_OFFSET]
mov dword [edx + RZDriverQuery.paramA + 0],eax
mov dword [edx + RZDriverQuery.paramA + 4],ebp

mov word [eax + 0],SMBUS_BASE_IO
mov word [eax + 2],0A500h
mov byte [eax + 4],0

lea eax,[edi + KERNEL_BUFFER_OFFSET + READ_SPD_DATA_OFFSET]
mov dword [edx + RZDriverQuery.paramB + 0],eax
mov dword [edx + RZDriverQuery.paramB + 4],ebp

;---------- Driver request use file I/O ---------------------------------------;
push 0
push Bytes
push sizeof.RZDriverQuery 
push edx
push [SCP_DATA.driver]
call [WriteFile]
;---------- Unload kernel mode driver kmd64.sys -------------------------------;
mov ecx,ebx
call UnloadKernelModeDriver
test eax,eax
jz .error
;---------- Memory buffer dump ------------------------------------------------;
lea esi,[edi + KERNEL_BUFFER_OFFSET + READ_SPD_CONTROL_OFFSET]
mov ebx,READ_SPD_BUFFER_SIZE 
call DumpMemory
;---------- Exit points -------------------------------------------------------;
clc
ret
.error:
stc
ret

;---------- Kernel mode callback ----------------------------------------------;

; Debug SPD Read.
; EAX = Pointer to control and status structure:
;       DW  SMBus base I/O
;       DB  SPD byte address
;       DB  SPD EPROM address
;       DB  Status byte
; EDX = Pointer to destination buffer for store one byte 

callbackReadSpdAtSmbus:

; DEBUG
; mov dword [eax],055555555h
; mov dword [edx],0AAAAAAAAh
; DEBUG


mov ecx,256
@@:
cmp byte [eax + 4],0
jne @f
call DebugReadSpd
inc byte [eax + 2]
inc edx
loop @b
@@:
ret

;---------------------------------------------------------;
; Read selected byte from selected SMBus device           ;
; Procedure used for SPD EPROM Read                       ;
;---------------------------------------------------------;
; INPUT:   BH = SMBus device address                      ;
;          BL = Byte address in the device                ;
; OUTPUT:  CF = Error status: 0(NC)=OK, 1(C)=Error        ;
;          AH = SMBus status: 00h=OK, 01h=Error           ;
;               02h-FFh = Reserved for future use         ;
;               AH>0 with CF=0 means no device ACK        ;
;          AL = Read byte                                 ;
;---------------------------------------------------------;
; Note.                                                   ;
; SMBus Address = 0Axh for SPD                            ;
; Bit0: 1=READ, 0=WRITE.                                  ;
; Bits 3,2,1 = SPD Address 0-7.                           ;
; 0A0h = Write SPD 0                                      ;
; 0A1h = Read  SPD 0                                      ;
; 0A2h = Write SPD 1                                      ;
; 0A3h = Read  SPD 1                                      ;
; 0A4h = Write SPD 2                                      ;
; 0A5h = Read  SPD 2                                      ;
; 0A6h = Write SPD 3                                      ;
; 0A7h = Read  SPD 3                                      ;
; 0A8h = Write SPD 4                                      ;
; 0A9h = Read  SPD 4                                      ;
; 0AAh = Write SPD 5                                      ;
; 0ABh = Read  SPD 5                                      ;
; 0ACh = Write SPD 6                                      ;
; 0ADh = Read  SPD 6                                      ;
; 0AEh = Write SPD 7                                      ;
; 0AFh = Read  SPD 7                                      ;
;---------------------------------------------------------;

DebugReadSpd:

pushad
mov esi,eax           ; ESI = Pointer to control structure
mov edi,edx           ; EDI = Pointer to destination buffer
mov	al,10111111b
;--- DX = SMB Controller Base I/O Address ---
mov dx,[esi + 0]
;--- Disable interrupts ---
cli
;--- Clear status bits ---
;--- [Base+0] = SMBus Host Status Register ---
out dx,al
;--- Set address of byte in 24C02 for transmit to 24C02 as command ---
;--- [Base+3] = SMBus Host Command Register ---
add dx,3
mov al,[esi + 2]
out dx,al
;--- Set address for SMBus device (24C02) addressing, and R/W-bit=1=R ---
;--- [Base+4] = SMBus Host Address Register ---
inc dx
mov al,[esi + 3]
out dx,al
;--- Send command for start transaction BYTE DATA READ ---
;--- Read and Write has same command code 48h, but ---
;--- Device Address.D0 = 0:Write, 1:Read ---
;--- [Base+2] = SMBus Host Control Register ---
dec dx
dec dx
mov al,48h
out dx,al
;--- Wait for transaction start and completion ---
;--- [Base+0] = SMBus Host Status Register ---
call SMB_Wait_Start_Stop
jc Set_IF1_AH1  ; Transaction run or stop timeout ?
;--- Read Byte, received from 24C02 and store it at AH ---
;--- [Base+5] = SMBus Host Data 0 Register ---
add dx,5
in al,dx
mov ah,al
;--- Read main status of completed operation ---
;--- [Base+0] = SMBus Host Status Register ---
sub dx,5
;--- Delay-IN required for 82C596 ---
;--- (only IN, without delay required for Intel PIIX4) ---
;--- Read 400 times ---
call SMB_Delayed_Read_for_596
;--- Interpreting main status ---
and al,00011110b
cmp al,00000010b
jne	Set_IF1_AH1

;--- Return if no errors, AL=BYTE ---
mov al,ah
Set_IF1_AH0:
sti
xor ah,ah		; Make AH=0, CF=0 (NC)
cld
stosb
jmp SMB_Ret

;--- Return if no ACK, but not fatal error  ---
Set_IF1_AH1:
sti
mov ah,1

SMB_Ret:
mov [esi + 4],ah
popad
ret

;---------------------------------------------------------------;
; Wait for SMBus Transaction Start and Stop                     ;
;---------------------------------------------------------------;
; INPUT:   DX = SMBus Host Status Register address +2           ;
;               Subtract 2 used                                 ;
; OUTPUT:  CF = Error indicator:                                ;
;               0(NC) = No errors                               ;
;               1(C)  = Start or Stop Timeout                   ;
;          DX = DX input -2                                     ;
;          AX = Corrupted                                       ;
;          CX = Corrupted                                       ;
;---------------------------------------------------------------;
SMB_Wait_Start_Stop:
dec dx
dec dx		; DX-2
;--- [Base+0] = SMBus Host Status Register ---
;--- Wait for transaction start ---
mov ecx,10000h
SMB_Wait_Start:
in al,dx
test al,1		; Check Bit0=BUSY=1
loopz SMB_Wait_Start
jz SMB_Error_StSt	; Transaction run timeout ?
;--- Wait for transaction stop ---		
mov ecx,10000h
SMB_Wait_Stop:
in al,dx
test al,1		; Check Bit0=BUSY=0
loopnz SMB_Wait_Stop
jnz SMB_Error_StSt	; Transaction stop timeout ?
;--- Exit points ---
clc
ret
SMB_Error_StSt:
stc
ret

;---------------------------------------------------------------;
; Read from I/O Port [DX] with delay                            ;
; Delay required for VIA 82C596 (not required for Intel PIIX4)  ;
; Read 400 times                                                ;
;---------------------------------------------------------------;
; INPUT:   DX = I/O Port Address                                ;
; OUTPUT:  AL = Byte from Port [DX]                             ;
;          CX = Corrupted = 0000h                               ;
;---------------------------------------------------------------;
SMB_Delayed_Read_for_596:
mov	ecx,400
SMB_Delay_596:
in al,dx
loop SMB_Delay_596
ret




