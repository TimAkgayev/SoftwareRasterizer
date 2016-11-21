#pragma once 
#include <windows.h>

#include <math.h>
#include "MacrosAndDefines.h"
//#include "D3DX10.h"

const float MATH_PI = 3.1415926f;



//basic 2D point, core of all 2D geometry
class VECTOR2D
{
public:

	VECTOR2D(float inX, float inY): x(inX), y(inY) {}
	VECTOR2D() {}

	//convinent operators overloaded
	VECTOR2D operator+ (const VECTOR2D& rhs)
	{
		return VECTOR2D(x + rhs.x, y + rhs.y);
	}

	VECTOR2D operator- (const VECTOR2D& rhs)
	{
		return VECTOR2D(x - rhs.x, y - rhs.y);
	}

	VECTOR2D operator* (float scalar)
	{
		return VECTOR2D(x * scalar, y * scalar);
	}


	bool operator == (const VECTOR2D& rhs)
	{
		if(rhs.x == x && rhs.y == y)
			return true;

		return false;
	}

	void Normalize()
	{
		//formula: || v || = v / |v|

		//avoid a Length() call overhead, just calculate on the spot
		float len = sqrt(x*x + y*y);
		
		if(len == 0)
			return;

		x = x/len;
		y = y/len;

	}

	void MakeNegative()
	{
		x = -x;
		y = -y;
	}

	float Length()
	{
		// x^2 + y^2 = r^2, r = sqrt(x^2 + y^2)
		return sqrt(x*x + y*y);
	}

	float DistanceTo(VECTOR2D v)
	{
		VECTOR2D dir;
		dir = v - (*this);

		return dir.Length();
	}
	
	float AngleToVector(VECTOR2D& v)
	{
		return MATH_RADTODEG(atan( (x*v.x + y*v.y)/(Length()*v.Length()) ));
	}

	float AngleToWorld()
	{
		
		float radANG;
	
		if(x == 0 && y == 0)
			return 0;

		if(x == 0 && y == -1)
			return -90;
		if(x == 0 && y == 1)
		{
			return 90;
		}
		if(x == 1 && y == 0)
		{
			return 0;
		}
		if(x == -1 && y == 0)
		{
			return -1;
		}

		if(x > 0 && y < 0)
		{
				return radANG =  MATH_RADTODEG(atan(y/x));
		}
		else if(x < 0 && y < 0)
		{
			float a = -atan(x/y);
			a -= MATH_PI/2;
			return MATH_RADTODEG(a);
		}
		else if(x > 0 && y > 0)
		{
			float a = atan(y/x);
			return MATH_RADTODEG(a);
		}
		else if( x < 0 && y > 0)
		{
			float a = atan(y/x);
			a += MATH_PI/2;
			return MATH_RADTODEG(a);
		}
		
	}

	float x;
	float y;

	static VECTOR2D DOWN_VECTOR();
	static VECTOR2D UP_VECTOR();
	static VECTOR2D LEFT_VECTOR();
	static VECTOR2D RIGHT_VECTOR();


};



class VECTOR3D
{
public:
	VECTOR3D(double inX, double inY, double inZ): x(inX), y(inY), z(inZ) {}
	VECTOR3D() {}

