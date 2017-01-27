#include "ResourceManager.h"

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