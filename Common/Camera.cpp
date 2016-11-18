#include "Camera.h"


Camera2D::Camera2D(int width, int height)
{
	this->width = width;
	this->height = height;
	worldPos = VECTOR2D(0, 0);
	camSpeed = 1;
}
Camera2D::~Camera2D()
{

}

void Camera2D::SetPosition(VECTOR2D& pos)
{
	if (movementConstraint == MOVE_CONSTRAINT_NONE)
		worldPos = pos;
	else if (movementConstraint == MOVE_CONSTRAINT_X)
		worldPos.y = pos.y;
	else if (movementConstraint == MOVE_CONSTRAINT_Y)
		worldPos.x = pos.x;
	else if (movementConstraint == MOVE_CONSTRAINT_ALL)
		return;

}
void Camera2D::SetPosition(int x, int y)
{
	SetPosition(VECTOR2D(x, y));
}

VECTOR2D Camera2D::GetPosition() const
{
	return worldPos;
}

int Camera2D::GetWidth() const
{
	return width;
}

int Camera2D::GetHeight() const
{
	return height;
}

//speed has to be a power of magnification strength otherwise artifacts will show up
void Camera2D::SetMoveSpeed(float speed)
{
	camSpeed = speed;
}

float Camera2D::GetMoveSpeed() const { return camSpeed; }


void Camera2D::SetMovementConstraint(int constraint)
{
	movementConstraint = constraint;
}