#include "MathStructures.h"

VECTOR2D VECTOR2D::UP_VECTOR()
{
	
	VECTOR2D v;
	v.x = 0.0f;
	v.y = 1.0f;
	return v;
	
}

VECTOR2D VECTOR2D::LEFT_VECTOR()
{
	VECTOR2D v;
	v.x = -1.0f;
	v.y = 0.0f;
	return v;
}

VECTOR2D VECTOR2D::RIGHT_VECTOR()
{
	VECTOR2D v;
	v.x = 1.0f;
	v.y = 0.0f;
	return v;
}

VECTOR2D VECTOR2D::DOWN_VECTOR()
{
	VECTOR2D v;
	v.x = 0.0f;
	v.y = -1.0f;
	return v;
}