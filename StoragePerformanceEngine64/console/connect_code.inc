;------------------------------------------------------------------------------;
;                         Console support library:                             ;
;             connect include files with globally used subroutines.            ;
;------------------------------------------------------------------------------;

include 'consoleread.inc'      ; Console input
include 'consolewrite.inc'     ; Console output
include 'getcolor.inc'         ; Get console color, saved at start
include 'setcolor.inc'         ; Set console color, for next outputs
include 'setfgcolor.inc'       ; Set console foreground color, background
                               ; color as saved at start, for next out.
include 'createreport.inc'     ; Open and Close report file, initialize access
include 'readscenario.inc'     ; Open, Read, Close scenario file
include 'parsescenario.inc'    ; Parse scenario file, extract and set options
include 'showscenario.inc'     ; Display current options settings by scenario
include 'showerror.inc'        ; Display details about detected error 

  