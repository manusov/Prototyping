;------------------------------------------------------------------------------;
; OpenGL programming examples. x64 version.                                    ;
;                                                                              ;
;                             UNDER CONSTRUCTION.                              ;
;                     Benchmark "Politburo" design step 13.4.                  ;
;          Heavy load with CPU-GPU lite traffic. FPS and GBPS measure.         ;
;                                                                              ;
; See also:                                                                    ;
; https://github.com/manusov                                                   ;
; https://github.com/manusov/Prototyping/tree/master/OpenGL                    ;
;                                                                              ;
; Special thanks:                                                              ;
; https://flatassembler.net/                                                   ;
; https://board.flatassembler.net/topic.php?t=15453                            ;
; https://ravesli.com/uroki-po-opengl/                                         ;
; https://www.manhunter.ru/assembler/923_vivod_izobrazheniya_na_assemblere_s_pomoschyu_gdi.html
; https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmap  ;
;------------------------------------------------------------------------------;

include 'win64a.inc'
include 'OpenGL.inc'

APPLICATION_NAME   EQU 'OpenGL with shaders. Engineering sample #14.0.x64.'

X_BASE             EQU 380     ; GUI window positions at application start.
Y_BASE             EQU 140
X_SIZE             EQU 1024    ; GUI window sizes at application start.
Y_SIZE             EQU 768

ID_EXE_ICON        EQU 101     ; Application exe file icon resource IDs.
ID_EXE_ICONS       EQU 102
IDR_JPEG           EQU 103     ; JPG file resource ID, used as texture. 

TEMP_BUFFER_SIZE   EQU 4096    ; Transit buffer(s) default size.

GL_VERTEX_SHADER   EQU 00008B31h   ; OpenGL API control codes
GL_FRAGMENT_SHADER EQU 00008B30h
GL_COMPILE_STATUS  EQU 00008B81h
GL_LINK_STATUS     EQU 00008B82h
GL_ARRAY_BUFFER    EQU 00008892h
GL_STATIC_DRAW     EQU 000088E4h
GL_DYNAMIC_DRAW    EQU 000088E8h
GL_TEXTURE0        EQU 000084C0h

GL_SHADING_LANGUAGE_VERSION EQU 00008B8Ch

TEXTURE_WIDTH      EQU 2952         ; Texture JPG file X, Y sizes,
TEXTURE_HEIGHT     EQU 1967         ; shaders update required if this changed 
TEXT_FRONT_COLOR   EQU 0FF267190h   ; Text output front and back colors  
TEXT_BACK_COLOR    EQU 0FFF8F8F8h
BACKGROUND_R       EQU 0.80         ; Render window background R,G,B as float
BACKGROUND_G       EQU 0.95
BACKGROUND_B       EQU 0.95

;--- This constant IMPORTANT for GPU load ---
; 128 x 4 = 512 chars positions matrix for text output.
; 9 x 3   = 27 portraits.
; + 500000 instances for GPU heavy load. 
; Change this last addend value for change GPU load.
INSTANCING_COUNT   EQU 128 * 4 + 27 + 500000  

struct GdiplusStartupInput          ; Structure for GDI+ support,
GdiplusVersion            dd ?      ; GDI+ required for load JPEG packed image
pad1                      dd ?
DebugEventCallback        dq ?
SuppressBackgroundThread  dd ?
SuppressExternalCodecs    dd ?
ends

;---------- Code section ------------------------------------------------------;

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
sub rsp,8                          ; Make stack dqword (16 byte) aligned
;--- Get handle of this application exe file ----------------------------------;
xor ecx,ecx                        ; RCX = Parm#1 = 0 = means this exe file 
call [GetModuleHandle]             ; Get handle of this exe file
test rax,rax
jz .iconFailed                     ; Go if this module handle = NULL 
;--- Get handle of this application icon --------------------------------------;
mov edx,ID_EXE_ICONS               ; RDX = Parm#2 = Resource ID
xchg rcx,rax                       ; RCX = Parm#1 = Module handle for resource 
call [LoadIcon]                    ; Load application icon, from this exe file
test rax,rax
jz .iconFailed                     ; Go if load error, icon handle = NULL
mov [hIcon],rax                    ; Store handle of application icon
.iconFailed:
;--- GDI+ initialization ------------------------------------------------------;
lea rdx,[startupInput]
xor eax,eax
mov [rdx + GdiplusStartupInput.GdiplusVersion],1
mov [rdx + GdiplusStartupInput.DebugEventCallback],rax
mov [rdx + GdiplusStartupInput.SuppressBackgroundThread],eax
mov [rdx + GdiplusStartupInput.SuppressExternalCodecs],eax
lea rcx,[gdiplusToken]
xor r8,r8
call [GdiplusStartup] 
;--- Access JPG file as application exe file resource -------------------------;
xor ecx,ecx
call [GetModuleHandleA]
test rax,rax
jz .skipJpgLoader
xchg rbx,rax            ; RBX = hMod
mov rcx,rbx
mov edx,IDR_JPEG
mov r8d,RT_RCDATA
call [FindResourceA]
test rax,rax
jz .skipJpgLoader
xchg rsi,rax            ; RSI = hrsrc 
mov rcx,rbx
mov rdx,rsi
call [SizeofResource]
test rax,rax
jz .skipJpgLoader
xchg rdi,rax            ; RDI = dwResourceSize
mov rcx,rbx
mov rdx,rsi
call [LoadResource]
test rax,rax
jz .skipJpgLoader
xchg rcx,rax
call [LockResource]
test rax,rax
jz .skipJpgLoader
;--- Locate JPG file as global memory object ----------------------------------; 
xchg rbx,rax            ; RBX = Pointer to loaded JPG file
mov ecx,GHND
mov rdx,rdi
call [GlobalAlloc]
test rax,rax
jz .skipJpgLoader
xchg rsi,rax            ; RSI = hGlobal
mov [hGlobal],rsi 
mov rcx,rsi
call [GlobalLock]
test rax,rax
jz .skipJpgLoader
push rsi
mov ecx,edi             ; ECX = Size, bytes, TODO. Optimize QWORDS + Tail
mov rsi,rbx             ; RSI = Source for JPG file image copy 
mov rdi,rax             ; RDI = Destination for JPG file image copy
cld
rep movsb
pop rsi
;--- Stream manipulations and extract raw image bitmap from JPG packed data ---; 
mov rcx,rsi
mov edx,1
lea rsi,[pStream]
mov r8,rsi
call [CreateStreamOnHGlobal]
test eax,eax
jnz .skipJpgLoader 
mov rcx,[rsi]
lea rdi,[gpBitmap]
mov rdx,rdi
call [GdipCreateBitmapFromStream]
test eax,eax
jnz .skipJpgLoader 
mov rcx,[rdi]
lea rbx,[hBitmap]
mov rdx,rbx
mov r8d,00FFFFFFh
call [GdipCreateHBITMAPFromBitmap]
test eax,eax
jnz .skipJpgLoader 
mov rcx,[rbx]
jrcxz .skipJpgLoader
;--- Get BITMAP structure as GDI object ---------------------------------------;
mov edx,sizeof.BITMAP
lea rsi,[bm]
mov r8,rsi
call [GetObjectA]
test rax,rax
jz .skipJpgLoader
;--- Store final result = pointer to raw image data ---------------------------;
mov rbx,[rsi +  BITMAP.bmBits]  ; Pointer to raw data, TODO bmWidth, bmHeight.
mov [pRawImage],rbx
.skipJpgLoader:
;--- Blank font unpack area, note part of image used for font build -----------;
mov rdi,rbx
mov ecx,TEXTURE_WIDTH * 16 
mov eax,TEXT_BACK_COLOR
cld
rep stosd
;--- Font unpack, build chars at graphics image -------------------------------;
lea rsi,[rasterFont]
mov rdi,rbx
mov dh,128
.unpackFont:       ; Cycle for unpack font
push rdi
add rdi,TEXTURE_WIDTH * 4 * 15
mov ch,16
.unpackChar:       ; Cycle for unpack each char of font
mov cl,8
mov dl,[rsi]
inc rsi
.unpackByte:       ; Cycle for unpack each 8-pixel line of each char
shl dl,1
mov eax,TEXT_BACK_COLOR
jnc @f
mov eax,TEXT_FRONT_COLOR
@@:
stosd
dec cl
jnz .unpackByte    ; Cycle for unpack each 8-pixel line of each char
sub rdi,(TEXTURE_WIDTH * 4) + (8 * 4)
dec ch
jnz .unpackChar    ; Cycle for unpack each char of font
pop rdi
add rdi,8 * 4
dec dh
jnz .unpackFont    ; Cycle for unpack font 
;--- GUI objects initialization and register GUI window class -----------------; 
lea rbx,[wc]
xor ecx,ecx
call[GetModuleHandle]
mov	[rbx + WNDCLASS.hInstance],rax
xor ecx,ecx
mov edx,IDI_APPLICATION
call [LoadIcon]
mov [rbx + WNDCLASS.hIcon],rax
xor ecx,ecx
mov edx,IDC_ARROW
call [LoadCursor]
mov [rbx + WNDCLASS.hCursor],rax
mov rcx,rbx
call [RegisterClass]
;--- Create GUI window --------------------------------------------------------;
xor eax,eax  ; RAX = 0 for compact PUSH 0
push rax
push [rbx + WNDCLASS.hInstance]
push rax
push rax
push Y_SIZE
push X_SIZE
push Y_BASE
push X_BASE
mov r9d,WS_VISIBLE + WS_OVERLAPPEDWINDOW + WS_CLIPCHILDREN + WS_CLIPSIBLINGS
lea r8,[appName]
lea rdx,[appClass]
xor ecx,ecx
sub rsp,32              ; Re-create parameters shadow after pushes
call [CreateWindowEx]
add rsp,32 + 64         ; Remove parameters shadow and 8 pushes (input parms) 
test rax,rax            ; Check for RAX = 0
jz ApplicationExit  ; Go if initialization error by WM_CREATE returns RAX = -1
;--- Handling events from GUI window ------------------------------------------;
lea rbx,[msg]
msgLoop:
mov rcx,rbx
xor edx,edx
xor r8,r8
xor r9,r9
call [GetMessage]
cmp eax,1
jb endLoop
jne msgLoop
mov rcx,rbx
call [TranslateMessage]
mov rcx,rbx
call [DispatchMessage]
jmp msgLoop
endLoop:
;--- Application context de-initializing --------------------------------------; 
ApplicationExit:
mov rax,[pStream]
mov rcx,rax
jrcxz @f
mov rax,[rax]
call qword [rax + 10h]  ; This means pStream->Release();
@@:
mov rcx,[hGlobal]
jrcxz @f
call [GlobalFree]
@@:
mov rcx,[pBitmapStruc]
jrcxz @f
call [GdipDisposeImage]
@@:
mov rcx,[hBitmap]
jrcxz @f
call [DeleteObject]
@@:
mov rcx,[gdiplusToken]
jrcxz @f
call [GdiplusShutdown]
@@:
;--- Application exit ---------------------------------------------------------;
xor ecx,ecx          ; Exit code = 0
call [ExitProcess]

;---------- Window callback procedure -----------------------------------------;

WindowProc:
push rbx rsi rdi rbp r15  ; note about other non-volatile regs, include XMM6-15
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h     ; Align stack
sub rsp,32                     ; Create parameters shadow
mov rbx,rcx                    ; RBX = HWND
lea r15,[OGL_API]              ; R15 = Pointer for compact call OpenGL API

;--- Select message handler ---------------------------------------------------;

cmp edx,WM_CREATE
je .wmcreate          ; Go window create procedure
cmp edx,WM_SIZE
je .wmsize            ; Go window re-size procedure
cmp edx,WM_PAINT
je .wmpaint           ; Go graphics paint procedure
cmp edx,WM_KEYDOWN
je .wmkeydown         ; Go key press handler, here used for ESC
cmp edx,WM_DESTROY
je .wmdestroy         ; Go window destroy procedure

;--- Default window procedure -------------------------------------------------;

.defwndproc:
call [DefWindowProc]  ; rcx, rdx, r8, r9 must be valid input at this point
jmp	.finish

;--- Window message handler : create window procedure -------------------------;

