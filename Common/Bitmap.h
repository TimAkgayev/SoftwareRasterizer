#pragma once
#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
 
#include "Math56.h"


using namespace std;


class BitmapFile
{
public:

	

	BitmapFile(int width, int height);
	BitmapFile(string fileName);
	~BitmapFile();
	BitmapFile& operator=(const BitmapFile& rhv);

	void FlipBitmap();
	void ResizeBitmap(RECT resizeRectangle);
	void ResizeBitmap(float xscale, float yscale);
	void RotateBitmapLeft();

	void SetResourceManagerKey(int id);
	void SetName(string);

	POINT GetDimensions() const;
	int   GetDataSize() const;
	const BITMAPFILEHEADER& GetFileHeader() const;
	const BITMAPINFOHEADER& GetInfoHeader() const;
	UCHAR* GetData() const;
	const PALETTEENTRY* GetPalette() const;
	int GetResouceManagerKey() const;
	string GetName() const;

private:
	int LoadBitmapFromDisk(string szFileName);
	int Unload_Bitmap_File();

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	PALETTEENTRY	 palette[256];
	UCHAR*			 data;

	string name;
	int resource_manager_key;


};


