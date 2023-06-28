/*
OpenGL GPUstress.
Texture loader class header.
*/

#pragma once
#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <windows.h>
#include <gdiplus.h>
#include "resource.h"
#include "Global.h"
using namespace Gdiplus;
using namespace DllExports;

class TextureLoader
{
public:
    TextureLoader(HINSTANCE hModule);
    ~TextureLoader();
    void* getRawPointer();
    BITMAP* getStrucPointer();
private:
    static ULONG_PTR gdiplusToken;
    static LPSTREAM pStream;
    static HGLOBAL hGlobal;
    static HBITMAP hBitmap;
    static void* rawPointer;
    static BITMAP bitmap;
};

#endif // TEXTURELOADER_H
