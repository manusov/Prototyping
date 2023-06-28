/*
OpenGL GPUstress.
Font loader class header.
*/

#pragma once
#ifndef FONTLOADER_H
#define FONTLOADER_H

#include <windows.h>
#include "Global.h"

class FontLoader
{
public:
    FontLoader();
    ~FontLoader();
    int init(const void* rawData);
private:
    static const BYTE rasterFont[];
};

#endif // FONTLOADER_H
