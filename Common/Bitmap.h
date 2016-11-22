#pragma once
#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "ResourceManager.h"
#include "Math56.h"


using namespace std;

//low level bitmap structure
typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	PALETTEENTRY	 palette[256];
	UCHAR*			 data;

}BITMAP_FILE, *BITMAP_FILE_PTR;


int LoadBitmapFromDisk(string szFileName, BITMAP_FILE_PTR bitmap);
int LoadFileToMemory(string szFileName, unsigned char** buffer);
int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);
void FlipBitmap(BITMAP_FILE* bitmap);
void ResizeBitmap(BITMAP_FILE* out, BITMAP_FILE* original, RECT resizeRectangle);
void ResizeBitmap(BITMAP_FILE* out, BITMAP_FILE* original, float xscale, float yscale);
void RotateBitmapLeft(BITMAP_FILE*);
