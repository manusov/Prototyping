;------------------------------------------------------------------------------;
;              Set of target fragments for debug, this part is:                ;
;     globally used data constants and variables with pre-defined state.       ; 
;------------------------------------------------------------------------------;

;--- Constants, part of executable file ---
;--- Application attributes ---
ProductID                  DB  PRODUCT_ID

;--- Names for parameters show after test ---
Parm_ReadSpeed             DB  'Read speed',0
Parm_WriteSpeed            DB  'Write speed',0
Parm_CopySpeed             DB  'Copy speed',0

;--- Names for operations steps (show as error phase) ---
Step01_MeasureClock        DB  'Measure CPU clock frequency',0
Step02_OsApiDetection      DB  'OS API functions detection',0
Step03_MemoryAllocation    DB  'Memory allocation',0
Step04_MemoryBlank         DB  'Memory blank',0
Step05_SetAffinity         DB  'Set CPU affinity mask',0
Step06_InitializeFileIO    DB  'Initialize file IO',0
Step07_ExecuteFileIO       DB  'Execute file IO',0
Step08_CheckStatus         DB  'Check file IO status',0
Step09_CalculateResults    DB  'Calculate benchmarks MBPS results',0
Step10_RestoreAffinity     DB  'Restore CPU affinity mask',0
Step11_ReleaseMemory       DB  'Release memory',0
;--- Names for operations sub-steps (show as error cause) ---
Status00_Unknown           DB  'Unknown error.',0
Status00_WinAPI            DB  'WinAPI returns error.',0
;--- Errors at measure TSC clock ---
Status_CPUID               DB  'CPUID not supported or locked.',0
Status_CPUID_1             DB  'CPUID function 1 not supported or locked.',0
Status_RDTSC               DB  'RDTSC not supported or locked.',0
Status_CLK                 DB  'TSC clock measurement failed.',0
Status_Frequency           DB  'TSC zero frequency.',0
;--- Errors at WinAPI functions address load ---
Status_Handle_Module       DB  'Executable module handle invalid.',0
Status_NUMA_Allocation     DB  'NUMA memory allocation function not found.',0
Status_Affinity_Mask       DB  'Set affinity mask function not found.',0
;--- Errors at memory allocation ---
Status_Handle_Process      DB  'Process handle invalid.',0
Status_Memory_No_NUMA      DB  'NUMA-unaware branch failed.',0
Status_Memory_NUMA         DB  'NUMA-aware branch failed.',0
;--- Errors at set affinity mask ---
Status_Handle_Thread       DB  'Thread handle invalid.',0
Status_Set_Affinity        DB  'Original mask is wrong.',0
;--- Errors at file I/O operation parameters setup ---
Status_File_Size           DB  'Wrong file size.',0
Status_Block_Size          DB  'Wrong block size.',0
Status_Block_Ratio         DB  'Wrong block size ratio.',0
Status_File_Count          DB  'Wrong file count.',0
Status_Repeat_Count        DB  'Wrong repeat count.',0
Status_Cache_Mode          DB  'Wrong cache mode.',0
;--- Errors at analyzing file IO status ---
Status_Read_Mismatch       DB  'Total read size verification failed.',0
Status_Write_Mismatch      DB  'Total write size verification failed.',0
Status_Copy_Mismatch       DB  'Total copy size verification failed.',0
;--- Errors at calculate benchmarks results ---
Status_FPU                 DB  'FPU x87 not found.',0
;--- Errors at memory release ---
Status_Memory_Release      DB  'Release memory failed.',0

;--- Errors with engineering sample restrictions ---
Restriction_IOPS           DB  'IOPS measurement not supported yet.', 0
Restriction_Single         DB  'Single file mode not supported yet.', 0           
Restriction_Data           DB  'Data patterns not supported yet.', 0
Restriction_AsyncRead      DB  'Async read not supported yet.', 0
Restriction_AsyncWrite     DB  'Async write not supported yet.', 0
Restriction_AsyncCopy      DB  'Async copy not supported yet.', 0
Restriction_ReadDelay      DB  'Read delay not supported yet.', 0
Restriction_WriteDelay     DB  'Write delay not supported yet.', 0
Restriction_CopyDelay      DB  'Copy delay not supported yet.', 0
Restriction_FileHints      DB  'File hints not supported yet.', 0
Restriction_MixedRead      DB  'Mixed read not supported yet.', 0
Restriction_MixedWrite     DB  'Mixed write not supported yet.', 0

;--- Messages with additional details at some steps --- 
MsgApi1                    DB  'Pointer to function VirtualAllocExNuma = ', 0
MsgApi2                    DB  'Pointer to function SetThreadAffinityMask = ',0
MsgMemoryBase              DB  'Allocated memory base = ',0
MsgMemorySize              DB  'Allocated memory size = ',0
MsgOrigAffinity            DB  'Original affinity mask = ',0
MsgSetAffinity             DB  'Set affinity mask = ',0

;--- Sub-steps ---
Step07_01_Write            DB  'Write',0
Step07_02_Copy             DB  'Copy',0
Step07_03_Read             DB  'Read',0
Step07_04_Delete           DB  'Delete',0

;--- Error reporting support ---
ErrorPhase                 DB  'I/O benchmarks or initialization failed.'
                           DB  0Dh, 0Ah
                           DB  'Error phase  = ',0
ErrorStatus                DB  'Error status = ',0

;--- OS API support ---
NameKernel32               DB  'KERNEL32.DLL',0
NameVirtualAllocExNuma     DB  'VirtualAllocExNuma',0
NameSetThreadAffinityMask  DB  'SetThreadAffinityMask',0

;--- Mathematics constants ---
Const1E6                   DD  1000000        ; for Bytes -> MB, Hertz -> MHz
Const1E9                   DD  1000000000     ; for 100 ns units -> seconds

;--- Variables with pre-defined value, for conditional restore control ---
TrueMemoryBase             DQ  0       ; This required for memory block release
OrigAffinity               DQ  0       ; Original affinity mask




