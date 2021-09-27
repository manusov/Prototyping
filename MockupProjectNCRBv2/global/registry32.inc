;==============================================================================;
;                                                                              ;
;         Registry of global variables and data blocks, for ia32 code.         ;
;                                                                              ;
;==============================================================================;

;---------- Operating system constants and structures definition --------------;
ALL_PROCESSOR_GROUPS   = 0000FFFFh  
struct MEMORYSTATUSEX_DEF
dwLength                 dd ?
dwMemoryLoad             dd ?
ullTotalPhys             dq ?
ullAvailPhys             dq ?
ullTotalPageFile         dq ?
ullAvailPageFile         dq ?
ullTotalVirtual          dq ?
ullAvailVirtual          dq ?
ullAvailExtendedVirtual  dq ?
ends
;---------- Operating system definitions for large pages support --------------;
MEM_LARGE_PAGES          =  020000000h 
SE_PRIVILEGE_ENABLED     = 2 
SE_LOCK_MEMORY_PRIVILEGE = 4
struct LUID 
usedpart             dd ?   
ignorehigh32bitpart  dd ? 
ends 
struct LUID_AND_ATTRIBUTES 
Luid                 LUID 
Attributes           dd ?  
ends 
struct TOKEN_PRIVILEGES 
PrivilegeCount       dd ? 
Privileges           LUID_AND_ATTRIBUTES 
ends 
virtual at esi 
tp                   TOKEN_PRIVILEGES 
end virtual
;---------- NCRB registry definitions -----------------------------------------;
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
_GlobalMemoryStatusEx              dd ?   ; This functions from KERNEL32.DLL          
_GetNativeSystemInfo               dd ?
_GetLogicalProcessorInformation    dd ?
_GetLogicalProcessorInformationEx  dd ?
_GetActiveProcessorGroupCount      dd ?  
_GetActiveProcessorCount           dd ?       
_GetNumaHighestNodeNumber          dd ?      
_GetLargePageMinimum               dd ?
_EnumSystemFirmwareTables          dd ?
_GetSystemFirmwareTable            dd ?
_OpenProcessToken                  dd ?   ; This functions from ADVAPI32.DLL              
_AdjustTokenPrivileges             dd ?         
ends
;---------- Key data for GUI application with resources -----------------------;  
struct APPDATA
hAdvapi32                  dd ?     ; ADVAPI32.DLL handle
hResources                 dd ?     ; Resource DLL handle
lockedStrings              dd ?     ; Pointer to strings pool
lockedBinders              dd ?     ; Pointer to binders pool
lockedDataCpuCommon        dd ?     ; Data for build common CPU feature bitmap
lockedDataCpuAvx512        dd ?     ; Data for build AVX512 feature bitmap
lockedDataOsContext        dd ?     ; Data for build OS context bitmap
lockedDataCpuMethod        dd ?     ; Data for build CPU methods bitmap
lockedDataAcpi             dd ?     ; Data base for ACPI tables detection
lockedImportList           dd ?     ; List for WinAPI dynamical import
lockedFontList             dd ?     ; List of fonts names
hFont1                     dd ?     ; Handles of created fonts
hFont2                     dd ?
hIcon                      dd ?     ; Application icon handle
hMain                      dd ?     ; Main window handle
hTab                       dd ?     ; Sheets container handle
hImageList                 dd ?     ; Image list handle
selectedTab                dd ?                   ; Current sheet number
tabCtrlItem                TC_ITEM ?              ; Tab item data structure
lockedIcons                dd ICON_COUNT dup (?)  ; Pointers to icons
hTabDlg                    dd ITEM_COUNT dup (?)  ; Sheets handles
hInstance                  dd ?                   ; This EXE file handle
ends
;---------- Operating system information --------------------------------------;
struct OSDATA
memoryStatusEx             MEMORYSTATUSEX_DEF ?
systemInfo                 SYSTEM_INFO        ?
nativeSystemInfo           SYSTEM_INFO        ?
activeProcessorGroupCount  dd ?
activeProcessorCount       dd ?
numaNodeCount              dd ?
largePageSize              dd ?
largePageEnable            dd ?
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
osData                     OSDATA     ?
cpuData                    CPUDATA    ?
dynaImport                 DYNAIMPORT ?
listTopology               ALLOCATOR  ?
listTopologyEx             ALLOCATOR  ?
listNuma                   ALLOCATOR  ?
listGroups                 ALLOCATOR  ?
listAcpi                   ALLOCATOR  ?
listAffCpuid               ALLOCATOR  ?
listKmd                    ALLOCATOR  ?
textOs                     ALLOCATOR  ?
textNativeOs               ALLOCATOR  ?
textProcessor              ALLOCATOR  ?
textTopology1              ALLOCATOR  ?
textTopology2              ALLOCATOR  ?
textTopologyEx1            ALLOCATOR  ?
textTopologyEx2            ALLOCATOR  ?
textNuma                   ALLOCATOR  ?
textGroups                 ALLOCATOR  ?
textAcpi1                  ALLOCATOR  ?
textAcpi2                  ALLOCATOR  ?
textAffCpuid               ALLOCATOR  ?
textKmd1                   ALLOCATOR  ?
textKmd2                   ALLOCATOR  ?
unformatted                db ?
ends

