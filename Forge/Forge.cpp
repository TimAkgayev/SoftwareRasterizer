#include "Forge.h"
#include <fstream>

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
const SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizerObject;
const SOFTWARERASTERIZER_DX12_OBJECTS* localRasterizerObject12;
RECT gClientRect;
Camera2D* cam2d;
SRTimer timer;
SRTimer timerHardware;
Viewport worldViewport;
HardwareSurface* s;
AnimatedBitmap RegionBtnImage;
HCURSOR CURRENT_CURSOR, ARROW_CURSOR, REGION_CURSOR;
Grid backgroundGrid;
VECTOR2D selectionBoxPoints[2];
int currentTool;
WINDOWS_SETTINGS LocalWindowsSettings;
UIWindow* newGUIWindow;
bool isResetRequested;
string GameDirectoryPath = "D:\\Users\\Tim\\Documents\\era\\";

enum {GUI_MODE, WORLD_MODE};
enum { TOOL_ARROW, TOOL_REGION };
int currentMode;
int world_gridspacing = 50;
int viewport_left_offset = 5;
int viewport_top_offset = 30;
int viewport_right_offset = 5;
int viewport_bottom_offset = 60;

int gMouseX = 0, gMouseY = 0;


DWORD headersig = 0xA0B1C2D3;
struct gui_file_header
{
	int header;
	int num_elements;
};

struct gui_element_header
{
	int header;
	int uitype;
};

int SaveGUIToFile(UserInterface* ui)
{
	gui_element_header header;
	gui_file_header fileHeader;
	fileHeader.header = headersig;
	header.header = headersig;
	ofstream outGUIFile((GameDirectoryPath + "ForgeGUI").data(), ios::out, ios::binary);
	int numElements = ui->GetNumberOfElements();

	if (!outGUIFile.is_open())
		return 0;

	fileHeader.num_elements = numElements;
	outGUIFile.write((char*)&fileHeader, sizeof(gui_file_header));

	for (int i = 0; i < numElements; i++)
	{
		UIElement* elm = ui->GetElement(i);
		
		switch (elm->GetType())
		{

			case UITYPE_BUTTON:
			{
				header.uitype = UITYPE_BUTTON;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIButton));
			}break;

			case UITYPE_TEXTFIELD:
			{
				header.uitype = UITYPE_TEXTFIELD;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UITextField));
			}break;

			case UITYPE_SELECTIONBOX:
			{
				header.uitype = UITYPE_SELECTIONBOX;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UISelectionBox));
			}break;

			case UITYPE_CHECKBOX:
			{
				header.uitype = UITYPE_CHECKBOX;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UICheckBox));
			}break;

			case UITYPE_RADIO:
			{
				header.uitype = UITYPE_RADIO;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIRadioGroup));
			}break;


			case UITYPE_DROPCONTAINER:
			{
				header.uitype = UITYPE_DROPCONTAINER;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIDropContainer));
			}break;

			case UITYPE_LIST:
			{
				header.uitype = UITYPE_LIST;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIList));
			}break;

			case UITYPE_REGION:
			{
				header.uitype = UITYPE_REGION;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIRegion));
			}break;

			case UITYPE_DROPDOWNMENU:
			{
				header.uitype = UITYPE_DROPDOWNMENU;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIDropdownMenu));
			}break;

			case UITYPE_WINDOW:
			{
				header.uitype = UITYPE_WINDOW;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIWindow));
			}break;

			case UITYPE_TEXT:
			{
				header.uitype = UITYPE_TEXT;
				outGUIFile.write((char*)&header, sizeof(gui_element_header));
				outGUIFile.write((char*)elm, sizeof(UIText));
			}	break;

			

			default: break;
		}

	}
	outGUIFile.close();

	return 1;
}

