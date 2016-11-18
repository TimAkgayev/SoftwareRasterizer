#include "VisualMatrix.h"


UserInterface myUserInterface;


/*

UserInterface* GetVisualMatrixGUI()
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



class SquareGrid
{
public:
	SquareGrid(int col, int row, int xpos, int ypos, int dim) {

		int x = xpos;
		int y = ypos;
		for (int c = 0; c < col; c++)
		{


			for (int r = 0; r < row; r++)
			{
				RECT2D rc;
				rc.setPos(x, y);
				rc.setWidth(dim);
				rc.setHeight(dim);

				mGridStates.push_back(false);
				mGrid.push_back(rc);

				y += dim;
			}

			y = ypos;
			x += dim;
		}

		m_numCols = col;
		m_numRows = row;
		m_dim = dim;

		mLinkedGrid = NULL;
	}
	~SquareGrid() {}

	void Set(int col, int row, bool val)
	{
		mGridStates[col + row*m_numCols] = val;
	}
	void Set(int vectorOffset, bool val)
	{
		mGridStates[vectorOffset] = val;
	}

	void LMouseDown(int x, int y)
	{
		vector<RECT2D>::iterator vIter;
		int offset = 0;
		for (vIter = mGrid.begin(); vIter < mGrid.end(); vIter++, offset++)
		{
			if ((*vIter).isPointInRect(x, y))
				mGridStates[offset] = !mGridStates[offset];
		}
	}

	void Draw(DWORD* mem, int lpitch32)
	{
		//update on each draw
		mInternalUpdate();

		vector<RECT2D>::iterator vIter;
		int offset = 0;
		for (vIter = mGrid.begin(); vIter < mGrid.end(); vIter++, offset++)
		{
			DWORD color;

			if (mGridStates[offset] == true)
			{
				color = COLOR_WHITE;

			}
			else
				color = COLOR_GREY;

			DrawRectangle(mem, lpitch32, vIter->GetWINRECT());
			DrawPixelSquare(vIter->GetWINRECT().left + 1, vIter->GetWINRECT().top + 1, color, m_dim - 2, mem, lpitch32);

		}
	}

	int GetPermuation()
	{
		return pow(2, m_numCols*m_numRows);
		
	}

	void Link(SquareGrid* grid)
	{
		if (this->GetPermuation() == grid->GetPermuation())
		{
			mLinkedGrid = grid;
		}
	}

private:

	void mInternalUpdate()
	{
		if (mLinkedGrid != NULL)
		{

			//set the linked block of memory to the same state as this block
			vector<bool>::iterator vIter = mGridStates.begin();
			int offset = 0;
			for (vIter; vIter < mGridStates.end(); vIter++)
			{
				mLinkedGrid->Set(offset, mGridStates[offset]);
			}
		}
	}

	int m_numCols, m_numRows, m_dim;
	vector<bool> mGridStates;
	vector<RECT2D> mGrid;
	SquareGrid* mLinkedGrid;

};

SquareGrid mmgrid(2, 1, 100, 100, 50);
SquareGrid memgrid(2, 4, 500, 100, 50);
SquareGrid seggrid(2, 2, 300, 100, 50);

void LButtonDown(int x, int y)
{
	mmgrid.LMouseDown(x, y);
}

void Init()
{
	myUserInterface.LoadRosources();

	DWORD* btnMem = new DWORD[50 * 30];
	memset(btnMem, 0, sizeof(DWORD)* 50 * 30);

	myUserInterface.SetLMouaseDownCallback(LButtonDown);
	//myUserInterface.createButton(ocbl, 150, 30, "LOAD");

}

void Main(DWORD* mem, int lpitch32, float timeDelta)
{
	int offset = myUserInterface.displayString("VisualMatrix", 10, 10, mem, lpitch32);
	offset += 10;
	offset = myUserInterface.displayString(1234567890, offset, 10, mem, lpitch32);
	offset += 10;

	//DrawCircleParametric(mem, lpitch32, 100, 100, 10, COLOR_RED);

	int width = 100;
	int height = 50;
	int xorig = 100;
	int yorig = 100;

	myUserInterface.displayString("Visual Input", 100, 80, mem, lpitch32);
	myUserInterface.displayString("Memory Matrix", 500, 80, mem, lpitch32);
	myUserInterface.displayString("Seg. Matrix", 300, 80, mem, lpitch32);
	

	mmgrid.Draw(mem, lpitch32);
	memgrid.Draw(mem, lpitch32);
	seggrid.Draw(mem, lpitch32);
	myUserInterface.DrawUI(mem, lpitch32, timeDelta);
}

void Shutdown()
{

}
*/