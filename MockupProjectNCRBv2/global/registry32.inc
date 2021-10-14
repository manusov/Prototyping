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

;--- Application definitions for Kernel Mode Driver Service Control Program ---;
 
; SCP_TEXT_BUFFER_SIZE = 512   ; Buffer size for text strings build
SCP_PATH_BUFFER_SIZE = 260     ; Buffer size for driver file string path build

;---------- Kernel Mode Driver (KMD) definitions ------------------------------;

RZ_DRIVER_QUERY_BUFFER_SIZE = 24     ; Buffer size for driver request structure
RZ_REQUEST_CODE             = 41h    ; Driver request code = user routine call 

;---------- Service Control Program (SCP) definitions -------------------------;

SC_MANAGER_ALL_ACCESS = 0000F003Fh   ; Used as desired access rights for SCM
SERVICE_ALL_ACCESS    = 0000F01FFh   ; Used as desired acc. rights for service
SERVICE_KERNEL_DRIVER = 000000001h   ; Used as service type for service
SERVICE_DEMAND_START  = 000000003h   ; Used as service start option for service
SERVICE_ERROR_NORMAL  = 000000001h   ; Used as error control option for service
SERVICE_CONTROL_STOP  = 000000001h   ; Used as control code for stop service
SERVICE_RUNNING       = 000000004h   ; Used for detect service current state


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
buffer    db RZ_DRIVER_QUERY_BUFFER_SIZE dup ?
ends

;---------- NCRB registry definitions -----------------------------------------;

REGISTRY32_MEMORY_SIZE     = 1024 * 1024
TEMP_BUFFER_INIT_SIZE      = 48 * 1024
BIND_BUFFER_INIT_SIZE      = 8 * 1024

;---------- Allocator for data block with variable base address and size ------; 

struct ALLOCATOR
objectStart       dd ?
objectStop        dd ?
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
_SetThreadGroupAffinity            dd ?
_VirtualAllocExNuma                dd ?
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
lockedIcons                dd ICON_COUNT dup ?    ; Pointers to icons resources
createdIcons               dd ICON_COUNT dup ?    ; Pointers to icons
hTabDlg                    dd ITEM_COUNT dup ?    ; Sheets handles
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

;---------- Memory Performance Engine (MPE32) definitions ---------------------;
; Entry for compact encoding performance method 
; Use 16-bit base-relative offsets instead full 64-bit offsets
; 6 or 2 bytes per entry, compact encoding

MACRO BANDWIDTH_TEMPORAL x1, x2, x3, x4
{
dw  x1 - RoutinesBandwidthTemporal  ; Target routine entry point
dw  x2 - RoutinesBandwidthTemporal  ; Dump fragment start, dump show at drawings Y=F(X)
db  x3 - x2                         ; Dump fragment size
db  x4                              ; Read or Write data bytes per instruction,
}                                   ; example YMM register op. = 32 bytes (256 bit) 

MACRO BANDWIDTH_NON_TEMPORAL x1, x2, x3, x4
{
dw  x1 - RoutinesBandwidthNonTemporal  ; Target routine entry point
dw  x2 - RoutinesBandwidthNonTemporal  ; Dump fragment start, dump show at drawings Y=F(X)
db  x3 - x2                            ; Dump fragment size
db  x4                                 ; Read or Write data bytes per instruction,
}                                      ; example YMM register op. = 32 bytes (256 bit) 

MACRO LATENCY_ENTRY  x1, x2, x3, x4   ; 6 bytes per entry, compact encoding
{
dw  x1 - RoutinesLatency   ; Target routine entry point
dw  x2 - RoutinesLatency   ; Dump fragment start, dump show at drawings Y=F(X)
db  x3 - x2                ; Dump fragment size
db  x4                     ; Data Bytes per Instruction,
}                          ; example YMM register op. = 32 bytes (256 bit) 

MACRO MATH_ENTRY x1
{
dw  x1 - RoutinesMath 
}

;---------- Threads and memory management definitions -------------------------; 

; Thread control entry, or entire benchmark control if single thread
; Note keep 64 bytes per entry, see ThreadEntry, can be non parametrized coding
; Note keep all pairs: Affinity Mask + Affinity Group as 2 sequental qwords,
; this rule required because WinAPI use with direct output store to this qwords 

