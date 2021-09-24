;=====================================================================================;
;                                                                                     ;
; Project NCRB ( NUMA CPU&RAM Benchmarks v2.xx.xx ).                                  ;
; (C)2021 Ilya Manusov.                                                               ;
; manusov1969@gmail.com                                                               ;
; Previous version v1.xx.xx                                                           ; 
; https://github.com/manusov/NumaCpuAndRamBenchmarks                                  ;
; This version v2.xx.xx ( UNDER CONSTRUCTION )                                        ;
; https://github.com/manusov/Prototyping                                              ; 
;                                                                                     ;
; DATA.ASM = source file for FASM                                                     ; 
; DATA.DLL = translation result, universal resource library for Win32 and Win64       ;
; Note. Resource-only DLLs is universal,                                              ; 
; it can be loaded both by ia32 and x64 applications.                                 ;
; See also other components:                                                          ;
; NCRB32.ASM, NCRB64.ASM, KMD32.ASM, KMD64.ASM.                                       ;
;                                                                                     ;
; Translation by Flat Assembler version 1.73.27 ( Jan 27, 2021 ).                     ;
; http://flatassembler.net/                                                           ;
;                                                                                     ;
; Edit by FASM Editor 2.0.                                                            ; 
; Use this editor for correct source file tabulations and format. (!)                 ;
; https://fasmworld.ru/instrumenty/fasm-editor-2-0/                                   ;
;                                                                                     ;
; User mode debug by OllyDbg ( 32-bit, actual for module NCRB32.EXE )                 ;
; http://www.ollydbg.de/version2.html                                                 ;
;                                                                                     ;
; User mode debug by FDBG ( 64-bit, actual for module NCRB64.EXE )                    ;
; https://board.flatassembler.net/topic.php?t=9689&postdays=0&postorder=asc&start=180 ;
; ( Search for archive fdbg0025.zip )                                                 ;
;                                                                                     ;
; Icons from open icon library                                                        ;
; https://sourceforge.net/projects/openiconlibrary/                                   ;
;                                                                                     ;
;=====================================================================================;

format PE GUI 4.0 DLL
include 'win32a.inc'
include 'global\definitions.inc'
section '.rsrc' resource data readable

;---------- Root directory of resources ---------------------------------------;
directory \
RT_DIALOG     , dialogs , \ 
RT_MENU       , menus   , \ 
RT_RCDATA     , raws    , \
RT_ICON       , icons   , \
RT_GROUP_ICON , gicons 

;---------- Resources directory for application main window and tabs ----------;
resource dialogs,\
IDD_MAIN        , LANG_ENGLISH + SUBLANG_DEFAULT, mainDialog    , \
IDD_SYSINFO     , LANG_ENGLISH + SUBLANG_DEFAULT, tabSysinfo    , \
IDD_MEMORY      , LANG_ENGLISH + SUBLANG_DEFAULT, tabMemory     , \
IDD_MATH        , LANG_ENGLISH + SUBLANG_DEFAULT, tabMath       , \
IDD_OS          , LANG_ENGLISH + SUBLANG_DEFAULT, tabOs         , \
IDD_NATIVE_OS   , LANG_ENGLISH + SUBLANG_DEFAULT, tabNativeOs   , \
IDD_PROCESSOR   , LANG_ENGLISH + SUBLANG_DEFAULT, tabProcessor  , \
IDD_TOPOLOGY    , LANG_ENGLISH + SUBLANG_DEFAULT, tabTopology   , \
IDD_TOPOLOGY_EX , LANG_ENGLISH + SUBLANG_DEFAULT, tabTopologyEx , \
IDD_NUMA        , LANG_ENGLISH + SUBLANG_DEFAULT, tabNuma       , \
IDD_PGROUPS     , LANG_ENGLISH + SUBLANG_DEFAULT, tabPgroups    , \
IDD_ACPI        , LANG_ENGLISH + SUBLANG_DEFAULT, tabAcpi       , \
IDD_AFF_CPUID   , LANG_ENGLISH + SUBLANG_DEFAULT, tabAffCpuid   , \
IDD_KMD         , LANG_ENGLISH + SUBLANG_DEFAULT, tabKmd

