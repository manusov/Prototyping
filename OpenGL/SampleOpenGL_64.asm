;------------------------------------------------------------------------------;
; OpenGL programming example. x64 version.                                     ;
; FASM OpenGL example with 2D-rotation updated for 3D-rotation.                ;
; Some low-level optimization by use native instruction "call"                 ;
; instead macro "invoke".                                                      ;
;                                                                              ;
; v3 with OpenGL information window before test.                               ;                   
;                                                                              ;
; See also:                                                                    ;
; https://github.com/manusov                                                   ;
;                                                                              ;
; Special thanks:                                                              ;
; https://flatassembler.net/                                                   ;
; https://ravesli.com/uroki-po-opengl/                                         ;
;------------------------------------------------------------------------------;

include 'win64a.inc'
include 'OpenGL.inc'

format PE64 GUI 5.0
entry start
section '.text' code readable executable
start:
sub rsp,8     ; Make stack dqword (16 byte) aligned
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
xor eax,eax
push rax
push [rbx + WNDCLASS.hInstance]
push rax
push rax
push 432
push 432
push 16
push 16
mov r9d,WS_VISIBLE + WS_OVERLAPPEDWINDOW + WS_CLIPCHILDREN + WS_CLIPSIBLINGS
lea r8,[_title]
lea rdx,[_class]
xor ecx,ecx
sub rsp,32
call [CreateWindowEx]
add rsp,32 + 64
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
mov rcx,[rbx + MSG.wParam]  
call [ExitProcess]

;---------- Window callback procedure. ----------------------------------------;

WindowProc:
push rbx rsi rdi rbp
mov rbp,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,32
mov rbx,rcx

cmp edx,WM_CREATE
je .wmcreate
cmp edx,WM_SIZE
je .wmsize
cmp edx,WM_PAINT
je .wmpaint
cmp edx,WM_KEYDOWN
je .wmkeydown
cmp edx,WM_DESTROY
je .wmdestroy

.defwndproc:
call [DefWindowProc]   ; rcx, rdx, r8, r9 must be valid input at this point
jmp	.finish

.wmcreate:
call [GetDC]           ; rcx = input
mov [hdc],rax
lea rdi,[pfd]
mov rsi,rdi            ; rsi = pointer to pfd
mov	ecx,sizeof.PIXELFORMATDESCRIPTOR shr 3
xor	eax,eax
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
mov rdi,[hdc]            ; rdi = hdc
mov rcx,rdi
mov rdx,rsi
call [ChoosePixelFormat] 
mov rcx,rdi
xchg edx,eax            ; in the current context XCHG compact than MOV
mov r8,rsi
call [SetPixelFormat]
mov rcx,rdi
call [wglCreateContext]
mov [hrc],rax
mov rcx,rdi
xchg rdx,rax            ; in the current context XCHG compact than MOV
call [wglMakeCurrent]	
lea rdi,[rc]            ; rdi = pointer to rc
mov rcx,rbx
mov rdx,rdi
call [GetClientRect]
xor ecx,ecx
xor edx,edx
mov r8d,[rdi + RECT.right]
mov r9d,[rdi + RECT.bottom]
call [glViewport]
call [GetTickCount]
mov [clock],eax

GL_SHADING_LANGUAGE_VERSION = 00008B8Ch
TEMP_BUFFER_SIZE = 512

cld
lea rsi,[listStrings]
lea rdi,[tempBuffer]
mov ebx,TEMP_BUFFER_SIZE - 3
.nextString:
lodsd
xchg ecx,eax
jrcxz .stringsDone
call [glGetString]
test rax,rax
jz .skipString
push rsi
xchg rsi,rax
.nextChar:
lodsb
cmp al,0
je .charsDone
dec ebx
jz .charsDone 
stosb
jmp .nextChar
.charsDone:
pop rsi
mov ax,0A0Dh
stosw
.skipString:
test ebx,ebx
jnz .nextString
.stringsDone:
mov al,0
stosb
xor ecx,ecx
lea rdx,[tempBuffer]
lea r8,[versionCaption]
mov r9d,MB_ICONINFORMATION
call [MessageBoxA]

xor eax,eax
jmp .finish

.wmsize:
lea rdi,[rc]            ; rdi = pointer to rc
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
  
