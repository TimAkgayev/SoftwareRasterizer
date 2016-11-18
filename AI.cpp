#include "AI.h"

namespace AI
{

	enum base_type {type_object, type_set};
    
	class base
	{
	public:
		base() {}
		~base() {}

		int GetType() const { return type;  }
		string GetName() { return name;  }

	protected:
		int type;
		string name;

	};

	class Object : public base
	{
	public:

	protected:
		string name;
		vector<string> DescriptorTable;
	};

	class ObjectSet : public base
	{
	public:

	protected:
		string name;
		vector<Object> objectSet;
	};

	class World
	{
	public:
		void clear()
		{
			objects.clear();
			sets.clear();
		}

		void AddObject(Object& o)
		{
			objects.push_back(o);
		}

		void AddSet(ObjectSet& s)
		{
			sets.push_back(s);
		}

	private:
		vector<Object> objects;
		vector<ObjectSet> sets;
	};

   

	class Algernon
	{
	public:
		Algernon() : isActionReady(false) {}

		void AddObject(Object& o)
		{
			knownObjects.push_back(o);
		}

		void AddSet(ObjectSet& s)
		{
			knownSets.push_back(s);
		}

		void parseCommand(string command)
		{
            //tells us if the action is queued up and now waiting for object, reset at every new command
			isActionReady = false;
			vector<string> words;

            //break up the string into words
			int spacePos = command.find(' ', 0);
			int offset = 0;
			while (spacePos != string::npos)
			{
				string w = command.substr(offset, spacePos - 1 - offset);
				words.push_back(w);

				offset = spacePos;
				spacePos = command.find(' ', offset);

			}

            //check if word is an action
			vector<string>::reverse_iterator rIter = words.rbegin();
			for (rIter; rIter != words.rend(); rIter++)
			{

				string w = *rIter;

				if (isActionReady == false)
				{
					for (int i = 0; i < knownActions.size(); i++)
					{
						if (w.compare(knownActions[i].name) == 0)
						{


							executePair.action = knownActions[i].action;
							isActionReady = true; //now waiting for object
							break;


						}


					}
				}
				else
				{
					//action is ready, look for object
					for (int i = 0; i < knownObjects.size(); i++)
					{
						if (w.compare(knownObjects[i].GetName()) == 0)
						{
							executePair.operand = &knownObjects[i];
							return;
						}
					}

                    //fell throught, so then look for set
					for (int i = 0; i < knownSets.size(); i++)
					{
						if (w.compare(knownSets[i].GetName()) == 0)
						{
							executePair.operand = &knownSets[i];
						}
					}
				}
			}

            //next word must be an object or null
		}


	private:
		struct ACTION
		{
			void(*action)();
			string name;
		};
		struct EXECUTEPAIR
		{
			void(*action)();
			base* operand;
		};

		World world;
		bool isActionReady;
		EXECUTEPAIR executePair;
		vector<Object> knownObjects;
		vector<ObjectSet> knownSets;
		vector<ACTION> knownActions;

	};
	void Evaluate()
	{

	}


	UserInterface* MyUserInterface;
	const SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizerObject;

	void SetRasterizerObject(SOFTWARERASTERIZER_DX10_OBJECTS* obj)
	{
		localRasterizerObject = obj;
	}

	void MainSoftware(DWORD* video_mem, int lpitch32, float deltaTime)
	{

	
	}


	void MainHardware(float deltaTime)
	{
	}


	void Init(RECT clientRect)
	{

	}


	void Shutdown()
	{

	}

	UserInterface* GetGUI()
	{
		return MyUserInterface;
	}

	void Update(float deltaTime)
	{

	}

	void myMouseCallBack(int x, int y)
	{

	}


	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		UserInterface*  UIPtr = GetGUI();
		PAINTSTRUCT		ps;
		HDC				hdc;


		switch (msg)
		{
		case WM_CREATE:
		{
						  return(0);
		} break;

		case WM_MOVE:
		{


		}break;

		case WM_LBUTTONDOWN:
		{

							   float xPos = GET_X_LPARAM(lParam);
							   float yPos = GET_Y_LPARAM(lParam);

							   if (UIPtr->ProcessMouseClick(0, true, xPos, yPos) == GUI_MOUSEDOWN_NOHIT)
							   {
							   }

		}break;

		case WM_LBUTTONUP:
		{
							 float xPos = GET_X_LPARAM(lParam);
							 float yPos = GET_Y_LPARAM(lParam);

							 if (UIPtr->ProcessMouseClick(0, false, xPos, yPos) == GUI_MOUSEUP_NOHIT)
							 {
							 }


		}break;

		case WM_MOUSEWHEEL:
		{

		}break;

		case WM_MOUSEMOVE:
		{
							int gMouseX = GET_X_LPARAM(lParam);
							int gMouseY = GET_Y_LPARAM(lParam);


							 UIPtr->ProcessMousePosition(gMouseX, gMouseY);



		}break;

		case WM_PAINT:
		{

						 //not used, directdraw takes over
						 hdc = BeginPaint(hwnd, &ps);
						 EndPaint(hwnd, &ps);
						 return(0);

		} break;

		case WM_KEYDOWN: //any keyboard press generates this message
		{
							 UIPtr->ProcessKeyboard(wParam);

		}break;

		case WM_KEYUP:
		{

		}break;

		case WM_DESTROY: //window is destoyed
		{
							 PostQuitMessage(0);
							 return(0);
		} break;


		default: break;

		} // end switch

		// process any messages that we didn't take care of
		return (DefWindowProc(hwnd, msg, wParam, lParam));

	}
}