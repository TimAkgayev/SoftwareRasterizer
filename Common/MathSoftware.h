#pragma once
#include "MathStructures.h"

void MatrixIdentity(MATRIX3D* mat);
void MatrixIdentity(MATRIX4D* mat);
void MatrixScale(MATRIX4D* mat, float x, float y, float z);
void MatrixScale(MATRIX3D* mat, float x, float y);
void MatrixTranslation(MATRIX4D* mat, float x, float y, float z);
void MatrixTranslation(MATRIX3D* mat, float x, float y);

void Vector2Rotation(VECTOR2D* v, float rad);
void MatrixRotation(MATRIX4D* mat, VECTOR3D axis, float rad);
void MatrixScalarMultiply(MATRIX2D* out, MATRIX2D& in, float scalar);
void MatrixMatrixMultiply(MATRIX4D* out, MATRIX4D* op1, MATRIX4D* op2);
void MatrixMatrixMultiply(MATRIX3D* out, MATRIX3D* op1, MATRIX3D* op2);
int MatrixDeterminant(MATRIX2D& in);
void MatrixInverse(MATRIX2D* out, MATRIX2D& in);

void MatrixVectorMultiply(VECTOR2D* operand, MATRIX3x3_PTR transform);
void MatrixVectorMultiply(VECTOR3D* operand, MATRIX4x4_PTR transform);
void Vector3CrossProduct(VECTOR3D* out, VECTOR3D in, VECTOR3D in2);


int CohenSutherlandLineClip(int& x1, int& y1, int& x2, int& y2, RECT clipRect);

float GetDistanceToLine(VECTOR2D Lstart, VECTOR2D Lend, VECTOR2D point);

MATRIX1x3 VectorToMatrix(VECTOR2D* v);