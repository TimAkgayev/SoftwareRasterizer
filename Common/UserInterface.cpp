#include "UserInterface.h"




UIElement::UIElement()
{
	typeID = 0;
	stateID = 0;
	isInFocus = false;
	isVisible = true;
	isEnabled = true;
}

void UIElement::OnHover(int xPos, int yPos) {}

//returns if hit the element or it's child
bool UIElement::OnLClick(int mouseX, int mouseY)
{
	return false;
}

bool UIElement::OnLUp(int mouseX, int mouseY)
{
	return false;
}

bool UIElement::OnLDown(int mouseX, int mouseY)
{
	return false;
}

bool UIElement::IsInRegion(int mouseX, int mouseY)
{
	return false;
}

void UIElement::SetVisibility(bool state)
{
	isVisible = state;
}

void UIElement::SetEnabledState(bool state)
{
	isEnabled = state;
}

bool UIElement::GetVisibility() const { return isVisible; }

bool UIElement::GetEnabledState() const { return isEnabled; }

void UIElement::SetFocus(bool state)
{
	isInFocus = state;
}

bool UIElement::GetFocus() const { return isInFocus; }

void UIElement::SetType(int t) { typeID = t; }
int UIElement::GetType() const { return typeID; }

void UIElement::SetState(int state) { stateID = state; }
int UIElement::GetState() const { return stateID; }

void UIElement::Draw(DWORD* mem, int lpitch32, float timeDelta) { 
	int x = 0;
}

void UIElement::DrawForeground(DWORD* mem, int lpitch32, float timeDelta)
{
	int x = 0;
}

void UIElement::DrawBackground(DWORD* mem, int lpitch32, float timeDelta)
{
	int x = 0;
}

void UIElement::SetPosition(int xpos, int ypos)
{
	region.setPos(xpos, ypos);
}

void UIElement::SetDimension(int width, int height)
{
	region.setWidth(width);
	region.setHeight(height);
}

void UIElement::SetDepthIndex(int index)
{
	depthIndex = index;
}

void UIElement::SetName(std::string name)
{
	mElementName = name;
}

std::string UIElement::GetName() const
{
	return mElementName;
}

int UIElement::GetDepthIndex() const { return depthIndex; }

VECTOR2D UIElement::GetPosition() const { return region.GetPosition(); }

RECT2D UIElement::GetRECT() const { return region; }

UIButton::UIButton()
{
	typeID = UITYPE_BUTTON;
	stateID = UISTATE_NORMAL;
	mFrameSize = 0;
	mFrameMem = NULL;
	memset(mFrameMem, 0, mFrameSize);
	mText = "Button";
	screen_h = 0;
	screen_w = 0;
	bgColor = COLOR_BLACK;
	mFont = new Font("../Resource/Fonts/arial.ttf", 0 ,  8);
	
}

UIButton::UIButton(int width, int height): UIButton() {

	region.setWidth(width);
	region.setHeight(height);
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);
	mFrameMem = new DWORD[region.getWidth() * region.getHeight()];
	memset(mFrameMem, 0, mFrameSize);

}

UIButton::~UIButton()
{
	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UIButton::OnHover(int xPos, int yPos)
{

	if (region.isPointInRect(xPos, yPos))
		stateID = UISTATE_HOVER;
	else
		stateID = UISTATE_NORMAL;
}

void UIButton::SetText(string text)
{
	mText = text;
}

string UIButton::GetText() const { return mText; }

bool UIButton::IsInRegion(int x, int y)
{
	return region.isPointInRect(x, y);
}

void UIButton::SetDimension(int w, int h)
{
	if (mFrameMem)
		delete mFrameMem;
	mFrameMem = new DWORD[w * h];

	region.setWidth(w);
	region.setHeight(h);
}

bool UIButton::OnLDown(int xPos, int yPos)
{
	if (region.isPointInRect(xPos, yPos))
	{
		stateID = UISTATE_PRESSED;
		return true;
	}

	return false;
}

bool UIButton::OnLUp(int xPos, int yPos)
{
	stateID = UISTATE_HOVER; //default to hover since pointer is still over the button

	if (region.isPointInRect(xPos, yPos))
	{
		mCallbackLClick();
		return true;
	}

	return false;
}

void UIButton::SetOnLClickCallback(void(*cb)())
{
	mCallbackLClick = cb;
}

void UIButton::SetImage(int RMKey)
{
	Bitmap* bmp = ImageManager::GetImage(RMKey);
	

	//scale the image to button size
	bmp->ResizeBitmap(region.GetWINRECT());
	mImage = new BitmapImage(bmp);


	
	//position must allways be zero because we're drawing in a local ui frame
	mImage->SetPosition(0, 0);	
}



void UIButton::Draw(DWORD* mem, int lpitch32, float timeDelta)
{

	ZeroMemory(mFrameMem, mFrameSize);
	DWORD col = COLOR_RED;


	//draw background
	for (int y = 0; y < region.getHeight() - 1; y++)
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(), 0, y, region.getWidth() - 1, y, bgColor, bgColor);


	if (mImage)
	{
		//if (mImage->GetNumFrames())
		//	mImage->Draw(mFrameMem, region.getWidth());
	}
	else
		mFont->Draw(mFrameMem, (int)region.getWidth(), mText, int(((region.getWidth() - mFont->GetStringPixelLength(mText)) / 2.0) + 0.5f), 5, &col);
	

	DWORD colorState = 0;
	if (stateID == UISTATE_PRESSED)
		colorState = COLOR_GREEN;
	else if (stateID == UISTATE_NORMAL)
		colorState = COLOR_WHITE;
	else if (stateID == UISTATE_HOVER)
		colorState = COLOR_RED;



	
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth() - 1, 0, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() - 1, 0, 0, colorState, colorState);
	
	
	//blit to screen
	DWORD* tempMem = mFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	VECTOR2D dim(region.getWidth(), region.getHeight());
	tempMainMem += int(pos.x + pos.y*lpitch32);



	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempMainMem, tempMem, sizeof(DWORD)*int(region.getWidth()));
		tempMainMem += lpitch32;
		tempMem += int(region.getWidth());
	}
}






UITextField::UITextField() {
	typeID = UITYPE_TEXTFIELD;
	stateID = UISTATE_NORMAL;
	mCursorPos = 0;
	mIsCursorOn = true;
	mBlinkTimeElapsed = 0;
	isInFocus = false;
	mMaxCharCount = -1;
	bckgColor = _RGBA32BIT(0, 0, 0, 255);
	mFont = NULL;

}

UITextField::UITextField(int xpos, int ypos, int width, int height, int maxcharcount): UITextField()
{

	region.setHeight(height);
	region.setWidth(width);
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);

	mForegroundFrameMem = new DWORD[region.getHeight() * region.getWidth()];
	mBackgroundFrameMem = new DWORD[region.getHeight() * region.getWidth()];

	memset(mForegroundFrameMem, 0, mFrameSize);
	memset(mBackgroundFrameMem, 0, mFrameSize);

	mFont = new Font("../Resource/Fonts/arial.ttf", 0, height - 10);

	region.setPos(xpos, ypos);

	mMaxCharCount = maxcharcount;
}

UITextField::~UITextField() {
	

	if (mForegroundFrameMem)
	{
		delete mForegroundFrameMem;
		mForegroundFrameMem = NULL;
	}

	if (mBackgroundFrameMem)
	{
		delete mBackgroundFrameMem;
		mBackgroundFrameMem = NULL;
	}

	if (mSelectionColor)
	{
		delete mSelectionColor;
		mSelectionColor = NULL;
	}

	if (mFont)
	{
		delete mFont;
		mFont = NULL;
	}
}



void UITextField::OnHover(int xPos, int yPos)
{
	if (region.isPointInRect(xPos, yPos))
		stateID = UISTATE_HOVER;
	else
		stateID = UISTATE_NORMAL;
}

bool UITextField::OnLDown(int xPos, int yPos)
{
	if (region.isPointInRect(xPos, yPos))
		return true;

	return false;
}

bool UITextField::OnLUp(int xPos, int yPos)
{

	if (region.isPointInRect(xPos, yPos))
		return true;

	return false;

}

void UITextField::SetMaxCharCount(int count)
{
	mMaxCharCount = count;
}

int UITextField::GetMaxCharCount() const
{
	return mMaxCharCount;
}

void UITextField::SetOnLClickCallback(void(*cb)())
{
	mCallbackLClick = cb;
}

void UITextField::SetFontSize(int pxWidth, int pxHeight)
{

	if (mFont)
		delete mFont;

	mFont = new Font("../Resource/Fonts/arial.ttf", pxWidth, pxHeight);

}






void UITextField::AddCharacter(char c, int pos)
{
	if (!isInFocus)
		return;

	if (mText.size() >= mMaxCharCount)
		return;

	//use cursor pos by default
	if (pos == -1)
		pos = mCursorPos;

	mText.insert(mText.begin() + pos, c);
	mCursorPos++;
}

void UITextField::RemoveCharacter(int pos)
{
	if (!isInFocus)
		return;

	if (mText.size() == 0)
		return;

	//use cursor pos by default
	if (pos == -1 || pos > mText.size())
		pos = mCursorPos;

	mText.erase(mText.begin() + pos - 1);
	mCursorPos--;
}

string UITextField::GetText()
{
	return string(mText.begin(), mText.end());
}



void UITextField::DrawBackground(DWORD* mem, int lpitch32, float timeDelta)
{
	DWORD* tempFrameMem = mBackgroundFrameMem;
	for (int r = 0; r < int(region.getHeight()); r++)
	{
		for (int c = 0; c < region.getWidth(); c++)
			tempFrameMem[c + int(r*region.getWidth())] = bckgColor;
	}

	tempFrameMem = mBackgroundFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempMainMem, tempFrameMem, sizeof(DWORD)*int(region.getWidth()));
		tempMainMem += lpitch32;
		tempFrameMem += int(region.getWidth());
	}
}

void UITextField::DrawForeground(DWORD* mem, int lpitch32, float timeDelta)
{
	ZeroMemory(mForegroundFrameMem, mFrameSize);


	//if string is empty, just draw the cursor
	if (mText.size() == 0 && isInFocus)
	{

		if (mIsCursorOn)
		{
			for (int i = 3; i < region.getHeight()-3; i++)
			{
				mForegroundFrameMem[(mFont->GetFontWidth()) + int(i*region.getWidth())] = COLOR_RED;
			}
		}

	}


	int cellNum = 0;

	DWORD* tFmem = mForegroundFrameMem;
	std::string outStr(mText.begin(), mText.end());
	int cursorOffset = mFont->Draw(mForegroundFrameMem, region.getWidth(), outStr, 0, 5);

	
	//calculate cell offset
	DWORD* cursorMem = mForegroundFrameMem;
	cursorMem += cursorOffset;
	if (mIsCursorOn && isInFocus)
	{
			
		//for (int iy = 0; iy < region.getHeight(); iy++)
		//	cursorMem[(mCellWidthPx - 1) + int(iy*region.getWidth() + 0.5f)] = COLOR_RED;
			
	}





	DWORD borderColor;
	if (stateID == UISTATE_NORMAL)
		borderColor = COLOR_GREY;
	else if (stateID == UISTATE_HOVER)
		borderColor = COLOR_RED;

	//draw border

	DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
	DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
	DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
	DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, region.getHeight() - 1, 0, 0, borderColor, borderColor);



	DWORD* tempFrameMem = mForegroundFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempMainMem, tempFrameMem, sizeof(DWORD)*int(region.getWidth()));
		tempMainMem += lpitch32;
		tempFrameMem += int(region.getWidth());
	}

	mBlinkTimeElapsed += timeDelta;
	if (mBlinkTimeElapsed >= 800.0f)
	{
		mIsCursorOn = !mIsCursorOn;
		mBlinkTimeElapsed = 0.0f;
	}

}

void UITextField::Draw(DWORD* mem, int lpitch32, float timeDelta)
{

	
}

void UITextField::Clear()
{
	mText.clear();
	mCursorPos = 0;
}

