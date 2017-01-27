#include "Forge.h"

struct LandCell
{
	AnimatedBitmap sprite;
	bool isColliding;
};


class HotBox
{
public:
	HotBox() : xPos(-1), yPos(-1), isVisible(false), uiHandle(NULL), buttonActive(-1), buttonW(50), buttonH(20) { }
	~HotBox() {}

	void Init(UserInterface* hUI)
	{
		uiHandle = hUI;
	}

	void SetPositionAndMakeVisible(int x, int y)
	{
		xPos = x;
		yPos = y;
		isVisible = true;

		int xdir, ydir;
		int count = 1;
		for (int i = 0; i < buttonList.size(); i++)
		{
			if (count == 1)
			{
				xdir = -20;
				ydir = -50;
			}
			else if (count == 2)
			{
				xdir = 40;
				ydir = 0;
			}
			else if (count == 3)
			{
				xdir = -20;
				ydir = 50;
			}
			else if (count == 4)
			{
				xdir = -80;
				ydir = 0;
			}
			buttonList[i].button->SetPosition(x + xdir, y + ydir);
			buttonList[i].submenu->SetPosition(x + xdir, y + ydir + buttonH);
			buttonList[i].button->SetVisibility(true);

			count++;
		}

	}

	void SetVisiblity(bool state)
	{
		isVisible = state;
		for (vector<_hb_button>::iterator vIter = buttonList.begin(); vIter < buttonList.end(); vIter++)
		{
			vIter->button->SetVisibility(state);
			
		}

	}

	//returns the menu pointer to be edited
	UIList* AddButton(string name)
	{
		_hb_button hbb;
		hbb.button = uiHandle->createButton(NULL, -1, -1, name, buttonW, buttonH);
		hbb.submenu = uiHandle->createList(-1, -1, buttonW);
		hbb.submenu->SetDepthIndex(DEPTHINDEX_FOREGROUND);
		hbb.submenu->SetVisibility(false);
		hbb.button->SetDepthIndex(DEPTHINDEX_BACKGROUND);
		hbb.button->SetVisibility(false);
		buttonList.push_back(hbb);

		return hbb.submenu;

	}

	UIList* AddMainRowButton(string name)
	{
		_hb_button hbb;
		hbb.button = uiHandle->createButton(NULL, -1, -1, name, buttonW, buttonH);
		hbb.submenu = uiHandle->createList(-1, -1, buttonW);
		hbb.submenu->SetDepthIndex(DEPTHINDEX_FOREGROUND);
		hbb.submenu->SetVisibility(false);
		hbb.button->SetDepthIndex(DEPTHINDEX_BACKGROUND);
		hbb.button->SetVisibility(false);
		mainRowButtonList.push_back(hbb);

		return hbb.submenu;
	}

	bool IsVisible()
	{
		return isVisible;
	}

	void Draw(DWORD* video, int lpitch32)
	{

	}

	void Update()
	{
		if (buttonActive != -1)
		{
			if (buttonList[buttonActive].button->GetState() == UISTATE_PRESSED)
			{
				//hide submenu
				buttonList[buttonActive].submenu->SetVisibility(false);
				buttonActive = -1;
			}
			else return;
		}

		for (int i = 0; i < buttonList.size(); i++)
		{
			if (buttonList[i].button->GetState() == UISTATE_PRESSED)
			{
				if (buttonList[i].button->GetText() == "CREATE")
				{
					//set submenu "list" as visible
					buttonActive = i;
					buttonList[buttonActive].submenu->SetVisibility(true);

				}
			}
		}
	}
private:
	struct _hb_button
	{
		UIButton* button;
		UIList* submenu;
	};

	vector <_hb_button> buttonList;
	vector <_hb_button> mainRowButtonList;
	int xPos, yPos;
	int buttonW, buttonH;
	bool isVisible;
	UserInterface* uiHandle;
	int buttonActive;

	

};
HotBox myHotBox;
UserInterface* MyUserInterface;

enum { GUI_MODE, WORLD_MODE};
enum { TOOL_ARROW, TOOL_REGION};
enum { FUNCTION_NONE, FUNCTION_LANDPLOT };

