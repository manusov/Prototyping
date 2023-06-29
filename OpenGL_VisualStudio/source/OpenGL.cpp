/*
OpenGL GPUstress.
OpenGL and graphics mathematics class.
Note.
OpenGL dynamically imported functions must be dynamically imported
after select pixel format, because it can be unique for each pixel format.
https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglgetprocaddress
*/

#include "OpenGL.h"

OpenGL::OpenGL() : pfd{ 0 }, viewRect{ 0 }, f{ 0 }, hglrc(nullptr), vao(0), vbo(0), texture1(0), shaderProgramId(0),
                   gpuLoadNow(APPCONST::DEFAULT_GPU_LOAD), gpuDepthTest(TRUE), ptrTimer(nullptr)
{
	constexpr int TRANS_MATRIXES_XYZ = 4 * 4 * 4;
	ptrTransfMatrixes = new GLfloat[TRANS_MATRIXES_XYZ];
	memset(ptrTransfMatrixes, 0, TRANS_MATRIXES_XYZ * sizeof(GLfloat));
	ptrScales = new GLfloat[APPCONST::MAXIMUM_INSTANCING_COUNT];
	memset(ptrScales, 0, APPCONST::TEMP_BUFFER_SIZE * sizeof(GLfloat));
	textOutput = new GLchar[APPCONST::TEMP_BUFFER_SIZE];
	memset(textOutput, 0, APPCONST::TEMP_BUFFER_SIZE);
	errorLog = new GLchar[APPCONST::TEMP_BUFFER_SIZE];
	memset(errorLog, 0, APPCONST::TEMP_BUFFER_SIZE);
}
OpenGL::~OpenGL()
{
	if (vao)
	{
		f.glDeleteVertexArrays(1, &vao);
	}
	if (vbo)
	{
		f.glDeleteBuffers(1, &vbo);
	}
	wglMakeCurrent(NULL, NULL);
	if (hglrc)
	{
		wglDeleteContext(hglrc);
	}
	if (ptrTransfMatrixes) delete[] ptrTransfMatrixes;
	if (ptrScales)         delete[] ptrScales;
	if (textOutput)        delete[] textOutput;
	if (errorLog)          delete[] errorLog;
}
int OpenGL::init(HWND hWnd, HDC hDC, const void* rawData, Timer* pTimer)
{
	ptrTimer = pTimer;
	gpuLoadNow = APPCONST::DEFAULT_GPU_LOAD;
	
	PIXELFORMATDESCRIPTOR* pPfd = &pfd;
	memset(pPfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	memset(&viewRect, 0, sizeof(RECT));
	memset(&f, 0, sizeof(f));
	pPfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pPfd->nVersion = 1;
	pPfd->dwFlags = PFD_SUPPORT_OPENGL + PFD_DOUBLEBUFFER + PFD_DRAW_TO_WINDOW;
	pPfd->iLayerType = PFD_MAIN_PLANE;
	pPfd->iPixelType = PFD_TYPE_RGBA;
	pPfd->cColorBits = 16;
	pPfd->cDepthBits = 16;
	pPfd->cAccumBits = 0;
	pPfd->cStencilBits = 0;
	int index = ChoosePixelFormat(hDC, pPfd);
	if (!index) return 0x100;
	if (!SetPixelFormat(hDC, index, pPfd)) return 0x101;
	hglrc = wglCreateContext(hDC);
	if (!hglrc) return 0x102;
	if (!wglMakeCurrent(hDC, hglrc)) return 0x103;
	if(!GetClientRect(hWnd, &viewRect)) return 0x104;
	glViewport(0, 0, viewRect.right, viewRect.bottom);

	GLchar* ptrText = textOutput;
	for (int i = 0; i < APPCONST::TEMP_BUFFER_SIZE; i++)
	{
		ptrText[i] = ' ';
	}

	const GLenum* ptrEnum = &infoNames[0];
	char* textOutputStrings = textOutput + 128 * 3 + 1;
	while (true)
	{
		GLenum infoType = *(ptrEnum++);
		if (!infoType) break;
		const GLubyte* outStr = glGetString(infoType);
		if (outStr)
		{
			snprintf(textOutputStrings, 128, "%s", outStr);;
			textOutputStrings -= 128;
		}
	}
	snprintf(textOutput + 128 * 3 + 60, 128, szSeconds);
	snprintf(textOutput + 128 * 2 + 60, 128, szFrames);
	snprintf(textOutput + 128 * 1 + 60, 128, szFPSavg);
	snprintf(textOutput + 128 * 0 + 60, 128, szFPScur);
	snprintf(textOutput + 128 * 3 + 90, 128, szBusSeconds);
	snprintf(textOutput + 128 * 2 + 90, 128, szBusMB);
	snprintf(textOutput + 128 * 1 + 90, 128, szBusMBPSavg);
	snprintf(textOutput + 128 * 0 + 90, 128, szBusMBPScur);
	snprintf(textOutput + 128 * 4 + 1,  128, szGpuLoad);
	snprintf(textOutput + 128 * 4 + 52, 128, szDepthTest);

	const char** pName = oglNamesList;
	size_t* pFunc = reinterpret_cast<size_t*>(&f);
	BOOL failure = FALSE;
	while(*pName)
	{
		const char* name = *pName;
		void* pFn = wglGetProcAddress(name);
		if (!pFn)
		{
			failure = TRUE;
			break;
		}
		*pFunc = reinterpret_cast<size_t>(pFn);
		pName++;
		pFunc++;
	}
	if(failure) return 0x105;

	GLuint vertexShaderId = f.glCreateShader(GL_VERTEX_SHADER);
	if(!vertexShaderId) return 0x106;
	f.glShaderSource(vertexShaderId, 1, &vertexShaderSource, nullptr);
	f.glCompileShader(vertexShaderId);
	GLint params = 0;
	f.glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &params);
	if (params == GL_FALSE)
	{
		if (!errorLog) return 0x107;
		char* p = errorLog;
		GLsizei n = APPCONST::TEMP_BUFFER_SIZE;
		GLsizei k = snprintf(p, n, "Vertex shader compiling error:\r\n");
		p += k;
		n -= k;
		f.glGetShaderInfoLog(vertexShaderId, n, nullptr, p);
		MessageBox(NULL, errorLog, nullptr, MB_ICONERROR);
		return 0x108;
	}

	GLuint fragmentShaderId = f.glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragmentShaderId) return 0x109;
	f.glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, nullptr);
	f.glCompileShader(fragmentShaderId);
	params = 0;
	f.glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &params);
	if (params == GL_FALSE)
	{
		if (!errorLog) return 0x10A;
		char* p = errorLog;
		GLsizei n = APPCONST::TEMP_BUFFER_SIZE;
		GLsizei k = snprintf(p, n, "Fragment shader compiling error:\r\n");
		p += k;
		n -= k;
		f.glGetShaderInfoLog(fragmentShaderId, n, nullptr, p);
		MessageBox(NULL, errorLog, nullptr, MB_ICONERROR);
		return 0x10B;
	}

	shaderProgramId = f.glCreateProgram();
	if (!shaderProgramId) return 0x10C;
	f.glAttachShader(shaderProgramId, vertexShaderId);
	f.glAttachShader(shaderProgramId, fragmentShaderId);
	f.glLinkProgram(shaderProgramId);
	f.glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &params);
	if (params == GL_FALSE)
	{
		if (!errorLog) return 0x10D;
		char* p = errorLog;
		GLsizei n = APPCONST::TEMP_BUFFER_SIZE;
		GLsizei k = snprintf(p, n, "Shader program linking error:\r\n");
		p += k;
		n -= k;
		f.glGetProgramInfoLog(shaderProgramId, n, nullptr, p);
		MessageBox(NULL, errorLog, nullptr, MB_ICONERROR);
		return 0x10E;
	}
	f.glDeleteShader(vertexShaderId);
	f.glDeleteShader(fragmentShaderId);

	vao = 0;
	f.glGenVertexArrays(1, &vao);
	if(!vao) return 0x10F;
	vbo = 0;
	f.glGenBuffers(1, &vbo);
	if (!vbo) return 0x110;
	f.glBindVertexArray(vao);
	if (glGetError()) return 0x111;
	f.glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (glGetError()) return 0x112;

	constexpr GLsizeiptr CUBE_MODEL_SIZE = 6 * 6 * 5 * 4;
	constexpr GLsizei CUBE_STRIDE = 5 * 4;
	constexpr size_t CUBE_TEXTURE_OFFSET = 3 * 4;
	f.glBufferData(GL_ARRAY_BUFFER, CUBE_MODEL_SIZE, &verticesCube, GL_STATIC_DRAW);
	if (glGetError()) return 0x113;
	f.glVertexAttribPointer(0, 3, GL_FLOAT, 0, CUBE_STRIDE, 0);
	if (glGetError()) return 0x114;
	f.glEnableVertexAttribArray(0);
	if (glGetError()) return 0x115;
	f.glVertexAttribPointer(1, 2, GL_FLOAT, 0, CUBE_STRIDE, (void*)CUBE_TEXTURE_OFFSET);
	if (glGetError()) return 0x116;
	f.glEnableVertexAttribArray(1);
	if (glGetError()) return 0x117;

	texture1 = 0;
	glGenTextures(1, &texture1);
	if (glGetError() || (!texture1)) return 0x118;
	glBindTexture(GL_TEXTURE_2D, texture1);
	if (glGetError()) return 0x119;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	if (glGetError()) return 0x11A;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (glGetError()) return 0x11B;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (glGetError()) return 0x11C;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (glGetError()) return 0x11D;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		APPCONST::TEXTURE_WIDTH, APPCONST::TEXTURE_HEIGHT,
		0, GL_BGRA, GL_UNSIGNED_BYTE, rawData);
	if (glGetError()) return 0x11E;
	f.glGenerateMipmap(GL_TEXTURE_2D);
	if (glGetError()) return 0x11F;

	f.glUseProgram(shaderProgramId);
	if (glGetError()) return 0x120;
	GLint location = f.glGetUniformLocation(shaderProgramId, textureName);
	f.glUniform1i(location, 0);
	if (glGetError()) return 0x121;

	GLfloat* p = ptrScales;
	if(!p) return 0x122;
	for (int i = 0; i < APPCONST::MAXIMUM_INSTANCING_COUNT; i++)
	{
		*(p++) = 0.55f;
	}

	p = ptrTransfMatrixes;
	if (!p) return 0x123;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				GLfloat a = (j == k) ? 1.0f : 0.0f;
				*(p++) = a;
			}
		}
	}

	GLuint ivbo = 0;
	f.glGenBuffers(1, &ivbo);
	if (glGetError() || (!ivbo)) return 0x124;
	f.glBindBuffer(GL_ARRAY_BUFFER, ivbo);
	if (glGetError()) return 0x125;

	f.glBufferData(GL_ARRAY_BUFFER, gpuLoadNow * 4, ptrScales, GL_DYNAMIC_DRAW);
	if (glGetError()) return 0x126;
	f.glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError()) return 0x127;
	f.glEnableVertexAttribArray(2);
	if (glGetError()) return 0x128;
	f.glBindBuffer(GL_ARRAY_BUFFER, ivbo);
	if (glGetError()) return 0x129;
	f.glVertexAttribPointer(2, 1, GL_FLOAT, 0, 4, 0);
	if (glGetError()) return 0x12A;
	f.glVertexAttribDivisor(2, 1);
	if (glGetError()) return 0x12B;

	ptrTimer->resetStatistics();
	ptrTimer->startApplicationSeconds();
	ptrTimer->startPerformanceSeconds();
	return 0;
}
void OpenGL::draw(HWND hWnd, HDC hDC, unsigned int optionLoad, BOOL optionDepth)
{
	double seconds = ptrTimer->getApplicationSeconds();
	gpuLoadNow = optionLoad;
	gpuDepthTest = optionDepth;
	
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT);

	if (gpuDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	f.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	f.glUseProgram(shaderProgramId);

	float xSin = static_cast<float>(sin(-seconds * 0.25));
	float xCos = static_cast<float>(cos(-seconds * 0.25));
	float ySin = static_cast<float>(sin(seconds * 0.5));
	float yCos = static_cast<float>(cos(seconds * 0.5));
	float zSin = static_cast<float>(sin(seconds * 1.5));
	float zCos = static_cast<float>(cos(seconds * 1.5));
	ptrTransfMatrixes[16 + 5]  =  xCos;
	ptrTransfMatrixes[16 + 6]  = -xSin;
	ptrTransfMatrixes[16 + 9]  =  xSin;
	ptrTransfMatrixes[16 + 10] =  xCos;
	ptrTransfMatrixes[32 + 0]  =  yCos;
	ptrTransfMatrixes[32 + 2]  =  ySin;
	ptrTransfMatrixes[32 + 8]  = -ySin;
	ptrTransfMatrixes[32 + 10] =  yCos;
	ptrTransfMatrixes[48 + 0]  =  zCos;
	ptrTransfMatrixes[48 + 1]  = -zSin;
	ptrTransfMatrixes[48 + 4]  =  zSin;
	ptrTransfMatrixes[48 + 5]  =  zCos;
	matrixMultiply(ptrTransfMatrixes + 16, ptrTransfMatrixes + 32, ptrTransfMatrixes);
	matrixMultiply(ptrTransfMatrixes, ptrTransfMatrixes + 48, ptrTransfMatrixes);

	float scale = static_cast<float>(sin(seconds * 0.45) * 0.6);
	const size_t vCount = gpuLoadNow / 4;
	__m128* vPtr = reinterpret_cast<__m128*>(ptrScales);
	__m128 vData = _mm_load_ps1(&scale);
	for (size_t i = 0; i < vCount; i++)
	{
		*(vPtr++) = vData;
	}

	GLsizeiptr bytesPerFrame = gpuLoadNow * 4;
	GLint location = f.glGetUniformLocation(shaderProgramId, modelName);
	f.glUniformMatrix4fv(location, 1, 0, ptrTransfMatrixes);
	ptrTimer->startTransferSeconds();
	f.glBufferData(GL_ARRAY_BUFFER, bytesPerFrame, ptrScales, GL_DYNAMIC_DRAW);
	double mbpsCurrent = ptrTimer->stopTransferSeconds(bytesPerFrame);

	f.glBindVertexArray(vao);
	constexpr GLint ARRAY_COUNT = 6 * 6;
	f.glDrawArraysInstanced(GL_TRIANGLES, 0, ARRAY_COUNT, static_cast<GLsizei>(gpuLoadNow));
	SwapBuffers(hDC);

	snprintf(textOutput + 128 * 4 + 36, 128, "%I64d       ", (long long)gpuLoadNow);
	const char* szOnOff;
	if (gpuDepthTest)
	{
		szOnOff = szDepthOn;
	}
	else
	{
		szOnOff = szDepthOff;
	}
	snprintf(textOutput + 128 * 4 + 82, 128, "%s   ", szOnOff);

	double busTrafficSeconds = ptrTimer->getTransferSeconds();
	double megabytesCount = ptrTimer->getMegabytesCount();
	double mbpsAverage = ptrTimer->getAverageMBPS();
	double elapsedSeconds = ptrTimer->getPerformanceSeconds();

	snprintf(textOutput + 128 * 3 + 112, 128, "%.2f    ", busTrafficSeconds);
	snprintf(textOutput + 128 * 2 + 112, 128, "%.2f    ", megabytesCount);
	snprintf(textOutput + 128 * 1 + 112, 128, "%.2f    ", mbpsAverage);
	if (ptrTimer->antiBlinkMBPS())
	{
		snprintf(textOutput + 128 * 0 + 112, 128, "%.2f    ", mbpsCurrent);
	}
	snprintf(textOutput + 128 * 3 + 73, 128, "%.1f    ", elapsedSeconds);
	DWORD64 framesCount = ptrTimer->getFramesCount();
	snprintf(textOutput + 128 * 2 + 73, 128, "%I64u ", framesCount);

	if (framesCount)
	{
		double fpsAverage = ptrTimer->getAverageFPS();
		double fpsCurrent = ptrTimer->stopFrameSeconds();
		snprintf(textOutput + 128 * 1 + 73, 128, "%.1f    ", fpsAverage);
		if (ptrTimer->antiBlinkFPS())
		{
			snprintf(textOutput + 128 * 0 + 73, 128, "%.1f    ", fpsCurrent);
		}
	}
	ptrTimer->startFrameSeconds();

	GLchar szTextIndex[APPCONST::MAX_TEXT_STRING];
	GLint* ptrTextDwords = reinterpret_cast<GLint*>(textOutput);
	for (int i = 0; i < (128 + 32); i++)
	{
		snprintf(szTextIndex, APPCONST::MAX_TEXT_STRING, "%s[%d]", showTextName, i);
		GLint location = f.glGetUniformLocation(shaderProgramId, szTextIndex);
		f.glUniform1i(location, *(ptrTextDwords++));
	}
}
void OpenGL::matrixMultiply(float* src1, float* src2, float* dst)
{
	__m128 a1 = _mm_load_ps(src1);
	__m128 a2 = _mm_load_ps(src1 + 4);
	__m128 a3 = _mm_load_ps(src1 + 8);
	__m128 a4 = _mm_load_ps(src1 + 12);
	for (int i = 0; i < 4; i++)
	{
		__m128 b1 = _mm_load_ps(src2);
		__m128 b2 = b1;
		__m128 b3 = b1;
		__m128 b4 = b1;
		b1 = _mm_shuffle_ps(b1, b1, 0x00);
		b2 = _mm_shuffle_ps(b2, b2, 0x55);
		b3 = _mm_shuffle_ps(b3, b3, 0xAA);
		b4 = _mm_shuffle_ps(b4, b4, 0xFF);
		b1 = _mm_mul_ps(b1, a1);
		b2 = _mm_mul_ps(b2, a2);
		b3 = _mm_mul_ps(b3, a3);
		b4 = _mm_mul_ps(b4, a4);
		b1 = _mm_add_ps(b1, b2);
		b3 = _mm_add_ps(b3, b4);
		b1 = _mm_add_ps(b1, b3);
		_mm_store_ps(dst, b1);
		src2 += 4;
		dst += 4;
	}
}
// Names for OpenGL pixel format-specific functions dynamical import.
const char* OpenGL::oglNamesList[]
{	"glCreateShader",
	"glShaderSource",
	"glCompileShader",
	"glGetShaderiv",
	"glGetShaderInfoLog",
	"glCreateProgram",
	"glAttachShader",
	"glLinkProgram",
	"glGetProgramiv",
	"glGetProgramInfoLog",
	"glDeleteShader",
	"glGenVertexArrays",
	"glGenBuffers",
	"glBindVertexArray",
	"glBindBuffer",
	"glBufferData",
	"glVertexAttribPointer",
	"glEnableVertexAttribArray",
	"glUseProgram",
	"glDeleteVertexArrays",
	"glDeleteBuffers",
	"glGetUniformLocation",
	"glUniformMatrix4fv",
	"glGenerateMipmap",
	"glUniform1i",
	"glActiveTexture",
	"glDrawArraysInstanced",
	"glVertexAttribDivisor",
	nullptr };

