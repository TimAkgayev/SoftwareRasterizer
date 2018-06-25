#include "BouncingLines.h"
#include "MathStructures.h"

namespace BouncingLines
{

	vector<VERTEX2D> pointList;
	RECT2D selectionRect;


	UserInterface* MyUserInterface;

	vector<int> indexList;
	VECTOR2D start;
	VECTOR2D current;
	VECTOR2D dest;
	VECTOR2D dirN;
	VECTOR2D dir;
	float vel = 2;
	float t = 0.0f;
	RECT clipRect;
	RECT selectionBox;
	float msFramerate = 33;
	float msElapsed = 0;
	float m_deltaTime = 0.0f;
	float internalDelta = 0.0f;
	int gMouseX = 0;
	int gMouseY = 0;
	vector<int> pointSelectionList;
	string mode = "edit";
	VECTOR2D camMovDir(0, 0);
	VECTOR2D movCam(0, 0);
	bool isGUIHit = false;

	void MergeVertices(int mergeToOff, int fromOff, vector<VERTEX2D>& masterList)
	{
		//VERTEX2D temp = masterList[mergeToOff];
		//masterList.erase(masterList.begin() + )

	}
	void editBtnCallback()
	{
		mode = "edit";
	}
	void mergeBtnCallBack()
	{
		if (pointSelectionList.size() == 2)
		{
			pointList.push_back(pointList[pointSelectionList[0]]);
		}
	}
	void moveBtnCallback()
	{
		mode = "move";
	}

	void myMouseDown(int x, int y)
	{

	}

	void myMouseUp(int x, int y)
	{

		if (mode == "move")
			camMovDir = camMovDir + (VECTOR2D(x, y) - VECTOR2D(MyUserInterface->getLMDPosition().x, MyUserInterface->getLMDPosition().y));

		if (mode != "edit")
			return;

		if (x == MyUserInterface->getLMDPosition().x && y == MyUserInterface->getLMDPosition().y)
		{

			pointList.push_back(VERTEX2D(VECTOR2D(x - camMovDir.x, y - camMovDir.y), COLOR_RED, 0, 0));
		}

		selectionRect = RECT2D(selectionBox);
		vector<VERTEX2D>::iterator vIter;
		vIter = pointList.begin();
		pointSelectionList.clear();
		int count = 0;
		for (vIter; vIter < pointList.end(); count++, vIter++)
		{
			if (selectionRect.isPointInRect(vIter->pos.x + camMovDir.x, vIter->pos.y + camMovDir.y))
			{
				
				pointSelectionList.push_back(count);
			}
		}
		selectionBox = { 0, 0, 0, 0 };

	
	}

	void buttonCallback()
	{
		int x = 0;
	}

	void textCallback()
	{
		int x = 0;
	}

	LARGE_INTEGER startTime, endTime, performanceCount, startInternalTime, endInternalTime;

	void Init(RECT clipRct)
	{
	//	MyUserInterface->LoadRosources();
		MyUserInterface = new UserInterface;
		MyUserInterface->createButton(mergeBtnCallBack, 300, 10, "M", 30, 30);
		MyUserInterface->createButton(moveBtnCallback, 350, 10, "MOV", 30, 30);
		MyUserInterface->createButton(editBtnCallback, 400, 10, "EDT", 30, 30);
		UIDropContainer* dropBox = MyUserInterface->createDropContainer(10, 50);

		int itmID1 = MyUserInterface->LoadRosource("D:\\Users\\Tim\\Desktop\\UIBUIlderGraphics\\Button.bmp");
		int itmID2 = MyUserInterface->LoadRosource("D:\\Users\\Tim\\Desktop\\UIBUIlderGraphics\\Text.bmp");
		int itmID3 = MyUserInterface->LoadRosource("D:\\Users\\Tim\\Desktop\\UIBUIlderGraphics\\Lable.bmp");
		int itmID4 = MyUserInterface->LoadRosource("D:\\Users\\Tim\\Desktop\\UIBUIlderGraphics\\Radio.bmp");
		int itmID5 = MyUserInterface->LoadRosource("D:\\Users\\Tim\\Desktop\\UIBUIlderGraphics\\Selection.bmp");
		int itmID6 = MyUserInterface->LoadRosource("D:\\Users\\Tim\\Desktop\\UIBUIlderGraphics\\Check.bmp");


		dropBox->AddItem(MyUserInterface->GetImageResource(itmID1), UITYPE_BUTTON, buttonCallback);
		dropBox->AddItem(MyUserInterface->GetImageResource(itmID2), UITYPE_TEXTFIELD, textCallback);
		dropBox->AddItem(MyUserInterface->GetImageResource(itmID3), UITYPE_BUTTON, textCallback);
		dropBox->AddItem(MyUserInterface->GetImageResource(itmID4), UITYPE_RADIO, textCallback);
		dropBox->AddItem(MyUserInterface->GetImageResource(itmID5), UITYPE_SELECTIONBOX, textCallback);
		dropBox->AddItem(MyUserInterface->GetImageResource(itmID6), UITYPE_CHECKBOX, textCallback);
		
		vector<void*> selBoxInitData;
		string* pEditStr = new string("EDIT");
		string* pMoveStr = new string("MOVE");

		selBoxInitData.push_back((void*)pEditStr);
		selBoxInitData.push_back((void*)pMoveStr);
		dropBox->InsertInitializatinData(UITYPE_SELECTIONBOX, selBoxInitData);

		startTime.QuadPart = 0;
		startInternalTime.QuadPart = 0;
		current = start = VECTOR2D(0.0f, 0.0f);
		dest = VECTOR2D(100, 100);
		dirN = VECTOR2D(1.0f, 1.0f);
		dirN.Normalize();
		dir = dest - start;
		dir.Normalize();
		clipRect = clipRct;
		QueryPerformanceFrequency(&performanceCount);
	}