UIRegion::UIRegion(int xpos, int ypos, string title, int width, int height, DWORD color)
{
	region.setPos(xpos, ypos);
	region.setWidth(width);
	region.setHeight(height);

	typeID = UITYPE_REGION;
	isInFocus = false;
	isVisible = true;
	stateID = UISTATE_NORMAL;
	depthIndex = DEPTHINDEX_BACKGROUND;
	
	mBorderColor = _RGBA32BIT(255, 255, 255, 255);
	mBckgColor = color;

	mFrameMem = new DWORD[region.getWidth() * region.getHeight()];
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);

	mElementName = title;
}

UIRegion::UIRegion()
{
	
	typeID = UITYPE_REGION;
	isInFocus = false;
	isVisible = true;
	stateID = UISTATE_NORMAL;
	depthIndex = DEPTHINDEX_BACKGROUND;

	mBorderColor = _RGBA32BIT(255, 255, 255, 255);
	mBckgColor = _RGBA32BIT(80, 15, 190, 255);

	mFrameMem = NULL;
	mFrameSize = 0;
}

UIRegion::~UIRegion()
{
	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UIRegion::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	// draw border
	ZeroMemory(mFrameMem, mFrameSize);

	RECT r = region.GetWINRECT();
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),    0, 0, region.getWidth()-1, 0, mBorderColor, mBorderColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(), region.getWidth()-1, 0, region.getWidth()-1, region.getHeight() - 1, mBorderColor, mBorderColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),    region.getWidth()-1, region.getHeight()-1, 0, region.getHeight()-1, mBorderColor, mBorderColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),   0, region.getHeight()-1, 0, 0, mBorderColor, mBorderColor);

	

	int xstart = 1;
	int xend = region.getWidth() - 1 ;
	int y = 1;

	for (y; y < region.getHeight() - 1; y++)
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  xstart, y, xend, y, mBckgColor, mBckgColor);

	//draw the title to new memory
	DWORD* titleFrameMem;
	int titlePitch;
	int titleHeight;
	VECTOR2D titlePos(region.GetPosition().x + region.getWidth() / 2, region.GetPosition().y - 10);
//	Font::RenderTextAt(mem, lpitch32, mElementName, titlePos.x, titlePos.y, 10.0f, COLOR_GREY);


	//blit main frame to screen
	DWORD* tempMem = mFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	VECTOR2D dim(region.getWidth(), region.getHeight());
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < dim.y; i++)
	{
		memcpy(tempMainMem, tempMem, sizeof(DWORD)*int(dim.x));
		tempMainMem += lpitch32;
		tempMem += int(region.getWidth());
	}

	
}

void UIRegion::SetBackgroundColor(DWORD color)
{
	mBckgColor = color;
}

void UIRegion::SetBorderColor(DWORD color)
{
	mBorderColor = color;
}

UIList::UIList()
{
	region = { float(0), float(0), float(0),  float(0) };
	typeID = UITYPE_LIST;
	stateID = UISTATE_NORMAL;
	isInFocus = false;
	isVisible = true;
	mCellHeight = 20;
	isHorizontal = false;
	mFrameMem = NULL;
	mFrameSize = 0;
}

UIList::UIList(int xpos, int ypos, int width)
{
	region = { float(xpos), float(ypos), float(xpos + width),  float(ypos) };
	typeID = UITYPE_LIST;
	stateID = UISTATE_NORMAL;
	isInFocus = false;
	isVisible = true;
	mCellHeight = 20;
	isHorizontal = false;
	mFrameMem = NULL;
	mFrameSize = 0;
}
UIList::~UIList()
{
	for (int i = 0; i < mItemList.size(); i++)
	{
		if (mItemList[i].itemFrame)
			delete[] mItemList[i].itemFrame;
	}

	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UIList::OnHover(int xPos, int yPos) {}

//returns if hit the element or it's child
bool UIList::OnLClick(int mouseX, int mouseY)
{
	return false;
}

bool UIList::OnLUp(int mouseX, int mouseY)
{
	for (int i = 0; i < mItemList.size(); i++)
	{
		if (mItemList[i].region.isPointInRect(mouseX, mouseY))
		{
			mItemList[i].callback();
			return true;
		}

	}

	return false;
}

bool UIList::OnLDown(int mouseX, int mouseY)
{
	if (region.isPointInRect(mouseX, mouseY))
		return true;

	return false;
}

bool UIList::IsInRegion(int mouseX, int mouseY)
{
	return region.isPointInRect(mouseX, mouseY);
}

void UIList::SetPosition(int px, int py)
{
	int xoff = px - region.GetPosition().x;
	int yoff = py - region.GetPosition().y;

	region.setPos(px, py);

	for (int i = 0; i < mItemList.size(); i++)
	{
		VECTOR2D p = mItemList[i].region.GetPosition();
		mItemList[i].region.setPos(p.x + xoff, p.y + yoff);
	}
}


void UIList::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	if (mItemList.size() == 0)
		return;


	ZeroMemory(mFrameMem, mFrameSize);


	DWORD frameColor = COLOR_GREY;
	if (stateID == UISTATE_HOVER)
		frameColor = COLOR_RED;
	else if (stateID == UISTATE_PRESSED)
		frameColor = COLOR_YELLOW;



	//draw dividers and cell innards
	for (int i = 0; i < mItemList.size(); i++)
	{

		//innards
		DWORD* destFrameOffset = mFrameMem;
		int localPosX = mItemList[i].region.GetPosition().x - region.GetPosition().x;
		int localPosY = mItemList[i].region.GetPosition().y - region.GetPosition().y;
		destFrameOffset += int(localPosX + localPosY* region.getWidth());
		DWORD* srcFrameOffset = mItemList[i].itemFrame;
		for (int x = 0; x < mCellHeight; x++)
		{
			memcpy(destFrameOffset, srcFrameOffset, sizeof(DWORD)*mItemList[i].region.getWidth());
			destFrameOffset += int(region.getWidth());
			srcFrameOffset += int(mItemList[i].region.getWidth());

		}


	}

	//draw the main frame
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth()-1, 0, frameColor, frameColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 1, region.getWidth()-1, 1, frameColor, frameColor); //1 pixel frame

	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth()-1, 0, region.getWidth()-1, region.getHeight()-1, frameColor, frameColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth()-2, 0, region.getWidth()-2, region.getHeight()-1, frameColor, frameColor);

	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth()-1, region.getHeight()-1, 0, region.getHeight()-1, frameColor, frameColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth()-1, region.getHeight()-2, 0, region.getHeight()-2, frameColor, frameColor);

	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight()-1, 0, 0, frameColor, frameColor);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  1, region.getHeight()-1, 1, 0, frameColor, frameColor);

	

	//dividers
	for (int i = 0; i < mItemList.size(); i++)
	{
		int cellDivider = (mItemList[i].region.GetPosition().y - region.GetPosition().y) + mCellHeight;
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, cellDivider, region.getWidth(), cellDivider, frameColor, frameColor);
	}



	//draw the final frame
	DWORD* destFrameOffset = mem;
	destFrameOffset += int(region.GetPosition().x + region.GetPosition().y*lpitch32);
	DWORD* srcFrameOffset = mFrameMem;
	for (int x = 0; x < region.getHeight(); x++)
	{
		memcpy(destFrameOffset, srcFrameOffset, sizeof(DWORD)*region.getWidth());
		destFrameOffset += lpitch32;
		srcFrameOffset += int(region.getWidth());

	}





}


void UIList::AddItem(string text, void(*callback)())
{
	item i;
	i.callback = callback;
	i.txt = text;
	i.region = { region.GetPosition().x, region.GetPosition().y + mCellHeight * mItemList.size(), region.GetPosition().x + region.getWidth(), region.GetPosition().y + mCellHeight * mItemList.size() + mCellHeight };
	i.itemFrame = new DWORD[int(i.region.getWidth() * i.region.getHeight()*sizeof(DWORD))];
	RenderTextToRegion(i.itemFrame, i.region.getWidth(), text, i.region.GetWINRECT(), COLOR_BLACK, COLOR_WHITE);
	mItemList.push_back(i);

	// recalculate the main region
	region.setHeight(mCellHeight*mItemList.size());

	//reinstate the main region pointer
	if (mFrameMem)
		delete mFrameMem;

	mFrameMem = new DWORD[region.getWidth() * region.getHeight()];
	mFrameSize = region.getHeight() * region.getWidth() * sizeof(DWORD);
}





UISelectionBox::UISelectionBox()
{
	typeID = UITYPE_SELECTIONBOX;
	stateID = UISTATE_NORMAL;
	isInFocus = false;
	region.setWidth(200);
	region.setHeight(30);
	mFrameSize = int(region.getWidth()) * int(region.getHeight()) * sizeof(DWORD);
	mFrameMem = new DWORD[region.getHeight() * region.getWidth()];
	mItemFrameMem = NULL;
	mItemFrameSize = 0;
	mIsSelectionMade = false;
	mRegionHoverIndex = -1;
	bgColor = COLOR_BLACK;
}

UISelectionBox::~UISelectionBox()
{
	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UISelectionBox::OnHover(int xPos, int yPos)
{
	//check main region first
	if (region.isPointInRect(xPos, yPos))
		stateID = UISTATE_HOVER;
	else
		stateID = UISTATE_NORMAL;

	//then other regions
	mRegionHoverIndex = -1;
	for (int i = 0; i < mItemRegions.size(); i++)
	{
		if (mItemRegions[i].isPointInRect(xPos, yPos))
		{
			mRegionHoverIndex = i;
		}
	}
}

bool UISelectionBox::OnLDown(int xPos, int yPos)
{
	//do nothing if hit the main region (just the title region)
	if (region.isPointInRect(xPos, yPos))
	{
		SetFocus(true);
		return true;
	}

	vector<RECT2D>::iterator vIter = mItemRegions.begin();
	int counter = 0;
	for (vIter, counter; vIter < mItemRegions.end(); vIter++, counter++)
	{
		if (vIter->isPointInRect(xPos, yPos))
		{
			mSelectedTitle = (mItems[counter]);
			mIsSelectionMade = true;
			SetFocus(false);
			return true;
		}


	}
}

bool UISelectionBox::OnLUp(int xPos, int yPos)
{
	//do nothing if hit the main region (just the title region)
	if (region.isPointInRect(xPos, yPos))
		return true;

	return true;
}

void UISelectionBox::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	ZeroMemory(mFrameMem, mFrameSize);
	ZeroMemory(mItemFrameMem, mItemFrameSize);

	if (isInFocus == false)
	{
		DWORD borderColor;
		if (stateID == UISTATE_HOVER)
			borderColor = COLOR_RED;
		else
			borderColor = COLOR_BLUE;

		//draw the title by itself
		RenderTextToRegion(mFrameMem, (int)region.getWidth(), mSelectedTitle, region.GetWINRECT(), bgColor, COLOR_BLUE);

		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() - 1, 0, 0, borderColor, borderColor);


	}
	else
	{
		DWORD borderColor;
		if (stateID == UISTATE_HOVER)
			borderColor = COLOR_RED;
		else
			borderColor = COLOR_BLUE;

		//draw title
		RenderTextToRegion(mFrameMem, (int)region.getWidth(), mSelectedTitle, region.GetWINRECT(), bgColor, COLOR_BLUE);

		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() - 1, 0, 0, borderColor, borderColor);

		//draw the elements
		DWORD* tempItemMem = mItemFrameMem;
		for (int i = 0; i < mItems.size(); i++)
		{
			if (mRegionHoverIndex == i)
				borderColor = COLOR_RED;
			else
				borderColor = COLOR_BLUE;

			RenderTextToRegion(tempItemMem, (int)region.getWidth(), mItems[i], mItemRegions[i].GetWINRECT(), bgColor, COLOR_BLUE);

			DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
			DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
			DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
			DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() - 1, 0, 0, borderColor, borderColor);

			tempItemMem += int(region.getHeight())*int(region.getWidth());
		}


	}

	//blit main frame to screen
	DWORD* tempMem = mFrameMem;
	VECTOR2D pos = region.GetPosition();
	DWORD* tempVidBuffer = mem;
	tempVidBuffer += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*int(region.getWidth()));
		tempVidBuffer += lpitch32;
		tempMem += int(region.getWidth());
	}

	//blit item frame to screen if more then one item
	if (mItems.size() != 0 && isInFocus)
	{
		tempMem = mItemFrameMem;
		pos = mItemRegions[0].GetPosition();
		tempVidBuffer = mem;
		tempVidBuffer += int(pos.x + pos.y*lpitch32);

		for (int item = 0; item < mItems.size(); item++)
		{
			for (int reg = 0; reg < mItemRegions[item].getHeight(); reg++)
			{
				memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*region.getWidth());
				tempVidBuffer += lpitch32;
				tempMem += int(region.getWidth());
			}
		}

	}

}

