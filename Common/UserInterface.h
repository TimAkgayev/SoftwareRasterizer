﻿#pragma once

#include "SoftwareRasterizer.h"
#include "stdinclude.h"
#include "MacrosAndDefines.h"
#include "SMath.h"
#include "EventListener.h"
#include "Font.h"

#define UI_CHAR_WIDTH_PIXELS 8
#define UI_CHAR_SPACING_PIXELS 2

enum {UITYPE_BUTTON, UITYPE_TEXTFIELD, UITYPE_SELECTIONBOX, UITYPE_CHECKBOX, UITYPE_RADIO, UITYPE_DROPCONTAINER, UITYPE_LIST, UITYPE_REGION, UITYPE_DROPDOWNMENU, UITYPE_WINDOW, UITYPE_TEXT};
enum {UISTATE_NORMAL = 0, UISTATE_HOVER, UISTATE_PRESSED};
enum { DEPTHINDEX_BACKGROUND, DEPTHINDEX_FOREGROUND };

#define UI_LMOUSEBUTTON 0
#define UI_MMOUSEBUTTON 1
#define UI_RMOUSEBUTTON 2

#define UI_BUTTONWIDTH  50
#define UI_BUTTONHEIGHT 20

#define UI_TEXTFIELDWIDHT 150
#define UI_TEXTFIELDHEIGHT 20

#define GUI_MOUSEDOWN_HIT 1
#define GUI_MOUSEDOWN_NOHIT 2
#define GUI_MOUSEUP_NOHIT 3
#define GUI_MOUSEUP_HIT 4


typedef unsigned int uint;
typedef unsigned char uchar;



void RenderTextToRegion(DWORD* buffer, int lpitch32, string text, RECT region, DWORD bgColor, DWORD color);
void RenderTextToRegion(DWORD* buffer, int lpitch32, char letter, RECT region, DWORD bgColor, DWORD color);


/*
class Font {
public:

	static void RenderTextAt(DWORD* buffer, int lpitch32, string text, int x, int y, float scale, DWORD color);

private:
	static DWORD* mBuffer;
	static	int mBufferPitch;
	static	int mFontHeight;

};

*/

struct ImageResource
{
	unsigned char* pData;

	uint biSize;
	uint width;
	uint height;
};

//external
void DrawCharBitmap(DWORD* mem_buffer, int lpitch32, char* bitmap, DWORD color, int xpos, int ypos, int rows, int columns, float scale = 1.0f);
void olccb();

//forward
class UserInterface;

class UIElement
{
public:
	UIElement();

	virtual void OnHover(int xPos, int yPos);
	virtual bool OnLClick(int mouseX, int mouseY);
	virtual bool OnLUp(int mouseX, int mouseY);
	virtual bool OnLDown(int mouseX, int mouseY);

	virtual void SetVisibility(bool state);
	virtual void SetFocus(bool state);
	virtual void SetType(int t);
	virtual void SetState(int state);
	virtual void SetPosition(int xpos, int ypos);
	virtual void SetDimension(int widht, int height);
	virtual void SetDepthIndex(int index);
	virtual void SetName(std::string name);
	virtual void SetEnabledState(bool state);

	virtual bool GetVisibility() const;
	virtual bool GetFocus() const;
	virtual int GetType() const;
	virtual int GetState() const;
	virtual int GetDepthIndex() const;
	virtual VECTOR2D GetPosition() const;
	virtual RECT2D GetRECT() const;
	virtual std::string GetName() const;
	virtual bool GetEnabledState() const;

	virtual void DrawForeground(DWORD* mem, int lpitch32, float timeDelta);
	virtual void DrawBackground(DWORD* mem, int lpitch32, float timeDelta);
	virtual void Draw(DWORD* mem, int lpitch32, float timeDelta);
	virtual bool IsInRegion(int mouseX, int mouseY);
	

protected:

	RECT2D region;
	int typeID;
	int stateID;
	bool isInFocus;
	bool isVisible;
	bool isEnabled;
	int depthIndex;
	std::string mElementName;
	DWORD mForegroundFrameMem;
	DWORD mBackgroundFrameMem;
	int mFrameSize;

};

class UIButton : public UIElement
{
public:
	UIButton();
	UIButton(int width, int height);
	~UIButton();

