#pragma once
#include <vector>
#include "Vertex.h"
#include "Bitmap.h"

using namespace std;

typedef struct MESHOBJECT_TYP
{
	MESHOBJECT_TYP()
	{
		texture.data = NULL;
	}

	MATRIX3D     worldTransform;
	vector<VERTEX2D> vertexBuffer;
	vector<int>      indexBuffer;
	BITMAP_FILE   texture;


}MESHOBJECT, *MESHOBJECT_PTR;
