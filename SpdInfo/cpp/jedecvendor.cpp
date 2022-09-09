#include <windows.h>
#include "jedecvendor.h"

const char* JedecGroup1[] =
{
   "AMD"                                              , // 1/1
   "AMI"                                              , // 1/2
   "Fairchild"                                        , // 1/3
   "Fujitsu"                                          , // 1/4
   "GTE"                                              , // 1/5
   "Harris"                                           , // 1/6
   "Hitachi"                                          , // 1/7
   "Inmos"                                            , // 1/8
   "Intel"                                            , // 1/9
   "I.T.T."                                           , // 1/10
   "Intersil"                                         , // 1/11
   "Monolithic Memories"                              , // 1/12
   "Mostek"                                           , // 1/13
   "Freescale (Motorola)"                             , // 1/14
   "National"                                         , // 1/15
   "NEC"                                              , // 1/16
   "RCA"                                              , // 1/17
   "Raytheon"                                         , // 1/18
   "Conexant (Rockwell)"                              , // 1/19
   "Seeq"                                             , // 1/20
   "NXP (Philips)"                                    , // 1/21
   "Synertek"                                         , // 1/22
   "Texas Instruments"                                , // 1/23
   "Kioxia Corporation"                               , // 1/24
   "Xicor"                                            , // 1/25
   "Zilog"                                            , // 1/26
   "Eurotechnique"                                    , // 1/27
   "Mitsubishi"                                       , // 1/28
   "Lucent (AT&T)"                                    , // 1/29
   "Exel"                                             , // 1/30
   "Atmel"                                            , // 1/31
   "STMicroelectronics"                               , // 1/32
   "Lattice Semi."                                    , // 1/33
   "NCR"                                              , // 1/34
   "Wafer Scale Integration"                          , // 1/35
   "IBM"                                              , // 1/36
   "Tristar"                                          , // 1/37
   "Visic"                                            , // 1/38
   "Intl. CMOS Technology"                            , // 1/39
   "SSSI"                                             , // 1/40
   "Microchip Technology"                             , // 1/41
   "Ricoh Ltd"                                        , // 1/42
   "VLSI"                                             , // 1/43
   "Micron Technology"                                , // 1/44
   "SK Hynix"                                         , // 1/45
   "OKI Semiconductor"                                , // 1/46
   "ACTEL"                                            , // 1/47
   "Sharp"                                            , // 1/48
   "Catalyst"                                         , // 1/49
   "Panasonic"                                        , // 1/50
   "IDT"                                              , // 1/51
   "Cypress"                                          , // 1/52
   "DEC"                                              , // 1/53
   "LSI Logic"                                        , // 1/54
   "Zarlink (Plessey)"                                , // 1/55
   "UTMC"                                             , // 1/56
   "Thinking Machine"                                 , // 1/57
   "Thomson CSF"                                      , // 1/58
   "Integrated CMOS (Vertex)"                         , // 1/59
   "Honeywell"                                        , // 1/60
   "Tektronix"                                        , // 1/61
   "Oracle Corporation"                               , // 1/62
   "Silicon Storage Technology"                       , // 1/63
   "ProMos/Mosel Vitelic"                             , // 1/64
   "Infineon (Siemens)"                               , // 1/65
   "Macronix"                                         , // 1/66
   "Xerox"                                            , // 1/67
   "Plus Logic"                                       , // 1/68
   "Western Digital Technologies Inc"                 , // 1/69
   "Elan Circuit Tech."                               , // 1/70
   "European Silicon Str."                            , // 1/71
   "Apple Computer"                                   , // 1/72
   "Xilinx"                                           , // 1/73
   "Compaq"                                           , // 1/74
   "Protocol Engines"                                 , // 1/75
   "SCI"                                              , // 1/76
   "Seiko Instruments"                                , // 1/77
   "Samsung"                                          , // 1/78
   "I3 Design System"                                 , // 1/79
   "Klic"                                             , // 1/80
   "Crosspoint Solutions"                             , // 1/81
   "Alliance Memory Inc"                              , // 1/82
   "Tandem"                                           , // 1/83
   "Hewlett-Packard"                                  , // 1/84
   "Integrated Silicon Solutions"                     , // 1/85
   "Brooktree"                                        , // 1/86
   "New Media"                                        , // 1/87
   "MHS Electronic"                                   , // 1/88
   "Performance Semi."                                , // 1/89
   "Winbond Electronic"                               , // 1/90
   "Kawasaki Steel"                                   , // 1/91
   "Bright Micro"                                     , // 1/92
   "TECMAR"                                           , // 1/93
   "Exar"                                             , // 1/94
   "PCMCIA"                                           , // 1/95
   "LG Semi (Goldstar)"                               , // 1/96
   "Northern Telecom"                                 , // 1/97
   "Sanyo"                                            , // 1/98
   "Array Microsystems"                               , // 1/99
   "Crystal Semiconductor"                            , // 1/100
   "Analog Devices"                                   , // 1/101
   "PMC-Sierra"                                       , // 1/102
   "Asparix"                                          , // 1/103
   "Convex Computer"                                  , // 1/104
   "Quality Semiconductor"                            , // 1/105
   "Nimbus Technology"                                , // 1/106
   "Transwitch"                                       , // 1/107
   "Micronas (ITT Intermetall)"                       , // 1/108
   "Cannon"                                           , // 1/109
   "Altera"                                           , // 1/110
   "NEXCOM"                                           , // 1/111
   "Qualcomm"                                         , // 1/112
   "Sony"                                             , // 1/113
   "Cray Research"                                    , // 1/114
   "AMS(Austria Micro)"                               , // 1/115
   "Vitesse"                                          , // 1/116
   "Aster Electronics"                                , // 1/117
   "Bay Networks (Synoptic)"                          , // 1/118
   "Zentrum/ZMD"                                      , // 1/119
   "TRW"                                              , // 1/120
   "Thesys"                                           , // 1/121
   "Solbourne Computer"                               , // 1/122
   "Allied-Signal"                                    , // 1/123
   "Dialog Semiconductor"                             , // 1/124
   "Media Vision"                                     , // 1/125
   "Numonyx Corporation"                                // 1/126
};

const char* JedecGroup2[] =
{
   "Cirrus Logic"                                     , // 2/1
   "National Instruments"                             , // 2/2
   "ILC Data Device"                                  , // 2/3
   "Alcatel Mietec"                                   , // 2/4
   "Micro Linear"                                     , // 2/5
   "Univ. of NC"                                      , // 2/6
   "JTAG Technologies"                                , // 2/7
   "BAE Systems (Loral)"                              , // 2/8
   "Nchip"                                            , // 2/9
   "Galileo Tech"                                     , // 2/10
   "Bestlink Systems"                                 , // 2/11
   "Graychip"                                         , // 2/12
   "GENNUM"                                           , // 2/13
   "VideoLogic"                                       , // 2/14
   "Robert Bosch"                                     , // 2/15
   "Chip Express"                                     , // 2/16
   "DATARAM"                                          , // 2/17
   "United Microelectronics Corp"                     , // 2/18
   "TCSI"                                             , // 2/19
   "Smart Modular"                                    , // 2/20
   "Hughes Aircraft"                                  , // 2/21
   "Lanstar Semiconductor"                            , // 2/22
   "Qlogic"                                           , // 2/23
   "Kingston"                                         , // 2/24
   "Music Semi"                                       , // 2/25
   "Ericsson Components"                              , // 2/26
   "SpaSE"                                            , // 2/27
   "Eon Silicon Devices"                              , // 2/28
   "Integrated Silicon Solution (ISSI)"               , // 2/29
   "DoD"                                              , // 2/30
   "Integ. Memories Tech."                            , // 2/31
   "Corollary Inc"                                    , // 2/32
   "Dallas Semiconductor"                             , // 2/33
   "Omnivision"                                       , // 2/34
   "EIV(Switzerland)"                                 , // 2/35
   "Novatel Wireless"                                 , // 2/36
   "Zarlink (Mitel)"                                  , // 2/37
   "Clearpoint"                                       , // 2/38
   "Cabletron"                                        , // 2/39
   "STEC (Silicon Tech)"                              , // 2/40
   "Vanguard"                                         , // 2/41
   "Hagiwara Sys-Com"                                 , // 2/42
   "Vantis"                                           , // 2/43
   "Celestica"                                        , // 2/44
   "Century"                                          , // 2/45
   "Hal Computers"                                    , // 2/46
   "Rohm Company Ltd"                                 , // 2/47
   "Juniper Networks"                                 , // 2/48
   "Libit Signal Processing"                          , // 2/49
   "Mushkin Enhanced Memory"                          , // 2/50
   "Tundra Semiconductor"                             , // 2/51
   "Adaptec Inc"                                      , // 2/52
   "LightSpeed Semi."                                 , // 2/53
   "ZSP Corp"                                         , // 2/54
   "AMIC Technology"                                  , // 2/55
   "Adobe Systems"                                    , // 2/56
   "Dynachip"                                         , // 2/57
   "PNY Technologies Inc"                             , // 2/58
   "Newport Digital"                                  , // 2/59
   "MMC Networks"                                     , // 2/60
   "T Square"                                         , // 2/61
   "Seiko Epson"                                      , // 2/62
   "Broadcom"                                         , // 2/63
   "Viking Components"                                , // 2/64
   "V3 Semiconductor"                                 , // 2/65
   "Flextronics (Orbit Semiconductor)"                , // 2/66
   "Suwa Electronics"                                 , // 2/67
   "Transmeta"                                        , // 2/68
   "Micron CMS"                                       , // 2/69
   "American Computer & Digital"                      , // 2/70
   "Enhance 3000 Inc"                                 , // 2/71
   "Tower Semiconductor"                              , // 2/72
   "CPU Design"                                       , // 2/73
   "Price Point"                                      , // 2/74
   "Maxim Integrated Product"                         , // 2/75
   "Tellabs"                                          , // 2/76
   "Centaur Technology"                               , // 2/77
   "Unigen Corporation"                               , // 2/78
   "Transcend Information"                            , // 2/79
   "Memory Card Technology"                           , // 2/80
   "CKD Corporation Ltd"                              , // 2/81
   "Capital Instruments Inc"                          , // 2/82
   "Aica Kogyo Ltd"                                   , // 2/83
   "Linvex Technology"                                , // 2/84
   "MSC Vertriebs GmbH"                               , // 2/85
   "AKM Company Ltd"                                  , // 2/86
   "Dynamem Inc"                                      , // 2/87
   "NERA ASA"                                         , // 2/88
   "GSI Technology"                                   , // 2/89
   "Dane-Elec (C Memory)"                             , // 2/90
   "Acorn Computers"                                  , // 2/91
   "Lara Technology"                                  , // 2/92
   "Oak Technology Inc"                               , // 2/93
   "Itec Memory"                                      , // 2/94
   "Tanisys Technology"                               , // 2/95
   "Truevision"                                       , // 2/96
   "Wintec Industries"                                , // 2/97
   "Super PC Memory"                                  , // 2/98
   "MGV Memory"                                       , // 2/99
   "Galvantech"                                       , // 2/100
   "Gadzoox Networks"                                 , // 2/101
   "Multi Dimensional Cons."                          , // 2/102
   "GateField"                                        , // 2/103
   "Integrated Memory System"                         , // 2/104
   "Triscend"                                         , // 2/105
   "XaQti"                                            , // 2/106
   "Goldenram"                                        , // 2/107
   "Clear Logic"                                      , // 2/108
   "Cimaron Communications"                           , // 2/109
   "Nippon Steel Semi. Corp"                          , // 2/110
   "Advantage Memory"                                 , // 2/111
   "AMCC"                                             , // 2/112
   "LeCroy"                                           , // 2/113
   "Yamaha Corporation"                               , // 2/114
   "Digital Microwave"                                , // 2/115
   "NetLogic Microsystems"                            , // 2/116
   "MIMOS Semiconductor"                              , // 2/117
   "Advanced Fibre"                                   , // 2/118
   "BF Goodrich Data."                                , // 2/119
   "Epigram"                                          , // 2/120
   "Acbel Polytech Inc"                               , // 2/121
   "Apacer Technology"                                , // 2/122
   "Admor Memory"                                     , // 2/123
   "FOXCONN"                                          , // 2/124
   "Quadratics Superconductor"                        , // 2/125
   "3COM"                                               // 2/126
};

