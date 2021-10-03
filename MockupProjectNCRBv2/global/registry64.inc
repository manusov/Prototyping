;------------------------------------------------------------------------------;
;                                                                              ;
;         Registry of global variables and data blocks, for x64 code.          ;
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
_IsWow64Process                    dq ?   ; This functions from KERNEL32.DLL
_GlobalMemoryStatusEx              dq ?          
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
_SetThreadAffinityMask             dq ?
_OpenProcessToken                  dq ?   ; This functions from ADVAPI32.DLL              
_AdjustTokenPrivileges             dq ?         
ends

;---------- Key data for GUI application with resources -----------------------;  

struct APPDATA
; hAdvapi32                dq ?     ; ADVAPI32.DLL handle
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

;---------- Kernel Mode Driver Service Control Program information ------------;

struct SCPDATA
drvPath                    dq ?
drvFile                    dq ?               
manager                    dq ?               
service                    dq ?               
vectors                    dq ?               
driver                     dq ?                
bytes                      dq ?                 
status                     SERVICE_STATUS ?   ; Driver status structure
query                      SERVICE_QUERY  ?   ; Driver request structure 
ends

;---------- List of pointers to strings used by GUI binders -------------------;
; This structure used as template for offsets in the bind buffer.

; TODO. Make this instead BUFFER_CPU_VENDOR ... BUFFER_VIEWER_A_CPU ? 
; struct BINDLIST
; pVendorString              dq ?
; pSignatureString           dq ?
; pModelString               dq ?
; pTscString                 dq ?
; ends

;---------- Root structure: NCRB64 application registry -----------------------; 

struct REGISTRY64
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

