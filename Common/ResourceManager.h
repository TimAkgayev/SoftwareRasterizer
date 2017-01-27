#pragma once
#include "stdinclude.h"
#include "Bitmap.h"

class ResourceManager
{
public:

	ResourceManager();
	~ResourceManager();

	static int LoadImage(string path);
	static vector<int> LoadImagesFromSprite(string path, int StartRow, int StartCol, int EndRow, int EndCol, int cell_width, int cell_height, int border_width = 1);

	static BitmapFile* GetImage(int key);



private:
	static map<int, BitmapFile*> bmpImageMap;

	static int totalBytesLoaded;

};