.wmcreate:
;--- Initialize display context -----------------------------------------------;
call [GetDC]           ; rcx = input
mov [r15 - sizeof.APPDATA + APPDATA.hdc],rax
;--- Start initialize OpenGL context ------------------------------------------;
lea rdi,[pfd]
mov rsi,rdi            ; rsi = pointer to pfd
mov	ecx,sizeof.PIXELFORMATDESCRIPTOR shr 3
xor	eax,eax
cld
rep stosq
mov [rsi + PIXELFORMATDESCRIPTOR.nSize],sizeof.PIXELFORMATDESCRIPTOR
mov [rsi + PIXELFORMATDESCRIPTOR.nVersion],1
mov [rsi + PIXELFORMATDESCRIPTOR.dwFlags],PFD_SUPPORT_OPENGL + PFD_DOUBLEBUFFER + PFD_DRAW_TO_WINDOW
mov [rsi + PIXELFORMATDESCRIPTOR.iLayerType],PFD_MAIN_PLANE
mov [rsi + PIXELFORMATDESCRIPTOR.iPixelType],PFD_TYPE_RGBA
mov [rsi + PIXELFORMATDESCRIPTOR.cColorBits],16
mov [rsi + PIXELFORMATDESCRIPTOR.cDepthBits],16
mov [rsi + PIXELFORMATDESCRIPTOR.cAccumBits],0
mov [rsi + PIXELFORMATDESCRIPTOR.cStencilBits],0
mov rdi,[r15 - sizeof.APPDATA + APPDATA.hdc]            ; rdi = hdc
mov rcx,rdi
mov rdx,rsi
call [ChoosePixelFormat] 
mov rcx,rdi
xchg edx,eax            ; in the current context XCHG compact than MOV
mov r8,rsi
call [SetPixelFormat]
mov rcx,rdi
call [wglCreateContext]
mov [r15 - sizeof.APPDATA + APPDATA.hrc],rax
mov rcx,rdi
xchg rdx,rax            ; in the current context XCHG compact than MOV
call [wglMakeCurrent]	
lea rdi,[r15 - sizeof.APPDATA + APPDATA.rc]    ; rdi = pointer to rc
mov rcx,rbx
mov rdx,rdi
call [GetClientRect]
xor ecx,ecx
xor edx,edx
mov r8d,[rdi + RECT.right]
mov r9d,[rdi + RECT.bottom]
call [glViewport]
;--- Set main window icon -----------------------------------------------------; 
mov r9,[hIcon]                       ; R9  = Parm#4 = LPARAM = Icon handle 
mov r8d,ICON_SMALL                   ; R8  = Parm#3 = WPARAM = Icon type 
mov edx,WM_SETICON                   ; RDX = Parm#2 = Message 
mov rcx,rbx                          ; RCX = Parm#1 = Window handle
call [SendMessageA]                  ; Set main window icon
;--- Get and build system information strings ---------------------------------;
lea rdi,[tempBuffer2]
mov ecx,128
mov eax,'    '
cld
rep stosd
lea rsi,[listStrings]
lea rdi,[tempBuffer2 + 128 * 3 + 1]
.nxString:
lodsd
xchg ecx,eax
jrcxz .strDone
push rsi rdi
sub rsp,32            ; Re-create parameters shadow required because pushes
call [glGetString]    ; Get information string, selected by input RCX
add rsp,32
test rax,rax
jz .skString
xchg rsi,rax
call StringWrite
.skString:
pop rdi rsi
sub rdi,128
jmp .nxString 
.strDone:
;--- Build template for FPS strings group -------------------------------------;
lea rsi,[szSeconds]
lea rdi,[tempBuffer2 + 128 * 3 + 60]
call StringWrite
lea rsi,[szFrames]
lea rdi,[tempBuffer2 + 128 * 2 + 60]
call StringWrite
lea rsi,[szFPSavg]
lea rdi,[tempBuffer2 + 128 * 1 + 60]
call StringWrite
lea rsi,[szFPScur]
lea rdi,[tempBuffer2 + 128 * 0 + 60]
call StringWrite
;--- Build template for bus traffic strings group -----------------------------;
lea rsi,[szBusSeconds]
lea rdi,[tempBuffer2 + 128 * 3 + 90]
call StringWrite
lea rsi,[szBusMB]
lea rdi,[tempBuffer2 + 128 * 2 + 90]
call StringWrite
lea rsi,[szBusMBPSavg]
lea rdi,[tempBuffer2 + 128 * 1 + 90]
call StringWrite
lea rsi,[szBusMBPScur]
lea rdi,[tempBuffer2 + 128 * 0 + 90]
call StringWrite
;--- Dynamically load required OpenGL API procedures --------------------------;
cld
lea rsi,[oglNamesList]  ; RSI = Pointer to functions names sequence
mov rdi,r15             ; RDI = Pointer to loaded functions pointers list
.loadCycle:
cmp byte [rsi],0
je .loadOk              ; Go if strings list done, second 0
mov rcx,rsi
call [wglGetProcAddress]
test rax,rax
jz .loadFailed
stosq
.skipName:              ; Skip this function name string for access next string
lodsb
cmp al,0
jne .skipName
jmp .loadCycle
.loadOk:
;--- Compile vertex shader ----------------------------------------------------;
mov ecx,GL_VERTEX_SHADER
call [r15 + OGLAPI.ptr_glCreateShader]
test eax,eax
jz .shaderFailed
mov [r15 - sizeof.APPDATA + APPDATA.vertexShader],eax
xchg ebx,eax
mov ecx,ebx
mov edx,1
lea r8,[ptr_vertexShaderSource]
xor r9,r9
call [r15 + OGLAPI.ptr_glShaderSource]   ; Set vertex shader source C++ code
mov ecx,ebx
call [r15 + OGLAPI.ptr_glCompileShader]  ; Compile C++ shader to GPU executable
mov ecx,ebx
mov edx,GL_COMPILE_STATUS
lea rsi,[r15 - sizeof.APPDATA + APPDATA.params]
mov r8,rsi
mov dword [rsi],0
call [r15 + OGLAPI.ptr_glGetShaderiv]
cmp dword [rsi],0
je .compileFailed 
;--- Compile fragment shader --------------------------------------------------;
mov ecx,GL_FRAGMENT_SHADER
call [r15 + OGLAPI.ptr_glCreateShader]
test eax,eax
jz .shaderFailed
mov [r15 - sizeof.APPDATA + APPDATA.fragmentShader],eax
xchg ebx,eax
mov ecx,ebx
mov edx,1
lea r8,[ptr_fragmentShaderSource]
xor r9,r9
call [r15 + OGLAPI.ptr_glShaderSource]   ; Set fragment shader source C++ code
mov ecx,ebx
call [r15 + OGLAPI.ptr_glCompileShader]  ; Compile C++ shader to GPU executable
mov ecx,ebx
mov edx,GL_COMPILE_STATUS
mov r8,rsi
mov dword [rsi],0
call [r15 + OGLAPI.ptr_glGetShaderiv]
cmp dword [rsi],0
je .compileFailed 
;--- Link shaders into shader program = GPU-executable program ----------------;
call [r15 + OGLAPI.ptr_glCreateProgram]
test eax,eax
jz .shaderFailed
mov [r15 - sizeof.APPDATA + APPDATA.shaderProgram],eax
xchg ebx,eax
mov ecx,ebx
mov edx,[r15 - sizeof.APPDATA + APPDATA.vertexShader]
call [r15 + OGLAPI.ptr_glAttachShader]   ; Attach vertex shader
mov ecx,ebx
mov edx,[r15 - sizeof.APPDATA + APPDATA.fragmentShader]
call [r15 + OGLAPI.ptr_glAttachShader]   ; Attach fragment shader
mov ecx,ebx
call [r15 + OGLAPI.ptr_glLinkProgram]    ; Link shader program
mov ecx,ebx
mov edx,GL_LINK_STATUS
mov r8,rsi
mov dword [rsi],0
call [r15 + OGLAPI.ptr_glGetProgramiv]
cmp dword [rsi],0
je .linkFailed 
;--- Delete shaders after used by shader program linker -----------------------;
mov ecx,[r15 - sizeof.APPDATA + APPDATA.vertexShader]
call [r15 + OGLAPI.ptr_glDeleteShader]
mov ecx,[r15 - sizeof.APPDATA + APPDATA.fragmentShader]
call [r15 + OGLAPI.ptr_glDeleteShader]
;--- Create and bind arrays ---------------------------------------------------;
lea rbx,[r15 - sizeof.APPDATA + APPDATA.VAO]
mov ecx,1
mov rdx,rbx
mov dword [rbx],0
call [r15 + OGLAPI.ptr_glGenVertexArrays]
cmp dword [rbx],0
je .shaderFailed
mov ecx,1
lea rdx,[rbx + 4]
mov dword [rbx + 4],0
call [r15 + OGLAPI.ptr_glGenBuffers]
cmp dword [rbx + 4],0
je .shaderFailed
mov ecx,[rbx]
call [r15 + OGLAPI.ptr_glBindVertexArray]
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,GL_ARRAY_BUFFER
mov edx,[rbx + 4]
call [r15 + OGLAPI.ptr_glBindBuffer]
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Set vertices data --------------------------------------------------------;
mov ecx,GL_ARRAY_BUFFER
mov edx,6 * 6 * 5 * 4
lea r8,[verticesCube]
mov r9d,GL_STATIC_DRAW
call [r15 + OGLAPI.ptr_glBufferData]
call [glGetError]
test eax,eax
jnz .shaderFailed 
xor ecx,ecx
mov edx,3
mov r8d,GL_FLOAT
xor r9,r9
push 0                                         ; attribute base = 0 elements
push 5 * 4                                     ; all entry size = 5 elements
sub rsp,32
call [r15 + OGLAPI.ptr_glVertexAttribPointer]  ; Set cubes vertices coordinates
add rsp,32 + 16
call [glGetError]
test eax,eax
jnz .shaderFailed 
xor ecx,ecx
call [r15 + OGLAPI.ptr_glEnableVertexAttribArray]  ; enable array use by shader
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,1
mov edx,2
mov r8d,GL_FLOAT
xor r9,r9
push 3 * 4                                     ; attribute base = 3 elements
push 5 * 4                                     ; all entry size = 5 elements
sub rsp,32
call [r15 + OGLAPI.ptr_glVertexAttribPointer]  ; Set cubes texture coordinates
add rsp,32 + 16
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,1
call [r15 + OGLAPI.ptr_glEnableVertexAttribArray]  ; enable array use by shader
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Initializing textures ----------------------------------------------------;
mov ecx,1
lea rdx,[r15 - sizeof.APPDATA + APPDATA.TXT1]
call [glGenTextures]
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,GL_TEXTURE_2D
mov edx,[r15 - sizeof.APPDATA + APPDATA.TXT1]
call [glBindTexture]
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,GL_TEXTURE_2D
mov edx,GL_TEXTURE_WRAP_S
mov r8d,GL_REPEAT
call [glTexParameteri]
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,GL_TEXTURE_2D
mov edx,GL_TEXTURE_WRAP_T
mov r8d,GL_REPEAT
call [glTexParameteri]
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,GL_TEXTURE_2D
mov edx,GL_TEXTURE_MIN_FILTER
mov r8d,GL_LINEAR
call [glTexParameteri]
call [glGetError]
test eax,eax
jnz .shaderFailed 
mov ecx,GL_TEXTURE_2D
mov edx,GL_TEXTURE_MAG_FILTER
mov r8d,GL_LINEAR
call [glTexParameteri]
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Specify texture image ----------------------------------------------------; 
mov ecx,GL_TEXTURE_2D
xor edx,edx
mov r8d,GL_RGB
mov r9d,TEXTURE_WIDTH
push rdx
mov rax,[pRawImage]
push rax
push GL_UNSIGNED_BYTE
push GL_BGRA           ; Adjust this parameter depend on image file type
push rdx
push TEXTURE_HEIGHT
sub rsp,32 
call [glTexImage2D]
add rsp,32 + 48 
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Generate mip map: texture copies with differrent detalization ------------;
mov ecx,GL_TEXTURE_2D
call [r15 + OGLAPI.ptr_glGenerateMipmap]
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Connect shader program ---------------------------------------------------;
mov ecx,[r15 - sizeof.APPDATA + APPDATA.shaderProgram]
call [r15 + OGLAPI.ptr_glUseProgram]
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Set texture name for shader program --------------------------------------;
mov ecx,[r15 - sizeof.APPDATA + APPDATA.shaderProgram]
lea rdx,[textureName]
call [r15 + OGLAPI.ptr_glGetUniformLocation]
xchg ecx,eax
xor edx,edx
call [r15 + OGLAPI.ptr_glUniform1i]
call [glGetError]
test eax,eax
jnz .shaderFailed 
;--- Build instancing array, used for CPU-GPU traffic only, scales values -----;
lea rdi,[scales]
push rdi
mov ecx,INSTANCING_COUNT
finit
fld1
push rax
fstp dword [rsp]
pop rax
cld
rep stosd  ; Write INSTANCING_COUNT dwords of float 1.0 
pop rdi
;--- Create and bind data buffer for instancing array -------------------------;
mov ecx,1
lea rdx,[r15 - sizeof.APPDATA + APPDATA.IVBO]
call [r15 + OGLAPI.ptr_glGenBuffers]
mov ecx,GL_ARRAY_BUFFER
mov edx,[r15 - sizeof.APPDATA + APPDATA.IVBO]
call [r15 + OGLAPI.ptr_glBindBuffer]
;--- Copy data buffer from CPU to GPU -----------------------------------------;
mov ecx,GL_ARRAY_BUFFER
mov edx,4 * INSTANCING_COUNT
mov r8,rdi
mov r9d,GL_DYNAMIC_DRAW 
call [r15 + OGLAPI.ptr_glBufferData]
;--- Specify format for data buffer -------------------------------------------;  
mov ecx,GL_ARRAY_BUFFER
xor edx,edx
call [r15 + OGLAPI.ptr_glBindBuffer]
mov ecx,2
call [r15 + OGLAPI.ptr_glEnableVertexAttribArray]
mov ecx,GL_ARRAY_BUFFER
mov edx,[r15 - sizeof.APPDATA + APPDATA.IVBO]
call [r15 + OGLAPI.ptr_glBindBuffer]
mov ecx,2
mov edx,1
mov r8d,GL_FLOAT
xor r9,r9
push 0
push 4
sub rsp,32
call [r15 + OGLAPI.ptr_glVertexAttribPointer]
add rsp,32 + 16
mov ecx,2
mov edx,1
call [r15 + OGLAPI.ptr_glVertexAttribDivisor]
jmp .shadersOk
;--- Initialization errors handling -------------------------------------------;
.linkFailed:
mov ecx,ebx
mov edx,TEMP_BUFFER_SIZE
xor r8d,r8d
lea rsi,[tempBuffer1]
mov r9,rsi
call [r15 + OGLAPI.ptr_glGetProgramInfoLog]
mov rdx,rsi
jmp .msgEntry
.compileFailed:
mov ecx,ebx
mov edx,TEMP_BUFFER_SIZE
xor r8d,r8d
lea rsi,[tempBuffer1]
mov r9,rsi
call [r15 + OGLAPI.ptr_glGetShaderInfoLog]
mov rdx,rsi
jmp .msgEntry
;--- Show message about initialization error ----------------------------------;
.shaderFailed:
lea rdx,[msgShader]   ; RDX = Parm #2 = Message
jmp .msgEntry 
.loadFailed:
lea rdx,[msgLoad]     ; RDX = Parm #2 = Message
.msgEntry:
xor ecx,ecx           ; RCX = Parm #1 = Parent window
xor r8,r8             ; R8  = Parm #3 = Caption (0 means error)
mov r9d,MB_ICONERROR  ; R9  = Parm #4 = Message flags
call [MessageBoxA]
mov [contextSkip],1
mov rax,-1            ; Status code -1 is request for destroy window by caller
jmp .finish
;--- Initialization OK, clear variables and exit window callback handler ------;
.shadersOk:
call HelperStartSeconds
lea rcx,[r15 - sizeof.APPDATA + APPDATA.framesCount]
xor eax,eax
mov [rcx + 00],rax
mov [rcx + 08],rax
mov [rcx + 16],rax
mov [contextSkip],al ; 0
jmp .finish

;--- Window message handler : resize window procedure -------------------------;

