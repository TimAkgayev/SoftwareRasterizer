#pragma once


#pragma once
#include <vector>
#include <windows.h>

struct ControlView
{
	void* variableView;

	HWND edit;
	HWND btnPlus;
	HWND btnMinus;
};

class DebugView
{

public:

	void AddVariableView(void* var);

	void IncrementVariable(int offset, float inc);
	float GetVariable(int offset);
	void SetEditControl(int offset, HWND ctr);
	void SetMinButton(int offset, HWND ctr);
	void SetPlusButton(int offset, HWND ctr);
	HWND GetEditControl(int offset);
	HWND GetMinusButton(int offset);
	HWND GetPlusButton(int offset);
	int ListSize();

private:
	std::vector<ControlView> controlList;
};