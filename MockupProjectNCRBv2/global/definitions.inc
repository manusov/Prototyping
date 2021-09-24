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
IDS_CPU_COMMON_POOL     = 122
IDS_CPU_AVX512_POOL     = 123
IDS_OS_CONTEXT_POOL     = 124
IDS_CPU_METHOD_POOL     = 125
IDS_ACPI_DATA_POOL      = 126
IDS_IMPORT_POOL         = 127
IDS_FONTS_POOL          = 128
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
IDC_SYSINFO_VENDOR      = 301
IDC_SYSINFO_TFMS        = 302
IDC_SYSINFO_NAME        = 303
IDC_SYSINFO_TSC         = 304
IDC_SYSINFO_CPUID       = 305
IDC_SYSINFO_MMX         = 306
IDC_SYSINFO_SSE         = 307
IDC_SYSINFO_SSE2        = 308
IDC_SYSINFO_SSE3        = 309
IDC_SYSINFO_SSSE3       = 310
IDC_SYSINFO_SSE41       = 311
IDC_SYSINFO_SSE42       = 312
IDC_SYSINFO_AVX         = 313
IDC_SYSINFO_AVX2        = 314
IDC_SYSINFO_AVX512F     = 315
IDC_SYSINFO_RDRAND      = 316
IDC_SYSINFO_VMX_SVM     = 317
IDC_SYSINFO_X8664       = 318
IDC_SYSINFO_A0          = 319
IDC_SYSINFO_A1          = 320
IDC_SYSINFO_A2          = 321
IDC_SYSINFO_A3          = 322
IDC_SYSINFO_A4          = 323
IDC_SYSINFO_A5          = 324
IDC_SYSINFO_B0          = 325
IDC_SYSINFO_B1          = 326
IDC_SYSINFO_B2          = 327
IDC_SYSINFO_B3          = 328
IDC_SYSINFO_B4          = 329
IDC_SYSINFO_B5          = 330
IDC_SYSINFO_C0          = 331
IDC_SYSINFO_C1          = 332
IDC_SYSINFO_C2          = 333
IDC_SYSINFO_C3          = 334
IDC_SYSINFO_D0          = 335
IDC_SYSINFO_D1          = 336
IDC_SYSINFO_D2          = 337
IDC_SYSINFO_D3          = 338
IDC_SYSINFO_XCR0        = 339 
IDC_SYSINFO_XMM015      = 340
IDC_SYSINFO_YMM015      = 341
IDC_SYSINFO_ZMM015      = 342
IDC_SYSINFO_ZMM1631     = 343
IDC_SYSINFO_K07         = 344
IDC_SYSINFO_BNDREG      = 345
IDC_SYSINFO_BNDCSR      = 346
IDC_SYSINFO_ACPI        = 347
IDC_SYSINFO_MADT        = 348
IDC_SYSINFO_MADT_1      = 349
IDC_SYSINFO_MADT_2      = 350
IDC_SYSINFO_MADT_3      = 351
IDC_SYSINFO_MADT_4      = 352
IDC_SYSINFO_SRAT        = 353
IDC_SYSINFO_SRAT_1      = 354
IDC_SYSINFO_SRAT_2      = 355
IDC_SYSINFO_SRAT_3      = 356
IDC_SYSINFO_SRAT_4      = 357
IDC_SYSINFO_L1C         = 358
IDC_SYSINFO_L1C_V       = 359
IDC_SYSINFO_L1D         = 360
IDC_SYSINFO_L1D_V       = 361
IDC_SYSINFO_L2U         = 362
IDC_SYSINFO_L2U_V       = 363
IDC_SYSINFO_L3U         = 364
IDC_SYSINFO_L3U_V       = 365
IDC_SYSINFO_L4U         = 366
IDC_SYSINFO_L4U_V       = 367
IDC_SYSINFO_THREADS     = 368
IDC_SYSINFO_THR_V       = 369
IDC_SYSINFO_CORES       = 370
IDC_SYSINFO_CORES_V     = 371
IDC_SYSINFO_SOCKETS     = 372
IDC_SYSINFO_SOCK_V      = 373
IDC_SYSINFO_OSPR        = 374
IDC_SYSINFO_OSPR_V      = 375
IDC_SYSINFO_OSPR_M      = 376
IDC_SYSINFO_OSNM        = 377
IDC_SYSINFO_OSNM_V      = 378
IDC_SYSINFO_OSNM_C      = 379
IDC_SYSINFO_OSNM_M      = 380
IDC_SYSINFO_PGRP        = 381
IDC_SYSINFO_PGRP_V      = 382
IDC_SYSINFO_PGRP_C      = 383
IDC_SYSINFO_MEM         = 384
IDC_SYSINFO_MEM_V       = 385 
IDC_SYSINFO_MEM_A       = 386 
IDC_SYSINFO_MEM_AV      = 387 
IDC_SYSINFO_LRPG        = 388 
IDC_SYSINFO_LRPG_V      = 389 
IDC_SYSINFO_LRPG_E      = 390
IDB_SYSINFO_REPORT      = 391 
IDB_SYSINFO_CANCEL      = 392
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
IDB_MATH_DRAW           = 701
IDB_MATH_RUN            = 702
IDB_MATH_DEFAULTS       = 703
IDB_MATH_CANCEL         = 704
;---------- Identifiers for tab #4 = Operating system sheet -------------------; 
IDC_OS                  = 900
IDE_OS_UP               = 901
IDE_OS_TEXT             = 902
IDB_OS_REPORT           = 903
IDB_OS_CANCEL           = 904
;---------- Identifiers for tab #5 = Native os ( for WoW64 ) sheet ------------;
IDC_NATIVE_OS           = 1100
IDE_NATIVE_OS_UP        = 1101
IDE_NATIVE_OS_TEXT      = 1102
IDB_NAT_OS_REPORT       = 1103
IDB_NAT_OS_CANCEL       = 1104
;---------- Identifiers for tab #6 = Processor information sheet --------------; 
IDC_PROCESSOR           = 1300
IDE_PROC_UP             = 1301
IDE_PROC_TEXT           = 1302
IDB_PROC_REPORT         = 1303
IDB_PROC_CANCEL         = 1304
;---------- Identifiers for tab #7 = Topology information sheet ---------------; 
IDC_TOPOLOGY            = 1500
IDE_TOPOL_UP_1          = 1501
IDE_TOPOL_TEXT_1        = 1502
IDE_TOPOL_UP_2          = 1503
IDE_TOPOL_TEXT_2        = 1504
IDB_TOPOL_REPORT        = 1505
IDB_TOPOL_CANCEL        = 1506
;---------- Identifiers for tab #8 = Extended topology information sheet ------;
IDC_TOPOLOGY_EX         = 1700
IDE_TOP_EX_UP_1         = 1701
IDE_TOP_EX_TEXT_1       = 1702
IDE_TOP_EX_UP_2         = 1703
IDE_TOP_EX_TEXT_2       = 1704
IDB_TOPOL_EX_REPORT     = 1705
IDB_TOPOL_EX_CANCEL     = 1706
;---------- Identifiers for tab #9 = NUMA domains sheet -----------------------; 
IDC_NUMA                = 1900
IDE_NUMA_UP             = 1901
IDE_NUMA_TEXT           = 1902
IDB_NUMA_REPORT         = 1903
IDB_NUMA_CANCEL         = 1904
;---------- Identifiers for tab #10 = Processor groups sheet ------------------;
IDC_P_GROUPS            = 2100
IDE_P_GROUPS_UP         = 2101
IDE_P_GROUPS_TEXT       = 2102
IDB_P_GROUPS_REPORT     = 2103
IDB_P_GROUPS_CANCEL     = 2104
;---------- Identifiers for tab #11 = ACPI information sheet ------------------;  
IDC_ACPI                = 2300
IDE_ACPI_UP_1           = 2301
IDE_ACPI_TEXT_1         = 2302
IDE_ACPI_UP_2           = 2303
IDE_ACPI_TEXT_2         = 2304
IDB_ACPI_REPORT         = 2305
IDB_ACPI_CANCEL         = 2306
;---------- Identifiers for tab #12 = Affinized CPUID dump sheet --------------; 
IDC_AFF_CPUID           = 2500
IDE_A_CPUID_UP          = 2501
IDE_A_CPUID_TEXT        = 2502
IDB_A_CPUID_REPORT      = 2503
IDB_A_CPUID_CANCEL      = 2504
;---------- Identifiers for tab #13 = Kernel Mode Driver information sheet ----;
IDC_KMD                 = 2700
IDE_KMD_UP_1            = 2701
IDE_KMD_TEXT_1          = 2702
IDE_KMD_UP_2            = 2703
IDE_KMD_TEXT_2          = 2704
IDB_KMD_REPORT          = 2705
IDB_KMD_CANCEL          = 2706

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
;---------- CPUID names for system information --------------------------------;
STR_CPUID              = STR_FULL_KMD + 1
STR_MMX                = STR_CPUID + 1
STR_SSE                = STR_CPUID + 2
STR_SSE2               = STR_CPUID + 3
STR_SSE3               = STR_CPUID + 4
STR_SSSE3              = STR_CPUID + 5
STR_SSE41              = STR_CPUID + 6
STR_SSE42              = STR_CPUID + 7
STR_AVX                = STR_CPUID + 8
STR_AVX2               = STR_CPUID + 9
STR_AVX512F            = STR_CPUID + 10
STR_RDRAND             = STR_CPUID + 11
STR_VMX                = STR_CPUID + 12
STR_SVM                = STR_CPUID + 13
STR_X8664              = STR_CPUID + 14
;---------- CPUID names for system information, AVX512 sub-sets ---------------;
STR_AVX512CD           = STR_X8664 + 1 
STR_AVX512PF           = STR_AVX512CD + 1
STR_AVX512ER           = STR_AVX512CD + 2
STR_AVX512VL           = STR_AVX512CD + 3
STR_AVX512BW           = STR_AVX512CD + 4
STR_AVX512DQ           = STR_AVX512CD + 5
STR_AVX512_IFMA        = STR_AVX512CD + 6
STR_AVX512_VBMI        = STR_AVX512CD + 7
STR_AVX512_VBMI2       = STR_AVX512CD + 8
STR_AVX512_BF16        = STR_AVX512CD + 9
STR_AVX512_VAES        = STR_AVX512CD + 10
STR_AVX512_GFNI        = STR_AVX512CD + 11
STR_AVX512_VNNI        = STR_AVX512CD + 12
STR_AVX512_BITALG      = STR_AVX512CD + 13
STR_AVX512_VPOPCNTDQ   = STR_AVX512CD + 14
STR_AVX512_VPCLMULQ    = STR_AVX512CD + 15
STR_AVX512_VP2INTERS   = STR_AVX512CD + 16
STR_AVX512_FP16        = STR_AVX512CD + 17 
STR_AVX512_4FMAPS      = STR_AVX512CD + 18
STR_AVX512_4VNNIW      = STR_AVX512CD + 19
;---------- XCR0 and XGETBV context components names --------------------------;
STR_XCR0               = STR_AVX512_4VNNIW + 1 
STR_XMM_0_15           = STR_XCR0 + 1 
STR_YMM_0_15           = STR_XCR0 + 2
STR_ZMM_0_15           = STR_XCR0 + 3
STR_ZMM_16_31          = STR_XCR0 + 4
STR_K_0_7              = STR_XCR0 + 5
STR_BNDREG             = STR_XCR0 + 6
STR_BNDCSR             = STR_XCR0 + 7
;---------- ACPI objects visualized at system information screen --------------;
STR_ACPI               = STR_BNDCSR + 1
STR_MADT               = STR_ACPI + 1
STR_SRAT               = STR_ACPI + 2
;---------- Cache memory ------------------------------------------------------;
STR_L1_CODE            = STR_SRAT + 1
STR_L1_DATA            = STR_L1_CODE + 1
STR_L2_UNIFIED         = STR_L1_CODE + 2
STR_L3_UNIFIED         = STR_L1_CODE + 3
STR_L4_UNIFIED         = STR_L1_CODE + 4
;---------- Platform topology by WinAPI ---------------------------------------;
STR_THREADS            = STR_L4_UNIFIED + 1
STR_CORES              = STR_THREADS + 1
STR_SOCKETS            = STR_THREADS + 2
STR_OS_PROCESSORS      = STR_THREADS + 3
STR_OS_NUMA_NODES      = STR_THREADS + 4
STR_OS_GROUPS          = STR_THREADS + 5
;---------- Memory information by WinAPI --------------------------------------;
STR_OS_PHYSICAL        = STR_OS_GROUPS + 1
STR_OS_AVAILABLE       = STR_OS_PHYSICAL + 1
STR_OS_MIN_LARGE       = STR_OS_PHYSICAL + 2
;--- Assembler instructions and modes names for memory and cache benchmarks ---;
STR_AM_IA32_00         = STR_OS_MIN_LARGE + 1  
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
STR_AM_LCM             = STR_AM_IA32_00 + 30
STR_AM_RDRAND          = STR_AM_IA32_00 + 31
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
STR_REPORT             = STR_DRAW + 3
STR_EXIT               = STR_DRAW + 4
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
STR_MHZ                = STR_UNITS_BYTES + 17
STR_KERNEL_MODE        = STR_UNITS_BYTES + 18
STR_TRUE_CLOCK         = STR_UNITS_BYTES + 19
STR_TFMS               = STR_UNITS_BYTES + 20
STR_TSC                = STR_UNITS_BYTES + 21
STR_H                  = STR_UNITS_BYTES + 22
;---------- Up strings for GUI tables -----------------------------------------;
STR_PARM_VALUE_HEX     = STR_H + 1
STR_PARM_VALUE         = STR_PARM_VALUE_HEX + 1
STR_TOPOLOGY           = STR_PARM_VALUE_HEX + 2
STR_TOPOLOGY_SUMMARY   = STR_PARM_VALUE_HEX + 3
STR_NUMA               = STR_PARM_VALUE_HEX + 4 
STR_GROUPS             = STR_PARM_VALUE_HEX + 5 
STR_ACPI_LIST          = STR_PARM_VALUE_HEX + 6
STR_ACPI_SUMMARY       = STR_PARM_VALUE_HEX + 7
STR_AFF_CPUID          = STR_PARM_VALUE_HEX + 8 
STR_KMD                = STR_PARM_VALUE_HEX + 9
;---------- Strings for operating system information text ---------------------;
STR_OS_MEMORY_LOAD     = STR_KMD + 1
STR_OS_TOTAL_PHYSICAL  = STR_OS_MEMORY_LOAD + 1
STR_OS_AVAIL_PHYSICAL  = STR_OS_MEMORY_LOAD + 2 
STR_OS_TOTAL_PAGE_FILE = STR_OS_MEMORY_LOAD + 3
STR_OS_AVAIL_PAGE_FILE = STR_OS_MEMORY_LOAD + 4
STR_OS_TOTAL_VIRTUAL   = STR_OS_MEMORY_LOAD + 5
STR_OS_AVAIL_VIRTUAL   = STR_OS_MEMORY_LOAD + 6
STR_OS_EXT_VIRTUAL     = STR_OS_MEMORY_LOAD + 7
STR_OS_MIN_ADDRESS     = STR_OS_MEMORY_LOAD + 8
STR_OS_MAX_ADDRESS     = STR_OS_MEMORY_LOAD + 9
STR_OS_PROC_MASK       = STR_OS_MEMORY_LOAD + 10
STR_OS_PROC_TYPE       = STR_OS_MEMORY_LOAD + 11
STR_OS_ALLOC_GRAN      = STR_OS_MEMORY_LOAD + 12
STR_OS_PROC_LEVEL      = STR_OS_MEMORY_LOAD + 13
STR_OS_PROC_REVISION   = STR_OS_MEMORY_LOAD + 14
STR_OS_PROC_CURRENT    = STR_OS_MEMORY_LOAD + 15
STR_OS_PROC_TOTAL      = STR_OS_MEMORY_LOAD + 16
STR_OS_PROC_GROUPS     = STR_OS_MEMORY_LOAD + 17
STR_OS_NUMA_DOMAINS    = STR_OS_MEMORY_LOAD + 18
STR_OS_NORMAL_PAGE     = STR_OS_MEMORY_LOAD + 19
STR_OS_LARGE_PAGE      = STR_OS_MEMORY_LOAD + 20
STR_OS_DISABLED        = STR_OS_MEMORY_LOAD + 21
STR_OS_ENABLED         = STR_OS_MEMORY_LOAD + 22

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

