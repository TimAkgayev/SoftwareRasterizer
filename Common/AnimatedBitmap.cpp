#include "AnimatedBitmap.h"



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

	for (int i = 0; i < rhv.frames.size(); i++)
	{
		BITMAP_FILE copy;
		copy.infoHeader = rhv.frames[i].infoHeader;
		copy.fileHeader = rhv.frames[i].fileHeader;
		copy.data = new unsigned char[rhv.frames[i].infoHeader.biSizeImage];
		memcpy(copy.data, rhv.frames[i].data, rhv.frames[i].infoHeader.biSizeImage);
		frames.push_back(copy);
	}


}


AnimatedBitmap AnimatedBitmap::operator=(const AnimatedBitmap& rhv)
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

	//first realease any frames that are present
	vector<BITMAP_FILE>::iterator vIter;
	for (vIter = frames.begin(); vIter < frames.end(); vIter++)
		Unload_Bitmap_File(&(*vIter));

	//copy over the frames
	for (int i = 0; i < rhv.frames.size(); i++)
	{
		BITMAP_FILE copy;
		copy.infoHeader = rhv.frames[i].infoHeader;
		copy.fileHeader = rhv.frames[i].fileHeader;
		copy.data = new unsigned char[rhv.frames[i].infoHeader.biSizeImage];
		memcpy(copy.data, rhv.frames[i].data, rhv.frames[i].infoHeader.biSizeImage);
		frames.push_back(copy);
	}

	return *this;
}

AnimatedBitmap::~AnimatedBitmap()
{
	vector<BITMAP_FILE>::iterator vIter;
	for (vIter = frames.begin(); vIter < frames.end(); vIter++)
	{
		Unload_Bitmap_File(&(*vIter));
	}
}

void AnimatedBitmap::AddFrame(string path, float scaleFactor)
{
	BITMAP_FILE tBmp;
	BITMAP_FILE newBmp;
	if (!LoadBitmapFromDisk(path, &tBmp))
		return;
	
	ResizeBitmap(&newBmp, &tBmp, scaleFactor, scaleFactor);

	FlipBitmap(&newBmp);
	frames.push_back(newBmp);

	//delete the original bitmap
	delete tBmp.data;
}

void AnimatedBitmap::AddFrame(BITMAP_FILE image)
{
	frames.push_back(image);
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

void AnimatedBitmap::GetFrameDimensions(POINT& wh) const
{
	wh.x = frames[0].infoHeader.biWidth;
	wh.y = frames[0].infoHeader.biHeight;
}

void AnimatedBitmap::GetBoundingRectangle(RECT2D& ret)
{

	ret.setHeight(frames[0].infoHeader.biHeight);
	ret.setWidth(frames[0].infoHeader.biWidth);
	ret.setPos(position);
}

BITMAP_FILE* AnimatedBitmap::GetFrame(int frame)
{
	if (frame < frames.size())
		return &frames[frame];
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


	DrawBitmapWithClipping(vmem, lpitch32, &frames[currentFrame], int(position.x + 0.5f), int(position.y + 0.5f), NULL);

	QueryPerformanceCounter(&startTime);
	isClockStarted = true;
}

bool AnimatedBitmap::CheckCollision(RECT& rect)
{

	RECT2D thisRect(position.x, position.y, frames[0].infoHeader.biWidth, frames[0].infoHeader.biHeight);


	if (thisRect.isPointInRect(rect.left, rect.top) || thisRect.isPointInRect(rect.right, rect.bottom))
		return true;

	return false;
}

void AnimatedBitmap::RotateLeft()
{
	vector<BITMAP_FILE>::iterator vIter = frames.begin();
	for (vIter; vIter < frames.end(); vIter++)
	{
		RotateBitmapLeft(&(*vIter));
	}
}