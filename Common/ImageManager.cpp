#include "ImageManager.h"





std::map<int, Bitmap*> ImageManager::bmpImageMap;

int ImageManager::totalBytesLoaded = 0;

ImageManager::ImageManager()
{

}
ImageManager::~ImageManager()
{
	auto iter = bmpImageMap.begin();
	for (iter; iter != bmpImageMap.end(); iter++)
	{
		delete iter->second;
		iter->second = nullptr;
	}
}

int ImageManager::AddImage(Bitmap* file)
{
	int returnKey = bmpImageMap.size();
	file->SetImageManagerKey(returnKey);
	bmpImageMap.insert(std::make_pair(bmpImageMap.size(), file));
	totalBytesLoaded += file->GetDataSize();
	return returnKey;
}

int ImageManager::LoadImageFromFile(std::string path)
{
	Bitmap* bmp = new Bitmap(path);

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
		bmp->SetImageManagerKey(returnKey);
		bmpImageMap.insert(std::make_pair(bmpImageMap.size(), bmp));
		totalBytesLoaded += bmp->GetDataSize();
		return returnKey;
	}

	return -1;

}

std::vector<int> ImageManager::LoadImagesFromSprite(std::string path, int StartRow, int StartCol, int EndRow, int EndCol, int cell_width, int cell_height, int border_width)
{
	//load sprite
	Bitmap bmp(path);

	//our return set of keys
	std::vector<int> returnKeys;

	//make sure we're in bounds
	if ((EndCol * cell_width + (EndCol + 1)*border_width > bmp.GetInfoHeader().biWidth) || (EndCol * cell_height + (EndCol + 1)*border_width > bmp.GetInfoHeader().biHeight))
		return returnKeys;


	//copy the region from main sprite into new bitmap
	Bitmap* spriteCell;
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
			spriteCell = new Bitmap(cell_width, cell_height);
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
			spriteCell->SetImageManagerKey(returnKey);
			bmpImageMap.insert(std::make_pair(bmpImageMap.size(), spriteCell));
			totalBytesLoaded += spriteCell->GetDataSize();
			returnKeys.push_back(returnKey);
		}
	}



	return returnKeys;
}

Bitmap* ImageManager::GetImage(int key)
{
	return bmpImageMap[key];
}


Bitmap* ImageManager::GetImage(std::string name)
{
	auto mIter = bmpImageMap.begin();
	for (mIter; mIter != bmpImageMap.end(); mIter++)
	{
		if (name.compare(mIter->second->GetName()) == 0)
			return mIter->second;
	}

	return nullptr;
}