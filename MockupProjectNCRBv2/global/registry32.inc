;------------------------------------------------------------------------------;
;                                                                              ;
;         Registry of global variables and data blocks, for ia32 code.         ;
;                                                                              ;
;------------------------------------------------------------------------------;

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

;---------- Structure for service request execution status (see MSDN) ---------;

struct SERVICE_STATUS
dwServiceType              dd ?     ; Type of system service
dwCurrentState             dd ?     ; State of service, run/stop/pause
dwControlsAccepted         dd ?     ; Accepted service operations flags
dwWin32ExitCode            dd ?     ; Service unified error code
dwServiceSpecificExitCode  dd ?     ; Service-specific error code
dwCheckPoint               dd ?     ; Incremnted progress indicator value
dwWaitHint                 dd ?     ; Estimated time of operation for tracking
ends

;---------- Structure for driver query ----------------------------------------;

struct SERVICE_QUERY
iocode    dd ?    ; user I/O code, request type selector
iodata    dd ?    ; user I/O data, request input parameter 
userproc  dq ?    ; procedure offset, callback address
parm1     dq ?    ; parameter A, callback routine optional input parameter 1
parm2     dq ?    ; parameter B, callback routine optional input parameter 2
result    dq ?    ; result, usage example: AL after IN AL,DX
buffer    db RZ_DRIVER_QUERY_BUFFER_SIZE dup (?)
ends

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
_IsWow64Process                    dd ?   ; This functions from KERNEL32.DLL
_GlobalMemoryStatusEx              dd ?          
_GetNativeSystemInfo               dd ?
_GetLogicalProcessorInformation    dd ?
_GetLogicalProcessorInformationEx  dd ?
_GetActiveProcessorGroupCount      dd ?  
_GetActiveProcessorCount           dd ?       
_GetLargePageMinimum               dd ?
_GetNumaHighestNodeNumber          dd ?
_GetNumaNodeProcessorMask          dd ?
_GetNumaAvailableMemoryNode        dd ?
_GetNumaNodeProcessorMaskEx        dd ?
_GetNumaAvailableMemoryNodeEx      dd ?
_EnumSystemFirmwareTables          dd ?
_GetSystemFirmwareTable            dd ?
_SetThreadAffinityMask             dd ?
_OpenProcessToken                  dd ?   ; This functions from ADVAPI32.DLL              
_AdjustTokenPrivileges             dd ?         
ends

;---------- Key data for GUI application with resources -----------------------;  

struct APPDATA
; hAdvapi32                dd ?     ; ADVAPI32.DLL handle
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
isWow64                    dd ?
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

;---------- Kernel Mode Driver Service Control Program information ------------;

struct SCPDATA
drvPath                    dd ?
drvFile                    dd ?               
manager                    dd ?               
service                    dd ?               
vectors                    dd ?               
driver                     dd ?                
bytes                      dd ?                 
status                     SERVICE_STATUS ?   ; Driver status structure
query                      SERVICE_QUERY  ?   ; Driver request structure 
ends

;---------- List of pointers to strings used by GUI binders -------------------;
; This structure used as template for offsets in the bind buffer.

; TODO. Make this instead BUFFER_CPU_VENDOR ... BUFFER_VIEWER_A_CPU ?
; struct BINDLIST
; pVendorString              dd ?
; pSignatureString           dd ?
; pModelString               dd ?
; pTscString                 dd ?
; ends

;---------- Root structure: NCRB32 application registry -----------------------;  

struct REGISTRY32
allocatorTempBuffer        ALLOCATOR  ?
allocatorBindBuffer        ALLOCATOR  ?
appData                    APPDATA    ?
osData                     OSDATA     ?
cpuData                    CPUDATA    ?
scpData                    SCPDATA    ?
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