	void Main(DWORD* video_mem, int lpitch32, float deltaTime)
	{
		float dt = deltaTime;
		float et = msElapsed;
		float x = 0.0f/1800.0f;
		
		if (msElapsed > x)
		{
			QueryPerformanceCounter(&endInternalTime);

			internalDelta = float(endInternalTime.QuadPart - startInternalTime.QuadPart) / float(performanceCount.QuadPart);
			current =  current + dir*vel;
		

			VECTOR2D cur = current;

			if (cur.y >= clipRect.bottom)
			{
				start.x = cur.x;
				start.y = clipRect.bottom-1;

				dir.y *= -1;
				dir.Normalize();
			}
			if (cur.y <= 0)
			{
				start.x = current.x;;
				start.y = 0;

				dir.y *= -1;
				dir.Normalize();
			}

			if (cur.x >= clipRect.right)
			{
				start.x = clipRect.right-1;
				start.y = cur.y;

				dir.x *= -1;
				dir.Normalize();
			}
			if (cur.x <= 0)
			{
				start.x = 0;
				start.y = cur.y;

				dir.x *= -1;
				dir.Normalize();
			}

			msElapsed = 0;

			QueryPerformanceCounter(&startInternalTime);
		}

	
		QueryPerformanceCounter(&endTime);

		m_deltaTime = float(endTime.QuadPart - startTime.QuadPart) / float(performanceCount.QuadPart);


	
	

		msElapsed += m_deltaTime;
		VECTOR2D c = current;
		int off = MyUserInterface->displayString(m_deltaTime, 0, 0, video_mem, lpitch32);
		float iInDelt = 1.0 / internalDelta;
		if (internalDelta > 0)
			MyUserInterface->displayString(iInDelt, off + 10, 0, video_mem, lpitch32);
	//	DrawLineParametricClipped2D(video_mem, lpitch32, start.x, start.y, current.x, current.y, COLOR_RED, COLOR_RED);
		MESHOBJECT m;
		m.vertexBuffer = pointList;
		m.indexBuffer = indexList;
		
		MATRIX3D worldMat;
		VECTOR2D totCam = camMovDir;
		MatrixIdentity(&worldMat);
		if (mode == "move")
		{
			if (MyUserInterface->isLMD())
				totCam = camMovDir + (VECTOR2D(gMouseX, gMouseY) - VECTOR2D(MyUserInterface->getLMDPosition().x, MyUserInterface->getLMDPosition().y));
			
		}

		
	
		MatrixTranslation(&worldMat, totCam.x, totCam.y);
		m.worldTransform = worldMat;

		DrawMeshObject(m, DO_LINELIST, video_mem, lpitch32);
	
		
		MyUserInterface->DrawUI(video_mem, lpitch32, deltaTime);

		if (pointSelectionList.size())
		{
			vector<int>::iterator vIter;
			vIter = pointSelectionList.begin();
			for (vIter; vIter < pointSelectionList.end(); vIter++)
			{
				DrawPixelSquare(m.vertexBuffer[*vIter].pos.x + totCam.x, m.vertexBuffer[*vIter].pos.y + totCam.y, COLOR_BLUE, 6, video_mem, lpitch32);
			}
		}
	
		QueryPerformanceCounter(&startTime);


		//draw selection box
		if (mode == "edit")
		{
			if (MyUserInterface->isLMD())
			{

				DrawLineParametricClipped2D(video_mem, lpitch32, selectionBox.left, selectionBox.top, selectionBox.right, selectionBox.top, COLOR_RED, COLOR_RED);
				DrawLineParametricClipped2D(video_mem, lpitch32, selectionBox.left, selectionBox.bottom, selectionBox.right, selectionBox.bottom, COLOR_RED, COLOR_RED);
				DrawLineParametricClipped2D(video_mem, lpitch32, selectionBox.left, selectionBox.top, selectionBox.left, selectionBox.bottom, COLOR_RED, COLOR_RED);
				DrawLineParametricClipped2D(video_mem, lpitch32, selectionBox.right, selectionBox.top, selectionBox.right, selectionBox.bottom, COLOR_RED, COLOR_RED);


			}
		}
	}


	  