struct THCTRL
eventStart      dd  ?   ; Event Handle for operation START signal
eventDone       dd  ?   ; Event Handle for operation COMPLETE signal 
threadHandle    dd  ?   ; Thread Handle for execution thread
threadAffinity  dd  ?   ; Affinity Mask = F (True Affinity Mask, Options) 
threadGroup     dd  ?   ; Processor group, associated with affinity mask
entryPoint      dd  ?   ; Entry point to operation subroutine
base1           dd  ?   ; Source for read, destination for write and modify
base2           dd  ?   ; Destination for copy
sizeBytes       dd  ?   ; Block size, units = bytes, for benchmarking, not for memory allocation (REDUNDANT FIELD RESERVED) 
sizeInst        dd  ?   ; Block size, units = instructions, for benchmarking (MUST BE UPDATED PER DRAW ITERATION)
repeats         dq  ?   ; Number of measurement repeats for precision requirements
largePages      dw  ?   ; 0=not supported, 1=supported but not used, 2=used (REDUNDANT FIELD RESERVED)
affinityMode    dw  ?   ; Affinity mode: 0=None, 1=Without groups, 2=With groups 
origBase        dd  ?   ; True (before alignment) memory block base for release (REDUNDANT FIELD RESERVED)
origAffinity    dd  ?   ; Store original affinity mask, also storage for return (STORED BY THREAD, NOT BY INITIALIZER) 
origGroup       dd  ?   ; Store processor group for original offinity mask (STORED BY THREAD, NOT BY INITIALIZER) 
ends

; NUMA node description entry, not a same as thread description enrty
; Note keep 32 bytes per entry, see ThreadEntry, can be non parametrized coding
; Note keep all pairs: Affinity Mask + Affinity Group as 2 sequental qwords,
; this rule required because WinAPI use with direct output store to this qwords 

struct NUMACTRL
nodeID          dd  ?   ; NUMA Node number, if no NUMA, this field = 0 for all entries
nodeAffinity    dd  ?   ; NUMA Node affinity mask
nodeGroup       dd  ?   ; Processors Group number, 0 if single group platform
alignedBase     dd  ?   ; Block size after alignment, for read/write operation
alignedSize     dd  ?   ; Block base address after alignment, for r/w operation 
trueBase        dd  ?   ; Base address, returned when allocated, for release, 0=not used 
reserved        dd  2 dup ?    ; Reserved for alignment
ends

;---------- Cache parameters blocks -------------------------------------------;

; Cache info format
; Qword=Size, bytes,
; Word=Maximum threads at this level
; Word=Maximum APIC IDs per package
; For Trace Cache size in micro operations, not bytes
struct CPUCACHE
CpuidTraceCache   dq  ?         ; Instruction trace cache
CpuidTraceSmp     dw  ? , ?
CpuidL1Code       dq  ?	        ; L1 instruction cache
CpuidL1Csmp       dw  ? , ?
CpuidL1Data       dq  ?	        ; L1 data cache
CpuidL1Dsmp       dw  ? , ?
CpuidL2Unified    dq  ?	        ; L2 unified cache
CpuidL2Usmp       dw  ? , ?
CpuidL3Unified    dq  ?	        ; L3 unified cache
CpuidL3Usmp       dw  ? , ?
ends

;--- 32 bytes CACHE SIZE INFO ---
; Qword [000-007] = L1 instruction cache size (bytes)
; Qword [008-015] = L1 data cache size (bytes)
; Qword [016-023] = L2 unified cache size (bytes)
; Qword [024-031] = L3 unified cache size (bytes)
;--- 16 bytes CACHE TOPOLOGY INFO ---
; Dword [032-035] = Threads per L1 instruction cache
; Dword [036-039] = Threads per L1 data cache
; Dword [040-043] = Threads per L2 unified cache 
; Dword [044-047] = Threads per L3 unified cache 
;--- 16 bytes MULTIPROCESSING TOPOLOGY INFO ---
; Dword [048-051] = Threads per one core (HT/SMT)
; Dword [052-055] = Cores per one package (or per one NUMA node)
; Dword [056-059] = Total NUMA nodes per platform
; Dword [060-063] = Total CPU packages per platform 
struct OSCACHE
OsL1Code          dq  ?         ; Cache size information
OsL1Data          dq  ?
OsL2Unified       dq  ?
OsL3Unified       dq  ?
OsThreadsPerL1C   dd  ?         ; Cache threads information
OsThreadsPerL1D   dd  ?
OsThreadsPerL2U   dd  ?
OsThreadsPerL3U   dd  ?
OsThreadsPerCore  dd  ?         ; Topology information
OsThreadsPerNode  dd  ?
OsNodesTotal      dd  ?
OsPackagesTotal   dd  ?
ends

;---------- System parameters block -------------------------------------------;