// Vertex shader source, compiled at runtime by GPU driver
const char* OpenGL::vertexShaderSource =
"#version 330 core\r\n"
"layout (location = 0) in vec3 aPos;\r\n"
"layout (location = 1) in vec2 aTexCoord;\r\n"
"layout (location = 2) in float sc;\r\n"
"out vec2 TexCoord;\r\n"
"uniform mat4 model_R;\r\n"
"uniform int showText[160];\r\n"
"void main()\r\n"
"{\r\n"
"if(gl_InstanceID < 640)\r\n"
"   {\r\n"
// Screen coordinates for 128x4 chars positions, screen down
"   int nx = gl_InstanceID & 0x7F;\r\n"
"   int ny = gl_InstanceID >> 7;\r\n"
"   if(ny == 4) ny = 43;\r\n"
"   float dx = 2.0f / 128.0f;\r\n"
"   float dy = 2.0f * 44.0f / 1967.0f;\r\n"
"   float x1 = nx * dx - 1.0f;\r\n"
"   float y1 = ny * dy - 1.0f;\r\n"
"   float x2 = x1 + dx;\r\n"
"   float y2 = y1 + dy;\r\n"
"   float rx = (aPos.x < 0) ? x1 : x2;\r\n"
"   float ry = (aPos.y < 0) ? y1 : y2;\r\n"
"   float rz = aPos.z;\r\n"
"   gl_Position = vec4(rx, ry, rz, 1.0f);\r\n"
// Texture coordinates(showed chars select) for 128x4 chars positions
"   bool b1 = false;\r\n"
"   bool b2 = false;\r\n"
"   bool b3 = false;\r\n"
"   bool b4 = false;\r\n"
"   if (ny == 0) b1 = true;\r\n"
"   if ((nx > 72)&&(nx < 82)) b2 = true;\r\n"
"   if ((nx > 111)&&(nx < 121)) b3 = true;\r\n"
"   if (ny == 43) b4 = true;\r\n"
"   bool b = (b1 && (b2 || b3)) || b4;\r\n"
"   float fs = b ? (16.0f / 1967.0f) : 0.0f;\r\n"
"   int index = gl_InstanceID / 4;\r\n"                // index of dword
"   int shift = (gl_InstanceID & 3) * 8;\r\n"          // shift of byte
"   int a = (showText[index] >> shift) & 0x7F;\r\n"    // a = char
"   float corx = 0.5f / 2952.0f;\r\n"
"   float cory = 0.5f / 1967.0f;\r\n"
"   float kx = 8.0f / 2952.0f;\r\n"
"   float ky = 16.0f / 1967.0f;\r\n"
"   float tx1 = kx * a - corx;\r\n"
"   float tx2 = tx1 + kx - corx;\r\n"
"   float ty1 = 0.0f + cory + fs;\r\n"
"   float ty2 = ky - cory + fs;\r\n"
"   float tx = (aPos.x < 0) ? tx1 : tx2;\r\n"
"   float ty = (aPos.y < 0) ? ty1 : ty2;\r\n"
"   TexCoord = vec2(tx, ty);\r\n"
"   }\r\n"
// Otherwise render cubes.
"   else\r\n"
"   {\r\n"
"   int nx = gl_InstanceID % 9;\r\n"
"   int ny = gl_InstanceID / 9 % 3;\r\n"
"   float dx = -0.85f + nx / 4.75f;\r\n"
"   float dy = -0.56f + ny / 1.80f;\r\n"
"   vec4 t = model_R * vec4(aPos, 1.0f);\r\n"
"   float sx = 5.50f + 7.5 - 8.5f * abs(sc);\r\n"
"   float sy = 3.55f + 7.5 - 8.5f * abs(sc);\r\n"
"   float sz = 5.50f + 7.5 - 8.5f * abs(sc);\r\n"
"   sx = 3.2f + sx / 3.0f;\r\n"
"   sy = 3.2f + sy / 3.0f;\r\n"
"   sy = 3.2f + sz / 3.0f;\r\n"
"   gl_Position = vec4(t.x/sx + dx , t.y/sy + dy, t.z/sz, t.w);\r\n"
"   float ctx = 94.0f   / 2952.0f;\r\n"
"   float cty = 1527.0f / 1967.0f;\r\n"
"   float mtx = 303.0f  / 2952.0f;\r\n"
"   float mty = 482.0f  / 1967.0f;\r\n"
"   float dtx = 344.0f  / 2952.0f;\r\n"
"   float dty = 344.0f  / 1967.0f;\r\n"
"   float tx  = ctx + dtx * aTexCoord.x + nx * mtx;\r\n"
"   float ty  = cty - dty * aTexCoord.y - mty * (2 - ny);\r\n"
"   TexCoord = vec2(tx, ty);\r\n"
"   }\r\n"
"}\r\n\0";

