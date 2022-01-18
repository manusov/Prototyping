;------------------------------------------------------------------------------;
;                         Console support library:                             ; 
;     globally used data constants and variables with pre-defined state.       ; 
;------------------------------------------------------------------------------;

; Strings for console output

StartMsg           DB  0Dh, 0Ah, 'Starting...', 0Dh, 0Ah, 0
DoneMsgWait        DB  'Done, press ENTER...', 0
DoneMsgNoWait      DB  'Done.', 0
TitleString        DB  'Hardware Shell v0.11 (ia32)', 0
CrLf2              DB  0Dh, 0Ah
CrLf               DB  0Dh, 0Ah, 0

; Names for scenario file and report file

InputName          DB  'input.txt',0
OutputName         DB  'output.txt',0

; Error messages
MsgError           DB  'ERROR: ', 0
MsgErrorTab        DB  '       ', 0
MsgErrorOS         DB  'OS error ', 0
MsgUnknownOption   DB  'Unknown option.', 0 
MsgOption          DB  'Bad option string.', 0
MsgInputSize       DB  'Input scenario file size too big.', 0 


; Options descriptors, for values, controlled by scenario file INPUT.TXT 

OpDesc:
OPTION_KEYS        NameDisplay     , OptionDisplay     , WordDisplay     , KeyOnOff
OPTION_KEYS        NameReport      , OptionReport      , WordReport      , KeyOnOff
OPTION_KEYS        NameWaitkey     , OptionWaitkey     , WordWaitkey     , KeyOnOff
OPTION_KEYS        NameMeasure     , OptionMeasure     , WordMeasure     , KeyMeasure
OPTION_KEYS        NameFilemode    , OptionFilemode    , WordFilemode    , KeyMode
OPTION_DECIMAL_32  NameFilecount   , OptionFilecount   , WordFilecount 
OPTION_SIZE_64     NameFilesize    , OptionFilesize    , WordFilesize
OPTION_SIZE_64     NameBlocksize   , OptionBlocksize   , WordBlocksize
OPTION_DECIMAL_32  NameFilerepeats , OptionFilerepeats , WordFilerepeats
OPTION_KEYS        NameFiledata    , OptionFiledata    , WordFiledata    , KeyData
OPTION_KEYS        NameReadsync    , OptionReadsync    , WordReadsync    , KeyOnOff
OPTION_KEYS        NameWritesync   , OptionWritesync   , WordWritesync   , KeyOnOff
OPTION_KEYS        NameCopysync    , OptionCopysync    , WordCopysync    , KeyOnOff
OPTION_DECIMAL_32  NameReaddelay   , OptionReaddelay   , WordReaddelay
OPTION_DECIMAL_32  NameWritedelay  , OptionWritedelay  , WordWritedelay
OPTION_DECIMAL_32  NameCopydelay   , OptionCopydelay   , WordCopydelay
OPTION_KEYS        NameFilehint    , OptionFilehint    , WordFilehint    , KeyHint
OPTION_DECIMAL_32  NameReadmixed   , OptionReadmixed   , WordReadmixed
OPTION_DECIMAL_32  NameWritemixed  , OptionWritemixed  , WordWritemixed
OPTION_KEYS        NameNumaaware   , OptionNumaaware   , WordNumaaware   , KeyOnOff
OPTION_DECIMAL_32  NameNumanode    , OptionNumanode    , WordNumanode
OPTION_HEX_64      NameAffinity    , OptionAffinity    , WordAffinity
OPTION_STRING      NameSrcfile     , OptionSrcfile     , WordSrcfile
OPTION_STRING      NameDstfile     , OptionDstfile     , WordDstfile
OPTION_END

; Options values, controlled by scenario file INPUT.TXT
; Located at constants part, because this variables has pre-defined values

