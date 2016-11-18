#include "../Common/ProgramEngineInterface.h"


UserInterface* DefaultUserInterface;
const SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizerObject;
WINDOWS_SETTINGS LocalWindowsSettings;

AnimatedBitmap backAnim, frontAnim, leftAnim, rightAnim;
AnimatedBitmap brickWall, brickWallRotated;
AnimatedBitmap wigwam;
AnimatedBitmap WigwamLarge;
string playerMove = "none";

UIButton* baseBtn;

UserInterface* mainMenuUI;
UserInterface* ingameUI;
Player inGamePlayer;

LARGE_INTEGER startTime;
LARGE_INTEGER endTime;
LARGE_INTEGER perfFreq;
bool isClockStarted = false;
float deltaTime2 = 0;

bool isWDown = false, isDDown = false, isSDown = false, isADown = false;


World mainMenu;
World inGame;
World testWorld;

World* currentWorld = &mainMenu;

void mmNewGameCb()
{
	currentWorld = &inGame;
}

void mmLoadGameCb()
{

}

void mmQuitGameCb()
{

}

void mmCreateWigwamBtn()
{
	WigwamLarge.SetPosition(currentWorld->player->GetPosition().x - 80, currentWorld->player->GetPosition().y);
	currentWorld->collisionObjects.push_back(WigwamLarge);
}

void CreateInGameWorld()
{

	POINT wh;
	brickWall.GetFrameDimensions(wh);
	int lastYPos = 0;
	for (int i = 0; i < 6; i++)
	{
		brickWall.SetPosition(0, i * wh.y);
		inGame.collisionObjects.push_back(brickWall);
		lastYPos = i* wh.y + int(float(wh.y) / 2.0f + 0.5f);
	}

	POINT wh2;
	brickWallRotated.GetFrameDimensions(wh2);
	int xOffset = wh.x;
	for (int i = 0; i < 8; i++)
	{
		brickWallRotated.SetPosition(xOffset + i * wh2.x, lastYPos);
		inGame.collisionObjects.push_back(brickWallRotated);
	}

	for (int i = 0; i < 8; i++)
	{
		brickWallRotated.SetPosition(xOffset + i * wh2.x, 0);
		inGame.collisionObjects.push_back(brickWallRotated);
	}

	for (int i = 0; i < 6; i++)
	{
		brickWall.SetPosition(localRasterizerObject->clientRect.right - wh.x, i * wh.y);
		inGame.collisionObjects.push_back(brickWall);

	}
}

void LoadData()
{
	brickWall.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\BrickWall.bmp", 0.1f);
	brickWallRotated.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\BrickWall.bmp", 0.1f);
	brickWallRotated.RotateLeft();

	WigwamLarge.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\WigwamLarge.bmp", 0.04f);

	backAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\EraCharacter1.bmp", 0.1f);
	backAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\EraCharacter2.bmp", 0.1f);
	backAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\EraCharacter3.bmp", 0.1f);
	backAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\EraCharacter4.bmp", 0.1f);
	backAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\EraCharacter5.bmp", 0.1f);
	backAnim.SetFramerate(7);

	frontAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\FrontSide1.bmp", 0.1f);
	frontAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\FrontSide2.bmp", 0.1f);
	frontAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\FrontSide3.bmp", 0.1f);
	frontAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\FrontSide4.bmp", 0.1f);
	frontAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\FrontSide5.bmp", 0.1f);
	frontAnim.SetFramerate(7);

	leftAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\LeftSide1.bmp", 0.1f);
	leftAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\LeftSide2.bmp", 0.1f);
	leftAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\LeftSide3.bmp", 0.1f);
	leftAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\LeftSide4.bmp", 0.1f);
	leftAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\LeftSide5.bmp", 0.1f);
	leftAnim.SetFramerate(7);

	rightAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\RightSide1.bmp", 0.1f);
	rightAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\RightSide2.bmp", 0.1f);
	rightAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\RightSide3.bmp", 0.1f);
	rightAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\RightSide4.bmp", 0.1f);
	rightAnim.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\RightSide5.bmp", 0.1f);
	rightAnim.SetFramerate(7);

	wigwam.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\wigwam.bmp");

	inGamePlayer.AddFrontAnimation(frontAnim);
	inGamePlayer.AddBackAnimation(backAnim);
	inGamePlayer.AddLeftAnimation(leftAnim);
	inGamePlayer.AddRightAnimation(rightAnim);
}

