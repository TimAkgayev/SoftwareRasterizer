#pragma once
#include <Windows.h>
#include "../FreeType/include/ft2build.h"
#include FT_FREETYPE_H
#include <string>;
#include <map>



class Font
{
public: 
	Font(std::string, int);
	void Draw(DWORD* video_mem, int lpitch32, std::string outText, int xpos, int ypos, DWORD* color = NULL);


	int GetCharKey(char);


private:
	FT_Library mFTLib;
	FT_Face mFTFace;
	std::map<char, int> mCharImageMap;

};