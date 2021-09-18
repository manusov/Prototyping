;==============================================================================;
;                                                                              ;
;               Global definitions for all components of project.              ;
;                         Both for ia32 and x64 code.                          ;
;                                                                              ;
;==============================================================================;

;---------- Identifiers for application main window and tabs ------------------; 
IDD_MAIN                = 100
IDD_SYSINFO             = 101
IDD_MEMORY              = 102
IDD_MATH                = 103
IDD_OS                  = 104
IDD_NATIVE_OS           = 105
IDD_PROCESSOR           = 106
IDD_TOPOLOGY            = 107
IDD_TOPOLOGY_EX         = 108
IDD_NUMA                = 109
IDD_PGROUPS             = 110
IDD_ACPI                = 111
IDD_AFF_CPUID           = 112
IDD_KMD                 = 113
;---------- Identifiers for application main menu and menu items --------------; 
IDR_MENU                = 114
IDM_SAVE_REPORT         = 115 
IDM_LOAD_REPORT         = 116
IDM_SAVE_IMAGE          = 117
IDM_ABOUT               = 118
IDM_EXIT                = 119
;---------- Identifiers for raw resources: strings and binder scripts ---------;
; Note. Strings represented as raw resources (not as string resources) for
; compact encoding: 1 byte per char.  
IDS_STRINGS_POOL        = 120
IDS_BINDERS_POOL        = 121
;---------- Identifiers for icons ---------------------------------------------;
IDI_SYSINFO             = 130
IDI_MEMORY              = 131
IDI_MATH                = 132
IDI_OS                  = 133
IDI_NATIVE_OS           = 134
IDI_PROCESSOR           = 135
IDI_TOPOLOGY            = 136
IDI_TOPOLOGY_EX         = 137
IDI_NUMA                = 138
IDI_P_GROUPS            = 139
IDI_ACPI                = 140
IDI_AFF_CPUID           = 141
IDI_KMD                 = 142
;---------- Identifiers for group icons ---------------------------------------;
IDG_SYSINFO             = 150
IDG_MEMORY              = 151
IDG_MATH                = 152
IDG_OS                  = 153
IDG_NATIVE_OS           = 154
IDG_PROCESSOR           = 155
IDG_TOPOLOGY            = 156
IDG_TOPOLOGY_EX         = 157
IDG_NUMA                = 158
IDG_P_GROUPS            = 159
IDG_ACPI                = 160
IDG_AFF_CPUID           = 161
IDG_KMD                 = 162
;---------- Initial values and counters for build GUI objects -----------------; 
ICON_FIRST              = IDG_SYSINFO
IDD_FIRST               = IDD_SYSINFO
ITEM_COUNT              = 13
ICON_COUNT              = ITEM_COUNT

;---------- Identifier for tabbed sheet component -----------------------------;
IDC_TAB                 = 200
;---------- Identifiers for tab #1 = System information sheet -----------------;
IDC_SYSINFO             = 300 
IDB_SYSINFO_CANCEL      = 301

