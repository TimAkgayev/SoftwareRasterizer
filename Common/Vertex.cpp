#include "Vertex.h"


void TransformVertexList(VERTEX2D* vertexBuffer, int numVerts, MATRIX3D* transform)
{

	for (int i = 0; i < numVerts; i++)
	{
		MatrixVectorMultiply(&vertexBuffer[i].pos, transform);
	}

}
