/*
OpenGL GPUstress.
OpenGL and graphics mathematics class header.
*/

#pragma once
#ifndef OPENGL_H
#define OPENGL_H

#include <windows.h>
#include <iostream>
#include <gl\GL.h>
#include <intrin.h>
#include "Global.h"
#include "Timer.h"

// Some definitions from glad.h.
#define GL_FRAGMENT_SHADER  0x8B30
#define GL_VERTEX_SHADER    0x8B31
#define GL_COMPILE_STATUS   0x8B81
#define GL_LINK_STATUS      0x8B82
#define GL_ARRAY_BUFFER     0x8892
#define GL_STATIC_DRAW      0x88E4
#define GL_DYNAMIC_DRAW     0x88E8
#define GL_BGRA             0x80E1
#define GL_TEXTURE0         0x84C0
#define GL_SHADING_LANGUAGE_VERSION  0x8B8C

typedef char GLchar;
#if defined(_WIN64)
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif
typedef khronos_ssize_t GLsizeiptr;

struct oglFunctionsList
{
    GLuint(__stdcall *glCreateShader)(GLenum shaderType);
    void(__stdcall *glShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
    void(__stdcall *glCompileShader)(GLuint shader);
    void(__stdcall *glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);
    void(__stdcall *glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
    GLuint(__stdcall *glCreateProgram)();
    void(__stdcall *glAttachShader)(GLuint program, GLuint shader);
    void(__stdcall *glLinkProgram)(GLuint program);
    void(__stdcall *glGetProgramiv)(GLuint program, GLenum pname, GLint* params);
    void(__stdcall *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
    void(__stdcall *glDeleteShader)(GLuint shader);
    void(__stdcall *glGenVertexArrays)(GLsizei n, GLuint* arrays);
    void(__stdcall *glGenBuffers)(GLsizei n, GLuint* buffers);
    void(__stdcall *glBindVertexArray)(GLuint vArray);
    void(__stdcall *glBindBuffer)(GLenum target, GLuint buffer);
    void(__stdcall *glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    void(__stdcall *glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
    void(__stdcall *glEnableVertexAttribArray)(GLuint index);
    void(__stdcall *glUseProgram)(GLuint program);
    void(__stdcall *glDeleteVertexArrays)(GLsizei n, const GLuint* arrays);
    void(__stdcall *glDeleteBuffers)(GLsizei n, const GLuint* buffers);
    GLint(__stdcall *glGetUniformLocation)(GLuint program, const GLchar* name);
    void(__stdcall *glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    void(__stdcall *glGenerateMipmap)(GLenum target);
    void(__stdcall *glUniform1i)(GLint location, GLint v0);
    void(__stdcall *glActiveTexture)(GLenum texture);
    void(__stdcall *glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
    void(__stdcall *glVertexAttribDivisor)(GLuint index, GLuint divisor);
};

class OpenGL
{
public:
    OpenGL();
    ~OpenGL();
    int init(HWND hWnd, HDC hDC, const void* rawData, Timer* pTimer);
    void draw(HWND hWnd, HDC hDC, unsigned int optionLoad, BOOL optionDepth);
private:
    void matrixMultiply(float* src1, float* src2, float* dst);
    PIXELFORMATDESCRIPTOR pfd;
    RECT viewRect;
    oglFunctionsList f;
    HGLRC hglrc;
    GLuint vao;
    GLuint vbo;
    GLuint texture1;
    GLuint shaderProgramId;
    GLsizeiptr gpuLoadNow;
    BOOL gpuDepthTest;
    GLfloat* ptrTransfMatrixes;
    GLfloat* ptrScales;
    GLchar* textOutput;
    GLchar* errorLog;
    Timer* ptrTimer;
    static const char* oglNamesList[];
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    static const alignas(16) GLfloat verticesCube[];
    static const GLclampf clearColor[];
    static const GLchar* modelName;
    static const GLchar* textureName;
    static const GLchar* showTextName;
    static const GLenum infoNames[];
    static const char* szSeconds;
    static const char* szFrames;
    static const char* szFPSavg;
    static const char* szFPScur;
    static const char* szBusSeconds;
    static const char* szBusMB;
    static const char* szBusMBPSavg;
    static const char* szBusMBPScur;
    static const char* szGpuLoad;
    static const char* szDepthTest;
    static const char* szDepthOn;
    static const char* szDepthOff;
};

#endif // OPENGL_H

