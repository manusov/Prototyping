;==============================================================================;
;                                                                              ;
;         Registry of global variables and data blocks, for x64 code.          ;
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
virtual at r8 
tp                   TOKEN_PRIVILEGES 
end virtual
;---------- NCRB registry definitions -----------------------------------------;
REGISTRY64_MEMORY_SIZE     = 1024 * 1024
TEMP_BUFFER_INIT_SIZE      = 48 * 1024
BIND_BUFFER_INIT_SIZE      = 8 * 1024
;---------- Allocator for data block with variable base address and size ------;
struct ALLOCATOR
objectStart                dq ?
objectStop                 dq ?
ends
;---------- Dynamical imported WinAPI functions pointers list -----------------;
struct DYNAIMPORT
_GlobalMemoryStatusEx              dq ?   ; This functions from KERNEL32.DLL          
_GetNativeSystemInfo               dq ?
_GetLogicalProcessorInformation    dq ?
_GetLogicalProcessorInformationEx  dq ?
_GetActiveProcessorGroupCount      dq ?  
_GetActiveProcessorCount           dq ?       
_GetLargePageMinimum               dq ?
_GetNumaHighestNodeNumber          dq ?
_GetNumaNodeProcessorMask          dq ?
_GetNumaAvailableMemoryNode        dq ?
_GetNumaNodeProcessorMaskEx        dq ?
_GetNumaAvailableMemoryNodeEx      dq ?
_EnumSystemFirmwareTables          dq ?
_GetSystemFirmwareTable            dq ?
_OpenProcessToken                  dq ?   ; This functions from ADVAPI32.DLL              
_AdjustTokenPrivileges             dq ?         
ends
;---------- Key data for GUI application with resources -----------------------;  
struct APPDATA
hAdvapi32                  dq ?     ; ADVAPI32.DLL handle
hResources                 dq ?     ; Resource DLL handle
lockedStrings              dq ?     ; Pointer to strings pool
lockedBinders              dq ?     ; Pointer to binders pool
lockedDataCpuCommon        dq ?     ; Data for build common CPU feature bitmap
lockedDataCpuAvx512        dq ?     ; Data for build AVX512 feature bitmap
lockedDataOsContext        dq ?     ; Data for build OS context bitmap
lockedDataCpuMethod        dq ?     ; Data for build CPU methods bitmap
lockedDataAcpi             dq ?     ; Data base for ACPI tables detection
lockedImportList           dq ?     ; List for WinAPI dynamical import
lockedFontList             dq ?     ; List of fonts names
hFont1                     dq ?     ; Handles of created fonts
hFont2                     dq ?
hIcon                      dq ?     ; Application icon handle
hMain                      dq ?     ; Main window handle
hTab                       dq ?     ; Sheets container handle
hImageList                 dq ?     ; Image list handle
selectedTab                dd ?                   ; Current sheet number
tabCtrlItem                TC_ITEM ?              ; Tab item data structure
lockedIcons                dq ICON_COUNT dup (?)  ; Pointers to icons
hTabDlg                    dq ITEM_COUNT dup (?)  ; Sheets handles
hInstance                  dq ?                   ; This EXE file handle
ends
;---------- Operating system information --------------------------------------;
struct OSDATA
memoryStatusEx             MEMORYSTATUSEX_DEF ?
systemInfo                 SYSTEM_INFO        ?
nativeSystemInfo           SYSTEM_INFO        ?
activeProcessorGroupCount  dd ?
activeProcessorCount       dd ?
numaNodeCount              dd ?
largePageSize              dq ?
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
pVendorString              dq ?
pSignatureString           dq ?
pModelString               dq ?
pTscString                 dq ?
ends

;---------- Root structure: NCRB64 application registry -----------------------; 
struct REGISTRY64
allocatorTempBuffer        ALLOCATOR  ?
allocatorBindBuffer        ALLOCATOR  ?       
appData                    APPDATA    ?
osData                     OSDATA     ?
cpuData                    CPUDATA    ?
dynaImport                 DYNAIMPORT ?
listTopology               ALLOCATOR  ?
listTopologyEx             ALLOCATOR  ?
listNuma                   ALLOCATOR  ?
listGroup                  ALLOCATOR  ?
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
textGroup                  ALLOCATOR  ?
textAcpi1                  ALLOCATOR  ?
textAcpi2                  ALLOCATOR  ?
textAffCpuid               ALLOCATOR  ?
textKmd1                   ALLOCATOR  ?
textKmd2                   ALLOCATOR  ?
unformatted                db ?
ends