const char* JedecGroup3[] =
{
   "Camintonn Corporation"                            , // 3/1
   "ISOA Incorporated"                                , // 3/2
   "Agate Semiconductor"                              , // 3/3
   "ADMtek Incorporated"                              , // 3/4
   "HYPERTEC"                                         , // 3/5
   "Adhoc Technologies"                               , // 3/6
   "MOSAID Technologies"                              , // 3/7
   "Ardent Technologies"                              , // 3/8
   "Switchcore"                                       , // 3/9
   "Cisco Systems Inc"                                , // 3/10
   "Allayer Technologies"                             , // 3/11
   "WorkX AG (Wichman)"                               , // 3/12
   "Oasis Semiconductor"                              , // 3/13
   "Novanet Semiconductor"                            , // 3/14
   "E-M Solutions"                                    , // 3/15
   "Power General"                                    , // 3/16
   "Advanced Hardware Arch."                          , // 3/17
   "Inova Semiconductors GmbH"                        , // 3/18
   "Telocity"                                         , // 3/19
   "Delkin Devices"                                   , // 3/20
   "Symagery Microsystems"                            , // 3/21
   "C-Port Corporation"                               , // 3/22
   "SiberCore Technologies"                           , // 3/23
   "Southland Microsystems"                           , // 3/24
   "Malleable Technologies"                           , // 3/25
   "Kendin Communications"                            , // 3/26
   "Great Technology Microcomputer"                   , // 3/27
   "Sanmina Corporation"                              , // 3/28
   "HADCO Corporation"                                , // 3/29
   "Corsair"                                          , // 3/30
   "Actrans System Inc"                               , // 3/31
   "ALPHA Technologies"                               , // 3/32
   "Silicon Laboratories Inc (Cygnal)"                , // 3/33
   "Artesyn Technologies"                             , // 3/34
   "Align Manufacturing"                              , // 3/35
   "Peregrine Semiconductor"                          , // 3/36
   "Chameleon Systems"                                , // 3/37
   "Aplus Flash Technology"                           , // 3/38
   "MIPS Technologies"                                , // 3/39
   "Chrysalis ITS"                                    , // 3/40
   "ADTEC Corporation"                                , // 3/41
   "Kentron Technologies"                             , // 3/42
   "Win Technologies"                                 , // 3/43
   "Tezzaron Semiconductor"                           , // 3/44
   "Extreme Packet Devices"                           , // 3/45
   "RF Micro Devices"                                 , // 3/46
   "Siemens AG"                                       , // 3/47
   "Sarnoff Corporation"                              , // 3/48
   "Itautec SA"                                       , // 3/49
   "Radiata Inc"                                      , // 3/50
   "Benchmark Elect. (AVEX)"                          , // 3/51
   "Legend"                                           , // 3/52
   "SpecTek Incorporated"                             , // 3/53
   "Hi/fn"                                            , // 3/54
   "Enikia Incorporated"                              , // 3/55
   "SwitchOn Networks"                                , // 3/56
   "AANetcom Incorporated"                            , // 3/57
   "Micro Memory Bank"                                , // 3/58
   "ESS Technology"                                   , // 3/59
   "Virata Corporation"                               , // 3/60
   "Excess Bandwidth"                                 , // 3/61
   "West Bay Semiconductor"                           , // 3/62
   "DSP Group"                                        , // 3/63
   "Newport Communications"                           , // 3/64
   "Chip2Chip Incorporated"                           , // 3/65
   "Phobos Corporation"                               , // 3/66
   "Intellitech Corporation"                          , // 3/67
   "Nordic VLSI ASA"                                  , // 3/68
   "Ishoni Networks"                                  , // 3/69
   "Silicon Spice"                                    , // 3/70
   "Alchemy Semiconductor"                            , // 3/71
   "Agilent Technologies"                             , // 3/72
   "Centillium Communications"                        , // 3/73
   "W.L. Gore"                                        , // 3/74
   "HanBit Electronics"                               , // 3/75
   "GlobeSpan"                                        , // 3/76
   "Element 14"                                       , // 3/77
   "Pycon"                                            , // 3/78
   "Saifun Semiconductors"                            , // 3/79
   "Sibyte Incorporated"                              , // 3/80
   "MetaLink Technologies"                            , // 3/81
   "Feiya Technology"                                 , // 3/82
   "I & C Technology"                                 , // 3/83
   "Shikatronics"                                     , // 3/84
   "Elektrobit"                                       , // 3/85
   "Megic"                                            , // 3/86
   "Com-Tier"                                         , // 3/87
   "Malaysia Micro Solutions"                         , // 3/88
   "Hyperchip"                                        , // 3/89
   "Gemstone Communications"                          , // 3/90
   "Anadigm (Anadyne)"                                , // 3/91
   "3ParData"                                         , // 3/92
   "Mellanox Technologies"                            , // 3/93
   "Tenx Technologies"                                , // 3/94
   "Helix AG"                                         , // 3/95
   "Domosys"                                          , // 3/96
   "Skyup Technology"                                 , // 3/97
   "HiNT Corporation"                                 , // 3/98
   "Chiaro"                                           , // 3/99
   "MDT Technologies GmbH"                            , // 3/100
   "Exbit Technology A/S"                             , // 3/101
   "Integrated Technology Express"                    , // 3/102
   "AVED Memory"                                      , // 3/103
   "Legerity"                                         , // 3/104
   "Jasmine Networks"                                 , // 3/105
   "Caspian Networks"                                 , // 3/106
   "nCUBE"                                            , // 3/107
   "Silicon Access Networks"                          , // 3/108
   "FDK Corporation"                                  , // 3/109
   "High Bandwidth Access"                            , // 3/110
   "MultiLink Technology"                             , // 3/111
   "BRECIS"                                           , // 3/112
   "World Wide Packets"                               , // 3/113
   "APW"                                              , // 3/114
   "Chicory Systems"                                  , // 3/115
   "Xstream Logic"                                    , // 3/116
   "Fast-Chip"                                        , // 3/117
   "Zucotto Wireless"                                 , // 3/118
   "Realchip"                                         , // 3/119
   "Galaxy Power"                                     , // 3/120
   "eSilicon"                                         , // 3/121
   "Morphics Technology"                              , // 3/122
   "Accelerant Networks"                              , // 3/123
   "Silicon Wave"                                     , // 3/124
   "SandCraft"                                        , // 3/125
   "Elpida"                                             // 3/126
};

const char* JedecGroup4[] =
{
   "Solectron"                                        , // 4/1
   "Optosys Technologies"                             , // 4/2
   "Buffalo (Formerly Melco)"                         , // 4/3
   "TriMedia Technologies"                            , // 4/4
   "Cyan Technologies"                                , // 4/5
   "Global Locate"                                    , // 4/6
   "Optillion"                                        , // 4/7
   "Terago Communications"                            , // 4/8
   "Ikanos Communications"                            , // 4/9
   "Princeton Technology"                             , // 4/10
   "Nanya Technology"                                 , // 4/11
   "Elite Flash Storage"                              , // 4/12
   "Mysticom"                                         , // 4/13
   "LightSand Communications"                         , // 4/14
   "ATI Technologies"                                 , // 4/15
   "Agere Systems"                                    , // 4/16
   "NeoMagic"                                         , // 4/17
   "AuroraNetics"                                     , // 4/18
   "Golden Empire"                                    , // 4/19
   "Mushkin"                                          , // 4/20
   "Tioga Technologies"                               , // 4/21
   "Netlist"                                          , // 4/22
   "TeraLogic"                                        , // 4/23
   "Cicada Semiconductor"                             , // 4/24
   "Centon Electronics"                               , // 4/25
   "Tyco Electronics"                                 , // 4/26
   "Magis Works"                                      , // 4/27
   "Zettacom"                                         , // 4/28
   "Cogency Semiconductor"                            , // 4/29
   "Chipcon AS"                                       , // 4/30
   "Aspex Technology"                                 , // 4/31
   "F5 Networks"                                      , // 4/32
   "Programmable Silicon Solutions"                   , // 4/33
   "ChipWrights"                                      , // 4/34
   "Acorn Networks"                                   , // 4/35
   "Quicklogic"                                       , // 4/36
   "Kingmax Semiconductor"                            , // 4/37
   "BOPS"                                             , // 4/38
   "Flasys"                                           , // 4/39
   "BitBlitz Communications"                          , // 4/40
   "eMemory Technology"                               , // 4/41
   "Procket Networks"                                 , // 4/42
   "Purple Ray"                                       , // 4/43
   "Trebia Networks"                                  , // 4/44
   "Delta Electronics"                                , // 4/45
   "Onex Communications"                              , // 4/46
   "Ample Communications"                             , // 4/47
   "Memory Experts Intl"                              , // 4/48
   "Astute Networks"                                  , // 4/49
   "Azanda Network Devices"                           , // 4/50
   "Dibcom"                                           , // 4/51
   "Tekmos"                                           , // 4/52
   "API NetWorks"                                     , // 4/53
   "Bay Microsystems"                                 , // 4/54
   "Firecron Ltd"                                     , // 4/55
   "Resonext Communications"                          , // 4/56
   "Tachys Technologies"                              , // 4/57
   "Equator Technology"                               , // 4/58
   "Concept Computer"                                 , // 4/59
   "SILCOM"                                           , // 4/60
   "3Dlabs"                                           , // 4/61
   "c't Magazine"                                     , // 4/62
   "Sanera Systems"                                   , // 4/63
   "Silicon Packets"                                  , // 4/64
   "Viasystems Group"                                 , // 4/65
   "Simtek"                                           , // 4/66
   "Semicon Devices Singapore"                        , // 4/67
   "Satron Handelsges"                                , // 4/68
   "Improv Systems"                                   , // 4/69
   "INDUSYS GmbH"                                     , // 4/70
   "Corrent"                                          , // 4/71
   "Infrant Technologies"                             , // 4/72
   "Ritek Corp"                                       , // 4/73
   "empowerTel Networks"                              , // 4/74
   "Hypertec"                                         , // 4/75
   "Cavium Networks"                                  , // 4/76
   "PLX Technology"                                   , // 4/77
   "Massana Design"                                   , // 4/78
   "Intrinsity"                                       , // 4/79
   "Valence Semiconductor"                            , // 4/80
   "Terawave Communications"                          , // 4/81
   "IceFyre Semiconductor"                            , // 4/82
   "Primarion"                                        , // 4/83
   "Picochip Designs Ltd"                             , // 4/84
   "Silverback Systems"                               , // 4/85
   "Jade Star Technologies"                           , // 4/86
   "Pijnenburg Securealink"                           , // 4/87
   "takeMS - Ultron AG"                               , // 4/88
   "Cambridge Silicon Radio"                          , // 4/89
   "Swissbit"                                         , // 4/90
   "Nazomi Communications"                            , // 4/91
   "eWave System"                                     , // 4/92
   "Rockwell Collins"                                 , // 4/93
   "Picocel Co Ltd (Paion)"                           , // 4/94
   "Alphamosaic Ltd"                                  , // 4/95
   "Sandburst"                                        , // 4/96
   "SiCon Video"                                      , // 4/97
   "NanoAmp Solutions"                                , // 4/98
   "Ericsson Technology"                              , // 4/99
   "PrairieComm"                                      , // 4/100
   "Mitac International"                              , // 4/101
   "Layer N Networks"                                 , // 4/102
   "MtekVision (Atsana)"                              , // 4/103
   "Allegro Networks"                                 , // 4/104
   "Marvell Semiconductors"                           , // 4/105
   "Netergy Microelectronic"                          , // 4/106
   "NVIDIA"                                           , // 4/107
   "Internet Machines"                                , // 4/108
   "Memorysolution GmbH"                              , // 4/109
   "Litchfield Communication"                         , // 4/110
   "Accton Technology"                                , // 4/111
   "Teradiant Networks"                               , // 4/112
   "Scaleo Chip"                                      , // 4/113
   "Cortina Systems"                                  , // 4/114
   "RAM Components"                                   , // 4/115
   "Raqia Networks"                                   , // 4/116
   "ClearSpeed"                                       , // 4/117
   "Matsushita Battery"                               , // 4/118
   "Xelerated"                                        , // 4/119
   "SimpleTech"                                       , // 4/120
   "Utron Technology"                                 , // 4/121
   "Astec International"                              , // 4/122
   "AVM gmbH"                                         , // 4/123
   "Redux Communications"                             , // 4/124
   "Dot Hill Systems"                                 , // 4/125
   "TeraChip"                                           // 4/126
};

