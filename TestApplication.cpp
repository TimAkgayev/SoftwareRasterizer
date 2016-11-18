#include "TestApplication.h"

/*
UserInterface myUserInterface;

UserInterface* GetTestGUI()
{
	return &myUserInterface;
}

void ocbl()
{
	int x = 0;
}

void octf()
{
	int x = 0;
}

void Init()
{
	myUserInterface.LoadRosources();

	DWORD* btnMem = new DWORD[50 * 30];
	memset(btnMem, 0, sizeof(DWORD)* 50 * 30);


	myUserInterface.createButton(ocbl, 150, 30, "LOAD");
	myUserInterface.createTextField(octf, 10, 100);
	myUserInterface.createSelectionBox(100, 300, "TEXT BOX");
	myUserInterface.createCheckBox(300, 500, false);
	myUserInterface.createRadioGroup(500, 200, 5);
	myUserInterface.createDropContainer(10, 10);
	int off = 	ProgressiveDraw::drawLetter('L', 10, 20, 5, 5, COLOR_RED, btnMem, 50);
	ProgressiveDraw::drawLetter('P', 10, 20, off + 2, 5, COLOR_RED, btnMem, 50);


//	myUserInterface.createButton(50, 30, btnMem);
}

void Main(DWORD* mem, int lpitch32, float timeDelta)
{
	int offset = myUserInterface.displayString("UI Test Application", 10, 10, mem, lpitch32);
	offset += 10;
	offset = myUserInterface.displayString(1234567890, offset, 10, mem, lpitch32);
	offset += 10;

	DrawCircleParametric(mem, lpitch32, 100, 100, 10, COLOR_RED);

	//myUserInterface.displayButton(offset, 10, 0, 50, 30, mem, lpitch32);
	myUserInterface.DrawUI(mem, lpitch32, timeDelta);
}

void Shutdown()
{

}
*/