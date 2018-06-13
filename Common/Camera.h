#pragma once
#include <Windows.h>
#include "Math.h"
#include "Math.h"

enum {MOVE_CONSTRAINT_NONE, MOVE_CONSTRAINT_ALL, MOVE_CONSTRAINT_X, MOVE_CONSTRAINT_Y};

class Camera2D
{
public:
	Camera2D(int width, int height);
	~Camera2D();
	

	void SetPosition(VECTOR2D& pos);
	void SetPosition(int x, int y);
	VECTOR2D GetPosition() const;
	int GetWidth() const;
	int GetHeight() const;
	//speed has to be a power of magnification strength otherwise artifacts will show up
	void SetMoveSpeed(float speed);
	float GetMoveSpeed() const;
	void SetMovementConstraint(int);

private:
	
	//this is the rectangle of what the camera can see
	VECTOR2D worldPos;
	int width;
	int height;
	float camSpeed;
	int movementConstraint;

};