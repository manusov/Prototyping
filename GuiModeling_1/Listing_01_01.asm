;------------------------------------------------------------------------------;
;                           GUI window example.                                ; 
;          FASM x64 version of C++ Listing 1.1 from Litvinenko book.           ;
;                         ISBN 978-5-9775-0600-7.                              ;
;------------------------------------------------------------------------------;

include 'win64w.inc'  ; FASM definitions for UNICODE application type

format PE64 GUI
entry start
section '.code' code readable executable
start: 
sub rsp,8*5
cld
xor ecx,ecx
call [GetModuleHandle] 
test rax,rax
jz .exit
xchg rbx,rax
mov [hModule],rax 
xor ecx,ecx
mov edx,IDI_APPLICATION
call [LoadIcon]
test rax,rax
jz .exit
xchg rsi,rax
xor ecx,ecx
mov edx,IDC_ARROW
call [LoadCursor] 
test rax,rax
jz .exit
xchg rdi,rax
lea rcx,[wcex]
xor eax,eax
mov [rcx + WNDCLASSEX.cbSize]        , sizeof.WNDCLASSEX
mov [rcx + WNDCLASSEX.hInstance]     , rbx
mov [rcx + WNDCLASSEX.lpszClassName] , WinName
mov [rcx + WNDCLASSEX.lpfnWndProc]   , WndProc
mov [rcx + WNDCLASSEX.style]         , CS_HREDRAW + CS_VREDRAW 
mov [rcx + WNDCLASSEX.hIcon]         , rsi
mov [rcx + WNDCLASSEX.hIconSm]       , rsi
mov [rcx + WNDCLASSEX.hCursor]       , rdi
mov [rcx + WNDCLASSEX.lpszMenuName]  , rax
mov [rcx + WNDCLASSEX.cbClsExtra]    , eax
mov [rcx + WNDCLASSEX.cbWndExtra]    , eax
mov [rcx + WNDCLASSEX.hbrBackground] , COLOR_WINDOW + 1
call [RegisterClassEx]
test rax,rax
jz .exit
xor eax,eax
push rax
push rbx
push rax
push rax
; mov edx,CW_USEDEFAULT
; push rdx
; push rdx
; push rdx
; push rdx 
push 400 ; 300
push 500 ; 400
push 100 
push 100
;
  mov r9d,WS_OVERLAPPEDWINDOW + WS_VISIBLE
; WS_SIZEBOX - for changeable window size
; mov r9d,WS_VISIBLE + WS_DLGFRAME + WS_SYSMENU + WS_SIZEBOX ; WS_OVERLAPPEDWINDOW
; mov r9d,WS_VISIBLE + WS_DLGFRAME + WS_SYSMENU  ; WS_OVERLAPPEDWINDOW
; mov r9d,WS_OVERLAPPEDWINDOW
lea r8,[WinTitle]
lea rdx,[WinName]
xor ecx,ecx
sub rsp,32
call [CreateWindowEx]
add rsp,32 + 8 * 8
test rax,rax
jz .exit
mov rcx,rbx
mov edx,SW_NORMAL
call [ShowWindow]
lea rdi,[msg]
.waitMessage:
mov rcx,rdi
xor edx,edx
xor r8d,r8d
xor r9d,r9d
call [GetMessage]
test rax,rax		     
jz .exit
mov rcx,rdi
call [TranslateMessage]
mov rcx,rdi
call [DispatchMessage]
jmp .waitMessage
.exit:
xor ecx,ecx           
call [ExitProcess]

WndProc:
push rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32 
cmp edx,WM_DESTROY
je .wmdestroy
call [DefWindowProc]
jmp .done
.wmdestroy:
xor ecx,ecx
call [PostQuitMessage]
xor eax,eax
.done:
mov rsp,rbp
pop rbp
ret

section '.data' data readable writeable
WinName   DU  'MainFrame' , 0
WinTitle  DU  'Windows application template (extended window class)' , 0
align 8
hModule   DQ          ?
hWnd      DQ          ?
wcex      WNDCLASSEX  ?
msg       MSG         ? 

section '.idata' import data readable writeable
library \ 
kernel32 , 'kernel32.dll', \
user32   , 'user32.dll'
include    'api\kernel32.inc'
include    'api\user32.inc'
