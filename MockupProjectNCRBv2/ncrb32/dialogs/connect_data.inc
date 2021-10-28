;------------------------------------------------------------------------------;
;                                                                              ;
;  Set of dialogue callback routines (window procedures) for application GUI:  ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;

;---------- Pointers to dialogues callbacks procedures, per each tab sheet ----;

ProcDialogs   DD  DialogProcSysinfo
              DD  DialogProcMemory
              DD  DialogProcOs
              DD  DialogProcNativeOs
              DD  DialogProcTopology
              DD  DialogProcTopologyEx
              DD  DialogProcNuma
              DD  DialogProcGroups
              DD  DialogProcAcpi
              DD  DialogProcAffCpuid

;---------- Data for Vector Brief test scenario -------------------------------;
; TODO. PORT COMMENTS FROM NCRB V1.

; id = routine ID at bits [6-0], bit 7 : 0 = BandwidthTemporalPattern, 1 = Math pattern
; count = instruction count per work cycle = per 4KB block, power of 2
; repeats = measurement cycle count, required * REPEATS_SCALE 
; cpuValid = bit number at CPUDATA.extractedFeaturesBitmap, 0FFh means don't check (x87 check is redundant, because run criteria)
; osValid  = bit mask at CPUDATA.extractedContextBitmap, 0FFh means don't check  (include SSE without XCR0) 

MACRO VECLK id, count, repeats, cpuValid, osValid 
{
DB  id          ; ID of measurement routine
DB  count       ; Instruction count per work cycle = per 4KB block, power of 2
DB  repeats     ; Measurement cycle count, required * REPEATS_SCALE
DB  cpuValid    ; CPU (CPUID) check criteria to run this
DB  osValid     ; OS (XCR0) check criteria to run this
}

; Equations for performance patterns select
ID_READ_SSE128   EQU 00h + 9    
ID_WRITE_SSE128  EQU 00h + 10  
ID_COPY_SSE128   EQU 00h + 11
ID_READ_AVX256   EQU 00h + 12
ID_WRITE_AVX256  EQU 00h + 13
ID_COPY_AVX256   EQU 00h + 14
ID_READ_AVX512   EQU 00h + 15
ID_WRITE_AVX512  EQU 00h + 16
ID_COPY_AVX512   EQU 00h + 17
ID_SQRT_SSE128   EQU 80h + 0   
ID_SQRT_AVX256   EQU 80h + 1
ID_SQRT_AVX512   EQU 80h + 2
ID_COS_X87       EQU 80h + 3
ID_SINCOS_X87    EQU 80h + 4

vectorBrief:
VECLK  ID_READ_SSE128  , 8 , 160 , 01 , 0FFh   ; SSE   
VECLK  ID_WRITE_SSE128 , 8 , 160 , 01 , 0FFh   ; SSE  
VECLK  ID_COPY_SSE128  , 7 , 160 , 01 , 0FFh   ; SSE 
VECLK  ID_READ_AVX256  , 7 , 160 , 07 , 003h   ; AVX
VECLK  ID_WRITE_AVX256 , 7 , 160 , 07 , 003h   ; AVX  
VECLK  ID_COPY_AVX256  , 6 , 160 , 07 , 003h   ; AVX  
VECLK  ID_READ_AVX512  , 6 , 160 , 09 , 01Fh   ; AVX512 
VECLK  ID_WRITE_AVX512 , 6 , 160 , 09 , 01Fh   ; AVX512  
VECLK  ID_COPY_AVX512  , 5 , 160 , 09 , 01Fh   ; AVX512   
VECLK  ID_SQRT_SSE128  , 8 ,  40 , 02 , 0FFh   ; SSE2 
VECLK  ID_SQRT_AVX256  , 8 ,  40 , 07 , 003h   ; AVX   
VECLK  ID_SQRT_AVX512  , 8 ,  40 , 09 , 01Fh   ; AVX512 
VECLK  ID_COS_X87      , 8 ,   4 , 16 , 0FFh   ; x87     
VECLK  ID_SINCOS_X87   , 8 ,   4 , 16 , 0FFh   ; x87  