.wmsize:
lea rdi,[r15 - sizeof.APPDATA + APPDATA.rc]       ; rdi = pointer to rc
mov rcx,rbx
mov rdx,rdi
call [GetClientRect]
xor ecx,ecx
xor edx,edx
mov r8d,[rdi + RECT.right]
mov r9d,[rdi + RECT.bottom]
call [glViewport]
xor eax,eax
jmp .finish

;--- Window message handler : graphics paint procedure ------------------------;

.wmpaint:
;--- Clear OpenGL buffers and start draw with shader program ------------------;
lea rcx,[clearColor]
movd xmm0,[rcx + 00]
movd xmm1,[rcx + 04]
movd xmm2,[rcx + 08]
movd xmm3,[rcx + 12]
call [glClearColor]
mov ecx,GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT
call [glClear]
mov ecx,GL_DEPTH_TEST
call [glEnable]
mov ecx,GL_TEXTURE0
call [r15 + OGLAPI.ptr_glActiveTexture]
mov ecx,GL_TEXTURE_2D 
mov edx,[r15 - sizeof.APPDATA + APPDATA.TXT1]
call [glBindTexture]
mov ecx,[r15 - sizeof.APPDATA + APPDATA.shaderProgram]
call [r15 + OGLAPI.ptr_glUseProgram]
;--- Build transformation matrices, rotation = f( time ) ----------------------;
call HelperGetSeconds
fsincos
fld st0
lea rdx,[scales]
mov ecx,INSTANCING_COUNT - 1
@@:
fst dword [rdx]
add rdx,4
dec ecx
jnz @b
fstp dword [rdx] 
lea rcx,[model_1]
lea rdx,[model_Empty]  ; [model_2]  ; TODO. Optimize, remove empty.
lea r8,[model_3]
fst dword [rcx + 4*0 + 0]
fst dword [rdx + 4*0 + 0]
fst dword [r8 + 4*4 + 4]
fst dword [rcx + 4*4 + 4]
fst dword [rdx + 4*8 + 8]
fstp dword [r8 + 4*8 + 8]
fst dword [rcx + 4*0 + 4]
fst dword [rdx + 4*8 + 0]
fst dword [r8 + 4*4 + 8]
fchs
fst dword [rcx + 4*4 + 0] 
fst dword [rdx + 4*0 + 8]
fstp dword [r8 + 4*8 + 4]
call HelperMultiplyMatrices
;--- Send transformations matrix to shader program ----------------------------;
mov ecx,[r15 - sizeof.APPDATA + APPDATA.shaderProgram]
lea rdx,[modelName_R]
call [r15 + OGLAPI.ptr_glGetUniformLocation]
xchg ecx,eax
mov edx,1
xor r8,r8
lea r9,[model_R]
call [r15 + OGLAPI.ptr_glUniformMatrix4fv]

;--- Data buffer copy, this operation time measured for MBPS statistics -------;
;--- Start of timings measurement interval ------------------------------------;

BUFFER_COUNT  EQU  (4 * INSTANCING_COUNT)  

rdtsc
mov esi,eax
mov edi,edx
xor eax,eax  ; this CPUID for serialization only, results ignored
cpuid

mov ecx,GL_ARRAY_BUFFER
mov edx,BUFFER_COUNT
lea r8,[scales]
mov r9d,GL_DYNAMIC_DRAW 
add [r15 - sizeof.APPDATA + APPDATA.bytesCount],rdx
call [r15 + OGLAPI.ptr_glBufferData]

xor eax,eax  ; this CPUID for serialization only, results ignored
cpuid
lea rcx,[mbpsStamp]
rdtsc
sub eax,esi
sbb edx,edi
mov [rcx + 0],eax
mov [rcx + 4],edx
mov qword [rcx + 8],BUFFER_COUNT
lea rcx,[r15 - sizeof.APPDATA + APPDATA.tscBytes]
add [rcx + 00],eax
adc [rcx + 04],edx
;--- End of timings measurement interval --------------------------------------;

;--- Draw and swap buffers ----------------------------------------------------;
mov ecx,[r15 - sizeof.APPDATA + APPDATA.VAO]
call [r15 + OGLAPI.ptr_glBindVertexArray]
mov ecx,GL_TRIANGLES
xor edx,edx
mov r8d,6 * 6
mov r9d,INSTANCING_COUNT
call [r15 + OGLAPI.ptr_glDrawArraysInstanced]
mov rcx,[r15 - sizeof.APPDATA + APPDATA.hdc]
call [SwapBuffers]
;--- Build text strings for FPS statistics strings group ----------------------;
call HelperGetSeconds
push rax
fstp qword [rsp]
mov bx,0200h
mov rax,[rsp]
lea rdi,[tempBuffer2 + 128 * 3 + 73]
call DoublePrint_Blanked         ; Print seconds
mov rax,[r15 - sizeof.APPDATA + APPDATA.framesCount]  ; TODO. Check bits RAX.[63-32] for overflow
push rax
mov bl,0
lea rdi,[tempBuffer2 + 128 * 2 + 73]
call DecimalPrint32_Blanked      ; Print frames
fld qword [rsp + 8]              ; st0 = seconds 
fild qword [rsp]                 ; st0 = frames, st1 = seconds. 
pop rax rax
fdiv st0,st1                     ; st0 = FPS
push rax
fstp qword [rsp]
pop rax
mov bx,0200h
lea rdi,[tempBuffer2 + 128 * 1 + 73]
call DoublePrint_Blanked         ; Print FPS average

;--- Build text string for FPS current actual value ---------------------------;
lea rbx,[fpsStamp1] 
mov rcx,[rbx + 0]
mov [rbx + 8],rcx 
rdtsc
shl rdx,32
add rax,rdx
mov [rbx + 0],rax
jrcxz .skipFps      ; Skip visualization if first frame, previous TSC not valid 
sub rax,rcx         ; RAX = delta TSC between frames
fld1
push rax
fild qword [rsp]
fmul [r15 - sizeof.APPDATA + APPDATA.tscPeriod]
fdivp st1,st0
fstp qword [rsp]
mov bx,0200h
lea rdi,[tempBuffer2 + 128 * 0 + 73]
pop rax
call DoublePrint_Blanked         ; Print FPS current
.skipFps:

;--- Build text strings for MBPS statistics strings group ---------------------;
lea rcx,[r15 - sizeof.APPDATA + APPDATA.tscPeriod]
push 0
push 1000000
push 0
fild qword [rcx + 32]   ; st0 = bus traffic time in the TSC clocks 
fmul qword [rcx + 00]   ; st0 = bus traffic seconds
fst qword [rsp + 00] 
fild qword [rcx + 40]   ; st0 = bytes, st1 = bus traffic seconds 
fidiv dword [rsp + 08]  ; st0 = megabytes
fst qword [rsp + 08]
fdiv st0,st1            ; st0 = MBPS
fst qword [rsp + 16] 
pop rax
mov bx,0200h
lea rdi,[tempBuffer2 + 128 * 3 + 112]
call DoublePrint_Blanked        ; Print Bus traffic seconds
pop rax
lea rdi,[tempBuffer2 + 128 * 2 + 112]
call DoublePrint_Blanked        ; Print Megabytes
pop rax
lea rdi,[tempBuffer2 + 128 * 1 + 112]
call DoublePrint_Blanked        ; Print MBPS average

;--- Build text string for MBPS current actual value --------------------------;
lea rbx,[mbpsStamp] 
mov rcx,[rbx + 0]     ; RCX = delta TSC per buffer copy (by GPU bus master?)
jrcxz .skipMbps       ; Skip visualization if delta TSC not valid 
push rcx
fild qword [rbx + 8]  ; st0 = bytes transferred
fild qword [rsp]      ; st0 = delta TSC, clocks, st1 = bytes transferred
fmul [r15 - sizeof.APPDATA + APPDATA.tscPeriod]   ; st0 = dTSC, seconds 
fdivp st1,st0                                     ; st0 = bytes per second
mov dword [rsp],1000000
fidiv dword [rsp]     ; st0 = decimal megabytes per second
fstp qword [rsp]
mov bx,0200h
lea rdi,[tempBuffer2 + 128 * 0 + 112]
pop rax
call DoublePrint_Blanked  ; Print MBPS current
.skipMbps:

;--- Transfer uniform array with text data into shader program ----------------;
xor ebx,ebx
.uniformArray:
lea rsi,[showTextName]
lea rdi,[tempBuffer2 + 128*4] 
call StringWrite
mov al,'['
stosb
push rbx
xchg eax,ebx
mov bl,0
call DecimalPrint32
pop rbx
mov ax,0000h + ']'
stosw
mov ecx,[r15 - sizeof.APPDATA + APPDATA.shaderProgram]
lea rdx,[tempBuffer2 + 128*4]
call [r15 + OGLAPI.ptr_glGetUniformLocation]
xchg ecx,eax
mov edx,dword [tempBuffer2 + rbx*4]
call [r15 + OGLAPI.ptr_glUniform1i]
inc ebx
cmp ebx,128
jb .uniformArray
;--- Frame counter increment and exit handler ---------------------------------;
inc [r15 - sizeof.APPDATA + APPDATA.framesCount]
xor eax,eax
jmp .finish

;--- Window message handler : resize window procedure -------------------------;

.wmkeydown:
cmp r8d,VK_ESCAPE
jne .defwndproc

;--- Window message handler : destroy window procedure ------------------------;

.wmdestroy:
cmp [contextSkip],0
jne .skipContext
;--- Close conditionally initialized context ----------------------------------;
call HelperStopSeconds
mov ecx,1
lea rdx,[r15 - sizeof.APPDATA + APPDATA.VAO]
call [r15 + OGLAPI.ptr_glDeleteVertexArrays]
mov ecx,1
lea rdx,[r15 - sizeof.APPDATA + APPDATA.VBO]
call [r15 + OGLAPI.ptr_glDeleteBuffers]
;--- Close unconditionally initialized context --------------------------------;
.skipContext:
xor ecx,ecx
xor edx,edx
call [wglMakeCurrent]
mov rcx,[r15 - sizeof.APPDATA + APPDATA.hrc]
call [wglDeleteContext]
mov rcx,rbx
;--- Release display context --------------------------------------------------; 
mov rdx,[r15 - sizeof.APPDATA + APPDATA.hdc]
call [ReleaseDC]	
;--- Close window -------------------------------------------------------------;
mov rcx,rbx                ; Parm#1 = RCX = Handle
mov edx,WM_CLOSE           ; Parm#2 = RDX = Message
xor r8d,r8d                ; Parm#3 = R8 = Not used
xor r9d,r9d                ; Parm#4 = R9 = Not used
call [SendMessageA]
xor ecx,ecx
call [PostQuitMessage]
xor	eax,eax

;--- Global window procedure exit point ---------------------------------------;

.finish:
mov rsp,rbp
pop r15 rbp rdi rsi rbx
ret

;---------- Helper procedures -------------------------------------------------;

;--- Initializing seconds counter from measuring session start, use TSC -------;
; INPUT:   R15 = Pointer for addressing global variables                       ;
; OUTPUT:  None                                                                ;  
;------------------------------------------------------------------------------;
HelperStartSeconds:
call HelperMeasureTsc
lea rcx,[r15 - sizeof.APPDATA + APPDATA.tscFreq]
mov [rcx + 00],rax    ; save frequency, hz, integer 64
finit
fld1
fild qword [rcx + 00]
fdivp st1,st0
fstp qword [rcx + 08] ; save period, seconds, double
rdtsc
mov [rcx + 16],eax    ; save TSC value at application start, clocks, integer 64
mov [rcx + 20],edx
ret 
;--- Get seconds from application start, use TSC ------------------------------;
; INPUT:   R15 = Pointer for addressing global variables                       ;
; OUTPUT:  x87 st0 = seconds                                                   ;
;------------------------------------------------------------------------------;
HelperGetSeconds:
lea rcx,[r15 - sizeof.APPDATA + APPDATA.tscPeriod]
rdtsc
shl rdx,32
add rax,rdx
sub rax,[rcx + 08]
finit
push rax
fild qword [rsp]
pop rax
fmul qword [rcx + 00] 
ret
;--- Store seconds counter at measuring session stop, use TSC -----------------;
; INPUT:   None                                                                ;
; OUTPUT:  None                                                                ;  
;------------------------------------------------------------------------------;
HelperStopSeconds:
lea rcx,[r15 - sizeof.APPDATA + APPDATA.tscStop]
rdtsc
mov [rcx + 0],eax    ; save TSC value at application stop, clocks, integer 64
mov [rcx + 4],edx
ret 
;---------- Measure CPU TSC (Time Stamp Counter) clock frequency --------------;
; Store results F = Frequency=[Hz].                                            ;
; Call this subroutine only if CPUID and RDTSC both supported.                 ;
;                                                                              ;
; INPUT:   None.                                                               ;
;                                                                              ;
; OUTPUT:  CF flag = Status: 0(NC)=Measured OK, 1(C)=Measurement error	       ;
;          RAX = TSC frequency, Hz = delta TSC per 1 second                    ;
;                                                                              ;
;------------------------------------------------------------------------------;
HelperMeasureTsc:
push rbx rsi rbp rbp        ; Last push for reserve local variable space
mov rbp,rsp                 ; RBP used for restore RSP and addressing variables
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32                  ; Make parameters shadow
;---------- Start measure frequency -------------------------------------------;
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get current count
mov rsi,[rbp]
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get next count for wait 100 ns
cmp rsi,[rbp]
je @b
mov rsi,[rbp]
add rsi,10000000                  ; 10^7 * 100ns = 1 second
rdtsc
shl rdx,32
lea rbx,[rax + rdx]               ; RBX = 64-bit TSC at operation start
@@:
mov rcx,rbp
call [GetSystemTimeAsFileTime]    ; Get count for wait 1 second
cmp rsi,[rbp]
ja @b
rdtsc
shl rdx,32
or rax,rdx                        ; RAX = 64-bit TSC at operation end
sub rax,rbx                       ; RAX = Delta TSC
jbe .error
;---------- Restore RSP, pop extra registers, exit ----------------------------;
; RAX = frequency, as 64-bit integer value, Hz, delta-TSC per second
clc             ; CF=0 (NC) means CPU clock measured OK
.exit:
mov rsp,rbp
pop rbp rbp rsi rbx
ret
.error:
stc             ; CF=1 (CY) means CPU clock measured ERROR
jmp .exit