WINDOWS_SETTINGS LocalWindowsSettings = { 1500, 1080,{ 0, 0 }, TEXT("Forge") };
RECT gClientRect;
int currentTool = TOOL_ARROW;
int currentFunction = FUNCTION_NONE;
bool isResetRequested = false;
UIWindow* newGUIWindow = NULL;
RECT2D gMarginRect = { 10, 30, 400, 60 }; //invsere area of a clip rect, contains margin offsets
RECT2D gControlPanelRect = {gMarginRect.left, LocalWindowsSettings.clientDimY - gMarginRect.bottom + 10.0f, LocalWindowsSettings.clientDimX - gMarginRect.right, LocalWindowsSettings.clientDimY -5.0f };
RECT2D gWorldPanelRect = { LocalWindowsSettings.clientDimX - gMarginRect.right + 10.0f , gMarginRect.top, LocalWindowsSettings.clientDimX - gMarginRect.left, gControlPanelRect.bottom};
string gRootImageDirectory = "D:\\Users\\Tim\\Documents\\Visual Studio Projects\\SoftwareRasterizer\\Forge\\Images\\";
int world_gridspacing = 35;
AnimatedBitmap* currentInsertElement;
vector<LandCell> LandSpriteList;

int currentMode;
int gMouseX = 0, gMouseY = 0;
vector<VERTEX2D> parabolaVs;
float dt;
float lpx = 0, lpy = 0;
float parabolaAnimTime = 0;
VECTOR2D v;



void ApplicationInitialization()
{

	//attach debug window
	debugger->AddVariableView((void*)&gMarginRect.left);
	debugger->AddVariableView((void*)&gMarginRect.top);
	debugger->AddVariableView((void*)&gMarginRect.right);
	debugger->AddVariableView((void*)&gMarginRect.bottom);

	//set cursor
	ARROW_CURSOR = LoadCursor(SoftwareRasterizer.hInstance, MAKEINTRESOURCE(FORGE_CURSOR_ARROW));
	REGION_CURSOR = LoadCursor(SoftwareRasterizer.hInstance, MAKEINTRESOURCE(FORGE_CURSOR_REGION));
	LAND1_CURSOR = LoadCursor(SoftwareRasterizer.hInstance, MAKEINTRESOURCE(FORGE_CURSOR_LAND1));
	CURRENT_CURSOR = ARROW_CURSOR;
	SetCursor(CURRENT_CURSOR);

	//timers
	timerHardware.Initialize();
	timer.Initialize();
	
	//camera
	float zoom = 1.5;
	cam2d = new Camera2D(int(float(SoftwareRasterizer.clientRect.right)*zoom + 0.5f), int(float(SoftwareRasterizer.clientRect.bottom)*zoom + 0.5f));
	cam2d->SetPosition(0, 0);
	cam2d->SetMoveSpeed(zoom*zoom*zoom); //has to be a power of zoom

	//UI
	CreateUserInterface();


	//Set mode to GUI by default
	modeGUICB();

	//Get client size for convinience
	gClientRect.right = SoftwareRasterizer.clientRect.right;
	gClientRect.bottom = SoftwareRasterizer.clientRect.bottom;

	//set up grid
	backgroundGrid.SetSpacing(35);
	
};

int ApplicationUpdate()
{

	if(isResetRequested)
		return APPUPDATE_RESET;
	
	float deltaTime = timer.Stop();
	parabolaAnimTime +=  deltaTime ;
	PlotParabola(parabolaAnimTime / 10000.0f, 15);

	DWORD* texMem;
	int texPitch;
	
	
	backgroundGrid.SetCenterCoordinate(cam2d->GetPosition().x, cam2d->GetPosition().y);

	myHotBox.Update();

	timer.Start();


	return APPUPDATE_NORMAL;	
}
	