;---------- Tabbed sheet ------------------------------------------------------;
dialog      mainDialog,        '',                      0,   0, 410, 282, DS_CENTER + WS_CAPTION + WS_SYSMENU, 0, IDR_MENU, 'Verdana', 10
dialogitem  'SysTabControl32', '', IDC_TAB          ,   1,   1, 408,  29, WS_VISIBLE + TCS_MULTILINE
enddialog
;---------- Tab 1 = system information ----------------------------------------;
dialog      tabSysinfo    , '',                         2, 30, 403,  253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_SYSINFO         ,   0,  3, 380,   10, WS_VISIBLE
dialogitem  'STATIC'      , '', IDC_SYSINFO_VENDOR  ,   1,  17,  55,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_TFMS    ,  58,  17,  70,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_NAME    , 130,  17, 196,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_TSC     , 328,  17,  74,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_CPUID   ,   7,  33,  49,  10, WS_VISIBLE + SS_LEFT + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MMX     ,  32,  33,  21,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SSE     ,  55,  33,  21,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SSE2    ,  78,  33,  24,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SSE3    , 104,  33,  24,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SSSE3   , 130,  33,  28,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SSE41   , 160,  33,  29,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SSE42   , 191,  33,  29,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_AVX     , 222,  33,  21,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_AVX2    , 245,  33,  24,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_AVX512F , 271,  33,  36,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_RDRAND  , 309,  33,  34,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_VMX_SVM , 345,  33,  21,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_X8664   , 369,  33,  33,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_A0      ,  32,  46,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_A1      ,  94,  46,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_A2      , 156,  46,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_A3      , 218,  46,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_A4      , 280,  46,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_A5      , 342,  46,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_B0      ,  32,  59,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_B1      ,  94,  59,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_B2      , 156,  59,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_B3      , 218,  59,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_B4      , 280,  59,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_B5      , 342,  59,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_C0      ,  32,  72,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_C1      , 125,  72,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_C2      , 218,  72,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_C3      , 311,  72,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_D0      ,  32,  85,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_D1      , 125,  85,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_D2      , 218,  85,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_D3      , 311,  85,  91,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_XCR0    ,   7, 100,  49,  10, WS_VISIBLE + SS_LEFT + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_XMM015  ,  32, 100,  57,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_YMM015  ,  91, 100,  57,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_ZMM015  , 150, 100,  57,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_ZMM1631 , 209, 100,  60,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_K07     , 271, 100,  43,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_BNDREG  , 316, 100,  42,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_BNDCSR  , 360, 100,  42,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_ACPI    ,   7, 115,  49,  10, WS_VISIBLE + SS_LEFT + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MADT    ,  32, 115,  28,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MADT_1  ,  62, 115,  36,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MADT_2  , 100, 115,  44,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MADT_3  , 146, 115,  80,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MADT_4  , 228, 115, 171,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SRAT    ,  32, 128,  28,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SRAT_1  ,  62, 128,  36,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SRAT_2  , 100, 128,  44,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SRAT_3  , 146, 128,  80,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SRAT_4  , 228, 128, 171,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L1C     ,   1, 146,  45,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L1C_V   ,  48, 146, 105,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L1D     ,   1, 160,  45,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L1D_V   ,  48, 160, 105,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L2U     ,   1, 174,  45,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L2U_V   ,  48, 174, 105,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L3U     ,   1, 188,  45,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L3U_V   ,  48, 188, 105,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L4U     ,   1, 202,  45,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_L4U_V   ,  48, 202, 105,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_THREADS , 170, 146,  38,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_THR_V   , 210, 146,  29,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_CORES   , 250, 146,  38,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_CORES_V , 290, 146,  29,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SOCKETS , 330, 146,  38,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_SOCK_V  , 370, 146,  29,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSPR    , 170, 160,  82,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSPR_V  , 255, 160,  28,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSPR_M  , 286, 160, 113,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSNM    , 170, 174,  82,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSNM_V  , 255, 174,  28,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSNM_C  , 286, 174, 113,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_OSNM_M  , 170, 188, 229,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_PGRP    , 170, 202,  82,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_PGRP_V  , 255, 202,  28,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_PGRP_C  , 286, 202, 113,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MEM     ,   1, 217,  79,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MEM_V   ,  82, 217,  50,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MEM_A   , 134, 217,  40,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_MEM_AV  , 176, 217,  50,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_LRPG    , 240, 217,  79,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_LRPG_V  , 321, 217,  38,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'STATIC'      , '', IDC_SYSINFO_LRPG_E  , 361, 217,  38,  10, WS_VISIBLE + SS_SUNKEN + SS_CENTER + SS_CENTERIMAGE
dialogitem  'BUTTON'      , '', IDB_SYSINFO_REPORT  , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_SYSINFO_CANCEL  , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 2 = memory and cache benchmark --------------------------------; 
dialog      tabMemory     , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_MEMORY          ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'STATIC'      , '', IDC_MEMORY_FRAME_1  ,   1,  17, 400,  90, WS_VISIBLE + SS_ETCHEDFRAME
dialogitem  'STATIC'      , '', IDC_MEMORY_FRAME_2  ,   1, 109, 400,  55, WS_VISIBLE + SS_ETCHEDFRAME
dialogitem  'STATIC'      , '', IDC_MEMORY_FRAME_3  ,   1, 166,  75,  82, WS_VISIBLE + SS_ETCHEDFRAME
dialogitem  'STATIC'      , '', IDC_MEMORY_FRAME_4  ,  78, 166,  75,  82, WS_VISIBLE + SS_ETCHEDFRAME
dialogitem  'STATIC'      , '', IDC_MEMORY_FRAME_5  , 155, 166,  87,  82, WS_VISIBLE + SS_ETCHEDFRAME
dialogitem  'STATIC'      , '', IDC_MEMORY_FRAME_6  , 244, 166, 157,  65, WS_VISIBLE + SS_ETCHEDFRAME
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A0   ,   5,  19, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A1   ,   5,  28, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A2   ,   5,  37, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A3   ,   5,  46, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A4   ,   5,  55, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A5   ,   5,  64, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A6   ,   5,  73, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A7   ,   5,  82, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_A8   ,   5,  91, 130,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B0   , 137,  19, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B1   , 137,  28, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B2   , 137,  37, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B3   , 137,  46, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B4   , 137,  55, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B5   , 137,  64, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B6   , 137,  73, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B7   , 137,  82, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_B8   , 137,  91, 100,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C0   , 245,  19, 150,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C1   , 245,  28, 150,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C2   , 245,  37, 150,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C3   , 245,  46, 150,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C4   , 245,  55, 150,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C5   , 245,  64, 150,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C6   , 245,  82,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_ASM_C7   , 245,  91,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NONTEMP  , 330,  82,  50,   9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_FORCE32  , 330,  91,  50,   9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_L1       ,   5, 114,  40,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP
dialogitem  'BUTTON'      , '', IDB_MEMORY_L2       ,   5, 123,  40,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_L3       ,   5, 132,  40,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_L4       ,   5, 141,  40,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_DRAM     ,   5, 150,  40,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_CUSTOM   ,  53, 114,  90,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_FILE     ,  53, 125,  90,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_GPU      ,  53, 137,  90,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_PHYSICAL ,  53, 149,  90,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'EDIT'        , '', IDE_MEMORY_B_SIZE   , 147, 113,  77,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'EDIT'        , '', IDE_MEMORY_F_SIZE   , 147, 125,  77,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'EDIT'        , '', IDE_MEMORY_G_SIZE   , 147, 137,  77,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'EDIT'        , '', IDE_MEMORY_M_START  , 147, 149,  77,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'EDIT'        , '', IDE_MEMORY_M_STOP   , 231, 149,  77,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'STATIC'      , '', IDC_MEMORY_M_HYPHEN , 226, 150,   5,  10, WS_VISIBLE
dialogitem  'COMBOBOX'    , '', IDC_MEMORY_COMBO_F  , 231, 113,  85,  10, WS_VISIBLE + CBS_DROPDOWNLIST + CBS_HASSTRINGS + WS_VSCROLL
dialogitem  'COMBOBOX'    , '', IDC_MEMORY_COMBO_F  , 231, 128,  85,  10, WS_VISIBLE + CBS_DROPDOWNLIST + CBS_HASSTRINGS + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_MEMORY_MTRR_WB  , 327, 114,  68,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_MTRR_WT  , 327, 123,  68,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_MTRR_WC  , 327, 132,  68,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_MTRR_WP  , 327, 141,  68,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_MTRR_UC  , 327, 150,  68,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_PARALLEL ,   5, 169,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_HT       ,   5, 178,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_PG       ,   5, 187,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NUMA_U   ,   5, 205,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NUMA_S   ,   5, 214,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NUMA_O   ,   5, 223,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NUMA_N   ,   5, 232,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NORMAL   ,  82, 169,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP
dialogitem  'BUTTON'      , '', IDB_MEMORY_SK_63    ,  82, 178,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_SK_4095  ,  82, 187,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_SK_CSTM  ,  82, 196,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'EDIT'        , '', IDE_MEMORY_SK_SIZE  , 100, 208,  43,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'BUTTON'      , '', IDB_MEMORY_LP       ,  82, 223,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_NO_PF    , 159, 169,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP
dialogitem  'BUTTON'      , '', IDB_MEMORY_DEF_PF   , 159, 178,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_MED_PF   , 159, 187,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_LNG_PF   , 159, 196,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_BLK_PF   , 159, 205,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_CST_PF   , 159, 214,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP
dialogitem  'EDIT'        , '', IDE_MEMORY_PF_SIZE  , 177, 226,  43,  10, WS_VISIBLE + WS_CHILD + WS_BORDER + WS_TABSTOP + ES_AUTOHSCROLL + WS_DISABLED
dialogitem  'BUTTON'      , '', IDB_MEMORY_BRF      , 248, 169,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP  
dialogitem  'BUTTON'      , '', IDB_MEMORY_CRF      , 248, 178,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_BRF_A    , 248, 187,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_CRF_A    , 248, 196,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_ALL_P    , 327, 169,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP + WS_GROUP
dialogitem  'BUTTON'      , '', IDB_MEMORY_X_16     , 327, 178,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_X_32     , 327, 187,  70,   9, WS_VISIBLE + WS_CHILD + BS_AUTORADIOBUTTON + WS_TABSTOP 
dialogitem  'BUTTON'      , '', IDB_MEMORY_3D_DRAW  , 248, 214,  150,  9, WS_VISIBLE + WS_CHILD + BS_AUTOCHECKBOX + WS_TABSTOP
dialogitem  'BUTTON'      , '', IDB_MEMORY_DRAW     , 245, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_MEMORY_RUN      , 284, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_MEMORY_DEFAULTS , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_MEMORY_CANCEL   , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 3 = processor mathematics benchmarks --------------------------; 
dialog      tabMath       , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_MATH            ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'BUTTON'      , '', IDB_MATH_DRAW       , 245, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_MATH_RUN        , 284, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_MATH_DEFAULTS   , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_MATH_CANCEL     , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 4 = operating system info -------------------------------------; 
dialog      tabOs         , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_OS              ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_OS_UP           ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_OS_TEXT         ,   3,  30, 400, 198, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_OS_REPORT       , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_OS_CANCEL       , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 5 = native os info, useable if run ncrb32 under win64 ---------;
dialog      tabNativeOs   , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_NATIVE_OS       ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_NATIVE_OS_UP    ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_NATIVE_OS_TEXT  ,   3,  30, 400, 198, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_NAT_OS_REPORT   , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_NAT_OS_CANCEL   , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 6 = processor info --------------------------------------------; 
dialog      tabProcessor  , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_PROCESSOR       ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_PROC_UP         ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_PROC_TEXT       ,   3,  30, 400, 198, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_PROC_REPORT     , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_PROC_CANCEL     , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 7 = platform topology info by winapi --------------------------; 
dialog      tabTopology   , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_TOPOLOGY        ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_TOPOL_UP_1      ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_TOPOL_TEXT_1    ,   3,  30, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'EDIT'        , '', IDE_TOPOL_UP_2      ,   3, 126, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_TOPOL_TEXT_2    ,   3, 139, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_TOPOL_REPORT    , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_TOPOL_CANCEL    , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 8 = platform topology info by winapi (ex, extended) -----------; 
dialog      tabTopologyEx , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_TOPOLOGY_EX     ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_TOP_EX_UP_1     ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_TOP_EX_TEXT_1   ,   3,  30, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'EDIT'        , '', IDE_TOP_EX_UP_2     ,   3, 126, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_TOP_EX_TEXT_2   ,   3, 139, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_TOPOL_EX_REPORT , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_TOPOL_EX_CANCEL , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 9 = platform NUMA domains list- -------------------------------; 
dialog      tabNuma       , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_NUMA            ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_NUMA_UP         ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_NUMA_TEXT       ,   3,  30, 400, 198, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_NUMA_REPORT     , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_NUMA_CANCEL     , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 10 = platform processor groups list ---------------------------; 
dialog      tabPgroups    , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_P_GROUPS        ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_P_GROUPS_UP     ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_P_GROUPS_TEXT   ,   3,  30, 400, 198, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_P_GROUPS_REPORT , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_P_GROUPS_CANCEL , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 11 = ACPI tables list -----------------------------------------; 
dialog      tabAcpi       , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_ACPI            ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_ACPI_UP_1       ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_ACPI_TEXT_1     ,   3,  30, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'EDIT'        , '', IDE_ACPI_UP_2       ,   3, 126, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_ACPI_TEXT_2     ,   3, 139, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_ACPI_REPORT     , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_ACPI_CANCEL     , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 12 = affinized CPUID dump, per each logical CPU ---------------; 
dialog      tabAffCpuid   , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_AFF_CPUID       ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_A_CPUID_UP      ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_A_CPUID_TEXT    ,   3,  30, 400, 198, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_A_CPUID_REPORT  , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_A_CPUID_CANCEL  , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Tab 13 = kernel mode driver information and probe results ---------; 
dialog      tabKmd        , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_KMD             ,   0,   3, 380,  10, WS_VISIBLE
dialogitem  'EDIT'        , '', IDE_KMD_UP_1        ,   3,  17, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_KMD_TEXT_1      ,   3,  30, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'EDIT'        , '', IDE_KMD_UP_2        ,   3, 126, 400,  10, WS_VISIBLE + WS_BORDER + ES_READONLY
dialogitem  'EDIT'        , '', IDE_KMD_TEXT_2      ,   3, 139, 400,  89, WS_VISIBLE + WS_BORDER + ES_MULTILINE + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_READONLY + WS_VSCROLL
dialogitem  'BUTTON'      , '', IDB_KMD_REPORT      , 323, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
dialogitem  'BUTTON'      , '', IDB_KMD_CANCEL      , 362, 234,  38,  13, WS_VISIBLE + BS_DEFPUSHBUTTON + BS_FLAT
enddialog                                   
;---------- Application main menu and service items ---------------------------; 
resource menus, IDR_MENU, LANG_ENGLISH + SUBLANG_DEFAULT, mainMenu
menu mainMenu
menuitem '&File'        , 0 , MFR_POPUP
menuitem '&Save report' , IDM_SAVE_REPORT , 0 , MFS_DISABLED
menuitem 'S&ave image'  , IDM_SAVE_IMAGE  , 0 , MFS_DISABLED
menuseparator
menuitem '&Load report' , IDM_LOAD_REPORT , 0 , MFS_DISABLED
menuseparator
menuitem 'E&xit'        , IDM_EXIT, MFR_END
menuitem '&Help'        , 0 , MFR_POPUP + MFR_END
menuitem '&About...'    , IDM_ABOUT, MFR_END
;---------- Raw resources strings and binder scripts --------------------------;
; Note. Strings represented as raw resources (not as string resources) for
; compact encoding: 1 byte per char.  
resource raws, \
IDS_STRINGS_POOL    , LANG_ENGLISH + SUBLANG_DEFAULT , stringsPool       , \
IDS_BINDERS_POOL    , LANG_ENGLISH + SUBLANG_DEFAULT , bindersPool       , \ 
IDS_CPU_COMMON_POOL , LANG_ENGLISH + SUBLANG_DEFAULT , cpuCommonFeatures , \ 
IDS_CPU_AVX512_POOL , LANG_ENGLISH + SUBLANG_DEFAULT , cpuAvx512Features , \
IDS_OS_CONTEXT_POOL , LANG_ENGLISH + SUBLANG_DEFAULT , osContextFeatures , \
IDS_CPU_METHOD_POOL , LANG_ENGLISH + SUBLANG_DEFAULT , cpuMethodFeatures , \
IDS_ACPI_DATA_POOL  , LANG_ENGLISH + SUBLANG_DEFAULT , acpiData          , \
IDS_IMPORT_POOL     , LANG_ENGLISH + SUBLANG_DEFAULT , importList        , \ 
IDS_FONTS_POOL      , LANG_ENGLISH + SUBLANG_DEFAULT , fontList 