;---------- Helpers for text output -------------------------------------------;

;---------- Copy text string terminated by 00h --------------------------------;
; Note last byte 00h not copied.                                               ;
;                                                                              ;
; INPUT:   RSI = Source address                                                ;
;          RDI = Destination address                                           ;
;                                                                              ;
; OUTPUT:  RSI = Modified by copy                                              ;
;          RDI = Modified by copy                                              ;
;          Memory at [Input RDI] modified                                      ;
;                                                                              ;
;------------------------------------------------------------------------------;
StringWrite:
cld
.cycle:
lodsb
cmp al,0
je .exit
stosb
jmp .cycle
.exit:
ret
;---------- Print 32-bit Decimal Number ---------------------------------------;
;                                                                              ;
; INPUT:   EAX = Number value                                                  ;
;          BL  = Template size, chars. 0=No template                           ;
;          RDI = Destination Pointer (flat)                                    ;
;                                                                              ;
; OUTPUT:  RDI = New Destination Pointer (flat)                                ;
;                modified because string write                                 ;
;                                                                              ;
;------------------------------------------------------------------------------;

DecimalPrint32_Blanked:  ; Additional entry point for pre-blank 12 chars
push rax rcx rdi
mov ecx,12
mov al,' '
cld
rep stosb
pop rdi rcx rax

DecimalPrint32:
cld
push rax rbx rcx rdx
mov bh,80h-10         ; Bit BH.7 = print zeroes flag
add bh,bl
mov ecx,1000000000    ; ECX = service divisor
.mainCycle:
xor edx,edx
div ecx               ; Produce current digit, EDX:EAX / ECX
and al,0Fh
test bh,bh
js .firstZero
cmp ecx,1
je .firstZero
cmp al,0              ; Not actual left zero ?
jz .skipZero
.firstZero:
mov bh,80h            ; Flag = 1
or al,30h
stosb                 ; Store char
.skipZero:
push rdx              ; Push remainder
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax          ; ECX = Quotient, used as divisor and cycle condition 
pop rax              ; EAX = remainder
inc bh
test ecx,ecx
jnz .mainCycle       ; Cycle if (unsigned) quotient still > 0 
pop rdx rcx rbx rax
ret
;---------- Print double precision value --------------------------------------;
; x87 FPU used, required x87 presence validation by CPUID before call this.    ;
;                                                                              ;
; INPUT:   RAX = Double precision number                                       ;
;          BL  = Number of digits in the INTEGER part,                         ;
;                used for add left non-signed zeroes.                          ;
;                BL=0 means not print left unsigned zeroes.                    ;
;          BH  = Number of digits in the FLOAT part,                           ;
;                used as precision control.                                    ;
;          RDI = Destination text buffer pointer                               ;
;                                                                              ;
; OUTPUT:  RDI = Modified by text string write                                 ;
;                                                                              ;
;------------------------------------------------------------------------------;

DoublePrint_Blanked:  ; Additional entry point for pre-blank 12 chars
push rax rcx rdi
mov ecx,12
mov al,' '
cld
rep stosb
pop rdi rcx rax

DoublePrint:
push rax rbx rcx rdx r8 r9 r10 r11
cld
mov rdx,07FFFFFFFFFFFFFFFh
and rdx,rax
jz .fp64_Zero
mov rcx,07FF8000000000000h
cmp rdx,rcx
je .fp64_QNAN
mov rcx,07FF0000000000000h
cmp rdx,rcx
je .fp64_INF
ja .fp64_NAN
finit
push rax
push rax
fstcw [rsp]
pop rax
or ax,0C00h
push rax
fldcw [rsp]
pop rax
fld qword [rsp]
pop rax
fld st0
frndint
fxch
fsub st0,st1
mov eax,1
movzx ecx,bh
jrcxz .orderDetected
@@:
imul rax,rax,10
loop @b
.orderDetected:
push rax
fimul dword [rsp]
pop rax
push rax rax
fbstp [rsp]
pop r8 r9
push rax rax
fbstp [rsp]
pop r10 r11
bt r11,15
setc dl
bt r9,15
setc dh
test dx,dx
jz @f
mov al,'-'
stosb
@@:
mov dl,0
mov ecx,18 
.cycleInteger:
mov al,r11l
shr al,4
cmp cl,1
je .store
cmp cl,bl
jbe .store
test dl,dl
jnz .store
test al,al
jz .position 
.store:
mov dl,1
or al,30h
stosb
.position:
shld r11,r10,4
shl r10,4
loop .cycleInteger
test bh,bh
jz .exit
mov al,'.'
stosb
std 
movzx ecx,bh     
lea rdi,[rdi + rcx]
push rdi
dec rdi
.cycleFloat:
mov al,r8l
and al,0Fh
or al,30h
stosb
shrd r8,r9,4
shr r9,4
loop .cycleFloat
pop rdi
cld
jmp .exit
.fp64_Zero:
mov eax,'0.0 '
jmp .fp64special
.fp64_INF:
mov eax,'INF '
jmp .fp64special
.fp64_NAN:
mov eax,'NAN '
jmp .fp64special
.fp64_QNAN:
mov eax,'QNAN'
.fp64special:
stosd
jmp .exit
.error:
mov al,'?'
stosb
.exit:
finit
pop r11 r10 r9 r8 rdx rcx rbx rax
ret
;--- Helper for multiply 4x4 matrices -----------------------------------------;
; model_R = model_1 * model_2                                                  ;
; model_R = model_R * model_3                                                  ;
;------------------------------------------------------------------------------;
HelperMultiplyMatrices:
push rbx rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
movaps [rsp + 00],xmm6
movaps [rsp + 16],xmm7
lea rax,[model_1]
lea rbx,[model_2]
lea rcx,[model_R]
call HelperMultiplyMatrix
lea rax,[model_R]
lea rbx,[model_3]
lea rcx,[model_R]
call HelperMultiplyMatrix
movaps xmm6,[rsp + 00]
movaps xmm7,[rsp + 16]
mov rsp,rbp
pop rbp rbx
ret
;--- Helper for multiply 4x4 matrices -----------------------------------------;
; mat[rcx] = mat[rax] * mat[rbx]                                               ;
;------------------------------------------------------------------------------;
HelperMultiplyMatrix:
xor edx,edx
movaps xmm4,[rbx + 16*0]
movaps xmm5,[rbx + 16*1]
movaps xmm6,[rbx + 16*2]
movaps xmm7,[rbx + 16*3] 
.oneVector:
movaps xmm0,[rax + rdx]
movaps xmm1,xmm0
movaps xmm2,xmm0
movaps xmm3,xmm0 
shufps xmm0,xmm0,00000000b 
shufps xmm1,xmm1,01010101b
shufps xmm2,xmm2,10101010b
shufps xmm3,xmm3,11111111b
mulps xmm0,xmm4
mulps xmm1,xmm5
mulps xmm2,xmm6
mulps xmm3,xmm7
addps xmm0,xmm1
addps xmm2,xmm3
addps xmm0,xmm2
movaps [rcx + rdx],xmm0
add edx,16
cmp edx,16*4
jb .oneVector 
ret

;---------- Data section ------------------------------------------------------;

section '.data' data readable writeable

appName       DB  APPLICATION_NAME , 0
appClass      DB  'FASMOPENGL32'   , 0
wc      WNDCLASS  0, WindowProc, 0, 0, NULL, NULL, NULL, NULL, NULL, appClass
contextSkip   DB  1

;--- OpenGL background color --------------------------------------------------;
align 16
clearColor    GLclampf  BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 1.0

;--- Vertex model -------------------------------------------------------------;
align 16
verticesCube  GLfloat   -0.5 , -0.5 , -0.5 ,  0.0 , 0.0
              GLfloat    0.5 , -0.5 , -0.5 ,  1.0 , 0.0
              GLfloat    0.5 ,  0.5 , -0.5 ,  1.0 , 1.0
              GLfloat    0.5 ,  0.5 , -0.5 ,  1.0 , 1.0
              GLfloat   -0.5 ,  0.5 , -0.5 ,  0.0 , 1.0
              GLfloat   -0.5 , -0.5 , -0.5 ,  0.0 , 0.0

              GLfloat   -0.5 , -0.5 ,  0.5 ,  0.0 , 1.0
              GLfloat    0.5 , -0.5 ,  0.5 ,  1.0 , 1.0
              GLfloat    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0
              GLfloat    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0
              GLfloat   -0.5 ,  0.5 ,  0.5 ,  0.0 , 0.0
              GLfloat   -0.5 , -0.5 ,  0.5 ,  0.0 , 1.0

              GLfloat   -0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0
              GLfloat   -0.5 ,  0.5 , -0.5 ,  1.0 , 0.0
              GLfloat   -0.5 , -0.5 , -0.5 ,  0.0 , 0.0
              GLfloat   -0.5 , -0.5 , -0.5 ,  0.0 , 0.0
              GLfloat   -0.5 , -0.5 ,  0.5 ,  0.0 , 1.0
              GLfloat   -0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0

              GLfloat    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0
              GLfloat    0.5 ,  0.5 , -0.5 ,  1.0 , 1.0
              GLfloat    0.5 , -0.5 , -0.5 ,  0.0 , 1.0
              GLfloat    0.5 , -0.5 , -0.5 ,  0.0 , 1.0
              GLfloat    0.5 , -0.5 ,  0.5 ,  0.0 , 0.0
              GLfloat    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0

              GLfloat   -0.5 , -0.5 , -0.5 ,  0.0 , 1.0
              GLfloat    0.5 , -0.5 , -0.5 ,  1.0 , 1.0
              GLfloat    0.5 , -0.5 ,  0.5 ,  1.0 , 0.0
              GLfloat    0.5 , -0.5 ,  0.5 ,  1.0 , 0.0
              GLfloat   -0.5 , -0.5 ,  0.5 ,  0.0 , 0.0
              GLfloat   -0.5 , -0.5 , -0.5 ,  0.0 , 1.0

              GLfloat   -0.5 ,  0.5 , -0.5 ,  0.0 , 0.0
              GLfloat    0.5 ,  0.5 , -0.5 ,  1.0 , 0.0
              GLfloat    0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0
              GLfloat    0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0
              GLfloat   -0.5 ,  0.5 ,  0.5 ,  0.0 , 1.0
              GLfloat   -0.5 ,  0.5 , -0.5 ,  0.0 , 0.0 

;--- Transformation matrices, blanked as identity matrix at application start -; 
align 16
model_1       GLfloat  1.0 , 0.0 , 0.0 , 0.0
              GLfloat  0.0 , 1.0 , 0.0 , 0.0
              GLfloat  0.0 , 0.0 , 1.0 , 0.0
              GLfloat  0.0 , 0.0 , 0.0 , 1.0

model_2       GLfloat  1.0 , 0.0 , 0.0 , 0.0
              GLfloat  0.0 , 1.0 , 0.0 , 0.0
              GLfloat  0.0 , 0.0 , 1.0 , 0.0
              GLfloat  0.0 , 0.0 , 0.0 , 1.0

model_3       GLfloat  1.0 , 0.0 , 0.0 , 0.0
              GLfloat  0.0 , 1.0 , 0.0 , 0.0
              GLfloat  0.0 , 0.0 , 1.0 , 0.0
              GLfloat  0.0 , 0.0 , 0.0 , 1.0

model_R       GLfloat  1.0 , 0.0 , 0.0 , 0.0
              GLfloat  0.0 , 1.0 , 0.0 , 0.0
              GLfloat  0.0 , 0.0 , 1.0 , 0.0
              GLfloat  0.0 , 0.0 , 0.0 , 1.0

model_Empty   GLfloat  1.0 , 0.0 , 0.0 , 0.0
              GLfloat  0.0 , 1.0 , 0.0 , 0.0
              GLfloat  0.0 , 0.0 , 1.0 , 0.0
              GLfloat  0.0 , 0.0 , 0.0 , 1.0

;--- Keys list for get information about GPU by OpenGL API --------------------; 
listStrings   DD  GL_VENDOR
              DD  GL_RENDERER
              DD  GL_VERSION
              DD  GL_SHADING_LANGUAGE_VERSION
              DD  0

;--- Dynamical import list for OpenGL API -------------------------------------; 
oglNamesList:
DB  'glCreateShader'            , 0
DB  'glShaderSource'            , 0
DB  'glCompileShader'           , 0
DB  'glGetShaderiv'             , 0
DB  'glGetShaderInfoLog'        , 0
DB  'glCreateProgram'           , 0
DB  'glAttachShader'            , 0
DB  'glLinkProgram'             , 0
DB  'glGetProgramiv'            , 0
DB  'glGetProgramInfoLog'       , 0
DB  'glDeleteShader'            , 0
DB  'glGenVertexArrays'         , 0
DB  'glGenBuffers'              , 0
DB  'glBindVertexArray'         , 0
DB  'glBindBuffer'              , 0
DB  'glBufferData'              , 0
DB  'glVertexAttribPointer'     , 0
DB  'glEnableVertexAttribArray' , 0
DB  'glUseProgram'              , 0
DB  'glDeleteVertexArrays'      , 0
DB  'glDeleteBuffers'           , 0
DB  'glGetUniformLocation'      , 0
DB  'glUniformMatrix4fv'        , 0
DB  'glGenerateMipmap'          , 0
DB  'glUniform1i'               , 0
DB  'glActiveTexture'           , 0
DB  'glDrawArraysInstanced'     , 0
DB  'glVertexAttribDivisor'     , 0 , 0

