#pragma once
#include "stdinclude.h"



class BezierCurve : EventListener
{
public:
	BezierCurve() : EventListener(this) { }
	void ProcessEvent(UINT, WPARAM, LPARAM) override;

	void Draw(DWORD* mem, int lpitch32);
};


void BezierCurve::Draw(DWORD* mem, int lpitch32)

{

}

void BezierCurve::ProcessEvent(UINT m, WPARAM w, LPARAM l)
{
	//etc
	int x = 0;
}