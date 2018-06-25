#include "SoftwareRasterizer.h"
#include "UserInterface.h"
#pragma once 

namespace BouncingLines
{


	void Init(RECT);
	void Main(DWORD* video_mem, int lpitch, float deltaTime);
	void Shutdown();
	UserInterface* GetGUI();
	void myMouseCallBack(int x, int y);



	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
