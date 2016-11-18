#include "UserInterface.h"
#include "SoftwareRasterizer.h"


namespace ModelBuilder
{


	void Init();
	void Main(DWORD* mem, int lpitch32, float timeDelta);
	void Shutdown();
	UserInterface* GetGUI();

}