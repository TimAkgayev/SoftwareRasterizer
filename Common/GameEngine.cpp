#include "GameEngine.h"
#include "ProgramEngineInterface.h"


int Initialization()
{

	//start rasterizer
	InitializeSoftwareRasterizer();

	//initialize the program
	ApplicationInitialization();

	return 0;
}


int Update()
{

	//Game update
	if (ApplicationUpdate() == APPUPDATE_RESET)
	{
		//shutdown rasterizer
		ShutdownSoftwareRasterizer();

		//release window
		DestroyWindow(SoftwareRasterizer.mainWindow);

		
		return APPUPDATE_RESET;
	}
	//clear render target
	SoftwareRasterizer.pD3D10Device->ClearRenderTargetView(SoftwareRasterizer.pD3D10RenderTargetView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	//Lock the texture and draw
	D3D10_MAPPED_TEXTURE2D mappedTex;
	SoftwareRasterizer.texture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);

	//clear texture and assign to temporary variable
	UINT lpitch32 = mappedTex.RowPitch >> 2; //in bytes, div by 4 to get num dwords
	DWORD* texture_buffer = (DWORD*)mappedTex.pData;
	int pitch = mappedTex.RowPitch;
	ZeroMemory(texture_buffer, SoftwareRasterizer.clientRect.bottom*mappedTex.RowPitch);

	//Draw to texture
	ApplicationSoftwareRender(texture_buffer, lpitch32);

	//Release the texture
	SoftwareRasterizer.texture->Unmap(D3D10CalcSubresource(0, 0, 1));


	//apply pass and draw the main texture
	SoftwareRasterizer.pD3D10EffectTechnique->GetPassByIndex(0)->Apply(0);
	SoftwareRasterizer.pD3D10Device->DrawIndexed(SoftwareRasterizer.numIndices, 0, 0);

	//allow the program to do optional hardware drawing now
	ApplicationHardwareRender();

	//end of frame
	SoftwareRasterizer.pD3D10SwapChain->Present(0, 0);


	return APPUPDATE_NORMAL;
}



int Shutdown()
{
	//close the rasterizer
	ShutdownSoftwareRasterizer();

	//close the program
	ApplicationShutdown();

	return 1;
}




Player::Player()
{
	position = VECTOR2D(0, 0);
	facingDirection = "up";
	moveSpeed = 400.0f;
	isPositionUpdated = false;
}

Player::~Player()
{
	
}

void Player::AddFrontAnimation(BitmapImage* anim)
{
	frontAnim = anim;
	frontAnim->SetPosition(position);
}

void Player::AddLeftAnimation(BitmapImage* anim)
{
	leftAnim = anim;
	leftAnim->SetPosition(position);
}

void Player::AddRightAnimation(BitmapImage* anim)
{
	rightAnim = anim;
	rightAnim->SetPosition(position);
}

void Player::AddBackAnimation(BitmapImage* anim)
{
	backAnim = anim;
	backAnim->SetPosition(position);
}

void Player::SetPosition(float x, float y)
{
	SetPosition(VECTOR2D(x, y));

}

void Player::SetPosition(VECTOR2D& pos)
{
	position = pos;
	backAnim->SetPosition(pos);
	frontAnim->SetPosition(pos);
	leftAnim->SetPosition(pos);
	rightAnim->SetPosition(pos);
	isPositionUpdated = true;

}

VECTOR2D Player::GetPosition() const
{
	return position;
}

float Player::GetMoveSpeed() const
{
	return moveSpeed;
}

void Player::GetBoundingRect(RECT2D& ret) 
{
	frontAnim->GetBoundingRectangle(ret);
	
}

void Player::SetDirection(string dir)
{
	facingDirection = dir;
}

void Player::Draw(DWORD* vmem, int lpitch32)
{
	if (!isPositionUpdated)
	{
		if (facingDirection == "up")
		{
			RECT2D r;
			backAnim->GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, ImageManager::GetImage(backAnim->GetFrame(0)), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}
		else if (facingDirection == "down")
		{
			RECT2D r;
			frontAnim->GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, ImageManager::GetImage(frontAnim->GetFrame(0)), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}
		else if (facingDirection == "left")
		{
			RECT2D r;
			leftAnim->GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, ImageManager::GetImage(leftAnim->GetFrame(0)), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}
		else if (facingDirection == "right")
		{
			RECT2D r;
			rightAnim->GetBoundingRectangle(r);
			DrawBitmapWithClipping(vmem, lpitch32, ImageManager::GetImage(rightAnim->GetFrame(0)), int(r.GetPosition().x + 0.5f), int(r.GetPosition().y + 0.5f), NULL);
		}

		return;
	}

	if (facingDirection == "up")
	{
		backAnim->Draw(vmem, lpitch32);
	}
	else if (facingDirection == "down")
	{
		frontAnim->Draw(vmem, lpitch32);
	}
	else if (facingDirection == "left")
	{
		leftAnim->Draw(vmem, lpitch32);
	}
	else if (facingDirection == "right")
	{
		rightAnim->Draw(vmem, lpitch32);
	}

	isPositionUpdated = false;
}

World::World()
{
	userInterface = (NULL);
	player = (NULL);
}

void World::SoftwareDraw(DWORD* mem, int lpitch32)
{
	
	if(player)
		player->Draw(mem, lpitch32);

 	vector<BitmapImage>::iterator vIter = collisionObjects.begin();
	for (vIter; vIter < collisionObjects.end(); vIter++)
	{
		vIter->Draw(mem, lpitch32);
	}
}