struct SYSPARMS
applicationMode   dd  ?    ; 0 = ia32 under Win32, 1 = x64, 2 = ia32 under Win64
sseSupported      dd  ?    ; Separate flag for save/restore SSE registers at child thread, 0=No, 1=Yes
; Performance pattern internal parameters
; SSE4.1 validation required for 128-bit MOVNTDQA
; AVX2 validation required for 256-bit VMOVNTDQA
; Note non-temporal read recommended for AMD CPU only,
; Note non-temporal write recommended for both Intel and AMD
; IMPORTANT patch v0.98.0 = non-temporal read use BOTH FOR INTEL AND AMD
; IMPORTANT patch v1.01.02 = this byte locked to 0, REJECT (V)MOVNTDQA
; selection because optimal variant is PREFETCHNTA from SSE1 
NonTemporalRead   dd  ?    ; 0 = Default (V)MOVNTPD write and PREFETCHNTA read 
                           ; 1 = SSE4.1 use MOVNTDQA for read 128-bit 
                           ; 2 = AVX2 use VMOVNTDQA for read 256-bit 
; Large page size and availability
largePageSize     dq  ?              ; Used large page size, bytes
largePageFlag     dd  ?              ; 0 = mapping not available, 1 = available
; Processor groups count for support systems with >64 logical processors
; 0 means unknown, supported as single group system
; 1 means detected single group system
; >1 means complex system with >1 groups, required processor group management
processorGroups   dd  ?
; Total processors count at all groups
processorsTotal   dd  ?
; NUMA = Non Uniform Memory Access info, maximum for 64 NUMA Nodes
numaNodes         dd  ?              ; Number of NUMA Nodes
numaNodesMasks    dq  64 dup ?       ; Affinity masks for 64 nodes maximum
; Cache information
cpuCache          CPUCACHE
osCache           OSCACHE
ends

;--- Memory and Cache Benchmark User Parameters Block (UPB) definitions -------;
; This variables used for collect data from GUI objects when benchmark runs.
; Test USER parameters, loaded from GUI widgets settings.

struct MEMUPB
optionAsm         dd  ?    ; Select test ASM method, one of procedures
optionDistance    dd  ?    ; Prefetch distance for non-temporal read: 0=default, 1=medium, 2=long
optionTarget      dd  ?    ; Objects { L1, L2, L3, L4, DRAM, CUSTOM }
optionParallel    dd  ?    ; Parallel { GRAY_NOT_SUP, DISABLED, ENABLED }
optionHT          dd  ?    ; HT { GRAY_NOT_SUP, DISABLED, ENABLED }
optionPG          dd  ?    ; PG { GRAY_NOT_SUP, DISABLED, ENABLED } 
optionNUMA        dd  ?    ; NUMA { GRAY_NOT_SUP, UNAWARE, SINGLE_DOMAIN, FORCE LOCAL, FORCE REMOTE }
optionLP          dd  ?    ; LP { GRAY_NOT_SUP, DISABLED, ENABLED }  
optionMeasure     dd  ?    ; Measurement repeats { 0=fast, 1=slow, 2=fast adaptive, 3=slow adaptive }
optionApprox      dd  ?    ; Approximation for X { 0=none, 1=X16, 2=X32 }
runContext        dd  ?    ; Run context: 0 = "Run simple" , 1 = "Run drawings" 
customBlockStart  dd  ?    ; Override start block size, or 0=default
; YET NOT USED FIELDS:
; customBlockEnd    dd  ?    ; Override end block size, or 0=default
; customBlockDelta  dd  ?    ; Override delta block size, or 0=default
ends

;--- Memory and Cache Benchmark Input Parameters Block (IPB) definitions ------;
; This variables used for build input parameters for Memory/Math performance
; engines procedures. IPB = f(UPB).
; Test INPUT parameters, build IPB = F ( UPB, SYSPARMS ).