void ApplicationInitialization(RECT)
{
	QueryPerformanceFrequency(&perfFreq);

	//get the rasterizer
	localRasterizerObject = GetSoftwareRasterizer();
	DefaultUserInterface = new UserInterface(localRasterizerObject);

	//load sprites
	LoadData();
		
	//set up player
	inGamePlayer.SetPosition(300, 100);

	//set up camera
	float centerX = float(localRasterizerObject->clientRect.right) / 2.0f;
	float centerY = float(localRasterizerObject->clientRect.bottom) / 2.0f;

	POINT fDim;
	frontAnim.GetFrameDimensions(fDim);
	float frameHalfX = float(fDim.x) / 2.0f;
	float frameHalfY = float(fDim.y) / 2.0f;

	float deltaX = centerX - inGamePlayer.GetPosition().x - frameHalfX;
	float deltaY = centerY - inGamePlayer.GetPosition().y - frameHalfY;

	IncrementDrawOffset(deltaX, deltaY);

	//set up the worlds
	mainMenuUI = new UserInterface(localRasterizerObject);
	mainMenuUI->createButton(mmNewGameCb, 100, 100, "NEW");
	mainMenuUI->createButton(mmLoadGameCb, 100, 200, "LOAD");
	mainMenuUI->createButton(mmQuitGameCb, 100, 300, "QUIT");
	mainMenu.userInterface = mainMenuUI;

	ingameUI = new UserInterface(localRasterizerObject);
	ingameUI->createRegion(5, localRasterizerObject->clientRect.bottom - 165, localRasterizerObject->clientRect.right - 10, 160);
	baseBtn = ingameUI->createButton(NULL, 15, 560, "BASE");
	baseBtn->SetImage(&wigwam);
	baseBtn->SetOnLClickCallback(mmCreateWigwamBtn);

	inGame.userInterface = ingameUI;
	inGame.player = &inGamePlayer;

	LocalWindowsSettings.clientDimX = GetSoftwareRasterizer()->clientRect.right;
	LocalWindowsSettings.clientDimY = GetSoftwareRasterizer()->clientRect.bottom;
	LocalWindowsSettings.clientOffset = { 0, 0 };
	LocalWindowsSettings.windowTitle = TEXT("Era");

	CreateInGameWorld();

}
void ApplicationSoftwareRender(DWORD* video_mem, int lpitch32)
{
	currentWorld->SoftwareDraw(video_mem, lpitch32);

	int off = currentWorld->userInterface->displayString("Allocated space: ", 10, 10, video_mem, lpitch32);
	currentWorld->userInterface->displayString(ResourceManager::getAllocatedSpace(), off, 10, video_mem, lpitch32);


}
void ApplicationHardwareRender()
{
		
	currentWorld->HardwareDraw(deltaTime2);
}
void ApplicationShutdown()
{

}
int ApplicationUpdate()
{
	if (isClockStarted)
	{
		QueryPerformanceCounter(&endTime);
		deltaTime2 = float(endTime.QuadPart - startTime.QuadPart) / perfFreq.QuadPart;

	}




	//see if the current wolrd contains a player
	if (currentWorld->player)
	{


		VECTOR2D playerPos = inGamePlayer.GetPosition();
		VECTOR2D oldPlayerPos = playerPos;
		RECT2D testLoc;
		inGamePlayer.GetBoundingRect(testLoc);
		bool isPlayerPosUpdated = false;

		if (isWDown)
		{
			inGamePlayer.SetDirection("up");
			playerPos = inGamePlayer.GetPosition() + VECTOR2D::DOWN_VECTOR() * deltaTime2 * inGamePlayer.GetMoveSpeed();
			inGamePlayer.GetBoundingRect(testLoc);
			testLoc.setPos(playerPos);
			isPlayerPosUpdated = true;
		}
		else if (isSDown)
		{
			inGamePlayer.SetDirection("down");
			playerPos = inGamePlayer.GetPosition() + VECTOR2D::UP_VECTOR() * deltaTime2 * inGamePlayer.GetMoveSpeed();
			inGamePlayer.GetBoundingRect(testLoc);
			testLoc.setPos(playerPos);
			isPlayerPosUpdated = true;
		}
		else if (isADown)
		{
			inGamePlayer.SetDirection("left");
			playerPos = inGamePlayer.GetPosition() + VECTOR2D::LEFT_VECTOR() * deltaTime2 * inGamePlayer.GetMoveSpeed();
			inGamePlayer.GetBoundingRect(testLoc);
			testLoc.setPos(playerPos);
			isPlayerPosUpdated = true;
		}
		else if (isDDown)
		{
			inGamePlayer.SetDirection("right");
			playerPos = inGamePlayer.GetPosition() + VECTOR2D::RIGHT_VECTOR() * deltaTime2 * inGamePlayer.GetMoveSpeed();
			inGamePlayer.GetBoundingRect(testLoc);
			testLoc.setPos(playerPos);
			isPlayerPosUpdated = true;
		}

		if (isPlayerPosUpdated)
		{
			bool isCollisionDetected = false;
			vector<AnimatedBitmap>::iterator vIter = inGame.collisionObjects.begin();
			RECT2D colObjRect;
			RECT winRect;
			for (vIter; vIter < inGame.collisionObjects.end(); vIter++)
			{
				vIter->GetBoundingRectangle(colObjRect);
				winRect = colObjRect.GetWINRECT();
				if (testLoc.isRectInRect(winRect))
				{
					isCollisionDetected = true;
					break;
				}



			}

			if (isCollisionDetected == false)
			{
				inGamePlayer.SetPosition(playerPos);

				VECTOR2D movOffset = playerPos - oldPlayerPos;
				//set the viewport offset
				IncrementDrawOffset(-movOffset.x, -movOffset.y);
			}
		}



	}






	QueryPerformanceCounter(&startTime);
	isClockStarted = true;

	return 0;
}


