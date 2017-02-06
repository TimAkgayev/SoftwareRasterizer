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




#pragma once
#include "SoftwareRasterizer.h"



class AnimatedBitmap
{
public:
	AnimatedBitmap(const AnimatedBitmap& rhv);
	AnimatedBitmap(int RMKey);
	AnimatedBitmap(BitmapFile*);
	~AnimatedBitmap();

	AnimatedBitmap& operator=(const AnimatedBitmap& rhv);

	void AddFrame(BitmapFile* bmpPtr);
	void AddFrame(int RMKey);
	void Draw(DWORD* vmem, int lpitch32);

	void SetFramerate(int framerate);
	void SetPosition(VECTOR2D& pos);
	void SetPosition(float x, float y);
	void SetName(string name);

	void GetFrameDimensions(POINT&) const;
	void GetBoundingRectangle(RECT2D&);
	int GetFrame(int frame);
	string GetName();
	int GetNumFrames();
	VECTOR2D GetPosition() const;

	void RemoveFrame(int);

	void RotateLeft();

	bool CheckCollision(RECT&);
private:
	vector<BitmapFile*> frames;
	VECTOR2D position;
	float timeOverflow;
	double currentTime;
	int currentFrame;
	bool isClockStarted;
	LARGE_INTEGER endTime, startTime, perfFreq;
	int mID;
	string mName;

	int GetID();
};