void UISelectionBox::SetTitle(string title)
{
	mSelectedTitle = title;
	mIsSelectionMade = false;
}

void UISelectionBox::SetFocus(bool state)
{
	isInFocus = state;
}

std::string UISelectionBox::GetSelectedTitle() const
{
	return mSelectedTitle;
}

void UISelectionBox::AddItem(string name)
{
	mItems.push_back(name);
	RECT2D reg;
	reg.setPos(region.GetPosition().x, region.GetPosition().y + mItems.size()*region.getHeight());
	reg.setHeight(region.getHeight());
	reg.setWidth(region.getWidth());
	mItemRegions.push_back(reg);

	//re allocate space
	if (mItemFrameMem)
	{
		free(mItemFrameMem);
		mItemFrameMem = NULL;
	}

	mItemFrameMem = (DWORD*)malloc(mFrameSize*mItems.size());
	mItemFrameSize = mFrameSize*mItems.size();


}

UIDropdownMenu::UIDropdownMenu()
{
	typeID = UITYPE_DROPDOWNMENU;
	stateID = UISTATE_NORMAL;
	isInFocus = false;
	mFrameSize = 0;
	mItemFrameMem = NULL;
	mItemFrameSize = 0;
	mRegionHoverIndex = -1;
	bgColor = COLOR_BLACK;
	mTitle = "";
	mFont = new Font("../Resource/Fonts/arial.ttf", 0, 10);
}

UIDropdownMenu::UIDropdownMenu(int xPos, int yPos, string title) :UIDropdownMenu()
{
	
	region.setPos(xPos, yPos);
	region.setWidth(70);
	region.setHeight(20);
	mFrameSize = int(region.getWidth()) * int(region.getHeight()) * sizeof(DWORD);
	mForegroundFrameMem = new DWORD[region.getWidth() * region.getHeight()];
	mBackgroundFrameMem = new DWORD[region.getWidth() * region.getHeight()];

	mTitle = title;
}

UIDropdownMenu::~UIDropdownMenu()
{


	if (mItemFrameMem)
	{
		delete mItemFrameMem;
		mItemFrameMem = NULL;
	}

	if (mForegroundFrameMem)
	{
		delete mForegroundFrameMem;
		mForegroundFrameMem = NULL;
	}

	if (mBackgroundFrameMem)
	{
		delete mBackgroundFrameMem;
		mBackgroundFrameMem = NULL;
	}


}

void UIDropdownMenu::OnHover(int xPos, int yPos)
{
	//check main region first
	if (region.isPointInRect(xPos, yPos))
		stateID = UISTATE_HOVER;
	else
		stateID = UISTATE_NORMAL;

	//then other regions
	mRegionHoverIndex = -1;
	for (int i = 0; i < mItemRegions.size(); i++)
	{
		if (mItemRegions[i].isPointInRect(xPos, yPos))
		{
			mRegionHoverIndex = i;
		}
	}
}
bool UIDropdownMenu::OnLDown(int xPos, int yPos)
{
	//do nothing if hit the main region (just the title region)
	if (region.isPointInRect(xPos, yPos))
	{
		SetFocus(true);
		return true;
	}

	vector<RECT2D>::iterator vIter = mItemRegions.begin();
	int counter = 0;
	for (vIter, counter; vIter < mItemRegions.end(); vIter++, counter++)
	{
		if (vIter->isPointInRect(xPos, yPos))
		{
			mItems[counter].callback();
			SetFocus(false);
			return true;
		}


	}
}
bool UIDropdownMenu::OnLUp(int xPos, int yPos)
{
	//do nothing if hit the main region (just the title region)
	if (region.isPointInRect(xPos, yPos))
		return true;

	return true;
}
void UIDropdownMenu::DrawForeground(DWORD * mem, int lpitch32, float timeDelta)
{
	ZeroMemory(mForegroundFrameMem, mFrameSize);
	ZeroMemory(mItemFrameMem, mItemFrameSize);

	if (isInFocus == false)
	{
		DWORD borderColor;
		if (stateID == UISTATE_HOVER)
			borderColor = COLOR_RED;
		else
			borderColor = COLOR_BLUE;

		DWORD col = COLOR_RED;
		mFont->Draw(mForegroundFrameMem, (int)region.getWidth(), mTitle, int(((region.getWidth() - mFont->GetStringPixelLength(mTitle)) / 2.0) + 0.5f), 5, &col);


		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, region.getHeight() - 1, 0, 0, borderColor, borderColor);


	}
	else
	{
		DWORD borderColor;
		if (stateID == UISTATE_HOVER)
			borderColor = COLOR_RED;
		else
			borderColor = COLOR_BLUE;

		DWORD text_col = COLOR_RED;
		mFont->Draw(mForegroundFrameMem, (int)region.getWidth(), mTitle, int(((region.getWidth() - mFont->GetStringPixelLength(mTitle)) / 2.0) + 0.5f), 5, &text_col);


		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
		DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, region.getHeight() - 1, 0, 0, borderColor, borderColor);


		//draw the elements
		DWORD* tempItemMem = mItemFrameMem;
		for (int i = 0; i < mItems.size(); i++)
		{
			if (mRegionHoverIndex == i)
				borderColor = COLOR_RED;
			else
				borderColor = COLOR_BLUE;



			mFont->Draw(tempItemMem, (int)region.getWidth(), mItems[i].title, int(((region.getWidth() - mFont->GetStringPixelLength(mItems[i].title)) / 2.0) + 0.5f), 5, &text_col);


			DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, 0, region.getWidth() - 1, 0, borderColor, borderColor);
			DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, borderColor, borderColor);
			DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, borderColor, borderColor);
			DrawLine(mForegroundFrameMem, region.getWidth(), region.getHeight(), 0, region.getHeight() - 1, 0, 0, borderColor, borderColor);


			tempItemMem += int(region.getHeight())*int(region.getWidth());
		}


	}

	//blit main frame to screen
	DWORD* tempMem = mForegroundFrameMem;
	DWORD* tempVidBuffer = mem;

	VECTOR2D pos = region.GetPosition();
	tempVidBuffer += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*int(region.getWidth()));

		tempVidBuffer += lpitch32;
		tempMem += int(region.getWidth());
	}

	//blit item frame to screen if more then one item
	if (mItems.size() != 0 && isInFocus)
	{
		tempMem = mItemFrameMem;
		pos = mItemRegions[0].GetPosition();
		tempVidBuffer = mem;
		tempVidBuffer += int(pos.x + pos.y*lpitch32);

		for (int item = 0; item < mItems.size(); item++)
		{
			for (int reg = 0; reg < mItemRegions[item].getHeight(); reg++)
			{
				memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*region.getWidth());
				tempVidBuffer += lpitch32;
				tempMem += int(region.getWidth());
			}
		}

	}

}

void UIDropdownMenu::DrawBackground(DWORD * mem, int lpitch32, float timeDelta)
{
	ZeroMemory(mBackgroundFrameMem, mFrameSize);

	//fill in main background with color
	for (int r = 0; r < region.bottom - region.top; r++)
		for (int c = 0; c < region.right - region.left; c++)
			mBackgroundFrameMem[int(c + r*(region.getWidth()))] = bgColor;

	if (isInFocus)
	{
		//clear elements
		ZeroMemory(mItemFrameMem, mItemFrameSize);

		//fill in element backgrounds
		DWORD* tempItemMem = mItemFrameMem;
		for (int i = 0; i < mItems.size(); i++)
		{
			for (int r = 0; r < region.bottom - region.top; r++)
				for (int c = 0; c < region.right - region.left; c++)
					tempItemMem[int(c + r*(region.getWidth()))] = bgColor;

			tempItemMem += int(region.getHeight())*int(region.getWidth());
		}
	}

	//blit the main section and elements to screen
	DWORD* tempMem = mBackgroundFrameMem;
	DWORD* tempVidBuffer = mem;

	VECTOR2D pos = region.GetPosition();
	tempVidBuffer += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*int(region.getWidth()));

		tempVidBuffer += lpitch32;
		tempMem += int(region.getWidth());
	}


	//blit item frame to screen if more than one item
	if (mItems.size() != 0 && isInFocus)
	{
		tempMem = mItemFrameMem;
		pos = mItemRegions[0].GetPosition();
		tempVidBuffer = mem;
		tempVidBuffer += int(pos.x + pos.y*lpitch32);

		for (int item = 0; item < mItems.size(); item++)
		{
			for (int reg = 0; reg < mItemRegions[item].getHeight(); reg++)
			{
				memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*region.getWidth());
				tempVidBuffer += lpitch32;
				tempMem += int(region.getWidth());
			}
		}

	}
}

void UIDropdownMenu::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	
}
void UIDropdownMenu::SetTitle(string title)
{
	mTitle = title;
}

std::string UIDropdownMenu::GetTitle() const
{
	return mTitle;
}

void UIDropdownMenu::SetFocus(bool state) 
{
	isInFocus = state;
}
void UIDropdownMenu::AddItem(string name, void(*cb)())
{
	_item newItem;
	newItem.title = name;
	newItem.callback = cb;
	mItems.push_back(newItem);

	RECT2D reg;
	reg.setPos(region.GetPosition().x, region.GetPosition().y + mItems.size()*region.getHeight());
	reg.setHeight(region.getHeight());
	reg.setWidth(region.getWidth());
	mItemRegions.push_back(reg);

	//re allocate space
	if (mItemFrameMem)
	{
		delete mItemFrameMem;
		mItemFrameMem = NULL;
	}

	mItemFrameMem = new DWORD[region.getWidth() * region.getHeight() *mItems.size()];
	mItemFrameSize = mFrameSize*mItems.size();

}

UICheckBox::UICheckBox()
{
	typeID = UITYPE_CHECKBOX;
	stateID = UISTATE_NORMAL;
	region.setWidth(30.0f);
	region.setHeight(30.0f);
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);
	mFrameMem = new DWORD[region.getHeight() * region.getWidth()];
	memset(mFrameMem, 0, mFrameSize);
	mIsChecked = false;
}

UICheckBox::~UICheckBox()
{
	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UICheckBox::OnHover(int xPos, int yPos)
{
	if (region.isPointInRect(xPos, yPos))
		stateID = UISTATE_HOVER;
	else
		stateID = UISTATE_NORMAL;
}

bool UICheckBox::OnLUp(int xPos, int yPos)
{
	if (region.isPointInRect(xPos, yPos))
	{
		mIsChecked = !mIsChecked;
		return true;
	}

	return false;
}

void UICheckBox::SetCheckState(bool state)
{
	mIsChecked = state;
}

bool UICheckBox::IsChecked() const
{
	return mIsChecked;
}



UIRadioGroup::UIRadioGroup()
{
	typeID = UITYPE_RADIO;
	stateID = UISTATE_NORMAL;
	mButtonDiameter = 20;
	region.setWidth(mButtonDiameter);
	region.setHeight(mButtonDiameter);
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);
	mFrameMem = new DWORD[region.getHeight() * region.getWidth()];
	memset(mFrameMem, 0, mFrameSize);
	mButtonSpacing = 10;
	mBttnHoverIndex = -1;
	mSelectedButton = 0;
}