;---------- Raw resource for strings pool -------------------------------------;
resdata stringsPool
;---------- Brief names for application sheets --------------------------------; 
DB  'sysinfo'           , 0
DB  'memory'            , 0
DB  'math'              , 0
DB  'operating system'  , 0
DB  'native os'         , 0
DB  'processor'         , 0
DB  'topology'          , 0
DB  'extended topology' , 0
DB  'numa domains'      , 0
DB  'processor groups'  , 0
DB  'acpi'              , 0
DB  'affinized cpuid'   , 0
DB  'kernel mode'       , 0
;---------- Full names for application sheets --------------------------------;
DB  'System summary.'                                                                          , 0
DB  'Memory and cache benchmarks, bandwidth (megabytes per second) and latency (nanoseconds).' , 0
DB  'Processor mathematics and load-store operations benchmarks.'                              , 0
DB  'System information by WinAPI.'                                                            , 0
DB  'Native OS information for ia32 application under x64 OS.'                                 , 0
DB  'Processor memory addressing, caches and TLB information by CPUID.'                        , 0
DB  'Platform topology by WinAPI GetLogicalProcessorInformation().'                            , 0
DB  'Platform topology by WinAPI GetLogicalProcessorInformationEx().'                          , 0
DB  'NUMA domains list by WinAPI GetNumaHighestNodeNumber() and other.'                        , 0
DB  'Processor groups list by WinAPI GetActiveProcessorGroupCount() and other.'                , 0
DB  'ACPI tables list by WinAPI EnumSystemFirmwareTables() and other.'                         , 0
DB  'CPUID per each thread affinized by WinAPI SetThreadAffinityMask().'                       , 0
DB  'Kernel mode driver load status and privileged resources info.'                            , 0
;---------- CPUID names for system information --------------------------------;
DB  'CPUID'     , 0
DB  'MMX'       , 0
DB  'SSE'       , 0
DB  'SSE2'      , 0
DB  'SSE3'      , 0
DB  'SSSE3'     , 0
DB  'SSE4.1'    , 0
DB  'SSE4.2'    , 0
DB  'AVX'       , 0
DB  'AVX2'      , 0
DB  'AVX512F'   , 0
DB  'RDRAND'    , 0
DB  'VMX'       , 0
DB  'SVM'       , 0
DB  'x86-64'    , 0
;---------- CPUID names for system information, AVX512 sub-sets ---------------;
DB  'AVX512CD'             , 0
DB  'AVX512PF'             , 0
DB  'AVX512ER'             , 0
DB  'AVX512VL'             , 0
DB  'AVX512BW'             , 0
DB  'AVX512DQ'             , 0
DB  'AVX512_IFMA'          , 0
DB  'AVX512_VBMI'          , 0
DB  'AVX512_VBMI2'         , 0
DB  'AVX512_BF16'          , 0
DB  'AVX512+VAES'          , 0
DB  'AVX512+GFNI'          , 0
DB  'AVX512_VNNI'          , 0
DB  'AVX512_BITALG'        , 0
DB  'AVX512_VPOPCNTDQ'     , 0
DB  'AVX512+VPCLMULQDQ'    , 0
DB  'AVX512_VP2INTERSECT'  , 0
DB  'AVX512_FP16'          , 0
DB  'AVX512_4FMAPS'        , 0
DB  'AVX512_4VNNIW'        , 0
;---------- XCR0 and XGETBV context components names --------------------------;
DB  'XCR0'                 , 0
DB  'XMM[0-15]'            , 0
DB  'YMM[0-15]'            , 0
DB  'ZMM[0-15]'            , 0
DB  'ZMM[16-31]'           , 0
DB  'K[0-7]'               , 0
DB  'BNDREG'               , 0
DB  'BNDCSR'               , 0
;---------- ACPI objects visualized at system information screen --------------;
DB  'ACPI'                 , 0
DB  'MADT'                 , 0
DB  'SRAT'                 , 0
;---------- Cache memory ------------------------------------------------------;
DB  'L1 Code'              , 0
DB  'L1 Data'              , 0
DB  'L2 Unified'           , 0
DB  'L3 Unified'           , 0
DB  'L4 Unified'           , 0
;---------- Platform topology by WinAPI ---------------------------------------;
DB  'Threads'              , 0
DB  'Cores'                , 0
DB  'Sockets'              , 0
DB  'OS processors'        , 0
DB  'OS NUMA nodes'        , 0
DB  'OS processor groups'  , 0
;---------- Memory information by WinAPI --------------------------------------;
DB  'OS physical memory'   , 0
DB  'Available'            , 0
DB  'Minimum large page'   , 0
;--- Assembler instructions and modes names for memory and cache benchmarks ---;
DB  'Read x86 (MOV)'                      , 0
DB  'Write x86 (MOV)'                     , 0
DB  'Copy x86 (MOV)'                      , 0
DB  'Modify x86 (NOT)'                    , 0
DB  'Write x86 strings (REP STOSD)'       , 0 
DB  'Copy x86 strings (REP MOVSD)'        , 0
DB  'Read x86-64 (MOV)'                   , 0
DB  'Write x86-64 (MOV)'                  , 0
DB  'Copy x86-64 (MOV)'                   , 0
DB  'Modify x86-64 (NOT)'                 , 0
DB  'Write x86-64 strings (REP STOSQ)'    , 0 
DB  'Copy x86-64 strings (REP MOVSQ)'     , 0
DB  'Read MMX-64 (MOVQ)'                  , 0
DB  'Write MMX-64 (MOVQ)'                 , 0
DB  'Copy MMX-64 (MOVQ)'                  , 0
DB  'Read SSE-128 (MOVAPS)'               , 0  
DB  'Write SSE-128 (MOVAPS)'              , 0
DB  'Copy SSE-128 (MOVAPS)'               , 0
DB  'Read AVX-256 (VMOVAPD)'              , 0
DB  'Write AVX-256 (VMOVAPD)'             , 0
DB  'Copy AVX-256 (VMOVAPD)'              , 0
DB  'Read AVX-512 (VMOVAPD)'              , 0   
DB  'Write AVX-512 (VMOVAPD)'             , 0
DB  'Copy AVX-512 (VMOVAPD)'              , 0
DB  'Dot product FMA-256 (VFMADD231PD)'   , 0 
DB  'Dot product FMA-512 (VFMADD231PD)'   , 0
DB  'Gather read AVX-256 (VGATHERQPD)'    , 0
DB  'Gather read AVX-512 (VGATHERQPD)'    , 0
DB  'Scatter write AVX-512 (VSCATTERQPD)' , 0
DB  'Cache optimized write (CLZERO)'      , 0
DB  'Latency (LCM)'                       , 0  
DB  'Latency (RDRAND)'                    , 0
DB  'Nontemporal'                         , 0
DB  'Force 32x2'                          , 0
;---------- Target objects names for memory and cache benchmark ---------------;  
DB  'L1 cache'                 , 0
DB  'L2 cache'                 , 0
DB  'L3 cache'                 , 0
DB  'L4 cache'                 , 0
DB  'DRAM'                     , 0
DB  'Custom block size'        , 0
DB  'Memory mapped file size'  , 0 
DB  'GPU memory block size'    , 0
DB  'Physical map start-stop'  , 0
;---------- Memory status names -----------------------------------------------;
DB  'Write back'               , 0
DB  'Write through'            , 0
DB  'Write combining'          , 0
DB  'Write protected'          , 0
DB  'Uncacheable'              , 0
;---------- Memory access and platform topology options names -----------------;
DB  'Parallel threads'         , 0
DB  'Hyper-threading'          , 0
DB  'Processor groups'         , 0
DB  'NUMA unaware'             , 0
DB  'Single domain'            , 0
DB  'Optimal'                  , 0
DB  'Non optimal'              , 0
DB  'Normal access'            , 0
DB  'One per 64 bytes'         , 0
DB  'One per 4K'               , 0
DB  'One per custom'           , 0
DB  'Large pages'              , 0
;---------- Memory prefetch options names -------------------------------------;
DB  'No soft prefetch'         , 0
DB  'Default distance'         , 0
DB  'Medium'                   , 0
DB  'Long'                     , 0
DB  'Block prefetch'           , 0
DB  'Custom distance'          , 0
;---------- Measurement options names -----------------------------------------;
DB  'Measure brief'                        , 0
DB  'Measure carefull'                     , 0
DB  'Brief adaptive'                       , 0
DB  'Carefull adaptive'                    , 0
DB  'All pixels'                           , 0
DB  'X / 16'                               , 0
DB  'X / 32'                               , 0
DB  'Draw 3D chart by repeat measurements' , 0
;---------- Buttons names -----------------------------------------------------;
DB  'Draw'     , 0
DB  'Run'      , 0
DB  'Defaults' , 0
DB  'Report'   , 0
DB  'Exit'     , 0
;---------- Memory size and speed units, additional information ---------------;
DB  'Bytes'        , 0
DB  'KB'           , 0
DB  'MB'           , 0
DB  'GB'           , 0
DB  'TB'           , 0
DB  'MBPS'         , 0
DB  'nanoseconds'  , 0
DB  'none'         , 0
DB  'mask = '      , 0
DB  'Enabled'      , 0
DB  'Disabled'     , 0
DB  '-'            , 0
DB  'TSC clks'     , 0
DB  'CPU clks'     , 0
DB  'data move'    , 0
DB  'calculation'  , 0
DB  'ns'           , 0
DB  'MHz'          , 0
DB  'Kernel mode'  , 0
DB  'True clock'   , 0
DB  'TFMS='        , 0
DB  'TSC='         , 0
DB  'h'            , 0
;---------- Up strings for GUI tables -----------------------------------------; 
DB  ' Parameter                     | Value                      | Hex'   , 0
DB  ' Parameter                     | Value'                              , 0
DB  ' Topology unit  | Logical CPU affinity | Comments'                   , 0
DB  ' Cache          | Size                 | Count'                      , 0
DB  ' NUMA domain  | Affinity (hex)           | Available memory at node' , 0
DB  ' Group  | Processors count'                                          , 0
DB  ' Sign | OEM ID | OEM Table ID | Creator ID | OEM Rev   | Creator Rev | Rev' , 0
DB  ' Summary'                                                            , 0
DB  ' Thread  | Function   | EAX      | EBX      | ECX      | EDX'        , 0
DB  ' APIC ID  | TSC(MHz)   | APERF(MHz) | MPERF(MHz) | APIC(MHz)  | CR0      | CR4' , 0 
;---------- Strings for operating system information text ---------------------;
DB  'Memory load'                  , 0
DB  'Total physical memory'        , 0
DB  'Available physical memory'    , 0
DB  'Total page file'              , 0
DB  'Available page file'          , 0
DB  'Total virtual user space'     , 0
DB  'Available virtual user space' , 0
DB  'Extended virtual'             , 0
DB  'Application minimum address'  , 0
DB  'Application maximum address'  , 0
DB  'Active processor mask'        , 0
DB  'Processor type'               , 0
DB  'Allocation granularity'       , 0
DB  'Processor level'              , 0
DB  'Processor revision'           , 0
DB  'Processors at current group'  , 0
DB  'Processors total'             , 0
DB  'Processor groups'             , 0
DB  'NUMA domains'                 , 0
DB  'Normal page size'             , 0
DB  'Minimum large page size'      , 0
DB  ' ( DISABLED )'                , 0
DB  ' ( ENABLED )'                 , 0
endres

