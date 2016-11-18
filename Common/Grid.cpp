#include "Grid.h"


Grid::Grid()
{
	line_thickness = 1;
	grid_spacing = 30;
	CenterCoordinate.x = CenterCoordinate.y = 0;
	viewport = NULL;
}

Grid::~Grid()
{

}

void Grid::SetCenterCoordinate(float x, float y)
{
	CenterCoordinate.x = x;
	CenterCoordinate.y = y;
}

void Grid::SetSpacing(int spacing)
{
	grid_spacing = spacing;
}

void Grid::SetLineThickness(int thickness)
{
	line_thickness = thickness;
}

void Grid::SetViewport(Viewport* viewport)
{
	this->viewport = viewport;
}

void Grid::DrawHorizontal(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart, DWORD colorEnd, int indentAxis, DWORD c1, DWORD c2)
{

	int counter = 1;

	for (int movingY = startY; movingY < endY; movingY += pxSpacing, counter++)
	{
		VECTOR3D v1(startX, movingY, 1);
		VECTOR3D v2(endX, movingY, 1);


		//draw main axis in different color
		if (counter == indentAxis)
			QueueTransformClipLine( v1.x, v1.y, v2.x, v2.y, c1, c2);
		else
			QueueTransformClipLine( v1.x, v1.y, v2.x, v2.y, colorStart, colorEnd);
		
	}
}

void Grid::DrawVertical(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart, DWORD colorEnd, int indentAxis, DWORD c1, DWORD c2)
{


	int counter = 1;

	for (int movingX = startX; movingX < endX; movingX += pxSpacing, counter++)
	{
		VECTOR3D v1(movingX, startY, 1);
		VECTOR3D v2(movingX, endY, 1);


		//draw main axis in different color
		if (counter == indentAxis)
			QueueTransformClipLine( v1.x, v1.y, v2.x, v2.y, c1, c2);
		else
			QueueTransformClipLine( v1.x, v1.y, v2.x, v2.y, colorStart, colorEnd);


	}
}

int Grid::GetSpacing() const
{
	return grid_spacing;
}


void Grid::Draw(DWORD* mem, int lpitch32)
{
	int viewWidth, viewHeight;
	if (viewport)
	{
		viewWidth = viewport->Rect.getWidth();
		viewHeight = viewport->Rect.getHeight();
		mem = viewport->TranslateMemory(mem, lpitch32);
	}
	else
	{
		viewWidth = GetSoftwareRasterizer()->clientRect.right;
		viewHeight = GetSoftwareRasterizer()->clientRect.bottom;
	}

	int gridStartX;
	int	gridStartY;

	//establish the start points for vertical and horizontal lines i.e allow grid to move with camera
	if (CenterCoordinate.x >= 0)
		gridStartX = grid_spacing - int(CenterCoordinate.x) % grid_spacing;  //need to go backwards so we subtract from spacing (space - remainder)
	else
		gridStartX = int(abs(CenterCoordinate.x) + 0.5f) % grid_spacing;

	if (CenterCoordinate.y >= 0)
		gridStartY = grid_spacing - int(CenterCoordinate.y + 0.5f) % grid_spacing;
	else
		gridStartY = int(abs(CenterCoordinate.y) + 0.5f) % grid_spacing;

	DWORD color1 = COLOR_RED;
	DWORD color2 = COLOR_BLUE;

	int yAxisIndent = -1;
	int xAxisIndent = -1;

	//see if axis is visible
	if (CenterCoordinate.x < 0 && CenterCoordinate.x + viewWidth > 0)
	{
		//fmod is same as mod but works on floats
		if (fmod((abs(CenterCoordinate.x)), grid_spacing) == 0)
			yAxisIndent = (abs(float(CenterCoordinate.x)) / grid_spacing) + 1;

		//ceil because it's going to be the + 1 line of whatever were drawing, so if we get 2.6, that means that 3rd grid line is the axis
		else
			yAxisIndent = ceil(abs(float(CenterCoordinate.x)) / grid_spacing);

	}
	if (CenterCoordinate.y < 0 && CenterCoordinate.y + viewHeight > 0)
	{
		if (fmod((abs(float(CenterCoordinate.y))), grid_spacing) == 0)
			xAxisIndent = (abs(float(CenterCoordinate.y)) / grid_spacing) + 1;
		//ceil because it's going to be the + 1 line of whatever were drawing, so if we get 2.6, that means that 3rd grid line is the axis
		else
			xAxisIndent = ceil(abs(float(CenterCoordinate.y)) / grid_spacing);

	}


	DrawHorizontal(mem, lpitch32, 0, gridStartY, viewWidth, viewHeight, grid_spacing, color1, color2, xAxisIndent);
	DrawVertical(mem, lpitch32, gridStartX, 0, viewWidth, viewHeight, grid_spacing, color1, color2, yAxisIndent);

}

