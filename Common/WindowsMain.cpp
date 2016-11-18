#include "stdinclude.h"
#include "ProgramEngineInterface.h"


SOFTWARERASTERIZER_DX7_OBJECTS softrest7_obj;
SOFTWARERASTERIZER_DX10_OBJECTS softrest10_obj;
SOFTWARERASTERIZER_DX12_OBJECTS softrest12_obj;


int Game_Main_DX7()
{

	//clear surface, REWRITE THIS!!!!!!!!!!!!!!!!! 
	//Fill_Surface32(softrest7_obj.lpDDSurfBack, COLOR_BLACK);

	DDRAW_INIT_STRUCT(softrest7_obj.ddSurfDesc);
	softrest7_obj.lpDDSurfBack->Lock(NULL, &softrest7_obj.ddSurfDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	/***************************************all software blitting goes here*******************************************/
	int lpitch32 = (int)(softrest7_obj.ddSurfDesc.lPitch >> 2); //divide by 4 to get number of DWORDS
	DWORD *video_buffer = (DWORD *)softrest7_obj.ddSurfDesc.lpSurface;


	//DrawMandelbrotSet(video_buffer, lpitch32, g_mndBrotCamX, g_mndBrotCamY, g_mndBrotZoom);

	//	SnowGameMain(video_buffer, lpitch32, Time.deltaTime);
	//	OceanMain(video_buffer, lpitch32, Time.deltaTime);
	/***************************************all software blitting goes here*******************************************/

	softrest7_obj.lpDDSurfBack->Unlock(NULL);
	//	softrest7_obj.lpDDSurfPrimary->Blt(&GetCamera()->GetWindowPortRect(softrest7_obj.mainWindow), softrest7_obj.lpDDSurfBack, NULL, DDBLT_WAIT, NULL);

	//	QueryPerformanceCounter(&lastTime);

	return 1;

}
int Game_Main_DX12()
{
	/*
	float deltaTime = 0;
	if (isClockStarted)
	{
	QueryPerformanceCounter(&endTime);
	double finalTime = endTime.QuadPart - startTime.QuadPart;
	deltaTime = finalTime / perfFreq.QuadPart / 1000;
	}

	Update(deltaTime);
	QueryPerformanceCounter(&startTime);
	isClockStarted = true;

	//populate command list

	// Command list allocators can only be reset when the associated
	// command lists have finished execution on the GPU; apps should use
	// fences to determine GPU execution progress.
	softrest12_obj.pD3D12CommandAllocator->Reset();



	// However, when ExecuteCommandList() is called on a particular command
	// list, that command list can then be reset at any time and must be before
	// re-recording.
	softrest12_obj.pD3D12CommandList->Reset(softrest12_obj.pD3D12CommandAllocator.Get(), softrest12_obj.pD3D12PipelineState.Get());

	softrest12_obj.pD3D12CommandList->SetGraphicsRootSignature(softrest12_obj.pD3D12RootSignature.Get());
	softrest12_obj.pD3D12CommandList->RSSetViewports(1, &softrest12_obj.d3d12Viewport);
	softrest12_obj.pD3D12CommandList->RSSetScissorRects(1, &softrest12_obj.d3d12ScissorRect);

	//Indicate that the back buffer will be used as a render target
	D3D12_RESOURCE_BARRIER resBarrier = {};
	resBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resBarrier.Transition.pResource = softrest12_obj.pD3D12RenderTargets[softrest12_obj.frameIndex].Get();
	resBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	softrest12_obj.pD3D12CommandList->ResourceBarrier(1, &resBarrier);

	softrest12_obj.pD3D12CommandList->Close();


	//Execute the command list
	ID3D12CommandList* ppCommandLists[] = { softrest12_obj.pD3D12CommandList.Get() };
	softrest12_obj.pD3D12CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	softrest12_obj.pD3D12SwapChain->Present(1, 0);

	//wait for previous frame
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. More advanced samples
	// illustrate how to use fences for efficient resource usage.

	const UINT64 fence = softrest12_obj.fenceValue;
	softrest12_obj.pD3D12CommandQueue->Signal(softrest12_obj.pD3D12Fence.Get(), fence);
	softrest12_obj.fenceValue++;

	if (softrest12_obj.pD3D12Fence->GetCompletedValue() < fence)
	{
	softrest12_obj.pD3D12Fence->SetEventOnCompletion(fence, softrest12_obj.hFenceEvent);
	WaitForSingleObject(softrest12_obj.hFenceEvent, INFINITE);
	}
	*/

	return 0;
}

WINDOWS_SETTINGS currentSettings;

int Initialization()
{

	//start rasterizer
	InitializeSoftwareRasterizer(&softrest10_obj);

	//initialize the program
	ApplicationInitialization(softrest10_obj.clientRect);
	
	return 0;
}


int Update()
{

	//Game update
	if (ApplicationUpdate() == APPUPDATE_RESET)
	{
		//shutdown rasterizer
		ShutdownSoftwareRasterizer(&softrest10_obj);

		//release window
		DestroyWindow(softrest10_obj.mainWindow);

		return APPUPDATE_RESET;
	}

	//Lock the texture and draw
	D3D10_MAPPED_TEXTURE2D mappedTex;
	softrest10_obj.texture->Map(D3D10CalcSubresource(0,0,1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);

	//clear texture and assign to temporary variable
	UINT lpitch32 = mappedTex.RowPitch >> 2; //in bytes, div by 4 to get num dwords
	DWORD* texture_buffer = (DWORD*)mappedTex.pData;
	int pitch = mappedTex.RowPitch;
	ZeroMemory(texture_buffer, softrest10_obj.clientRect.bottom*mappedTex.RowPitch); 
	
	//Draw to texture
	ApplicationSoftwareRender(texture_buffer, lpitch32);

	//Release the texture
	softrest10_obj.texture->Unmap(D3D10CalcSubresource(0, 0, 1));
	  
	
	//apply pass and draw the main texture
	softrest10_obj.pD3D10EffectTechnique->GetPassByIndex(0)->Apply(0);
	softrest10_obj.pD3D10Device->DrawIndexed(softrest10_obj.numIndices, 0, 0);

	//allow the program to do optional hardware drawing now
	ApplicationHardwareRender();
	
	//end of frame
	softrest10_obj.pD3D10SwapChain->Present(0, 0);


	return APPUPDATE_NORMAL;
}



int Shutdown()
{
	//close the rasterizer
	ShutdownSoftwareRasterizer(&softrest10_obj);

	//close the program
	ApplicationShutdown();
	
	return 1;
}

void CreateEngineWindow(WINDOWS_SETTINGS& settings)
{
	HWND hwnd;
	POINT windowFrameDim = { 16, 39 };

	wchar_t * windowTitle = settings.windowTitle;

	if (!(hwnd = CreateWindowEx(NULL,
		TEXT(WINDOW_CLASS_NAME),     // class
		windowTitle, // title
		WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,		//style
		0, 0,	  // initial x,y
		settings.clientDimX + settings.clientOffset.x + windowFrameDim.x, settings.clientDimY + settings.clientOffset.y + windowFrameDim.y,  // initial width, height
		NULL,		// handle to parent 
		NULL,		 // handle to menu
		softrest10_obj.hInstance,	// instance of this application
		NULL)))	// extra creation parms
	{
		softrest10_obj.mainWindow = 0;
	}


	// save main window handle
	softrest10_obj.mainWindow = hwnd;

	//save the client rectangle
	GetClientRect(hwnd, &softrest10_obj.clientRect);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpCmdLine, int nCmdShow)
{

	//lock thread to one processor to make the timing work
	DWORD_PTR mask = 1;
	SetThreadAffinityMask(GetCurrentThread(), mask);

	//create default settings
	currentSettings.clientDimX = 960;
	currentSettings.clientDimY = 720;
	currentSettings.clientOffset = { 0, 0 };
	currentSettings.windowTitle = TEXT("Software Rasterizer");

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

	softrest10_obj.hInstance = hinstance; //save instance to global

//entry point when resetting
resetEntry:
	
	CreateEngineWindow(currentSettings);

	//----------------------------------------------------------------
	Initialization();

	while (TRUE)
	{
		// test if there is a message in queue, if so get it and remove it from the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			//if (msg.message == WM_QUIT)
			//	break;


			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			SystemHotKeys(softrest7_obj.mainWindow);
			
			int code = Update();
			if (code == APPUPDATE_RESET)
			{
				GetSettings(currentSettings);
				Shutdown();
				goto resetEntry;
			}
		}

	}

	Shutdown();
	//---------------------------------------------------------------------------------------

	return msg.wParam;

}