;--- Vertex shader source, compiled at runtime by GPU driver ------------------;
ptr_vertexShaderSource DQ vertexShaderSource
vertexShaderSource:
DB  '#version 330 core'                             , 0Dh, 0Ah
DB  'layout (location = 0) in vec3 aPos;'           , 0Dh, 0Ah
DB  'layout (location = 1) in vec2 aTexCoord;'      , 0Dh, 0Ah
DB  'layout (location = 2) in float sc;'            , 0Dh, 0Ah
DB  'out vec2 TexCoord;'                            , 0Dh, 0Ah
DB  'uniform mat4 model_R;'                         , 0Dh, 0Ah
DB  'uniform int showText[128];'                    , 0Dh, 0Ah
DB  'void main()'                                   , 0Dh, 0Ah
DB  '{'                                             , 0Dh, 0Ah
DB  'if(gl_InstanceID < 512)'                       , 0Dh, 0Ah
DB  '   {'                                          , 0Dh, 0Ah
;--- Screen coordinates for 128x4 chars positions -----------------------------;
DB  '   int nx = gl_InstanceID & 0x7F;'             , 0Dh, 0Ah
DB  '   int ny = gl_InstanceID >> 7;'               , 0Dh, 0Ah  
DB  '   float dx = 2.0f / 128.0f;'                  , 0Dh, 0Ah
DB  '   float dy = 2.0f * 44.0f / 1967.0f;'         , 0Dh, 0Ah
DB  '   float x1 = nx * dx - 1.0f;'                 , 0Dh, 0Ah   
DB  '   float y1 = ny * dy - 1.0f;'                 , 0Dh, 0Ah
DB  '   float x2 = x1 + dx;'                        , 0Dh, 0Ah 
DB  '   float y2 = y1 + dy;'                        , 0Dh, 0Ah
DB  '   float rx = (aPos.x < 0) ? x1 : x2;'         , 0Dh, 0Ah   
DB  '   float ry = (aPos.y < 0) ? y1 : y2;'         , 0Dh, 0Ah
DB  '   float rz = aPos.z;'                         , 0Dh, 0Ah
DB  '   gl_Position = vec4(rx, ry, rz, 1.0f);'      , 0Dh, 0Ah   
;--- Texture coordinates (showed chars select) for 128x4 chars positions ------;
DB  '   int index = gl_InstanceID / 4;'             , 0Dh, 0Ah
DB  '   int shift = (gl_InstanceID & 3) * 8;'       , 0Dh, 0Ah  
DB  '   int a = (showText[index] >> shift) & 0x7F;' , 0Dh, 0Ah
DB  '   float corx = 0.5f / 2952.0f;'               , 0Dh, 0Ah    
DB  '   float cory = 0.5f / 1967.0f;'               , 0Dh, 0Ah
DB  '   float kx = 8.0f / 2952.0f;'                 , 0Dh, 0Ah
DB  '   float ky = 16.0f / 1967.0f;'                , 0Dh, 0Ah
DB  '   float tx1 = kx * a - corx;'                 , 0Dh, 0Ah 
DB  '   float tx2 = tx1 + kx - corx;'               , 0Dh, 0Ah
DB  '   float ty1 = 0.0f + cory;'                   , 0Dh, 0Ah 
DB  '   float ty2 = ky - cory;'                     , 0Dh, 0Ah
DB  '   float tx = (aPos.x < 0) ? tx1 : tx2;'       , 0Dh, 0Ah
DB  '   float ty = (aPos.y < 0) ? ty1 : ty2;'       , 0Dh, 0Ah
DB  '   TexCoord = vec2(tx, ty);'                   , 0Dh, 0Ah
DB  '   }'                                          , 0Dh, 0Ah
;--- Otherwise render cubes. --------------------------------------------------;
DB  '   else'                                       , 0Dh, 0Ah
DB  '   {' ,0Dh, 0Ah
DB  '   int nx = gl_InstanceID % 9;'                , 0Dh, 0Ah
DB  '   int ny = gl_InstanceID / 9 % 3;'            , 0Dh, 0Ah
DB  '   float dx = -0.85f + nx / 4.75f;'            , 0Dh, 0Ah
DB  '   float dy = -0.56f + ny / 1.80f;'            , 0Dh, 0Ah
DB  '   vec4 t = model_R * vec4(aPos, 1.0f);'       , 0Dh, 0Ah
DB  '   float sx = 5.50f + 7.5 - 8.5f * abs(sc);'   , 0Dh, 0Ah
DB  '   float sy = 3.55f + 7.5 - 8.5f * abs(sc);'   , 0Dh, 0Ah
DB  '   float sz = 5.50f + 7.5 - 8.5f * abs(sc);'   , 0Dh, 0Ah
DB  '   sx = 3.2f + sx / 3.0f;'   , 0Dh, 0Ah
DB  '   sy = 3.2f + sy / 3.0f;'   , 0Dh, 0Ah
DB  '   sy = 3.2f + sz / 3.0f;'   , 0Dh, 0Ah
DB  '   gl_Position = vec4(t.x/sx + dx , t.y/sy + dy, t.z/sz, t.w);' , 0Dh, 0Ah
DB  '   float ctx = 94.0f   / 2952.0f;'             , 0Dh, 0Ah
DB  '   float cty = 1527.0f / 1967.0f;'             , 0Dh, 0Ah
DB  '   float mtx = 303.0f  / 2952.0f;'             , 0Dh, 0Ah
DB  '   float mty = 482.0f  / 1967.0f;'             , 0Dh, 0Ah
DB  '   float dtx = 344.0f  / 2952.0f;'             , 0Dh, 0Ah
DB  '   float dty = 344.0f  / 1967.0f;'             , 0Dh, 0Ah
DB  '   float tx  = ctx + dtx * aTexCoord.x + nx * mtx;'       , 0Dh, 0Ah  
DB  '   float ty  = cty - dty * aTexCoord.y - mty * (2 - ny);' , 0Dh, 0Ah 
DB  '   TexCoord = vec2(tx, ty);'                   , 0Dh, 0Ah
DB  '   }' ,0Dh, 0Ah
DB  '}'                                             , 0Dh, 0Ah, 0

;--- Fragment shader source, compiled at runtime by GPU driver ----------------;
ptr_fragmentShaderSource DQ fragmentShaderSource
fragmentShaderSource:
DB  '#version 330 core'                             , 0Dh, 0Ah
DB  'out vec4 FragColor;'                           , 0Dh, 0Ah
DB  'in vec2 TexCoord;'                             , 0Dh, 0Ah
DB  'uniform sampler2D texture1;'                   , 0Dh, 0Ah
DB  'void main()'                                   , 0Dh, 0Ah
DB  '{'                                             , 0Dh, 0Ah            
DB  '   FragColor = texture(texture1, TexCoord);'   , 0Dh, 0Ah
DB  '}'                                             , 0Dh, 0Ah, 0

;--- Data for CPU-GPU communication with named variables ----------------------;
modelName_R   DB  'model_R'  , 0
textureName   DB  'texture1' , 0
showTextName  DB  'showText' , 0

;--- Messages and parameters names strings ------------------------------------;
szSeconds     DB  'Seconds'                        , 0
szFrames      DB  'Frames'                         , 0
szFPSavg      DB  'FPS average'                    , 0
szFPScur      DB  'FPS current'                    , 0
szBusSeconds  DB  'Bus traffic seconds'            , 0
szBusMB       DB  'Megabytes'                      , 0
szBusMBPSavg  DB  'MBPS average'                   , 0
szBusMBPScur  DB  'MBPS current'                   , 0
msgLoad       DB  'Load OpenGL API failed.'        , 0
msgShader     DB  'Shaders initialization failed.' , 0