const char* JedecGroup5[] =
{
   "T-RAM Incorporated"                               , // 5/1
   "Innovics Wireless"                                , // 5/2
   "Teknovus"                                         , // 5/3
   "KeyEye Communications"                            , // 5/4
   "Runcom Technologies"                              , // 5/5
   "RedSwitch"                                        , // 5/6
   "Dotcast"                                          , // 5/7
   "Silicon Mountain Memory"                          , // 5/8
   "Signia Technologies"                              , // 5/9
   "Pixim"                                            , // 5/10
   "Galazar Networks"                                 , // 5/11
   "White Electronic Designs"                         , // 5/12
   "Patriot Scientific"                               , // 5/13
   "Neoaxiom Corporation"                             , // 5/14
   "3Y Power Technology"                              , // 5/15
   "Scaleo Chip"                                      , // 5/16
   "Potentia Power Systems"                           , // 5/17
   "C-guys Incorporated"                              , // 5/18
   "Digital Communications Technology Inc"            , // 5/19
   "Silicon-Based Technology"                         , // 5/20
   "Fulcrum Microsystems"                             , // 5/21
   "Positivo Informatica Ltd"                         , // 5/22
   "XIOtech Corporation"                              , // 5/23
   "PortalPlayer"                                     , // 5/24
   "Zhiying Software"                                 , // 5/25
   "ParkerVision Inc"                                 , // 5/26
   "Phonex Broadband"                                 , // 5/27
   "Skyworks Solutions"                               , // 5/28
   "Entropic Communications"                          , // 5/29
   "I'M Intelligent Memory Ltd"                       , // 5/30
   "Zensys A/S"                                       , // 5/31
   "Legend Silicon Corp"                              , // 5/32
   "Sci-worx GmbH"                                    , // 5/33
   "SMSC (Standard Microsystems)"                     , // 5/34
   "Renesas Electronics"                              , // 5/35
   "Raza Microelectronics"                            , // 5/36
   "Phyworks"                                         , // 5/37
   "MediaTek"                                         , // 5/38
   "Non-cents Productions"                            , // 5/39
   "US Modular"                                       , // 5/40
   "Wintegra Ltd"                                     , // 5/41
   "Mathstar"                                         , // 5/42
   "StarCore"                                         , // 5/43
   "Oplus Technologies"                               , // 5/44
   "Mindspeed"                                        , // 5/45
   "Just Young Computer"                              , // 5/46
   "Radia Communications"                             , // 5/47
   "OCZ"                                              , // 5/48
   "Emuzed"                                           , // 5/49
   "LOGIC Devices"                                    , // 5/50
   "Inphi Corporation"                                , // 5/51
   "Quake Technologies"                               , // 5/52
   "Vixel"                                            , // 5/53
   "SolusTek"                                         , // 5/54
   "Kongsberg Maritime"                               , // 5/55
   "Faraday Technology"                               , // 5/56
   "Altium Ltd"                                       , // 5/57
   "Insyte"                                           , // 5/58
   "ARM Ltd"                                          , // 5/59
   "DigiVision"                                       , // 5/60
   "Vativ Technologies"                               , // 5/61
   "Endicott Interconnect Technologies"               , // 5/62
   "Pericom"                                          , // 5/63
   "Bandspeed"                                        , // 5/64
   "LeWiz Communications"                             , // 5/65
   "CPU Technology"                                   , // 5/66
   "Ramaxel Technology"                               , // 5/67
   "DSP Group"                                        , // 5/68
   "Axis Communications"                              , // 5/69
   "Legacy Electronics"                               , // 5/70
   "Chrontel"                                         , // 5/71
   "Powerchip Semiconductor"                          , // 5/72
   "MobilEye Technologies"                            , // 5/73
   "Excel Semiconductor"                              , // 5/74
   "A-DATA Technology"                                , // 5/75
   "VirtualDigm"                                      , // 5/76
   "G Skill Intl"                                     , // 5/77
   "Quanta Computer"                                  , // 5/78
   "Yield Microelectronics"                           , // 5/79
   "Afa Technologies"                                 , // 5/80
   "KINGBOX Technology Co Ltd"                        , // 5/81
   "Ceva"                                             , // 5/82
   "iStor Networks"                                   , // 5/83
   "Advance Modules"                                  , // 5/84
   "Microsoft"                                        , // 5/85
   "Open-Silicon"                                     , // 5/86
   "Goal Semiconductor"                               , // 5/87
   "ARC International"                                , // 5/88
   "Simmtec"                                          , // 5/89
   "Metanoia"                                         , // 5/90
   "Key Stream"                                       , // 5/91
   "Lowrance Electronics"                             , // 5/92
   "Adimos"                                           , // 5/93
   "SiGe Semiconductor"                               , // 5/94
   "Fodus Communications"                             , // 5/95
   "Credence Systems Corp"                            , // 5/96
   "Genesis Microchip Inc"                            , // 5/97
   "Vihana Inc"                                       , // 5/98
   "WIS Technologies"                                 , // 5/99
   "GateChange Technologies"                          , // 5/100
   "High Density Devices AS"                          , // 5/101
   "Synopsys"                                         , // 5/102
   "Gigaram"                                          , // 5/103
   "Enigma Semiconductor Inc"                         , // 5/104
   "Century Micro Inc"                                , // 5/105
   "Icera Semiconductor"                              , // 5/106
   "Mediaworks Integrated Systems"                    , // 5/107
   "O'Neil Product Development"                       , // 5/108
   "Supreme Top Technology Ltd"                       , // 5/109
   "MicroDisplay Corporation"                         , // 5/110
   "Team Group Inc"                                   , // 5/111
   "Sinett Corporation"                               , // 5/112
   "Toshiba Corporation"                              , // 5/113
   "Tensilica"                                        , // 5/114
   "SiRF Technology"                                  , // 5/115
   "Bacoc Inc"                                        , // 5/116
   "SMaL Camera Technologies"                         , // 5/117
   "Thomson SC"                                       , // 5/118
   "Airgo Networks"                                   , // 5/119
   "Wisair Ltd"                                       , // 5/120
   "SigmaTel"                                         , // 5/121
   "Arkados"                                          , // 5/122
   "Compete IT gmbH Co KG"                            , // 5/123
   "Eudar Technology Inc"                             , // 5/124
   "Focus Enhancements"                               , // 5/125
   "Xyratex"                                            // 5/126
};

const char* JedecGroup6[] =
{
   "Specular Networks"                                , // 6/1
   "Patriot Memory (PDP Systems)"                     , // 6/2
   "U-Chip Technology Corp"                           , // 6/3
   "Silicon Optix"                                    , // 6/4
   "Greenfield Networks"                              , // 6/5
   "CompuRAM GmbH"                                    , // 6/6
   "Stargen Inc"                                      , // 6/7
   "NetCell Corporation"                              , // 6/8
   "Excalibrus Technologies Ltd"                      , // 6/9
   "SCM Microsystems"                                 , // 6/10
   "Xsigo Systems Inc"                                , // 6/11
   "CHIPS & Systems Inc"                              , // 6/12
   "Tier 1 Multichip Solutions"                       , // 6/13
   "CWRL Labs"                                        , // 6/14
   "Teradici"                                         , // 6/15
   "Gigaram Inc"                                      , // 6/16
   "g2 Microsystems"                                  , // 6/17
   "PowerFlash Semiconductor"                         , // 6/18
   "P.A. Semi Inc"                                    , // 6/19
   "NovaTech Solutions S.A."                          , // 6/20
   "c2 Microsystems Inc"                              , // 6/21
   "Level5 Networks"                                  , // 6/22
   "COS Memory AG"                                    , // 6/23
   "Innovasic Semiconductor"                          , // 6/24
   "02IC Co Ltd"                                      , // 6/25
   "Tabula Inc"                                       , // 6/26
   "Crucial Technology"                               , // 6/27
   "Chelsio Communications"                           , // 6/28
   "Solarflare Communications"                        , // 6/29
   "Xambala Inc"                                      , // 6/30
   "EADS Astrium"                                     , // 6/31
   "Terra Semiconductor Inc"                          , // 6/32
   "Imaging Works Inc"                                , // 6/33
   "Astute Networks Inc"                              , // 6/34
   "Tzero"                                            , // 6/35
   "Emulex"                                           , // 6/36
   "Power-One"                                        , // 6/37
   "Pulse~LINK Inc"                                   , // 6/38
   "Hon Hai Precision Industry"                       , // 6/39
   "White Rock Networks Inc"                          , // 6/40
   "Telegent Systems USA Inc"                         , // 6/41
   "Atrua Technologies Inc"                           , // 6/42
   "Acbel Polytech Inc"                               , // 6/43
   "eRide Inc"                                        , // 6/44
   "ULi Electronics Inc"                              , // 6/45
   "Magnum Semiconductor Inc"                         , // 6/46
   "neoOne Technology Inc"                            , // 6/47
   "Connex Technology Inc"                            , // 6/48
   "Stream Processors Inc"                            , // 6/49
   "Focus Enhancements"                               , // 6/50
   "Telecis Wireless Inc"                             , // 6/51
   "uNav Microelectronics"                            , // 6/52
   "Tarari Inc"                                       , // 6/53
   "Ambric Inc"                                       , // 6/54
   "Newport Media Inc"                                , // 6/55
   "VMTS"                                             , // 6/56
   "Enuclia Semiconductor Inc"                        , // 6/57
   "Virtium Technology Inc"                           , // 6/58
   "Solid State System Co Ltd"                        , // 6/59
   "Kian Tech LLC"                                    , // 6/60
   "Artimi"                                           , // 6/61
   "Power Quotient International"                     , // 6/62
   "Avago Technologies"                               , // 6/63
   "ADTechnology"                                     , // 6/64
   "Sigma Designs"                                    , // 6/65
   "SiCortex Inc"                                     , // 6/66
   "Ventura Technology Group"                         , // 6/67
   "eASIC"                                            , // 6/68
   "M.H.S. SAS"                                       , // 6/69
   "Micro Star International"                         , // 6/70
   "Rapport Inc"                                      , // 6/71
   "Makway International"                             , // 6/72
   "Broad Reach Engineering Co"                       , // 6/73
   "Semiconductor Mfg Intl Corp"                      , // 6/74
   "SiConnect"                                        , // 6/75
   "FCI USA Inc"                                      , // 6/76
   "Validity Sensors"                                 , // 6/77
   "Coney Technology Co Ltd"                          , // 6/78
   "Spans Logic"                                      , // 6/79
   "Neterion Inc"                                     , // 6/80
   "Qimonda"                                          , // 6/81
   "New Japan Radio Co Ltd"                           , // 6/82
   "Velogix"                                          , // 6/83
   "Montalvo Systems"                                 , // 6/84
   "iVivity Inc"                                      , // 6/85
   "Walton Chaintech"                                 , // 6/86
   "AENEON"                                           , // 6/87
   "Lorom Industrial Co Ltd"                          , // 6/88
   "Radiospire Networks"                              , // 6/89
   "Sensio Technologies Inc"                          , // 6/90
   "Nethra Imaging"                                   , // 6/91
   "Hexon Technology Pte Ltd"                         , // 6/92
   "CompuStocx (CSX)"                                 , // 6/93
   "Methode Electronics Inc"                          , // 6/94
   "Connect One Ltd"                                  , // 6/95
   "Opulan Technologies"                              , // 6/96
   "Septentrio NV"                                    , // 6/97
   "Goldenmars Technology Inc"                        , // 6/98
   "Kreton Corporation"                               , // 6/99
   "Cochlear Ltd"                                     , // 6/100
   "Altair Semiconductor"                             , // 6/101
   "NetEffect Inc"                                    , // 6/102
   "Spansion Inc"                                     , // 6/103
   "Taiwan Semiconductor Mfg"                         , // 6/104
   "Emphany Systems Inc"                              , // 6/105
   "ApaceWave Technologies"                           , // 6/106
   "Mobilygen Corporation"                            , // 6/107
   "Tego"                                             , // 6/108
   "Cswitch Corporation"                              , // 6/109
   "Haier (Beijing) IC Design Co"                     , // 6/110
   "MetaRAM"                                          , // 6/111
   "Axel Electronics Co Ltd"                          , // 6/112
   "Tilera Corporation"                               , // 6/113
   "Aquantia"                                         , // 6/114
   "Vivace Semiconductor"                             , // 6/115
   "Redpine Signals"                                  , // 6/116
   "Octalica"                                         , // 6/117
   "InterDigital Communications"                      , // 6/118
   "Avant Technology"                                 , // 6/119
   "Asrock Inc"                                       , // 6/120
   "Availink"                                         , // 6/121
   "Quartics Inc"                                     , // 6/122
   "Element CXI"                                      , // 6/123
   "Innovaciones Microelectronicas"                   , // 6/124
   "VeriSilicon Microelectronics"                     , // 6/125
   "W5 Networks"                                        // 6/126
};

