#include "Bitmap.h"


int LoadBitmapFromDisk(string szFileName, BITMAP_FILE_PTR bitmap)
{

	// Open the bitmap file
	HANDLE hFile = CreateFileA(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	// Read the bitmap file header
	DWORD dwBytesRead;
	BOOL bOK = ReadFile(hFile, &bitmap->fileHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);

	//see if the correct byte number was read and that it's actually a bitmap (bfType has to match)
	if (!bOK || (dwBytesRead != sizeof(BITMAPFILEHEADER)) || (bitmap->fileHeader.bfType != 0x4D42))
	{
		CloseHandle(hFile);
		return 0;
	}
	 

	// Read the bitmap info head
	bOK = ReadFile(hFile, &bitmap->infoHeader, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
	if (!bOK || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
	{
		CloseHandle(hFile);
		return 0;
	}

	// Store the width and height of the bitmap
	int m_iWidth = (int)bitmap->infoHeader.biWidth;
	int m_iHeight = (int)bitmap->infoHeader.biHeight;


	unsigned int sz;
	SetFilePointer(hFile, bitmap->fileHeader.bfOffBits, NULL, FILE_BEGIN);
	bitmap->data = (UCHAR*) new unsigned char[bitmap->infoHeader.biSizeImage];
	bOK = ReadFile(hFile, bitmap->data, bitmap->infoHeader.biSizeImage, &dwBytesRead, NULL);

	//flip the bytes to little endian
	DWORD* temp = (DWORD*)bitmap->data;
	for (int i = 0; i < bitmap->infoHeader.biSizeImage / 4; i++)
	{
		//photoshop saves a,r,g,b into little endian b,g,r,a
		//this engine is expecting r,g,b,a so all that's needed is to flip 1st and 3rd bytes
		//be careful with this code if the image source program is working in r,g,b,a instead of a,r,g,b
		char* byteMem = (char*)&temp[i];
		char btemp = byteMem[0];
		byteMem[0] = byteMem[2];
		byteMem[2] = btemp;

	}

	if (bOK)
		return 1;



	// Something went wrong, so cleanup everything
	return 0;

}

int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
	// this function releases all memory associated with the bitmap
	if (bitmap->data)
	{
		// release memory
		delete (bitmap->data);

		// reset pointer
		bitmap->data = NULL;
	}

	return 1;
}

void FlipBitmap(BITMAP_FILE* bitmap)
{
	if (bitmap->infoHeader.biBitCount == 32)
	{
		DWORD* workingBuffer = (DWORD*) new UCHAR[bitmap->infoHeader.biSizeImage];
		DWORD* workingBufferStart = workingBuffer;
		DWORD* sourceBuffer = (DWORD*)bitmap->data;
		
		//go to the end of the source and go backwards
		sourceBuffer += bitmap->infoHeader.biWidth * (bitmap->infoHeader.biHeight - 1);

		for (int row = bitmap->infoHeader.biHeight - 1; row >= 0; row--)
		{
			memcpy(workingBuffer, sourceBuffer, bitmap->infoHeader.biWidth * (bitmap->infoHeader.biBitCount / 8));
 
			workingBuffer += bitmap->infoHeader.biWidth;
			sourceBuffer -= bitmap->infoHeader.biWidth; 
		}

		memcpy(bitmap->data, workingBufferStart, bitmap->infoHeader.biSizeImage);
		delete[] workingBufferStart;

	}


}

void ResizeBitmap(BITMAP_FILE* out, BITMAP_FILE* original, RECT resizeRectangle)
{
	//only works for 32 bit bitmaps for now
	if (original->infoHeader.biBitCount != 32)
		return;

	int newXDimension = resizeRectangle.right - resizeRectangle.left;
	int newYDimension = resizeRectangle.bottom - resizeRectangle.top;
	int newDataSize = (original->infoHeader.biBitCount / 8) * newXDimension * newYDimension;

	//layout of bmp
	//file header
	//dib header (info header)
	//data

	//first create a new file header
	//everything is the same as the old bitmap except the file size
	BITMAPFILEHEADER newFH = original->fileHeader;
	newFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + newFH.bfOffBits + newDataSize;

	//create a new info header
	BITMAPINFOHEADER newIH = original->infoHeader;
	newIH.biHeight = newYDimension;
	newIH.biWidth = newXDimension;
	newIH.biSizeImage = newDataSize;

	//allocate the data (this is only for 32 bit bitmap)
	DWORD* newData = (DWORD*) new DWORD[newXDimension * newYDimension];


	//calculate the resize ratio in x dimension
	float ratioX = float(newXDimension) / original->infoHeader.biWidth;
	float ratioY = float(newYDimension) / original->infoHeader.biHeight;

	DWORD* newDataOffset = newData;
	DWORD* origDataOffset = (DWORD*) original->data;

	//iterate throught the original bitmap and write to the new one
	for (int row = 0; row < original->infoHeader.biHeight; row++)
	{
		for (int column = 0; column < original->infoHeader.biWidth; column++)
		{
			int destinationRow = int(ratioY * row - 0.5f);
			int destinationPixel = int(ratioX * column - 0.5f);
			newDataOffset[destinationRow* newXDimension + destinationPixel] = origDataOffset[column];
		}

		//newDataOffset += newXDimension;
		origDataOffset += original->infoHeader.biWidth;
	}

	//copy everything back to the output bitmap
	out->fileHeader = newFH;
	out->infoHeader = newIH;
	out->data = (unsigned char*) newData;
	
}
void ResizeBitmap(BITMAP_FILE* out, BITMAP_FILE* original, float xscale, float yscale)
{
	//calculate the size based on percentage
	int newXDim = int(original->infoHeader.biWidth * xscale + 0.5f);
	int newYDim = int(original->infoHeader.biHeight * yscale + 0.5f);
	RECT rc;
	rc.left = 0;
	rc.right = newXDim;
	rc.top = 0;
	rc.bottom = newYDim;

	ResizeBitmap(out, original, rc);
}

void RotateBitmapLeft(BITMAP_FILE* bmp)
{
	//allocate temporary data
	DWORD* tempData = (DWORD*) new char[bmp->infoHeader.biSizeImage];

	//create new info header
	int newXDim = bmp->infoHeader.biHeight;
	int newYDim = bmp->infoHeader.biWidth;

	DWORD* origData = (DWORD*)bmp->data;

	//iterate throught the original bitmap and write to the new one
	for (int row = 0; row < bmp->infoHeader.biHeight; row++)
	{
		for (int columnOld = 0, columnNew = bmp->infoHeader.biWidth - 1; columnOld < bmp->infoHeader.biWidth, columnNew >= 0 ; columnOld++, columnNew--)
		{
			
			tempData[columnNew*newXDim + row] = origData[columnOld];
		}

		//newDataOffset += newXDimension;
		origData += bmp->infoHeader.biWidth;
	}

	//copy over the temp data
	memcpy(bmp->data, tempData, bmp->infoHeader.biSizeImage);
	bmp->infoHeader.biWidth = newXDim;
	bmp->infoHeader.biHeight = newYDim;

	delete tempData;
}