// Fragment shader source, compiled at runtime by GPU driver
const char* OpenGL::fragmentShaderSource =
"#version 330 core\r\n"
"out vec4 FragColor;\r\n"
"in vec2 TexCoord;\r\n"
"uniform sampler2D texture1;\r\n"
"void main()\r\n"
"{\r\n"
"   FragColor = texture(texture1, TexCoord);\r\n"
"}\r\n\0";

// Cube vertices coordinates.
const alignas(16) GLfloat OpenGL::verticesCube[]
{
  - 0.5 , -0.5 , -0.5 ,  0.0 , 0.0,
    0.5 , -0.5 , -0.5 ,  1.0 , 0.0,
    0.5 ,  0.5 , -0.5 ,  1.0 , 1.0,
    0.5 ,  0.5 , -0.5 ,  1.0 , 1.0,
  - 0.5 ,  0.5 , -0.5 ,  0.0 , 1.0,
  - 0.5 , -0.5 , -0.5 ,  0.0 , 0.0,

  - 0.5 , -0.5 ,  0.5 ,  0.0 , 1.0,
    0.5 , -0.5 ,  0.5 ,  1.0 , 1.0,
    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0,
    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0,
  - 0.5 ,  0.5 ,  0.5 ,  0.0 , 0.0,
  - 0.5 , -0.5 ,  0.5 ,  0.0 , 1.0,

  - 0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0,
  - 0.5 ,  0.5 , -0.5 ,  1.0 , 0.0,
  - 0.5 , -0.5 , -0.5 ,  0.0 , 0.0,
  - 0.5 , -0.5 , -0.5 ,  0.0 , 0.0,
  - 0.5 , -0.5 ,  0.5 ,  0.0 , 1.0,
  - 0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0,

    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0,
    0.5 ,  0.5 , -0.5 ,  1.0 , 1.0,
    0.5 , -0.5 , -0.5 ,  0.0 , 1.0,
    0.5 , -0.5 , -0.5 ,  0.0 , 1.0,
    0.5 , -0.5 ,  0.5 ,  0.0 , 0.0,
    0.5 ,  0.5 ,  0.5 ,  1.0 , 0.0,

  - 0.5 , -0.5 , -0.5 ,  0.0 , 1.0,
    0.5 , -0.5 , -0.5 ,  1.0 , 1.0,
    0.5 , -0.5 ,  0.5 ,  1.0 , 0.0,
    0.5 , -0.5 ,  0.5 ,  1.0 , 0.0,
  - 0.5 , -0.5 ,  0.5 ,  0.0 , 0.0,
  - 0.5 , -0.5 , -0.5 ,  0.0 , 1.0,

  - 0.5 ,  0.5 , -0.5 ,  0.0 , 0.0,
    0.5 ,  0.5 , -0.5 ,  1.0 , 0.0,
    0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0,
    0.5 ,  0.5 ,  0.5 ,  1.0 , 1.0,
  - 0.5 ,  0.5 ,  0.5 ,  0.0 , 1.0,
  - 0.5 ,  0.5 , -0.5 ,  0.0 , 0.0
};