OptionDisplay      DB  1              ; on = console output enabled, off = disabled
OptionReport       DB  1              ; on = save report to file output.txt enabled, off = disabled
OptionWaitkey      DB  1              ; on = wait "Press ENTER" after operation, off = skip this waiting
OptionMeasure      DB  0              ; measurement scenario, variants = mbps (bandwidth), iops (performance)
OptionFilemode     DB  0              ; file mode, variants = group (group of files), single (repeat for one file)
OptionFilecount    DD  10             ; file count per group (group mode) or iterations count (single mode)
OptionFilesize     DQ  100*1024*1024  ; one file size
OptionBlocksize    DQ  10*1024*1024   ; one block size, this is data size per one OS API function call
OptionFilerepeats  DD  1              ; repeat read, write, copy operation inside one file
OptionFiledata     DB  1              ; data source, none, zeroes, ones, srng, hrng (software and hardware random number generator)
OptionReadsync     DB  1              ; on = read synchronization, disable OS read caching, off = no control (caching enabled)
OptionWritesync    DB  1              ; on = write synchronization, disable OS write caching, off = no control (caching enabled)
OptionCopysync     DB  1              ; on = copy synchronization, disable OS copy caching, off = no control (caching enabled)
OptionReaddelay    DD  0              ; delay time before read phase, milliseconds, 0 = no delay
OptionWritedelay   DD  0              ; delay time before write phase, milliseconds, 0 = no delay
OptionCopydelay    DD  0              ; delay time before copy phase, milliseconds, 0 = no delay
OptionFilehint     DB  0              ; access type hints for file operations OS API, variants = none, sequental, random
OptionReadmixed    DD  70             ; part of read operations per mixed group, io requests count = readmixed + writemixed
OptionWritemixed   DD  30             ; part of write operations per mixed group, io requests count = readmixed + writemixed
OptionNumaaware    DB  0              ; on = use NUMA-aware node selection, off = not use
OptionNumanode     DD  0              ; NUMA node number, used only if numaaware = on
OptionAffinity     DQ  0              ; CPU affinity mask, hex value, 0 = not affinize
OptionSrcfile      DD  BufferSrc      ; source file path, used for read and write
OptionDstfile      DD  BufferDst      ; destination file path, used for copy as destination

DefaultSrc         DB  'src.bin',0   ; default path for source files
DefaultDst         DB  'dst.bin',0   ; default path for destination files

; Long names for options, used for display and save report with parameters list

NameDisplay        DB  'Display console messages'    , 0  
NameReport         DB  'Generate report file'        , 0
NameWaitkey        DB  'Wait key press from user'    , 0
NameMeasure        DB  'Measurement scenario'        , 0
NameFilemode       DB  'File creation mode'          , 0
NameFilecount      DB  'File count'                  , 0
NameFilesize       DB  'File size'                   , 0
NameBlocksize      DB  'File I/O block size'         , 0
NameFilerepeats    DB  'File I/O repeats'            , 0
NameFiledata       DB  'File data generation'        , 0
NameReadsync       DB  'Read synchronization'        , 0
NameWritesync      DB  'Write synchronization'       , 0
NameCopysync       DB  'Copy synchronization'        , 0
NameReaddelay      DB  'Delay before read, ms'       , 0
NameWritedelay     DB  'Delay before write, ms'      , 0
NameCopydelay      DB  'Delay before copy, ms'       , 0
NameFilehint       DB  'OS API optimization hints'   , 0
NameReadmixed      DB  'Mixed read part'             , 0    
NameWritemixed     DB  'Mixed write part'            , 0
NameNumaaware      DB  'NUMA aware'                  , 0
NameNumanode       DB  'NUMA node selected'          , 0
NameAffinity       DB  'CPU affinity mask (0=None)'  , 0
NameSrcfile        DB  'Source file path'            , 0
NameDstfile        DB  'Destination file path'       , 0

; Short single word names for options, used for parsing

WordDisplay        DB  'display'      , 0
WordReport         DB  'report'       , 0
WordWaitkey        DB  'waitkey'      , 0
WordMeasure        DB  'measure'      , 0
WordFilemode       DB  'filemode'     , 0
WordFilecount      DB  'filecount'    , 0
WordFilesize       DB  'filesize'     , 0
WordBlocksize      DB  'blocksize'    , 0
WordFilerepeats    DB  'filerepeats'  , 0
WordFiledata       DB  'filedata'     , 0
WordReadsync       DB  'readsync'     , 0
WordWritesync      DB  'writesync'    , 0
WordCopysync       DB  'copysync'     , 0
WordReaddelay      DB  'readdelay'    , 0
WordWritedelay     DB  'writedelay'   , 0
WordCopydelay      DB  'copydelay'    , 0
WordFilehint       DB  'filehint'     , 0
WordReadmixed      DB  'readmixed'    , 0
WordWritemixed     DB  'writemixed'   , 0
WordNumaaware      DB  'numaaware'    , 0
WordNumanode       DB  'numanode'     , 0
WordAffinity       DB  'affinity'     , 0
WordSrcfile        DB  'srcfile'      , 0
WordDstfile        DB  'dstfile'      , 0

; Keywords for options

KeyOnOff           DB  'off', 0, 'on', 0, 0
KeyMeasure         DB  'mbps', 0, 'iops', 0, 0
KeyMode            DB  'group', 0, 'single', 0, 0
KeyData            DB  'none', 0, 'zeroes', 0, 'ones', 0, 'softrng', 0, 'hardrng', 0, 0
KeyHint            DB  'none', 0, 'sequental', 0, 'random', 0, 0
   