	void OnHover(int xPos, int yPos) override;
	bool OnLDown(int xPos, int yPos) override;
	bool OnLUp(int xPos, int yPos) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;
	bool IsInRegion(int x, int y) override;
	void SetDimension(int, int) override;

	void SetText(string text);
	string GetText() const;
	
	

	void SetOnLClickCallback(void(*cb)());
	void SetImage(int RMKey);


private:

	void(*mCallbackLClick)();
	DWORD* mFrameMem;
	DWORD bgColor;
	int mFrameSize;
	int screen_w, screen_h;
	string mText;
	BitmapImage* mImage;
	Font* mFont;


};

class UITextField : public UIElement
{
public:

	UITextField();
	UITextField(int xpos, int ypos, int width, int height, int maxcharcount =-1);
	~UITextField();


	void OnHover(int xPos, int yPos) override;
	bool OnLDown(int xPos, int yPos) override;
	bool OnLUp(int xPos, int yPos) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;
	void DrawForeground(DWORD* mem, int lpitch32, float timeDelta) override;
	void DrawBackground(DWORD* mem, int lpitch32, float timeDelta) override;

	void SetMaxCharCount(int count);
	int GetMaxCharCount() const;

	void SetOnLClickCallback(void(*cb)());
	void SetFontSize(int pxWidth, int pxHeight);
	void AddCharacter(char c, int pos = -1);
	void RemoveCharacter(int pos = -1);
	void Clear();
	std::string GetText();
	

private:
	vector<bool>  mCellSelectionList;
	vector<char>  mText;
	Font* mFont;
	void(*mCallbackLClick)();
	DWORD* mForegroundFrameMem;
	DWORD* mBackgroundFrameMem;
	int mFrameSize;

	DWORD* mSelectionColor;
	DWORD bckgColor;

	float mBlinkTimeElapsed;
	bool mIsCursorOn;

	int mMaxCharCount;
	int mCursorPos;

};

class UIRegion : public UIElement
{
public:
	UIRegion();
	UIRegion(int xpos, int ypos, string title, int width, int height, DWORD color);
	~UIRegion();

	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;

	void SetBackgroundColor(DWORD color);
	void SetBorderColor(DWORD color);

private:
	DWORD mBckgColor;
	DWORD mBorderColor;
	
	DWORD* mFrameMem;
	
	int mFrameSize;

};

class UIList : public UIElement
{
public:
	UIList();
	UIList(int xpos, int ypos, int width);
	~UIList();

	void OnHover(int xPos, int yPos) override;
	//returns if hit the element or it's child
	bool OnLClick(int mouseX, int mouseY) override;
	bool OnLUp(int mouseX, int mouseY) override;
	bool OnLDown(int mouseX, int mouseY) override;
	bool IsInRegion(int mouseX, int mouseY) override;
	void SetPosition(int px, int py) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;


	void AddItem(string text, void(*callback)());


private:

	struct item
	{

		string txt;
		void(*callback)();
		RECT2D region;
		DWORD* itemFrame;
	};

	vector<item> mItemList;

	int mCellHeight;
	bool isHorizontal;
	DWORD* mFrameMem;
	int mFrameSize;

};

class UISelectionBox : public UIElement
{
public:
	UISelectionBox();
	~UISelectionBox();

	void OnHover(int xPos, int yPos) override;
	bool OnLDown(int xPos, int yPos) override;
	bool OnLUp(int xPos, int yPos) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;
	void SetFocus(bool state) override;

	void SetTitle(string title);
	std::string GetSelectedTitle() const;
	void AddItem(string name);

private:

	string mSelectedTitle;
	vector<string> mItems;
	vector<RECT2D> mItemRegions;
	DWORD* mFrameMem;
	DWORD* mItemFrameMem;
	DWORD bgColor;
	int mFrameSize;
	int mItemFrameSize;
	bool mIsSelectionMade;
	int mRegionHoverIndex;
	

};

class UIDropdownMenu : public UIElement
{
public:
	UIDropdownMenu();
	UIDropdownMenu(int xPos, int yPos, string title);
	~UIDropdownMenu();

	void OnHover(int xPos, int yPos) override;
	bool OnLDown(int xPos, int yPos) override;
	bool OnLUp(int xPos, int yPos) override;
	void DrawForeground(DWORD* mem, int lpitch32, float timeDelta) override;
	void DrawBackground(DWORD* mem, int lpitch32, float timeDelta) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;
	void SetFocus(bool state) override;