void GetSettings(WINDOWS_SETTINGS& set)
{
	set = LocalWindowsSettings;
}

LRESULT CALLBACK ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	UserInterface* UserInterfaceInUse = DefaultUserInterface;

	if (currentWorld->userInterface)
		UserInterfaceInUse = currentWorld->userInterface;

	switch (msg)
	{
	case WM_CREATE:
	{
		return(0);
	} break;

	case WM_MOVE:
	{


	}break;

	case WM_LBUTTONDOWN:
	{

		float xPos = GET_X_LPARAM(lParam);
		float yPos = GET_Y_LPARAM(lParam);

		if (UserInterfaceInUse->ProcessMouseClick(0, true, xPos, yPos) == GUI_MOUSEDOWN_NOHIT)
		{
		}

	}break;

	case WM_LBUTTONUP:
	{
		float xPos = GET_X_LPARAM(lParam);
		float yPos = GET_Y_LPARAM(lParam);

		if (UserInterfaceInUse->ProcessMouseClick(0, false, xPos, yPos) == GUI_MOUSEUP_NOHIT)
		{
		}


	}break;

	case WM_MOUSEWHEEL:
	{

	}break;

	case WM_MOUSEMOVE:
	{
		int MouseX = GET_X_LPARAM(lParam);
		int MouseY = GET_Y_LPARAM(lParam);


		UserInterfaceInUse->ProcessMousePosition(MouseX, MouseY);



	}break;

	case WM_PAINT:
	{
		PAINTSTRUCT		ps;
		HDC				hdc;

		//not used, directdraw takes over
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return(0);

	} break;

	case WM_KEYDOWN: //any keyboard press generates this message
	{
		//	int cam_speed_x = cam2d->GetMoveSpeed(), cam_speed_y = cam2d->GetMoveSpeed();
		UserInterfaceInUse->ProcessKeyboard(wParam);



		if (UserInterfaceInUse->UIHasFocus() == false)
		{
			if (wParam == VK_SPACE)
			{
				//	if (!myHotBox.IsVisible()) //do only if not already visible, so it doesn't move with the mouse
					//	myHotBox.SetPositionAndMakeVisible(gMouseX, gMouseY);
			}
			else if (wParam == 0x57) //w key
			{


				isWDown = true;
			}
			else if (wParam == 0x53) //s key
			{

				isSDown = true;

			}
			else if (wParam == 0x41) //a key
			{
				isADown = true;

			}
			else if (wParam == 0x44) //d key
			{

				isDDown = true;


			}
		}

	}break;

	case WM_KEYUP:
	{
		playerMove = "none";

		if (wParam == VK_SPACE)
		{
			//myHotBox.SetVisiblity(false);
		}

		else if (wParam == 0x57) //w key
		{
			isWDown = false;
		}
		else if (wParam == 0x53) //s key
		{

			isSDown = false;

		}
		else if (wParam == 0x41) //a key
		{

			isADown = false;

		}
		else if (wParam == 0x44) //d key
		{

			isDDown = false;

		}

	}break;

	case WM_DESTROY: //window is destoyed
	{
		PostQuitMessage(0);
		return(0);
	} break;


	default: break;

	} // end switch

		// process any messages that we didn't take care of
	return (DefWindowProc(hwnd, msg, wParam, lParam));

}