;---------- Identifiers for tab #2 = Memory and cache benchmarks sheet --------;
IDC_MEMORY              = 500
IDC_MEMORY_FRAME_1      = 501
IDC_MEMORY_FRAME_2      = 502
IDC_MEMORY_FRAME_3      = 503
IDC_MEMORY_FRAME_4      = 504
IDC_MEMORY_FRAME_5      = 505
IDC_MEMORY_FRAME_6      = 506
IDB_MEMORY_ASM_A0       = 507
IDB_MEMORY_ASM_A1       = 508
IDB_MEMORY_ASM_A2       = 509
IDB_MEMORY_ASM_A3       = 510
IDB_MEMORY_ASM_A4       = 511
IDB_MEMORY_ASM_A5       = 512
IDB_MEMORY_ASM_A6       = 513
IDB_MEMORY_ASM_A7       = 514
IDB_MEMORY_ASM_A8       = 515
IDB_MEMORY_ASM_B0       = 516 
IDB_MEMORY_ASM_B1       = 517
IDB_MEMORY_ASM_B2       = 518
IDB_MEMORY_ASM_B3       = 519
IDB_MEMORY_ASM_B4       = 520
IDB_MEMORY_ASM_B5       = 521
IDB_MEMORY_ASM_B6       = 522
IDB_MEMORY_ASM_B7       = 523
IDB_MEMORY_ASM_B8       = 524
IDB_MEMORY_ASM_C0       = 525 
IDB_MEMORY_ASM_C1       = 526
IDB_MEMORY_ASM_C2       = 527
IDB_MEMORY_ASM_C3       = 528
IDB_MEMORY_ASM_C4       = 529
IDB_MEMORY_ASM_C5       = 530
IDB_MEMORY_ASM_C6       = 531
IDB_MEMORY_ASM_C7       = 532
IDB_MEMORY_NONTEMP      = 533
IDB_MEMORY_FORCE32      = 534
IDB_MEMORY_L1           = 535
IDB_MEMORY_L2           = 536
IDB_MEMORY_L3           = 537
IDB_MEMORY_L4           = 538
IDB_MEMORY_DRAM         = 539
IDB_MEMORY_CUSTOM       = 540
IDB_MEMORY_FILE         = 541
IDB_MEMORY_GPU          = 542
IDB_MEMORY_PHYSICAL     = 543
IDE_MEMORY_B_SIZE       = 544
IDE_MEMORY_F_SIZE       = 545
IDE_MEMORY_G_SIZE       = 546
IDE_MEMORY_M_START      = 547
IDE_MEMORY_M_STOP       = 548
IDC_MEMORY_M_HYPHEN     = 549
IDC_MEMORY_COMBO_F      = 550
IDC_MEMORY_COMBO_F      = 551
IDB_MEMORY_MTRR_WB      = 552 
IDB_MEMORY_MTRR_WT      = 553
IDB_MEMORY_MTRR_WC      = 554
IDB_MEMORY_MTRR_WP      = 555
IDB_MEMORY_MTRR_UC      = 556
IDB_MEMORY_PARALLEL     = 557
IDB_MEMORY_HT           = 558
IDB_MEMORY_PG           = 559
IDB_MEMORY_NUMA_U       = 560
IDB_MEMORY_NUMA_S       = 561
IDB_MEMORY_NUMA_O       = 562
IDB_MEMORY_NUMA_N       = 563
IDB_MEMORY_NORMAL       = 564
IDB_MEMORY_SK_63        = 565
IDB_MEMORY_SK_4095      = 566
IDB_MEMORY_SK_CSTM      = 567
IDE_MEMORY_SK_SIZE      = 568
IDB_MEMORY_LP           = 569
IDB_MEMORY_NO_PF        = 570
IDB_MEMORY_DEF_PF       = 571
IDB_MEMORY_MED_PF       = 572
IDB_MEMORY_LNG_PF       = 573
IDB_MEMORY_BLK_PF       = 574
IDB_MEMORY_CST_PF       = 575
IDE_MEMORY_PF_SIZE      = 576
IDB_MEMORY_BRF          = 577  
IDB_MEMORY_CRF          = 578 
IDB_MEMORY_BRF_A        = 579 
IDB_MEMORY_CRF_A        = 580 
IDB_MEMORY_ALL_P        = 581
IDB_MEMORY_X_16         = 582 
IDB_MEMORY_X_32         = 583 
IDB_MEMORY_3D_DRAW      = 584
IDB_MEMORY_DRAW         = 585
IDB_MEMORY_RUN          = 586
IDB_MEMORY_DEFAULTS     = 587
IDB_MEMORY_CANCEL       = 588

;---------- Identifiers for tab #3 = Processor mathematics benchmarks sheet ---;
IDC_MATH                = 700
IDB_MATH_CANCEL         = 701

;---------- Identifiers for tab #4 = Operating system sheet -------------------; 
IDC_OS                  = 900
IDB_OS_CANCEL           = 901

