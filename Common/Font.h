#pragma once
#include <Windows.h>
#include "../FreeType/include/ft2build.h"
#include FT_FREETYPE_H
#include <string>;
#include <map>



class Font
{
public: 
	Font(std::string fontPath, int fontWidth,  int fontHeight);
	int Draw(DWORD* video_mem, int lpitch32, std::string outText, int xpos, int ypos, DWORD* color = NULL);

	int GetFontHieght() const;
	int GetFontWidth() const;
	int GetCharKey(char);

	int GetStringPixelLength(std::string str);


private:
	FT_Library mFTLib;
	FT_Face mFTFace;
	std::map<char, int> mCharImageMap;

	int mCharWidth;
	int mCharHeight;
	

};