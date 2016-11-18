}#pragma once

void DrawMandelbrotSet2(DWORD* memory, int lpitch32, float& campos_x, float& campos_y, float& chaseX, float& chaseY, float& zoomScale)
{
	//plotting the object in imag plane
	float cx, cy;
	float zx, zy;

	float sample_rate_x = 1.0f / (zoomScale * float(SCREEN_WIDTH));
	float sample_rate_y = 1.0f / (zoomScale * float(SCREEN_WIDTH));

	zoomScale += 15 * sample_rate_x;

	//finite coordinates
	cx = -2.0f + campos_x*sample_rate_x;
	cy = -1.0f + campos_y*sample_rate_y;


	for (int screen_y = 0; screen_y < SCREEN_HEIGHT; screen_y++)
	{
		for (int screen_x = 0; screen_x < SCREEN_WIDTH; screen_x++)
		{
			//z starts over every new pixel
			zx = 0.0f;
			zy = 0.0f;

			//iteration at every pixel
			int iter_count;
			int MAX_ITERATIONS = 100;
			for (iter_count = 0; iter_count < MAX_ITERATIONS; iter_count++)
			{
				float znextx = (zx * zx - zy * zy) + cx;
				float znexty = 2 * zx*zy + cy;

				if (znextx * znextx + znexty*znexty > 2 * 2)
				{
					break;
				}

				zx = znextx;
				zy = znexty;
			}

			//plotting the pixel
			if (iter_count == MAX_ITERATIONS)
			{
				DWORD color = COLOR_WHITE;//_RGB32BITX888(iterNum/MAX_ITER * 255, iterNum/MAX_ITER * 255, iterNum/MAX_ITER * 255);
				Plot_Pixel32(screen_x, screen_y, color, memory, lpitch32);
				/*				if(prevPixUnboud)
				{


				chaseX = screen_x;
				chaseY = screen_y;
				prevPixUnboud = false;
				}*/
			}
			else
			{
				//choose color based on escape velocity
				float scale = float(iter_count) / float(MAX_ITERATIONS);



				DWORD color = _RGB32BITX888(int(scale * 255 + 0.5), int(scale * 255 + 0.5), int(scale * 255 + 0.5));
				Plot_Pixel32(screen_x, screen_y, color, memory, lpitch32);

				//				prevPixUnboud = true;
			}

			cx += sample_rate_x;
		}
		cy += sample_rate_y;
		cx = -2.0f + campos_x*sample_rate_x;

	}

	//update camera
	//campos_x += ((chaseX - campos_x) ) ;
	//campos_y += ((chaseY - campos_y) ) ;

}


void DrawMandelbrotSet(DWORD* memory, int lpitch32, float campos_x, float campos_y, float zoomScale)
{
	float cx, cy; //c = x + iy, components of c, cx is real , cy is imaginary
				  //	float zx, zy;
	const int MAX_ITERATIONS = 100;
	int iter_count;
	DWORD color;
	float x_samplerate = zoomScale / float(SCREEN_WIDTH);
	float y_samplerate = zoomScale / float(SCREEN_WIDTH);

	cx = -2.0f + campos_x*x_samplerate; //starting condition, 0 x on screen
	cy = -1.0f + campos_y*y_samplerate; //corresponds to 0 y on the screen
	float zx = 0.0f;
	float zy = 0.0f;
	float znextx, znexty;

	for (int screen_y = 0; screen_y < SCREEN_HEIGHT; screen_y++)
	{
		for (int screen_x = 0; screen_x < SCREEN_WIDTH; screen_x++)
		{
			zx = 0.0f;
			zy = 0.0f;
			for (iter_count = 0; iter_count < MAX_ITERATIONS; iter_count++)
			{

				znextx = (zx*zx - zy*zy) + cx;
				znexty = cy + 2 * zx*zy;
				zx = znextx;
				zy = znexty;

				//check boundary
				if (zx*zx + zy*zy > 2 * 2)
					break;


			}



			//point is in the set, it didn't escape
			if (iter_count == MAX_ITERATIONS)
			{
				color = COLOR_WHITE;//_RGB32BITX888(iterNum/MAX_ITER * 255, iterNum/MAX_ITER * 255, iterNum/MAX_ITER * 255);
				Plot_Pixel32(screen_x, screen_y, color, memory, lpitch32);
			}
			else
			{
				//choose color based on escape velocity
				float scale = float(iter_count) / float(MAX_ITERATIONS);

				color = _RGB32BITX888(int(scale * 255 + 0.5), int(scale * 255 + 0.5), int(scale * 255 + 0.5));
				Plot_Pixel32(screen_x, screen_y, color, memory, lpitch32);
			}

			//decrement because were going left to right the x variable in relation to the screen
			cx += x_samplerate;
		}

		//now th y variable
		cy += y_samplerate;
		cx = -2.0 + campos_x*x_samplerate; //ing x back to 0 on screen space
	}


	return;

