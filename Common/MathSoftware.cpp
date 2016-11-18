#include "MathSoftware.h"

void MatrixZero(MATRIX3x3_PTR out)
{
	memset(out, 0, sizeof(MATRIX3x3));
}

void MatrixZero(MATRIX4x4_PTR out)
{
	memset(out, 0, sizeof(MATRIX4D));
}

void MatrixIdentity(MATRIX3D* mat)
{
	mat->_00 = 1.0f;
	mat->_01 = 0.0f;
	mat->_02 = 0.0f;

	mat->_10 = 0.0f;
	mat->_11 = 1.0f;
	mat->_12 = 0.0f;

	mat->_20 = 0.0f;
	mat->_21 = 0.0f;
	mat->_22 = 1.0f;


}

void MatrixScale(MATRIX3D* mat, float x, float y)
{
	MatrixIdentity(mat);
	mat->_00 = x;
	mat->_11 = y;
}


void MatrixIdentity(MATRIX4x4_PTR out)
{
	out->_00 = 1.0f;
	out->_01 = 0.0f;
	out->_02 = 0.0f;
	out->_03 = 0.0f;

	out->_10 = 0.0f;
	out->_11 = 1.0f;
	out->_12 = 0.0f;
	out->_13 = 0.0f;

	out->_20 = 0.0f;
	out->_21 = 0.0f;
	out->_22 = 1.0f;
	out->_23 = 0.0f;

	out->_30 = 0.0f;
	out->_31 = 0.0f;
	out->_32 = 0.0f;
	out->_33 = 1.0f;


}

int MatrixDeterminant(MATRIX2D& in)
{
	int ad = in._00*in._11;
	int bc = in._01*in._10;
	return ad - bc;
}

void MatrixInverse(MATRIX2D* out, MATRIX2D& in)
{
	//if determinant is zero, matrix has no inverse
	if (MatrixDeterminant(in) == 0)
		return;

	//rearrange the matrix, and negate some terms
	MATRIX2D temp;
	temp._00 = in._11;
	temp._01 = -in._01;
	temp._10 = -in._10;
	temp._11 = in._00;

	//inverse formula is 1/Det(A) * Rearranged A
	float invDet = 1.0f / MatrixDeterminant(in);
	MatrixScalarMultiply(out, temp, invDet);
}

void MatrixTranslation(MATRIX4x4_PTR out, float x, float y, float z)
{
	MatrixIdentity(out);
	out->_03 = x;
	out->_13 = y;
	out->_23 = z;

}

void MatrixTranslation(MATRIX3D* out, float x, float y)
{
	MatrixIdentity(out);
	out->_20 = x;
	out->_21 = y;
}

void Vector2Rotation(VECTOR2D* v, float rad)
{
	if (v->x == 0 && v->y == 0)
		return;

	// matrix rotation
	v->x = v->x*cos(rad) - v->y*sin(rad);
	v->y = v->x*sin(rad) + v->y*cos(rad);
}

void MatrixRotation(MATRIX4x4_PTR out, VECTOR3D axis, float rad)
{
	MatrixIdentity(out);
	out->_00 = cos(rad);
	out->_01 = sin(rad);

	out->_10 = -sin(rad);
	out->_11 = cos(rad);
	
}
void MatrixScale(MATRIX4x4_PTR out, float scaleX, float scaleY, float scaleZ)
{
	MatrixIdentity(out);
	out->_00 = scaleX;
	out->_11 = scaleY;
}

void MatrixScalarMultiply(MATRIX2D* out, MATRIX2D& in, float scalar)
{
	out->_00 = in._00*scalar;
	out->_01 = in._01*scalar;
	out->_10 = in._10*scalar;
	out->_11 = in._11*scalar;
}

void MatrixMatrixMultiply(MATRIX1x3_PTR operand, MATRIX3x3_PTR transform)
{
	MATRIX1x3 final;
		
	final._00 = ( operand->_00 * transform->_00 ) + ( operand->_01 * transform->_10 ) + (operand->_02 * transform->_20);
	final._01 = ( operand->_00 * transform->_01 ) + ( operand->_01 * transform->_11 ) + (operand->_02 * transform->_21);
	final._02 = ( operand->_00 * transform->_02 ) + ( operand->_01 * transform->_12 ) + (operand->_02 * transform->_22);
	
	(*operand) = final;
}


