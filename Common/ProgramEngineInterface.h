#pragma once


extern struct WINDOWS_SETTINGS
{
	int clientDimX;
	int clientDimY;
	POINT clientOffset;
	wchar_t * windowTitle;
} Windows_Settings;

enum {APPUPDATE_NORMAL, APPUPDATE_RESET};

void ApplicationInitialization(RECT);
void ApplicationSoftwareRender(DWORD* video_mem, int lpitch);
void ApplicationHardwareRender();
void ApplicationShutdown();
int  ApplicationUpdate();

//allows program to set intial settings, this is called even before the main initialization
void UpdateSettings(); 

LRESULT CALLBACK ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void AttachDebugger(DebugView*);
