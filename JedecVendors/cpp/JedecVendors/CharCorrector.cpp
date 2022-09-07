#include "HeadBlock.h"
#include "CharCorrector.h"

unsigned char detector[3] = { 0,0,0 };  // special support for unexpected encodings

void CorrectorReset()
{
	detector[0] = 0;
	detector[1] = 0;
	detector[2] = 0;
}

void CorrectorChar(char*& dst, char*& src)
{
	detector[0] = detector[1];
	detector[1] = detector[2];
	detector[2] = *src;
	if ((detector[0] == 0xE2) && (detector[1] == 0x80) && (detector[2] == 0x99))
	{
		dst -= 2;
		src++;
		*dst++ = '\'';
	}
	else
	{
		*dst++ = *src++;
	}
}
