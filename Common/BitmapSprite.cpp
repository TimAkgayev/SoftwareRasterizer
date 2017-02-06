#include "BitmapSprite.h"

/*
SpriteLoader::~SpriteLoader()
{
	if (sprite.data)
		delete sprite.data;
}

void SpriteLoader::LoadSprite(string filename, int cell_width, int cell_height, int num_cells_x, int num_cells_y, int border_width)
{
	if (!LoadBitmapFromDisk(filename, &sprite))
		return;

	FlipBitmap(&sprite);

	numXCells = num_cells_x;
	numYCells = num_cells_y;
	borderWidth = border_width;
	cellWidth = cell_width;
	cellHeight = cell_height;
}

BITMAP_FILE SpriteLoader::GetCell(int row, int col)
{
	if (row > numXCells || col > numYCells || row <= 0 || col <= 0)
		return sprite;

	//calculate the starting memory
	int xoffset = col*borderWidth + (col - 1)*cellWidth;
	int yoffset = row*borderWidth + (row - 1)*cellHeight;

	DWORD* spriteMem = (DWORD*)sprite.data;
	spriteMem += xoffset + (yoffset * sprite.infoHeader.biWidth);

	//copy the region from main sprite into new bitmap
	BITMAP_FILE singleSprite;
	singleSprite.infoHeader.biBitCount = 32;
	singleSprite.infoHeader.biHeight = cellHeight;
	singleSprite.infoHeader.biWidth = cellWidth;
	singleSprite.infoHeader.biSizeImage = cellHeight * cellWidth * sizeof(DWORD);
	singleSprite.infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	singleSprite.data = (UCHAR*)new DWORD[cellHeight * cellWidth];

	DWORD* destSpriteMem = (DWORD*)singleSprite.data;


	//blit one to the other
	for (int i = 0; i < cellHeight; i++)
	{
		memcpy(destSpriteMem, spriteMem, cellWidth*sizeof(DWORD));
		spriteMem += sprite.infoHeader.biWidth;
		destSpriteMem += cellWidth;
	}


	return singleSprite;

}


*/


BitmapSprite::BitmapSprite(const BitmapSprite& rhv)
{
	timeOverflow = rhv.timeOverflow;
	currentTime = rhv.currentTime;
	currentFrame = rhv.currentFrame;
	position = rhv.position;
	isClockStarted = rhv.isClockStarted;
	startTime.QuadPart = rhv.startTime.QuadPart;
	endTime.QuadPart = rhv.endTime.QuadPart;
	perfFreq = rhv.perfFreq;
	mName = rhv.mName;

	for (int i = 0; i < rhv.frames.size(); i++)	
		frames.push_back(ResourceManager::GetImage(rhv.frames[i]->GetResouceManagerKey()));

	
}

BitmapSprite::BitmapSprite(int RMKey)
{
	timeOverflow = 1.0f;
	currentTime = 0;
	currentFrame = 0;
	position = VECTOR2D(0, 0);
	isClockStarted = false;
	startTime.QuadPart = 0;
	endTime.QuadPart = 0;
	QueryPerformanceFrequency(&perfFreq);

	BitmapFile* bmp = ResourceManager::GetImage(RMKey);
	mName = bmp->GetName();

	frames.push_back(bmp);

}

BitmapSprite::BitmapSprite(BitmapFile* bmp)
{

	timeOverflow = 1.0f;
	currentTime = 0;
	currentFrame = 0;
	position = VECTOR2D(0, 0);
	isClockStarted = false;
	startTime.QuadPart = 0;
	endTime.QuadPart = 0;
	QueryPerformanceFrequency(&perfFreq);

	mName = bmp->GetName();
	frames.push_back(bmp);

}

BitmapSprite& BitmapSprite::operator=(const BitmapSprite& rhv)
{
	//make sure it's not the same object we're assigning to 
	if (this == &rhv)
		return *this;


	timeOverflow = rhv.timeOverflow;
	currentTime = rhv.currentTime;
	currentFrame = rhv.currentFrame;
	position = rhv.position;
	isClockStarted = rhv.isClockStarted;
	startTime.QuadPart = rhv.startTime.QuadPart;
	endTime.QuadPart = rhv.endTime.QuadPart;
	perfFreq = rhv.perfFreq;
	mName = rhv.mName;

	//reload the frames into new instance
	for (int i = 0; i < rhv.frames.size(); i++)
		frames.push_back(ResourceManager::GetImage(frames[i]->GetResouceManagerKey()));

	return *this;
}

BitmapSprite::~BitmapSprite()
{

}

int BitmapSprite::GetID()
{
	return mID;
}

void BitmapSprite::AddFrame(BitmapFile* bmp_p)
{
	mName = bmp_p->GetName();
	frames.push_back(bmp_p);
}
void BitmapSprite::AddFrame(int key)
{
	
	frames.push_back(ResourceManager::GetImage(key));
}

void BitmapSprite::SetFramerate(int rate)
{
	timeOverflow = 1.0f / rate;
}

void BitmapSprite::SetPosition(float x, float y)
{
	position = VECTOR2D(x, y);
}

void BitmapSprite::SetPosition(VECTOR2D& pos)
{
	position = pos;
}

void BitmapSprite::SetName(string name)
{
	mName = name;
}

void BitmapSprite::GetFrameDimensions(POINT& wh) const
{
	wh.x = frames[0]->GetInfoHeader().biWidth;
	wh.y = frames[0]->GetInfoHeader().biHeight;
}

void BitmapSprite::GetBoundingRectangle(RECT2D& ret)
{

	ret.setHeight(frames[0]->GetInfoHeader().biHeight);
	ret.setWidth(frames[0]->GetInfoHeader().biWidth);
	ret.setPos(position);
}

int BitmapSprite::GetFrame(int frame)
{
	if (frame < frames.size())
		return frames[frame]->GetResouceManagerKey();
}

string BitmapSprite::GetName()
{
	return mName;
}

int BitmapSprite::GetNumFrames()
{
	return frames.size();
}

VECTOR2D BitmapSprite::GetPosition() const
{
	return position;
}



void BitmapSprite::RemoveFrame(int frame)
{
	frames.erase(frames.begin() + frame);
}

void BitmapSprite::Draw(DWORD* vmem, int lpitch32)
{
	double deltaTime = 0;
	if (isClockStarted)
	{
		QueryPerformanceCounter(&endTime);
		double finalTime = endTime.QuadPart - startTime.QuadPart;
		deltaTime = finalTime / perfFreq.QuadPart;
	}

	currentTime += deltaTime;
	if (currentTime >= timeOverflow)
	{
		currentFrame++;
		currentTime = 0;
	}

	if (currentFrame >= frames.size())
		currentFrame = 0;


	DrawBitmapWithClipping(vmem, lpitch32, frames[currentFrame], int(position.x + 0.5f), int(position.y + 0.5f), NULL);

	QueryPerformanceCounter(&startTime);
	isClockStarted = true;
}

bool BitmapSprite::CheckCollision(RECT& rect)
{

	RECT2D thisRect(position.x, position.y, frames[0]->GetInfoHeader().biWidth, frames[0]->GetInfoHeader().biHeight);


	if (thisRect.isPointInRect(rect.left, rect.top) || thisRect.isPointInRect(rect.right, rect.bottom))
		return true;

	return false;
}

void BitmapSprite::RotateLeft()
{
	vector<BitmapFile*>::iterator vIter = frames.begin();
	for (vIter; vIter < frames.end(); vIter++)
	{
		(*vIter)->RotateBitmapLeft();
	}
}
