/*
OpenGL GPUstress.
Application constants.
*/

#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

namespace APPCONST
{
// Application title strings and x64/ia32 differentiation constants.
	const char* const APP_NAME = "OpenGL GPUstress (C++ edition). Engineering sample #22.";
#if _WIN64
	#define NATIVE_WIDTH_64
	const char* const BUILD_NAME = "x64.";
#elif _WIN32
	#define NATIVE_WIDTH_32
	const char* const BUILD_NAME = "ia32.";
#endif
// GUI window start positions and sizes.
	constexpr int X_BASE = 380;   // GUI window positions at application start.
	constexpr int Y_BASE = 140;
	constexpr int X_SIZE = 990;   // GUI window sizes at application start.
	constexpr int Y_SIZE = 750;
// Texture image sizes.
	constexpr int TEXTURE_WIDTH  = 2952;   // Texture JPG file X, Y sizes,
	constexpr int TEXTURE_HEIGHT = 1967;   // shaders update required if this changed
// Render window background R, G, B as float
	constexpr float BACKGROUND_R = 0.95f;
	constexpr float BACKGROUND_G = 0.95f;
	constexpr float BACKGROUND_B = 0.95f;
// Text output front and back colors
	constexpr DWORD32 TEXT_FRONT_COLOR_1 = 0xFF616161;
	constexpr DWORD32 TEXT_FRONT_COLOR_2 = 0xFFE05757;
	constexpr DWORD32 TEXT_BACK_COLOR    = 0xFFF2F2F2;
// This constants is IMPORTANT for GPU load.
// 128 x 5 = 640 chars positions matrix for text output:
// 1 up string + 4 down strings.
// 9 x 3 = 27 portraits.
// Render objects count = 128 * 5 + 27 + duplications for GPU load.
	constexpr int INSTANCING_COUNT_LOAD_0  = 1000;
	constexpr int INSTANCING_COUNT_LOAD_1  = 30000;
	constexpr int INSTANCING_COUNT_LOAD_2  = 100000;
	constexpr int INSTANCING_COUNT_LOAD_3  = 300000;
	constexpr int INSTANCING_COUNT_LOAD_4  = 500000;
	constexpr int INSTANCING_COUNT_LOAD_5  = 700000;
	constexpr int INSTANCING_COUNT_LOAD_6  = 1000000;
	constexpr int INSTANCING_COUNT_LOAD_7  = 1500000;
	constexpr int DEFAULT_GPU_LOAD_SELECT  = 0;
	constexpr int MAXIMUM_GPU_LOAD_SELECT  = 7;
	constexpr int DEFAULT_GPU_LOAD         = INSTANCING_COUNT_LOAD_0;
	constexpr int MAXIMUM_GPU_LOAD         = INSTANCING_COUNT_LOAD_7;
	constexpr int MAXIMUM_INSTANCING_COUNT = MAXIMUM_GPU_LOAD;
	// Text output parameters: sizes.
	constexpr int MAX_TEXT_STRING = 160;
	// Text buffer for shaders compiler error log.
	constexpr int TEMP_BUFFER_SIZE = 4096;
}

#endif // GLOBAL_H