;--- Raster font, chars codes 00h-7Fh, format 8x16 ----------------------------;
align 16
rasterFont:
DB  000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 000
DB  000h,000h,07Eh,081h,0A5h,081h,081h,0A5h,099h,081h,081h,07Eh,000h,000h,000h,000h  ; 001
DB  000h,000h,07Eh,0FFh,0DBh,0FFh,0FFh,0DBh,0E7h,0FFh,0FFh,07Eh,000h,000h,000h,000h  ; 002
DB  000h,000h,000h,000h,06Ch,0FEh,0FEh,0FEh,0FEh,07Ch,038h,010h,000h,000h,000h,000h  ; 003
DB  000h,000h,000h,000h,010h,038h,07Ch,0FEh,07Ch,038h,010h,000h,000h,000h,000h,000h  ; 004
DB  000h,000h,000h,018h,03Ch,03Ch,0E7h,0E7h,0E7h,018h,018h,03Ch,000h,000h,000h,000h  ; 005
DB  000h,000h,000h,018h,03Ch,07Eh,0FFh,0FFh,07Eh,018h,018h,03Ch,000h,000h,000h,000h  ; 006
DB  000h,000h,000h,000h,000h,000h,018h,03Ch,03Ch,018h,000h,000h,000h,000h,000h,000h  ; 007
DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0E7h,0C3h,0C3h,0E7h,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh  ; 008
DB  000h,000h,000h,000h,000h,03Ch,066h,042h,042h,066h,03Ch,000h,000h,000h,000h,000h  ; 009
DB  0FFh,0FFh,0FFh,0FFh,0FFh,0C3h,099h,0BDh,0BDh,099h,0C3h,0FFh,0FFh,0FFh,0FFh,0FFh  ; 00A
DB  000h,000h,01Eh,006h,00Eh,01Ah,078h,0CCh,0CCh,0CCh,0CCh,078h,000h,000h,000h,000h  ; 00B
DB  000h,000h,03Ch,066h,066h,066h,066h,03Ch,018h,07Eh,018h,018h,000h,000h,000h,000h  ; 00C
DB  000h,000h,03Fh,033h,03Fh,030h,030h,030h,030h,070h,0F0h,0E0h,000h,000h,000h,000h  ; 00D
DB  000h,000h,07Fh,063h,07Fh,063h,063h,063h,063h,067h,0E7h,0E6h,0C0h,000h,000h,000h  ; 00E
DB  000h,000h,000h,018h,018h,0DBh,03Ch,0E7h,03Ch,0DBh,018h,018h,000h,000h,000h,000h  ; 00F
DB  000h,080h,0C0h,0E0h,0F0h,0F8h,0FEh,0F8h,0F0h,0E0h,0C0h,080h,000h,000h,000h,000h  ; 010
DB  000h,002h,006h,00Eh,01Eh,03Eh,0FEh,03Eh,01Eh,00Eh,006h,002h,000h,000h,000h,000h  ; 011
DB  000h,000h,018h,03Ch,07Eh,018h,018h,018h,07Eh,03Ch,018h,000h,000h,000h,000h,000h  ; 012
DB  000h,000h,066h,066h,066h,066h,066h,066h,066h,000h,066h,066h,000h,000h,000h,000h  ; 013
DB  000h,000h,07Fh,0DBh,0DBh,0DBh,07Bh,01Bh,01Bh,01Bh,01Bh,01Bh,000h,000h,000h,000h  ; 014
DB  000h,07Ch,0C6h,060h,038h,06Ch,0C6h,0C6h,06Ch,038h,00Ch,0C6h,07Ch,000h,000h,000h  ; 015
DB  000h,000h,000h,000h,000h,000h,000h,000h,0FEh,0FEh,0FEh,0FEh,000h,000h,000h,000h  ; 016
DB  000h,000h,018h,03Ch,07Eh,018h,018h,018h,07Eh,03Ch,018h,07Eh,000h,000h,000h,000h  ; 017
DB  000h,000h,018h,03Ch,07Eh,018h,018h,018h,018h,018h,018h,018h,000h,000h,000h,000h  ; 018
DB  000h,000h,018h,018h,018h,018h,018h,018h,018h,07Eh,03Ch,018h,000h,000h,000h,000h  ; 019
DB  000h,000h,000h,000h,000h,018h,00Ch,0FEh,00Ch,018h,000h,000h,000h,000h,000h,000h  ; 01A
DB  000h,000h,000h,000h,000h,030h,060h,0FEh,060h,030h,000h,000h,000h,000h,000h,000h  ; 01B
DB  000h,000h,000h,000h,000h,000h,0C0h,0C0h,0C0h,0FEh,000h,000h,000h,000h,000h,000h  ; 01C
DB  000h,000h,000h,000h,000h,028h,06Ch,0FEh,06Ch,028h,000h,000h,000h,000h,000h,000h  ; 01D
DB  000h,000h,000h,000h,010h,038h,038h,07Ch,07Ch,0FEh,0FEh,000h,000h,000h,000h,000h  ; 01E
DB  000h,000h,000h,000h,0FEh,0FEh,07Ch,07Ch,038h,038h,010h,000h,000h,000h,000h,000h  ; 01F
DB  000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 020
DB  000h,000h,018h,03Ch,03Ch,03Ch,018h,018h,018h,000h,018h,018h,000h,000h,000h,000h  ; 021
DB  000h,066h,066h,066h,024h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 022
DB  000h,000h,000h,06Ch,06Ch,0FEh,06Ch,06Ch,06Ch,0FEh,06Ch,06Ch,000h,000h,000h,000h  ; 023
DB  018h,018h,07Ch,0C6h,0C2h,0C0h,07Ch,006h,006h,086h,0C6h,07Ch,018h,018h,000h,000h  ; 024
DB  000h,000h,000h,000h,0C2h,0C6h,00Ch,018h,030h,060h,0C6h,086h,000h,000h,000h,000h  ; 025
DB  000h,000h,038h,06Ch,06Ch,038h,076h,0DCh,0CCh,0CCh,0CCh,076h,000h,000h,000h,000h  ; 026
DB  000h,030h,030h,030h,060h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 027
DB  000h,000h,00Ch,018h,030h,030h,030h,030h,030h,030h,018h,00Ch,000h,000h,000h,000h  ; 028
DB  000h,000h,030h,018h,00Ch,00Ch,00Ch,00Ch,00Ch,00Ch,018h,030h,000h,000h,000h,000h  ; 029
DB  000h,000h,000h,000h,000h,066h,03Ch,0FFh,03Ch,066h,000h,000h,000h,000h,000h,000h  ; 02A
DB  000h,000h,000h,000h,000h,018h,018h,07Eh,018h,018h,000h,000h,000h,000h,000h,000h  ; 02B
DB  000h,000h,000h,000h,000h,000h,000h,000h,000h,018h,018h,018h,030h,000h,000h,000h  ; 02C
DB  000h,000h,000h,000h,000h,000h,000h,0FEh,000h,000h,000h,000h,000h,000h,000h,000h  ; 02D
DB  000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,018h,018h,000h,000h,000h,000h  ; 02E
DB  000h,000h,000h,000h,002h,006h,00Ch,018h,030h,060h,0C0h,080h,000h,000h,000h,000h  ; 02F
DB  000h,000h,038h,06Ch,0C6h,0C6h,0D6h,0D6h,0C6h,0C6h,06Ch,038h,000h,000h,000h,000h  ; 030
DB  000h,000h,018h,038h,078h,018h,018h,018h,018h,018h,018h,07Eh,000h,000h,000h,000h  ; 031
DB  000h,000h,07Ch,0C6h,006h,00Ch,018h,030h,060h,0C0h,0C6h,0FEh,000h,000h,000h,000h  ; 032
DB  000h,000h,07Ch,0C6h,006h,006h,03Ch,006h,006h,006h,0C6h,07Ch,000h,000h,000h,000h  ; 033
DB  000h,000h,00Ch,01Ch,03Ch,06Ch,0CCh,0FEh,00Ch,00Ch,00Ch,01Eh,000h,000h,000h,000h  ; 034
DB  000h,000h,0FEh,0C0h,0C0h,0C0h,0FCh,006h,006h,006h,0C6h,07Ch,000h,000h,000h,000h  ; 035
DB  000h,000h,038h,060h,0C0h,0C0h,0FCh,0C6h,0C6h,0C6h,0C6h,07Ch,000h,000h,000h,000h  ; 036
DB  000h,000h,0FEh,0C6h,006h,006h,00Ch,018h,030h,030h,030h,030h,000h,000h,000h,000h  ; 037
DB  000h,000h,07Ch,0C6h,0C6h,0C6h,07Ch,0C6h,0C6h,0C6h,0C6h,07Ch,000h,000h,000h,000h  ; 038
DB  000h,000h,07Ch,0C6h,0C6h,0C6h,07Eh,006h,006h,006h,00Ch,078h,000h,000h,000h,000h  ; 039
DB  000h,000h,000h,000h,018h,018h,000h,000h,000h,018h,018h,000h,000h,000h,000h,000h  ; 03A
DB  000h,000h,000h,000h,018h,018h,000h,000h,000h,018h,018h,030h,000h,000h,000h,000h  ; 03B
DB  000h,000h,000h,006h,00Ch,018h,030h,060h,030h,018h,00Ch,006h,000h,000h,000h,000h  ; 03C
DB  000h,000h,000h,000h,000h,07Eh,000h,000h,07Eh,000h,000h,000h,000h,000h,000h,000h  ; 03D
DB  000h,000h,000h,060h,030h,018h,00Ch,006h,00Ch,018h,030h,060h,000h,000h,000h,000h  ; 03E
DB  000h,000h,07Ch,0C6h,0C6h,00Ch,018h,018h,018h,000h,018h,018h,000h,000h,000h,000h  ; 03F
DB  000h,000h,000h,07Ch,0C6h,0C6h,0DEh,0DEh,0DEh,0DCh,0C0h,07Ch,000h,000h,000h,000h  ; 040
DB  000h,000h,010h,038h,06Ch,0C6h,0C6h,0FEh,0C6h,0C6h,0C6h,0C6h,000h,000h,000h,000h  ; 041
DB  000h,000h,0FCh,066h,066h,066h,07Ch,066h,066h,066h,066h,0FCh,000h,000h,000h,000h  ; 042
DB  000h,000h,03Ch,066h,0C2h,0C0h,0C0h,0C0h,0C0h,0C2h,066h,03Ch,000h,000h,000h,000h  ; 043
DB  000h,000h,0F8h,06Ch,066h,066h,066h,066h,066h,066h,06Ch,0F8h,000h,000h,000h,000h  ; 044
DB  000h,000h,0FEh,066h,062h,068h,078h,068h,060h,062h,066h,0FEh,000h,000h,000h,000h  ; 045
DB  000h,000h,0FEh,066h,062h,068h,078h,068h,060h,060h,060h,0F0h,000h,000h,000h,000h  ; 046
DB  000h,000h,03Ch,066h,0C2h,0C0h,0C0h,0DEh,0C6h,0C6h,066h,03Ah,000h,000h,000h,000h  ; 047
DB  000h,000h,0C6h,0C6h,0C6h,0C6h,0FEh,0C6h,0C6h,0C6h,0C6h,0C6h,000h,000h,000h,000h  ; 048
DB  000h,000h,03Ch,018h,018h,018h,018h,018h,018h,018h,018h,03Ch,000h,000h,000h,000h  ; 049
DB  000h,000h,01Eh,00Ch,00Ch,00Ch,00Ch,00Ch,0CCh,0CCh,0CCh,078h,000h,000h,000h,000h  ; 04A
DB  000h,000h,0E6h,066h,066h,06Ch,078h,078h,06Ch,066h,066h,0E6h,000h,000h,000h,000h  ; 04B
DB  000h,000h,0F0h,060h,060h,060h,060h,060h,060h,062h,066h,0FEh,000h,000h,000h,000h  ; 04C
DB  000h,000h,0C6h,0EEh,0FEh,0FEh,0D6h,0C6h,0C6h,0C6h,0C6h,0C6h,000h,000h,000h,000h  ; 04D
DB  000h,000h,0C6h,0E6h,0F6h,0FEh,0DEh,0CEh,0C6h,0C6h,0C6h,0C6h,000h,000h,000h,000h  ; 04E
DB  000h,000h,07Ch,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,07Ch,000h,000h,000h,000h  ; 04F
DB  000h,000h,0FCh,066h,066h,066h,07Ch,060h,060h,060h,060h,0F0h,000h,000h,000h,000h  ; 050
DB  000h,000h,07Ch,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0D6h,0DEh,07Ch,00Ch,00Eh,000h,000h  ; 051
DB  000h,000h,0FCh,066h,066h,066h,07Ch,06Ch,066h,066h,066h,0E6h,000h,000h,000h,000h  ; 052
DB  000h,000h,07Ch,0C6h,0C6h,060h,038h,00Ch,006h,0C6h,0C6h,07Ch,000h,000h,000h,000h  ; 053
DB  000h,000h,07Eh,07Eh,05Ah,018h,018h,018h,018h,018h,018h,03Ch,000h,000h,000h,000h  ; 054
DB  000h,000h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,07Ch,000h,000h,000h,000h  ; 055
DB  000h,000h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,06Ch,038h,010h,000h,000h,000h,000h  ; 056
DB  000h,000h,0C6h,0C6h,0C6h,0C6h,0D6h,0D6h,0D6h,0FEh,0EEh,06Ch,000h,000h,000h,000h  ; 057
DB  000h,000h,0C6h,0C6h,06Ch,07Ch,038h,038h,07Ch,06Ch,0C6h,0C6h,000h,000h,000h,000h  ; 058
DB  000h,000h,066h,066h,066h,066h,03Ch,018h,018h,018h,018h,03Ch,000h,000h,000h,000h  ; 059
DB  000h,000h,0FEh,0C6h,086h,00Ch,018h,030h,060h,0C2h,0C6h,0FEh,000h,000h,000h,000h  ; 05A
DB  000h,000h,03Ch,030h,030h,030h,030h,030h,030h,030h,030h,03Ch,000h,000h,000h,000h  ; 05B
DB  000h,000h,000h,080h,0C0h,0E0h,070h,038h,01Ch,00Eh,006h,002h,000h,000h,000h,000h  ; 05C
DB  000h,000h,03Ch,00Ch,00Ch,00Ch,00Ch,00Ch,00Ch,00Ch,00Ch,03Ch,000h,000h,000h,000h  ; 05D
DB  010h,038h,06Ch,0C6h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 05E
DB  000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,0FFh,000h,000h  ; 05F
DB  030h,030h,018h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 060
DB  000h,000h,000h,000h,000h,078h,00Ch,07Ch,0CCh,0CCh,0CCh,076h,000h,000h,000h,000h  ; 061
DB  000h,000h,0E0h,060h,060h,078h,06Ch,066h,066h,066h,066h,07Ch,000h,000h,000h,000h  ; 062
DB  000h,000h,000h,000h,000h,07Ch,0C6h,0C0h,0C0h,0C0h,0C6h,07Ch,000h,000h,000h,000h  ; 063
DB  000h,000h,01Ch,00Ch,00Ch,03Ch,06Ch,0CCh,0CCh,0CCh,0CCh,076h,000h,000h,000h,000h  ; 064
DB  000h,000h,000h,000h,000h,07Ch,0C6h,0FEh,0C0h,0C0h,0C6h,07Ch,000h,000h,000h,000h  ; 065
DB  000h,000h,038h,06Ch,064h,060h,0F0h,060h,060h,060h,060h,0F0h,000h,000h,000h,000h  ; 066
DB  000h,000h,000h,000h,000h,076h,0CCh,0CCh,0CCh,0CCh,0CCh,07Ch,00Ch,0CCh,078h,000h  ; 067
DB  000h,000h,0E0h,060h,060h,06Ch,076h,066h,066h,066h,066h,0E6h,000h,000h,000h,000h  ; 068
DB  000h,000h,018h,018h,000h,038h,018h,018h,018h,018h,018h,03Ch,000h,000h,000h,000h  ; 069
DB  000h,000h,006h,006h,000h,00Eh,006h,006h,006h,006h,006h,006h,066h,066h,03Ch,000h  ; 06A
DB  000h,000h,0E0h,060h,060h,066h,06Ch,078h,078h,06Ch,066h,0E6h,000h,000h,000h,000h  ; 06B
DB  000h,000h,038h,018h,018h,018h,018h,018h,018h,018h,018h,03Ch,000h,000h,000h,000h  ; 06C
DB  000h,000h,000h,000h,000h,0ECh,0FEh,0D6h,0D6h,0D6h,0D6h,0C6h,000h,000h,000h,000h  ; 06D
DB  000h,000h,000h,000h,000h,0DCh,066h,066h,066h,066h,066h,066h,000h,000h,000h,000h  ; 06E
DB  000h,000h,000h,000h,000h,07Ch,0C6h,0C6h,0C6h,0C6h,0C6h,07Ch,000h,000h,000h,000h  ; 06F
DB  000h,000h,000h,000h,000h,0DCh,066h,066h,066h,066h,066h,07Ch,060h,060h,0F0h,000h  ; 070
DB  000h,000h,000h,000h,000h,076h,0CCh,0CCh,0CCh,0CCh,0CCh,07Ch,00Ch,00Ch,01Eh,000h  ; 071
DB  000h,000h,000h,000h,000h,0DCh,076h,066h,060h,060h,060h,0F0h,000h,000h,000h,000h  ; 072
DB  000h,000h,000h,000h,000h,07Ch,0C6h,060h,038h,00Ch,0C6h,07Ch,000h,000h,000h,000h  ; 073
DB  000h,000h,010h,030h,030h,0FCh,030h,030h,030h,030h,036h,01Ch,000h,000h,000h,000h  ; 074
DB  000h,000h,000h,000h,000h,0CCh,0CCh,0CCh,0CCh,0CCh,0CCh,076h,000h,000h,000h,000h  ; 075
DB  000h,000h,000h,000h,000h,066h,066h,066h,066h,066h,03Ch,018h,000h,000h,000h,000h  ; 076
DB  000h,000h,000h,000h,000h,0C6h,0C6h,0D6h,0D6h,0D6h,0FEh,06Ch,000h,000h,000h,000h  ; 077
DB  000h,000h,000h,000h,000h,0C6h,06Ch,038h,038h,038h,06Ch,0C6h,000h,000h,000h,000h  ; 078
DB  000h,000h,000h,000h,000h,0C6h,0C6h,0C6h,0C6h,0C6h,0C6h,07Eh,006h,00Ch,0F8h,000h  ; 079
DB  000h,000h,000h,000h,000h,0FEh,0CCh,018h,030h,060h,0C6h,0FEh,000h,000h,000h,000h  ; 07A
DB  000h,000h,00Eh,018h,018h,018h,070h,018h,018h,018h,018h,00Eh,000h,000h,000h,000h  ; 07B
DB  000h,000h,018h,018h,018h,018h,000h,018h,018h,018h,018h,018h,000h,000h,000h,000h  ; 07C
DB  000h,000h,070h,018h,018h,018h,00Eh,018h,018h,018h,018h,070h,000h,000h,000h,000h  ; 07D
DB  000h,000h,076h,0DCh,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h,000h  ; 07E
DB  000h,000h,000h,000h,010h,038h,06Ch,0C6h,0C6h,0C6h,0FEh,000h,000h,000h,000h,000h  ; 07F

;--- Global variables ---------------------------------------------------------;

;--- Additional timings variables for current actual FPS and MBPS values ------;
align 8
fpsStamp1       dq      ?
fpsStamp2       dq      ?
mbpsStamp       dq      ?
mbpsOneBlock    dq      ?

;--- Structures for compact addressing at range base-128 ... base + 127 -------;
struct APPDATA
;--- Timings and benchmarks variables ---
tscFreq         dq      ?
tscPeriod       dq      ?
tscStart        dq      ?
tscStop         dq      ?
framesCount     dq      ?
tscBytes        dq      ?
bytesCount      dq      ?
;--- Temporary variables ---
rc              RECT    ?
;--- OpenGL variables ---
vertexShader    GLuint  ?
fragmentShader  GLuint  ?
shaderProgram   GLuint  ?
params          GLint   ?
VAO             GLuint  ?
VBO             GLuint  ?
IVBO            GLuint  ?
TXT1            GLuint  ?
;--- Display context variables ---
hdc             dq      ?
hrc             dq      ?
ends
;--- Pointers to OpenGL API functions, dynamical import used ------------------;
struct OGLAPI
ptr_glCreateShader             dq ?
ptr_glShaderSource             dq ?              
ptr_glCompileShader            dq ?             
ptr_glGetShaderiv              dq ?               
ptr_glGetShaderInfoLog         dq ?          
ptr_glCreateProgram            dq ?             
ptr_glAttachShader             dq ?              
ptr_glLinkProgram              dq ?               
ptr_glGetProgramiv             dq ?              
ptr_glGetProgramInfoLog        dq ?         
ptr_glDeleteShader             dq ?              
ptr_glGenVertexArrays          dq ?           
ptr_glGenBuffers               dq ?                
ptr_glBindVertexArray          dq ?           
ptr_glBindBuffer               dq ?                
ptr_glBufferData               dq ?                
ptr_glVertexAttribPointer      dq ?       
ptr_glEnableVertexAttribArray  dq ?   
ptr_glUseProgram               dq ?                
ptr_glDeleteVertexArrays       dq ?        
ptr_glDeleteBuffers            dq ?             
ptr_glGetUniformLocation       dq ?        
ptr_glUniformMatrix4fv         dq ?          
ptr_glGenerateMipmap           dq ?            
ptr_glUniform1i                dq ?                 
ptr_glActiveTexture            dq ?             
ptr_glDrawArraysInstanced      dq ?       
ptr_glVertexAttribDivisor      dq ?       
ends
align 8
APP_DATA APPDATA ?
OGL_API  OGLAPI  ?