	void Shutdown()
	{
	}

	UserInterface* GetGUI() { return MyUserInterface; }



	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		UserInterface*  UIPtr = GetGUI();
		PAINTSTRUCT		ps;		//reserved for future use, debug text output
		HDC				hdc;

		// what is the message 
		switch (msg)
		{
		case WM_CREATE: //sent when window is created
		{

							return(0);
		} break;

		case WM_MOVE:
		{
						//window is moved

		}break;

		case WM_LBUTTONDOWN:
		{

							   float xPos = GET_X_LPARAM(lParam);
							   float yPos = GET_Y_LPARAM(lParam);
					
							   if (UIPtr->ProcessMouseClick(0, true, xPos, yPos) == GUI_MOUSEDOWN_NOHIT)
								   isGUIHit = false;
							   else
								   isGUIHit = true;

							  
	

		}break;

		case WM_LBUTTONUP:
		{
							 float xPos = GET_X_LPARAM(lParam);
							 float yPos = GET_Y_LPARAM(lParam);
							
							 if (UIPtr->ProcessMouseClick(0, false, xPos, yPos) == GUI_MOUSEUP_NOHIT)
								 myMouseUp(xPos, yPos);

							 selectionBox = { 0, 0, 0, 0 };


		}break;

		case WM_MOUSEWHEEL:
		{

		}break;

		case WM_MOUSEMOVE:
		{
							 gMouseX = GET_X_LPARAM(lParam);
							 gMouseY = GET_Y_LPARAM(lParam);

							 if (mode == "edit")
							 {
								 if (UIPtr->isLMD() && isGUIHit == false)
								 {

									 selectionBox = {UIPtr->getLMDPosition().x, UIPtr->getLMDPosition().y, gMouseX, gMouseY };

								 }
							 }
							 
							 UIPtr->ProcessMousePosition(gMouseX, gMouseY);

					

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
							 	UIPtr->ProcessKeyboard(wParam);

							 if (UIPtr->mode == "move")
							 {
								 if (wParam == VK_LEFT)
								 {
									 //		 GetCamera()->MoveCamera(VECTOR2D::LEFT_VECTOR(), Time.deltaTime);
								 }
								 if (wParam == VK_RIGHT)
								 {
									 //	 GetCamera()->MoveCamera(VECTOR2D::RIGHT_VECTOR(), Time.deltaTime);
								 }
								 if (wParam == VK_UP)
								 {
									 //	 GetCamera()->MoveCamera(VECTOR2D::UP_VECTOR(), Time.deltaTime);
								 }
								 if (wParam == VK_DOWN)
								 {
									 //	 GetCamera()->MoveCamera(VECTOR2D::DOWN_VECTOR(), Time.deltaTime);
								 }


							 }


							 /*
							 //tilde key (~)
							 if(wParam == VK_OEM_3)
							 {
							 if(console.isVisible())
							 console.hide();
							 else
							 console.show();
							 }

							 //all hotkeys disbled when console is active
							 if(console.isVisible())
							 {
							 console.processKey(wParam);
							 }
							 else
							 {

							 //compare to virtual key code of Enter (return) key
							 if(wParam == VK_RETURN)
							 {

							 if(mode == mode_edit)
							 {

							 }

							 }
							 else if(wParam == 0x45) //E key
							 {

							 if(mode == mode_edit)
							 mode = mode_normal;
							 else
							 mode = mode_edit;
							 }

							 else if(wParam == 0x53) //S key
							 {

							 if(mode == mode_edit)
							 {
							 tool = tool_select;
							 }


							 }
							 else if(wParam == VK_LEFT)
							 {
							 g_mndBrotCamX-= g_mndBrotCamSpeed;

							 }
							 else if(wParam == VK_RIGHT)
							 {
							 g_mndBrotCamX+= g_mndBrotCamSpeed;

							 }
							 else if(wParam == VK_UP)
							 {
							 g_mndBrotCamY-= g_mndBrotCamSpeed;

							 }
							 else if(wParam == VK_DOWN)
							 {
							 g_mndBrotCamY+= g_mndBrotCamSpeed;

							 }
							 else if(wParam == 0x5A)
							 {
							 g_mndBrotZoom -= 0.05f;
							 g_mndBrotCamSpeed = g_mndBrotZoom*10;
							 }
							 else if(wParam == 0x43)
							 {
							 g_mndBrotZoom += 0.05f;
							 g_mndBrotCamSpeed = g_mndBrotZoom*10;


							 }
							 }*/

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




}