;---------- Raw resource for binders pool -------------------------------------;
resdata bindersPool

;---------- GUI binder script for system information screen -------------------;
BIND_STRING  STR_FULL_SYSINFO     , IDC_SYSINFO   
BIND_STRING  STR_CPUID            , IDC_SYSINFO_CPUID  
BIND_STRING  STR_MMX              , IDC_SYSINFO_MMX 
BIND_STRING  STR_SSE              , IDC_SYSINFO_SSE 
BIND_STRING  STR_SSE2             , IDC_SYSINFO_SSE2 
BIND_STRING  STR_SSE3             , IDC_SYSINFO_SSE3 
BIND_STRING  STR_SSSE3            , IDC_SYSINFO_SSSE3 
BIND_STRING  STR_SSE41            , IDC_SYSINFO_SSE41 
BIND_STRING  STR_SSE42            , IDC_SYSINFO_SSE42 
BIND_STRING  STR_AVX              , IDC_SYSINFO_AVX 
BIND_STRING  STR_AVX2             , IDC_SYSINFO_AVX2 
BIND_STRING  STR_AVX512F          , IDC_SYSINFO_AVX512F 
BIND_STRING  STR_RDRAND           , IDC_SYSINFO_RDRAND 
BIND_STRING  STR_VMX              , IDC_SYSINFO_VMX_SVM 
BIND_STRING  STR_X8664            , IDC_SYSINFO_X8664 
BIND_STRING  STR_AVX512CD         , IDC_SYSINFO_A0 
BIND_STRING  STR_AVX512PF         , IDC_SYSINFO_A1
BIND_STRING  STR_AVX512ER         , IDC_SYSINFO_A2
BIND_STRING  STR_AVX512VL         , IDC_SYSINFO_A3
BIND_STRING  STR_AVX512BW         , IDC_SYSINFO_A4
BIND_STRING  STR_AVX512DQ         , IDC_SYSINFO_A5
BIND_STRING  STR_AVX512_IFMA      , IDC_SYSINFO_B0
BIND_STRING  STR_AVX512_VBMI      , IDC_SYSINFO_B1
BIND_STRING  STR_AVX512_VBMI2     , IDC_SYSINFO_B2
BIND_STRING  STR_AVX512_BF16      , IDC_SYSINFO_B3
BIND_STRING  STR_AVX512_VAES      , IDC_SYSINFO_B4
BIND_STRING  STR_AVX512_GFNI      , IDC_SYSINFO_B5
BIND_STRING  STR_AVX512_VNNI      , IDC_SYSINFO_C0
BIND_STRING  STR_AVX512_BITALG    , IDC_SYSINFO_C1
BIND_STRING  STR_AVX512_VPOPCNTDQ , IDC_SYSINFO_C2
BIND_STRING  STR_AVX512_VPCLMULQ  , IDC_SYSINFO_C3
BIND_STRING  STR_AVX512_VP2INTERS , IDC_SYSINFO_D0
BIND_STRING  STR_AVX512_FP16      , IDC_SYSINFO_D1
BIND_STRING  STR_AVX512_4FMAPS    , IDC_SYSINFO_D2
BIND_STRING  STR_AVX512_4VNNIW    , IDC_SYSINFO_D3
BIND_STRING  STR_XCR0             , IDC_SYSINFO_XCR0 
BIND_STRING  STR_XMM_0_15         , IDC_SYSINFO_XMM015  
BIND_STRING  STR_YMM_0_15         , IDC_SYSINFO_YMM015 
BIND_STRING  STR_ZMM_0_15         , IDC_SYSINFO_ZMM015 
BIND_STRING  STR_ZMM_16_31        , IDC_SYSINFO_ZMM1631 
BIND_STRING  STR_K_0_7            , IDC_SYSINFO_K07 
BIND_STRING  STR_BNDREG           , IDC_SYSINFO_BNDREG 
BIND_STRING  STR_BNDCSR           , IDC_SYSINFO_BNDCSR 
BIND_STRING  STR_ACPI             , IDC_SYSINFO_ACPI
BIND_STRING  STR_MADT             , IDC_SYSINFO_MADT
BIND_STRING  STR_SRAT             , IDC_SYSINFO_SRAT
BIND_STRING  STR_L1_CODE          , IDC_SYSINFO_L1C
BIND_STRING  STR_L1_DATA          , IDC_SYSINFO_L1D
BIND_STRING  STR_L2_UNIFIED       , IDC_SYSINFO_L2U
BIND_STRING  STR_L3_UNIFIED       , IDC_SYSINFO_L3U
BIND_STRING  STR_L4_UNIFIED       , IDC_SYSINFO_L4U
BIND_STRING  STR_THREADS          , IDC_SYSINFO_THREADS
BIND_STRING  STR_CORES            , IDC_SYSINFO_CORES
BIND_STRING  STR_SOCKETS          , IDC_SYSINFO_SOCKETS
BIND_STRING  STR_OS_PROCESSORS    , IDC_SYSINFO_OSPR
BIND_STRING  STR_OS_NUMA_NODES    , IDC_SYSINFO_OSNM
BIND_STRING  STR_OS_GROUPS        , IDC_SYSINFO_PGRP
BIND_STRING  STR_OS_PHYSICAL      , IDC_SYSINFO_MEM
BIND_STRING  STR_OS_AVAILABLE     , IDC_SYSINFO_MEM_A
BIND_STRING  STR_OS_MIN_LARGE     , IDC_SYSINFO_LRPG
BIND_STRING  STR_REPORT           , IDB_SYSINFO_REPORT
BIND_STRING  STR_EXIT             , IDB_SYSINFO_CANCEL
BIND_BIG     BUFFER_CPU_VENDOR    , IDC_SYSINFO_VENDOR
BIND_INFO    BUFFER_CPU_TFMS      , IDC_SYSINFO_TFMS
BIND_BIG     BUFFER_CPU_NAME      , IDC_SYSINFO_NAME
BIND_INFO    BUFFER_CPU_TSC       , IDC_SYSINFO_TSC      
;---------- CPU common features bitmap ----------------------------------------; 
BIND_BOOL    BUFFER_CPU_BITMAP + 7 , 7 , IDC_SYSINFO_CPUID
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 0 , IDC_SYSINFO_MMX
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 1 , IDC_SYSINFO_SSE
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 2 , IDC_SYSINFO_SSE2
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 3 , IDC_SYSINFO_SSE3
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 4 , IDC_SYSINFO_SSSE3
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 5 , IDC_SYSINFO_SSE41
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 6 , IDC_SYSINFO_SSE42
BIND_BOOL    BUFFER_CPU_BITMAP + 0 , 7 , IDC_SYSINFO_AVX
BIND_BOOL    BUFFER_CPU_BITMAP + 1 , 0 , IDC_SYSINFO_AVX2
BIND_BOOL    BUFFER_CPU_BITMAP + 1 , 1 , IDC_SYSINFO_AVX512F
BIND_BOOL    BUFFER_CPU_BITMAP + 1 , 2 , IDC_SYSINFO_RDRAND
BIND_BOOL    BUFFER_CPU_BITMAP + 1 , 3 , IDC_SYSINFO_VMX_SVM
BIND_BOOL    BUFFER_CPU_BITMAP + 1 , 5 , IDC_SYSINFO_X8664
;---------- CPU AVX512 features bitmap ----------------------------------------;
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 0 , IDC_SYSINFO_A0
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 1 , IDC_SYSINFO_A1
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 2 , IDC_SYSINFO_A2
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 3 , IDC_SYSINFO_A3
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 4 , IDC_SYSINFO_A4
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 5 , IDC_SYSINFO_A5
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 6 , IDC_SYSINFO_B0
BIND_BOOL    BUFFER_AVX512_BITMAP + 0 , 7 , IDC_SYSINFO_B1
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 0 , IDC_SYSINFO_B2
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 1 , IDC_SYSINFO_B3
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 2 , IDC_SYSINFO_B4
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 3 , IDC_SYSINFO_B5
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 4 , IDC_SYSINFO_C0
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 5 , IDC_SYSINFO_C1
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 6 , IDC_SYSINFO_C2
BIND_BOOL    BUFFER_AVX512_BITMAP + 1 , 7 , IDC_SYSINFO_C3
BIND_BOOL    BUFFER_AVX512_BITMAP + 2 , 0 , IDC_SYSINFO_D0
BIND_BOOL    BUFFER_AVX512_BITMAP + 2 , 1 , IDC_SYSINFO_D1
BIND_BOOL    BUFFER_AVX512_BITMAP + 2 , 2 , IDC_SYSINFO_D2
BIND_BOOL    BUFFER_AVX512_BITMAP + 2 , 3 , IDC_SYSINFO_D3
;---------- OS context management features bitmap -----------------------------;
BIND_BOOL    BUFFER_OS_BITMAP + 7 , 7 , IDC_SYSINFO_XCR0 
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 0 , IDC_SYSINFO_XMM015
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 1 , IDC_SYSINFO_YMM015
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 2 , IDC_SYSINFO_ZMM015
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 3 , IDC_SYSINFO_ZMM1631
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 4 , IDC_SYSINFO_K07
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 5 , IDC_SYSINFO_BNDREG
BIND_BOOL    BUFFER_OS_BITMAP + 0 , 6 , IDC_SYSINFO_BNDCSR
BIND_STOP

