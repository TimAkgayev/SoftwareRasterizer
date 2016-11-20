#pragma once

#include "SoftwareRasterizer.h"
#include "Shapes.h"
#include "UserInterface.h"
#include "AnimatedBitmap.h"
#include "Camera.h"
#include "Timer.h"
#include "HardwareSurface.h"
#include "Grid.h"


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