;------------------------------------------------------------------------------;
; TODO. Port.
; TODO. Move this to resources.


BasePointGui:


;------------------------------------------------------------------------------;
;            BITMAPINFOHEADRE descriptor structure (for Win64).                ;
;------------------------------------------------------------------------------;
; This structure used when initializing graphics window context, drawings Y=F(X)
;--------------------------------------------------------------
; Name		          Size    Comments
;--------------------------------------------------------------
; biSize           dd ?   Number of bytes
; biWidth          dd ?   Width
; biHeight         dd ?   Height
; biPlanes         dw ?   Bit Planes
; biBitCount       dw ?   Bits Per Pixel
; biCompression    dd ?   Compression Mode
; biSizeImage      dd ?   Image size, not required for uncompressed
; biXPelsPerMeter  dd ?   X pixels per meter, here not required
; biYPelsPerMeter  dd ?   Y pixels per meter, here not required
; biClrUsed        dd ?   Number of colors used, here not required (true color)
; biClrImportant   dd ?   Number of important colors, 0=All, here not used
;                         Important colors - used for visualization.
;                         0 means all colors used for visualization.
;--------------------------------------------------------------
BitmapInfo    BITMAPINFOHEADER 40,SUBWINX,SUBWINY,1,32,BI_RGB,0,0,0,0,0

;------------------------------------------------------------------------------;
;                             GUI color brushes.                               ;
;------------------------------------------------------------------------------;
; Used for graphics window, drawings Y=F(X)
; Brush color values = 00bbggrrh, bb=blue, gg=green, rr=red, 1 byte per color
BrushesList:
DD  BRUSH_GRID         ; Grid with horizontal and vertical lines 
DD  BRUSH_LINE         ; Draw Line Speed = F (Block Size)
DD  BRUSH_BACKGROUND   ; Draw window background
DD  BRUSH_STATISTICS   ; Statistics table lines

;------------------------------------------------------------------------------;
;               Strings for Window 1 - benchmarks drawings.                    ; 
;------------------------------------------------------------------------------;
; Window name, visualized at top of GUI window
WindowTitle_Win1  DB  'Performance = F (Block Size)',0
; Text strings for write timings in the drawings window
; 12 (not 9) for blank area with variable (font char defined) Xsize
; CPI = Clocks Per Instruction
DrawTextTSC       DB  'TSC clock (MHz)  = '
DrawValueTSC      DB  12 DUP (' '), 0

MACRO SHORTPOINTER x1
{
DW x1 - BasePointGui
}

MACRO SHORTPOINTEREND
{
DW 0
}

DrawTableUp:
SHORTPOINTER DrawTextCPI
SHORTPOINTER DrawTextNSPI
SHORTPOINTER DrawTextMBPS
SHORTPOINTEREND

DrawTableLeft:
SHORTPOINTER DrawTextMinimum
SHORTPOINTER DrawTextMaximum
SHORTPOINTER DrawTextAverage
SHORTPOINTER DrawTextMedian
SHORTPOINTEREND


DrawTextCPI       DB  'CPI',0
DrawTextNSPI      DB  'nsPI',0
DrawTextMBPS      DB  'MBPS',0

DrawTextMinimum   DB  'minimum',0
DrawTextMaximum   DB  'maximum',0
DrawTextAverage   DB  'average',0
DrawTextMedian    DB  'median',0


;------------------------------------------------------------------------------;
; Memory size and speed units.                                                 ;
;------------------------------------------------------------------------------;
U_B            DB  'Bytes',0
U_KB           DB  'KB',0
U_MB           DB  'MB',0
U_GB           DB  'GB',0
U_TB           DB  'TB',0
U_MBPS         DB  'MBPS',0
U_NS           DB  'nanoseconds',0


;------------------------------------------------------------------------------;