UIRadioGroup::~UIRadioGroup()
{
	vector<DWORD*>::iterator vIter = mButtonsMem.begin();
	for (vIter; vIter < mButtonsMem.end(); vIter++)
	{
		if ((*vIter))
		{
			delete (*vIter);

		}
	}

	mButtonsMem.clear();

	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

bool UIRadioGroup::OnLUp(int xPos, int yPos)
{

	//check the main button first
	if (region.isPointInRect(xPos, yPos))
	{
		mSelectedButton = 0;
		return true;
	}

	//then the other buttons
	for (int i = 0; i < mButtons.size(); i++)
	{
		if (mButtons[i].isPointInRect(xPos, yPos))
		{
			mSelectedButton = i + 1;
			return true;
		}
	}

	return false;
}

void UIRadioGroup::OnHover(int xPos, int yPos)
{
	//normal by default
	stateID = UISTATE_NORMAL;

	//check the main button first
	if (region.isPointInRect(xPos, yPos))
		stateID = UISTATE_HOVER;

	//then the other buttons
	mBttnHoverIndex = -1;
	for (int i = 0; i < mButtons.size(); i++)
	{
		if (mButtons[i].isPointInRect(xPos, yPos))
		{
			mBttnHoverIndex = i;
		}
	}
}

void UIRadioGroup::SetDiameter(int diam)
{
	mButtonDiameter = diam;
}

int UIRadioGroup::GetNumButtons() const
{
	return mButtons.size();
}

void UIRadioGroup::AddButton()
{
	RECT2D r;

	if (mButtons.size() > 0)
		r = mButtons[mButtons.size() - 1];
	else
		r = region;

	r.setPos(r.GetPosition().x, r.GetPosition().y + r.getHeight() + mButtonSpacing);
	mButtons.push_back(r);

	//allocate more space to frame
	DWORD* mem = (DWORD*)malloc(mFrameSize);
	mButtonsMem.push_back(mem);
}




UIDropContainer::UIDropContainer()
{
	typeID = UITYPE_DROPCONTAINER;
	stateID = UISTATE_NORMAL;
	region.setWidth(120.0f);
	region.setHeight(300.0f);
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);
	mFrameMem = new DWORD[region.getHeight() * region.getWidth()];
	memset(mFrameMem, 0, mFrameSize);
	m_borderPx = 10;
	m_spacingPx = 17;
	m_numColumns = 1;
	mPickedUpItem = -1;
	mCurrentWindow = GetActiveWindow();
	mUIInstance = NULL;
}

UIDropContainer::~UIDropContainer()
{
	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UIDropContainer::OnHover(int xPos, int yPos)
{
}

void UIDropContainer::AddItem(int RMKey, UINT uitype, void(*func)())
{
	cDropItem item;
	item.actionFunction = func;
	item.image = ImageManager::GetImage(RMKey);
	item.localRegion.setHeight(item.image->GetInfoHeader().biHeight);
	item.localRegion.setWidth(item.image->GetInfoHeader().biWidth);
	item.uiType = uitype;
	item.itemInstance = NULL;

	mItemList.push_back(item);
}


void UIDropContainer::SetUIInstance(UserInterface* inst)
{
	mUIInstance = inst;
}

bool UIDropContainer::OnLDown(int mouseX, int mouseY)
{
	vector<cDropItem>::iterator vIter = mItemList.begin();
	int offset = 0;
	for (vIter; vIter < mItemList.end(); vIter++, offset++)
	{
		if (vIter->localRegion.isPointInRect(mouseX, mouseY))
		{
			mPickedUpItem = offset - 1;
			return true;
		}
	}

	return false;
}


void UIDropContainer::InsertInitializatinData(UINT uiType, vector<void*>& data)
{
	cElemData dta;
	dta.data = data;
	dta.uiType = uiType;
	mInitDataMap[uiType] = dta;
}


void UIDropContainer::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	//reseize region to fit items
	/*	int newWidth, newHeight;
	newWidth = (m_numColumns - 1)*m_spacingPx + 2 * m_borderPx + m_numColumns*mItemList[0].image->GetInfoHeader().biWidth;
	newHeight = m_borderPx * 2 + (int((float(mItemList.size()) / 2.0f) + 0.5f) - 1)*m_spacingPx + (int((float(mItemList.size()) / 2.0f) + 0.5f) - 1)*mItemList[0].image->GetInfoHeader().biHeight;
	if (mFrameMem)
	{
	free(mFrameMem);
	mFrameSize = newWidth * newHeight * sizeof(DWORD);
	mFrameMem = (DWORD*)malloc(mFrameSize);
	}

	region.setWidth(newWidth);
	region.setHeight(newHeight);
	*/
	memset(mFrameMem, 0, mFrameSize);



	DWORD colorState = 0;
	if (stateID == UISTATE_NORMAL)
		colorState = COLOR_BLUE;
	else if (stateID == UISTATE_HOVER)
		colorState = COLOR_RED;

	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth() - 1, 0, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() - 1, 0, 0, colorState, colorState);

	//draw items
	DWORD* tempFrameMem;
	vector<cDropItem>::iterator vIter = mItemList.begin();
	int x = m_borderPx;
	int y = m_borderPx;
	int colNum = 1;
	for (vIter; vIter < mItemList.end(); vIter++)
	{
		tempFrameMem = mFrameMem;
		DWORD* bitmapMem = (DWORD*)vIter->image->GetData();
		uint imgWidth = vIter->image->GetInfoHeader().biWidth;
		uint imgHeight = vIter->image->GetInfoHeader().biHeight;
		VECTOR2D pos = VECTOR2D(x, y);
		vIter->localRegion.setPos(pos.x, pos.y);

		//calculate position
		tempFrameMem += int(pos.x) + int(pos.y)*int(region.getWidth());

		for (uint row = 0; row < imgHeight; row++)
		{
			memcpy(tempFrameMem, bitmapMem, sizeof(DWORD)*imgWidth); //scan row by row
			tempFrameMem += int(region.getWidth());
			bitmapMem += imgWidth;
		}

		x += m_spacingPx + imgWidth;
		colNum++;

		if (colNum > m_numColumns)
		{
			colNum = 1;
			x = m_borderPx;
			//if (y + m_spacingPx < region.getHeight())
			y += m_spacingPx + imgHeight;// +imgHeight;
		}


	}


	//blit frame to screen
	tempFrameMem = mFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempMainMem, tempFrameMem, sizeof(DWORD)*int(region.getWidth()));
		tempMainMem += lpitch32;
		tempFrameMem += int(region.getWidth());
	}

	//item is picked up
	if (mPickedUpItem != -1)
	{
		DWORD* itemIconMem = (DWORD*)mItemList[mPickedUpItem].image->GetData();
		DWORD* tempMainMem = mem;
		POINT cursorPos;
		POINT clientPos = { 0, 0 };
		RECT clRct;
		GetCursorPos(&cursorPos);
		ClientToScreen(mCurrentWindow, &clientPos);
		cursorPos.x -= clientPos.x;
		cursorPos.y -= clientPos.y;
		tempMainMem += cursorPos.x + cursorPos.y*lpitch32;
		int itemHeight = mItemList[mPickedUpItem].image->GetInfoHeader().biHeight;
		int itemWidth = mItemList[mPickedUpItem].image->GetInfoHeader().biWidth;
		for (int i = 0; i < itemHeight; i++)
		{
			memcpy(tempMainMem, itemIconMem, sizeof(DWORD)*itemWidth);
			tempMainMem += lpitch32;
			itemIconMem += itemWidth;
		}
	}
}

UIElement* UIDropContainer::GetItemInstance(int id)
{
	return mItemList[id].itemInstance;
}


UIWindow::UIWindow()
{
	typeID = UITYPE_WINDOW;
	stateID = UISTATE_NORMAL;
	titleBarWidth = 0;
	mBackgroundColor = COLOR_BLACK;
	mFrameSize = 0;
	mForegroundFrameMem = NULL;
	mBackgroundFrameMem = NULL;
	mTitle = "";
	mTitleBarColor = _RGBA32BIT(80, 12, 200, 255);
	mIsDragged = false;
	dragOffset = { 0,0 };

}

UIWindow::UIWindow(int xpos, int ypos, int width, int height, string title, DWORD color) : UIWindow()
{
	region.setPos(xpos, ypos);
	region.setWidth(width);
	region.setHeight(height);
	titleBarWidth = int(height*0.1 + 0.5f);
	mBackgroundColor = color;
	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);
	mForegroundFrameMem = new DWORD[region.getWidth() * region.getHeight()];
	mBackgroundFrameMem = new DWORD[region.getWidth() * region.getHeight()];
	memset(mForegroundFrameMem, 0, mFrameSize);
	memset(mBackgroundFrameMem, 0, mFrameSize);
	mTitle = title;
}
UIWindow::~UIWindow()
{
	if (mForegroundFrameMem)
		delete mForegroundFrameMem;

	if (mBackgroundFrameMem)
		delete mBackgroundFrameMem;

	mForegroundFrameMem = mBackgroundFrameMem = NULL;

}


void UIWindow::OnHover(int xPos, int yPos)
{
	if (mIsDragged)
	{
		VECTOR2D previousPos = region.GetPosition();
		region.setPos(xPos - dragOffset.x, yPos - dragOffset.y);
		VECTOR2D posDelta = region.GetPosition() - previousPos;

		vector<UIElement*>::iterator vIter;
		for (vIter = mChildren.begin(); vIter < mChildren.end(); vIter++)
		{
			VECTOR2D chpos = (*vIter)->GetPosition();
			(*vIter)->SetPosition(chpos.x + posDelta.x, chpos.y + posDelta.y);

		}
	}
}

bool UIWindow::OnLClick(int mouseX, int mouseY)
{
	return false;
}

bool UIWindow::OnLUp(int mouseX, int mouseY) 
{
	if (mIsDragged)
		mIsDragged = false;

	return false;
}

bool UIWindow::OnLDown(int mouseX, int mouseY)
{
	RECT2D r = region;
	r.setHeight(titleBarWidth);

	if (r.isPointInRect(mouseX, mouseY))
	{
		dragOffset = { mouseX - (int)region.GetPosition().x, mouseY - (int)region.GetPosition().y };
		mIsDragged = true;
		return true;
	}

	return false;
}


void UIWindow::DrawForeground(DWORD* mem, int lpitch32, float timeDelta)
{


}

void UIWindow::DrawBackground(DWORD* mem, int lpitch32, float timeDelta)
{
	ZeroMemory(mBackgroundFrameMem, mFrameSize);
	for (int i = 0; i < region.getHeight(); i++)
	{
		if (i < titleBarWidth)
			DrawLine(mBackgroundFrameMem, region.getWidth(), region.getHeight(), 0, i, region.getWidth() - 1, i, mTitleBarColor, mTitleBarColor);
		else
			DrawLine(mBackgroundFrameMem, region.getWidth(), region.getHeight(), 0, i, region.getWidth() - 1, i, mBackgroundColor, mBackgroundColor);

	}

	RECT r = region.GetWINRECT();
	r.bottom = r.top + titleBarWidth;
	RenderTextToRegion(mBackgroundFrameMem, region.getWidth(), mTitle, r, mTitleBarColor, COLOR_RED);

	//blit to screen
	DWORD* tempMem = mBackgroundFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	VECTOR2D dim(region.getWidth(), region.getHeight());
	tempMainMem += int(pos.x + pos.y*lpitch32);


	for (int i = 0; i < dim.y; i++)
	{
		memcpy(tempMainMem, tempMem, sizeof(DWORD)*int(dim.x));
		tempMainMem += lpitch32;
		tempMem += int(region.getWidth());
	}
}

void UIWindow::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	
	
	
	
	

}

void UIWindow::AddChild(UIElement* child)
{
	//reposition element relative to window
	child->SetPosition(region.GetPosition().x + child->GetPosition().x, region.GetPosition().y + child->GetPosition().y);

	mChildren.push_back(child);
}

std::string UIWindow::GetTitle() const
{
	return mTitle;
}

DWORD UIWindow::GetBackgroundColor() const
{
	return mBackgroundColor;
}

