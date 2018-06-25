#pragma once

#include <Windows.h>
#include <vector>
using namespace std;

#include "MathStructures.h"
#include "MacrosAndDefines.h"
#include "UserInterface.h"
#include "WindowsInterface.h"
#include "Camera.h"


#define WATER_WIDTH 200
#define WATER_HEIGHT 200



struct flake
{
	float xpos;
	float ypos;
	DWORD color;

	VECTOR2D dir;
	int screenX;
	int screenY;
};




void OceanInit();
void OceanMain(DWORD* mem_buffer, int lpitch32, double timeDelta);
void OceanShutdown();
void drop(int x, int y, int size, int strength, int w, int h);
Camera2D* GetCamera();
UserInterface* GetUI();

void SnowGameInit();

void SnowGameMain(DWORD* mem_buffer, int lpitch32, double timeDelta);

void SnowGameShutdown();