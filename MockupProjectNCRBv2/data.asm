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
; User mode debug by OllyDbg ( 32-bit, actual for other module NCRB32.EXE )           ;
; http://www.ollydbg.de/version2.html                                                 ;
;                                                                                     ;
; User mode debug by FDBG ( 64-bit, actual for this module NCRB64.EXE )               ;
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
dialog      mainDialog,        '',          0,   0, 410, 282, DS_CENTER + WS_CAPTION + WS_SYSMENU, 0, IDR_MENU, 'Verdana', 10
dialogitem  'SysTabControl32', '', IDC_TAB, 1,   1, 408,  29, WS_VISIBLE + TCS_MULTILINE   ; + TCS_FORCELABELLEFT ;+ TCS_FIXEDWIDTH
enddialog
;---------- Tab 1 = system information ----------------------------------------;
dialog      tabSysinfo    , '',                        2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_SYSINFO         ,  0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 2 = memory and cache benchmark --------------------------------; 
dialog      tabMemory     , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_MEMORY          ,   0,   3, 250,  10, WS_VISIBLE
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
dialogitem  'STATIC'      , '', IDC_MATH            ,   0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 4 = operating system info -------------------------------------; 
dialog      tabOs         , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_OS              ,   0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 5 = native os info, useable if run ncrb32 under win64 ---------;
dialog      tabNativeOs   , '',                         2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_NATIVE_OS       ,   0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 6 = processor info --------------------------------------------; 
dialog      tabProcessor  , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_PROCESSOR  , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 7 = platform topology info by winapi --------------------------; 
dialog      tabTopology   , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_TOPOLOGY   , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 8 = platform topology info by winapi (ex) ---------------------; 
dialog      tabTopologyEx , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_TOPOLOGY_EX , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 9 = platform NUMA domains list- -------------------------------; 
dialog      tabNuma       , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_NUMA       , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 10 = platform processor groups list ---------------------------; 
dialog      tabPgroups    , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_P_GROUPS   , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 11 = ACPI tables list -----------------------------------------; 
dialog      tabAcpi       , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_ACPI       , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 12 = affinized CPUID dump, per each logical CPU ---------------; 
dialog      tabAffCpuid   , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_AFF_CPUID  , 0,   3, 250,  10, WS_VISIBLE
enddialog                                   
;---------- Tab 13 = kernel mode driver information and probe results ---------; 
dialog      tabKmd        , '',                  2,  30, 403, 253, WS_CHILD + WS_VISIBLE, 0, 0, 'Verdana', 10
dialogitem  'STATIC'      , '', IDC_KMD        , 0,   3, 250,  10, WS_VISIBLE
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
IDS_STRINGS_POOL, LANG_ENGLISH + SUBLANG_DEFAULT, stringsPool, \
IDS_BINDERS_POOL, LANG_ENGLISH + SUBLANG_DEFAULT, bindersPool 
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
DB  'System summary.'                                                      , 0
DB  'Memory and cache benchmarks, bandwidth (MBPS) and latency (ns).'      , 0
DB  'Processor mathematics and load-store operations benchmarks.'          , 0
DB  'System information by WinAPI.'                                        , 0
DB  'Native OS information for ia32 application under x64 OS.'             , 0
DB  'Processor information by CPUID, XGETBV and RDTSC instructions.'       , 0
DB  'Platform topology by WinAPI GetLogicalProcessorInformation().'        , 0
DB  'Platform topology by WinAPI GetLogicalProcessorInformationEx().'      , 0
DB  'NUMA domains by WinAPI GetNumaHighestNodeNumber() and other.'         , 0
DB  'Processor groups by WinAPI GetActiveProcessorGroupCount() and other.' , 0
DB  'ACPI tables list by WinAPI EnumSystemFirmwareTables() and other.'     , 0
DB  'CPUID per each thread affinized by WinAPI SetThreadAffinityMask().'   , 0
DB  'Kernel mode driver load status and privileged resources info.'        , 0
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
DB  'Latency (LCM)',0  
DB  'Latency (RDRAND)',0
DB  'Nontemporal',0
DB  'Force 32x2',0
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
DB  'Kernel mode'  , 0
DB  'True clock'   , 0
endres

;---------- Raw resource for binders pool -------------------------------------;
resdata bindersPool
;---------- GUI binder script for system information screen -------------------;
BIND_STRING  STR_FULL_SYSINFO     , IDC_SYSINFO   
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
BIND_STRING  STR_LCM              , IDB_MEMORY_ASM_C6
BIND_STRING  STR_RDRAND           , IDB_MEMORY_ASM_C7
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
BIND_STOP
;---------- GUI binder script operating system for screen ---------------------;
BIND_STRING  STR_FULL_OS          , IDC_OS
BIND_STOP
;---------- GUI binder script for native operating system screen --------------;
BIND_STRING  STR_FULL_NATIVE_OS   , IDC_NATIVE_OS
BIND_STOP
;---------- GUI binder script for processor information screen ----------------;
BIND_STRING  STR_FULL_PROCESSOR   , IDC_PROCESSOR
BIND_STOP
;---------- GUI binder script for topology screen -----------------------------;
BIND_STRING  STR_FULL_TOPOLOGY    , IDC_TOPOLOGY
BIND_STOP
;---------- GUI binder script for extended topology screen --------------------;
BIND_STRING  STR_FULL_TOPOLOGY_EX , IDC_TOPOLOGY_EX 
BIND_STOP
;---------- GUI binder script for NUMA nodes list screen ----------------------;
BIND_STRING  STR_FULL_NUMA        , IDC_NUMA
BIND_STOP
;---------- GUI binder script for processor groups list screen ----------------;
BIND_STRING  STR_FULL_P_GROUPS    , IDC_P_GROUPS
BIND_STOP
;---------- GUI binder script for ACPI information screen ---------------------;
BIND_STRING  STR_FULL_ACPI        , IDC_ACPI
BIND_STOP
;---------- GUI binder script for affinized CPUID dump screen -----------------;
BIND_STRING  STR_FULL_AFF_CPUID   , IDC_AFF_CPUID
BIND_STOP
;---------- GUI binder script for kernel mode driver operations screen --------;
BIND_STRING  STR_FULL_KMD         , IDC_KMD
BIND_STOP
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