;---------- GUI binder script for memory and cache screen ( both ia32, x64 ) --;
BIND_STRING  STR_FULL_MEMORY      , IDC_MEMORY
BIND_STRING  STR_AM_06            , IDB_MEMORY_ASM_A6
BIND_STRING  STR_AM_07            , IDB_MEMORY_ASM_A7
BIND_STRING  STR_AM_08            , IDB_MEMORY_ASM_A8
BIND_STRING  STR_AM_09            , IDB_MEMORY_ASM_B0 
BIND_STRING  STR_AM_10            , IDB_MEMORY_ASM_B1 
BIND_STRING  STR_AM_11            , IDB_MEMORY_ASM_B2 
BIND_STRING  STR_AM_12            , IDB_MEMORY_ASM_B3 
BIND_STRING  STR_AM_13            , IDB_MEMORY_ASM_B4 
BIND_STRING  STR_AM_14            , IDB_MEMORY_ASM_B5 
BIND_STRING  STR_AM_15            , IDB_MEMORY_ASM_B6 
BIND_STRING  STR_AM_16            , IDB_MEMORY_ASM_B7 
BIND_STRING  STR_AM_17            , IDB_MEMORY_ASM_B8 
BIND_STRING  STR_AM_18            , IDB_MEMORY_ASM_C0
BIND_STRING  STR_AM_19            , IDB_MEMORY_ASM_C1
BIND_STRING  STR_AM_20            , IDB_MEMORY_ASM_C2
BIND_STRING  STR_AM_21            , IDB_MEMORY_ASM_C3
BIND_STRING  STR_AM_22            , IDB_MEMORY_ASM_C4
BIND_STRING  STR_AM_23            , IDB_MEMORY_ASM_C5
BIND_STRING  STR_AM_LCM           , IDB_MEMORY_ASM_C6
BIND_STRING  STR_AM_RDRAND        , IDB_MEMORY_ASM_C7
BIND_STRING  STR_NON_TEMPORAL     , IDB_MEMORY_NONTEMP
BIND_STRING  STR_32X2             , IDB_MEMORY_FORCE32 
BIND_STRING  STR_MEMORY_L1        , IDB_MEMORY_L1
BIND_STRING  STR_MEMORY_L2        , IDB_MEMORY_L2
BIND_STRING  STR_MEMORY_L3        , IDB_MEMORY_L3
BIND_STRING  STR_MEMORY_L4        , IDB_MEMORY_L4
BIND_STRING  STR_MEMORY_DRAM      , IDB_MEMORY_DRAM
BIND_STRING  STR_MEMORY_CUSTOM    , IDB_MEMORY_CUSTOM
BIND_STRING  STR_MEMORY_FILE      , IDB_MEMORY_FILE
BIND_STRING  STR_MEMORY_GPU       , IDB_MEMORY_GPU
BIND_STRING  STR_MEMORY_PHYSICAL  , IDB_MEMORY_PHYSICAL
BIND_STRING  STR_HYPHEN           , IDC_MEMORY_M_HYPHEN
BIND_STRING  STR_MEMORY_MTRR_WB   , IDB_MEMORY_MTRR_WB
BIND_STRING  STR_MEMORY_MTRR_WT   , IDB_MEMORY_MTRR_WT 
BIND_STRING  STR_MEMORY_MTRR_WC   , IDB_MEMORY_MTRR_WC 
BIND_STRING  STR_MEMORY_MTRR_WP   , IDB_MEMORY_MTRR_WP 
BIND_STRING  STR_MEMORY_MTRR_UC   , IDB_MEMORY_MTRR_UC 
BIND_STRING  STR_PARALLEL_THREADS , IDB_MEMORY_PARALLEL 
BIND_STRING  STR_HYPER_THREADING  , IDB_MEMORY_HT 
BIND_STRING  STR_PROCESSOR_GROUPS , IDB_MEMORY_PG
BIND_STRING  STR_NUMA_UNAWARE     , IDB_MEMORY_NUMA_U
BIND_STRING  STR_NUMA_SINGLE      , IDB_MEMORY_NUMA_S
BIND_STRING  STR_NUMA_OPTIMAL     , IDB_MEMORY_NUMA_O
BIND_STRING  STR_NUMA_NO_OPTIMAL  , IDB_MEMORY_NUMA_N
BIND_STRING  STR_NORMAL_ACCESS    , IDB_MEMORY_NORMAL
BIND_STRING  STR_ACCESS_64B       , IDB_MEMORY_SK_63 
BIND_STRING  STR_ACCESS_4096B     , IDB_MEMORY_SK_4095 
BIND_STRING  STR_ACCES_CUSTOM     , IDB_MEMORY_SK_CSTM 
BIND_STRING  STR_LARGE_PAGES      , IDB_MEMORY_LP
BIND_STRING  STR_NO_PREFETCH      , IDB_MEMORY_NO_PF
BIND_STRING  STR_DEFAULT_DISTANCE , IDB_MEMORY_DEF_PF
BIND_STRING  STR_MEDIUM_DISTANCE  , IDB_MEMORY_MED_PF
BIND_STRING  STR_LONG_DISTANCE    , IDB_MEMORY_LNG_PF
BIND_STRING  STR_BLOCK_PREFETCH   , IDB_MEMORY_BLK_PF
BIND_STRING  STR_CUSTOM_DISTANCE  , IDB_MEMORY_CST_PF
BIND_STRING  STR_MEASURE_BRIEF    , IDB_MEMORY_BRF  
BIND_STRING  STR_MEASURE_CAREF    , IDB_MEMORY_CRF 
BIND_STRING  STR_BRIEFF_ADAPTIVE  , IDB_MEMORY_BRF_A
BIND_STRING  STR_CAREF_ADAPTIVE   , IDB_MEMORY_CRF_A
BIND_STRING  STR_ALL_POINTS       , IDB_MEMORY_ALL_P
BIND_STRING  STR_X_16_POINTS      , IDB_MEMORY_X_16
BIND_STRING  STR_X_32_POINTS      , IDB_MEMORY_X_32
BIND_STRING  STR_3D_DRAW          , IDB_MEMORY_3D_DRAW 
BIND_STRING  STR_DRAW             , IDB_MEMORY_DRAW  
BIND_STRING  STR_RUN              , IDB_MEMORY_RUN
BIND_STRING  STR_DEFAULTS         , IDB_MEMORY_DEFAULTS
BIND_STRING  STR_EXIT             , IDB_MEMORY_CANCEL
BIND_STOP        