void MatrixMatrixMultiply(MATRIX4D* out, MATRIX4D* op1, MATRIX4D* op2)
{
	MATRIX4D final;
	MatrixZero(&final);
	
	
	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			for(int inner = 0; inner < 4; inner++)
			{
				
				final.M[row][col] += op1->M[row][inner] * op2->M[inner][col];
				
			}


		}
	}
	
	(*out) = final;
}
void MatrixVectorMultiply(VECTOR3D* operand, MATRIX4x4_PTR transform)
{
	MATRIX1x3 final;
	
	final._00 = ( operand->x * transform->_00 ) + ( operand->y * transform->_10 ) + (operand->z * transform->_20);
	final._01 = ( operand->x * transform->_01 ) + ( operand->y * transform->_11 ) + (operand->z * transform->_21);
	final._02 = ( operand->x * transform->_02 ) + ( operand->y * transform->_12 ) + (operand->z * transform->_22);
	
	operand->x = final._00 + transform->_03;
	operand->y = final._01 + transform->_13;
	operand->z = final._02 + transform->_23;
}

void MatrixVectorMultiply(VECTOR2D* operand, MATRIX3x3_PTR transform)
{
	//project vector into 3D space
	MATRIX1x3 vect3D;
	vect3D._00 = operand->x;
	vect3D._01 = operand->y;
	vect3D._02 = 1.0f;

	//matrix multiplication
	VECTOR2D out;
	out.x = vect3D._00 * transform->_00 + vect3D._01 * transform->_10 + vect3D._02 * transform->_20;
	out.y = vect3D._00 * transform->_01 + vect3D._01 * transform->_11 + vect3D._02 * transform->_21;

	*operand = out;
}

void MatrixMatrixMultiply(MATRIX3D* out, MATRIX3D* op1, MATRIX3D* op2)
{
	MATRIX3D final;
	MatrixZero(&final);


	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			for (int inner = 0; inner < 3; inner++)
			{
				final.M[row][col] += op1->M[row][inner] * op2->M[inner][col];
			}


		}
	}

	(*out) = final;
}



void MatrixRotation(MATRIX3D* out, VECTOR3D axis, float rad)
{
	
}

void RotateVector2D(VECTOR2D* vector, float angleRad)
{

	float origX = vector->x;
	float origY = vector->y;

	//FIX! use a lookup table for trig functions to improve speed
	float newX = origX * cos(angleRad) - origY * sin(angleRad);
	float newY = origX * sin(angleRad) + origY * cos(angleRad);

	vector->x = newX;
	vector->y = newY;
}


float GetDistanceToLine(VECTOR2D Lstart, VECTOR2D Lend, VECTOR2D point)
{
	float a = point.y - Lend.y;
	float b = point.x - Lstart.x;
	float c = Lend.x * point.y - point.x * Lend.y;

	return abs(long(a * Lstart.x + b * Lstart.y + c)) / sqrt(a * a + b * b);
}


MATRIX1x3 VectorToMatrix(VECTOR2D* v)
{
	MATRIX1x3 m;
	
	m._00  =  v->x;
	m._01  =  v->y;
	m._02  =  0.0f;

	return m;
	
}