;--- Application and GUI window context variables -----------------------------;
msg             MSG   ?
pfd             PIXELFORMATDESCRIPTOR  ?
;--- Instancing array for CPU-GPU traffic -------------------------------------;
align 16
scales  GLfloat  INSTANCING_COUNT DUP (?)  
;--- GUI and GDI+ variables ---------------------------------------------------;
startupInput  GdiplusStartupInput  ?
gdiplusToken  DQ  ?
pBitmapStruc  DQ  ?
hBitmap       DQ  ?
pRawImage     DQ  ?
pStream       DQ  ?
gpBitmap      DQ  ?
hGlobal       DQ  ?
hIcon         DQ  ?
bm            BITMAP
;--- Universal temporary buffers ----------------------------------------------;
align 16
tempBuffer1  DB  TEMP_BUFFER_SIZE  DUP (?)
tempBuffer2  DB  TEMP_BUFFER_SIZE  DUP (?)

;---------- Import section ----------------------------------------------------;

section '.idata' import data readable writeable

  library kernel,'KERNEL32.DLL',\
	  user,'USER32.DLL',\
	  gdi,'GDI32.DLL',\
    gdiplus,'GDIPLUS.DLL',\
	  opengl,'OPENGL32.DLL',\
	  glu,'GLU32.DLL',\
    ole,'OLE32.DLL'

  import kernel,\
	 GetModuleHandle,'GetModuleHandleA',\
   GetSystemTimeAsFileTime, 'GetSystemTimeAsFileTime',\
	 ExitProcess,'ExitProcess',\
   MultiByteToWideChar,'MultiByteToWideChar',\
   GetModuleHandleA,'GetModuleHandleA',\
   FindResourceA,'FindResourceA',\
   SizeofResource,'SizeofResource',\
   LoadResource,'LoadResource',\
   LockResource,'LockResource',\
   GlobalAlloc,'GlobalAlloc',\
   GlobalLock,'GlobalLock',\
   GlobalFree,'GlobalFree'

  import user,\
	 RegisterClass,'RegisterClassA',\
	 CreateWindowEx,'CreateWindowExA',\
	 DefWindowProc,'DefWindowProcA',\
	 GetMessage,'GetMessageA',\
	 TranslateMessage,'TranslateMessage',\
	 DispatchMessage,'DispatchMessageA',\
	 LoadCursor,'LoadCursorA',\
	 LoadIcon,'LoadIconA',\
	 GetClientRect,'GetClientRect',\
	 GetDC,'GetDC',\
	 ReleaseDC,'ReleaseDC',\
	 PostQuitMessage,'PostQuitMessage',\
   MessageBoxA,'MessageBoxA',\
   DestroyWindow,'DestroyWindow',\
   SendMessageA,'SendMessageA',\
   DrawTextA,'DrawTextA'

  import gdi,\
	 ChoosePixelFormat,'ChoosePixelFormat',\
	 SetPixelFormat,'SetPixelFormat',\
	 SwapBuffers,'SwapBuffers',\
   GetObjectA,'GetObjectA',\
   DeleteObject,'DeleteObject'
   
  import gdiplus,\
   GdiplusStartup,'GdiplusStartup',\
   GdiplusShutdown,'GdiplusShutdown',\
   GdipCreateBitmapFromStream,'GdipCreateBitmapFromStream',\
   GdipCreateFromHWND,'GdipCreateFromHWND',\
   GdipDrawImageRectI,'GdipDrawImageRectI',\
   GdipGetImageDimension,'GdipGetImageDimension',\
   GdipDeleteGraphics,'GdipDeleteGraphics',\
   GdipDisposeImage,'GdipDisposeImage',\
   GdipGetImageWidth,'GdipGetImageWidth',\
   GdipGetImageHeight,'GdipGetImageHeight',\
   GdipCreateBitmapFromFile,'GdipCreateBitmapFromFile',\
   GdipCreateHBITMAPFromBitmap,'GdipCreateHBITMAPFromBitmap'

  import opengl,\
	 glAccum,'glAccum',\
	 glAlphaFunc,'glAlphaFunc',\
	 glAreTexturesResident,'glAreTexturesResident',\
	 glArrayElement,'glArrayElement',\
	 glBegin,'glBegin',\
	 glBindTexture,'glBindTexture',\
	 glBitmap,'glBitmap',\
	 glBlendFunc,'glBlendFunc',\
	 glCallList,'glCallList',\
	 glCallLists,'glCallLists',\
	 glClear,'glClear',\
	 glClearAccum,'glClearAccum',\
	 glClearColor,'glClearColor',\
	 glClearDepth,'glClearDepth',\
	 glClearIndex,'glClearIndex',\
	 glClearStencil,'glClearStencil',\
	 glClipPlane,'glClipPlane',\
	 glColor3b,'glColor3b',\
	 glColor3bv,'glColor3bv',\
	 glColor3d,'glColor3d',\
	 glColor3dv,'glColor3dv',\
	 glColor3f,'glColor3f',\
	 glColor3fv,'glColor3fv',\
	 glColor3i,'glColor3i',\
	 glColor3iv,'glColor3iv',\
	 glColor3s,'glColor3s',\
	 glColor3sv,'glColor3sv',\
	 glColor3ub,'glColor3ub',\
	 glColor3ubv,'glColor3ubv',\
	 glColor3ui,'glColor3ui',\
	 glColor3uiv,'glColor3uiv',\
	 glColor3us,'glColor3us',\
	 glColor3usv,'glColor3usv',\
	 glColor4b,'glColor4b',\
	 glColor4bv,'glColor4bv',\
	 glColor4d,'glColor4d',\
	 glColor4dv,'glColor4dv',\
	 glColor4f,'glColor4f',\
	 glColor4fv,'glColor4fv',\
	 glColor4i,'glColor4i',\
	 glColor4iv,'glColor4iv',\
	 glColor4s,'glColor4s',\
	 glColor4sv,'glColor4sv',\
	 glColor4ub,'glColor4ub',\
	 glColor4ubv,'glColor4ubv',\
	 glColor4ui,'glColor4ui',\
	 glColor4uiv,'glColor4uiv',\
	 glColor4us,'glColor4us',\
	 glColor4usv,'glColor4usv',\
	 glColorMask,'glColorMask',\
	 glColorMaterial,'glColorMaterial',\
	 glColorPointer,'glColorPointer',\
	 glCopyPixels,'glCopyPixels',\
	 glCopyTexImage1D,'glCopyTexImage1D',\
	 glCopyTexImage2D,'glCopyTexImage2D',\
	 glCopyTexSubImage1D,'glCopyTexSubImage1D',\
	 glCopyTexSubImage2D,'glCopyTexSubImage2D',\
	 glCullFace,'glCullFace',\
	 glDeleteLists,'glDeleteLists',\
	 glDeleteTextures,'glDeleteTextures',\
	 glDepthFunc,'glDepthFunc',\
	 glDepthMask,'glDepthMask',\
	 glDepthRange,'glDepthRange',\
	 glDisable,'glDisable',\
	 glDisableClientState,'glDisableClientState',\
	 glDrawArrays,'glDrawArrays',\
	 glDrawBuffer,'glDrawBuffer',\
	 glDrawElements,'glDrawElements',\
	 glDrawPixels,'glDrawPixels',\
	 glEdgeFlag,'glEdgeFlag',\
	 glEdgeFlagPointer,'glEdgeFlagPointer',\
	 glEdgeFlagv,'glEdgeFlagv',\
	 glEnable,'glEnable',\
	 glEnableClientState,'glEnableClientState',\
	 glEnd,'glEnd',\
	 glEndList,'glEndList',\
	 glEvalCoord1d,'glEvalCoord1d',\
	 glEvalCoord1dv,'glEvalCoord1dv',\
	 glEvalCoord1f,'glEvalCoord1f',\
	 glEvalCoord1fv,'glEvalCoord1fv',\
	 glEvalCoord2d,'glEvalCoord2d',\
	 glEvalCoord2dv,'glEvalCoord2dv',\
	 glEvalCoord2f,'glEvalCoord2f',\
	 glEvalCoord2fv,'glEvalCoord2fv',\
	 glEvalMesh1,'glEvalMesh1',\
	 glEvalMesh2,'glEvalMesh2',\
	 glEvalPoint1,'glEvalPoint1',\
	 glEvalPoint2,'glEvalPoint2',\
	 glFeedbackBuffer,'glFeedbackBuffer',\
	 glFinish,'glFinish',\
	 glFlush,'glFlush',\
	 glFogf,'glFogf',\
	 glFogfv,'glFogfv',\
	 glFogi,'glFogi',\
	 glFogiv,'glFogiv',\
	 glFrontFace,'glFrontFace',\
	 glFrustum,'glFrustum',\
	 glGenLists,'glGenLists',\
	 glGenTextures,'glGenTextures',\
	 glGetBooleanv,'glGetBooleanv',\
	 glGetClipPlane,'glGetClipPlane',\
	 glGetDoublev,'glGetDoublev',\
	 glGetError,'glGetError',\
	 glGetFloatv,'glGetFloatv',\
	 glGetIntegerv,'glGetIntegerv',\
	 glGetLightfv,'glGetLightfv',\
	 glGetLightiv,'glGetLightiv',\
	 glGetMapdv,'glGetMapdv',\
	 glGetMapfv,'glGetMapfv',\
	 glGetMapiv,'glGetMapiv',\
	 glGetMaterialfv,'glGetMaterialfv',\
	 glGetMaterialiv,'glGetMaterialiv',\
	 glGetPixelMapfv,'glGetPixelMapfv',\
	 glGetPixelMapuiv,'glGetPixelMapuiv',\
	 glGetPixelMapusv,'glGetPixelMapusv',\
	 glGetPointerv,'glGetPointerv',\
	 glGetPolygonStipple,'glGetPolygonStipple',\
	 glGetString,'glGetString',\
	 glGetTexEnvfv,'glGetTexEnvfv',\
	 glGetTexEnviv,'glGetTexEnviv',\
	 glGetTexGendv,'glGetTexGendv',\
	 glGetTexGenfv,'glGetTexGenfv',\
	 glGetTexGeniv,'glGetTexGeniv',\
	 glGetTexImage,'glGetTexImage',\
	 glGetTexLevelParameterfv,'glGetTexLevelParameterfv',\
	 glGetTexLevelParameteriv,'glGetTexLevelParameteriv',\
	 glGetTexParameterfv,'glGetTexParameterfv',\
	 glGetTexParameteriv,'glGetTexParameteriv',\
	 glHint,'glHint',\
	 glIndexMask,'glIndexMask',\
	 glIndexPointer,'glIndexPointer',\
	 glIndexd,'glIndexd',\
	 glIndexdv,'glIndexdv',\
	 glIndexf,'glIndexf',\
	 glIndexfv,'glIndexfv',\
	 glIndexi,'glIndexi',\
	 glIndexiv,'glIndexiv',\
	 glIndexs,'glIndexs',\
	 glIndexsv,'glIndexsv',\
	 glIndexub,'glIndexub',\
	 glIndexubv,'glIndexubv',\
	 glInitNames,'glInitNames',\
	 glInterleavedArrays,'glInterleavedArrays',\
	 glIsEnabled,'glIsEnabled',\
	 glIsList,'glIsList',\
	 glIsTexture,'glIsTexture',\
	 glLightModelf,'glLightModelf',\
	 glLightModelfv,'glLightModelfv',\
	 glLightModeli,'glLightModeli',\
	 glLightModeliv,'glLightModeliv',\
	 glLightf,'glLightf',\
	 glLightfv,'glLightfv',\
	 glLighti,'glLighti',\
	 glLightiv,'glLightiv',\
	 glLineStipple,'glLineStipple',\
	 glLineWidth,'glLineWidth',\
	 glListBase,'glListBase',\
	 glLoadIdentity,'glLoadIdentity',\
	 glLoadMatrixd,'glLoadMatrixd',\
	 glLoadMatrixf,'glLoadMatrixf',\
	 glLoadName,'glLoadName',\
	 glLogicOp,'glLogicOp',\
	 glMap1d,'glMap1d',\
	 glMap1f,'glMap1f',\
	 glMap2d,'glMap2d',\
	 glMap2f,'glMap2f',\
	 glMapGrid1d,'glMapGrid1d',\
	 glMapGrid1f,'glMapGrid1f',\
	 glMapGrid2d,'glMapGrid2d',\
	 glMapGrid2f,'glMapGrid2f',\
	 glMaterialf,'glMaterialf',\
	 glMaterialfv,'glMaterialfv',\
	 glMateriali,'glMateriali',\
	 glMaterialiv,'glMaterialiv',\
	 glMatrixMode,'glMatrixMode',\
	 glMultMatrixd,'glMultMatrixd',\
	 glMultMatrixf,'glMultMatrixf',\
	 glNewList,'glNewList',\
	 glNormal3b,'glNormal3b',\
	 glNormal3bv,'glNormal3bv',\
	 glNormal3d,'glNormal3d',\
	 glNormal3dv,'glNormal3dv',\
	 glNormal3f,'glNormal3f',\
	 glNormal3fv,'glNormal3fv',\
	 glNormal3i,'glNormal3i',\
	 glNormal3iv,'glNormal3iv',\
	 glNormal3s,'glNormal3s',\
	 glNormal3sv,'glNormal3sv',\
	 glNormalPointer,'glNormalPointer',\
	 glOrtho,'glOrtho',\
	 glPassThrough,'glPassThrough',\
	 glPixelMapfv,'glPixelMapfv',\
	 glPixelMapuiv,'glPixelMapuiv',\
	 glPixelMapusv,'glPixelMapusv',\
	 glPixelStoref,'glPixelStoref',\
	 glPixelStorei,'glPixelStorei',\
	 glPixelTransferf,'glPixelTransferf',\
	 glPixelTransferi,'glPixelTransferi',\
	 glPixelZoom,'glPixelZoom',\
	 glPointSize,'glPointSize',\
	 glPolygonMode,'glPolygonMode',\
	 glPolygonOffset,'glPolygonOffset',\
	 glPolygonStipple,'glPolygonStipple',\
	 glPopAttrib,'glPopAttrib',\
	 glPopClientAttrib,'glPopClientAttrib',\
	 glPopMatrix,'glPopMatrix',\
	 glPopName,'glPopName',\
	 glPrioritizeTextures,'glPrioritizeTextures',\
	 glPushAttrib,'glPushAttrib',\
	 glPushClientAttrib,'glPushClientAttrib',\
	 glPushMatrix,'glPushMatrix',\
	 glPushName,'glPushName',\
	 glRasterPos2d,'glRasterPos2d',\
	 glRasterPos2dv,'glRasterPos2dv',\
	 glRasterPos2f,'glRasterPos2f',\
	 glRasterPos2fv,'glRasterPos2fv',\
	 glRasterPos2i,'glRasterPos2i',\
	 glRasterPos2iv,'glRasterPos2iv',\
	 glRasterPos2s,'glRasterPos2s',\
	 glRasterPos2sv,'glRasterPos2sv',\
	 glRasterPos3d,'glRasterPos3d',\
	 glRasterPos3dv,'glRasterPos3dv',\
	 glRasterPos3f,'glRasterPos3f',\
	 glRasterPos3fv,'glRasterPos3fv',\
	 glRasterPos3i,'glRasterPos3i',\
	 glRasterPos3iv,'glRasterPos3iv',\
	 glRasterPos3s,'glRasterPos3s',\
	 glRasterPos3sv,'glRasterPos3sv',\
	 glRasterPos4d,'glRasterPos4d',\
	 glRasterPos4dv,'glRasterPos4dv',\
	 glRasterPos4f,'glRasterPos4f',\
	 glRasterPos4fv,'glRasterPos4fv',\
	 glRasterPos4i,'glRasterPos4i',\
	 glRasterPos4iv,'glRasterPos4iv',\
	 glRasterPos4s,'glRasterPos4s',\
	 glRasterPos4sv,'glRasterPos4sv',\
	 glReadBuffer,'glReadBuffer',\
	 glReadPixels,'glReadPixels',\
	 glRectd,'glRectd',\
	 glRectdv,'glRectdv',\
	 glRectf,'glRectf',\
	 glRectfv,'glRectfv',\
	 glRecti,'glRecti',\
	 glRectiv,'glRectiv',\
	 glRects,'glRects',\
	 glRectsv,'glRectsv',\
	 glRenderMode,'glRenderMode',\
	 glRotated,'glRotated',\
	 glRotatef,'glRotatef',\
	 glScaled,'glScaled',\
	 glScalef,'glScalef',\
	 glScissor,'glScissor',\
	 glSelectBuffer,'glSelectBuffer',\
	 glShadeModel,'glShadeModel',\
	 glStencilFunc,'glStencilFunc',\
	 glStencilMask,'glStencilMask',\
	 glStencilOp,'glStencilOp',\
	 glTexCoord1d,'glTexCoord1d',\
	 glTexCoord1dv,'glTexCoord1dv',\
	 glTexCoord1f,'glTexCoord1f',\
	 glTexCoord1fv,'glTexCoord1fv',\
	 glTexCoord1i,'glTexCoord1i',\
	 glTexCoord1iv,'glTexCoord1iv',\
	 glTexCoord1s,'glTexCoord1s',\
	 glTexCoord1sv,'glTexCoord1sv',\
	 glTexCoord2d,'glTexCoord2d',\
	 glTexCoord2dv,'glTexCoord2dv',\
	 glTexCoord2f,'glTexCoord2f',\
	 glTexCoord2fv,'glTexCoord2fv',\
	 glTexCoord2i,'glTexCoord2i',\
	 glTexCoord2iv,'glTexCoord2iv',\
	 glTexCoord2s,'glTexCoord2s',\
	 glTexCoord2sv,'glTexCoord2sv',\
	 glTexCoord3d,'glTexCoord3d',\
	 glTexCoord3dv,'glTexCoord3dv',\
	 glTexCoord3f,'glTexCoord3f',\
	 glTexCoord3fv,'glTexCoord3fv',\
	 glTexCoord3i,'glTexCoord3i',\
	 glTexCoord3iv,'glTexCoord3iv',\
	 glTexCoord3s,'glTexCoord3s',\
	 glTexCoord3sv,'glTexCoord3sv',\
	 glTexCoord4d,'glTexCoord4d',\
	 glTexCoord4dv,'glTexCoord4dv',\
	 glTexCoord4f,'glTexCoord4f',\
	 glTexCoord4fv,'glTexCoord4fv',\
	 glTexCoord4i,'glTexCoord4i',\
	 glTexCoord4iv,'glTexCoord4iv',\
	 glTexCoord4s,'glTexCoord4s',\
	 glTexCoord4sv,'glTexCoord4sv',\
	 glTexCoordPointer,'glTexCoordPointer',\
	 glTexEnvf,'glTexEnvf',\
	 glTexEnvfv,'glTexEnvfv',\
	 glTexEnvi,'glTexEnvi',\
	 glTexEnviv,'glTexEnviv',\
	 glTexGend,'glTexGend',\
	 glTexGendv,'glTexGendv',\
	 glTexGenf,'glTexGenf',\
	 glTexGenfv,'glTexGenfv',\
	 glTexGeni,'glTexGeni',\
	 glTexGeniv,'glTexGeniv',\
	 glTexImage1D,'glTexImage1D',\
	 glTexImage2D,'glTexImage2D',\
	 glTexParameterf,'glTexParameterf',\
	 glTexParameterfv,'glTexParameterfv',\
	 glTexParameteri,'glTexParameteri',\
	 glTexParameteriv,'glTexParameteriv',\
	 glTexSubImage1D,'glTexSubImage1D',\
	 glTexSubImage2D,'glTexSubImage2D',\
	 glTranslated,'glTranslated',\
	 glTranslatef,'glTranslatef',\
	 glVertex2d,'glVertex2d',\
	 glVertex2dv,'glVertex2dv',\
	 glVertex2f,'glVertex2f',\
	 glVertex2fv,'glVertex2fv',\
	 glVertex2i,'glVertex2i',\
	 glVertex2iv,'glVertex2iv',\
	 glVertex2s,'glVertex2s',\
	 glVertex2sv,'glVertex2sv',\
	 glVertex3d,'glVertex3d',\
	 glVertex3dv,'glVertex3dv',\
	 glVertex3f,'glVertex3f',\
	 glVertex3fv,'glVertex3fv',\
	 glVertex3i,'glVertex3i',\
	 glVertex3iv,'glVertex3iv',\
	 glVertex3s,'glVertex3s',\
	 glVertex3sv,'glVertex3sv',\
	 glVertex4d,'glVertex4d',\
	 glVertex4dv,'glVertex4dv',\
	 glVertex4f,'glVertex4f',\
	 glVertex4fv,'glVertex4fv',\
	 glVertex4i,'glVertex4i',\
	 glVertex4iv,'glVertex4iv',\
	 glVertex4s,'glVertex4s',\
	 glVertex4sv,'glVertex4sv',\
	 glVertexPointer,'glVertexPointer',\
	 glViewport,'glViewport',\
	 wglGetProcAddress,'wglGetProcAddress',\
	 wglCopyContext,'wglCopyContext',\
	 wglCreateContext,'wglCreateContext',\
	 wglCreateLayerContext,'wglCreateLayerContext',\
	 wglDeleteContext,'wglDeleteContext',\
	 wglDescribeLayerPlane,'wglDescribeLayerPlane',\
	 wglGetCurrentContext,'wglGetCurrentContext',\
	 wglGetCurrentDC,'wglGetCurrentDC',\
	 wglGetLayerPaletteEntries,'wglGetLayerPaletteEntries',\
	 wglMakeCurrent,'wglMakeCurrent',\
	 wglRealizeLayerPalette,'wglRealizeLayerPalette',\
	 wglSetLayerPaletteEntries,'wglSetLayerPaletteEntries',\
	 wglShareLists,'wglShareLists',\
	 wglSwapLayerBuffers,'wglSwapLayerBuffers',\
	 wglSwapMultipleBuffers,'wglSwapMultipleBuffers',\
	 wglUseFontBitmapsA,'wglUseFontBitmapsA',\
	 wglUseFontOutlinesA,'wglUseFontOutlinesA',\
	 wglUseFontBitmapsW,'wglUseFontBitmapsW',\
	 wglUseFontOutlinesW,'wglUseFontOutlinesW',\
	 glDrawRangeElements,'glDrawRangeElements',\
	 glTexImage3D,'glTexImage3D',\
	 glBlendColor,'glBlendColor',\
	 glBlendEquation,'glBlendEquation',\
	 glColorSubTable,'glColorSubTable',\
	 glCopyColorSubTable,'glCopyColorSubTable',\
	 glColorTable,'glColorTable',\
	 glCopyColorTable,'glCopyColorTable',\
	 glColorTableParameteriv,'glColorTableParameteriv',\
	 glColorTableParameterfv,'glColorTableParameterfv',\
	 glGetColorTable,'glGetColorTable',\
	 glGetColorTableParameteriv,'glGetColorTableParameteriv',\
	 glGetColorTableParameterfv,'glGetColorTableParameterfv',\
	 glConvolutionFilter1D,'glConvolutionFilter1D',\
	 glConvolutionFilter2D,'glConvolutionFilter2D',\
	 glCopyConvolutionFilter1D,'glCopyConvolutionFilter1D',\
	 glCopyConvolutionFilter2D,'glCopyConvolutionFilter2D',\
	 glGetConvolutionFilter,'glGetConvolutionFilter',\
	 glSeparableFilter2D,'glSeparableFilter2D',\
	 glGetSeparableFilter,'glGetSeparableFilter',\
	 glConvolutionParameteri,'glConvolutionParameteri',\
	 glConvolutionParameteriv,'glConvolutionParameteriv',\
	 glConvolutionParameterf,'glConvolutionParameterf',\
	 glConvolutionParameterfv,'glConvolutionParameterfv',\
	 glGetConvolutionParameteriv,'glGetConvolutionParameteriv',\
	 glGetConvolutionParameterfv,'glGetConvolutionParameterfv',\
	 glHistogram,'glHistogram',\
	 glResetHistogram,'glResetHistogram',\
	 glGetHistogram,'glGetHistogram',\
	 glGetHistogramParameteriv,'glGetHistogramParameteriv',\
	 glGetHistogramParameterfv,'glGetHistogramParameterfv',\
	 glMinmax,'glMinmax',\
	 glResetMinmax,'glResetMinmax',\
	 glGetMinmax,'glGetMinmax',\
	 glGetMinmaxParameteriv,'glGetMinmaxParameteriv',\
	 glGetMinmaxParameterfv,'glGetMinmaxParameterfv'

  import glu,\
	 gluBeginCurve,'gluBeginCurve',\
	 gluBeginPolygon,'gluBeginPolygon',\
	 gluBeginSurface,'gluBeginSurface',\
	 gluBeginTrim,'gluBeginTrim',\
	 gluBuild1DMipmaps,'gluBuild1DMipmaps',\
	 gluBuild2DMipmaps,'gluBuild2DMipmaps',\
	 gluCylinder,'gluCylinder',\
	 gluDeleteNurbsRenderer,'gluDeleteNurbsRenderer',\
	 gluDeleteQuadric,'gluDeleteQuadric',\
	 gluDeleteTess,'gluDeleteTess',\
	 gluDisk,'gluDisk',\
	 gluEndCurve,'gluEndCurve',\
	 gluEndPolygon,'gluEndPolygon',\
	 gluEndSurface,'gluEndSurface',\
	 gluEndTrim,'gluEndTrim',\
	 gluErrorString,'gluErrorString',\
	 gluGetNurbsProperty,'gluGetNurbsProperty',\
	 gluGetString,'gluGetString',\
	 gluGetTessProperty,'gluGetTessProperty',\
	 gluLoadSamplingMatrices,'gluLoadSamplingMatrices',\
	 gluLookAt,'gluLookAt',\
	 gluNewNurbsRenderer,'gluNewNurbsRenderer',\
	 gluNewQuadric,'gluNewQuadric',\
	 gluNewTess,'gluNewTess',\
	 gluNextContour,'gluNextContour',\
	 gluNurbsCallback,'gluNurbsCallback',\
	 gluNurbsCurve,'gluNurbsCurve',\
	 gluNurbsProperty,'gluNurbsProperty',\
	 gluNurbsSurface,'gluNurbsSurface',\
	 gluOrtho2D,'gluOrtho2D',\
	 gluPartialDisk,'gluPartialDisk',\
	 gluPerspective,'gluPerspective',\
	 gluPickMatrix,'gluPickMatrix',\
	 gluProject,'gluProject',\
	 gluPwlCurve,'gluPwlCurve',\
	 gluQuadricCallback,'gluQuadricCallback',\
	 gluQuadricDrawStyle,'gluQuadricDrawStyle',\
	 gluQuadricNormals,'gluQuadricNormals',\
	 gluQuadricOrientation,'gluQuadricOrientation',\
	 gluQuadricTexture,'gluQuadricTexture',\
	 gluScaleImage,'gluScaleImage',\
	 gluSphere,'gluSphere',\
	 gluTessBeginContour,'gluTessBeginContour',\
	 gluTessBeginPolygon,'gluTessBeginPolygon',\
	 gluTessCallback,'gluTessCallback',\
	 gluTessEndContour,'gluTessEndContour',\
	 gluTessEndPolygon,'gluTessEndPolygon',\
	 gluTessNormal,'gluTessNormal',\
	 gluTessProperty,'gluTessProperty',\
	 gluTessVertex,'gluTessVertex',\
	 gluUnProject,'gluUnProject'

  import ole,\
   CreateStreamOnHGlobal,'CreateStreamOnHGlobal'

;---------- Resource section --------------------------------------------------;

section '.rsrc' resource data readable
directory  RT_ICON       , icons  , \
           RT_GROUP_ICON , gicons , \
           RT_RCDATA     , raws

resource icons  , ID_EXE_ICON  , LANG_NEUTRAL , exeicon
resource gicons , ID_EXE_ICONS , LANG_NEUTRAL , exegicon
icon exegicon, exeicon, 'politburo.ico'

resource raws, IDR_JPEG, LANG_ENGLISH + SUBLANG_DEFAULT, jpegFileImage
resdata jpegFileImage
file 'politburo.jpg'
endres