struct MEMIPB
; First 8 items of IPB associated with first 8 items of UPB
updatedAsm        dd  ?    ; Routine selector, set after detect features
updatedDistance   dd  ?    ; Prefetch distance for non-temporal read: 0=default, 1=medium, 2=long
updatedTarget     dd  ?    ; Objects { L1, L2, L3, L4, DRAM, CUSTOM }
updatedThreads    dd  ?    ; Number of threads, set after detect features
updatedHT         dd  ?    ; 0=Not sup. by platform, 1=sup. but not used, 2=used
updatedPG         dd  ?    ; 0=Not sup. by platform, 1=sup. but not used, 2=used 
updatedNUMA       dd  ?    ; 0=None, 1=No control, 2-Single domain, 3=Optimal, 4=Non optim. 
updatedLP         dd  ?    ; 0=Not sup. by platform, 1=sup. but not used, 2=used
updatedMeasure    dd  ?    ; 0=fast, 1=slow, 2=fast adaptive, 3=slow adaptive
updatedApprox     dd  ?    ; Approximation for X { 0=none, 1=X16, 2=X32 }  
; Two work blocks used by benchmark scenario
allocatedBlock1   dd  ?
allocatedBlock2   dd  ?
; This items generated as f( first 8 items, platform configuration )
startBlockSize    dd  ?    ; Start block size, or 0=default, unite = bytes
endBlockSize      dd  ?    ; End block size, or 0=default
deltaBlockSize    dd  ?    ; Delta block size, or 0=default
measureRepeats    dq  ?    ; Number of measurement repeats, note 64-bit value
operandWidth      dd  ?    ; Used instructions operand width, bits 
groupsCount       dd  ?    ; Number of Processor Groups
domainsCount      dd  ?    ; Number of NUMA domains
pageSize          dd  ?    ; Memory page size, bytes
; Memory allocation results
memoryTotal       dd  ?    ; Memory allocated, per all threads
memoryPerThread   dd  ?    ; Memory allocated, per each thread 
; Benchmarks select and routine dump support
patternRoutine    dd  ?    ; Pointer to target benchmark routine, this is fill routine for latency mode
walkRoutine       dd  ?    ; Second routine, required for latency measurement
dumpMethodStart   dd  ?    ; Start of fragment, visualized as dump during benchmarks
dumpMethodLength  dd  ?    ; Length of fragment, visualized as dump during benchmarks, bytes
; Adaptive measurement timings support
adaptiveSeconds   dq  ?    ; Target measurement time for stabilization, seconds, floation point
adaptiveProduct   dq  ?    ; Size * Repeats = Product , practic calc: Repeats = Product / Size 
; Additionsl system information
applicationMode   dd  ?    ; 0 = ia32 under Win32, 1 = x64, 2 = ia32 under Win64
sseSupported      dd  ?    ; Separate flag for save/restore SSE registers at child thread, 0=No, 1=Yes
; Flags
threadStop        dd  ?    ; Flag for stop child threads
ends

;--- Memory and Cache Benchmark Output Parameters Block (OPB) definitions -----;
; This variables used for collect output results of Memory/Math 
; Performance Engines procedures.
; Test OUTPUT parameters, store OPB = F ( Benchmarks and TSC meas. results ).

struct MEMOPB
deltaTSC          dq  ?    ; TSC measured clock, Hz, 64-bit long integer
tscFrequencyHz    dq  ?    ; TSC frequency, Hz, as double precision 
tscFrequencyMHz   dq  ?    ; TSC frequency, MHz, as double precision
tscPeriodS        dq  ?    ; TSC period, seconds, as double precision
tscPeriodNs       dq  ?    ; TSC period, nanoseconds, as double precision 
osTimerDelta      dq  ?    ; delta time at units = 100 ns, 64-bit long integer 
tscTimerDelta     dq  ?    ; delta time at units = 1 TSC clk., 64-bit long int. 
ends

;--- Math Benchmark User Parameters Block (UPB) definitions -------------------;
; This variables used for collect data from GUI objects when benchmark runs.
; Test USER parameters, loaded from GUI widgets settings.

struct MATHUPB
reserved  db  ?
ends

;--- Math Benchmark Input Parameters Block (IPB) definitions ------------------;
; This variables used for build input parameters for Memory/Math performance
; engines procedures. IPB = f(UPB).
; Test INPUT parameters, build IPB = F ( UPB, SYSPARMS ).

struct MATHIPB
reserved  db  ?
ends

;--- Math Benchmark Output Parameters Block (OPB) definitions -----------------;
; This variables used for collect output results of Memory/Math 
; Performance Engines procedures.
; Test OUTPUT parameters, store OPB = F ( Benchmarks and TSC meas. results ).

struct MATHOPB
reserved  db  ?
ends

;---------- Memory Performance Engine (MPE32) data structure ------------------;

; Maximum number of supported threads: total and per processor group

MAX_THREADS            =  256
MAX_THREADS_PER_GROUP  =  64

; Maximum number of supported NUMA nodes

MAX_NODES              =  64

; Thread entry size = 64 bytes, const because optimal shift by 6 can be used
; 16384 bytes, 256 entries * 64 bytes, used for (multi) thread management

THCTRL_SIZE            =  sizeof.THCTRL
THREAD_LIST_SIZE       =  MAX_THREADS * THCTRL_SIZE

; Event handle size = 4 bytes (32-bit)
; 1024 bytes, 256 handles * 4 bytes, 
; separate sequental list req. for WaitForMultipleObjects function  

