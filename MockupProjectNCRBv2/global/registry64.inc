;==============================================================================;
;                                                                              ;
;         Registry of global variables and data blocks, for x64 code.          ;
;                                                                              ;
;==============================================================================;

;---------- Definitions -------------------------------------------------------;
REGISTRY64_MEMORY_SIZE     = 1024 * 1024
TEMP_BUFFER_INIT_SIZE      = 48 * 1024
;---------- Allocator for data block with variable base address and size ------;
struct ALLOCATOR
objectStart                dq  ?
objectStop                 dq  ?
ends
;---------- Dynamical imported WinAPI functions pointers list -----------------;
struct DYNAIMPORT
_EnumSystemFirmwareTables  dq  ?
_GetSystemFirmwareTable    dq  ?
ends
;---------- Key data for GUI application with resources -----------------------;  
struct APPDATA
hAdvapi32                  dq  ?                   ; ADVAPI32.DLL handle
hResources                 dq  ?                   ; Resource DLL handle
lockedStrings              dq  ?                   ; Pointer to strings pool
lockedBinders              dq  ?                   ; Pointer to binders pool
hIcon                      dq  ?                   ; Application icon handle
hMain                      dq  ?                   ; Main window handle
hTab                       dq  ?                   ; Sheets container handle
hImageList                 dq  ?                   ; Image list handle
selectedTab                dd  ?                   ; Current sheet number
tabCtrlItem                TC_ITEM ?               ; Tab item data structure
lockedIcons                dq  ICON_COUNT dup (?)  ; Pointers to icons
hTabDlg                    dq  ITEM_COUNT dup (?)  ; Sheets handles
hInstance                  dq  ?                   ; This EXE file handle
ends

;---------- Root structure: NCRB64 application registry -----------------------; 
struct REGISTRY64
allocatorTempBuffer        ALLOCATOR   ?
appData                    APPDATA     ?
dynaImport                 DYNAIMPORT  ?
listNuma                   ALLOCATOR   ?
listGroups                 ALLOCATOR   ?
ends

