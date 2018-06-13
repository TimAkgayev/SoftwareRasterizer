#include "BitmapImage.h"
#include "SoftwareRasterizer.h"





BitmapImage::BitmapImage()
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



BitmapImage::BitmapImage(int RMKey)
{
	timeOverflow = 1.0f;
	currentTime = 0;
	currentFrame = 0;
	position = VECTOR2D(0, 0);
	isClockStarted = false;
	startTime.QuadPart = 0;
	endTime.QuadPart = 0;
	QueryPerformanceFrequency(&perfFreq);

	Bitmap* bmp = ImageManager::GetImage(RMKey);
	mName = bmp->GetName();

	frames.push_back(bmp);

}

BitmapImage::BitmapImage(Bitmap* bmp)
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


BitmapImage::~BitmapImage()
{

}

int BitmapImage::GetID()
{
	return mID;
}

void BitmapImage::AddFrame(Bitmap* bmp_p)
{
	mName = bmp_p->GetName();
	frames.push_back(bmp_p);
}
void BitmapImage::AddFrame(int key)
{

	frames.push_back(ImageManager::GetImage(key));
}

void BitmapImage::SetFramerate(int rate)
{
	timeOverflow = 1.0f / rate;
}

void BitmapImage::SetPosition(float x, float y)
{
	position = VECTOR2D(x, y);
}

void BitmapImage::SetPosition(VECTOR2D& pos)
{
	position = pos;
}

void BitmapImage::SetName(std::string name)
{
	mName = name;
}

void BitmapImage::GetFrameDimensions(POINT& wh) const
{
	wh.x = frames[0]->GetInfoHeader().biWidth;
	wh.y = frames[0]->GetInfoHeader().biHeight;
}

void BitmapImage::GetBoundingRectangle(RECT2D& ret)
{

	ret.setHeight(frames[0]->GetInfoHeader().biHeight);
	ret.setWidth(frames[0]->GetInfoHeader().biWidth);
	ret.setPos(position);
}

int BitmapImage::GetFrame(int frame)
{
	if (frame < frames.size())
		return frames[frame]->GetResouceManagerKey();
}

std::string BitmapImage::GetName()
{
	return mName;
}

int BitmapImage::GetNumFrames()
{
	return frames.size();
}

VECTOR2D BitmapImage::GetPosition() const
{
	return position;
}



void BitmapImage::RemoveFrame(int frame)
{
	frames.erase(frames.begin() + frame);
}

void BitmapImage::Draw(DWORD* vmem, int lpitch32)
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

bool BitmapImage::CheckCollision(RECT& rect)
{

	RECT2D thisRect(position.x, position.y, frames[0]->GetInfoHeader().biWidth, frames[0]->GetInfoHeader().biHeight);


	if (thisRect.isPointInRect(rect.left, rect.top) || thisRect.isPointInRect(rect.right, rect.bottom))
		return true;

	return false;
}

void BitmapImage::RotateLeft()
{
	std::vector<Bitmap*>::iterator vIter = frames.begin();
	for (vIter; vIter < frames.end(); vIter++)
	{
		(*vIter)->RotateBitmapLeft();
	}
}