const char* JedecGroup7[] =
{
   "MOVEKING"                                         , // 7/1
   "Mavrix Technology Inc"                            , // 7/2
   "CellGuide Ltd"                                    , // 7/3
   "Faraday Technology"                               , // 7/4
   "Diablo Technologies Inc"                          , // 7/5
   "Jennic"                                           , // 7/6
   "Octasic"                                          , // 7/7
   "Molex Incorporated"                               , // 7/8
   "3Leaf Networks"                                   , // 7/9
   "Bright Micron Technology"                         , // 7/10
   "Netxen"                                           , // 7/11
   "NextWave Broadband Inc"                           , // 7/12
   "DisplayLink"                                      , // 7/13
   "ZMOS Technology"                                  , // 7/14
   "Tec-Hill"                                         , // 7/15
   "Multigig Inc"                                     , // 7/16
   "Amimon"                                           , // 7/17
   "Euphonic Technologies Inc"                        , // 7/18
   "BRN Phoenix"                                      , // 7/19
   "InSilica"                                         , // 7/20
   "Ember Corporation"                                , // 7/21
   "Avexir Technologies Corporation"                  , // 7/22
   "Echelon Corporation"                              , // 7/23
   "Edgewater Computer Systems"                       , // 7/24
   "XMOS Semiconductor Ltd"                           , // 7/25
   "GENUSION Inc"                                     , // 7/26
   "Memory Corp NV"                                   , // 7/27
   "SiliconBlue Technologies"                         , // 7/28
   "Rambus Inc"                                       , // 7/29
   "Andes Technology Corporation"                     , // 7/30
   "Coronis Systems"                                  , // 7/31
   "Achronix Semiconductor"                           , // 7/32
   "Siano Mobile Silicon Ltd"                         , // 7/33
   "Semtech Corporation"                              , // 7/34
   "Pixelworks Inc"                                   , // 7/35
   "Gaisler Research AB"                              , // 7/36
   "Teranetics"                                       , // 7/37
   "Toppan Printing Co Ltd"                           , // 7/38
   "Kingxcon"                                         , // 7/39
   "Silicon Integrated Systems"                       , // 7/40
   "I-O Data Device Inc"                              , // 7/41
   "NDS Americas Inc"                                 , // 7/42
   "Solomon Systech Limited"                          , // 7/43
   "On Demand Microelectronics"                       , // 7/44
   "Amicus Wireless Inc"                              , // 7/45
   "SMARDTV SNC"                                      , // 7/46
   "Comsys Communication Ltd"                         , // 7/47
   "Movidia Ltd"                                      , // 7/48
   "Javad GNSS Inc"                                   , // 7/49
   "Montage Technology Group"                         , // 7/50
   "Trident Microsystems"                             , // 7/51
   "Super Talent"                                     , // 7/52
   "Optichron Inc"                                    , // 7/53
   "Future Waves UK Ltd"                              , // 7/54
   "SiBEAM Inc"                                       , // 7/55
   "InicoreInc"                                       , // 7/56
   "Virident Systems"                                 , // 7/57
   "M2000 Inc"                                        , // 7/58
   "ZeroG Wireless Inc"                               , // 7/59
   "Gingle Technology Co Ltd"                         , // 7/60
   "Space Micro Inc"                                  , // 7/61
   "Wilocity"                                         , // 7/62
   "Novafora Inc"                                     , // 7/63
   "iKoa Corporation"                                 , // 7/64
   "ASint Technology"                                 , // 7/65
   "Ramtron"                                          , // 7/66
   "Plato Networks Inc"                               , // 7/67
   "IPtronics AS"                                     , // 7/68
   "Infinite-Memories"                                , // 7/69
   "Parade Technologies Inc"                          , // 7/70
   "Dune Networks"                                    , // 7/71
   "GigaDevice Semiconductor"                         , // 7/72
   "Modu Ltd"                                         , // 7/73
   "CEITEC"                                           , // 7/74
   "Northrop Grumman"                                 , // 7/75
   "XRONET Corporation"                               , // 7/76
   "Sicon Semiconductor AB"                           , // 7/77
   "Atla Electronics Co Ltd"                          , // 7/78
   "TOPRAM Technology"                                , // 7/79
   "Silego Technology Inc"                            , // 7/80
   "Kinglife"                                         , // 7/81
   "Ability Industries Ltd"                           , // 7/82
   "Silicon Power Computer &"                         , // 7/83
   "Augusta Technology Inc"                           , // 7/84
   "Nantronics Semiconductors"                        , // 7/85
   "Hilscher Gesellschaft"                            , // 7/86
   "Quixant Ltd"                                      , // 7/87
   "Percello Ltd"                                     , // 7/88
   "NextIO Inc"                                       , // 7/89
   "Scanimetrics Inc"                                 , // 7/90
   "FS-Semi Company Ltd"                              , // 7/91
   "Infinera Corporation"                             , // 7/92
   "SandForce Inc"                                    , // 7/93
   "Lexar Media"                                      , // 7/94
   "Teradyne Inc"                                     , // 7/95
   "Memory Exchange Corp"                             , // 7/96
   "Suzhou Smartek Electronics"                       , // 7/97
   "Avantium Corporation"                             , // 7/98
   "ATP Electronics Inc"                              , // 7/99
   "Valens Semiconductor Ltd"                         , // 7/100
   "Agate Logic Inc"                                  , // 7/101
   "Netronome"                                        , // 7/102
   "Zenverge Inc"                                     , // 7/103
   "N-trig Ltd"                                       , // 7/104
   "SanMax Technologies Inc"                          , // 7/105
   "Contour Semiconductor Inc"                        , // 7/106
   "TwinMOS"                                          , // 7/107
   "Silicon Systems Inc"                              , // 7/108
   "V-Color Technology Inc"                           , // 7/109
   "Certicom Corporation"                             , // 7/110
   "JSC ICC Milandr"                                  , // 7/111
   "PhotoFast Global Inc"                             , // 7/112
   "InnoDisk Corporation"                             , // 7/113
   "Muscle Power"                                     , // 7/114
   "Energy Micro"                                     , // 7/115
   "Innofidei"                                        , // 7/116
   "CopperGate Communications"                        , // 7/117
   "Holtek Semiconductor Inc"                         , // 7/118
   "Myson Century Inc"                                , // 7/119
   "FIDELIX"                                          , // 7/120
   "Red Digital Cinema"                               , // 7/121
   "Densbits Technology"                              , // 7/122
   "Zempro"                                           , // 7/123
   "MoSys"                                            , // 7/124
   "Provigent"                                        , // 7/125
   "Triad Semiconductor Inc"                            // 7/126
};