int LoadGUIFromFile(string filename, UserInterface* ui)
{
	ifstream inGUIFile((GameDirectoryPath + "ForgeGUI").data(), ios::in, ios::binary);
	if (!inGUIFile.is_open())
		return 0;

	gui_file_header fileHeader;
	//read in the file header
	inGUIFile.read((char*)&fileHeader, sizeof(gui_file_header));

	//read in all the elements
	gui_element_header elmHeader;
	for (int i = 0; i < fileHeader.num_elements; i++)
	{
		inGUIFile.read((char*)&elmHeader, sizeof(gui_element_header));
		
		switch (elmHeader.uitype)
		{

		case UITYPE_BUTTON:
		{
			UIButton* btn = new UIButton(0, 0);
			inGUIFile.read((char*)btn, sizeof(UIButton));
			ui->createButton(NULL, btn->GetPosition().x, btn->GetPosition().y, btn->GetText(), btn->GetRECT().getWidth(), btn->GetRECT().getHeight());
			delete btn;
			
		}break;

		case UITYPE_TEXTFIELD:
		{
			UITextField* txtField = new UITextField();
			inGUIFile.read((char*)txtField, sizeof(UITextField));
			ui->createTextField(NULL, txtField->GetPosition().x, txtField->GetPosition().y, txtField->GetCellCount());
			delete txtField;

		}break;

		case UITYPE_SELECTIONBOX:
		{
			UISelectionBox* selBox = new UISelectionBox();
			inGUIFile.read((char*)selBox, sizeof(UISelectionBox));
			ui->createSelectionBox(selBox->GetPosition().x, selBox->GetPosition().y, selBox->GetSelectedTitle() );
			delete selBox;

		}break;

		case UITYPE_CHECKBOX:
		{
			UICheckBox* checkBox = new UICheckBox();
			inGUIFile.read((char*)checkBox, sizeof(UICheckBox));
			ui->createCheckBox(checkBox->GetPosition().x, checkBox->GetPosition().y, checkBox->IsChecked());
			delete checkBox;

		}break;

		case UITYPE_RADIO:
		{
			UIRadioGroup* radio = new UIRadioGroup();
			inGUIFile.read((char*)radio, sizeof(UIRadioGroup));
			ui->createRadioGroup(radio->GetPosition().x, radio->GetPosition().y, radio->GetNumButtons());
			delete radio;

		}break;


		case UITYPE_DROPCONTAINER:
		{
			UIDropContainer* dropContainer = new UIDropContainer();
			inGUIFile.read((char*)dropContainer, sizeof(UIDropContainer));
			ui->createDropContainer(dropContainer->GetPosition().x, dropContainer->GetPosition().y);
			delete dropContainer;

		}break;

		case UITYPE_LIST:
		{
			UIList* uilist = new UIList();
			inGUIFile.read((char*)uilist, sizeof(UIList));
			ui->createList(uilist->GetPosition().x, uilist->GetPosition().y, uilist->GetRECT().getWidth());
			delete uilist;

		}break;

		case UITYPE_REGION:
		{
			UIRegion* uiregion = new UIRegion();
			inGUIFile.read((char*)uiregion, sizeof(UIRegion));
			ui->createRegion(uiregion->GetPosition().x, uiregion->GetPosition().y, uiregion->GetRECT().getWidth(), uiregion->GetRECT().getHeight());
			delete uiregion;

		}break;

		case UITYPE_DROPDOWNMENU:
		{
			UIDropdownMenu* uiDropMenu = new UIDropdownMenu();
			inGUIFile.read((char*)uiDropMenu, sizeof(UIDropdownMenu));
			ui->createDropdownMenu(uiDropMenu->GetPosition().x, uiDropMenu->GetPosition().y, uiDropMenu->GetTitle());
			delete uiDropMenu;

		}break;

		case UITYPE_WINDOW:
		{
			UIWindow* uiWindow = new UIWindow();
			inGUIFile.read((char*)uiWindow, sizeof(UIWindow));
			ui->createWindow(uiWindow->GetPosition().x, uiWindow->GetPosition().y, uiWindow->GetRECT().getWidth(), uiWindow->GetRECT().getHeight(), uiWindow->GetTitle(), uiWindow->GetBackgroundColor());
			delete uiWindow;

		}break;

		case UITYPE_TEXT:
		{
			UIText* uiText = new UIText();
			inGUIFile.read((char*)uiText, sizeof(UIText));
			ui->createText(uiText->GetPosition().x, uiText->GetPosition().y, uiText->GetRECT().getWidth(), uiText->GetRECT().getHeight(), uiText->GetText());
			delete uiText;
		}	break;



		default: break;
		}

	}
	

}

