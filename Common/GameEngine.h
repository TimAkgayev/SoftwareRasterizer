#pragma once

#include "SoftwareRasterizer.h"
#include "Shapes.h"
#include "UserInterface.h"
#include "Camera.h"
#include "Timer.h"
#include "SMath.h"


int Initialization();
int Update();
int Shutdown();



class Player
{
public:
	Player();
	~Player();

	void AddFrontAnimation(BitmapImage* anim);
	void AddLeftAnimation(BitmapImage* anim);
	void AddRightAnimation(BitmapImage* anim);
	void AddBackAnimation(BitmapImage* anim);

	void SetPosition(VECTOR2D& pos);
	void SetPosition(float x, float y);
	void SetDirection(string dir);

	VECTOR2D GetPosition() const;
	float GetMoveSpeed() const;
	void GetBoundingRect(RECT2D&);

	void Draw(DWORD* vmem, int lpitch32);

private:
	VECTOR2D position;
	string facingDirection;
	float moveSpeed;
	BitmapImage *frontAnim, *leftAnim, *rightAnim, *backAnim;
	bool isPositionUpdated;

};
class World
{
public:
	World();

	UserInterface* userInterface;
	Player* player;
	vector<BitmapImage> collisionObjects;

	void SoftwareDraw(DWORD*, int);
	void HardwareDraw(double);
};




class Grid
{
public:
	Grid();
	~Grid();

	void Draw(DWORD* mem, int lpitch32);

	void SetCenterCoordinate(float x, float y);
	void SetSpacing(int spacing);
	void SetLineThickness(int thickness);

	int GetSpacing() const;
private:

	VECTOR2D CenterCoordinate;
	int grid_spacing;
	int line_thickness;
	void DrawHorizontal(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart = COLOR_RED, DWORD colorEnd = COLOR_RED, int indentAxis = -1, DWORD c1 = COLOR_YELLOW, DWORD c2 = COLOR_YELLOW);
	void DrawVertical(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart = COLOR_RED, DWORD colorEnd = COLOR_RED, int indentAxis = -1, DWORD c1 = COLOR_YELLOW, DWORD c2 = COLOR_YELLOW);
};



//unused for now
int Game_Main_DX7();
int Game_Main_DX12();