		//convinent operators overloaded
	VECTOR3D operator+ (const VECTOR3D& rhs)
	{
		return VECTOR3D(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	VECTOR3D operator- (const VECTOR3D& rhs)
	{
		return VECTOR3D(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	VECTOR3D operator* (float scalar)
	{
		return VECTOR3D(x * scalar, y * scalar, z * scalar);
	}

	bool operator == (const VECTOR3D& rhs)
	{
		if(rhs.x == x && rhs.y == y && rhs.z == z)
			return true;

		return false;
	}

	void Normalize()
	{
		//formula: || v || = v / |v|

		//avoid a Length() call overhead, just calculate on the spot
		float len = sqrt(x*x + y*y + z*z);
		
		if(len == 0)
			return;

		x = x/len;
		y = y/len;
		z = z/len;

	}

	float Length()
	{
		// x^2 + y^2 = r^2, r = sqrt(x^2 + y^2)
		return sqrt(x*x + y*y + z*z);
	}

	float DistanceTo(VECTOR3D v)
	{
		VECTOR3D dir;
		dir = v - (*this);

		return dir.Length();
	}

	double x;
	double y;
	double z;
};

class VECTOR4D
{
public:
	VECTOR4D(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
	VECTOR4D() {}

	//convinent operators overloaded
	VECTOR4D operator+ (const VECTOR4D& rhs)
	{
		return VECTOR4D(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	VECTOR4D operator- (const VECTOR4D& rhs)
	{
		return VECTOR4D(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	VECTOR4D operator* (float scalar)
	{
		return VECTOR4D(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	bool operator == (const VECTOR4D& rhs)
	{
		if (rhs.x == x && rhs.y == y && rhs.z == z && rhs.w == w)
			return true;

		return false;
	}


	void Normalize()
	{
		//formula: || v || = v / |v|

		//avoid a Length() call overhead, just calculate on the spot
		float len = sqrt(x*x + y*y + z*z + w*w);

		if (len == 0)
			return;

		x = x / len;
		y = y / len;
		z = z / len;
		w = w / len;

	}

	float Length()
	{
		// x^2 + y^2 = r^2, r = sqrt(x^2 + y^2)
		return sqrt(x*x + y*y + z*z + w*w);
	}

	float x, y, z, w;

};
typedef struct MATRIX1x3_TYP
{

	union
	{
		double M[3]; //array index

		struct
		{
			double _00, _01, _02;
		};
	};

}MATRIX1x3, *MATRIX1x3_PTR;


typedef struct MATRIX2x2_TYP
{
	union
	{
		float M[2][2]; //array index

		struct
		{
			float _00, _01;
			float _10, _11;
		};
	};

}MATRIX2x2, MATRIX2D, *MATRIX2x2_PTR, *MATRIX2D_PTR;

typedef struct MATRIX3x3_TYP
{
	union
	{
		float M[3][3]; //array index

		struct
		{
			float _00, _01, _02;
			float _10, _11, _12;
			float _20, _21, _22;
		};
	};

}MATRIX3x3, MATRIX3D, *MATRIX3x3_PTR, *MATRIX3D_PTR;


typedef struct MATRIX4x4_TYP
{
	/*
	MATRIX4x4_TYP(D3DXMATRIX& m)
	{
		_00 = m._11;
		_01 = m._12;
		_02 = m._13;
		_03 = m._14;

		_10 = m._21;
		_11 = m._22;
		_12 = m._23;
		_13 = m._24;

		_20 = m._31;
		_21 = m._32;
		_22 = m._33;
		_23 = m._34;

		_30 = m._41;
		_31 = m._42;
		_32 = m._43;
		_33 = m._44;

	}

	MATRIX4x4_TYP()
	{

	}

	D3DXMATRIX toDirecXMatrix()
	{
		D3DXMATRIX m;
		m._11 = _00;
		m._12 = _01;
		m._13 = _02;
		m._14 = _03;

		m._21 = _10;
		m._22 = _11;
		m._23 = _12;
		m._24 = _13;

		m._31 = _20;
		m._32 = _21;
		m._33 = _22;
		m._34 = _23;

		m._41 = _30;
		m._42 = _31;
		m._43 = _32;
		m._44 = _33;
	
		return m;
	}
	*/
	union
	{
		double M[4][4]; //array index

		struct
		{
			double _00, _01, _02, _03;
			double _10, _11, _12, _13;
			double _20, _21, _22, _23;
			double _30, _31, _32, _33;
		};
	};

}MATRIX4x4, MATRIX4D, *MATRIX4x4_PTR, *MATRIX4_PTR;

class RECT2D
{
public:

	RECT2D() :left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
	RECT2D(float x, float y, float x1, float y1) : left(x), top(y), right(x1), bottom(y1) {}
	RECT2D(RECT& r){
		left = r.left;
		right = r.right;
		top = r.top;
		bottom = r.bottom;
	}
	void setWidth(float w)
	{
		right = left + w;

	}

	void setHeight(float h)
	{
		bottom = top + h;
	}

	void setPos(float x, float y)
	{
		right = (right - left) + x;
		bottom = (bottom - top) + y;
		left = x;
		top = y;
		
	}

	void SetRECT(RECT r)
	{
		left = r.left;
		top = r.top;
		right = r.right;
		bottom = r.bottom;
	}

	void setPos(VECTOR2D& v)
	{
		right = (right - left) + v.x;
		bottom = (bottom - top) + v.y;
		left = v.x;
		top = v.y;
	}

	float getWidth() const
	{
		return right - left;
	}

	float getHeight() const
	{
		return bottom - top;
	}

	void move(VECTOR2D dir, int pxPerSec, float deltaTime)
	{
		left = left + pxPerSec*dir.x * deltaTime;
		right = right + pxPerSec*dir.x * deltaTime;
		top = top +pxPerSec*dir.y * deltaTime;
		bottom = bottom + pxPerSec*dir.y * deltaTime;
	}

	VECTOR2D GetPosition() const
	{
		VECTOR2D v;
		v.x = left;
		v.y = top;

		return v;
	}

	inline bool isPointInRect(float x, float y)
	{
		//reverse signs if the region is negative (made from right to left)
		if (left > right)
		{
			if (x <= left && x >= right)
				if (y >= top && y <= bottom)
					return true;
		}

		if (x >= left && x <=right)
			if (y >= top && y <= bottom)
				return true;

		return false;
	}

	bool isRectInRect(RECT& region)
	{
		//test x dimension
		if (region.left <= left)
		{
			
			if (region.right > left)
			{
				//now check y dimension
				if (region.top <= top)
				{
					if (region.bottom > top)
						return true;
					else return false;
				}
				else if (region.top > top)
				{
					if (region.top < bottom)
						return true;
					else
						return false;
				}
			}
			else
				return false;
				
		}
		else if (region.left > left)
		{
			if (region.left < right)
			{
				//now check y dimension
				if (region.top <= top)
				{
					if (region.bottom > top)
						return true;
					else return false;
				}
				else if (region.top > top)
				{
					if (region.top < bottom)
						return true;
					else
						return false;
				}
			}
			else
				return false;
		}

		return false;
	}

	RECT GetWINRECT()
	{
		RECT r;
		r.left = int(left + 0.5f);
		r.right = int(right + 0.5f);
		r.top = int(top + 0.5f);
		r.bottom = int(bottom + 0.5f);

		return r;
	}

public:
	float left;
	float top;
	float right;
	float bottom;

};