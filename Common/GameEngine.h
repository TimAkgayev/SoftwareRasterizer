#pragma once

#include "SoftwareRasterizer.h"
#include "Shapes.h"
#include "UserInterface.h"
#include "AnimatedBitmap.h"
#include "Camera.h"
#include "Timer.h"
#include "HardwareSurface.h"
#include "Grid.h"
#include "DebugView.h"
#include "ProgramEngineInterface.h"


extern SOFTWARERASTERIZER_DX10_OBJECTS SoftwareRasterizer;


int Initialization();
int Update();
int Shutdown();



class Player
{
public:
	Player();
	~Player();

	void AddFrontAnimation(AnimatedBitmap anim);
	void AddLeftAnimation(AnimatedBitmap anim);
	void AddRightAnimation(AnimatedBitmap anim);
	void AddBackAnimation(AnimatedBitmap anim);

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
	AnimatedBitmap frontAnim, leftAnim, rightAnim, backAnim;
	bool isPositionUpdated;

};
class World
{
public:
	World();

	UserInterface* userInterface;
	Player* player;
	vector<AnimatedBitmap> collisionObjects;

	void SoftwareDraw(DWORD*, int);
	void HardwareDraw(double);
};




//unused for now
extern SOFTWARERASTERIZER_DX7_OBJECTS softrest7_obj;
extern SOFTWARERASTERIZER_DX12_OBJECTS softrest12_obj;
int Game_Main_DX7();
int Game_Main_DX12();
