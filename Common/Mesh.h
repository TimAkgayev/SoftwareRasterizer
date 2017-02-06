#pragma once
#include <vector>
#include "Vertex.h"
#include "ResourceManager.h"

using namespace std;

typedef struct MESHOBJECT_TYP
{
	MESHOBJECT_TYP()
	{
		
	}

	MATRIX3D     worldTransform;
	vector<VERTEX2D> vertexBuffer;
	vector<int>      indexBuffer;
	BitmapFile*   texture;


}MESHOBJECT, *MESHOBJECT_PTR;
