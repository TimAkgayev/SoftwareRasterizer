#include "GameEngine.h"
#include "DebugView.h"
#include "ProgramEngineInterface.h"

WINDOWS_SETTINGS Windows_Settings;

void CreateEngineWindow()
{
	HWND hwnd;
	POINT windowFrameDim = { 16, 39 };

	

	if (!(hwnd = CreateWindowEx(NULL,
		TEXT(WINDOW_CLASS_NAME),     // class
		Windows_Settings.windowTitle, // title
		WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,		//style
		0, 0,	  // initial x,y
		Windows_Settings.clientDimX + Windows_Settings.clientOffset.x + windowFrameDim.x, Windows_Settings.clientDimY + Windows_Settings.clientOffset.y + windowFrameDim.y,  // initial width, height
		NULL,		// handle to parent 
		NULL,		 // handle to menu
		SoftwareRasterizer.hInstance,	// instance of this application
		NULL)))	// extra creation parms
	{
		SoftwareRasterizer.mainWindow = 0;
	}


	// save main window handle
	SoftwareRasterizer.mainWindow = hwnd;

	//save the client rectangle
	GetClientRect(hwnd, &SoftwareRasterizer.clientRect);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpCmdLine, int nCmdShow)
{

	//lock thread to one processor to make the timing work
	DWORD_PTR mask = 1;
	SetThreadAffinityMask(GetCurrentThread(), mask);
	
	//allow the program to set it's window settings
	Windows_Settings = UpdateSettings();

	WNDCLASSEX winclass;
	HWND	   hwnd;
	MSG		   msg;
	HDC        hdc;

	// setup the window class (look and feel of window)
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = ApplicationWindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = NULL;
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = TEXT(WINDOW_CLASS_NAME);
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
		return(0);

	SoftwareRasterizer.hInstance = hinstance; //save instance to global

//entry point when resetting
resetEntry:
	
	CreateEngineWindow();

	Initialization();

	while (TRUE)
	{
		// test if there is a message in queue, if so get it and remove it from the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			if (msg.message == WM_QUIT)
				break;


			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			
			int code = Update();
			if (code == APPUPDATE_RESET)
			{
				//allow the program to set it's window settings
				Windows_Settings = UpdateSettings();
				Shutdown();
				goto resetEntry;
			}
		}

	}

	Shutdown();

	return msg.wParam;

}