	void SetTitle(string title);
	std::string GetTitle() const;
	void AddItem(string name, void(*cb)());
	

private:

	struct _item
	{
		string title;
		void(*callback)();
	};

	string mTitle;
	vector<_item> mItems;
	vector<RECT2D> mItemRegions;
	DWORD* mForegroundFrameMem;
	DWORD* mBackgroundFrameMem;
	DWORD* mItemFrameMem;
	DWORD bgColor;
	int mFrameSize;
	int mItemFrameSize;
	int mRegionHoverIndex;
	Font* mFont;

};

class UICheckBox : public UIElement
{
public:
	UICheckBox();
	~UICheckBox();

	void OnHover(int xPos, int yPos) override;
	bool OnLUp(int xPos, int yPos) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;

	void SetCheckState(bool state);
	bool IsChecked() const;

private:

	DWORD* mFrameMem;
	int mFrameSize;
	bool mIsChecked;

};

class UIRadioGroup : public UIElement
{
public:
	UIRadioGroup();
	~UIRadioGroup();

	bool OnLUp(int xPos, int yPos) override;
	void OnHover(int xPos, int yPos) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;

	void SetDiameter(int diam);
	int GetNumButtons() const;
	void AddButton();

private:
	int mButtonSpacing;
	int mButtonDiameter;
	int mFrameSize;
	int mBttnHoverIndex;
	DWORD* mFrameMem;
	int mSelectedButton;

	vector<RECT2D> mButtons;
	vector<DWORD*> mButtonsMem;
};

//UIDropContainer added on 9/14/15
class UIDropContainer : public UIElement
{
public:
	UIDropContainer();
	~UIDropContainer();

	void OnHover(int xPos, int yPos) override;
	bool OnLDown(int mouseX, int mouseY) override;
	bool OnLUp(int mouseX, int mouseY) override;



	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;
	

	void AddItem(int RMkey, UINT uitype, void(*func)());
	void SetUIInstance(UserInterface* inst);
	void InsertInitializatinData(UINT uiType, vector<void*>& data);
	UIElement* GetItemInstance(int id);

private:

	DWORD* mFrameMem;
	uint mFrameSize;

	struct cDropItem
	{
		Bitmap* image;
		void(*actionFunction)();
		RECT2D localRegion;
		UINT uiType;
		UIElement* itemInstance;
	};
	struct cElemData
	{
		vector<void*> data;
		UINT uiType;
	};

	vector<cDropItem> mItemList;
	map<UINT, cElemData> mInitDataMap;

	uint m_borderPx;
	uint m_spacingPx;
	uint m_numColumns;

	int mPickedUpItem;

	HWND mCurrentWindow;
	UserInterface* mUIInstance;


};



//UIWindow added on 7/4/16
class UIWindow : public UIElement
{
public:
	UIWindow();
	UIWindow(int xpos, int ypos, int width, int height, string title, DWORD color);
	~UIWindow();


	void OnHover(int xPos, int yPos) override;
	bool OnLClick(int mouseX, int mouseY) override;
	bool OnLUp(int mouseX, int mouseY) override;
	bool OnLDown(int mouseX, int mouseY) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;
	void DrawBackground(DWORD* mem, int lpitch32, float timeDelta) override;
	void DrawForeground(DWORD* mem, int lpitch32, float timeDelta) override;
	void SetVisibility(bool state) override;

	void AddChild(UIElement* child);
	std::string GetTitle() const;
	DWORD GetBackgroundColor() const;

private:

	vector<UIElement*> mChildren;
	DWORD mBackgroundColor;
	DWORD mTitleBarColor;

	int mFrameSize;
	DWORD* mForegroundFrameMem;
	DWORD* mBackgroundFrameMem;
	int titleBarWidth;

	bool mIsDragged;
	POINT dragOffset;
	string mTitle;
};

class UIText : public UIElement
{
public:
	UIText();
	UIText(int xpos, int ypos, int width, int height, std::string s, DWORD fColor, DWORD bColor);
	~UIText();

	void Draw(DWORD* mem, int lpitch32, float deltaTime) override;
	
	void SetText(std::string s);
	std::string GetText();

private:
	std::string mText;
	DWORD mBackgroundColor;
	DWORD mTextColor;