;---------- Identifiers for tab #5 = Native os ( for WoW64 ) sheet ------------;
IDC_NATIVE_OS           = 1100
IDB_NATIVE_OS_CANCEL    = 1101

;---------- Identifiers for tab #6 = Processor information sheet --------------; 
IDC_PROCESSOR           = 1300
IDB_PROCESSOR_CANCEL    = 1301

;---------- Identifiers for tab #7 = Topology information sheet ---------------; 
IDC_TOPOLOGY            = 1500
IDB_TOPOLOGY_CANCEL     = 1501

;---------- Identifiers for tab #8 = Extended topology information sheet ------;
IDC_TOPOLOGY_EX         = 1700
IDB_TOPOLOGY_EX_CANCEL  = 1701

;---------- Identifiers for tab #9 = NUMA domains sheet -----------------------; 
IDC_NUMA                = 1900
IDC_NUMA_CANCEL         = 1901

;---------- Identifiers for tab #10 = Processor groups sheet ------------------;
IDC_P_GROUPS            = 2100
IDC_P_GROUPS_CANCEL     = 2101

;---------- Identifiers for tab #11 = ACPI information sheet ------------------;  
IDC_ACPI                = 2300
IDC_ACPI_CANCEL         = 2301

;---------- Identifiers for tab #12 = Affinized CPUID dump sheet --------------; 
IDC_AFF_CPUID           = 2500
IDC_AFF_CPUID_CANCEL    = 2501

;---------- Identifiers for tab #13 = Kernel Mode Driver information sheet ----;
IDC_KMD                 = 2700
IDC_KMD_CANCEL          = 2701

