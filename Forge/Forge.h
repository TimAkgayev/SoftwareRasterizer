#pragma once

#include "../Common/GameEngine.h"
#include "../Common/ProgramEngineInterface.h"
#include "../Common/DebugView.h"

#include "resource.h"
#include "SaveLoad.h"

Camera2D* cam2d;
DebugView* debugger;
SRTimer timer;
SRTimer timerHardware;
AnimatedBitmap RegionBtnImage;
HCURSOR CURRENT_CURSOR, ARROW_CURSOR, REGION_CURSOR;
Grid backgroundGrid;
VECTOR2D selectionBoxPoints[2];


enum { GUI_MODE, WORLD_MODE };
enum { TOOL_ARROW, TOOL_REGION };

void RegionBtnCB();
void ArrowBtnCB();
void textFieldCallback();
void fileLoadCB();
void fileSaveCB();
void fileExitCB();
void editMoveCB();
void editScaleCB();
void modeGUICB();
void modeWorldCB();
void newGUIOkCB();
void newGUICancelCB();
void widthTextCB();
void heightTextCB();
void fileNewGUICB();
void myMouseCallBack(int x, int y);


//utility 
void DrawClipRectBorder();
POINT FindNearestSnapPoint(POINT mouse);
void CreateCameraToViewportTransform(MATRIX4x4& m);
void CreateWorldToCameraTransform(MATRIX4x4& m);
void PlotParabola(float constant, int divisions);
void DrawParabola(DWORD* vid, int lpitch32);
void CreateUserInterface();