void ApplicationSoftwareRender(DWORD* video_mem, int lpitch32)
{

	myHotBox.Draw(video_mem, lpitch32);
	MyUserInterface->displayString(dt, SoftwareRasterizer.clientRect.right - 230, 10, video_mem, lpitch32);
	string clientDim = std::to_string((int)SoftwareRasterizer.clientRect.right) + 'X' + std::to_string((int)SoftwareRasterizer.clientRect.bottom);
	MyUserInterface->displayString(clientDim, SoftwareRasterizer.clientRect.right / 2.0f, 10, video_mem, lpitch32);
	DrawClipRectBorder();

	QueueRectangle(gWorldPanelRect.GetWINRECT(), COLOR_WHITE);

	//generate clip from margin rectangle
	RECT cRect = gMarginRect.GetWINRECT();
	cRect.right = gClientRect.right - gMarginRect.right;
	cRect.bottom = gClientRect.bottom - gMarginRect.bottom;
	RECT2D oldClip = SetClipRectangle(cRect);
	backgroundGrid.Draw(video_mem, lpitch32);
	DrawParabola(video_mem, lpitch32);

	if (currentTool == TOOL_REGION && MyUserInterface->isLMD())
	{
		RECT r;
		r.left = selectionBoxPoints[0].x - 1;
		r.top = selectionBoxPoints[0].y - 1;
		r.right = selectionBoxPoints[1].x + 1;
		r.bottom = selectionBoxPoints[1].y + 1;

		QueueRectangle(r, COLOR_WHITE);
	} 
	
	SetClipRectangle(oldClip);

	DrawLineQueue(video_mem, lpitch32);

	vector<LandCell>::iterator vIter = LandSpriteList.begin();
	for (vIter; vIter < LandSpriteList.end(); vIter++)
	{
		vIter->sprite.Draw(video_mem, lpitch32);
	}
}

void ApplicationHardwareRender()
{
	float deltaTime = timerHardware.Stop();
	XMMATRIX scale, transform, total;
	scale = XMMatrixScaling(200, 200, 1);
	transform = XMMatrixTranslation(110, 110, 0);
	total = XMMatrixMultiply(scale, transform);


	MATRIX4D matTrans, matScale, matTot;
	MatrixIdentity(&matScale);
	MatrixScale(&matScale, 200, 200, 1);
	MatrixTranslation(&matTrans, 110, 110, 0);
	MatrixMatrixMultiply(&matTot, &matTrans, &matScale);

	MATRIX4D m;
	MatrixIdentity(&m);
	m._00 = 2.0f / SoftwareRasterizer.clientRect.right;
	m._02 = -1;
	m._11 = 2.0f / SoftwareRasterizer.clientRect.bottom;
	m._12 = -1;

	VECTOR3D v(0.5, 0.5, 1);
	MatrixVectorMultiply(&v, &matTot);
	MatrixVectorMultiply(&v, &m);


	//	MatrixVectorMultiply()

	XMFLOAT4X4 totalF; 
	XMStoreFloat4x4(&totalF, total);
	

	
	MyUserInterface->DrawUI(deltaTime);

	timerHardware.Start();
		
}

void ApplicationShutdown()
{
	
}



