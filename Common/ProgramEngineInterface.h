#pragma once

struct WINDOWS_SETTINGS
{
	int clientDimX;
	int clientDimY;
	POINT clientOffset;
	wchar_t * windowTitle;
};

enum {APPUPDATE_NORMAL, APPUPDATE_RESET};

void ApplicationInitialization();
void ApplicationSoftwareRender(DWORD* video_mem, int lpitch);
void ApplicationHardwareRender();
void ApplicationShutdown();
int  ApplicationUpdate();

//allows program to set intial settings, this is called even before the main initialization
WINDOWS_SETTINGS& UpdateSettings(); 

LRESULT CALLBACK ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

