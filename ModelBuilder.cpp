#include "ModelBuilder.h"

namespace ModelBuilder
{

	UserInterface ModelBuilderUserInterface;

	VERTEX2D tri[3];
	MATRIX3D MTransform;
	MATRIX3D Mtrans;
	MATRIX3D Mscale;
	MESHOBJECT triOBJ;
	MATRIX3D STransform;
	LARGE_INTEGER frequency, startTime, endTime;
	vector<VERTEX2D> ModelList;

	UserInterface* GetGUI()
	{
		return &ModelBuilderUserInterface;
	}

	void ModelBuilderMouseDownCallback(int x, int y)
	{
		ModelList.push_back(VERTEX2D(VECTOR2D(x, y), COLOR_RED, 0, 0));
	}



	void Init()
	{
		QueryPerformanceFrequency(&frequency);

//		ModelBuilderUserInterface.LoadRosources();



		MatrixIdentity(&MTransform);
		MatrixIdentity(&STransform);
		MatrixTranslation(&Mtrans, 500, 350);
		MatrixScale(&Mscale, 300, 300);
		//combine transforms

		//MatrixMatrixMultiply(&MTransform, &Mtrans, &Mscale);


		VERTEX2D v[3];
		v[0].pos = VECTOR2D(0, 1);
		v[0].color = COLOR_RED;

		v[1].pos = VECTOR2D(-1, -1);
		v[1].color = COLOR_GREEN;

		v[2].pos = VECTOR2D(1, -1);
		v[2].color = COLOR_BLUE;

		TransformVertexBuffer(v, 3, &Mscale);
		TransformVertexBuffer(v, 3, &Mtrans);

		triOBJ.vertexBuffer.push_back(v[0]);
		triOBJ.vertexBuffer.push_back(v[1]);
		triOBJ.vertexBuffer.push_back(v[2]);

		triOBJ.indexBuffer.push_back(0);
		triOBJ.indexBuffer.push_back(1);
		triOBJ.indexBuffer.push_back(2);

		triOBJ.worldTransform = STransform;




	}


	void Main(DWORD* mem, int lpitch32, float timeDelta)
	{

		LARGE_INTEGER startTime;
		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&startTime);

		//DrawMeshObject(triOBJ, DO_FLAT, mem, lpitch32);
		vector<VERTEX2D>::iterator vIter = ModelList.begin();
		for (vIter; vIter < ModelList.end(); vIter++)
		{

		}

		QueryPerformanceCounter(&endTime);

		//drawing the model list



		//counts per millisecond
		double timerFreq = double(frequency.QuadPart) / 1000.0;

		double deltaTime = double((endTime.QuadPart - startTime.QuadPart) / timerFreq);

		DrawLineParametricClipped2D(mem, lpitch32, 100, 100, 200, 200, COLOR_RED, COLOR_BLUE);

		int offset = ModelBuilderUserInterface.displayString("DrawMeshObject latency: ", 10, 10, mem, lpitch32);
		ModelBuilderUserInterface.displayString(deltaTime, offset, 10, mem, lpitch32);
	}
	void Shutdown()
	{
	}

}