void CreateUserInterface()
{
	//UI
	MyUserInterface = new UserInterface(&SoftwareRasterizer);
	myHotBox.Init(MyUserInterface);
	UIList* createMenu = myHotBox.AddButton("GUI");
	createMenu->AddItem("TRIANGLE", NULL);
	createMenu->AddItem("RECTANGLE", NULL);
	createMenu->AddItem("CIRCLE", NULL);

	UIList* loadeMenu = myHotBox.AddButton("MOVE");
	loadeMenu->AddItem("RECENT", NULL);
	loadeMenu->AddItem("LAST", NULL);
	loadeMenu->AddItem("FILE", NULL);

	UIList* saveMenu = myHotBox.AddButton("WORLD");
	saveMenu->AddItem("SAVE AS", NULL);
	saveMenu->AddItem("SAVE COPY", NULL);
	saveMenu->AddItem("SAVE SETTINGS", NULL);

	UIList* quitMenu = myHotBox.AddButton("SCALE");
	quitMenu->AddItem("RESTART", NULL);
	quitMenu->AddItem("QUIT", NULL);
	//UITextField* f = MyUserInterface->createTextField(textFieldCallback, 1, SoftwareRasterizer.clientRect.bottom - 21);
	//f->SetCellCount(40);

	UIDropdownMenu* fileMenu = MyUserInterface->createDropdownMenu(5, 5, "FILE");
	fileMenu->AddItem("NEW GUI", fileNewGUICB);
	fileMenu->AddItem("LOAD", NULL);
	fileMenu->AddItem("SAVE", NULL);
	fileMenu->AddItem("EXIT", NULL);

	UIDropdownMenu* editMenu = MyUserInterface->createDropdownMenu(85, 5, "EDIT");
	editMenu->AddItem("MOVE", NULL);
	editMenu->AddItem("SCALE", NULL);


	UIDropdownMenu* modeMenu = MyUserInterface->createDropdownMenu(165, 5, "MODE");
	modeMenu->AddItem("GUI", modeGUICB);
	modeMenu->AddItem("WORLD", modeWorldCB);

	//tool panel
	MyUserInterface->createRegion(gControlPanelRect.GetPosition().x, gControlPanelRect.GetPosition().y, gControlPanelRect.getWidth(), gControlPanelRect.getHeight());

	//world panel
	MyUserInterface->createRegion(gWorldPanelRect.GetPosition().x, gWorldPanelRect.GetPosition().y, gWorldPanelRect.getWidth(), gWorldPanelRect.getHeight(), _RGBA32BIT(200, 200, 200, 255));

	
	UIButton* b = MyUserInterface->createButton(ArrowBtnCB, 15, SoftwareRasterizer.clientRect.bottom - 45, "ARROW", 35, 35);
	b->SetImage(ResourceManager::LoadImage(gRootImageDirectory + "ArrowButton.bmp"));

	UIButton* b2 = MyUserInterface->createButton(RegionBtnCB, 55, SoftwareRasterizer.clientRect.bottom - 45, "SELECT", 35, 35);
	b2->SetImage(ResourceManager::LoadImage(gRootImageDirectory + "RegionButton.bmp"));

	UIButton* b3 = MyUserInterface->createButton(Land1CB, gWorldPanelRect.GetPosition().x + 10, gWorldPanelRect.GetPosition().y + 10, "LAND1", 35, 35);
	b3->SetImage(ResourceManager::LoadImage(gRootImageDirectory + "Land1.bmp"));

	UIButton* b4 = MyUserInterface->createButton(Land2CB, gWorldPanelRect.GetPosition().x + 55, gWorldPanelRect.GetPosition().y + 10, "LAND2", 35, 35);
	b4->SetImage(ResourceManager::LoadImage(gRootImageDirectory + "Land2.bmp"));

	UIButton* b5 = MyUserInterface->createButton(Land3CB, gWorldPanelRect.GetPosition().x + 10, gWorldPanelRect.GetPosition().y + 55, "LAND3", 35, 35);
	b5->SetImage(ResourceManager::LoadImage(gRootImageDirectory + "Land3.bmp"));


}


void ArrowBtnCB()
{
	CURRENT_CURSOR = ARROW_CURSOR;
	SetCursor(CURRENT_CURSOR);
	currentTool = TOOL_ARROW;
}

void RegionBtnCB()
{
	CURRENT_CURSOR = REGION_CURSOR;
	SetCursor(CURRENT_CURSOR);
	currentTool = TOOL_REGION;
}

void Land1CB()
{
	CURRENT_CURSOR = LAND1_CURSOR;
	SetCursor(CURRENT_CURSOR);
	currentFunction = FUNCTION_LANDPLOT;
	currentInsertElement = AnimatedBitmap::GetLoadedInstance("Land1.bmp");
	int x = 9;
}

void Land2CB()
{
	CURRENT_CURSOR = LAND1_CURSOR;
	SetCursor(CURRENT_CURSOR);
	currentFunction = FUNCTION_LANDPLOT;
	currentInsertElement = AnimatedBitmap::GetLoadedInstance("Land2.bmp");
	int x = 9;
}

void Land3CB()
{
	CURRENT_CURSOR = LAND1_CURSOR;
	SetCursor(CURRENT_CURSOR);
	currentFunction = FUNCTION_LANDPLOT;
	currentInsertElement = AnimatedBitmap::GetLoadedInstance("Land3.bmp");
	int x = 9;
}


