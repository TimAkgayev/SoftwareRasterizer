#pragma once
#include "SoftwareRasterizer.h"



class AnimatedBitmap
{
public:
	AnimatedBitmap();
	AnimatedBitmap(const AnimatedBitmap& rhv);
	AnimatedBitmap(int RMKey, string name = "", float scaleFactor = 1.0f);
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
	static AnimatedBitmap* GetLoadedInstance(string name);
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

	static vector<AnimatedBitmap*> allLoaded;
};

