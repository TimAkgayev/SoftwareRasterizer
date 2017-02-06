#include "ResourceManager.h"



int BitmapFile::LoadBitmapFromDisk(string szFileName)
{

	// Open the bitmap file
	HANDLE hFile = CreateFileA(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	// Read the bitmap file header
	DWORD dwBytesRead;
	BOOL bOK = ReadFile(hFile, &fileHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);

	//see if the correct byte number was read and that it's actually a bitmap (bfType has to match)
	if (!bOK || (dwBytesRead != sizeof(BITMAPFILEHEADER)) || (fileHeader.bfType != 0x4D42))
	{
		CloseHandle(hFile);
		return 0;
	}


	// Read the bitmap info head
	bOK = ReadFile(hFile, &infoHeader, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
	if (!bOK || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
	{
		CloseHandle(hFile);
		return 0;
	}

	// Store the width and height of the bitmap
	int m_iWidth = (int)infoHeader.biWidth;
	int m_iHeight = (int)infoHeader.biHeight;


	unsigned int sz;
	SetFilePointer(hFile, fileHeader.bfOffBits, NULL, FILE_BEGIN);
	data = (UCHAR*) new unsigned char[infoHeader.biSizeImage];
	bOK = ReadFile(hFile, data, infoHeader.biSizeImage, &dwBytesRead, NULL);

	//flip the bytes to little endian
	DWORD* temp = (DWORD*)data;
	for (int i = 0; i < infoHeader.biSizeImage / 4; i++)
	{
		//photoshop saves a,r,g,b into little endian b,g,r,a
		//this engine is expecting r,g,b,a so all that's needed is to flip 1st and 3rd bytes
		//be careful with this code if the image source program is working in r,g,b,a instead of a,r,g,b
		char* byteMem = (char*)&temp[i];
		char btemp = byteMem[0];
		byteMem[0] = byteMem[2];
		byteMem[2] = btemp;

	}

	if (bOK)
		return 1;



	// Something went wrong, so cleanup everything
	return 0;

}

int BitmapFile::Unload_Bitmap_File()
{
	// this function releases all memory associated with the bitmap
	if (data)
	{
		// release memory
		delete (data);

		// reset pointer
		data = nullptr;
	}

	return 1;
}

void BitmapFile::FlipBitmap()
{
	if (infoHeader.biBitCount == 32)
	{
		DWORD* workingBuffer = (DWORD*) new UCHAR[infoHeader.biSizeImage];
		DWORD* workingBufferStart = workingBuffer;
		DWORD* sourceBuffer = (DWORD*)data;

		//go to the end of the source and go backwards
		sourceBuffer += infoHeader.biWidth * (infoHeader.biHeight - 1);

		for (int row = infoHeader.biHeight - 1; row >= 0; row--)
		{
			memcpy(workingBuffer, sourceBuffer, infoHeader.biWidth * (infoHeader.biBitCount / 8));

			workingBuffer += infoHeader.biWidth;
			sourceBuffer -= infoHeader.biWidth;
		}

		memcpy(data, workingBufferStart, infoHeader.biSizeImage);
		delete workingBufferStart;

	}



}

void BitmapFile::ResizeBitmap(RECT resizeRectangle)
{
	//only works for 32 bit bitmaps for now
	if (infoHeader.biBitCount != 32)
		return;

	int newXDimension = resizeRectangle.right - resizeRectangle.left;
	int newYDimension = resizeRectangle.bottom - resizeRectangle.top;



	int newDataSize = (infoHeader.biBitCount / 8) * newXDimension * newYDimension;

	//layout of bmp
	//file header
	//dib header (info header)
	//data

	//first create a new file header
	//everything is the same as the old bitmap except the file size
	BITMAPFILEHEADER newFH = fileHeader;
	newFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + newFH.bfOffBits + newDataSize;


	//create a new info header
	BITMAPINFOHEADER newIH = infoHeader;
	newIH.biSize = sizeof(BITMAPINFOHEADER);
	newIH.biHeight = newYDimension;
	newIH.biWidth = newXDimension;
	newIH.biSizeImage = newDataSize;

	//allocate the data (this is only for 32 bit bitmap)
	DWORD* newData = (DWORD*) new DWORD[newXDimension * newYDimension];


	//calculate the resize ratio in x dimension
	float ratioX = float(newXDimension) / infoHeader.biWidth;
	float ratioY = float(newYDimension) / infoHeader.biHeight;

	DWORD* newDataOffset = newData;
	DWORD* origDataOffset = (DWORD*)data;

	//iterate throught the original bitmap and write to the new one
	for (int row = 0; row < infoHeader.biHeight; row++)
	{
		for (int column = 0; column < infoHeader.biWidth; column++)
		{
			int destinationRow = int(ratioY * row);
			int destinationPixel = int(ratioX * column);
			newDataOffset[destinationRow* newXDimension + destinationPixel] = origDataOffset[column];
		}

		//newDataOffset += newXDimension;
		origDataOffset += infoHeader.biWidth;
	}

	//delete the old data
	delete data;

	//copy everything back to the output bitmap
	fileHeader = newFH;
	infoHeader = newIH;
	data = (unsigned char*)newData;



}
void BitmapFile::ResizeBitmap(float xscale, float yscale)
{
	//calculate the size based on percentage
	int newXDim = int(infoHeader.biWidth * xscale + 0.5f);
	int newYDim = int(infoHeader.biHeight * yscale + 0.5f);
	RECT rc;
	rc.left = 0;
	rc.right = newXDim;
	rc.top = 0;
	rc.bottom = newYDim;

	ResizeBitmap(rc);
}

void BitmapFile::RotateBitmapLeft()
{
	//allocate temporary data
	DWORD* tempData = (DWORD*) new char[infoHeader.biSizeImage];

	//create new info header
	int newXDim = infoHeader.biHeight;
	int newYDim = infoHeader.biWidth;

	DWORD* origData = (DWORD*)data;

	//iterate throught the original bitmap and write to the new one
	for (int row = 0; row < infoHeader.biHeight; row++)
	{
		for (int columnOld = 0, columnNew = infoHeader.biWidth - 1; columnOld < infoHeader.biWidth, columnNew >= 0; columnOld++, columnNew--)
		{

			tempData[columnNew*newXDim + row] = origData[columnOld];
		}

		//newDataOffset += newXDimension;
		origData += infoHeader.biWidth;
	}

	//copy over the temp data
	memcpy(data, tempData, infoHeader.biSizeImage);
	infoHeader.biWidth = newXDim;
	infoHeader.biHeight = newYDim;

	delete tempData;

	
}

BitmapFile::BitmapFile(string filename)
{
	LoadBitmapFromDisk(filename);
	size_t pos = filename.find_last_of('\\');
	name = filename.substr(pos + 1);
	int x = 0;

}

BitmapFile::BitmapFile(int width, int height)
{
	memset(&fileHeader, 0, sizeof(BITMAPFILEHEADER));
	memset(&infoHeader, 0, sizeof(BITMAPINFOHEADER));

	//set up info
	infoHeader.biBitCount = 32;
	infoHeader.biClrImportant = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biHeight = height;
	infoHeader.biWidth = width;
	infoHeader.biPlanes = 1;
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biSizeImage = width*height * sizeof(DWORD);
	infoHeader.biXPelsPerMeter = 2835;
	infoHeader.biYPelsPerMeter = 2835;

	//allocate space
	data = new UCHAR[infoHeader.biSizeImage];

	//now set up file header last
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 1024);
	fileHeader.bfSize = infoHeader.biSize + sizeof(BITMAPFILEHEADER) + infoHeader.biSizeImage;
	fileHeader.bfType = 'BM';

}

BitmapFile::~BitmapFile()
{
	if (data)
		delete data;

	data = nullptr;

}

BitmapFile& BitmapFile::operator=(const BitmapFile& rhv)
{
	if (data)
		delete data;

	data = nullptr;
	memset(&fileHeader, 0, sizeof(BITMAPFILEHEADER));
	memset(&infoHeader, 0, sizeof(BITMAPINFOHEADER));

	fileHeader = rhv.fileHeader;
	infoHeader = rhv.infoHeader;

	data = new UCHAR[rhv.GetDataSize()];
	memcpy(data, rhv.GetData(), rhv.GetDataSize());

	return (*this);
}


void BitmapFile::SetResourceManagerKey(int key)
{
	resource_manager_key = key;
}

void BitmapFile::SetName(string n)
{
	name = n;
}

const PALETTEENTRY* BitmapFile::GetPalette() const
{
	return palette;
}

POINT BitmapFile::GetDimensions() const
{
	POINT p = { infoHeader.biWidth, infoHeader.biHeight };
	return p;
}
int   BitmapFile::GetDataSize() const
{
	return infoHeader.biSizeImage;
}
const BITMAPFILEHEADER& BitmapFile::GetFileHeader() const
{
	return fileHeader;
}
const BITMAPINFOHEADER& BitmapFile::GetInfoHeader() const
{
	return infoHeader;
}
UCHAR* BitmapFile::GetData() const
{
	return data;
}


int BitmapFile::GetResouceManagerKey() const
{
	return resource_manager_key;
}

string BitmapFile::GetName() const
{
	return name;
}












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



AnimatedBitmap::AnimatedBitmap(int RMKey)
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

AnimatedBitmap::AnimatedBitmap(BitmapFile* bmp)
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


AnimatedBitmap::~AnimatedBitmap()
{

}

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
























map<int, BitmapFile*> ResourceManager::bmpImageMap;

int ResourceManager::totalBytesLoaded = 0;

ResourceManager::ResourceManager()
{

}
ResourceManager::~ResourceManager()
{
	auto iter = bmpImageMap.begin();
	for (iter; iter != bmpImageMap.end(); iter++)
	{
		delete iter->second;
		iter->second = nullptr;
	}
}

int ResourceManager::LoadImage(string path)
{
	BitmapFile* bmp = new BitmapFile(path);

	//make sure this file doesn't already exist, if it does just send back the handle
	auto mIter = bmpImageMap.begin();
	for (mIter; mIter != bmpImageMap.end(); mIter++)
	{
		if (bmp->GetName().compare(mIter->second->GetName()) == 0)
		{
			delete bmp;
			return mIter->first;
		}
	}



	if (bmp->GetData() != 0)
	{
		int returnKey = bmpImageMap.size();
		bmp->SetResourceManagerKey(returnKey);
		bmpImageMap.insert(std::make_pair(bmpImageMap.size(), bmp));
		totalBytesLoaded += bmp->GetDataSize();
		return returnKey;
	}

	return -1;

}

vector<int> ResourceManager::LoadImagesFromSprite(string path, int StartRow, int StartCol, int EndRow, int EndCol, int cell_width, int cell_height, int border_width)
{
	//load sprite
	BitmapFile bmp(path);

	//our return set of keys
	vector<int> returnKeys;

	//make sure we're in bounds
	if ((EndCol * cell_width + (EndCol + 1)*border_width > bmp.GetInfoHeader().biWidth) || (EndCol * cell_height + (EndCol + 1)*border_width > bmp.GetInfoHeader().biHeight))
		return returnKeys;


	//copy the region from main sprite into new bitmap
	BitmapFile* spriteCell;
	DWORD* destSpriteMem;
	
	

	for (int row = StartRow; row <= EndRow; row++)
	{

		for (int col = StartCol; col <= EndCol; col++)
		{
			//calculate the starting memory
			int xoffset = col*border_width + (col - 1)*cell_width;
			int yoffset = row*border_width + (row - 1)*cell_height;

			DWORD* spriteMem = (DWORD*)bmp.GetData();
			spriteMem += xoffset + (yoffset * bmp.GetInfoHeader().biWidth);


			//allocate memory for new cell
			spriteCell = new BitmapFile(cell_width, cell_height);
			destSpriteMem = (DWORD*)spriteCell->GetData();

			//blit each cell
			for (int i = 0; i < cell_height; i++)
			{
				memcpy(destSpriteMem, spriteMem, cell_width * sizeof(DWORD));
				spriteMem += bmp.GetInfoHeader().biWidth;
				destSpriteMem += cell_width;
			}

			//assign a key and add to map
			int returnKey = bmpImageMap.size();
			spriteCell->SetResourceManagerKey(returnKey);
			bmpImageMap.insert(std::make_pair(bmpImageMap.size(), spriteCell));
			totalBytesLoaded += spriteCell->GetDataSize();
			returnKeys.push_back(returnKey);
		}
	}



	return returnKeys;
}

BitmapFile* ResourceManager::GetImage(int key)
{
	return bmpImageMap[key];
}


BitmapFile* ResourceManager::GetImage(string name)
{
	auto mIter = bmpImageMap.begin();
	for (mIter; mIter != bmpImageMap.end(); mIter++)
	{
		if (name.compare(mIter->second->GetName()) == 0)
			return mIter->second;
	}

	return nullptr;
}