void UIWindow::SetVisibility(bool state)
{
	isVisible = state;
	vector<UIElement*>::iterator vIter;
	for (vIter = mChildren.begin(); vIter < mChildren.end(); vIter++)
	{
		(*vIter)->SetVisibility(state);

	}
}

UIText::UIText()
{

	typeID = UITYPE_TEXT;
	stateID = UISTATE_NORMAL;
	depthIndex = DEPTHINDEX_FOREGROUND;
	mBackgroundColor = COLOR_BLACK;
	mTextColor = COLOR_WHITE;
	mFrameSize = 0;
	mFrameMem = NULL;
	mText = "";
	mFont = new Font("../Resource/Fonts/arial.ttf", 0, 16);
}

UIText::UIText(int xpos, int ypos, int width, int height, std::string s, DWORD fColor, DWORD bColor): UIText()
{

	region.setPos(xpos, ypos);
	mBackgroundColor = bColor;
	mTextColor = fColor;
	mText = s;

	if (mFont)
		delete mFont;

	mFont = new Font("../Resource/Fonts/arial.ttf", width, height);

	region.setHeight(mFont->GetFontHieght());
	region.setWidth(mFont->GetStringPixelLength(s));

	mFrameSize = region.getWidth() * region.getHeight() * sizeof(DWORD);
	mFrameMem = new DWORD[region.getWidth() * region.getHeight()];

}

UIText::~UIText()
{
	if (mFrameMem)
	{
		delete mFrameMem;
		mFrameMem = NULL;
	}
}

void UIText::Draw(DWORD* mem, int lpitch32, float deltaTime)
{
	
	ZeroMemory(mFrameMem, mFrameSize);

	/*
	for (int r = 0; r < region.bottom - region.top; r++)
		for (int c = 0; c < region.right - region.left; c++)
			mFrameMem[int(c + r*(region.getWidth()))] = mBackgroundColor;*/

	//RenderTextToRegion(mFrameMem, region.getWidth(), mText, region.GetWINRECT(), mBackgroundColor, mTextColor);
	mFont->Draw(mFrameMem, (int)region.getWidth(), mText, 0, 0, &mTextColor);



	//blit main frame to screen
	DWORD* tempMem = mFrameMem;
	DWORD* tempVidBuffer = mem;

	VECTOR2D pos = region.GetPosition();
	tempVidBuffer += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempVidBuffer, tempMem, sizeof(DWORD)*int(region.getWidth()));

		tempVidBuffer += lpitch32;
		tempMem += int(region.getWidth());
	}

}


void UIText::SetText(std::string s)
{
	mText = s;
}

std::string UIText::GetText()
{
	return mText;
}


bool UserInterface::CheckLastProcessEventResults(int code)
{
	auto vIter = lastEventResultList.begin();
	for (;vIter < lastEventResultList.end(); vIter++)
	{
		if (*vIter == code)
			return true;
	}

	return false;
}

void UserInterface::ProcessEvent(UINT msg, WPARAM wparam, LPARAM lparam)
{
	lastEventResultList.clear();

	switch (msg)
	{

	case WM_LBUTTONDOWN:
	{
		float xPos = GET_X_LPARAM(lparam);
		float yPos = GET_Y_LPARAM(lparam);

		mIsLButtonDown = true;
		LARGE_INTEGER timestmp;
		QueryPerformanceCounter(&timestmp);
		m_LMouseDownTimeStamp = timestmp.QuadPart;
		mLMouseDownPosition = VECTOR2D(xPos, yPos);

		int elmOffset = 0;
		bool isElementHit = false;
		vector<UIElement*>::iterator vIter;

		//check if any gui element is hit
		for (vIter = mUIElements.begin(); vIter < mUIElements.end(); vIter++, elmOffset++)
		{
			//make sure the element is visible and enabled
			if ((*vIter)->GetVisibility() && (*vIter)->GetEnabledState())
			{
				//on hit, do this
				if ((*vIter)->OnLDown(xPos, yPos))
				{
					//if clicked on element not previously focused
					if (elmOffset != mElementInFocus)
					{

						RemoveFocus();
						(*vIter)->SetFocus(true);
						mElementInFocus = elmOffset;
					}


					isElementHit = true;
					break;
				}
			}

		}

		//nothing is hit
		if (isElementHit == false)
		{
			RemoveFocus();
			lastEventResultList.push_back(GUI_MOUSEDOWN_NOHIT);	
		}
		else
			lastEventResultList.push_back(GUI_MOUSEDOWN_HIT);

	}break;

	case WM_LBUTTONUP:
	{
		float xPos = GET_X_LPARAM(lparam);
		float yPos = GET_Y_LPARAM(lparam);

		mIsLButtonDown = false;
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		double nowCount = t.QuadPart;
		double countDifference = nowCount - m_LMouseDownTimeStamp;


		if (mElementInFocus != -1) //something has focus
		{
			//let the previously focused element handle the up click
			mUIElements[mElementInFocus]->OnLUp(xPos, yPos);

		}
		else
		{

			/*
			//see if anytyhing is hit on up click
			int elmOffset = 0;
			bool isElementHit = false;
			vector<UIElement*>::iterator vIter;
			for (vIter = mUIElements.begin(); vIter < mUIElements.end(); vIter++, elmOffset++)
			{

			//if hit, do this
			if ((*vIter)->OnLUp(xPos, yPos))
			{
			//clicked on previously focused element
			if (elmOffset == mElementInFocus)
			{
			//	(*vIter)->isInFocus = false;
			//		mElementInFocus = -1;
			}
			else //if (focusable)
			{
			RemoveFocus();
			(*vIter)->isInFocus = true;
			mElementInFocus = elmOffset;
			}
			//else //clicked on unfocusable element
			//	RemoveFocus();

			isElementHit = true;
			break;
			}

			}

			//nothing was hit
			if (isElementHit == false)
			{

			RemoveFocus();

			return GUI_MOUSEUP_NOHIT;
			}

			return GUI_MOUSEUP_HIT;
			*/
		}

	}break;

	case WM_MOUSEMOVE:
	{
		float xPos = GET_X_LPARAM(lparam);
		float yPos = GET_Y_LPARAM(lparam);

		mousePos = { (LONG)xPos, (LONG)yPos };
		vector<UIElement*>::iterator vIter;
		for (vIter = mUIElements.begin(); vIter < mUIElements.end(); vIter++)
		{
			//make sure the element is visible and enabled
			if ((*vIter)->GetVisibility() && (*vIter)->GetEnabledState())
				(*vIter)->OnHover(xPos, yPos);

		}

	}break;


	case WM_KEYDOWN:
	{

		//if element has focus, filter for text characters and send to element
		if (mElementInFocus != -1)
		{
			//make sure the element is enabled and visible
			if (!(mUIElements[mElementInFocus]->GetVisibility() && mUIElements[mElementInFocus]->GetEnabledState()))
				return;

			//virtual key codes match directly to ascii table
			bool isNumber = wparam >= 0x30 && wparam <= 0x39;
			bool isLetter = wparam >= 0x41 && wparam <= 0x5A;
			bool isSpace = wparam == 32;

			if (isNumber || isLetter || isSpace)
				if (mUIElements[mElementInFocus]->GetType() == UITYPE_TEXTFIELD)
				{
					UITextField* tfld = (UITextField*)mUIElements[mElementInFocus];
					tfld->AddCharacter(wparam);
				}

			if (wparam == VK_BACK)
			{
				UITextField* tfld = (UITextField*)mUIElements[mElementInFocus];
				tfld->RemoveCharacter();
			}
		}


		if (wparam == 0xC0)
		{
			isUIVisible = !isUIVisible;


		}


	}


	default: break;
	}


}

UIButton* UserInterface::createButton(void(*lbCallback)(), int xPos, int yPos, string text, int width, int height)
{

	UIButton* pBttn = new UIButton(width, height);
	pBttn->SetPosition(xPos, yPos);
	pBttn->SetText(text);
	pBttn->SetOnLClickCallback(lbCallback);
	pBttn->SetDepthIndex(DEPTHINDEX_FOREGROUND);

	UIElement* e = (UIElement*)pBttn;
	mUIElements.push_back(e);

	return pBttn;
}
UITextField* UserInterface::createTextField(void(*tfCallback)(), int xPos, int yPos, int width, int height, int maxcharcount)
{
	UITextField* pTxtField = new UITextField(xPos, yPos, width, height, maxcharcount) ;
	pTxtField->SetOnLClickCallback(tfCallback);
	pTxtField->SetDepthIndex(DEPTHINDEX_FOREGROUND);
	UIElement* e = (UIElement*)pTxtField;
	mUIElements.push_back(e);
	return pTxtField;

}

UISelectionBox* UserInterface::createSelectionBox(int xPos, int yPos, string title)
{
	UISelectionBox* sb = new UISelectionBox;
	sb->SetPosition(xPos, yPos);
	sb->SetTitle(title);
	sb->SetDepthIndex(DEPTHINDEX_FOREGROUND);

	//downcast from selection box to element
	UIElement* e = (UIElement*)sb;

	mUIElements.push_back(e);
	return sb;
}

UICheckBox* UserInterface::createCheckBox(int xPos, int yPos, bool isChecked)
{
	UICheckBox* pChckBox = new UICheckBox;
	pChckBox->SetPosition(xPos, yPos);
	pChckBox->SetCheckState(isChecked);
	pChckBox->SetDepthIndex(DEPTHINDEX_FOREGROUND);

	UIElement* e = (UIElement*)pChckBox;
	mUIElements.push_back(e);

	return pChckBox;

}


UIRadioGroup* UserInterface::createRadioGroup(int xPos, int yPos, int numButtons)
{
	UIRadioGroup* pRadio = new UIRadioGroup;
	pRadio->SetDiameter(20);
	pRadio->SetPosition(xPos, yPos);
	pRadio->SetDepthIndex(DEPTHINDEX_FOREGROUND);

	for (int i = 0; i < numButtons; i++)
	{
		pRadio->AddButton();
	}

	UIElement* e = (UIElement*)pRadio;
	mUIElements.push_back(e);

	return pRadio;
}

UIDropContainer* UserInterface::createDropContainer(int xPos, int yPos)
{
	UIDropContainer* dc = new UIDropContainer;
	dc->SetPosition(xPos, yPos);
	//	dc->AddItem(&mImageData[0], NULL);
	//	dc->AddItem(&mImageData[1], NULL);
	//	dc->AddItem(&mImageData[2], NULL);
	//	dc->AddItem(&mImageData[3], NULL);
	//	dc->AddItem(&mImageData[4], NULL);
	dc->SetUIInstance(this);
	dc->SetDepthIndex(DEPTHINDEX_FOREGROUND);
	UIElement* e = (UIElement*)dc;
	mUIElements.push_back(e);
	return dc;

}

UIList* UserInterface::createList(int xPos, int yPos, int width)
{
	UIList* list = new UIList(xPos, yPos, width);
	list->SetDepthIndex(DEPTHINDEX_FOREGROUND);
	mUIElements.push_back((UIElement*)(list));
	return list;
}

UIRegion* UserInterface::createRegion(int xPos, int yPos, string title, int width, int height, DWORD color)
{
	UIRegion * reg = new UIRegion(xPos, yPos, title, width, height, color);
	mUIElements.push_back((UIElement*)reg);

	return reg;
}

UIDropdownMenu* UserInterface::createDropdownMenu(int xPos, int yPos, string title)
{
	UIDropdownMenu* menu = new UIDropdownMenu(xPos, yPos, title);
	mUIElements.push_back((UIElement*)menu);
	return menu;
}

UIWindow* UserInterface::createWindow(int xPos, int yPos, int width, int height, string Title, DWORD color)
{
	UIWindow* win = new UIWindow(xPos, yPos, width, height, Title, color);
	mUIElements.push_back((UIElement*)win);
	return win;
}

UIText* UserInterface::createText(int xPos, int yPos, int charWidth, int charHeight, string text, DWORD fColor, DWORD bColor)
{
	UIText* newText = new UIText(xPos, yPos, charWidth, charHeight, text, fColor, bColor);
	mUIElements.push_back((UIElement*)newText);
	return newText;
}