void RegionBtnCB()
{
	CURRENT_CURSOR = REGION_CURSOR;
	SetCursor(CURRENT_CURSOR);
	currentTool = TOOL_REGION;
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
	UITextField* widthField =  (UITextField*) MyUserInterface->GetElementByName("widthField");
	UITextField* heightField = (UITextField*) MyUserInterface->GetElementByName("heightField");

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
		UIButton* okBtn  = MyUserInterface->createButton(newGUIOkCB, 140, 250, "OK");
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



void ApplicationInitialization(RECT clientRect)
{
	localRasterizerObject = GetSoftwareRasterizer();

	ARROW_CURSOR = LoadCursor(localRasterizerObject->hInstance, MAKEINTRESOURCE(FORGE_CURSOR_ARROW));
	REGION_CURSOR = LoadCursor(localRasterizerObject->hInstance, MAKEINTRESOURCE(FORGE_CURSOR_REGION));
	CURRENT_CURSOR = ARROW_CURSOR;

	SetCursor(CURRENT_CURSOR);

	timerHardware.Initialize();
	timer.Initialize();
	
	float zoom = 1.0;
	cam2d = new Camera2D(int(float(localRasterizerObject->clientRect.right)*zoom + 0.5f), int(float(localRasterizerObject->clientRect.bottom)*zoom + 0.5f));
	cam2d->SetPosition(0, 0);
	cam2d->SetMoveSpeed(zoom*zoom*zoom); //has to be a power of zoom
	s = new HardwareSurface(100, 100);
	MyUserInterface = new UserInterface(localRasterizerObject);
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
	//UITextField* f = MyUserInterface->createTextField(textFieldCallback, 1, GetSoftwareRasterizer()->clientRect.bottom - 21);
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

	MyUserInterface->createRegion(5, GetSoftwareRasterizer()->clientRect.bottom - 50, GetSoftwareRasterizer()->clientRect.right - 10, 45);

	RegionBtnImage.AddFrame("D:\\Users\\Tim\\Documents\\era\\Source Images\\RegionButton.bmp");
	UIButton* b = MyUserInterface->createButton(RegionBtnCB, 10, GetSoftwareRasterizer()->clientRect.bottom - 45, "REGION", 35, 35);
	b->SetImage(&RegionBtnImage);

	modeGUICB();

	worldViewport.Rect.SetRECT({ viewport_left_offset, viewport_top_offset, GetSoftwareRasterizer()->clientRect.right - viewport_right_offset, GetSoftwareRasterizer()->clientRect.bottom - viewport_bottom_offset });

	cam2d->SetMoveSpeed(cam2d->GetMoveSpeed() + 3);
	backgroundGrid.SetViewport(&worldViewport);
	newGUIWindow = NULL;
	currentTool = TOOL_ARROW;

	isResetRequested = false;

	LocalWindowsSettings.clientDimX = GetSoftwareRasterizer()->clientRect.right;
	LocalWindowsSettings.clientDimY = GetSoftwareRasterizer()->clientRect.bottom;
	LocalWindowsSettings.clientOffset = { viewport_left_offset + viewport_right_offset, viewport_top_offset + viewport_bottom_offset };
	LocalWindowsSettings.windowTitle = TEXT("Reset Window");

	int widht = worldViewport.Rect.getWidth();
	int height = worldViewport.Rect.getHeight();
	int x = 0;
};




POINT FindNearestSnapPoint(POINT mouse)
{
	POINT snap;
	snap.x = int(float(mouse.x) / backgroundGrid.GetSpacing() + 0.5f) * backgroundGrid.GetSpacing();
	snap.y = int(float(mouse.y) / backgroundGrid.GetSpacing() + 0.5f) * backgroundGrid.GetSpacing();
	return snap;
}


void DrawViewportBorder(DWORD* mem, int lpitch32, Viewport& viewport)
{
	QueueLine( viewport.Rect.GetPosition().x - 1, viewport.Rect.GetPosition().y - 1, viewport.Rect.GetPosition().x + viewport.Rect.getWidth() + 1, viewport.Rect.GetPosition().y -1, COLOR_WHITE, COLOR_WHITE);
	QueueLine( viewport.Rect.GetPosition().x + viewport.Rect.getWidth() + 1, viewport.Rect.GetPosition().y - 1, viewport.Rect.GetPosition().x + viewport.Rect.getWidth() + 1, viewport.Rect.GetPosition().y + viewport.Rect.getHeight() + 1, COLOR_WHITE, COLOR_WHITE);
	QueueLine( viewport.Rect.GetPosition().x + viewport.Rect.getWidth() + 1, viewport.Rect.GetPosition().y + viewport.Rect.getHeight() + 1, viewport.Rect.GetPosition().x - 1, viewport.Rect.GetPosition().y + viewport.Rect.getHeight() + 1, COLOR_WHITE, COLOR_WHITE);
	QueueLine( viewport.Rect.GetPosition().x - 1, viewport.Rect.GetPosition().y + viewport.Rect.getHeight() + 1, viewport.Rect.GetPosition().x - 1, viewport.Rect.GetPosition().y -1, COLOR_WHITE, COLOR_WHITE);
}


void CreateCameraToViewportTransform(MATRIX4x4& m)
{

	m._00 = float(localRasterizerObject->clientRect.right) / cam2d->GetWidth();
	m._01 = 0;
	m._02 = 0;
	m._03 = 0;// -cam2d->GetPosition().x;

	m._10 = 0;
	m._11 = float(localRasterizerObject->clientRect.bottom) / cam2d->GetHeight();
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







vector<VERTEX2D> parabolaVs;
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
	if (constant * xStart * xStart > (camPos.y + cam2d->GetHeight()))
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
		QueueLine( pv1.x, pv1.y, pv2.x, pv2.y, parabolaVs[i].color, parabolaVs[i + 1].color);
	}
}

