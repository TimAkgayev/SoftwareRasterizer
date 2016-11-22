#pragma once

#include "SoftwareRasterizer.h"
#include "stdinclude.h"
#include "MacrosAndDefines.h"
#include "Math56.h"
#include "AnimatedBitmap.h"

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
#define GUI_MOUSEUP_HIT 3
#define GUI_MOUSEUP_NOHIT 4

typedef unsigned int uint;
typedef unsigned char uchar;



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

	virtual bool GetVisibility() const;
	virtual bool GetFocus() const;
	virtual int GetType() const;
	virtual int GetState() const;
	virtual int GetDepthIndex() const;
	virtual VECTOR2D GetPosition() const;
	virtual RECT2D GetRECT() const;
	virtual std::string GetName() const;

	virtual void Draw(DWORD* mem, int lpitch32, float timeDelta);
	virtual bool IsInRegion(int mouseX, int mouseY);
	

protected:

	RECT2D region;
	int typeID;
	int stateID;
	bool isInFocus;
	bool isVisible;
	int depthIndex;
	std::string mElementName;

};

void renderTextToRegion(DWORD* buffer, int lpitch32, string text, RECT region, DWORD bgColor, DWORD color);
void renderTextToRegion(DWORD* buffer, int lpitch32, char letter, RECT region, DWORD bgColor, DWORD color);
void renderLetterToBuffer(DWORD* buffer, int lpitch32, int pxWidth, char letter, DWORD color);

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

	void SetText(string text);
	string GetText() const;
	

	void SetOnLClickCallback(void(*cb)());
	void LoadImage(string filename);


private:

	void(*mCallbackLClick)();
	DWORD* mFrameMem;
	DWORD bgColor;
	int mFrameSize;
	int screen_w, screen_h;
	string mText;
	AnimatedBitmap mImage;


};

class UITextField : public UIElement
{
public:

	UITextField();
	~UITextField();


	void OnHover(int xPos, int yPos) override;
	bool OnLDown(int xPos, int yPos) override;
	bool OnLUp(int xPos, int yPos) override;
	void Draw(DWORD* mem, int lpitch32, float timeDelta) override;

	void SetOnLClickCallback(void(*cb)());
	void SetCellCount(int cellcount);
	void SetFontSize(int pxWidth);
	void AddCharacter(char c, int pos = -1);
	void RemoveCharacter(int pos = -1);
	std::string GetText();
	int GetCellCount() const;


private:
	vector<bool>  mCellSelectionList;
	vector<char>  mText;
	vector<DWORD*> mCharFrames;
	void(*mCallbackLClick)();
	DWORD* mFrameMem;
	DWORD* mSelectionColor;
	DWORD bckgColor;
	float mBlinkTimeElapsed;
	bool mIsCursorOn;
	int mCellSpacing;
	int mCellCount;
	int mFrameSize;
	int mCellWidthPx;
	int mCursorPos;
	int mCellSizeBy;
};

class UIRegion : public UIElement
{
public:
	UIRegion();
	UIRegion(int xpos, int ypos, int width, int height, DWORD color);
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
	DWORD* mFrameMem;
	DWORD* mItemFrameMem;
	DWORD bgColor;
	int mFrameSize;
	int mItemFrameSize;
	int mRegionHoverIndex;


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
	

	void AddItem(BITMAP_FILE* image, UINT uitype, void(*func)());
	void SetUIInstance(UserInterface* inst);
	void InsertInitializatinData(UINT uiType, vector<void*>& data);
	UIElement* GetItemInstance(int id);

private:

	DWORD* mFrameMem;
	uint mFrameSize;

	struct cDropItem
	{
		BITMAP_FILE* image;
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
	void SetVisibility(bool state) override;

	void AddChild(UIElement* child);
	std::string GetTitle() const;
	DWORD GetBackgroundColor() const;

private:

	vector<UIElement*> mChildren;
	DWORD mBackgroundColor;
	DWORD mTitleBarColor;

	int mFrameSize;
	DWORD* mFrameMem;
	int titleBarWidth;

	bool mIsDragged;
	POINT dragOffset;
	string mTitle;
};

class UIText : public UIElement
{
public:
	UIText();
	UIText(int xpos, int ypos, int width, int height, std::string s, DWORD bColor, DWORD fColor);
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
};


//-------------------------------------------------------------------------------------------------------------------------------

class UserInterface
{
public:

	UserInterface(const SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizer);
	~UserInterface();

	int LoadRosource(string filename);
	BITMAP_FILE* GetImageResource(int resourceID);

	UIButton* createButton(void(*lbCallback)(), int xPos, int yPos, string text, int width = UI_BUTTONWIDTH, int height = UI_BUTTONHEIGHT);
	UITextField* createTextField(void(*tfCallback)(), int xPos, int yPos, int numChars);
	UISelectionBox* createSelectionBox(int xPos, int yPos, string title);
	UICheckBox* createCheckBox(int xPos, int yPos, bool isChecked);
	UIRadioGroup* createRadioGroup(int xPos, int yPos, int numButtons);
	UIDropContainer* createDropContainer(int xPos, int yPos);
	UIRegion* createRegion(int xPos, int yPos, int width, int height, DWORD color = COLOR_BLUE);
	UIDropdownMenu* createDropdownMenu(int xPos, int yPos, string title);
	UIWindow* createWindow(int xPos, int yPos, int width, int height, string Title, DWORD color = COLOR_BLUE); 
	UIText* createText(int xPos, int yPos, int width, int height, string text, DWORD bColor = COLOR_BLACK, DWORD fColor = COLOR_RED);
	UIList* createList(int xPos, int yPos, int width);

	bool isLMD();
	POINT getLMDPosition();
	//process mouse down and up for gui, returns if hit and mouse state
	int ProcessMouseClick(int mouseButton, bool isPressedDown, int xPos, int yPos);
	void RemoveFocus();
	//process highlights and OnHover functions
	void ProcessMousePosition(int xPos, int yPos);
	//process the keyboard buttons
	void ProcessKeyboard(uint virtual_key);
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

	string mode;
	POINT mousePos;

private:
	
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
	int xpos;
	int ypos;
	vector<string> mImageLoadList;
	vector<BITMAP_FILE> mImageData;
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
