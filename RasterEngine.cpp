#include "RasterEngine.h"

namespace RasterEngine
{
	void GetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color)
	{
		a = (color >> 24);
		r = (color << 24) >> 24;
		g = (color << 16) >> 24;
		b = (color << 8) >> 24;
	}

	void DrawLine(DWORD* buffer, int lpitch32, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect)
	{
		if (clipRect)
		{
			//clip line
			int clipValue = CohenSutherlandLineClip(x0, y0, x1, y1, *clipRect);

			//line is invisible, don't draw
			if (clipValue == 0)
				return;
		}

		//which side is longer
		int xLen = abs(x1 - x0);
		int yLen = abs(y1 - y0);

		if (xLen >= yLen)
		{

			//decode colors from DWORD
			int c1R, c1G, c1B, c1A;
			int c2R, c2G, c2B, c2A;
			GetRGBA32FromDWORD(c1R, c1G, c1B, c1A, color0);
			GetRGBA32FromDWORD(c2R, c2G, c2B, c2A, color1);

			//compute color deltas for the gradient
			double deltaR = (double)(c2R - c1R) / (abs(x1 - x0));
			double deltaG = (double)(c2G - c1G) / (abs(x1 - x0));
			double deltaB = (double)(c2B - c1B) / (abs(x1 - x0));
			double deltaA = (double)(c2A - c1A) / (abs(x1 - x0));

			//final display colors
			double outR = c1R;
			double outG = c1G;
			double outB = c1B;
			double outA = c1A;

			//determine direction term
			int incTerm = 1;
			if (x0 > x1)
				incTerm = -1;

			//how much does y change with each x step
			double deltaY = (double)(y1 - y0) / (x1 - x0);
			double y = y0;

			for (int x = x0; x != x1; x += incTerm)
			{
				DWORD color = _RGBA32BIT(int(outR + 0.5f), int(outG + 0.5f), int(outB + 0.5f), int(outA + 0.5f));
				buffer[x + int(y + 0.5f)*lpitch32] = color;
				y += deltaY*incTerm;
				outR = (outR + deltaR);
				outG = (outG + deltaG);
				outB = (outB + deltaB);
				outA = (outA + deltaA);


			}
		}

		if (yLen > xLen)
		{

			//decode colors from DWORD
			int c1R, c1G, c1B, c1A;
			int c2R, c2G, c2B, c2A;
			GetRGBA32FromDWORD(c1R, c1G, c1B, c1A, color0);
			GetRGBA32FromDWORD(c2R, c2G, c2B, c2A, color1);

			//compute color deltas for the gradient
			double deltaR = (double)(c2R - c1R) / (abs(y1 - y0));
			double deltaG = (double)(c2G - c1G) / (abs(y1 - y0));
			double deltaB = (double)(c2B - c1B) / (abs(y1 - y0));
			double deltaA = (double)(c2A - c1A) / (abs(y1 - y0));

			//final display colors
			double outR = c1R;
			double outG = c1G;
			double outB = c1B;
			double outA = c1A;

			//compute direction term
			int incTerm = 1;
			if (y0 > y1)
				incTerm = -1;

			//how much does y change with each x step
			double deltaX = (double)(x1 - x0) / (y1 - y0);
			double x = x0;

			for (int y = y0; y != y1; y += incTerm)
			{

				DWORD color = _RGBA32BIT(int(outR + 0.5f), int(outG + 0.5f), int(outB + 0.5f), int(outA + 0.5f));
				buffer[int(x + 0.5) + y*lpitch32] = color;
				x += deltaX*incTerm;
				outR = (outR + deltaR);
				outG = (outG + deltaG);
				outB = (outB + deltaB);
				outA = (outA + deltaA);


			}
		}
	}

}