const char* JedecGroup8[] =
{
   "Siklu Communication Ltd"                          , // 8/1
   "A Force Manufacturing Ltd"                        , // 8/2
   "Strontium"                                        , // 8/3
   "ALi Corp (Abilis Systems)"                        , // 8/4
   "Siglead Inc"                                      , // 8/5
   "Ubicom Inc"                                       , // 8/6
   "Unifosa Corporation"                              , // 8/7
   "Stretch Inc"                                      , // 8/8
   "Lantiq Deutschland GmbH"                          , // 8/9
   "Visipro."                                         , // 8/10
   "EKMemory"                                         , // 8/11
   "Microelectronics Institute ZTE"                   , // 8/12
   "u-blox AG"                                        , // 8/13
   "Carry Technology Co Ltd"                          , // 8/14
   "Nokia"                                            , // 8/15
   "King Tiger Technology"                            , // 8/16
   "Sierra Wireless"                                  , // 8/17
   "HT Micron"                                        , // 8/18
   "Albatron Technology Co Ltd"                       , // 8/19
   "Leica Geosystems AG"                              , // 8/20
   "BroadLight"                                       , // 8/21
   "AEXEA"                                            , // 8/22
   "ClariPhy Communications Inc"                      , // 8/23
   "Green Plug"                                       , // 8/24
   "Design Art Networks"                              , // 8/25
   "Mach Xtreme Technology Ltd"                       , // 8/26
   "ATO Solutions Co Ltd"                             , // 8/27
   "Ramsta"                                           , // 8/28
   "Greenliant Systems Ltd"                           , // 8/29
   "Teikon"                                           , // 8/30
   "Antec Hadron"                                     , // 8/31
   "NavCom Technology Inc"                            , // 8/32
   "Shanghai Fudan Microelectronics"                  , // 8/33
   "Calxeda Inc"                                      , // 8/34
   "JSC EDC Electronics"                              , // 8/35
   "Kandit Technology Co Ltd"                         , // 8/36
   "Ramos Technology"                                 , // 8/37
   "Goldenmars Technology"                            , // 8/38
   "XeL Technology Inc"                               , // 8/39
   "Newzone Corporation"                              , // 8/40
   "ShenZhen MercyPower Tech"                         , // 8/41
   "Nanjing Yihuo Technology"                         , // 8/42
   "Nethra Imaging Inc"                               , // 8/43
   "SiTel Semiconductor BV"                           , // 8/44
   "SolidGear Corporation"                            , // 8/45
   "Topower Computer Ind Co Ltd"                      , // 8/46
   "Wilocity"                                         , // 8/47
   "Profichip GmbH"                                   , // 8/48
   "Gerad Technologies"                               , // 8/49
   "Ritek Corporation"                                , // 8/50
   "Gomos Technology Limited"                         , // 8/51
   "Memoright Corporation"                            , // 8/52
   "D-Broad Inc"                                      , // 8/53
   "HiSilicon Technologies"                           , // 8/54
   "Syndiant Inc."                                    , // 8/55
   "Enverv Inc"                                       , // 8/56
   "Cognex"                                           , // 8/57
   "Xinnova Technology Inc"                           , // 8/58
   "Ultron AG"                                        , // 8/59
   "Concord Idea Corporation"                         , // 8/60
   "AIM Corporation"                                  , // 8/61
   "Lifetime Memory Products"                         , // 8/62
   "Ramsway"                                          , // 8/63
   "Recore Systems B.V."                              , // 8/64
   "Haotian Jinshibo Science Tech"                    , // 8/65
   "Being Advanced Memory"                            , // 8/66
   "Adesto Technologies"                              , // 8/67
   "Giantec Semiconductor Inc"                        , // 8/68
   "HMD Electronics AG"                               , // 8/69
   "Gloway International (HK)"                        , // 8/70
   "Kingcore"                                         , // 8/71
   "Anucell Technology Holding"                       , // 8/72
   "Accord Software & Systems Pvt. Ltd"               , // 8/73
   "Active-Semi Inc"                                  , // 8/74
   "Denso Corporation"                                , // 8/75
   "TLSI Inc"                                         , // 8/76
   "Qidan"                                            , // 8/77
   "Mustang"                                          , // 8/78
   "Orca Systems"                                     , // 8/79
   "Passif Semiconductor"                             , // 8/80
   "GigaDevice Semiconductor (Beijing)"               , // 8/81
   "Memphis Electronic"                               , // 8/82
   "Beckhoff Automation GmbH"                         , // 8/83
   "Harmony Semiconductor Corp"                       , // 8/84
   "Air Computers SRL"                                , // 8/85
   "TMT Memory"                                       , // 8/86
   "Eorex Corporation"                                , // 8/87
   "Xingtera"                                         , // 8/88
   "Netsol"                                           , // 8/89
   "Bestdon Technology Co Ltd"                        , // 8/90
   "Baysand Inc"                                      , // 8/91
   "Uroad Technology Co Ltd"                          , // 8/92
   "Wilk Elektronik S.A."                             , // 8/93
   "AAI"                                              , // 8/94
   "Harman"                                           , // 8/95
   "Berg Microelectronics Inc"                        , // 8/96
   "ASSIA Inc"                                        , // 8/97
   "Visiontek Products LLC"                           , // 8/98
   "OCMEMORY"                                         , // 8/99
   "Welink Solution Inc"                              , // 8/100
   "Shark Gaming"                                     , // 8/101
   "Avalanche Technology"                             , // 8/102
   "R&D Center ELVEES OJSC"                           , // 8/103
   "KingboMars Technology Co Ltd"                     , // 8/104
   "High Bridge Solutions Industria"                  , // 8/105
   "Transcend Technology Co Ltd"                      , // 8/106
   "Everspin Technologies"                            , // 8/107
   "Hon-Hai Precision"                                , // 8/108
   "Smart Storage Systems"                            , // 8/109
   "Toumaz Group"                                     , // 8/110
   "Zentel Electronics Corporation"                   , // 8/111
   "Panram International Corporation"                 , // 8/112
   "Silicon Space Technology"                         , // 8/113
   "LITE-ON IT Corporation"                           , // 8/114
   "Inuitive"                                         , // 8/115
   "HMicro"                                           , // 8/116
   "BittWare Inc"                                     , // 8/117
   "GLOBALFOUNDRIES"                                  , // 8/118
   "ACPI Digital Co Ltd"                              , // 8/119
   "Annapurna Labs"                                   , // 8/120
   "AcSiP Technology Corporation"                     , // 8/121
   "Idea! Electronic Systems"                         , // 8/122
   "Gowe Technology Co Ltd"                           , // 8/123
   "Hermes Testing Solutions Inc"                     , // 8/124
   "Positivo BGH"                                     , // 8/125
   "Intelligence  Silicon Technology"                   // 8/126
};

const char* JedecGroup9[] =
{
   "3D PLUS"                                          , // 9/1
   "Diehl Aerospace"                                  , // 9/2
   "Fairchild"                                        , // 9/3
   "Mercury Systems"                                  , // 9/4
   "Sonics Inc"                                       , // 9/5
   "Emerson Automation Solutions"                     , // 9/6
   "Shenzhen Jinge Information Co Ltd"                , // 9/7
   "SCWW"                                             , // 9/8
   "Silicon Motion Inc"                               , // 9/9
   "Anurag"                                           , // 9/10
   "King Kong"                                        , // 9/11
   "FROM30 Co Ltd"                                    , // 9/12
   "Gowin Semiconductor Corp"                         , // 9/13
   "Fremont Micro Devices Ltd"                        , // 9/14
   "Ericsson Modems"                                  , // 9/15
   "Exelis"                                           , // 9/16
   "Satixfy Ltd"                                      , // 9/17
   "Galaxy Microsystems Ltd"                          , // 9/18
   "Gloway International Co Ltd"                      , // 9/19
   "Lab"                                              , // 9/20
   "Smart Energy Instruments"                         , // 9/21
   "Approved Memory Corporation"                      , // 9/22
   "Axell Corporation"                                , // 9/23
   "Essencore Limited"                                , // 9/24
   "Phytium"                                          , // 9/25
   "Xi'an UniIC Semiconductors Co Ltd"                , // 9/26
   "Ambiq Micro"                                      , // 9/27
   "eveRAM Technology Inc"                            , // 9/28
   "Infomax"                                          , // 9/29
   "Butterfly Network Inc"                            , // 9/30
   "Shenzhen City Gcai Electronics"                   , // 9/31
   "Stack Devices Corporation"                        , // 9/32
   "ADK Media Group"                                  , // 9/33
   "TSP Global Co Ltd"                                , // 9/34
   "HighX"                                            , // 9/35
   "Shenzhen Elicks Technology"                       , // 9/36
   "XinKai/Silicon Kaiser"                            , // 9/37
   "Google Inc"                                       , // 9/38
   "Dasima International Development"                 , // 9/39
   "Leahkinn Technology Limited"                      , // 9/40
   "HIMA Paul Hildebrandt GmbH Co KG"                 , // 9/41
   "Keysight Technologies"                            , // 9/42
   "Techcomp International (Fastable)"                , // 9/43
   "Ancore Technology Corporation"                    , // 9/44
   "Nuvoton"                                          , // 9/45
   "Korea Uhbele International Group Ltd"             , // 9/46
   "Ikegami Tsushinki Co Ltd"                         , // 9/47
   "RelChip Inc"                                      , // 9/48
   "Baikal Electronics"                               , // 9/49
   "Nemostech Inc"                                    , // 9/50
   "Memorysolution GmbH"                              , // 9/51
   "Silicon Integrated Systems Corporation"           , // 9/52
   "Xiede"                                            , // 9/53
   "BRC"                                              , // 9/54
   "Flash Chi"                                        , // 9/55
   "Jone"                                             , // 9/56
   "GCT Semiconductor Inc"                            , // 9/57
   "Hong Kong Zetta Device Technology"                , // 9/58
   "Unimemory Technology(s) Pte Ltd"                  , // 9/59
   "Cuso"                                             , // 9/60
   "Kuso"                                             , // 9/61
   "Uniquify Inc"                                     , // 9/62
   "Skymedi Corporation"                              , // 9/63
   "Core Chance Co Ltd"                               , // 9/64
   "Tekism Co Ltd"                                    , // 9/65
   "Seagate Technology PLC"                           , // 9/66
   "Hong Kong Gaia Group Co Limited"                  , // 9/67
   "Gigacom Semiconductor LLC"                        , // 9/68
   "V2 Technologies"                                  , // 9/69
   "TLi"                                              , // 9/70
   "Neotion"                                          , // 9/71
   "Lenovo"                                           , // 9/72
   "Shenzhen Zhongteng Electronic Corp Ltd"           , // 9/73
   "Compound Photonics"                               , // 9/74
   "in2H2 inc"                                        , // 9/75
   "Shenzhen Pango Microsystems Co Ltd"               , // 9/76
   "Vasekey"                                          , // 9/77
   "Cal-Comp Industria de"                            , // 9/78
   "Eyenix Co Ltd"                                    , // 9/79
   "Heoriady"                                         , // 9/80
   "Accelerated Memory Production Inc"                , // 9/81
   "INVECAS Inc"                                      , // 9/82
   "AP Memory"                                        , // 9/83
   "Douqi Technology"                                 , // 9/84
   "Etron Technology Inc"                             , // 9/85
   "Indie Semiconductor"                              , // 9/86
   "Socionext Inc"                                    , // 9/87
   "HGST"                                             , // 9/88
   "EVGA"                                             , // 9/89
   "Audience Inc"                                     , // 9/90
   "EpicGear"                                         , // 9/91
   "Vitesse Enterprise Co"                            , // 9/92
   "Foxtronn International Corporation"               , // 9/93
   "Bretelon Inc"                                     , // 9/94
   "Graphcore"                                        , // 9/95
   "Eoplex Inc"                                       , // 9/96
   "MaxLinear Inc"                                    , // 9/97
   "ETA Devices"                                      , // 9/98
   "LOKI"                                             , // 9/99
   "IMS Electronics Co Ltd"                           , // 9/100
   "Dosilicon Co Ltd"                                 , // 9/101
   "Dolphin Integration"                              , // 9/102
   "Shenzhen Mic Electronics Technolog"               , // 9/103
   "Boya Microelectronics Inc"                        , // 9/104
   "Geniachip (Roche)"                                , // 9/105
   "Axign"                                            , // 9/106
   "Kingred Electronic Technology Ltd"                , // 9/107
   "Chao Yue Zhuo Computer Business Dept."            , // 9/108
   "Guangzhou Si Nuo Electronic"                      , // 9/109
   "Crocus Technology Inc"                            , // 9/110
   "Creative Chips GmbH"                              , // 9/111
   "GE Aviation Systems LLC."                         , // 9/112
   "Asgard"                                           , // 9/113
   "Good Wealth Technology Ltd"                       , // 9/114
   "TriCor Technologies"                              , // 9/115
   "Nova-Systems GmbH"                                , // 9/116
   "JUHOR"                                            , // 9/117
   "Zhuhai Douke Commerce Co Ltd"                     , // 9/118
   "DSL Memory"                                       , // 9/119
   "Anvo-Systems Dresden GmbH"                        , // 9/120
   "Realtek"                                          , // 9/121
   "AltoBeam"                                         , // 9/122
   "Wave Computing"                                   , // 9/123
   "Beijing TrustNet Technology Co Ltd"               , // 9/124
   "Innovium Inc"                                     , // 9/125
   "Starsway Technology Limited"                        // 9/126
};