;---------- Strings numbers in the raw strings pool ---------------------------;
; Note. Strings enumerated by zero-terminations.
;---------- Strings numbers for application sheets brief names ----------------;
STR_BRIEF_SYSINFO      = 0   
STR_BRIEF_MEMORY       = 1
STR_BRIEF_MATH         = 2
STR_BRIEF_OS           = 3
STR_BRIEF_NATIVE_OS    = 4
STR_BRIEF_PROCESSOR    = 5
STR_BRIEF_TOPOLOGY     = 6
STR_BRIEF_TOPOLOGY_EX  = 7
STR_BRIEF_NUMA         = 8
STR_BRIEF_P_GROUPS     = 9
STR_BRIEF_ACPI         = 10
STR_BRIEF_AFF_CPUID    = 11
STR_BRIEF_KMD          = 12
;---------- Strings numbers for application sheets full names -----------------;
STR_FULL_SYSINFO       = 13   
STR_FULL_MEMORY        = 14
STR_FULL_MATH          = 15
STR_FULL_OS            = 16
STR_FULL_NATIVE_OS     = 17
STR_FULL_PROCESSOR     = 18
STR_FULL_TOPOLOGY      = 19
STR_FULL_TOPOLOGY_EX   = 20
STR_FULL_NUMA          = 21
STR_FULL_P_GROUPS      = 22
STR_FULL_ACPI          = 23
STR_FULL_AFF_CPUID     = 24
STR_FULL_KMD           = 25
;--- Assembler instructions and modes names for memory and cache benchmarks ---;
STR_AM_IA32_00         = 26
STR_AM_IA32_01         = STR_AM_IA32_00 + 1   
STR_AM_IA32_02         = STR_AM_IA32_00 + 2 
STR_AM_IA32_03         = STR_AM_IA32_00 + 3 
STR_AM_IA32_04         = STR_AM_IA32_00 + 4 
STR_AM_IA32_05         = STR_AM_IA32_00 + 5 
STR_AM_X64_00          = STR_AM_IA32_00 + 6 
STR_AM_X64_01          = STR_AM_IA32_00 + 7  
STR_AM_X64_02          = STR_AM_IA32_00 + 8
STR_AM_X64_03          = STR_AM_IA32_00 + 9
STR_AM_X64_04          = STR_AM_IA32_00 + 10
STR_AM_X64_05          = STR_AM_IA32_00 + 11
STR_AM_06              = STR_AM_IA32_00 + 12
STR_AM_07              = STR_AM_IA32_00 + 13
STR_AM_08              = STR_AM_IA32_00 + 14
STR_AM_09              = STR_AM_IA32_00 + 15
STR_AM_10              = STR_AM_IA32_00 + 16
STR_AM_11              = STR_AM_IA32_00 + 17
STR_AM_12              = STR_AM_IA32_00 + 18
STR_AM_13              = STR_AM_IA32_00 + 19
STR_AM_14              = STR_AM_IA32_00 + 20
STR_AM_15              = STR_AM_IA32_00 + 21
STR_AM_16              = STR_AM_IA32_00 + 22
STR_AM_17              = STR_AM_IA32_00 + 23
STR_AM_18              = STR_AM_IA32_00 + 24
STR_AM_19              = STR_AM_IA32_00 + 25
STR_AM_20              = STR_AM_IA32_00 + 26
STR_AM_21              = STR_AM_IA32_00 + 27
STR_AM_22              = STR_AM_IA32_00 + 28
STR_AM_23              = STR_AM_IA32_00 + 29
STR_LCM                = STR_AM_IA32_00 + 30
STR_RDRAND             = STR_AM_IA32_00 + 31
STR_NON_TEMPORAL       = STR_AM_IA32_00 + 32
STR_32X2               = STR_AM_IA32_00 + 33
;---------- Target objects names for memory and cache benchmark ---------------;
STR_MEMORY_L1          = STR_32X2 + 1
STR_MEMORY_L2          = STR_MEMORY_L1 + 1 
STR_MEMORY_L3          = STR_MEMORY_L1 + 2
STR_MEMORY_L4          = STR_MEMORY_L1 + 3
STR_MEMORY_DRAM        = STR_MEMORY_L1 + 4
STR_MEMORY_CUSTOM      = STR_MEMORY_L1 + 5
STR_MEMORY_FILE        = STR_MEMORY_L1 + 6
STR_MEMORY_GPU         = STR_MEMORY_L1 + 7
STR_MEMORY_PHYSICAL    = STR_MEMORY_L1 + 8
;---------- Memory status names -----------------------------------------------;
STR_MEMORY_MTRR_WB     = STR_MEMORY_PHYSICAL + 1
STR_MEMORY_MTRR_WT     = STR_MEMORY_MTRR_WB + 1 
STR_MEMORY_MTRR_WC     = STR_MEMORY_MTRR_WB + 2 
STR_MEMORY_MTRR_WP     = STR_MEMORY_MTRR_WB + 3 
STR_MEMORY_MTRR_UC     = STR_MEMORY_MTRR_WB + 4 
;---------- Memory access and platform topology options names -----------------;
STR_PARALLEL_THREADS   = STR_MEMORY_MTRR_UC + 1 
STR_HYPER_THREADING    = STR_PARALLEL_THREADS + 1 
STR_PROCESSOR_GROUPS   = STR_PARALLEL_THREADS + 2
STR_NUMA_UNAWARE       = STR_PARALLEL_THREADS + 3
STR_NUMA_SINGLE        = STR_PARALLEL_THREADS + 4
STR_NUMA_OPTIMAL       = STR_PARALLEL_THREADS + 5
STR_NUMA_NO_OPTIMAL    = STR_PARALLEL_THREADS + 6
STR_NORMAL_ACCESS      = STR_PARALLEL_THREADS + 7 
STR_ACCESS_64B         = STR_PARALLEL_THREADS + 8 
STR_ACCESS_4096B       = STR_PARALLEL_THREADS + 9 
STR_ACCES_CUSTOM       = STR_PARALLEL_THREADS + 10 
STR_LARGE_PAGES        = STR_PARALLEL_THREADS + 11
;---------- Memory prefetch options names -------------------------------------;
STR_NO_PREFETCH        = STR_LARGE_PAGES + 1
STR_DEFAULT_DISTANCE   = STR_NO_PREFETCH + 1
STR_MEDIUM_DISTANCE    = STR_NO_PREFETCH + 2
STR_LONG_DISTANCE      = STR_NO_PREFETCH + 3
STR_BLOCK_PREFETCH     = STR_NO_PREFETCH + 4
STR_CUSTOM_DISTANCE    = STR_NO_PREFETCH + 5
;---------- Measurement options names -----------------------------------------;
STR_MEASURE_BRIEF      = STR_CUSTOM_DISTANCE + 1  
STR_MEASURE_CAREF      = STR_MEASURE_BRIEF + 1 
STR_BRIEFF_ADAPTIVE    = STR_MEASURE_BRIEF + 2
STR_CAREF_ADAPTIVE     = STR_MEASURE_BRIEF + 3
STR_ALL_POINTS         = STR_MEASURE_BRIEF + 4
STR_X_16_POINTS        = STR_MEASURE_BRIEF + 5
STR_X_32_POINTS        = STR_MEASURE_BRIEF + 6
STR_3D_DRAW            = STR_MEASURE_BRIEF + 7 
;---------- Actions buttons names ---------------------------------------------;
STR_DRAW               = STR_3D_DRAW + 1 
STR_RUN                = STR_DRAW + 1
STR_DEFAULTS           = STR_DRAW + 2
STR_EXIT               = STR_DRAW + 3
;---------- Memory size and speed units, additional information ---------------;
STR_UNITS_BYTES        = STR_EXIT + 1   
STR_UNITS_KB           = STR_UNITS_BYTES + 1 
STR_UNITS_MB           = STR_UNITS_BYTES + 2
STR_UNITS_GB           = STR_UNITS_BYTES + 3
STR_UNITS_TB           = STR_UNITS_BYTES + 4
STR_UNITS_MBPS         = STR_UNITS_BYTES + 5
STR_UNITS_NS           = STR_UNITS_BYTES + 6
STR_NONE               = STR_UNITS_BYTES + 7
STR_MASK               = STR_UNITS_BYTES + 8
STR_ENABLED            = STR_UNITS_BYTES + 9
STR_DISABLED           = STR_UNITS_BYTES + 10 
STR_HYPHEN             = STR_UNITS_BYTES + 11 
STR_TSC_CLKS           = STR_UNITS_BYTES + 12
STR_CPU_CLKS           = STR_UNITS_BYTES + 13
STR_DATA_MOVE          = STR_UNITS_BYTES + 14
STR_CALC               = STR_UNITS_BYTES + 15
STR_NS                 = STR_UNITS_BYTES + 16
STR_KERNEL_MODE        = STR_UNITS_BYTES + 17
STR_TRUE_CLOCK         = STR_UNITS_BYTES + 18




;---------- Binders numbers for GUI scripts -----------------------------------;
BINDER_SYSINFO         = 0   
BINDER_MEMORY          = 1
BINDER_MEMORY_IA32     = 2 
BINDER_MEMORY_X64      = 3
BINDER_MATH            = 4
BINDER_OS              = 5
BINDER_NATIVE_OS       = 6
BINDER_PROCESSOR       = 7
BINDER_TOPOLOGY        = 8
BINDER_TOPOLOGY_EX     = 9
BINDER_NUMA            = 10
BINDER_P_GROUPS        = 11
BINDER_ACPI            = 12
BINDER_AFF_CPUID       = 13
BINDER_KMD             = 14

;---------- Macro definitions for binders scripts instructions ----------------;
macro BIND_STOP
{ DB  0 }
MACRO BIND_STRING srcid, dstid
{ DD 01h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_INFO srcid, dstid
{ DD 02h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_BOOL srcid, srcbit, dstid
{ DD 03h + (srcid) SHL 9 + (srcbit) SHL 6 + (dstid) SHL 19 }

