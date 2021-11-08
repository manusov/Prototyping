;---------- Memory performance pattern ----------------------------------------;
; Memory access method = " Copy SSE-128 (MOVNTDQA + MOVNTPD) " ,               ;
; mode = non temporal, one of variants, depends on options.                    ;
;                                                                              ;
; INPUT:   ESI = Block #1 pointer (32-bit flat)                                ;
;          EDI = Block #2 pointer (32-bit flat)                                ;
;                For Read, Write, Modify use ESI as Source or Destination      ;
;                For Copy use ESI = Source , EDI = Destination                 ;
;          ECX = Block length, units = instructions                            ;
;          EBX:EBP = Number of measurement repeats, EBX=High32, EBP=Low32      ;
;                                                                              ;
; OUTPUT:  None                                                                ;
;          All registers corrupted                                             ;
;                                                                              ;
;------------------------------------------------------------------------------;
Pattern_NtRCopy_SSE128:
push ebx                 ; EBX = High 32 bits of measurement repeats count
Measurement_NtRCopy_SSE128:
;---------- Prepare big cycle -------------------------------------------------;
; Set pointer to middle of 256-byte interval, +/- offsets is code size optimal,
; because offsets [-128...+127] encoded as one byte
lea eax,[esi+128]        ; EAX = Reload source address
lea ebx,[edi+128]        ; EBX = Reload destination address
mov edx,ecx              ; EDX = Reload length
shr edx,4                ; EDX = Block length, convert from INSTRUCTIONS to 16xINSTRUCTION units
jz Small_NtRCopy_SSE128  ; Go if Length < 16 instructions
;---------- Big cycle ---------------------------------------------------------;
DumpStart_NtRCopy_SSE128:
Block_NtRCopy_SSE128:
movntdqa xmm0,[eax-16*08]
movntpd [ebx-16*08],xmm0
movntdqa xmm1,[eax-16*07]
movntpd [ebx-16*07],xmm1
movntdqa xmm2,[eax-16*06]
movntpd [ebx-16*06],xmm2
movntdqa xmm3,[eax-16*05]
movntpd [ebx-16*05],xmm3
movntdqa xmm4,[eax-16*04]
movntpd [ebx-16*04],xmm4
movntdqa xmm5,[eax-16*03]
movntpd [ebx-16*03],xmm5
movntdqa xmm6,[eax-16*02]
movntpd [ebx-16*02],xmm6
movntdqa xmm7,[eax-16*01]
movntpd [ebx-16*01],xmm7
movntdqa xmm0,[eax+16*00]
movntpd [ebx+16*00],xmm0
movntdqa xmm1,[eax+16*01]
movntpd [ebx+16*01],xmm1
movntdqa xmm2,[eax+16*02]
movntpd [ebx+16*02],xmm2
movntdqa xmm3,[eax+16*03]
movntpd [ebx+16*03],xmm3
movntdqa xmm4,[eax+16*04]
movntpd [ebx+16*04],xmm4
movntdqa xmm5,[eax+16*05]
movntpd [ebx+16*05],xmm5
movntdqa xmm6,[eax+16*06]
movntpd [ebx+16*06],xmm6
movntdqa xmm7,[eax+16*07]
movntpd [ebx+16*07],xmm7
add eax,256                 ; Modify source address
add ebx,256                 ; Modify destination address
dec edx
jnz Block_NtRCopy_SSE128    ; Cycle for block data transfer, DEC/JNZ is faster than LOOP!
DumpStop_NtRCopy_SSE128:
;---------- Prepare tail cycle ------------------------------------------------;
Small_NtRCopy_SSE128:
mov edx,ecx
and edx,00001111b           ; ECX = Extract tail length
jz Measure_NtRCopy_SSE128
;---------- Tail cycle --------------------------------------------------------;
Tail_NtRCopy_SSE128:
movntdqa xmm0,[eax-16*08]
movntpd [ebx-16*08],xmm0
add eax,16                  ; Modify source address
add ebx,16                  ; Modify destination address
dec edx
jnz Tail_NtRCopy_SSE128     ; Cycle for tail data transfer, DEC/JNZ is faster than LOOP!
;---------- Measurement cycle -------------------------------------------------;
Measure_NtRCopy_SSE128:
sub ebp,1                   ; EBX:EBP = Measurement cycle counter
sbb dword [esp],0
mov eax,ebp
or eax,[esp]
jnz Measurement_NtRCopy_SSE128  ; Cycle for measurement, repeat same operation
pop ebx
ret