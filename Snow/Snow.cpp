#include "Snow.h"

vector<flake> snowFlakes;
int flake_count = 50000;
int y_spread = SCREEN_HEIGHT - 1;
float pixPerSec = 60;
float timeElapsed = 0;
//Camera2D camera;
BITMAP_FILE OceanBMP;
//UserInterface myUI;



int oldwater[WATER_WIDTH*WATER_HEIGHT];
int newwater[WATER_WIDTH*WATER_HEIGHT];
int copywater[WATER_WIDTH*WATER_HEIGHT];

/*
Camera2D* GetCamera()
{
	//return &camera;
}

UserInterface* GetUI()
{
	return &myUI;
}
*/



void SnowGameInit()
{
	
	//camera = cam;

	flake f;
	for (int i = 0; i < flake_count; i++)
	{
//		VECTOR2D camPos = camera.GetPosition();
		f.xpos = rand() % (SCREEN_WIDTH - 1);
		f.ypos = (rand() % y_spread);
		f.color = COLOR_RED;
		f.dir = VECTOR2D(f.xpos, f.ypos);
		f.screenX = f.xpos;
		f.screenY = f.ypos;

		f.dir.Normalize();
		snowFlakes.push_back(f);
	}
}

void SnowGameMain(DWORD* mem_buffer, int lpitch32, double timeDelta)
{
	for (int i = 0; i < snowFlakes.size(); i++)
	{
		//move in the direction
		snowFlakes[i].ypos =  snowFlakes[i].ypos + snowFlakes[i].dir.y*pixPerSec/1000*timeDelta;
		snowFlakes[i].xpos =  snowFlakes[i].xpos + snowFlakes[i].dir.x*pixPerSec / 1000 * timeDelta;


//		snowFlakes[i].screenX = snowFlakes[i].xpos - camera.GetPosition().x;
//		snowFlakes[i].screenY = snowFlakes[i].ypos - camera.GetPosition().y;

		//draw flake if visible
//		if (camera.GetViewPortRect().isPointInRect(snowFlakes[i].xpos, snowFlakes[i].ypos))
//		{
		
	
	//		mem_buffer[int(snowFlakes[i].screenX + 0.5f) + int(snowFlakes[i].screenY  + 0.5f)*lpitch32] = snowFlakes[i].color;
//		}
			
		
		snowFlakes[i].dir = VECTOR2D(-(snowFlakes[i].ypos), snowFlakes[i].xpos);
		snowFlakes[i].dir.Normalize();
		
		
	}


}

void SnowGameShutdown()
{
	snowFlakes.clear();
}


void drop(int x, int y, int size, int strength, int width, int height)
{
	if (x < 0 || y < 0 || x >= WATER_WIDTH || y >= WATER_HEIGHT)
		return;

	int xstart = x - size;
	int xend = x + size;
	int ystart = y - size;
	int yend = y + size;

	if (xstart < 0)
		xstart = 0;
	if (xend >= WATER_WIDTH)
		xend = WATER_WIDTH - 1;
	if (ystart < 0)
		ystart = 0;
	if (ystart >= WATER_HEIGHT)
		ystart = WATER_HEIGHT - 1;

	int radSqrd = size*size;
	for (int xcur = xstart; xcur < xend; xcur++)
	{
		for (int ycur = ystart; ycur < yend; ycur++)
		{
			if ((xcur - x)*(xcur - x) + (ycur - y)*(ycur - y) <= radSqrd)
				oldwater[xcur + ycur*WATER_WIDTH] += int(float(strength)*sqrt((xcur - x)*(xcur - x) + (ycur - y)*(ycur - y)) + 0.5f);
		}
	}


}


void OceanInit()
{

	//load background
	string oceanBcg("..//Resource//OceanBcg.bmp");
	LoadBitmapFromDisk(oceanBcg, &OceanBMP);
	//oldwater = new int[WATER_HEIGHT*WATER_WIDTH];
	//newwater = new int[WATER_HEIGHT*WATER_WIDTH];

	memset(oldwater, 0, sizeof(int)*WATER_HEIGHT*WATER_WIDTH);
	memset(newwater, 0, sizeof(int)*WATER_HEIGHT*WATER_WIDTH);

	flake f;
	for (int x = 0, xcoord = 0; x < WATER_WIDTH; x += 1, xcoord += 3)
	{
		for (int y = 0, ycoord = 0; y < WATER_HEIGHT; y += 1, ycoord += 3)
		{
			f.xpos = xcoord;
			f.ypos = ycoord;
			f.color = COLOR_RED;
			snowFlakes.push_back(f);

		}

	}

}

void OceanMain(DWORD* mem_buffer, int lpitch32, double timeDelta)
{
	/*	if (timeElapsed > 1000)
	timeElapsed = 0;
	else
	{
	timeElapsed += timeDelta;
	goto oceanDrawLoop;
	}
	*/

	myUI.DrawUI(mem_buffer, lpitch32, 0);
	myUI.displayString(timeDelta, 10, 10, mem_buffer, lpitch32);
	if (timeDelta)
		myUI.displayString(1000 / timeDelta, 100, 10, mem_buffer, lpitch32);

	for (int x = 0; x < WATER_WIDTH; x++)
	{
		for (int y = 0; y < WATER_HEIGHT; y++)
		{
			int xm1 = x - 1;
			int xp1 = x + 1;
			int ym1 = y - 1;
			int yp1 = y + 1;

			if (xm1 < 0)
				xm1 = 0;

			if (xp1 >= WATER_WIDTH)
				xp1 = WATER_WIDTH - 1;

			if (ym1 < 0)
				ym1 = 0;

			if (yp1 >= WATER_HEIGHT)
				yp1 = WATER_HEIGHT - 1;

			float valSum = oldwater[xm1 + y*WATER_WIDTH];
			valSum += oldwater[xp1 + y*WATER_WIDTH];
			valSum += oldwater[x + ym1*WATER_WIDTH];
			valSum += oldwater[x + yp1*WATER_WIDTH];
			
			valSum /= 2.0f;

			valSum -= newwater[x + y*WATER_WIDTH];
			valSum /= 1.00001;

			newwater[x + y*WATER_WIDTH] = int(valSum + 0.5f);
		}
	}

	for (int i = 0; i < snowFlakes.size(); i++)
	{
//		snowFlakes[i].screenX = snowFlakes[i].xpos - camera.GetPosition().x;
//		snowFlakes[i].screenY = snowFlakes[i].ypos - camera.GetPosition().y;

		if (newwater[i] == 0)
			snowFlakes[i].color = COLOR_WHITE;
		else
			snowFlakes[i].color = COLOR_RED;

		//snowFlakes[i].color = _RGB32BITX888(0, 0, newwater[i]);

		//draw flake if visible
//		if (camera.GetViewPortRect().isPointInRect(snowFlakes[i].xpos, snowFlakes[i].ypos))
//		{

//			mem_buffer[int(snowFlakes[i].screenX + 0.5f) + int(snowFlakes[i].screenY + 0.5f)*lpitch32] = snowFlakes[i].color;
//		}
	}

	memcpy(copywater, newwater, sizeof(int)*WATER_WIDTH*WATER_HEIGHT);
	memcpy(newwater, oldwater, sizeof(int)*WATER_WIDTH*WATER_HEIGHT);
	memcpy(oldwater, copywater, sizeof(int)*WATER_WIDTH*WATER_HEIGHT);


}


void OceanShutdown()
{
	snowFlakes.clear();
}