bool UserInterface::isLMD()
{
	return mIsLButtonDown;
}

POINT UserInterface::getLMDPosition()
{
	POINT p;
	p.x = (int)mLMouseDownPosition.x;
	p.y = (int)mLMouseDownPosition.y;

	return p;
}




void UserInterface::RemoveFocus()
{
	//remove previous focus
	if (mElementInFocus != -1)
	{
		mUIElements[mElementInFocus]->SetFocus(false);
		mElementInFocus = -1;
	}
}







bool UserInterface::UIHasFocus() const
{
	//only gives ui focus to text field 
	for (int i = 0; i < mUIElements.size(); i++)
		if (mUIElements[i]->GetType() == UITYPE_TEXTFIELD)
		{
			if(mUIElements[i]->GetFocus())
				return true;
		}

	return false;
}


void UserInterface::DrawUI(float deltaTime)
{
	//set the curent resource view in the shader
	pForegroundSV->SetResource(pForegroundSRV);
	pBackgroundSV->SetResource(pBackgroundSRV);

	UINT stride = sizeof(DX10VERTEX);
	UINT offset = 0;

	ID3D10Buffer* lastVB, *lastIB;
	DXGI_FORMAT ibFormat;
	//save old buffers
	lro->pD3D10Device->IAGetVertexBuffers(0, 1, &lastVB, &stride, &offset);
	lro->pD3D10Device->IAGetIndexBuffer(&lastIB, &ibFormat, &offset);

	//set new ones
	lro->pD3D10Device->IASetVertexBuffers(0, 1, &pD3D10VertexBuffer, &stride, &offset);
	lro->pD3D10Device->IASetIndexBuffer(pD3D10IndexBuffer, DXGI_FORMAT_R32_UINT, offset);


	D3D10_MAPPED_TEXTURE2D mappedTex;
	DWORD* texture_buffer;

	//first draw background elements
	backgroundTexture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);

	texture_buffer = (DWORD*)mappedTex.pData;
	ZeroMemory(texture_buffer, SoftwareRasterizer.clientRect.bottom * mappedTex.RowPitch);

	int lpitch32 = mappedTex.RowPitch >> 2;;

	vector<UIElement*>::iterator vIter;
	int elmOffset = 0;

	for (vIter = mUIElements.begin(); vIter < mUIElements.end(); vIter++, elmOffset)
	{
		if ((*vIter)->GetVisibility())
		{
			if((*vIter)->GetDepthIndex() == DEPTHINDEX_BACKGROUND)
				(*vIter)->Draw(texture_buffer, lpitch32, deltaTime);

			(*vIter)->DrawBackground(texture_buffer, lpitch32, deltaTime);


		}
	}

	backgroundTexture->Unmap(D3D10CalcSubresource(0, 0, 1));


	//now draw foreground
	foregroundTexture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);

	texture_buffer = (DWORD*)mappedTex.pData;
	ZeroMemory(texture_buffer,SoftwareRasterizer.clientRect.bottom * mappedTex.RowPitch);

	lpitch32 = mappedTex.RowPitch >> 2;;

	for (vIter = mUIElements.begin(); vIter < mUIElements.end(); vIter++, elmOffset)
	{
		if ((*vIter)->GetVisibility())
		{
			if ((*vIter)->GetDepthIndex() == DEPTHINDEX_FOREGROUND)
				(*vIter)->Draw(texture_buffer, lpitch32, deltaTime);

			(*vIter)->DrawForeground(texture_buffer, lpitch32, deltaTime);
		}
	}

	foregroundTexture->Unmap(D3D10CalcSubresource(0, 0, 1));

	//apply backgrounds and foreground passes
	lro->pD3D10EffectTechnique->GetPassByName("UIBackgroundPass")->Apply(0);
	lro->pD3D10Device->DrawIndexed(6, 0, 0);


	lro->pD3D10EffectTechnique->GetPassByName("UIForegroundPass")->Apply(0);
	lro->pD3D10Device->DrawIndexed(6, 0, 0);


	//replace old buffers
	lro->pD3D10Device->IASetVertexBuffers(0, 1, &lastVB, &stride, &offset);
	lro->pD3D10Device->IASetIndexBuffer(lastIB, ibFormat, offset);

}

UIElement* UserInterface::GetHitElement(int x, int y)
{
	auto vIter = mUIElements.begin();
	for (vIter; vIter != mUIElements.end(); vIter++)
		if ((*vIter)->IsInRegion(x, y))
			return *vIter;;
	
	return 0;
}

char* UserInterface::GetCharacterBitmap(char c)
{
	return getCharBitmap(c);
}

POINT UserInterface::GetMousePosition() const
{
	return mousePos;
}

UIElement* UserInterface::GetElementByName(std::string name)
{
	vector<UIElement*>::iterator vIter;
	for (vIter = mUIElements.begin(); vIter < mUIElements.end(); vIter++)
	{
		if ((*vIter)->GetName() == name)
			return (*vIter);
	}
}

UIElement* UserInterface::GetElement(int offset)
{
	return *(mUIElements.begin() + offset);
}

int UserInterface::GetNumberOfElements()
{
	return mUIElements.size();
}

void UserInterface::sortElementsByDepth()
{
	vector<UIElement*> sortedElm;
	UIElement* t = NULL;
	for (int i = 1; i < mUIElements.size() - 1; i++)
	{
		t = mUIElements[i];
		int j = i;
		while ((j > 0) && (mUIElements[j - 1]->GetDepthIndex() > t->GetDepthIndex()))
		{
			mUIElements[j] = mUIElements[j - 1];
			j--;
		}
		mUIElements[j] = t;
	}
}

void UserInterface::Clear()
{
	mUIElements.clear();
}


void RenderTextToRegion(DWORD* buffer, int lpitch32, string text, RECT region, DWORD bgColor, DWORD color)
{
	for (int r = 0; r < region.bottom - region.top; r++)
		for (int c = 0; c < region.right - region.left; c++)
			buffer[c + r*(region.right - region.left)] = bgColor;

	//how many characters to draw
	int numChars = text.size();
	int numSpaces = numChars - 1;

	//null string, break
	if (numChars == 0)
		return;

	int pxBorder = (region.bottom - region.top - 10) / 2;
	int pxCharSpacing = 2;

	//how many pixels in width per charactermBufferPitch
	int regWidth = region.right - region.left - pxBorder * 2;
	int regHeight = region.bottom - region.top - pxBorder * 2;
	int regHalfHeight = int((float(regHeight) / 2.0f) + 0.5f);
	int regThreeFourthHeight = int((float(regHeight) * 3.0f / 4.0f) + 0.5f);
	int regQuarterHeight = int((float(regHeight) / 4.0f) + 0.5f);
	int pxCharWidth = int((float(regWidth - pxCharSpacing*numSpaces) / float(numChars)) + 0.5f);
	int pxHalfChar = int((float(pxCharWidth) / 2.0f) + 0.5f);
	int pxQuarterChar = int((float(pxCharWidth) / 4.0f) + 0.5f);
	int pxThreeFourthChar = int((float(pxCharWidth) * 3.0f / 4.0f) + 0.5f);


	string::iterator sIter = text.begin();
	int cursorPos = pxBorder;
	for (sIter; sIter < text.end(); sIter++)
	{
		if (*sIter == 'A')
		{
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxHalfChar, pxBorder, color, color);
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//	DrawLine(buffer,lpitch32, regHeight, xOffset + (width / 2) - 2, yOffset + (height / 2), xOffset + (width / 2) + 2, yOffset + (height / 2), color, color);
		}
		else if (*sIter == 'B')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos , pxBorder, cursorPos , regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos , pxBorder, cursorPos  + pxThreeFourthChar, pxBorder, color, color);
			//mid horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos , regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos , regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//top cell vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxThreeFourthChar, pxBorder, cursorPos + pxThreeFourthChar, regHalfHeight + pxBorder, color, color);
			//bottom cell vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);


		}
		else if (*sIter == 'C')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top cell vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//bottom cell vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'D')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxThreeFourthChar, pxBorder, color, color);
			//bottom horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxThreeFourthChar, regHeight + pxBorder, color, color);
			//top chord
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxThreeFourthChar, pxBorder, cursorPos + pxCharWidth, regQuarterHeight + pxBorder, color, color);
			//right vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder + regQuarterHeight, cursorPos + pxCharWidth, regHeight + pxBorder - regQuarterHeight, color, color);
			//bottom chord
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxThreeFourthChar, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder - regQuarterHeight, color, color);

		}
		else if (*sIter == 'E')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'F')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'G')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//left vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'H')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'I')
		{
			//vertical stem 
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, pxBorder, cursorPos + pxHalfChar, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'J')
		{
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'K')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top diagonal
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//bottom diagonal
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'L')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'M')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//diag left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxHalfChar, regHalfHeight + pxBorder, color, color);
			//diag right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == 'N')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//diag left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'O')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'P')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'Q')
		{
			//vertical stem
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//bottom horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxThreeFourthChar, regHeight + pxBorder, color, color);
			//right vertical
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//bottom chord
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxThreeFourthChar, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder - regQuarterHeight, color, color);
			
		}
		else if (*sIter == 'R')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//bottom diagonal
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'S')
		{
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxQuarterChar, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//1st chord
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regQuarterHeight + pxBorder, cursorPos + pxQuarterChar, pxBorder, color, color);
			//2nd vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regQuarterHeight + pxBorder, cursorPos, regHalfHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//3rd vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regThreeFourthHeight + pxBorder, color, color);
			//bottom horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxThreeFourthChar, regHeight + pxBorder, color, color);
			//bottom chord
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxThreeFourthChar, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder - regQuarterHeight, color, color);

		}
		else if (*sIter == 'T')
		{
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical stem 
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, pxBorder, cursorPos + pxHalfChar, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'U')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'V')
		{
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxHalfChar, regHeight + pxBorder, color, color);
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, regHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == 'W')
		{
			//vertical left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos, regHeight + pxBorder, color, color);
			//vertical right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//diag left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxHalfChar, regHalfHeight + pxBorder, color, color);
			//diag right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'X')
		{
		
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == 'Y')
		{
			//diag left
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxHalfChar, regHalfHeight + pxBorder, color, color);
			//diag right
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical stem 
			DrawLine(buffer,lpitch32, regHeight, cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxHalfChar, regHeight + pxBorder, color, color);

		}
		else if (*sIter == 'Z')
		{
			//top horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, regHeight + pxBorder, color, color);
			//diag 
			DrawLine(buffer,lpitch32, regHeight, cursorPos, regHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == ' ')
		{
			//do nothing, let it skip
		}

		cursorPos += pxCharWidth + pxCharSpacing;

	} //end for string iterator
}

void RenderTextToRegion(DWORD* buffer, int lpitch32, char letter, RECT region, DWORD bckgColor, DWORD color)
{

	string s(&letter);
	RenderTextToRegion(buffer, lpitch32, s, region, bckgColor, color);
}

