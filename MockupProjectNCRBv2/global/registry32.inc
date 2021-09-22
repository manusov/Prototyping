;==============================================================================;
;                                                                              ;
;         Registry of global variables and data blocks, for ia32 code.         ;
;                                                                              ;
;==============================================================================;

;---------- Definitions -------------------------------------------------------;
REGISTRY32_MEMORY_SIZE     = 1024 * 1024
TEMP_BUFFER_INIT_SIZE      = 48 * 1024
BIND_BUFFER_INIT_SIZE      = 8 * 1024
;---------- Allocator for data block with variable base address and size ------; 
struct ALLOCATOR
objectStart                dd ?
objectStop                 dd ?
ends
;---------- Dynamical imported WinAPI functions pointers list -----------------;
struct DYNAIMPORT
_EnumSystemFirmwareTables  dd ?
_GetSystemFirmwareTable    dd ?
ends
;---------- Key data for GUI application with resources -----------------------;  
struct APPDATA
hAdvapi32                  dd ?                   ; ADVAPI32.DLL handle
hResources                 dd ?                   ; Resource DLL handle
lockedStrings              dd ?                   ; Pointer to strings pool
lockedBinders              dd ?                   ; Pointer to binders pool
lockedDataCpuCommon        dd ?     ; Data for build common CPU feature bitmap
lockedDataCpuAvx512        dd ?     ; Data for build AVX512 feature bitmap
lockedDataOsContext        dd ?     ; Data for build OS context bitmap
lockedDataCpuMethod        dd ?     ; Data for build CPU methods bitmap
lockedDataAcpi             dd ?     ; Data base for ACPI tables detection
hIcon                      dd ?                   ; Application icon handle
hMain                      dd ?                   ; Main window handle
hTab                       dd ?                   ; Sheets container handle
hImageList                 dd ?                   ; Image list handle
selectedTab                dd ?                   ; Current sheet number
tabCtrlItem                TC_ITEM ?              ; Tab item data structure
lockedIcons                dd ICON_COUNT dup (?)  ; Pointers to icons
hTabDlg                    dd ITEM_COUNT dup (?)  ; Sheets handles
hInstance                  dd ?                   ; This EXE file handle
ends
;---------- Processor detection results ---------------------------------------;
struct CPUDATA
vendorString               db 13 dup ?
modelString                db 49 dup ?
cpuSignature               dd ?
extractedFeaturesBitmap    dq ?
extractedAvx512Bitmap      dq ?
extractedContextBitmap     dq ?
extractedMethodsBitmap     dq ?
tscClockHz                 dq ?
ends
;---------- List of pointers to strings used by GUI binders -------------------;
; This structure used as template for offsets in the bind buffer.
struct BINDLIST
pVendorString              dd ?
pSignatureString           dd ?
pModelString               dd ?
pTscString                 dd ?
ends

;---------- Root structure: NCRB32 application registry -----------------------;  
struct REGISTRY32
allocatorTempBuffer        ALLOCATOR  ?
allocatorBindBuffer        ALLOCATOR  ?
appData                    APPDATA    ?
cpuData                    CPUDATA    ?
dynaImport                 DYNAIMPORT ?
listNuma                   ALLOCATOR  ?
listGroups                 ALLOCATOR  ?
ends

