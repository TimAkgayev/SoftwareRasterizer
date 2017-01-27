#include "AnimatedBitmap.h"

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

AnimatedBitmap::AnimatedBitmap()
{
	timeOverflow = 1.0f;
	currentTime = 0;
	currentFrame = 0;
	position = VECTOR2D(0, 0);
	isClockStarted = false;
	startTime.QuadPart = 0;
	endTime.QuadPart = 0;
	QueryPerformanceFrequency(&perfFreq);
	mID = allLoaded.size();

	//save this instance
	allLoaded.push_back(this);
}

AnimatedBitmap::AnimatedBitmap(const AnimatedBitmap& rhv)
{
	timeOverflow = rhv.timeOverflow;
	currentTime = rhv.currentTime;
	currentFrame = rhv.currentFrame;
	position = rhv.position;
	isClockStarted = rhv.isClockStarted;
	startTime.QuadPart = rhv.startTime.QuadPart;
	endTime.QuadPart = rhv.endTime.QuadPart;
	perfFreq = rhv.perfFreq;
	mID = allLoaded.size();
	mName = rhv.mName;

	for (int i = 0; i < rhv.frames.size(); i++)	
		frames.push_back(ResourceManager::GetImage(rhv.frames[i]->GetResouceManagerKey()));

	//save this instance
	allLoaded.push_back(this);
}

AnimatedBitmap::AnimatedBitmap(int RMKey, string name, float scaleFactor)
{
	mID = allLoaded.size();
	mName = name;

	BitmapFile* bmp = ResourceManager::GetImage(RMKey);
	bmp->ResizeBitmap(scaleFactor, scaleFactor);
	bmp->FlipBitmap();

	frames.push_back(bmp);

	mName = bmp->GetName();

	//save this instance
	allLoaded.push_back(this);
}



AnimatedBitmap& AnimatedBitmap::operator=(const AnimatedBitmap& rhv)
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

AnimatedBitmap::~AnimatedBitmap()
{

	if (allLoaded.size())
	{
		vector<AnimatedBitmap*>::iterator thisIter;
		for (thisIter = allLoaded.begin(); thisIter != allLoaded.end(); thisIter++)
		{
			if ((*thisIter)->GetID() == this->mID)
			{
				allLoaded.erase(thisIter);
				break;
			}
		
				
		}
	}
	
}

vector<AnimatedBitmap*> AnimatedBitmap::allLoaded;

int AnimatedBitmap::GetID()
{
	return mID;
}

void AnimatedBitmap::AddFrame(BitmapFile* bmp_p)
{
	mName = bmp_p->GetName();
	frames.push_back(bmp_p);
}
void AnimatedBitmap::AddFrame(int key)
{
	
	frames.push_back(ResourceManager::GetImage(key));
}

void AnimatedBitmap::SetFramerate(int rate)
{
	timeOverflow = 1.0f / rate;
}

void AnimatedBitmap::SetPosition(float x, float y)
{
	position = VECTOR2D(x, y);
}

void AnimatedBitmap::SetPosition(VECTOR2D& pos)
{
	position = pos;
}

void AnimatedBitmap::SetName(string name)
{
	mName = name;
}

void AnimatedBitmap::GetFrameDimensions(POINT& wh) const
{
	wh.x = frames[0]->GetInfoHeader().biWidth;
	wh.y = frames[0]->GetInfoHeader().biHeight;
}

void AnimatedBitmap::GetBoundingRectangle(RECT2D& ret)
{

	ret.setHeight(frames[0]->GetInfoHeader().biHeight);
	ret.setWidth(frames[0]->GetInfoHeader().biWidth);
	ret.setPos(position);
}

int AnimatedBitmap::GetFrame(int frame)
{
	if (frame < frames.size())
		return frames[frame]->GetResouceManagerKey();
}

string AnimatedBitmap::GetName()
{
	return mName;
}

int AnimatedBitmap::GetNumFrames()
{
	return frames.size();
}

VECTOR2D AnimatedBitmap::GetPosition() const
{
	return position;
}

AnimatedBitmap* AnimatedBitmap::GetLoadedInstance(string name)
{
	vector<AnimatedBitmap*>::iterator thisIter;
	for (thisIter = allLoaded.begin(); thisIter < allLoaded.end(); thisIter++)
	{
		if ((*thisIter)->GetName().compare(name) == 0)
			return *thisIter;
	}
}

void AnimatedBitmap::RemoveFrame(int frame)
{
	frames.erase(frames.begin() + frame);
}

void AnimatedBitmap::Draw(DWORD* vmem, int lpitch32)
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

bool AnimatedBitmap::CheckCollision(RECT& rect)
{

	RECT2D thisRect(position.x, position.y, frames[0]->GetInfoHeader().biWidth, frames[0]->GetInfoHeader().biHeight);


	if (thisRect.isPointInRect(rect.left, rect.top) || thisRect.isPointInRect(rect.right, rect.bottom))
		return true;

	return false;
}

void AnimatedBitmap::RotateLeft()
{
	vector<BitmapFile*>::iterator vIter = frames.begin();
	for (vIter; vIter < frames.end(); vIter++)
	{
		(*vIter)->RotateBitmapLeft();
	}
}
