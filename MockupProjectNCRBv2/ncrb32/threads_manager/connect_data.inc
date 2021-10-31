;------------------------------------------------------------------------------;
;                                                                              ;
;   Library for memory and threads management, include adjust measurements:    ;
;               constants and variables with pre-defined values.               ;
;                                                                              ;
;------------------------------------------------------------------------------;
;---------- Memory size constants ---------------------------------------------;
; Block size for DRAM benchmarks, default values 
; must be corrected by platform configuration checks
DEFAULT_DRAM_BLOCK      =  32 * 1024 * 1024
; Constants for custom block size restrictions
CUSTOM_BLOCK_MIN        =  16 * 1024
CUSTOM_BLOCK_MAX        =  64 * 1024 * 1024
CUSTOM_BLOCK_ALIGN      =  16 * 1024
;---------- Memory timings constants ------------------------------------------;
; 10000 milliseconds = 10 seconds, events wait time for
; communication with child thread
THREAD_TIME_LIMIT       =  10000  
; Benchmarks repeat parameters, precision=f(repeats), for Cache&RAM mode
; Calibrate iterations for objects, normal mode
; this constants must be multiple of LATENCY DIVISOR (50)
L1_REPEATS_FAST         =  100000
L2_REPEATS_FAST         =  50000
L3_REPEATS_FAST         =  1000
L4_REPEATS_FAST         =  200
DRAM_REPEATS_FAST       =  100
CUSTOM_REPEATS_FAST     =  100000
; Calibrate iterations for objects, slow mode
; this constants must be multiple of LATENCY DIVISOR (50)
L1_REPEATS_SLOW         =  2000000
L2_REPEATS_SLOW         =  500000
L3_REPEATS_SLOW         =  10000
L4_REPEATS_SLOW         =  2000
DRAM_REPEATS_SLOW       =  200
CUSTOM_REPEATS_SLOW     =  1000000
; Repeats options definitions: 
; objects entries count (L1-L4, DRAM, Custom),
; bytes per entry (dword=4 bytes), 
; list size
REPEAT_OBJECTS          =  6
REPEAT_ENTRY_BYTES      =  4
REPEAT_SERIES_BYTES     =  REPEAT_OBJECTS * REPEAT_ENTRY_BYTES    
; Constants for adaptive measurement mode, target measurement time, seconds,
; for fast and slow (precision) modes,
; adaptive logic criteria is:
; number of iterations must be adjusted for measurement time equal this time
ADAPTIVE_FAST           =  0.01
ADAPTIVE_SLOW           =  0.05
;------------------------------------------------------------------------------;
;     Benchmarks calibration repeats select table and time intervals table.    ;
;------------------------------------------------------------------------------;
; Number of measurement iterations N = F ( target object , careful mode )
; Target Objects: L1, L2, L3, L4 caches, DRAM, Storage, Custom (different sizes)
; Careful mode: Fast(inaccurate), Slow(precision)
BenchmarksRepeats:
; Number of measurement iterations for objects, for fast (inaccurate) mode,
; for Cache, RAM, Custom modes
; Group must be 24 bytes size for indexed access, 6 dwords
DD  L1_REPEATS_FAST , L2_REPEATS_FAST , L3_REPEATS_FAST , L4_REPEATS_FAST
DD  DRAM_REPEATS_FAST , CUSTOM_REPEATS_FAST
; Number of measurement iterations for objects, for slow (precision) mode,
; for Cache, RAM, Custom modes
; Group must be 24 bytes size for indexed access, 6 dwords
DD  L1_REPEATS_SLOW , L2_REPEATS_SLOW , L3_REPEATS_SLOW , L4_REPEATS_SLOW
DD  DRAM_REPEATS_SLOW , CUSTOM_REPEATS_SLOW
; Constants for adaptive measurement mode, target measurement time, seconds
; double floating point constants
AdaptiveRepeats:
DQ  ADAPTIVE_FAST
DQ  ADAPTIVE_SLOW