/*

DWORD* Font::mBuffer;
int Font::mBufferPitch;
int Font::mFontHeight;

void Font::RenderTextAt(DWORD* buffer, int lpitch32, string text, int x, int y, float scale, DWORD color)
{


	//how many characters to draw
	int numChars = text.size();
	int numSpaces = numChars - 1;

	//null string, break
	if (numChars == 0)
		return;

	//define aspect ratio
	float aspectRatio = 0.5f;

	int pxCharWidth = 1 * scale;

	int pxBorder = 0;
	int pxCharSpacing = pxCharWidth/5;

	//how many pixels in width per character
	int regWidth = numChars * pxCharWidth;
	int regHeight = pxCharWidth / aspectRatio;

	int regHalfHeight = int((float(regHeight) / 2.0f) + 0.5f);
	int regThreeFourthHeight = int((float(regHeight) * 3.0f / 4.0f) + 0.5f);
	int regQuarterHeight = int((float(regHeight) / 4.0f) + 0.5f);
	
	int pxHalfChar = int((float(pxCharWidth) / 2.0f) + 0.5f);
	int pxQuarterChar = int((float(pxCharWidth) / 4.0f) + 0.5f);
	int pxThreeFourthChar = int((float(pxCharWidth) * 3.0f / 4.0f) + 0.5f);

	//create a buffer
	mBuffer = new DWORD[regWidth * regHeight];
	mBufferPitch = regWidth;
	mFontHeight = regHeight;

	string::iterator sIter = text.begin();
	int cursorPos = pxBorder;
	
	for (sIter; sIter < text.end(); sIter++)
	{/*
		if (*sIter == 'A')
		{
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxHalfChar, pxBorder, color, color);
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//	DrawLine(mBuffer,mBufferPitch, xOffset + (width / 2) - 2, yOffset + (height / 2), xOffset + (width / 2) + 2, yOffset + (height / 2), color, color);
		}
		
		else if (*sIter == 'B')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxThreeFourthChar, pxBorder, color, color);
			//mid horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//top cell vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxThreeFourthChar, pxBorder, cursorPos + pxThreeFourthChar, regHalfHeight + pxBorder, color, color);
			//bottom cell vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);


		}
		else if (*sIter == 'C')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top cell vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//bottom cell vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
				else if (*sIter == 'D')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxThreeFourthChar, pxBorder, color, color);
			//bottom horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxThreeFourthChar, regHeight - pxBorder, color, color);
			//top chord
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxThreeFourthChar, pxBorder, cursorPos + pxCharWidth, regQuarterHeight + pxBorder, color, color);
			//right vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder + regQuarterHeight, cursorPos + pxCharWidth, regHeight - pxBorder - regQuarterHeight, color, color);
			//bottom chord
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxThreeFourthChar, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder - regQuarterHeight, color, color);

		}

		
			else if (*sIter == 'E')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'F')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'G')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//left vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'H')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
		else if (*sIter == 'I')
		{
			//vertical stem 
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, pxBorder, cursorPos + pxHalfChar, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'J')
		{
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'K')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top diagonal
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//bottom diagonal
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
				else if (*sIter == 'L')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'M')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//diag left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxHalfChar, regHalfHeight + pxBorder, color, color);
			//diag right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == 'N')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//diag left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'O')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'P')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);

		}
				else if (*sIter == 'Q')
		{
			//vertical stem
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//bottom horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxThreeFourthChar, regHeight - pxBorder, color, color);
			//right vertical
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//bottom chord
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxThreeFourthChar, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder - regQuarterHeight, color, color);

		}
		else if (*sIter == 'R')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//bottom diagonal
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
	else if (*sIter == 'S')
		{
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxQuarterChar, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//1st chord
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regQuarterHeight + pxBorder, cursorPos + pxQuarterChar, pxBorder, color, color);
			//2nd vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regQuarterHeight + pxBorder, cursorPos, regHalfHeight + pxBorder, color, color);
			//middle horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHalfHeight + pxBorder, color, color);
			//3rd vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regThreeFourthHeight + pxBorder, color, color);
			//bottom horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxThreeFourthChar, regHeight - pxBorder, color, color);
			//bottom chord
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxThreeFourthChar, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder - regQuarterHeight, color, color);

		}
		else if (*sIter == 'T')
		{
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical stem 
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, pxBorder, cursorPos + pxHalfChar, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'U')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'V')
		{
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxHalfChar, regHeight - pxBorder, color, color);
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, regHeight - pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == 'W')
		{
			//vertical left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos, regHeight - pxBorder, color, color);
			//vertical right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxCharWidth, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//diag left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxHalfChar, regHalfHeight + pxBorder, color, color);
			//diag right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'X')
		{

			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == 'Y')
		{
			//diag left
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxHalfChar, regHalfHeight + pxBorder, color, color);
			//diag right
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//vertical stem 
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos + pxHalfChar, regHalfHeight + pxBorder, cursorPos + pxHalfChar, regHeight - pxBorder, color, color);

		}
		else if (*sIter == 'Z')
		{
			//top horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);
			//lower horiz line
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, regHeight - pxBorder, color, color);
			//diag 
			DrawLine(mBuffer, mBufferPitch, mFontHeight,cursorPos, regHeight - pxBorder, cursorPos + pxCharWidth, pxBorder, color, color);

		}
		else if (*sIter == ' ')
		{
			//do nothing, let it skip
		}
		
		cursorPos += pxCharWidth + pxCharSpacing;
	
	} //end for string iterator
	
	

	//draw to screen
	DWORD* tempMem = mBuffer;
	DWORD* tempMainMem = buffer;
	VECTOR2D pos = VECTOR2D(x, y);
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < mFontHeight; i++)
	{
		memcpy(tempMainMem, tempMem, sizeof(DWORD)*mBufferPitch);
		tempMainMem += lpitch32;
		tempMem += int(mBufferPitch);
	}

	delete mBuffer;
	
	
}
*/

bool UIDropContainer::OnLUp(int mouseX, int mouseY)
{
	//something is picked up
	if (mPickedUpItem != -1)
	{
		UINT dropType = mItemList[mPickedUpItem].uiType;
		void(*cbfunc) () = mItemList[mPickedUpItem].actionFunction;
		
		switch (dropType)
		{
		case UITYPE_BUTTON:
		{
			mUIInstance->createButton(cbfunc, mouseX, mouseY, "INBTN");

		}break;

		case UITYPE_TEXTFIELD:
		{
			mUIInstance->createTextField(cbfunc, mouseX, mouseY, 100, 50, 4);
		}break;

		case UITYPE_SELECTIONBOX:
		{
			UIElement* elm = mItemList[mPickedUpItem].itemInstance = (UIElement*)mUIInstance->createSelectionBox(mouseX, mouseY, "INSBOX");
			UISelectionBox* selBox = (UISelectionBox*)elm;
			vector<void*> tempData = mInitDataMap[dropType].data;
			vector<void*>::iterator vIter = tempData.begin();
			for (vIter; vIter < tempData.end(); vIter++)
			{
				string* s = (string*)(*vIter);
				selBox->AddItem(*s);
			}

			

		}break;

		case UITYPE_CHECKBOX:
		{
			mUIInstance->createCheckBox(mouseX, mouseY, false);
		}break;

		case UITYPE_RADIO:
		{
			mUIInstance->createRadioGroup(mouseX, mouseY, 1);
		}break;

		case UITYPE_DROPCONTAINER:
		{
			//mUIInstance->createButton(NULL, mouseX, mouseY, "INSERTED");
		}break;


		default: break;
		}

		mPickedUpItem = -1;
		return true;
		
	}

	return false;
}



void UICheckBox::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	//clear
	memset(mFrameMem, 0, mFrameSize);


	DWORD colorState = 0;
	if (stateID == UISTATE_NORMAL)
		colorState = COLOR_BLUE;
	else if (stateID == UISTATE_HOVER)
		colorState = COLOR_RED;

	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, 0, region.getWidth() - 1, 0, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, 0, region.getWidth() - 1, region.getHeight() - 1, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() - 1, region.getHeight() - 1, 0, region.getHeight() - 1, colorState, colorState);
	DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() - 1, 0, 0, colorState, colorState);

	if (mIsChecked)
	{
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  0, region.getHeight() / 2, region.getWidth() / 2, int(region.getHeight()), COLOR_RED, COLOR_RED);
		DrawLine(mFrameMem, region.getWidth(), region.getHeight(),  region.getWidth() / 2 , region.getHeight(), region.getWidth(), 0, COLOR_RED, COLOR_RED);

	}

	//blit to screen
	DWORD* tempMem = mFrameMem;
	DWORD* tempMainMem = mem;
	VECTOR2D pos = region.GetPosition();
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < region.getHeight(); i++)
	{
		memcpy(tempMainMem, tempMem, sizeof(DWORD)*int(region.getWidth()));
		tempMainMem += lpitch32;
		tempMem += int(region.getWidth());
	}
}

void UIRadioGroup::Draw(DWORD* mem, int lpitch32, float timeDelta)
{
	//clear
	memset(mFrameMem, 0, mFrameSize);

	int rad = mButtonDiameter/2 - 1;
	if (stateID == UISTATE_NORMAL)
	{
		DrawCircle(mFrameMem, int(region.getWidth()), rad, rad, rad, COLOR_BLUE);
	}
	else if (stateID == UISTATE_HOVER)
	{
		DrawCircle(mFrameMem, int(region.getWidth()), rad, rad, rad, COLOR_RED);
	}

	if (mSelectedButton == 0)
		DrawCircle(mFrameMem, int(region.getWidth()), rad, rad, rad/2, COLOR_RED);


	//blit main frame to screen
	DWORD* tempMainMem = mem;
	DWORD* tempFrameMem = mFrameMem;
	VECTOR2D pos = region.GetPosition();
	tempMainMem += int(pos.x + pos.y*lpitch32);

	for (int i = 0; i < (int)region.getHeight(); i++)
	{
		memcpy(tempMainMem, tempFrameMem, sizeof(DWORD)*int(region.getWidth()));
		tempMainMem += lpitch32;
		tempFrameMem += int(region.getWidth());
	}


	//draw and blit other buttons
	DWORD* tempBtnMem;
	vector<RECT2D>::iterator vIter = mButtons.begin();
	int btnCount = 0;
	for (vIter; vIter < mButtons.end(); vIter++, btnCount++)
	{
		//reset pointers back to start
		tempMainMem = mem;
		tempBtnMem = mButtonsMem[btnCount];

		//clear
		memset(tempBtnMem, 0, mFrameSize);

		//draw button
		if (mBttnHoverIndex == btnCount)
			DrawCircle(tempBtnMem, int(region.getWidth()), rad, rad, rad, COLOR_RED);
		else
			DrawCircle(tempBtnMem, int(region.getWidth()), rad, rad, rad, COLOR_BLUE);

		if (mSelectedButton == (btnCount + 1))
		{
			DrawCircle(tempBtnMem, int(region.getWidth()), rad, rad, rad / 2, COLOR_RED);
		}

		//blit buttons to screen
		pos = vIter->GetPosition();
		tempMainMem += int(pos.x + pos.y*lpitch32);
		for (int row = 0; row < region.getHeight(); row++)
		{
			memcpy(tempMainMem, tempBtnMem, sizeof(DWORD)*int(region.getWidth()));
			tempMainMem += lpitch32;
			tempBtnMem += int(region.getWidth());
		}


	}

}

UserInterface::~UserInterface()
{

	if (pForegroundSRV)
	{
		pForegroundSRV->Release();
		pForegroundSRV = NULL;
	}

	if (pBackgroundSRV)
	{
		pBackgroundSRV->Release();
		pBackgroundSRV = NULL;
	}

	if (foregroundTexture)
	{
		foregroundTexture->Release();
		foregroundTexture = NULL;
	}

	if (backgroundTexture)
	{
		backgroundTexture->Release();
		backgroundTexture = NULL;
	}

	if (pD3D10IndexBuffer)
	{
		pD3D10IndexBuffer->Release();
		pD3D10IndexBuffer = NULL;
	}

	if (pD3D10VertexBuffer)
	{
		pD3D10VertexBuffer->Release();
		pD3D10VertexBuffer = NULL;
	}

}