;---------- GUI binder script for memory and cache screen ( ia32 only ) -------;
BIND_STRING  STR_AM_IA32_00       , IDB_MEMORY_ASM_A0
BIND_STRING  STR_AM_IA32_01       , IDB_MEMORY_ASM_A1
BIND_STRING  STR_AM_IA32_02       , IDB_MEMORY_ASM_A2
BIND_STRING  STR_AM_IA32_03       , IDB_MEMORY_ASM_A3
BIND_STRING  STR_AM_IA32_04       , IDB_MEMORY_ASM_A4
BIND_STRING  STR_AM_IA32_05       , IDB_MEMORY_ASM_A5
BIND_STOP        

;---------- GUI binder script for memory and cache screen ( x64 only ) --------;
BIND_STRING  STR_AM_X64_00        , IDB_MEMORY_ASM_A0
BIND_STRING  STR_AM_X64_01        , IDB_MEMORY_ASM_A1
BIND_STRING  STR_AM_X64_02        , IDB_MEMORY_ASM_A2
BIND_STRING  STR_AM_X64_03        , IDB_MEMORY_ASM_A3
BIND_STRING  STR_AM_X64_04        , IDB_MEMORY_ASM_A4
BIND_STRING  STR_AM_X64_05        , IDB_MEMORY_ASM_A5
BIND_STOP        

;---------- GUI binder script for CPU mathematics screen ----------------------;
BIND_STRING  STR_FULL_MATH        , IDC_MATH
BIND_STRING  STR_DRAW             , IDB_MATH_DRAW  
BIND_STRING  STR_RUN              , IDB_MATH_RUN
BIND_STRING  STR_DEFAULTS         , IDB_MATH_DEFAULTS
BIND_STRING  STR_EXIT             , IDB_MATH_CANCEL
BIND_STOP

;---------- GUI binder script operating system for screen ---------------------;
BIND_STRING  STR_FULL_OS          , IDC_OS
BIND_FONT    ID_FONT_2            , IDE_OS_UP
BIND_FONT    ID_FONT_2            , IDE_OS_TEXT
BIND_STRING  STR_PARM_VALUE_HEX   , IDE_OS_UP
BIND_BIG     BUFFER_VIEWER_OS     , IDE_OS_TEXT
BIND_STRING  STR_REPORT           , IDB_OS_REPORT
BIND_STRING  STR_EXIT             , IDB_OS_CANCEL
BIND_STOP

