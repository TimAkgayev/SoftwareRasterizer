#include "UserInterface.h"
#pragma once 

namespace AI
{

	void SetRasterizerObject(SOFTWARERASTERIZER_DX10_OBJECTS* obj);
	void Init(RECT);
	void MainSoftware(DWORD* video_mem, int lpitch, float deltaTime);
	void MainHardware(float deltaTime);
	void Shutdown();
	UserInterface* GetGUI();
	void myMouseCallBack(int x, int y);
	void Update(float deltaTime);



	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}