const char* JedecGroup10[] =
{
   "Weltronics Co LTD"                                , // 10/1
   "VMware Inc"                                       , // 10/2
   "Hewlett Packard Enterprise"                       , // 10/3
   "INTENSO"                                          , // 10/4
   "Puya Semiconductor"                               , // 10/5
   "MEMORFI"                                          , // 10/6
   "MSC Technologies GmbH"                            , // 10/7
   "Txrui"                                            , // 10/8
   "SiFive Inc"                                       , // 10/9
   "Spreadtrum Communications"                        , // 10/10
   "XTX Technology Limited"                           , // 10/11
   "UMAX Technology"                                  , // 10/12
   "Shenzhen Yong Sheng Technology"                   , // 10/13
   "SNOAMOO (Shenzhen Kai Zhuo Yue)"                  , // 10/14
   "Daten Tecnologia LTDA"                            , // 10/15
   "Shenzhen XinRuiYan Electronics"                   , // 10/16
   "Eta Compute"                                      , // 10/17
   "Energous"                                         , // 10/18
   "Raspberry Pi Trading Ltd"                         , // 10/19
   "Shenzhen Chixingzhe Tech Co Ltd"                  , // 10/20
   "Silicon Mobility"                                 , // 10/21
   "IQ-Analog Corporation"                            , // 10/22
   "Uhnder Inc"                                       , // 10/23
   "Impinj"                                           , // 10/24
   "DEPO Computers"                                   , // 10/25
   "Nespeed Sysems"                                   , // 10/26
   "Yangtze Memory Technologies Co Ltd"               , // 10/27
   "MemxPro Inc"                                      , // 10/28
   "Tammuz Co Ltd"                                    , // 10/29
   "Allwinner Technology"                             , // 10/30
   "Shenzhen City"                                    , // 10/31
   "XMC"                                              , // 10/32
   "Teclast"                                          , // 10/33
   "Maxsun"                                           , // 10/34
   "Haiguang Integrated Circuit Design"               , // 10/35
   "RamCENTER Technology"                             , // 10/36
   "Phison Electronics Corporation"                   , // 10/37
   "Guizhou Huaxintong Semi-Conductor"                , // 10/38
   "Network Intelligence"                             , // 10/39
   "Continental Technology (Holdings)"                , // 10/40
   "Guangzhou Huayan Suning Electronic"               , // 10/41
   "Guangzhou Zhouji Electronic Co Ltd"               , // 10/42
   "Shenzhen Giant Hui Kang Tech Co Ltd"              , // 10/43
   "Shenzhen Yilong Innovative Co Ltd"                , // 10/44
   "Neo Forza"                                        , // 10/45
   "Lyontek Inc"                                      , // 10/46
   "Shanghai Kuxin Microelectronics Ltd"              , // 10/47
   "Shenzhen Larix Technology Co Ltd"                 , // 10/48
   "Qbit Semiconductor Ltd"                           , // 10/49
   "Insignis Technology Corporation"                  , // 10/50
   "Lanson Memory Co Ltd"                             , // 10/51
   "Shenzhen Superway Electronics Co Ltd"             , // 10/52
   "Canaan-Creative Co Ltd"                           , // 10/53
   "Black Diamond Memory"                             , // 10/54
   "Shenzhen City Parker Baking Electronics"          , // 10/55
   "Shenzhen Baihong Technology Co Ltd"               , // 10/56
   "GEO Semiconductors"                               , // 10/57
   "OCPC"                                             , // 10/58
   "Artery Technology Co Ltd"                         , // 10/59
   "Jinyu"                                            , // 10/60
   "ShenzhenYing Chi Technology Development"          , // 10/61
   "Shenzhen Pengcheng Xin Technology"                , // 10/62
   "Pegasus Semiconductor (Shanghai) Co"              , // 10/63
   "Mythic Inc"                                       , // 10/64
   "Elmos Semiconductor AG"                           , // 10/65
   "Kllisre"                                          , // 10/66
   "Shenzhen Winconway Technology"                    , // 10/67
   "Shenzhen Xingmem Technology Corp"                 , // 10/68
   "Gold Key Technology Co Ltd"                       , // 10/69
   "Habana Labs Ltd"                                  , // 10/70
   "Hoodisk Electronics Co Ltd"                       , // 10/71
   "SemsoTai (SZ) Technology Co Ltd"                  , // 10/72
   "OM Nanotech Pvt. Ltd"                             , // 10/73
   "Shenzhen Zhifeng Weiye Technology"                , // 10/74
   "Xinshirui (Shenzhen) Electronics Co"              , // 10/75
   "Guangzhou Zhong Hao Tian Electronic"              , // 10/76
   "Shenzhen Longsys Electronics Co Ltd"              , // 10/77
   "Deciso B.V."                                      , // 10/78
   "Puya Semiconductor (Shenzhen)"                    , // 10/79
   "Shenzhen Veineda Technology Co Ltd"               , // 10/80
   "Antec Memory"                                     , // 10/81
   "Cortus SAS"                                       , // 10/82
   "Dust Leopard"                                     , // 10/83
   "MyWo AS"                                          , // 10/84
   "J&A Information Inc"                              , // 10/85
   "Shenzhen JIEPEI Technology Co Ltd"                , // 10/86
   "Heidelberg University"                            , // 10/87
   "Flexxon PTE Ltd"                                  , // 10/88
   "Wiliot"                                           , // 10/89
   "Raysun Electronics International Ltd"             , // 10/90
   "Aquarius Production Company LLC"                  , // 10/91
   "MACNICA DHW LTDA"                                 , // 10/92
   "Intelimem"                                        , // 10/93
   "Zbit Semiconductor Inc"                           , // 10/94
   "Shenzhen Technology Co Ltd"                       , // 10/95
   "Signalchip"                                       , // 10/96
   "Shenzen Recadata Storage Technology"              , // 10/97
   "Hyundai Technology"                               , // 10/98
   "Shanghai Fudi Investment Development"             , // 10/99
   "Aixi Technology"                                  , // 10/100
   "Tecon MT"                                         , // 10/101
   "Onda Electric Co Ltd"                             , // 10/102
   "Jinshen"                                          , // 10/103
   "Kimtigo Semiconductor (HK) Limited"               , // 10/104
   "IIT Madras"                                       , // 10/105
   "Shenshan (Shenzhen) Electronic"                   , // 10/106
   "Hefei Core Storage Electronic Limited"            , // 10/107
   "Colorful Technology Ltd"                          , // 10/108
   "Visenta (Xiamen) Technology Co Ltd"               , // 10/109
   "Roa Logic BV"                                     , // 10/110
   "NSITEXE Inc"                                      , // 10/111
   "Hong Kong Hyunion Electronics"                    , // 10/112
   "ASK Technology Group Limited"                     , // 10/113
   "GIGA-BYTE Technology Co Ltd"                      , // 10/114
   "Terabyte Co Ltd"                                  , // 10/115
   "Hyundai Inc"                                      , // 10/116
   "EXCELERAM"                                        , // 10/117
   "PsiKick"                                          , // 10/118
   "Netac Technology Co Ltd"                          , // 10/119
   "PCCOOLER"                                         , // 10/120
   "Jiangsu Huacun Electronic Technology"             , // 10/121
   "Shenzhen Micro Innovation Industry"               , // 10/122
   "Beijing Tongfang Microelectronics Co"             , // 10/123
   "XZN Storage Technology"                           , // 10/124
   "ChipCraft Sp. z.o.o."                             , // 10/125
   "ALLFLASH Technology Limited"                        // 10/126
};

const char* JedecGroup11[] =
{
   "Foerd Technology Co Ltd"                          , // 11/1
   "KingSpec"                                         , // 11/2
   "Codasip GmbH"                                     , // 11/3
   "SL Link Co Ltd"                                   , // 11/4
   "Shenzhen Kefu Technology Co Limited"              , // 11/5
   "Shenzhen ZST Electronics Technology"              , // 11/6
   "Kyokuto Electronic Inc"                           , // 11/7
   "Warrior Technology"                               , // 11/8
   "TRINAMIC Motion Control GmbH & Co"                , // 11/9
   "PixelDisplay Inc"                                 , // 11/10
   "Shenzhen Futian District Bo Yueda Elec"           , // 11/11
   "Richtek Power"                                    , // 11/12
   "Shenzhen LianTeng Electronics Co Ltd"             , // 11/13
   "AITC Memory"                                      , // 11/14
   "UNIC Memory Technology Co Ltd"                    , // 11/15
   "Shenzhen Huafeng Science Technology"              , // 11/16
   "CXMT"                                             , // 11/17
   "Guangzhou Xinyi Heng Computer"                    , // 11/18
   "SambaNova Systems"                                , // 11/19
   "V-GEN"                                            , // 11/20
   "Jump Trading"                                     , // 11/21
   "Ampere Computing"                                 , // 11/22
   "Shenzhen Zhongshi Technology Co Ltd"              , // 11/23
   "Shenzhen Zhongtian Bozhong Technology"            , // 11/24
   "Tri-Tech International"                           , // 11/25
   "Silicon Intergrated Systems Corporation"          , // 11/26
   "Shenzhen HongDingChen Information"                , // 11/27
   "Plexton Holdings Limited"                         , // 11/28
   "AMS (Jiangsu Advanced Memory Semi)"               , // 11/29
   "Wuhan Jing Tian Interconnected Tech Co"           , // 11/30
   "Axia Memory Technology"                           , // 11/31
   "Chipset Technology Holding Limited"               , // 11/32
   "Shenzhen Xinshida Technology Co Ltd"              , // 11/33
   "Shenzhen Chuangshifeida Technology"               , // 11/34
   "Guangzhou MiaoYuanJi Technology"                  , // 11/35
   "ADVAN Inc"                                        , // 11/36
   "Shenzhen Qianhai Weishengda"                      , // 11/37
   "Guangzhou Guang Xie Cheng Trading"                , // 11/38
   "StarRam International Co Ltd"                     , // 11/39
   "Shen Zhen XinShenHua Tech Co Ltd"                 , // 11/40
   "UltraMemory Inc"                                  , // 11/41
   "New Coastline Global Tech Industry Co"            , // 11/42
   "Sinker"                                           , // 11/43
   "Diamond"                                          , // 11/44
   "PUSKILL"                                          , // 11/45
   "Guangzhou Hao Jia Ye Technology Co"               , // 11/46
   "Ming Xin Limited"                                 , // 11/47
   "Barefoot Networks"                                , // 11/48
   "Biwin Semiconductor (HK) Co Ltd"                  , // 11/49
   "UD INFO Corporation"                              , // 11/50
   "Trek Technology (S) PTE Ltd"                      , // 11/51
   "Xiamen Kingblaze Technology Co Ltd"               , // 11/52
   "Shenzhen Lomica Technology Co Ltd"                , // 11/53
   "Nuclei System Technology Co Ltd"                  , // 11/54
   "Wuhan Xun Zhan Electronic Technology"             , // 11/55
   "Shenzhen Ingacom Semiconductor Ltd"               , // 11/56
   "Zotac Technology Ltd"                             , // 11/57
   "Foxline"                                          , // 11/58
   "Shenzhen Farasia Science Technology"              , // 11/59
   "Efinix Inc"                                       , // 11/60
   "Hua Nan San Xian Technology Co Ltd"               , // 11/61
   "Goldtech Electronics Co Ltd"                      , // 11/62
   "Shanghai Han Rong Microelectronics Co"            , // 11/63
   "Shenzhen Zhongguang Yunhe Trading"                , // 11/64
   "Smart Shine(QingDao) Microelectronics"            , // 11/65
   "Thermaltake Technology Co Ltd"                    , // 11/66
   "Shenzhen O'Yang Maile Technology Ltd"             , // 11/67
   "UPMEM"                                            , // 11/68
   "Chun Well Technology Holding Limited"             , // 11/69
   "Astera Labs Inc"                                  , // 11/70
   "Winconway"                                        , // 11/71
   "Advantech Co Ltd"                                 , // 11/72
   "Chengdu Fengcai Electronic Technology"            , // 11/73
   "The Boeing Company"                               , // 11/74
   "Blaize Inc"                                       , // 11/75
   "Ramonster Technology Co Ltd"                      , // 11/76
   "Wuhan Naonongmai Technology Co Ltd"               , // 11/77
   "Shenzhen Hui ShingTong Technology"                , // 11/78
   "Yourlyon"                                         , // 11/79
   "Fabu Technology"                                  , // 11/80
   "Shenzhen Yikesheng Technology Co Ltd"             , // 11/81
   "NOR-MEM"                                          , // 11/82
   "Cervoz Co Ltd"                                    , // 11/83
   "Bitmain Technologies Inc."                        , // 11/84
   "Facebook Inc"                                     , // 11/85
   "Shenzhen Longsys Electronics Co Ltd"              , // 11/86
   "Guangzhou Siye Electronic Technology"             , // 11/87
   "Silergy"                                          , // 11/88
   "Adamway"                                          , // 11/89
   "PZG"                                              , // 11/90
   "Shenzhen King Power Electronics"                  , // 11/91
   "Guangzhou ZiaoFu Tranding Co Ltd"                 , // 11/92
   "Shenzhen SKIHOTAR Semiconductor"                  , // 11/93
   "PulseRain Technology"                             , // 11/94
   "Seeker Technology Limited"                        , // 11/95
   "Shenzhen OSCOO Tech Co Ltd"                       , // 11/96
   "Shenzhen Yze Technology Co Ltd"                   , // 11/97
   "Shenzhen Jieshuo Electronic Commerce"             , // 11/98
   "Gazda"                                            , // 11/99
   "Hua Wei Technology Co Ltd"                        , // 11/100
   "Esperanto Technologies"                           , // 11/101
   "JinSheng Electronic (Shenzhen) Co Ltd"            , // 11/102
   "Shenzhen Shi Bolunshuai Technology"               , // 11/103
   "Shanghai Rei Zuan Information Tech"               , // 11/104
   "Fraunhofer IIS"                                   , // 11/105
   "Kandou Bus SA"                                    , // 11/106
   "Acer"                                             , // 11/107
   "Artmem Technology Co Ltd"                         , // 11/108
   "Gstar Semiconductor Co Ltd"                       , // 11/109
   "ShineDisk"                                        , // 11/110
   "Shenzhen CHN Technology Co Ltd"                   , // 11/111
   "UnionChip Semiconductor Co Ltd"                   , // 11/112
   "Tanbassh"                                         , // 11/113
   "Shenzhen Tianyu Jieyun Intl Logistics"            , // 11/114
   "MCLogic Inc"                                      , // 11/115
   "Eorex Corporation"                                , // 11/116
   "Arm Technology (China) Co Ltd"                    , // 11/117
   "Lexar Co Limited"                                 , // 11/118
   "QinetiQ Group plc"                                , // 11/119
   "Exascend"                                         , // 11/120
   "Hong Kong Hyunion Electronics Co Ltd"             , // 11/121
   "Shenzhen Banghong Electronics Co Ltd"             , // 11/122
   "MBit Wireless Inc"                                , // 11/123
   "Hex Five Security Inc"                            , // 11/124
   "ShenZhen Juhor Precision Tech Co Ltd"             , // 11/125
   "Shenzhen Reeinno Technology Co Ltd"                 // 11/126
};