	DWORD* mFrameMem;
	int mFrameSize;
	Font* mFont;
};


//-------------------------------------------------------------------------------------------------------------------------------

class UserInterface : EventListener
{
public:

	UserInterface(const SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizer);
	~UserInterface();

	void ProcessEvent(UINT msg, WPARAM wparam, LPARAM lparam) override;
	bool CheckLastProcessEventResults(int code);

	UIButton* createButton(void(*lbCallback)(), int xPos, int yPos, string text, int width = UI_BUTTONWIDTH, int height = UI_BUTTONHEIGHT);
	UITextField* createTextField(void(*tfCallback)(), int xPos, int yPos, int width, int height, int numChars);
	UISelectionBox* createSelectionBox(int xPos, int yPos, string title);
	UICheckBox* createCheckBox(int xPos, int yPos, bool isChecked);
	UIRadioGroup* createRadioGroup(int xPos, int yPos, int numButtons);
	UIDropContainer* createDropContainer(int xPos, int yPos);
	UIRegion* createRegion(int xPos, int yPos, string title, int width, int height, DWORD color = COLOR_BLUE);
	UIDropdownMenu* createDropdownMenu(int xPos, int yPos, string title);
	UIWindow* createWindow(int xPos, int yPos, int width, int height, string Title, DWORD color = COLOR_BLUE); 
	UIText* createText(int xPos, int yPos, int charWidth, int charHeight, string text, DWORD fColor = COLOR_RED, DWORD bColor = COLOR_BLACK);
	UIList* createList(int xPos, int yPos, int width);

	bool isLMD();
	POINT getLMDPosition();

	void RemoveFocus();
	//returns offset in pixel to the end of the string
	static int displayString(string str, int x, int y, DWORD* mem, int lpitch32);
	//returns offset in pixel to the end of the string
	static int displayString(double num, int x, int y, DWORD* mem, int lpitch32);
	bool UIHasFocus() const;
	void DrawUI(float deltaTime);		
	char* GetCharacterBitmap(char c);
	POINT GetMousePosition() const;
	UIElement* GetElementByName(std::string name);
	UIElement* GetElement(int offset);
	int GetNumberOfElements();
	UIElement* GetHitElement(int x, int y);

	//removes all children
	void Clear();

	string mode;
	POINT mousePos;

private:
	
	vector<int> lastEventResultList;

	vector<UIElement*> mUIElements;
	const SOFTWARERASTERIZER_DX10_OBJECTS* lro;

	ID3D10Buffer* pD3D10VertexBuffer;
	ID3D10Buffer* pD3D10IndexBuffer;
	ID3D10Texture2D* backgroundTexture;
	ID3D10Texture2D* foregroundTexture;

	ID3D10ShaderResourceView* pBackgroundSRV;
	ID3D10ShaderResourceView* pForegroundSRV;

	ID3D10EffectShaderResourceVariable* pBackgroundSV;
	ID3D10EffectShaderResourceVariable* pForegroundSV;


	int m_screenW, m_screenH;
	int mElementInFocus;
	bool isUIVisible;
	int mXpos;
	int mYpos;
	vector<string> mImageLoadList;
	vector<Bitmap*> mImageData;
	bool mIsLButtonDown;
	VECTOR2D mLMouseDownPosition;
	double m_LMouseDownTimeStamp;

	void sortElementsByDepth();


	/////////////////////////bitmaps used for text/////////////////////////////////////


	static char SPACE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];

	static char A_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];

	static char B_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char C_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char D_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char E_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char F_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char G_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char H_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char I_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char J_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char K_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char L_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char M_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char N_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char O_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char P_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char Q_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char R_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char S_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char T_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char U_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char V_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char W_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char X_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char Y_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char Z_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];


	static char ZERO_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char ONE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char TWO_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char THREE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char FOUR_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char FIVE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char SIX_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char SEVEN_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char EIGHT_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char NINE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];

	static char COLON_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char BACKSLASH_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char PERIOD_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char OPENBRACE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char CLOSEBRACE_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char COMMA_CHAR_BMP[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];
	static char SMILEY_FACE_CHAR[UI_CHAR_WIDTH_PIXELS * UI_CHAR_WIDTH_PIXELS];

	static char* getCharBitmap(char c);

};
