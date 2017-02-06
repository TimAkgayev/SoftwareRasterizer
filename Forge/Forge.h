#pragma once
#pragma strict_gs_check(on) 

#include "../Common/GameEngine.h"
#include "../Common/ProgramEngineInterface.h"
#include "../Common/DebugView.h"

#include "../Common/EventListener.h"
#include "../Common/BezzierCurve.h"
#include "resource.h"
#include "SaveLoad.h"

Camera2D* cam2d;
DebugView* debugger;
SRTimer timer;
SRTimer timerHardware;

HCURSOR CURRENT_CURSOR, ARROW_CURSOR, REGION_CURSOR;
HCURSOR LAND1_CURSOR;
Grid backgroundGrid;
VECTOR2D selectionBoxPoints[2];




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
void Land1CB();
void Land2CB();
void Land3CB();
void Obstacle1CB();
void GridDivisionsCB();
void insertButtonCB();
void insertRegionCB();
void insertLableCB();
void insertTextCB();
void insertSelectionCB();
void insertRadioCB();
void insertListCB();
void insertCheckCB();

void CreateWorldBuilderUI();
void CreateGUIBuilderUI();

//utility 
void DrawClipRectBorder();
POINT FindNearestSnapPoint(POINT mouse);
void CreateCameraToViewportTransform(MATRIX4x4& m);
void CreateWorldToCameraTransform(MATRIX4x4& m);
void PlotParabola(float constant, int divisions);
void DrawParabola(DWORD* vid, int lpitch32);
void CreateUserInterface();