void World::HardwareDraw(double deltaTime)
{
	if(userInterface)
		userInterface->DrawUI(deltaTime);
}


Grid::Grid()
{
	line_thickness = 1;
	grid_spacing = 30;
	CenterCoordinate.x = CenterCoordinate.y = 0;
}

Grid::~Grid()
{

}

void Grid::SetCenterCoordinate(float x, float y)
{
	CenterCoordinate.x = x;
	CenterCoordinate.y = y;
}

void Grid::SetSpacing(int spacing)
{
	grid_spacing = spacing;
}

void Grid::SetLineThickness(int thickness)
{
	line_thickness = thickness;
}



void Grid::DrawHorizontal(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart, DWORD colorEnd, int indentAxis, DWORD c1, DWORD c2)
{

	int counter = 1;

	for (int movingY = startY; movingY < endY; movingY += pxSpacing, counter++)
	{
		VECTOR3D v1(startX, movingY, 1);
		VECTOR3D v2(endX, movingY, 1);


		//draw main axis in different color
		if (counter == indentAxis)
			QueueTransformClipLine(v1.x, v1.y, v2.x, v2.y, c1, c2);
		else
			QueueTransformClipLine(v1.x, v1.y, v2.x, v2.y, colorStart, colorEnd);

	}
}

void Grid::DrawVertical(DWORD* mem, int lpitch32, int startX, int startY, int endX, int endY, float pxSpacing, DWORD colorStart, DWORD colorEnd, int indentAxis, DWORD c1, DWORD c2)
{


	int counter = 1;

	for (int movingX = startX; movingX < endX; movingX += pxSpacing, counter++)
	{
		VECTOR3D v1(movingX, startY, 1);
		VECTOR3D v2(movingX, endY, 1);


		//draw main axis in different color
		if (counter == indentAxis)
			QueueTransformClipLine(v1.x, v1.y, v2.x, v2.y, c1, c2);
		else
			QueueTransformClipLine(v1.x, v1.y, v2.x, v2.y, colorStart, colorEnd);


	}
}

int Grid::GetSpacing() const
{
	return grid_spacing;
}


void Grid::Draw(DWORD* mem, int lpitch32)
{
	int viewWidth, viewHeight;

	viewWidth = SoftwareRasterizer.clientRect.right;
	viewHeight = SoftwareRasterizer.clientRect.bottom;


	int gridStartX;
	int	gridStartY;

	//establish the start points for vertical and horizontal lines i.e allow grid to move with camera
	if (CenterCoordinate.x >= 0)
		gridStartX = grid_spacing - int(CenterCoordinate.x) % grid_spacing;  //need to go backwards so we subtract from spacing (space - remainder)
	else
		gridStartX = int(abs(CenterCoordinate.x) + 0.5f) % grid_spacing;

	if (CenterCoordinate.y >= 0)
		gridStartY = grid_spacing - int(CenterCoordinate.y + 0.5f) % grid_spacing;
	else
		gridStartY = int(abs(CenterCoordinate.y) + 0.5f) % grid_spacing;

	DWORD color1 = COLOR_RED;
	DWORD color2 = COLOR_BLUE;

	int yAxisIndent = -1;
	int xAxisIndent = -1;

	//see if axis is visible
	if (CenterCoordinate.x < 0 && CenterCoordinate.x + viewWidth > 0)
	{
		//fmod is same as mod but works on floats
		if (fmod((abs(CenterCoordinate.x)), grid_spacing) == 0)
			yAxisIndent = (abs(float(CenterCoordinate.x)) / grid_spacing) + 1;

		//ceil because it's going to be the + 1 line of whatever were drawing, so if we get 2.6, that means that 3rd grid line is the axis
		else
			yAxisIndent = ceil(abs(float(CenterCoordinate.x)) / grid_spacing);

	}
	if (CenterCoordinate.y < 0 && CenterCoordinate.y + viewHeight > 0)
	{
		if (fmod((abs(float(CenterCoordinate.y))), grid_spacing) == 0)
			xAxisIndent = (abs(float(CenterCoordinate.y)) / grid_spacing) + 1;
		//ceil because it's going to be the + 1 line of whatever were drawing, so if we get 2.6, that means that 3rd grid line is the axis
		else
			xAxisIndent = ceil(abs(float(CenterCoordinate.y)) / grid_spacing);

	}


	DrawHorizontal(mem, lpitch32, 0, gridStartY, viewWidth, viewHeight, grid_spacing, color1, color2, xAxisIndent);
	DrawVertical(mem, lpitch32, gridStartX, 0, viewWidth, viewHeight, grid_spacing, color1, color2, yAxisIndent);

}










//unused for now
SOFTWARERASTERIZER_DX7_OBJECTS softrest7_obj;
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


	//DrawMandelbrotSet(video_*buffer, lpitch32, g_mndBrotCamX, g_mndBrotCamY, g_mndBrotZoom);

	//	SnowGameMain(video_*buffer, lpitch32, Time.deltaTime);
	//	OceanMain(video_*buffer, lpitch32, Time.deltaTime);
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

	softrest12_obj.pD3D12CommandList->SetGraphicSoftwareRasterizerootSignature(softrest12_obj.pD3D12RootSignature.Get());
	softrest12_obj.pD3D12CommandList->RSSetViewports(1, &softrest12_obj.d3d12Viewport);
	softrest12_obj.pD3D12CommandList->RSSetScissorRects(1, &softrest12_obj.d3d12ScissorRect);

	//Indicate that the back *buffer will be used as a render target
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