UserInterface::UserInterface(const SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizer): EventListener(this)
{
	



	isUIVisible = true;
	mXpos = 0;
	mYpos = 0;
	mode = "normal";
	mElementInFocus = -1;
	lro = localRasterizer;

	mIsLButtonDown = false;


	float aspectRatio = float(SoftwareRasterizer.clientRect.right) / float(SoftwareRasterizer.clientRect.bottom);
	float planeXOrig = -1.0f, planeYOrig = -1.0f;
	float planeWidth = 2.0f;
	float planeHeight = 2.0f;

	//create the VERTEX BUFFER  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	D3D10_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D10_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(DX10VERTEX)* 4; //total size of buffer in bytes
	vbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vbDesc.MiscFlags = 0;

	if (FAILED(lro->pD3D10Device->CreateBuffer(&vbDesc, NULL, &pD3D10VertexBuffer))) return;


	//create the INDEX BUFFER   ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	D3D10_BUFFER_DESC ibDesc;
	ibDesc.Usage = D3D10_USAGE_DYNAMIC;
	ibDesc.ByteWidth = sizeof(unsigned int)* 6; //6 indices for a square
	ibDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	ibDesc.MiscFlags = 0;

	if (FAILED(lro->pD3D10Device->CreateBuffer(&ibDesc, NULL, &pD3D10IndexBuffer))) return;


	//initialize the VERTEX BUFFER ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	UINT numVertices = 4;
	DX10VERTEX* v = NULL;

	//lock vertex buffer for CPU use
	pD3D10VertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);

	v[0] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 1.0f));
	v[1] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 1.0f));
	v[2] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 0.0f));
	v[3] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 0.0f));

	pD3D10VertexBuffer->Unmap();

	//initialize the INDEX BUFFER ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	int* i = NULL;
	pD3D10IndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&i);

	i[0] = 0;
	i[1] = 3;
	i[2] = 2;
	i[3] = 2;
	i[4] = 1;
	i[5] = 0;

	pD3D10IndexBuffer->Unmap();



	//create our texture (or load it) ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = SoftwareRasterizer.clientRect.right;
	desc.Height = SoftwareRasterizer.clientRect.bottom;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	lro->pD3D10Device->CreateTexture2D(&desc, 0, &foregroundTexture);
	lro->pD3D10Device->CreateTexture2D(&desc, 0, &backgroundTexture);

	//create intreface to shader kept texture
	lro->pD3D10Device->CreateShaderResourceView(foregroundTexture, NULL, &pForegroundSRV);
	lro->pD3D10Device->CreateShaderResourceView(backgroundTexture, NULL, &pBackgroundSRV);

	pForegroundSV = lro->pD3D10Effect->GetVariableByName("uiTexForeground")->AsShaderResource();
	pBackgroundSV = lro->pD3D10Effect->GetVariableByName("uiTexBackground")->AsShaderResource();

}


char* UserInterface::getCharBitmap(char c)
{

	if (c == 'A' || c == 'a')
		return A_CHAR_BMP;
	else if (c == 'B' || c == 'b')
		return (B_CHAR_BMP);
	else if (c == 'C' || c == 'c')
		return (C_CHAR_BMP);
	else if (c == 'D' || c == 'd')
		return (D_CHAR_BMP);
	else if (c == 'E' || c == 'e')
		return (E_CHAR_BMP);
	else if (c == 'F' || c == 'f')
		return (F_CHAR_BMP);
	else if (c == 'G' || c == 'g')
		return (G_CHAR_BMP);
	else if (c == 'H' || c == 'h')
		return (H_CHAR_BMP);
	else if (c == 'I' || c == 'i')
		return (I_CHAR_BMP);
	else if (c == 'J' || c == 'j')
		return (J_CHAR_BMP);
	else if (c == 'K' || c == 'k')
		return (K_CHAR_BMP);
	else if (c == 'L' || c == 'l')
		return (L_CHAR_BMP);
	else if (c == 'M' || c == 'm')
		return (M_CHAR_BMP);
	else if (c == 'N' || c == 'n')
		return (N_CHAR_BMP);
	else if (c == 'O' || c == 'o')
		return (O_CHAR_BMP);
	else if (c == 'P' || c == 'p')
		return (P_CHAR_BMP);
	else if (c == 'Q' || c == 'q')
		return (Q_CHAR_BMP);
	else if (c == 'R' || c == 'r')
		return (R_CHAR_BMP);
	else if (c == 'S' || c == 's')
		return (S_CHAR_BMP);
	else if (c == 'T' || c == 't')
		return (T_CHAR_BMP);
	else if (c == 'U' || c == 'u')
		return (U_CHAR_BMP);
	else if (c == 'V' || c == 'v')
		return (V_CHAR_BMP);
	else if (c == 'W' || c == 'w')
		return (W_CHAR_BMP);
	else if (c == 'X' || c == 'x')
		return (X_CHAR_BMP);
	else if (c == 'Y' || c == 'y')
		return (Y_CHAR_BMP);
	else if (c == 'Z' || c == 'z')
		return (Z_CHAR_BMP);

	if (c == '0')
		return (ZERO_CHAR_BMP);
	else if (c == '1')
		return (ONE_CHAR_BMP);
	else if (c == '2')
		return (TWO_CHAR_BMP);
	else if (c == '3')
		return (THREE_CHAR_BMP);
	else if (c == '4')
		return (FOUR_CHAR_BMP);
	else if (c == '5')
		return (FIVE_CHAR_BMP);
	else if (c == '6')
		return (SIX_CHAR_BMP);
	else if (c == '7')
		return (SEVEN_CHAR_BMP);
	else if (c == '8')
		return (EIGHT_CHAR_BMP);
	else if (c == '9')
		return (NINE_CHAR_BMP);
	else if (c == ' ')
		return (SPACE_CHAR_BMP);
	else if (c == 0xBE)
		return (PERIOD_CHAR_BMP);
	else if (c == '\\')
		return (BACKSLASH_CHAR_BMP);
	else if (c == ':')
		return (COLON_CHAR_BMP);
	else if (c == '(')
		return (OPENBRACE_CHAR_BMP);
	else if (c == ')')
		return (CLOSEBRACE_CHAR_BMP);
	else if (c == ',')
		return (COMMA_CHAR_BMP);
	else if (c == '.')
		return (PERIOD_CHAR_BMP);

	return 0;
}

int UserInterface::displayString(string str, int x, int y, DWORD* mem, int lpitch32)
{ 
	int offset = 0;
	for (int i = 0; i < str.size(); i++)
	{
		char* bmp = getCharBitmap(str.at(i));
		DrawCharBitmap(mem, lpitch32, bmp, COLOR_RED, x + offset, y, UI_CHAR_WIDTH_PIXELS, UI_CHAR_WIDTH_PIXELS);
		offset += UI_CHAR_WIDTH_PIXELS; 
		offset += UI_CHAR_SPACING_PIXELS;
	}

	return x + offset;
}

int UserInterface::displayString(double num, int x, int y, DWORD* mem, int lpitch32)
{

	std::ostringstream strs;
	strs << std::setprecision(16) << std::fixed << num;
	string s = strs.str();
	int offset = 0;
	for (int i = 0; i < s.size(); i++)
	{
		char* bmp = getCharBitmap(s.at(i));
		DrawCharBitmap(mem, lpitch32, bmp, COLOR_RED, x + offset, y, UI_CHAR_WIDTH_PIXELS, UI_CHAR_WIDTH_PIXELS);
		offset += UI_CHAR_WIDTH_PIXELS;
		offset += UI_CHAR_SPACING_PIXELS;
	}

	return x + offset;
}


char UserInterface::SPACE_CHAR_BMP[] = { 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0
};

char UserInterface::A_CHAR_BMP[] = { 0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 1, 0, 0,
0, 0, 0, 1, 0, 1, 0, 0,
0, 0, 0, 1, 0, 1, 0, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 1, 1, 1, 1, 0,
0, 1, 0, 0, 0, 0, 0, 1
};

char UserInterface::B_CHAR_BMP[] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0
};
char UserInterface::C_CHAR_BMP[64] = { 0, 0, 0, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 1, 0, 0
};
char UserInterface::D_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0, 0
};
char UserInterface::E_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0
};
char UserInterface::F_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0
};
char UserInterface::G_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0
};
char UserInterface::H_CHAR_BMP[64] = { 0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0
};
char UserInterface::I_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 1, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 1, 1, 1, 1, 1, 0
};
char UserInterface::J_CHAR_BMP[64] = { 0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0
};
char UserInterface::K_CHAR_BMP[64] = { 0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 1, 0, 0, 0,
0, 0, 1, 1, 0, 0, 0, 0,
0, 0, 1, 0, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0
};
char UserInterface::L_CHAR_BMP[64] = { 0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0
};
char UserInterface::M_CHAR_BMP[64] = { 0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 1, 0, 0, 1, 1, 0,
0, 1, 0, 1, 1, 0, 1, 0,
0, 1, 0, 1, 1, 0, 1, 0,
0, 1, 0, 1, 1, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0
};
char UserInterface::N_CHAR_BMP[64] = { 0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 1, 0, 0, 0, 1, 0,
0, 1, 0, 1, 0, 0, 1, 0,
0, 1, 0, 1, 1, 0, 1, 0,
0, 1, 0, 0, 1, 0, 1, 0,
0, 1, 0, 0, 1, 0, 1, 0,
0, 1, 0, 0, 0, 1, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0
};
char UserInterface::O_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 0, 1, 1, 1, 1, 0, 0
};
char UserInterface::P_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0
};
char UserInterface::Q_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 0, 1, 1, 1, 1, 1, 1
};
char UserInterface::R_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 1, 0, 0, 0,
0, 0, 1, 1, 0, 0, 0, 0,
0, 0, 1, 0, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0
};
char UserInterface::S_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0, 0
};
char UserInterface::T_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 1, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0
};
char UserInterface::U_CHAR_BMP[64] = { 0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 0, 1, 1, 1, 0, 0
};
char UserInterface::V_CHAR_BMP[64] = { 0, 1, 0, 0, 0, 0, 0, 1,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 0, 1, 0, 1, 0, 0,
0, 0, 0, 1, 1, 1, 0, 0
};
char UserInterface::W_CHAR_BMP[64] = { 0, 1, 0, 0, 1, 0, 0, 1,
0, 1, 0, 0, 1, 0, 0, 1,
0, 1, 0, 0, 1, 0, 0, 1,
0, 1, 0, 0, 1, 0, 0, 1,
0, 1, 0, 0, 1, 0, 0, 1,
0, 1, 0, 0, 1, 0, 0, 1,
0, 1, 0, 0, 1, 0, 0, 1,
0, 0, 1, 1, 1, 1, 1, 0
};
char UserInterface::X_CHAR_BMP[64] = { 1, 0, 0, 0, 0, 0, 0, 1,
0, 1, 0, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 1, 0, 0, 0, 0, 1, 0,
1, 0, 0, 0, 0, 0, 0, 1
};
char UserInterface::Y_CHAR_BMP[64] = { 0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 0, 1, 0, 1, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0
};
char UserInterface::Z_CHAR_BMP[64] = { 0, 1, 1, 1, 1, 1, 1, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 1, 0, 0, 0, 0, 0, 0,
0, 1, 0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 1, 0
};


char UserInterface::ZERO_CHAR_BMP[64] = { 0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0
};
char UserInterface::ONE_CHAR_BMP[64] = { 0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0
};
char UserInterface::TWO_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 1, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 1, 1, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0
};
char UserInterface::THREE_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0, 0
};
char UserInterface::FOUR_CHAR_BMP[64] = { 0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0
};
char UserInterface::FIVE_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0, 0
};
char UserInterface::SIX_CHAR_BMP[64] = { 0, 0, 0, 0, 1, 1, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 1, 0, 0
};
char UserInterface::SEVEN_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 1, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0
};
char UserInterface::EIGHT_CHAR_BMP[64] = { 0, 0, 1, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0
};
char UserInterface::NINE_CHAR_BMP[64] = { 0, 0, 0, 1, 1, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0
};

char UserInterface::COLON_CHAR_BMP[64] = { 0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0
};

char UserInterface::BACKSLASH_CHAR_BMP[64] = { 1, 0, 0, 0, 0, 0, 0, 0,
0, 1, 0, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 0, 1, 0,
0, 0, 0, 0, 0, 0, 0, 1
};

char UserInterface::PERIOD_CHAR_BMP[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0
};

char UserInterface::OPENBRACE_CHAR_BMP[64] = { 0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0
};


char UserInterface::CLOSEBRACE_CHAR_BMP[64] = { 0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0
};

char UserInterface::COMMA_CHAR_BMP[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0
};

char UserInterface::SMILEY_FACE_CHAR[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 1, 1, 0, 0, 1, 1, 0,
0, 1, 0, 1, 1, 0, 1, 0,
0, 1, 0, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0
};