;---------- GUI binder script for native operating system screen --------------;
BIND_STRING  STR_FULL_NATIVE_OS   , IDC_NATIVE_OS
BIND_STRING  STR_PARM_VALUE_HEX   , IDE_NATIVE_OS_UP
BIND_STRING  STR_REPORT           , IDB_NAT_OS_REPORT
BIND_STRING  STR_EXIT             , IDB_NAT_OS_CANCEL
BIND_STOP

;---------- GUI binder script for processor information screen ----------------;
BIND_STRING  STR_FULL_PROCESSOR   , IDC_PROCESSOR
BIND_STRING  STR_PARM_VALUE       , IDE_PROC_UP
BIND_STRING  STR_REPORT           , IDB_PROC_REPORT
BIND_STRING  STR_EXIT             , IDB_PROC_CANCEL
BIND_STOP

;---------- GUI binder script for topology screen -----------------------------;
BIND_STRING  STR_FULL_TOPOLOGY    , IDC_TOPOLOGY
BIND_STRING  STR_TOPOLOGY         , IDE_TOPOL_UP_1
BIND_STRING  STR_TOPOLOGY_SUMMARY , IDE_TOPOL_UP_2
BIND_STRING  STR_REPORT           , IDB_TOPOL_REPORT
BIND_STRING  STR_EXIT             , IDB_TOPOL_CANCEL
BIND_STOP

;---------- GUI binder script for extended topology screen --------------------;
BIND_STRING  STR_FULL_TOPOLOGY_EX , IDC_TOPOLOGY_EX
BIND_STRING  STR_TOPOLOGY         , IDE_TOP_EX_UP_1
BIND_STRING  STR_TOPOLOGY_SUMMARY , IDE_TOP_EX_UP_2
BIND_STRING  STR_REPORT           , IDB_TOPOL_EX_REPORT
BIND_STRING  STR_EXIT             , IDB_TOPOL_EX_CANCEL
BIND_STOP

;---------- GUI binder script for NUMA nodes list screen ----------------------;
BIND_STRING  STR_FULL_NUMA        , IDC_NUMA
BIND_STRING  STR_NUMA             , IDE_NUMA_UP
BIND_STRING  STR_REPORT           , IDB_NUMA_REPORT
BIND_STRING  STR_EXIT             , IDB_NUMA_CANCEL
BIND_STOP

;---------- GUI binder script for processor groups list screen ----------------;
BIND_STRING  STR_FULL_P_GROUPS    , IDC_P_GROUPS
BIND_STRING  STR_GROUPS           , IDE_P_GROUPS_UP
BIND_STRING  STR_REPORT           , IDB_P_GROUPS_REPORT
BIND_STRING  STR_EXIT             , IDB_P_GROUPS_CANCEL
BIND_STOP

;---------- GUI binder script for ACPI information screen ---------------------;
BIND_STRING  STR_FULL_ACPI        , IDC_ACPI
BIND_STRING  STR_ACPI_LIST        , IDE_ACPI_UP_1
BIND_STRING  STR_ACPI_SUMMARY     , IDE_ACPI_UP_2
BIND_STRING  STR_REPORT           , IDB_ACPI_REPORT
BIND_STRING  STR_EXIT             , IDB_ACPI_CANCEL
BIND_STOP

;---------- GUI binder script for affinized CPUID dump screen -----------------;
BIND_STRING  STR_FULL_AFF_CPUID   , IDC_AFF_CPUID
BIND_STRING  STR_AFF_CPUID        , IDE_A_CPUID_UP
BIND_STRING  STR_REPORT           , IDB_A_CPUID_REPORT
BIND_STRING  STR_EXIT             , IDB_A_CPUID_CANCEL
BIND_STOP

;---------- GUI binder script for kernel mode driver operations screen --------;
BIND_STRING  STR_FULL_KMD         , IDC_KMD
BIND_STRING  STR_PARM_VALUE       , IDE_KMD_UP_1
BIND_STRING  STR_KMD              , IDE_KMD_UP_2
BIND_STRING  STR_REPORT           , IDB_KMD_REPORT
BIND_STRING  STR_EXIT             , IDB_KMD_CANCEL
BIND_STOP
endres

;---------- CPU common features bitmap builder script -------------------------;
resdata cpuCommonFeatures
ENTRY_CPUID    00000001h             , R_EDX , 23   ; MMX
ENTRY_CPUID    00000001h             , R_EDX , 25   ; SSE  
ENTRY_CPUID    00000001h             , R_EDX , 26   ; SSE2
ENTRY_CPUID    00000001h             , R_ECX , 01   ; SSE3
ENTRY_CPUID    00000001h             , R_ECX , 09   ; SSSE3
ENTRY_CPUID    00000001h             , R_ECX , 19   ; SSE4.1
ENTRY_CPUID    00000001h             , R_ECX , 20   ; SSE4.2
ENTRY_CPUID    00000001h             , R_ECX , 28   ; AVX
ENTRY_CPUID    00000007h             , R_EBX , 05   ; AVX2
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 16   ; AVX512F
ENTRY_CPUID    00000001h             , R_ECX , 30   ; RDRAND
ENTRY_CPUID    00000001h             , R_ECX , 05   ; VMX
ENTRY_CPUID    80000001h             , R_ECX , 02   ; SVM
ENTRY_CPUID    80000001h             , R_EDX , 29   ; x86-64
ENTRY_STOP
endres  

;---------- CPU AVX512 features bitmap builder script -------------------------;
resdata cpuAvx512Features
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 28   ; AVX512CD
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 26   ; AVX512PF
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 27   ; AVX512ER
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 31   ; AVX512VL
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 30   ; AVX512BW
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 17   ; AVX512DQ
ENTRY_CPUID_S  00000007h , 00000000h , R_EBX , 21   ; AVX512_IFMA
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 01   ; AVX512_VBMI
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 06   ; AVX512_VBMI2
ENTRY_CPUID_S  00000007h , 00000000h , R_EAX , 05   ; AVX512_BF16
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 09   ; AVX512+VAES
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 08   ; AVX512+GFNI
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 11   ; AVX512_VNNI
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 12   ; AVX512_BITALG
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 14   ; AVX512_VPOPCNTDQ
ENTRY_CPUID_S  00000007h , 00000000h , R_ECX , 10   ; AVX512+VPCLMULQDQ
ENTRY_CPUID_S  00000007h , 00000000h , R_EDX , 08   ; AVX512_VP2INTERSECT
ENTRY_CPUID_S  00000007h , 00000000h , R_EDX , 23   ; AVX512_FP16
ENTRY_CPUID_S  00000007h , 00000000h , R_EDX , 03   ; AVX512_4FMAPS
ENTRY_CPUID_S  00000007h , 00000000h , R_EDX , 02   ; AVX512_4VNNIW
ENTRY_STOP
endres  

;---------- OS context features bitmap builder script -------------------------;
resdata osContextFeatures
ENTRY_XCR0     01   ; XMM[0-15]  
ENTRY_XCR0     02   ; YMM[0-15] 
ENTRY_XCR0     06   ; ZMM[0-15]
ENTRY_XCR0     07   ; ZMM[16-31]
ENTRY_XCR0     05   ; K[0-7]
ENTRY_XCR0     03   ; BNDREGS
ENTRY_XCR0     04   ; BNDCSR
ENTRY_STOP
endres  

;---------- CPU instruction-based methods bitmap builder script ---------------;
resdata cpuMethodFeatures
; TODO.
endres  

;---------- Raw resource for dynamical imported functions list ----------------;
resdata importList
DB  'GlobalMemoryStatusEx'         , 0      ; This functions from KERNEL32.DLL          
DB  'GetActiveProcessorGroupCount' , 0    
DB  'GetActiveProcessorCount'      , 0              
DB  'GetNumaHighestNodeNumber'     , 0            
DB  'GetLargePageMinimum'          , 0                      
DB  'EnumSystemFirmwareTables'     , 0      
DB  'GetSystemFirmwareTable'       , 0 , 0  ; Two zeroes means end of sub-list        
DB  'OpenProcessToken'             , 0      ; This functions from ADVAPI32.DLL              
DB  'AdjustTokenPrivileges'        , 0 , 0  ; Two zeroes means end of sub-list                  
endres

