#pragma once
#include "SoftwareRasterizer.h"

class SpriteLoader
{
public:
	~SpriteLoader();

	void LoadSprite(string filename, int cell_width, int cell_height, int num_cells_x, int num_cells_y, int border_width);
	BITMAP_FILE GetCell(int row, int col);

private:
	BITMAP_FILE sprite;
	int numXCells;
	int numYCells;
	int borderWidth;
	int cellWidth;
	int cellHeight;
};


class AnimatedBitmap
{
public:
	AnimatedBitmap();
	AnimatedBitmap(const AnimatedBitmap& rhv);
	AnimatedBitmap(string filename, float scaleFactor = 1.0f);
	~AnimatedBitmap();

	AnimatedBitmap operator=(const AnimatedBitmap& rhv);

	void AddFrame(string filename, float scaleFactor = 1.0f);
	void AddFrame(BITMAP_FILE);
	void Draw(DWORD* vmem, int lpitch32);

	void SetFramerate(int framerate);
	void SetPosition(VECTOR2D& pos);
	void SetPosition(float x, float y);

	void GetFrameDimensions(POINT&) const;
	void GetBoundingRectangle(RECT2D&);
	BITMAP_FILE* GetFrame(int frame);

	void RemoveFrame(int);

	void RotateLeft();

	bool CheckCollision(RECT&);
private:
	vector<BITMAP_FILE> frames;
	VECTOR2D position;
	float timeOverflow;
	double currentTime;
	int currentFrame;
	bool isClockStarted;
	LARGE_INTEGER endTime, startTime, perfFreq;
};