void PlotParabola(float constant, int divisions)
{
	parabolaVs.clear();
	VECTOR2D camPos = cam2d->GetPosition();
	float xStart = camPos.x;
	int yIntercept = camPos.y + cam2d->GetHeight();

	//find intersectioni with top of the camnera if y is negative (up)
	if (constant < 0)
	{
		yIntercept = camPos.y;
	}
	if (constant * xStart * xStart >(camPos.y + cam2d->GetHeight()))
	{
		xStart = -sqrt((yIntercept) / constant);
	}
	float xEnd = camPos.x + cam2d->GetWidth();
	if (constant * xEnd * xEnd > (camPos.y + cam2d->GetHeight()))
	{
		xEnd = sqrt((yIntercept) / constant);
	}
	float increments = (xEnd - xStart) / divisions;
	float x, y;

	for (x = xStart; x <= xEnd; x += increments)
	{
		y = constant*x*x;


		parabolaVs.push_back(VERTEX2D(VECTOR2D(x, y), COLOR_BLUE, 0, 0));

	}
	int n = 0;

}
void DrawParabola(DWORD* vid, int lpitch32)
{

	MATRIX4x4 viewportTx;
	MATRIX4x4 cameraTx;

	CreateWorldToCameraTransform(cameraTx);
	for (int i = 0; i < parabolaVs.size() - 1; i++)
	{
		VECTOR3D pv1(parabolaVs[i].pos.x, parabolaVs[i].pos.y, 1);
		MatrixVectorMultiply(&pv1, &cameraTx);
		VECTOR3D pv2(parabolaVs[i + 1].pos.x, parabolaVs[i + 1].pos.y, 1);
		MatrixVectorMultiply(&pv2, &cameraTx);
		QueueTransformClipLine(pv1.x, pv1.y, pv2.x, pv2.y, parabolaVs[i].color, parabolaVs[i + 1].color);
	}
}
void DrawClipRectBorder()
{
	RECT cRect = gMarginRect.GetWINRECT();
	cRect.left -= 1;
	cRect.top -= 1;
	cRect.right = gClientRect.right - gMarginRect.right + 1;
	cRect.bottom = gClientRect.bottom - gMarginRect.bottom + 1;

	QueueRectangle(cRect, COLOR_WHITE);

}
POINT FindNearestSnapPoint(POINT mouse)
{
	POINT snap;
	snap.x = int(float(mouse.x) / backgroundGrid.GetSpacing() + 0.5f) * backgroundGrid.GetSpacing();
	snap.y = int(float(mouse.y) / backgroundGrid.GetSpacing() + 0.5f) * backgroundGrid.GetSpacing();
	return snap;
}

POINT FindNearestCellCenter(POINT mouse)
{
	POINT center;
	center.x = int(float(mouse.x) / backgroundGrid.GetSpacing() + 0.5f) * backgroundGrid.GetSpacing();
	center.y = int(float(mouse.y) / backgroundGrid.GetSpacing() + 0.5f) * backgroundGrid.GetSpacing();

	center.x += 12;
	center.y += 12;

	return center;
}
void CreateCameraToViewportTransform(MATRIX4x4& m)
{

	m._00 = float(SoftwareRasterizer.clientRect.right) / cam2d->GetWidth();
	m._01 = 0;
	m._02 = 0;
	m._03 = 0;// -cam2d->GetPosition().x;

	m._10 = 0;
	m._11 = float(SoftwareRasterizer.clientRect.bottom) / cam2d->GetHeight();
	m._12 = 0;
	m._13 = 0;//-cam2d->GetPosition().y;

	m._20 = 0;
	m._21 = 0;
	m._22 = 1;
	m._23 = 0;

	m._30 = 0;
	m._31 = 0;
	m._32 = 0;
	m._33 = 1;
}
void CreateWorldToCameraTransform(MATRIX4x4& m)
{

	m._00 = 1;
	m._01 = 0;
	m._02 = 0;
	m._03 = -cam2d->GetPosition().x;

	m._10 = 0;
	m._11 = 1;
	m._12 = 0;
	m._13 = -cam2d->GetPosition().y;

	m._20 = 0;
	m._21 = 0;
	m._22 = 1;
	m._23 = 0;

	m._30 = 0;
	m._31 = 0;
	m._32 = 0;
	m._33 = 1;
}






void AttachDebugger(DebugView* dbg)
{
	debugger = dbg;
}

WINDOWS_SETTINGS& UpdateSettings()
{
	return LocalWindowsSettings;
}



