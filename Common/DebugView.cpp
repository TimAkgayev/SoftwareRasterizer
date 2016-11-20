#pragma once
#include "DebugView.h"


void DebugView::AddVariableView(void* var)
{
	ControlView v;
	v.variableView = var;
	v.edit = NULL;
	v.btnPlus = NULL;
	v.btnMinus = NULL;

	controlList.push_back(v);
}

void DebugView::IncrementVariable(int offset, float inc)
{

	*((float*)controlList[offset].variableView) += inc;
}

float DebugView::GetVariable(int offset)
{
	return *((float*)controlList[offset].variableView);
}

int DebugView::ListSize()
{
	return controlList.size();
}

void  DebugView::SetEditControl(int offset, HWND ctr)
{
	controlList[offset].edit = ctr;
}
void  DebugView::SetMinButton(int offset, HWND ctr)
{
	controlList[offset].btnMinus = ctr;
}
void  DebugView::SetPlusButton(int offset, HWND ctr)
{
	controlList[offset].btnPlus = ctr;
}
HWND  DebugView::GetEditControl(int offset)
{
	return controlList[offset].edit;
}
HWND  DebugView::GetMinusButton(int offset)
{
	return controlList[offset].btnMinus;

}
HWND  DebugView::GetPlusButton(int offset)
{
	return controlList[offset].btnPlus;

}