const char* JedecGroup12[] =
{
   "ABIT Electronics (Shenzhen) Co Ltd"               , // 12/1
   "Semidrive"                                        , // 12/2
   "MyTek Electronics Corp"                           , // 12/3
   "Wxilicon Technology Co Ltd"                       , // 12/4
   "Shenzhen Meixin Electronics Ltd"                  , // 12/5
   "Ghost Wolf"                                       , // 12/6
   "LiSion Technologies Inc"                          , // 12/7
   "Power Active Co Ltd"                              , // 12/8
   "Pioneer High Fidelity Taiwan Co. Ltd"             , // 12/9
   "LuoSilk"                                          , // 12/10
   "Shenzhen Chuangshifeida Technology"               , // 12/11
   "Black Sesame Technologies Inc"                    , // 12/12
   "Jiangsu Xinsheng Intelligent Technology"          , // 12/13
   "MLOONG"                                           , // 12/14
   "Quadratica LLC"                                   , // 12/15
   "Anpec Electronics"                                , // 12/16
   "Xi'an Morebeck Semiconductor Tech Co"             , // 12/17
   "Kingbank Technology Co Ltd"                       , // 12/18
   "ITRenew Inc"                                      , // 12/19
   "Shenzhen Eaget Innovation Tech Ltd"               , // 12/20
   "Jazer"                                            , // 12/21
   "Xiamen Semiconductor Investment Group"            , // 12/22
   "Guangzhou Longdao Network Tech Co"                , // 12/23
   "Shenzhen Futian SEC Electronic Market"            , // 12/24
   "Allegro Microsystems LLC"                         , // 12/25
   "Hunan RunCore Innovation Technology"              , // 12/26
   "C-Corsa Technology"                               , // 12/27
   "Zhuhai Chuangfeixin Technology Co Ltd"            , // 12/28
   "Beijing InnoMem Technologies Co Ltd"              , // 12/29
   "YooTin"                                           , // 12/30
   "Shenzhen Pengxiong Technology Co Ltd"             , // 12/31
   "Dongguan Yingbang Commercial Trading Co"          , // 12/32
   "Shenzhen Ronisys Electronics Co Ltd"              , // 12/33
   "Hongkong Xinlan Guangke Co Ltd"                   , // 12/34
   "Apex Microelectronics Co Ltd"                     , // 12/35
   "Beijing Hongda Jinming Technology Co Ltd"         , // 12/36
   "Ling Rui Technology (Shenzhen) Co Ltd"            , // 12/37
   "Hongkong Hyunion Electronics Co Ltd"              , // 12/38
   "Starsystems Inc"                                  , // 12/39
   "Shenzhen Yingjiaxun Industrial Co Ltd"            , // 12/40
   "Dongguan Crown Code Electronic Commerce"          , // 12/41
   "Monolithic Power Systems Inc"                     , // 12/42
   "WuHan SenNaiBo E-Commerce Co Ltd"                 , // 12/43
   "Hangzhou Hikstorage Technology Co"                , // 12/44
   "Shenzhen Goodix Technology Co Ltd"                , // 12/45
   "Aigo Electronic Technology Co Ltd"                , // 12/46
   "Hefei Konsemi Storage Technology Co Ltd"          , // 12/47
   "Cactus Technologies Limited"                      , // 12/48
   "DSIN"                                             , // 12/49
   "Blu Wireless Technology"                          , // 12/50
   "Nanjing UCUN Technology Inc"                      , // 12/51
   "Acacia Communications"                            , // 12/52
   "Beijinjinshengyihe Technology Co Ltd"             , // 12/53
   "Zyzyx"                                            , // 12/54
   "T-HEAD Semiconductor Co Ltd"                      , // 12/55
   "Shenzhen Hystou Technology Co Ltd"                , // 12/56
   "Syzexion"                                         , // 12/57
   "Kembona"                                          , // 12/58
   "Qingdao Thunderobot Technology Co Ltd"            , // 12/59
   "Morse Micro"                                      , // 12/60
   "Shenzhen Envida  Technology Co Ltd"               , // 12/61
   "UDStore Solution Limited"                         , // 12/62
   "Shunlie"                                          , // 12/63
   "Shenzhen Xin Hong Rui Tech Ltd"                   , // 12/64
   "Shenzhen Yze Technology Co Ltd"                   , // 12/65
   "Shenzhen Huang Pu He Xin Technology"              , // 12/66
   "Xiamen Pengpai Microelectronics Co Ltd"           , // 12/67
   "JISHUN"                                           , // 12/68
   "Shenzhen WODPOSIT Technology Co"                  , // 12/69
   "Unistar"                                          , // 12/70
   "UNICORE Electronic (Suzhou) Co Ltd"               , // 12/71
   "Axonne Inc"                                       , // 12/72
   "Shenzhen SOVERECA Technology Co"                  , // 12/73
   "Dire Wolf"                                        , // 12/74
   "Whampoa Core Technology Co Ltd"                   , // 12/75
   "CSI Halbleiter GmbH"                              , // 12/76
   "ONE Semiconductor"                                , // 12/77
   "SimpleMachines Inc"                               , // 12/78
   "Shenzhen Chengyi Qingdian Electronic"             , // 12/79
   "Shenzhen Xinlianxin Network Technology"           , // 12/80
   "Vayyar Imaging Ltd"                               , // 12/81
   "Paisen Network Technology Co Ltd"                 , // 12/82
   "Shenzhen Fengwensi Technology Co Ltd"             , // 12/83
   "Caplink Technology Limited"                       , // 12/84
   "JJT Solution Co Ltd"                              , // 12/85
   "HOSIN Global Electronics Co Ltd"                  , // 12/86
   "Shenzhen KingDisk Century Technology"             , // 12/87
   "SOYO"                                             , // 12/88
   "DIT Technology Co Ltd"                            , // 12/89
   "iFound"                                           , // 12/90
   "Aril Computer Company"                            , // 12/91
   "ASUS"                                             , // 12/92
   "Shenzhen Ruiyingtong Technology Co"               , // 12/93
   "HANA Micron"                                      , // 12/94
   "RANSOR"                                           , // 12/95
   "Axiado Corporation"                               , // 12/96
   "Tesla Corporation"                                , // 12/97
   "Pingtouge (Shanghai) Semiconductor Co"            , // 12/98
   "S3Plus Technologies SA"                           , // 12/99
   "Integrated Silicon Solution Israel Ltd"           , // 12/100
   "GreenWaves Technologies"                          , // 12/101
   "NUVIA Inc"                                        , // 12/102
   "Guangzhou Shuvrwine Technology Co"                , // 12/103
   "Shenzhen Hangshun Chip Technology"                , // 12/104
   "Chengboliwei Electronic Business"                 , // 12/105
   "Kowin Memory Technology Co Ltd"                   , // 12/106
   "Euronet Technology Inc"                           , // 12/107
   "SCY"                                              , // 12/108
   "Shenzhen Xinhongyusheng Electrical"               , // 12/109
   "PICOCOM"                                          , // 12/110
   "Shenzhen Toooogo Memory Technology"               , // 12/111
   "VLSI Solution"                                    , // 12/112
   "Costar Electronics Inc"                           , // 12/113
   "Shenzhen Huatop Technology Co Ltd"                , // 12/114
   "Inspur Electronic Information Industry"           , // 12/115
   "Shenzhen Boyuan Computer Technology"              , // 12/116
   "Beijing Welldisk Electronics Co Ltd"              , // 12/117
   "Suzhou EP Semicon Co Ltd"                         , // 12/118
   "Zhejiang Dahua Memory Technology"                 , // 12/119
   "Virtu Financial"                                  , // 12/120
   "Datotek International Co Ltd"                     , // 12/121
   "Telecom and Microelectronics Industries"          , // 12/122
   "Echow Technology Ltd"                             , // 12/123
   "APEX-INFO"                                        , // 12/124
   "Yingpark"                                         , // 12/125
   "Shenzhen Bigway Tech Co Ltd"                        // 12/126
};