float dt;
float lpx = 0, lpy = 0;
float c = 0;
VECTOR2D v;
int ApplicationUpdate()
{

	if(isResetRequested)
		return APPUPDATE_RESET;
	
	float deltaTime = timer.Stop();
	v.x = 10;
	v.y = 10;
	c +=  deltaTime ;
	v.Normalize();
	PlotParabola(c / 10000.0f, 15);
	dt = deltaTime;
	DWORD* texMem;
	int texPitch;
	s->LockFrame(0, &texMem, &texPitch);
	for (int r = 0; r < 100; r++)
		for (int col = 0; col < 100; col++)
		{
			texMem[col + r*texPitch] = _RGBA32BIT(255, 255, 255, 120);
		}
	float lineSpeed = 20.0f;

	backgroundGrid.SetCenterCoordinate(cam2d->GetPosition().x, cam2d->GetPosition().y);

	int endx, endy;
	endx = int(lpx + c*v.x*lineSpeed + 0.5f);
	endy = int(lpy + c*v.y*lineSpeed + 0.5f);

	if (endx* endx + endy*endy >= 150*150)
		c = 0;

	QueueLine(lpx, lpy, endx, endy , _RGBA32BIT(255, 255, 255, 0), _RGBA32BIT(255, 0, 255, 255));
	s->UnlockFrame(0);

	myHotBox.Update();

	
	 
	timer.Start();


	return APPUPDATE_NORMAL;
		
}
	
void DrawRectangle(DWORD* mem, int lpitch32, VECTOR2D& p1, VECTOR2D& p2)
{
	QueueLine( p1.x, p1.y, p2.x, p1.y, COLOR_RED, COLOR_RED);
	QueueLine( p2.x, p1.y, p2.x, p2.y, COLOR_RED, COLOR_RED);
	QueueLine( p2.x, p2.y, p1.x, p2.y, COLOR_RED, COLOR_RED);
	QueueLine( p1.x, p2.y, p1.x, p1.y, COLOR_RED, COLOR_RED);

}

void GetSettings(WINDOWS_SETTINGS& settings)
{
	settings = LocalWindowsSettings;
}

void ApplicationSoftwareRender(DWORD* video_mem, int lpitch32)
{

	myHotBox.Draw(video_mem, lpitch32);
	backgroundGrid.Draw(video_mem, lpitch32);
	DrawParabola(worldViewport.TranslateMemory(video_mem, lpitch32), lpitch32);
	MyUserInterface->displayString(dt, GetSoftwareRasterizer()->clientRect.right- 230, 10, video_mem, lpitch32 );

	if (currentTool == TOOL_REGION && MyUserInterface->isLMD())
	{
		DrawRectangle(worldViewport.TranslateMemory(video_mem, lpitch32), lpitch32, selectionBoxPoints[0], selectionBoxPoints[1]);
	} 
	
	DrawLineQueue(video_mem, lpitch32);

	DrawViewportBorder(video_mem, lpitch32, worldViewport);

	string clientDim = std::to_string((int)worldViewport.Rect.getWidth()) + 'X' + std::to_string((int)worldViewport.Rect.getHeight());
	MyUserInterface->displayString(clientDim, GetSoftwareRasterizer()->clientRect.right / 2.0f, 10, video_mem, lpitch32);
	
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
	m._00 = 2.0f / GetSoftwareRasterizer()->clientRect.right;
	m._02 = -1;
	m._11 = 2.0f / GetSoftwareRasterizer()->clientRect.bottom;
	m._12 = -1;

	VECTOR3D v(0.5, 0.5, 1);
	MatrixVectorMultiply(&v, &matTot);
	MatrixVectorMultiply(&v, &m);


	//	MatrixVectorMultiply()

	XMFLOAT4X4 totalF; 
	XMStoreFloat4x4(&totalF, total);
	s->SetTransform(totalF);
	s->Draw();

	
	MyUserInterface->DrawUI(deltaTime);

	timerHardware.Start();
		
}

void ApplicationShutdown()
{
	if (s) {
		delete s;
		s = NULL;
	}
}

void myMouseCallBack(int x, int y)
{

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

