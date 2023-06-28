/*
OpenGL GPUstress.
Texture loader class.
*/

#include "TextureLoader.h"

TextureLoader::TextureLoader(HINSTANCE hModule)
{
	gdiplusToken = NULL;
	pStream = nullptr;
	hGlobal = nullptr;
	hBitmap = nullptr;
	rawPointer = nullptr;
	memset(&bitmap, 0, sizeof(BITMAP));
	GdiplusStartupInput gdiplusStartupInput;
	gdiplusStartupInput.GdiplusVersion = 1;
	gdiplusStartupInput.DebugEventCallback = nullptr;
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	gdiplusStartupInput.SuppressExternalCodecs = FALSE;
	if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) == Gdiplus::GpStatus::Ok)
	{
		HRSRC hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDR_JPEG_TEXTURE), RT_RCDATA);
		if(hResInfo)
		{
			DWORD byteCount = SizeofResource(hModule, hResInfo);
			if (byteCount)
			{
				HGLOBAL hResData = LoadResource(hModule, hResInfo);
				if (hResData)
				{
					LPVOID ptrLocked = LockResource(hResData);
					if (ptrLocked)
					{
						hGlobal = GlobalAlloc(GHND, byteCount);
						if (hGlobal)
						{
							LPVOID ptrGlocked = GlobalLock(hGlobal);
							if (ptrGlocked)
							{
								memcpy(ptrGlocked, ptrLocked, byteCount);
								Gdiplus::GpBitmap* pBitmap = nullptr;
								HRESULT result = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
								if (result == S_OK)
								{
									GpStatus status = GdipCreateBitmapFromStream(pStream, &pBitmap);
									if (status == S_OK)
									{
										status = GdipCreateHBITMAPFromBitmap(pBitmap, &hBitmap, 0xFFFFFF);
										if (hBitmap)
										{
											int countStruc = GetObject(hBitmap, sizeof(BITMAP), &bitmap);
											if (countStruc == sizeof(BITMAP))
											{
												if ((bitmap.bmWidth == APPCONST::TEXTURE_WIDTH) &&
													(bitmap.bmHeight == APPCONST::TEXTURE_HEIGHT))
												{
													rawPointer = bitmap.bmBits;
												}
											}
										}
									}
								}
							}
						}
					}
				}

			}
		}
	}
}
TextureLoader::~TextureLoader()
{
	if (pStream)
	{
		pStream->Release();
	}
	if (hGlobal)
	{
		hGlobal = GlobalFree(hGlobal);
	}
	if (hBitmap)
	{
		DeleteObject(hBitmap);
	}
	if (gdiplusToken)
	{
		GdiplusShutdown(gdiplusToken);
	}
}
void* TextureLoader::getRawPointer()
{
	return rawPointer;
}
BITMAP* TextureLoader::getStrucPointer()
{
	return &bitmap;
}
ULONG_PTR TextureLoader::gdiplusToken = NULL;
LPSTREAM TextureLoader::pStream = nullptr;
HGLOBAL TextureLoader::hGlobal = nullptr;
HBITMAP TextureLoader::hBitmap = nullptr;
void* TextureLoader::rawPointer = nullptr;
BITMAP TextureLoader::bitmap;