;---------- Raw resource for dynamical created fonts list ---------------------;
resdata fontList
; cHeight, cWidth, cWeight, iCharset, iOutPrecision,
; iClipPrecision, iQuality, iPitchAndFamily  
DW  17 , 10 , FW_DONTCARE , DEFAULT_CHARSET
DW  OUT_TT_ONLY_PRECIS  , CLIP_DEFAULT_PRECIS , CLEARTYPE_QUALITY , FIXED_PITCH
DB  'Verdana' , 0
DW  16 , 40 , FW_DONTCARE , DEFAULT_CHARSET
DW  OUT_TT_ONLY_PRECIS  , CLIP_DEFAULT_PRECIS , CLEARTYPE_QUALITY , FIXED_PITCH
DB  'System monospace' , 0
DW  0
endres

;---------- ACPI tables data base ---------------------------------------------; 
resdata acpiData
DB  'AEST' , 'Arm Error Source'                                 , 0
DB  'APIC' , 'Multiple APIC Description'                        , 0
DB  'BDAT' , 'BIOS Data ACPI'                                   , 0
DB  'BERT' , 'Boot Error Record'                                , 0
DB  'BGRT' , 'Boot Graphics Resource'                           , 0
DB  'BOOT' , 'Simple Boot Flag'                                 , 0
DB  'CDIT' , 'Component Distance Information'                   , 0
DB  'CEDT' , 'CXL Early Discovery'                              , 0
DB  'CPEP' , 'Corrected Platform Error Polling'                 , 0
DB  'CRAT' , 'Component Resource Attribute'                     , 0
DB  'CSRT' , 'Core System Resource'                             , 0
DB  'DBGP' , 'Debug Port'                                       , 0
DB  'DMAR' , 'DMA Remapping'                                    , 0
DB  'DSDT' , 'Differentiated System Description'                , 0
DB  'DPPT' , 'DMA Protection Policy'                            , 0
DB  'DRTM' , 'Dynamic Root of Trust for Measurement'            , 0
DB  'ECDT' , 'Embedded Controller Boot Resources'               , 0
DB  'EINJ' , 'Error Injection'                                  , 0
DB  'ERST' , 'Error Record Serialization'                       , 0
DB  'ETDT' , 'Event Timer Description'                          , 0
DB  'FACP' , 'Fixed ACPI Description'                           , 0
DB  'FACS' , 'Firmware ACPI Control Structure'                  , 0
DB  'FPDT' , 'Firmware Performance Data'                        , 0
DB  'GTDT' , 'Generic Timer Description'                        , 0
DB  'HEST' , 'Hardware Error Source'                            , 0
DB  'HPET' , 'High Precision Event Timer'                       , 0
DB  'IBFT' , 'iSCSI Boot Firmware'                              , 0
DB  'IORT' , 'I/O Remapping'                                    , 0
DB  'IVRS' , 'I/O Virtualization Reporting'                     , 0
DB  'LPIT' , 'Low Power Idle'                                   , 0
DB  'MCFG' , 'Memory Mapped Configuration'                      , 0
DB  'MCHI' , 'Management Controller Host Interface'             , 0
DB  'MPAM' , 'Arm Memory Partitioning and Monitoring'           , 0
DB  'MSDM' , 'Microsoft Data Management'                        , 0
DB  'MSCT' , 'Maximum System Characteristics'                   , 0
DB  'MPST' , 'Memory Power State'                               , 0
DB  'NFIT' , 'NVDIMM Firmware Interface'                        , 0
DB  'OEMx' , 'OEM Specific Information'                         , 0
DB  'PHAT' , 'Platform Health Assessment'                       , 0
DB  'PCCT' , 'Platform Communications Channel'                  , 0
DB  'PMTT' , 'Platform Memory Topology'                         , 0
DB  'PSDT' , 'Persistent System Description'                    , 0
DB  'PRMT' , 'Platform Runtime Mechanism Table'                 , 0
DB  'RASF' , 'ACPI RAS Feature'                                 , 0
DB  'RGRT' , 'Regulatory Graphics Resource Table'               , 0
DB  'RSDT' , 'Root System Description'                          , 0
DB  'SBST' , 'Smart Battery Specification'                      , 0
DB  'SDEI' , 'Software Delegated Exceptions Interface'          , 0
DB  'SDEV' , 'Secure Devices Table'                             , 0
DB  'SLIC' , 'Microsoft Software Licensing'                     , 0
DB  'SLIT' , 'System Locality Distance Information'             , 0
DB  'SRAT' , 'Static/System Resource Affinity'                  , 0
DB  'SSDT' , 'Secondary System Description'                     , 0
DB  'SPCR' , 'Serial Port Console Redirection'                  , 0
DB  'SPMI' , 'Server Platform Management Interface'             , 0
DB  'STAO' , '_STA Override'                                    , 0
DB  'SVKL' , 'Storage Volume Key Data'                          , 0
DB  'TCPA' , 'Trusted Computing Platform Alliance Capabilities' , 0
DB  'TPM2' , 'Trusted Platform Module 2'                        , 0
DB  'UEFI' , 'Unified Extensible Firmware Interface'            , 0
DB  'WAET' , 'Windows ACPI Emulated Devices'                    , 0
DB  'WDAT' , 'Watch Dog Action Table'                           , 0
DB  'WDRT' , 'Watch Dog Resource Table'                         , 0
DB  'WPBT' , 'Windows Platform Binary'                          , 0
DB  'WSMT' , 'Windows Security Mitigations'                     , 0
DB  'XENV' , 'Xen Project'                                      , 0
DB  'XSDT' , 'Extended System Description'                      , 0
DB  0
endres  

;---------- Directory of icon resources ---------------------------------------; 
resource icons, \
IDI_SYSINFO     , LANG_NEUTRAL , iSysinfo    , \
IDI_MEMORY      , LANG_NEUTRAL , iMemory     , \
IDI_MATH        , LANG_NEUTRAL , iMath       , \
IDI_OS          , LANG_NEUTRAL , iOs         , \
IDI_NATIVE_OS   , LANG_NEUTRAL , iNativeOs   , \
IDI_PROCESSOR   , LANG_NEUTRAL , iProcessor  , \
IDI_TOPOLOGY    , LANG_NEUTRAL , iTopology   , \
IDI_TOPOLOGY_EX , LANG_NEUTRAL , iTopologyEx , \
IDI_NUMA        , LANG_NEUTRAL , iNuma       , \
IDI_P_GROUPS    , LANG_NEUTRAL , iPgroups    , \
IDI_ACPI        , LANG_NEUTRAL , iAcpi       , \
IDI_AFF_CPUID   , LANG_NEUTRAL , iAffCpuid   , \
IDI_KMD         , LANG_NEUTRAL , iKmd
;---------- Directory of group icon resources ---------------------------------;
resource gicons, \
IDG_SYSINFO     , LANG_NEUTRAL , gSysinfo    , \
IDG_MEMORY      , LANG_NEUTRAL , gMemory     , \
IDG_MATH        , LANG_NEUTRAL , gMath       , \
IDG_OS          , LANG_NEUTRAL , gOs         , \
IDG_NATIVE_OS   , LANG_NEUTRAL , gNativeOs   , \
IDG_PROCESSOR   , LANG_NEUTRAL , gProcessor  , \
IDG_TOPOLOGY    , LANG_NEUTRAL , gTopology   , \
IDG_TOPOLOGY_EX , LANG_NEUTRAL , gTopologyEx , \
IDG_NUMA        , LANG_NEUTRAL , gNuma       , \
IDG_P_GROUPS    , LANG_NEUTRAL , gPgroups    , \
IDG_ACPI        , LANG_NEUTRAL , gAcpi       , \
IDG_AFF_CPUID   , LANG_NEUTRAL , gAffCpuid   , \
IDG_KMD         , LANG_NEUTRAL , gKmd
;---------- Icon resources ----------------------------------------------------;
icon iSysinfo    , gSysinfo    , 'images\sysinfo.ico'
icon iMath       , gMath       , 'images\math.ico'
icon iMemory     , gMemory     , 'images\memory.ico'
icon iOs         , gOs         , 'images\os.ico'
icon iNativeOs   , gNativeOs   , 'images\nativeos.ico'
icon iProcessor  , gProcessor  , 'images\processor.ico'
icon iTopology   , gTopology   , 'images\topology.ico'
icon iTopologyEx , gTopologyEx , 'images\topologyex.ico'
icon iNuma       , gNuma       , 'images\numa.ico'
icon iPgroups    , gPgroups    , 'images\pgroups.ico'
icon iAcpi       , gAcpi       , 'images\acpi.ico'
icon iAffCpuid   , gAffCpuid   , 'images\affcpuid.ico'
icon iKmd        , gKmd        , 'images\kmd.ico'