.wmpaint:
call [GetTickCount]
sub eax,[clock]
cmp eax,10
jb .skipRotate
add [clock],eax
cld
lea rsi,[theta]
lodsd
movd xmm0,eax
lodsd
movd xmm1,eax
lodsd
movd xmm2,eax
lodsd
movd xmm3,eax
call [glRotatef] 
.skipRotate:
mov ecx,GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT
call [glClear]
mov ecx,GL_DEPTH_TEST
call [glEnable]
mov ecx,GL_QUADS
call [glBegin]
cld
lea rsi,[cube]
mov edi,24
.renderCycle:
lodsd
movd xmm0,eax
lodsd
movd xmm1,eax
lodsd
movd xmm2,eax
call [glColor3f]
lodsq
movq xmm0,rax
lodsq
movq xmm1,rax
lodsq
movq xmm2,rax
call [glVertex3d]
dec edi
jnz .renderCycle
call [glEnd]
mov rcx,[hdc]
call [SwapBuffers]
xor eax,eax
jmp .finish

.wmkeydown:
cmp r8d,VK_ESCAPE
jne .defwndproc

.wmdestroy:
xor ecx,ecx
xor edx,edx
call [wglMakeCurrent]
mov rcx,[hrc]
call [wglDeleteContext]
mov rcx,rbx
mov rdx,[hdc]
call [ReleaseDC]	
xor ecx,ecx
call [PostQuitMessage]
xor	eax,eax

.finish:
mov rsp,rbp
pop rbp rdi rsi rbx
ret

;---------- Data section not changed from original example, -------------------;
;           except floating point data for trigonometry, alignment reasons
;           and (x64) and (ia32) notes in the title string.

section '.data' data readable writeable

  _title db 'OpenGL example (x64)',0
  _class db 'FASMOPENGL32',0

  wc WNDCLASS 0,WindowProc,0,0,NULL,NULL,NULL,NULL,NULL,_class
  
  align 8
  theta GLfloat   0.6
        GLfloat   0.95 , 0.85 , 1.0
        
  cube  GLfloat   0.1 ,  0.1 ,  1.0
        GLdouble -0.5 , -0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 , -0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 ,  0.5 , -0.5
        GLfloat   1.0 ,  1.0 ,  0.1
        GLdouble -0.5 ,  0.5 , -0.5
        
        GLfloat   0.1 ,  1.0 ,  0.1
        GLdouble -0.5 , -0.5 ,  0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 , -0.5 ,  0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 ,  0.5 ,  0.5
        GLfloat   1.0 ,  0.1 ,  1.0
        GLdouble -0.5 ,  0.5 ,  0.5

        GLfloat   0.1 ,  1.0 ,  1.0
        GLdouble -0.5 , -0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble -0.5 ,  0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble -0.5 ,  0.5 ,  0.5
        GLfloat   1.0 ,  0.1 ,  0.1
        GLdouble -0.5 , -0.5 ,  0.5

        GLfloat   1.0 ,  0.1 ,  0.1
        GLdouble  0.5 , -0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 ,  0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 ,  0.5 ,  0.5
        GLfloat   1.0 ,  0.1 ,  1.0
        GLdouble  0.5 , -0.5 ,  0.5

        GLfloat   0.1 ,  1.0 ,  0.1
        GLdouble -0.5 , -0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 , -0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 , -0.5 ,  0.5
        GLfloat   1.0 ,  1.0 ,  1.0
        GLdouble -0.5 , -0.5 ,  0.5

        GLfloat   1.0 ,  1.0 ,  1.0
        GLdouble -0.5 ,  0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 ,  0.5 , -0.5
        GLfloat   0.1 ,  0.1 ,  0.1
        GLdouble  0.5 ,  0.5 ,  0.5
        GLfloat   1.0 ,  0.1 ,  0.1
        GLdouble -0.5 ,  0.5 ,  0.5
 
  listStrings     DD  GL_VENDOR
                  DD  GL_RENDERER
                  DD  GL_VERSION
                  DD  GL_SHADING_LANGUAGE_VERSION
                  DD  0
  versionCaption  DB  'OpenGL info (x64)', 0
  tempBuffer      DB  TEMP_BUFFER_SIZE  DUP (?)

  align 8
  hdc   dq ?
  hrc   dq ?
  clock dd ?

  msg MSG
  rc  RECT
  pfd PIXELFORMATDESCRIPTOR

;---------- Import section not changed from original example. -----------------;

section '.idata' import data readable writeable

  library kernel,'KERNEL32.DLL',\
	  user,'USER32.DLL',\
	  gdi,'GDI32.DLL',\
	  opengl,'OPENGL32.DLL',\
	  glu,'GLU32.DLL'

  import kernel,\
	 GetModuleHandle,'GetModuleHandleA',\
	 GetTickCount,'GetTickCount',\
	 ExitProcess,'ExitProcess'

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
   MessageBoxA,'MessageBoxA'

  import gdi,\
	 ChoosePixelFormat,'ChoosePixelFormat',\
	 SetPixelFormat,'SetPixelFormat',\
	 SwapBuffers,'SwapBuffers'

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