const GLclampf OpenGL::clearColor[]
{ APPCONST::BACKGROUND_R, APPCONST::BACKGROUND_G, APPCONST::BACKGROUND_B, 1.0f };

const GLchar* OpenGL::modelName    = "model_R";
const GLchar* OpenGL::textureName  = "texture1";
const GLchar* OpenGL::showTextName = "showText";

const GLenum OpenGL::infoNames[]
{ GL_VENDOR, GL_RENDERER, GL_VERSION, GL_SHADING_LANGUAGE_VERSION, 0 };

const char* OpenGL::szSeconds     =  "Seconds";
const char* OpenGL::szFrames      =  "Frames";
const char* OpenGL::szFPSavg      =  "FPS average";
const char* OpenGL::szFPScur      =  "FPS current";
const char* OpenGL::szBusSeconds  =  "Bus traffic seconds";
const char* OpenGL::szBusMB       =  "Megabytes";
const char* OpenGL::szBusMBPSavg  =  "MBPS average";
const char* OpenGL::szBusMBPScur  =  "MBPS current";
const char* OpenGL::szGpuLoad     =  "GPU load instances (up/down keys)";
const char* OpenGL::szDepthTest   =  "Depth test (left/right keys)";
const char* OpenGL::szDepthOn     =  "ON";
const char* OpenGL::szDepthOff    =  "OFF";