;---------- Offsets definitions for bind buffer -------------------------------;
BUFFER_CPU_VENDOR      = 0
BUFFER_CPU_TFMS        = BUFFER_CPU_VENDOR    + 8
BUFFER_CPU_NAME        = BUFFER_CPU_TFMS      + 15
BUFFER_CPU_TSC         = BUFFER_CPU_NAME      + 8
BUFFER_CPU_BITMAP      = BUFFER_CPU_TSC       + 32
BUFFER_AVX512_BITMAP   = BUFFER_CPU_BITMAP    + 8
BUFFER_OS_BITMAP       = BUFFER_AVX512_BITMAP + 8
BUFFER_METHOD_BITMAP   = BUFFER_OS_BITMAP     + 8
BUFFER_VIEWER_OS       = BUFFER_METHOD_BITMAP + 8 

;---------- Macro definitions for binders scripts instructions ----------------;
macro BIND_STOP
{ DB  0 }
MACRO BIND_STRING srcid, dstid
{ DD 01h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_INFO srcid, dstid
{ DD 02h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_BIG srcid, dstid
{ DD 03h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_BOOL srcid, srcbit, dstid
{ DD 04h + (srcid) SHL 9 + (srcbit) SHL 6 + (dstid) SHL 19 }
COMBO_STOP_ON  = 0
COMBO_STOP_OFF = 1
COMBO_CURRENT  = 2
COMBO_ADD      = 3
COMBO_INACTIVE = 4
MACRO BIND_COMBO srcid, dstid
{ DD 05h + (srcid) SHL 6 + (dstid) SHL 19 }
MACRO BIND_FONT srcid, dstid
{ DD 06h + (srcid) SHL 6 + (dstid) SHL 19 }

;---------- Macro definitions for CPU/OS features bitmaps builders scripts ----;
R_EAX = 0
R_EBX = 1
R_ECX = 2
R_EDX = 3
MACRO ENTRY_STOP
{ DB 00h }
MACRO ENTRY_CPUID function, register, bit 
{
DB 01h + ( register SHL 6 )
DD function
DB bit
}
MACRO ENTRY_CPUID_S function, subfunction, register, bit 
{
DB 02h + ( register SHL 6 )
DD function, subfunction
DB bit
}
MACRO PREFIX_CPUID function, register, bit 
{
DB 03h + ( register SHL 6 )
DD function
DB bit
}
MACRO PREFIX_CPUID_S function, subfunction, register, bit 
{
DB 04h + ( register SHL 6 )
DD function, subfunction
DB bit
}
MACRO ENTRY_XCR0 bit
{ DB  05h , bit }
MACRO PREFIX_XCR0 bit
{ DB  06h , bit }

;---------- GUI definitions ---------------------------------------------------; 
CLEARTYPE_QUALITY = 5
ID_FONT_1         = 0
ID_FONT_2         = 1