int CohenSutherlandLineClip(int& x1, int& y1, int& x2, int& y2, RECT clipRect)
{
#define CLIP_CODE_CENTER	0x0000
#define CLIP_CODE_NORTH		0x0008
#define CLIP_CODE_SOUTH		0x0004
#define CLIP_CODE_EAST		0x0002
#define CLIP_CODE_WEST		0x0001
#define CLIP_CODE_NE		0x000A
#define CLIP_CODE_SE		0x0006
#define CLIP_CODE_NW		0x0009
#define CLIP_CODE_SW		0x0005

	//working values
	int p1X = x1;
	int p1Y = y1;
	int p2X = x2;
	int p2Y = y2;

	//codes, assume center region
	int p1_code = CLIP_CODE_CENTER, p2_code = CLIP_CODE_CENTER;

	//determine intersection regions
	//point 1
	if (p1X < clipRect.left)
		p1_code = p1_code | CLIP_CODE_WEST;
	else if (p1X > clipRect.right)
		p1_code = p1_code | CLIP_CODE_EAST;

	if (p1Y < clipRect.top)
		p1_code = p1_code | CLIP_CODE_NORTH;
	else if (p1Y > clipRect.bottom)
		p1_code = p1_code | CLIP_CODE_SOUTH;

	//point 2
	if (p2X < clipRect.left)
		p2_code = p2_code | CLIP_CODE_WEST;
	else if (p2X > clipRect.right)
		p2_code = p2_code | CLIP_CODE_EAST;

	if (p2Y < clipRect.top)
		p2_code = p2_code | CLIP_CODE_NORTH;
	else if (p2Y > clipRect.bottom)
		p2_code = p2_code | CLIP_CODE_SOUTH;


	//totally visible
	if (p1_code == 0 && p2_code == 0)
		return 1;


	//totally invisible
	if (p1_code == p2_code) //same region
		return 0;



	//calculate new coordinates
	switch (p1_code)
	{
	case CLIP_CODE_CENTER: break;

	case CLIP_CODE_NORTH:
	{
		if (y2 - y1 == 0)
			return 0;
		p1X = x1 + 0.5 + (clipRect.top - y1) * (x2 - x1) / (y2 - y1);
		p1Y = clipRect.top;
	}break;

	case CLIP_CODE_SOUTH:
	{

		if (y2 - y1 == 0)
			return 0;
		p1X = x1 + 0.5 + (clipRect.bottom - y1) * (x2 - x1) / (y2 - y1);
		p1Y = clipRect.bottom;
	}break;

	case CLIP_CODE_EAST:
	{
		if (x2 - x1 == 0)
			return 0;
		p1X = clipRect.right;
		p1Y = y1 + 0.5 + (clipRect.right - x1) * (y2 - y1) / (x2 - x1);
	}break;


	case CLIP_CODE_WEST:
	{
		//find y intersection with left vertical line
		if (x2 - x1 == 0)
			return 0;
		p1X = clipRect.left;
		p1Y = y1 + 0.5 + (clipRect.left - x1) * (y2 - y1) / (x2 - x1);
	}break;

	case CLIP_CODE_NE:
	{
		if (y2 - y1 == 0)
			return 0;
		p1X = x1 + 0.5 + (clipRect.top - y1) * (x2 - x1) / (y2 - y1);
		p1Y = clipRect.top;

		if (p1X < clipRect.left || p1X > clipRect.right)
		{
			if (x2 - x1 == 0)
				return 0;
			//east vline intersection
			p1X = clipRect.right;
			p1Y = y1 + 0.5 + (clipRect.right - x1) * (y2 - y1) / (x2 - x1);
		}

	}break;

	case CLIP_CODE_SE:
	{
		if (y2 - y1 == 0)
			return 0;
		p1X = x1 + 0.5 + (clipRect.bottom - y1) * (x2 - x1) / (y2 - y1);
		p1Y = clipRect.bottom;

		if (p1X < clipRect.left || p1X > clipRect.right)
		{
			if (x2 - x1 == 0)
				return 0;
			p1X = clipRect.left;
			p1Y = y1 + 0.5 + (clipRect.right - x1) * (y2 - y1) / (x2 - x1);

		}
	}break;
	case CLIP_CODE_NW:
	{
		if (y2 - y1 == 0)
			return 0;
		p1X = x1 + 0.5 + (clipRect.top - y1) * (x2 - x1) / (y2 - y1);

		if (p1X < clipRect.left || p1X > clipRect.right)
		{
			if (x2 - x1 == 0)
				return 0;
			p1X = clipRect.left;
			p1Y = y1 + 0.5 + (clipRect.left - x1) * (y2 - y1) / (x2 - x1);

		}
	}break;

	case CLIP_CODE_SW:
	{
		if (y2 - y1 == 0)
			return 0;
		p1X = x1 + 0.5 + (clipRect.bottom - y1) * (x2 - x1) / (y2 - y1);
		p1Y = clipRect.bottom;

		if (p1X < clipRect.left || p1X > clipRect.right)
		{
			if (x2 - x1 == 0)
				return 0;
			p1X = clipRect.left;
			p1Y = y1 + 0.5 + (clipRect.left - x1) * (y2 - y1) / (x2 - x1);

		}
	}break;

	default: break;

	}

	switch (p2_code)
	{
	case CLIP_CODE_CENTER: break;

	case CLIP_CODE_NORTH:
	{
		if (y1 - y2 == 0)
			return 0;
		p2X = x2 + (clipRect.top - y2) * (x1 - x2) / (y1 - y2);
		p2Y = clipRect.top;

	}break;

	case CLIP_CODE_SOUTH:
	{
		if (y1 - y2 == 0)
			return 0;
		p2X = x2 + (clipRect.bottom - y2) * (x1 - x2) / (y1 - y2);
		p2Y = clipRect.bottom;
	}break;

	case CLIP_CODE_WEST:
	{
		if (x1 - x2 == 0)
			return 0;
		p2X = clipRect.left;
		p2Y = y2 + (clipRect.left - x2) * (y1 - y2) / (x1 - x2);

	}break;

	case CLIP_CODE_EAST:
	{
		if (x1 - x2 == 0)
			return 0;
		p2X = clipRect.right;
		p2Y = y2 + (clipRect.right - x2) * (y1 - y2) / (x1 - x2);
	}break;

	case CLIP_CODE_NE:
	{
		if (y1 - y2 == 0)
			return 0;
		p2X = x2 + 0.5 + (clipRect.top - y2) * (x1 - x2) / (y1 - y2);
		p2Y = clipRect.top;

		if (p2X < clipRect.left || p2X > clipRect.right)
		{
			if (x1 - x2 == 0)
				return 0;
			p2X = clipRect.right;
			p2Y = y2 + 0.5 + (clipRect.right - x2) * (y1 - y2) / (x1 - x2);
		}
	}break;

	case CLIP_CODE_SE:
	{
		if (y1 - y2 == 0)
			return 0;
		p2X = x2 + 0.5 + (clipRect.bottom - y2) * (x1 - x2) / (y1 - y2);
		p2Y = clipRect.bottom;

		if (p2X < clipRect.left || p2X > clipRect.right)
		{
			if (x1 - x2 == 0)
				return 0;
			p2X = clipRect.right;
			p2Y = y2 + 0.5 + (clipRect.right - x2) * (y1 - y2) / (x1 - x2);

		}
	}break;

	case CLIP_CODE_NW:
	{
		if (y1 - y2 == 0)
			return 0;
		p2X = x2 + 0.5 + (clipRect.top - y2) * (x1 - x2) / (y1 - y2);
		p2Y = clipRect.top;

		if (p2X < clipRect.left || p2X > clipRect.right)
		{
			if (x1 - x2 == 0)
				return 0;
			p2X = clipRect.left;
			p2Y = y2 + 0.5 + (clipRect.left - x2) * (y1 - y2) / (x1 - x2);

		}
	}break;

	case CLIP_CODE_SW:
	{
		if (y1 - y2 == 0)
			return 0;
		p2X = x2 + 0.5 + (clipRect.bottom - y2) * (x1 - x2) / (y1 - y2);
		p2Y = clipRect.bottom;

		if (p2X < clipRect.left || p2X > clipRect.right)
		{
			if (x1 - x2 == 0)
				return 0;
			p2X = clipRect.left;
			p2Y = y2 + 0.5 + (clipRect.left - x2) * (y1 - y2) / (x1 - x2);


		}
	}break;

	default: break;


	}//end switch

	 //do bounds checking
	if (p1X < clipRect.left || p1X > clipRect.right ||
		p1Y < clipRect.top || p1Y > clipRect.bottom ||
		p2X < clipRect.left || p2X > clipRect.right ||
		p2Y < clipRect.top || p2Y > clipRect.bottom)
	{
		return 0;
	}


	//appy results
	x1 = p1X;
	y1 = p1Y;
	x2 = p2X;
	y2 = p2Y;

	return 1;

}


void Vector3CrossProduct(VECTOR3D* out, VECTOR3D in1, VECTOR3D in2)
{
	VECTOR3D o(in1.y*in2.z - in1.z*in2.y, in1.z*in2.x - in1.x*in2.z, in1.x*in2.y - in1.y*in2.x);
	*out = o;
}