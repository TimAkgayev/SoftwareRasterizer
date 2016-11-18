#pragma once
#include "MathSoftware.h"

namespace Rasterizer
{

#define _RGBA32BIT(r, g, b, a) ( (r & 255) + ((g & 255) << 8) + ((b & 255) << 16)  + ((a & 255) << 24) )

	void DrawLine(DWORD* buffer, int lpitch32, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect = NULL);

	void GetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color;
	
}