EVENT_SIZE             =  4
EVENT_LIST_SIZE        =  MAX_THREADS * EVENT_SIZE

; NUMA node entry size = 32 bytes, const because optimal shift by 5 can be used
; 8192 bytes, 256 entries * 32 bytes,
; for NUMA nodes description, not a same as THREAD

NUMACTRL_SIZE          =  sizeof.NUMACTRL
NUMA_LIST_SIZE         =  MAX_NODES * NUMACTRL_SIZE  

;------------------------------------------------------------------------------;
;     MPECSB = Memory Performance Engine control and status block (CSB).       ;
;------------------------------------------------------------------------------;
; Benchmarks input and output operands located by alignment-optimal ordering
; and 64-byte cache line allocation
; Threads management lists, must be sequentally located, this fact used in code,
; events list must be sequental, this fact used by OS events API. 

struct MPE32CSB
threadList      db  THREAD_LIST_SIZE  dup ? 
startEventList  db  EVENT_LIST_SIZE   dup ?  
doneEventList   db  EVENT_LIST_SIZE   dup ?  
numaNodeList    db  NUMA_LIST_SIZE    dup ?    
ends

;---------- Equations for UPB, IPB, OPB data build and interpreting -----------;

; Assembler Methods (AM) option values count and Bandwidth/Latency criteria

AM_BYTE_COUNT       =  24     ; 24 bytes per methods primary list
LATENCY_MODE        =  44     ; modes ID = 44-47 for latency measurement
READ_SSE128_MODE    =  9      ; for add information about prefetch distance
READ_AVX256_MODE    =  12     ; for add information about prefetch distance
READ_AVX512_MODE    =  15     ; for add information about prefetch distance

; Options limits and values
; 24 codes of assembler option, argument for AM_Selector translation,
; because 24 checkboxes for assembler method select in GUI

ASM_ARGUMENT_LIMIT  =  AM_BYTE_COUNT - 1

; 42 names for assembler methods, result of AM_Selector translation

ASM_RESULT_LIMIT    =  47 + 1

; Parallel threads option

PARALLEL_LIMIT      =  2
PARALLEL_NOT_SUP    =  0
PARALLEL_NOT_USED   =  1
PARALLEL_USED       =  2

; Target Object: Cache, DRAM, Custom

TARGET_LIMIT        =  5 
TARGET_L1           =  0
TARGET_L2           =  1
TARGET_L3           =  2
TARGET_L4           =  3
TARGET_DRAM         =  4
TARGET_CUSTOM       =  5

; PD = Prefetch distance

PD_LIMIT            =  3
PD_DEFAULT          =  0
PD_MEDIUM           =  1
PD_LONG             =  2
PD_NOT_USED         =  3

; HT = Hyper Threading

HT_LIMIT            =  2
HT_NOT_SUPPORTED    =  0
HT_NOT_USED         =  1
HT_USED             =  2

; PG = Processor Group

PG_LIMIT            =  2
PG_NOT_SUPPORTED    =  0
PG_NO_CONTROL       =  1
PG_OPTIMAL          =  2

; NUMA topology

NUMA_LIMIT          =  4
NUMA_NOT_SUPPORTED  =  0
NUMA_NO_CONTROL     =  1
NUMA_CURRENT_ONLY   =  2
NUMA_OPTIMAL        =  3
NUMA_NON_OPTIMAL    =  4    

; LP = Large Pages

LP_LIMIT            =  2
LP_NOT_SUPPORTED    =  0
LP_NOT_USED         =  1
LP_USED             =  2

; Measure repeats selection modes

MEASURE_LIMIT       =  3
MEASURE_BRIEF       =  0
MEASURE_CAREFUL     =  1    
MEASURE_B_ADAPTIVE  =  2
MEASURE_C_ADAPTIVE  =  3

; Approximation by X modes

APPROX_LIMIT        =  2
APPROX_NONE         =  0
APPROX_X16          =  1
APPROX_X32          =  2


;---------- Root structure: NCRB32 application registry -----------------------;  

struct REGISTRY32
; common system and application information
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
align 8
sysParms                   SYSPARMS   ?
; memory and cache benchmarks data
align 8
memUpb                     MEMUPB     ?
align 8
memIpb                     MEMIPB     ?
align 8                             
memOpb                     MEMOPB     ?
align 64
; math benchmarks data
align 8
mathUpb                    MATHUPB     ?
align 8
mathIpb                    MATHIPB     ?
align 8
mathOpb                    MATHOPB     ?
; memory allocation and thread management data for benchmarks
align 64
mpe32csb                   MPE32CSB   ?
; dynamically allocated unformatted part
align 64
unformatted                db ?
ends

