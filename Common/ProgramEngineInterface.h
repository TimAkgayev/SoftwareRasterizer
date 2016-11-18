#pragma once
#include "GameEngine.h"

struct WINDOWS_SETTINGS
{
	int clientDimX;
	int clientDimY;
	POINT clientOffset;
	wchar_t * windowTitle;
};

enum {APPUPDATE_NORMAL, APPUPDATE_RESET};

void ApplicationInitialization(RECT);
void ApplicationSoftwareRender(DWORD* video_mem, int lpitch);
void ApplicationHardwareRender();
void ApplicationShutdown();
int  ApplicationUpdate();
void GetSettings(WINDOWS_SETTINGS& settings);
LRESULT CALLBACK ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


