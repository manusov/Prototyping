/*
EXPERIMENTAL TEMPLATE FOR OPENGL GPUSTRESS.
BUILD WITH VISUAL STUDIO 2022.
Special thanks to:
https://ravesli.com/uroki-po-opengl/
https://ravesli.com/uroki-cpp/#toc-0
*/

#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "resource.h"
#include "Global.h"
#include "Timer.h"
#include "TextureLoader.h"
#include "FontLoader.h"
#include "OpenGL.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void WndDestroyHelper(HWND, HDC);

Timer* pTimer = nullptr;
TextureLoader* pTextureLoader = nullptr;
FontLoader* pFontLoader = nullptr;
OpenGL* pOpenGL = nullptr;
HINSTANCE hInst = NULL;
HDC hDC = NULL;
void* rawPtr = nullptr;
double tscFrequency = 0.0;
double tscPeriod = 0.0;
int windowExitCode = 0;

constexpr unsigned int GPU_LOADS[]
{
    APPCONST::INSTANCING_COUNT_LOAD_0,
    APPCONST::INSTANCING_COUNT_LOAD_1,
    APPCONST::INSTANCING_COUNT_LOAD_2,
    APPCONST::INSTANCING_COUNT_LOAD_3,
    APPCONST::INSTANCING_COUNT_LOAD_4,
    APPCONST::INSTANCING_COUNT_LOAD_5,
    APPCONST::INSTANCING_COUNT_LOAD_6,
    APPCONST::INSTANCING_COUNT_LOAD_7
};
int optionLoadIndex = APPCONST::DEFAULT_GPU_LOAD_SELECT;
BOOL optionDepthTest = TRUE;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    int exitCode = 0;
    windowExitCode = 0;
    char szAppMsg[APPCONST::MAX_TEXT_STRING];
    hInst = hInstance;
    snprintf(szAppMsg, APPCONST::MAX_TEXT_STRING, "%s %s", APPCONST::APP_NAME, APPCONST::BUILD_NAME);
    int userInput = MessageBox(NULL,
        "This application can overheat your GPU,\r\nespecially if Depth test OFF.\r\n\r\nRun application ?",
        szAppMsg, MB_YESNO + MB_ICONWARNING);
    if (userInput == IDYES)
    {
        pTimer = new Timer();
        pTextureLoader = new TextureLoader(hInst);
        pFontLoader = new FontLoader();
        pOpenGL = new OpenGL();
        if (pTimer && pTextureLoader && pFontLoader && pOpenGL)
        {
            if (pTimer->getStatus())
            {
                rawPtr = pTextureLoader->getRawPointer();
                if (rawPtr)
                {
                    const char* szClassName = "OPENGLSAMPLE";
                    WNDCLASSEX wcex;
                    wcex.cbSize = sizeof(WNDCLASSEX);
                    wcex.style = CS_HREDRAW | CS_VREDRAW;
                    wcex.lpfnWndProc = WndProc;
                    wcex.cbClsExtra = 0;
                    wcex.cbWndExtra = 0;
                    wcex.hInstance = hInstance;
                    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GPUSTRESS));
                    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
                    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                    wcex.lpszMenuName = szAppMsg;
                    wcex.lpszClassName = szClassName;
                    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GPUSTRESS));
                    ATOM atom = RegisterClassEx(&wcex);
                    if (atom)
                    {
                        HWND hWnd = CreateWindow(szClassName, szAppMsg, WS_OVERLAPPEDWINDOW,
                            APPCONST::X_BASE, APPCONST::Y_BASE, APPCONST::X_SIZE, APPCONST::Y_SIZE,
                            nullptr, nullptr, hInstance, nullptr);
                        if (hWnd)
                        {
                            ShowWindow(hWnd, nCmdShow);
                            UpdateWindow(hWnd);
                            MSG msg;
                            while (GetMessage(&msg, nullptr, 0, 0))
                            {
                                TranslateMessage(&msg);
                                DispatchMessage(&msg);
                            }
                            if ((msg.message != WM_QUIT) || (msg.wParam != 0))
                            {
                                exitCode = 7;
                            }
                        }
                        else
                        {
                            exitCode = 6;
                        }
                    }
                    else
                    {
                        exitCode = 5;
                    }
                }
                else
                {
                    exitCode = 4;
                }
            }
            else
            {
                exitCode = 3;
            }
        }
        else
        {
            exitCode = 2;
        }
        if ((exitCode) && (!windowExitCode))
        {
            char szError[APPCONST::MAX_TEXT_STRING];
            snprintf(szError, APPCONST::MAX_TEXT_STRING, "Initialization failed (%d).", exitCode);
            MessageBox(NULL, szError, nullptr, MB_ICONERROR);
        }
    }
    else
    {
        exitCode = 1;
    }
    
    if (pTimer) delete pTimer;
    if (pTextureLoader) delete pTextureLoader;
    if (pFontLoader) delete pFontLoader;
    if (pOpenGL) delete pOpenGL;
    return exitCode;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            hDC = GetDC(hWnd);
            windowExitCode = pFontLoader->init(rawPtr);
            if (!windowExitCode)
            {
                windowExitCode = pOpenGL->init(hWnd, hDC, rawPtr, pTimer); 
            }
            if (windowExitCode)
            {
                char szError[APPCONST::MAX_TEXT_STRING];
                snprintf(szError, APPCONST::MAX_TEXT_STRING, "Window initialization failed (%d).", windowExitCode);
                MessageBox(NULL, szError, nullptr, MB_ICONERROR);
                return -1;
            }
        }
        break;

        case WM_SIZE:
        {
            RECT r;
            GetClientRect(hWnd, &r);
            glViewport(0, 0, r.right, r.bottom);
        }
        break;

        case WM_PAINT:
        {
            pOpenGL->draw(hWnd, hDC, GPU_LOADS[optionLoadIndex], optionDepthTest);
        }
        break;

        case WM_KEYDOWN:
        {
            int key = wParam & 0xFFFFFFFFLL;
            switch (key)
            {
            case VK_UP:
                if (optionLoadIndex < APPCONST::MAXIMUM_GPU_LOAD_SELECT)
                {
                    optionLoadIndex++;
                    pTimer->resetStatistics();
                }
                break;

            case VK_DOWN:
                if (optionLoadIndex)
                {
                    optionLoadIndex--;
                    pTimer->resetStatistics();
                }
                break;

            case VK_LEFT:
                optionDepthTest = FALSE;
                pTimer->resetStatistics();
                break;

            case VK_RIGHT:
                optionDepthTest = TRUE;
                pTimer->resetStatistics();
                break;

            case VK_ESCAPE:
                WndDestroyHelper(hWnd, hDC);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

        case WM_DESTROY:
            {
                WndDestroyHelper(hWnd, hDC);
            }
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void WndDestroyHelper(HWND hWnd, HDC hDC)
{
    if (hDC)
    {
        ReleaseDC(hWnd, hDC);
    }
    SendMessage(hWnd, WM_CLOSE, NULL, NULL);
    PostQuitMessage(0);
}