const char* JedecGroup13[] =
{
   "Beijing Haawking Technology Co Ltd"               , // 13/1
   "Open HW Group"                                    , // 13/2
   "JHICC"                                            , // 13/3
   "ncoder AG"                                        , // 13/4
   "ThinkTech Information Technology Co"              , // 13/5
   "Shenzhen Chixingzhe Technology Co Ltd"            , // 13/6
   "Biao Ram Technology Co Ltd"                       , // 13/7
   "Shenzhen Kaizhuoyue Electronics Co Ltd"           , // 13/8
   "Shenzhen YC Storage Technology Co Ltd"            , // 13/9
   "Shenzhen Chixingzhe Technology Co"                , // 13/10
   "Wink Semiconductor  (Shenzhen) Co Ltd"            , // 13/11
   "AISTOR"                                           , // 13/12
   "Palma Ceia SemiDesign"                            , // 13/13
   "EM Microelectronic-Marin SA"                      , // 13/14
   "Shenzhen Monarch Memory Technology"               , // 13/15
   "Reliance Memory Inc"                              , // 13/16
   "Jesis"                                            , // 13/17
   "Espressif Systems (Shanghai)  Co Ltd"             , // 13/18
   "Shenzhen Sati Smart Technology Co Ltd"            , // 13/19
   "NeuMem Co Ltd"                                    , // 13/20
   "Lifelong"                                         , // 13/21
   "Beijing Oitech Technology Co Ltd"                 , // 13/22
   "Groupe LDLC"                                      , // 13/23
   "Semidynamics Technology Services SLU"             , // 13/24
   "swordbill"                                        , // 13/25
   "YIREN"                                            , // 13/26
   "Shenzhen Yinxiang Technology Co Ltd"              , // 13/27
   "PoweV Electronic Technology Co Ltd"               , // 13/28
   "LEORICE"                                          , // 13/29
   "Waymo LLC"                                        , // 13/30
   "Ventana Micro Systems"                            , // 13/31
   "Hefei Guangxin Microelectronics Co Ltd"           , // 13/32
   "Shenzhen Sooner Industrial Co Ltd"                , // 13/33
   "Horizon Robotics"                                 , // 13/34
   "Tangem AG"                                        , // 13/35
   "FuturePath Technology (Shenzhen) Co"              , // 13/36
   "RC Module"                                        , // 13/37
   "Timetec International Inc"                        , // 13/38
   "ICMAX Technologies Co Limited"                    , // 13/39
   "Lynxi Technologies Ltd Co"                        , // 13/40
   "Guangzhou Taisupanke Computer Equipment"          , // 13/41
   "Ceremorphic Inc"                                  , // 13/42
   "Biwin Storage Technology Co Ltd"                  , // 13/43
   "Beijing ESWIN Computing Technology"               , // 13/44
   "WeForce Co Ltd"                                   , // 13/45
   "Shenzhen Fanxiang Information Technology"         , // 13/46
   "Unisoc"                                           , // 13/47
   "YingChu"                                          , // 13/48
   "GUANCUN"                                          , // 13/49
   "IPASON"                                           , // 13/50
   "Ayar Labs"                                        , // 13/51
   "Amazon"                                           , // 13/52
   "Shenzhen Xinxinshun Technology Co"                , // 13/53
   "Galois Inc"                                       , // 13/54
   "Ubilite Inc"                                      , // 13/55
   "Shenzhen Quanxing Technology Co Ltd"              , // 13/56
   "Group RZX Technology LTDA"                        , // 13/57
   "Yottac Technology (XI'AN) Cooperation"            , // 13/58
   "Shenzhen RuiRen Technology Co Ltd"                , // 13/59
   "Group Star Technology Co Ltd"                     , // 13/60
   "RWA (Hong Kong) Ltd"                              , // 13/61
   "Genesys Logic Inc"                                , // 13/62
   "T3 Robotics Inc."                                 , // 13/63
   "Biostar Microtech International Corp"             , // 13/64
   "Shenzhen SXmicro Technology Co Ltd"               , // 13/65
   "Shanghai Yili Computer Technology Co"             , // 13/66
   "Zhixin Semicoducotor Co Ltd"                      , // 13/67
   "uFound"                                           , // 13/68
   "Aigo Data Security Technology Co. Ltd"            , // 13/69
   ".GXore Technologies"                              , // 13/70
   "Shenzhen Pradeon Intelligent Technology"          , // 13/71
   "Power LSI"                                        , // 13/72
   "PRIME"                                            , // 13/73
   "Shenzhen Juyang Innovative Technology"            , // 13/74
   "CERVO"                                            , // 13/75
   "SiEngine Technology Co., Ltd."                    , // 13/76
   "Beijing Unigroup Tsingteng MicroSystem"           , // 13/77
   "Brainsao GmbH"                                    , // 13/78
   "Credo Technology Group Ltd"                       , // 13/79
   "Shanghai Biren Technology Co Ltd"                 , // 13/80
   "Nucleu Semiconductor"                             , // 13/81
   "Shenzhen Guangshuo Electronics Co Ltd"            , // 13/82
   "ZhongsihangTechnology Co Ltd"                     , // 13/83
   "Suzhou Mainshine Electronic Co Ltd."              , // 13/84
   "Guangzhou Riss Electronic Technology"             , // 13/85
   "Shenzhen Cloud Security Storage  Co"              , // 13/86
   "ROG"                                              , // 13/87
   "Perceive"                                         , // 13/88
   "e-peas"                                           , // 13/89
   "Fraunhofer IPMS"                                  , // 13/90
   "Shenzhen Daxinlang Electronic Tech Co"            , // 13/91
   "Abacus Peripherals Private Limited"               , // 13/92
   "OLOy Technology"                                  , // 13/93
   "Wuhan P&S Semiconductor Co Ltd"                   , // 13/94
   "Sitrus Technology"                                , // 13/95
   "AnHui Conner Storage Co Ltd"                      , // 13/96
   "Rochester Electronics"                            , // 13/97
   "Wuxi Petabyte Technologies Co Ltd"                , // 13/98
   "Star Memory"                                      , // 13/99
   "Agile Memory Technology Co Ltd"                   , // 13/100
   "MEJEC"                                            , // 13/101
   "Rockchip Electronics Co Ltd"                      , // 13/102
   "Dongguan Guanma e-commerce Co Ltd"                , // 13/103
   "Rayson Hi-Tech (SZ) Limited"                      , // 13/104
   "MINRES Technologies GmbH"                         , // 13/105
   "Himax Technologies Inc"                           , // 13/106
   "Shenzhen Cwinner Technology Co Ltd"               , // 13/107
   "Tecmiyo"                                          , // 13/108
   "Shenzhen Suhuicun Technology Co Ltd"              , // 13/109
   "Vickter Electronics Co. Ltd."                     , // 13/110
   "lowRISC"                                          , // 13/111
   "EXEGate FZE"                                      , // 13/112
   "Shenzhen 9 Chapter Technologies Co"               , // 13/113
   "Addlink"                                          , // 13/114
   "Starsway"                                         , // 13/115
   "Pensando Systems Inc."                            , // 13/116
   "AirDisk"                                          , // 13/117
   "Shenzhen Speedmobile Technology Co"               , // 13/118
   "PEZY Computing"                                   , // 13/119
   "Extreme Engineering Solutions Inc"                , // 13/120
   "Shangxin Technology Co Ltd"                       , // 13/121
   "Shanghai Zhaoxin Semiconductor Co"                , // 13/122
   "Xsight Labs Ltd"                                  , // 13/123
   "Hangzhou Hikstorage Technology Co"                , // 13/124
   "Dell Technologies"                                , // 13/125
   "Guangdong StarFive Technology Co"                   // 13/126
};

const char* JedecGroup14[] =
{
   "TECOTON"                                          , // 14/1
   "Abko Co Ltd"                                      , // 14/2
   "Shenzhen Feisrike Technology Co Ltd"              , // 14/3
   "Shenzhen Sunhome Electronics Co Ltd"              , // 14/4
   "Global Mixed-mode Technology Inc"                 , // 14/5
   "Shenzhen Weien Electronics Co. Ltd."              , // 14/6
   "Shenzhen Cooyes Technology Co Ltd"                , // 14/7
   "Keymos Electronics Co., Limited"                  , // 14/8
   "E-Rockic Technology Company Limited"              , // 14/9
   "Aerospace Science Memory Shenzhen"                , // 14/10
   "Shenzhen Quanji Technology Co Ltd"                , // 14/11
   "Dukosi"                                           , // 14/12
   "Maxell Corporation of America"                    , // 14/13
   "Shenshen Xinxintao Electronics Co Ltd"            , // 14/14
   "Zhuhai Sanxia Semiconductor Co Ltd"               , // 14/15
   "Groq Inc"                                         , // 14/16
   "AstraTek"                                         , // 14/17
   "Shenzhen Xinyuze Technology  Co Ltd"              , // 14/18
   "All Bit Semiconductor"                            , // 14/19
   "ACFlow"                                           , // 14/20
   "Shenzhen Sipeed Technology Co Ltd"                , // 14/21
   "Linzhi Hong Kong Co Limited"                      , // 14/22
   "Supreme Wise Limited"                             , // 14/23
   "Blue Cheetah Analog Design Inc"                   , // 14/24
   "Hefei Laiku Technology Co Ltd"                    , // 14/25
   "Zord"                                             , // 14/26
   "SBO Hearing A/S"                                  , // 14/27
   "Regent Sharp International Limited"               , // 14/28
   "Permanent Potential Limited"                      , // 14/29
   "Creative World International Limited"             , // 14/30
   "Base Creation International Limited"              , // 14/31
   "Shenzhen Zhixin Chuanglian Technology"            , // 14/32
   "Protected Logic Corporation"                      , // 14/33
   "Sabrent"                                          , // 14/34
   "Union Memory"                                     , // 14/35
   "NEUCHIPS Corporation"                             , // 14/36
   "Ingenic Semiconductor Co Ltd"                     , // 14/37
   "SiPearl"                                          , // 14/38
   "Shenzhen Actseno Information Technology"          , // 14/39
   "RIVAI Technologies (Shenzhen) Co Ltd"             , // 14/40
   "Shenzhen Sunny Technology Co Ltd"                 , // 14/41
   "Cott Electronics Ltd"                             , // 14/42
   "Shanghai Synsense Technologies Co Ltd"            , // 14/43
   "Shenzhen Jintang Fuming Optoelectronics"          , // 14/44
   "CloudBEAR LLC"                                    , // 14/45
   "Emzior, LLC"                                      , // 14/46
   "Ehiway Microelectronic Science Tech Co"           , // 14/47
   "UNIM Innovation Technology (Wu XI)"               , // 14/48
   "GDRAMARS"                                         , // 14/49
   "Meminsights Technology"                           , // 14/50
   "Zhuzhou Hongda Electronics Corp Ltd"              , // 14/51
   "Luminous Computing Inc"                           , // 14/52
   "PROXMEM"                                          , // 14/53
   "Draper Labs"                                      , // 14/54
   "ORICO  Technologies Co. Ltd."                     , // 14/55
   "Space Exploration Technologies Corp"              , // 14/56
   "AONDEVICES Inc"                                     // 14/57
};

const char** JEDEC_GROUPS[] =
{
    JedecGroup1,
    JedecGroup2,
    JedecGroup3,
    JedecGroup4,
    JedecGroup5,
    JedecGroup6,
    JedecGroup7,
    JedecGroup8,
    JedecGroup9,
    JedecGroup10,
    JedecGroup11,
    JedecGroup12,
    JedecGroup13,
    JedecGroup14
};

const int JEDEC_LIMITS[] =
{
    sizeof(JedecGroup1) / sizeof(char*),
    sizeof(JedecGroup2) / sizeof(char*),
    sizeof(JedecGroup3) / sizeof(char*),
    sizeof(JedecGroup4) / sizeof(char*),
    sizeof(JedecGroup5) / sizeof(char*),
    sizeof(JedecGroup6) / sizeof(char*),
    sizeof(JedecGroup7) / sizeof(char*),
    sizeof(JedecGroup8) / sizeof(char*),
    sizeof(JedecGroup9) / sizeof(char*),
    sizeof(JedecGroup10) / sizeof(char*),
    sizeof(JedecGroup11) / sizeof(char*),
    sizeof(JedecGroup12) / sizeof(char*),
    sizeof(JedecGroup13) / sizeof(char*),
    sizeof(JedecGroup14) / sizeof(char*)
};

#define SEQUENCE_LIMIT 8

const char* jedecDecoderSequental(unsigned char* bytes)
{
    const char* name = "[UNKNOWN VENDOR]";
    for (int i = 0; i < SEQUENCE_LIMIT; i++)
    {
        int id = bytes[i] & 0x7F;
        if (id != 0x7F)
        {
            name = jedecDecoderIndexed(i, id);
            break;
        }
    }
    return name;
}

const char* jedecDecoderIndexed(int group, int index)
{
    const char* name = "[UNKNOWN VENDOR]";
    int groupLimit = sizeof(JEDEC_GROUPS) / sizeof(char**);
    group = (group & 0x7F);
    index = (index & 0x7F) - 1;
    if ((group >= 0) && (group < groupLimit))
    {
        const char** groupArray = JEDEC_GROUPS[group];
        int indexLimit = JEDEC_LIMITS[group];
        if ((index >= 0) && (index < indexLimit))
        {
            name = groupArray[index];
        }
    }
    return name;
}