LRESULT CALLBACK ApplicationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	PAINTSTRUCT		ps;		
	HDC				hdc;

		
	switch (msg)
	{
	case WM_CREATE: 
	{
		return(0);
	} break;

	case WM_MOVE:
	{
						

	}break;

	case WM_COMMAND:
	{
		int ButtonID = LOWORD(wParam);
		int offset;
		int inc;

		if (ButtonID == 0)
			break;
		if (ButtonID / 2 > debugger->ListSize())
			break;
		//odd
		if (ButtonID % 2 != 0)
		{
			offset = (ButtonID - 1) / 2 ;
			inc = -1;

		}

		else if (ButtonID % 2 == 0)//even
		{
			offset = ButtonID / 2 - 1;
			inc = 1;
		}

	
		if (debugger)
		{
			debugger->IncrementVariable(offset, inc);
			//update text
			SetWindowText(debugger->GetEditControl(offset), std::to_wstring(debugger->GetVariable(offset)).c_str());
		}

	}break;

	case WM_LBUTTONDOWN:
	{

		float xPos = GET_X_LPARAM(lParam);
		float yPos = GET_Y_LPARAM(lParam);

		if (MyUserInterface->ProcessMouseClick(0, true, xPos, yPos) == GUI_MOUSEDOWN_NOHIT)
		{
			if (currentTool == TOOL_REGION)
			{
				POINT snap = FindNearestSnapPoint({ int(xPos + 0.5f), int(yPos + 0.5f) });
				selectionBoxPoints[0].x = snap.x;
				selectionBoxPoints[0].y = snap.y;

				//set both points so that were not drawing 0,0 -> pt1
				selectionBoxPoints[1].x = snap.x;
				selectionBoxPoints[1].y = snap.y;
			}
			
			if (currentFunction == FUNCTION_LANDPLOT)
			{
				POINT corner = FindNearestSnapPoint({ int(xPos + 0.5f), int(yPos + 0.5f) });
				//make sure that there is no other bitmap in that cell, if so delete it
				auto vIter = LandSpriteList.begin();
				for (vIter; vIter < LandSpriteList.end(); vIter++)
				{
					if (corner.x == vIter->sprite.GetPosition().x && corner.y == vIter->sprite.GetPosition().y)
					{
						LandSpriteList.erase(vIter);
						break;
					}
				}

				LandCell lc;
				lc.sprite = *currentInsertElement;
				lc.sprite.SetPosition( corner.x, corner.y);
				
				LandSpriteList.push_back(lc);
			}

			
		}
					
	}break;

	case WM_RBUTTONDOWN:
	{
		float xPos = GET_X_LPARAM(lParam);
		float yPos = GET_Y_LPARAM(lParam);

		if (currentFunction == FUNCTION_LANDPLOT)
		{

			POINT corner = FindNearestSnapPoint({ int(xPos + 0.5f), int(yPos + 0.5f) });
			
			//erase sprite clicked on
			vector<AnimatedBitmap>::iterator vIter = LandSpriteList.begin();
			for (vIter; vIter < LandSpriteList.end(); vIter++)
			{
				if (corner.x == vIter->GetPosition().x && corner.y == vIter->GetPosition().y)
				{
					vIter = LandSpriteList.erase(vIter);
					break;
				}
			}

			int x = 0;
		}

	}break;
	
	case WM_LBUTTONUP:
	{
		float xPos = GET_X_LPARAM(lParam);
		float yPos = GET_Y_LPARAM(lParam);

		if (MyUserInterface->ProcessMouseClick(0, false, xPos, yPos) == GUI_MOUSEUP_NOHIT)
		{
			if (currentTool == TOOL_REGION)
			{
				
				//make sure the points are ordere in increasing order
				if (selectionBoxPoints[1].x < selectionBoxPoints[0].x)
				{
					int temp = selectionBoxPoints[0].x;
					selectionBoxPoints[0].x = selectionBoxPoints[1].x;
					selectionBoxPoints[1].x = temp;

					temp = selectionBoxPoints[0].y;
					selectionBoxPoints[0].y = selectionBoxPoints[1].y;
					selectionBoxPoints[1].y = temp;
				}
			
				MyUserInterface->createRegion(selectionBoxPoints[0].x, selectionBoxPoints[0].y, selectionBoxPoints[1].x - selectionBoxPoints[0].x, selectionBoxPoints[1].y - selectionBoxPoints[0].y);
			}
		}

		
	}break;

	case WM_MOUSEWHEEL:
	{

	}break;

	case WM_MOUSEMOVE:
	{
		gMouseX = GET_X_LPARAM(lParam);
		gMouseY = GET_Y_LPARAM(lParam);

						
		MyUserInterface->ProcessMousePosition(gMouseX, gMouseY);

		if (currentTool == TOOL_REGION)
		{
			if (MyUserInterface->isLMD())
			{
				POINT snap = FindNearestSnapPoint({ gMouseX, gMouseY });
				POINT firstPos = { int(selectionBoxPoints[0].x + 0.5f), int(selectionBoxPoints[0].y + 0.5f) };
				selectionBoxPoints[1].x = snap.x;
				selectionBoxPoints[1].y = snap.y;

	


			}
		}


	}break;

	case WM_PAINT:
	{

		//not used, directdraw takes over
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return(0);

	} break;

	case WM_KEYDOWN: //any keyboard press generates this message
	{
			int cam_speed_x = cam2d->GetMoveSpeed(), cam_speed_y = cam2d->GetMoveSpeed();
			MyUserInterface->ProcessKeyboard(wParam);

			if (MyUserInterface->UIHasFocus() == false)
			{
				if (wParam == VK_SPACE)
				{
					if (!myHotBox.IsVisible()) //do only if not already visible, so it doesn't move with the mouse
						myHotBox.SetPositionAndMakeVisible(gMouseX, gMouseY);
				}
				else if (wParam == 0x57) //w key
				{
					cam2d->SetPosition(cam2d->GetPosition().x, cam2d->GetPosition().y - cam_speed_y);
				}
				else if (wParam == 0x53) //s key
				{
					cam2d->SetPosition(cam2d->GetPosition().x, cam2d->GetPosition().y + cam_speed_y);
				}
				else if (wParam == 0x41) //a key
				{
					cam2d->SetPosition(cam2d->GetPosition().x - cam_speed_x, cam2d->GetPosition().y);
				}
				else if (wParam == 0x44) //d key
				{
					cam2d->SetPosition(cam2d->GetPosition().x + cam_speed_x, cam2d->GetPosition().y);

				} 
			}
	}break; 

	case WM_KEYUP:
	{
			if (wParam == VK_SPACE)
			{
				myHotBox.SetVisiblity(false);
			}
				
	}break;

	case WM_SETCURSOR:
	{
		SetCursor(CURRENT_CURSOR);
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




//Callbacks
/////////////////////////////////////////////////////////////////////////////////////////



void myMouseCallBack(int x, int y)
{

}

void textFieldCallback()
{

}
void fileLoadCB()
{
	int x = 0;
}
void fileSaveCB()
{
	int x = 0;

}
void fileExitCB()
{
	int x = 0;

}
void editMoveCB()
{
	int x = 0;

}
void editScaleCB()
{

}
void modeGUICB()
{
	currentMode = GUI_MODE;
	cam2d->SetMovementConstraint(MOVE_CONSTRAINT_ALL);

}
void modeWorldCB()
{
	currentMode = WORLD_MODE;
	cam2d->SetMovementConstraint(MOVE_CONSTRAINT_NONE);
}
void newGUIOkCB()
{
	UITextField* widthField = (UITextField*)MyUserInterface->GetElementByName("widthField");
	UITextField* heightField = (UITextField*)MyUserInterface->GetElementByName("heightField");

	LocalWindowsSettings.clientDimX = std::stoi(widthField->GetText());
	LocalWindowsSettings.clientDimY = std::stoi(heightField->GetText());

	isResetRequested = true;
}
void newGUICancelCB()
{
	//destroy window 
	newGUIWindow->SetVisibility(false);

}
void widthTextCB()
{

}
void heightTextCB()
{

}
void fileNewGUICB()
{
	if (!newGUIWindow)
	{
		newGUIWindow = MyUserInterface->createWindow(100, 100, 400, 300, "NEW GUI");

		UIText* widthText = MyUserInterface->createText(100, 100, 70, 18, "WIDTH", COLOR_BLUE);
		UIText* heightText = MyUserInterface->createText(100, 170, 70, 18, "HEIGHT", COLOR_BLUE);


		UITextField* widthTxtField = MyUserInterface->createTextField(widthTextCB, 200, 100, 4);
		widthTxtField->SetName("widthField");
		UITextField* heightTxtField = MyUserInterface->createTextField(heightTextCB, 200, 170, 4);
		heightTxtField->SetName("heightField");
		UIButton* okBtn = MyUserInterface->createButton(newGUIOkCB, 140, 250, "OK");
		UIButton* canBtn = MyUserInterface->createButton(newGUICancelCB, 220, 250, "CANCEL");
		newGUIWindow->AddChild(okBtn);
		newGUIWindow->AddChild(canBtn);
		newGUIWindow->AddChild(widthTxtField);
		newGUIWindow->AddChild(heightTxtField);
		newGUIWindow->AddChild(widthText);
		newGUIWindow->AddChild(heightText);
	}
	else
		newGUIWindow->SetVisibility(true);
}
