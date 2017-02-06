#include "Vertex.h"


void TransformVertexList(VERTEX2D* vertexbuffer, int numVerts, MATRIX3D* transform)
{

	for (int i = 0; i < numVerts; i++)
	{
		MatrixVectorMultiply(&vertexbuffer[i].pos, transform);
	}

}
