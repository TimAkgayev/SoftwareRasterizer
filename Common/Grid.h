#pragma once
#include "SoftwareRasterizer.h"
#include "MathStructures.h"
#include "MacrosAndDefines.h"
#include <windows.h>

class Grid
{
public:
	Grid();
	~Grid();

	void Draw(DWORD* mem, int lpitch32);

	void SetCenterCoordinate(float x, float y);
	void SetSpacing(int spacing);
	void SetLineThickness(int thickness);
	void SetViewport(Viewport* viewport);

	int GetSpacing() const;
private:
	Viewport* viewport;
	VECTOR2D CenterCoordinate;
	int grid_spacing;
	int line_thickness;
	void DrawHorizontal(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart = COLOR_RED, DWORD colorEnd = COLOR_RED, int indentAxis = -1, DWORD c1 = COLOR_YELLOW, DWORD c2 = COLOR_YELLOW);
	void DrawVertical(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart = COLOR_RED, DWORD colorEnd = COLOR_RED, int indentAxis = -1, DWORD c1 = COLOR_YELLOW, DWORD c2 = COLOR_YELLOW);
};