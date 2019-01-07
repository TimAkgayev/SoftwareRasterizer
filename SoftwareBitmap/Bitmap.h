#pragma once
#include <string>
#include <Windows.h>

namespace SoftwareBitmap
{

#define _RGBA32BIT(r, g, b, a) ( (r & 255) + ((g & 255) << 8) + ((b & 255) << 16)  + ((a & 255) << 24) )

	void GetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color);

	class Bitmap
	{
	public:


		Bitmap(int rows, int pitch, unsigned char* buffer);
		Bitmap(int width, int height);
		Bitmap(std::wstring fileName);
		~Bitmap();
		Bitmap& operator=(const Bitmap& rhv);

		void FlipBitmap();
		void ResizeBitmap(RECT resizeRectangle);
		void ResizeBitmap(float xscale, float yscale);
		void RotateBitmapLeft();

		void SetName(std::wstring);

		int GetWidth() const;
		int GetHeight() const;
		int   GetDataSize() const;
		const BITMAPFILEHEADER& GetFileHeader() const;
		const BITMAPINFOHEADER& GetInfoHeader() const;
		UCHAR* GetData() const;
		const PALETTEENTRY* GetPalette() const;
		std::wstring GetName() const;
		int GetPitch() const;

		void Draw(DWORD* dest, int destLPitch32, int destPosX, int destPosY, DWORD* color = NULL, RECT* sourceRegion = NULL);

	private:
		int LoadBitmapFromDisk(std::wstring szFileName);
		int Unload_Bitmap_File();

		BITMAPFILEHEADER fileHeader;
		BITMAPINFOHEADER infoHeader;
		PALETTEENTRY	 palette[256];
		UCHAR*			 data;

		std::wstring name;
		int mPitch;
	


	};

}