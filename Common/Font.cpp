#include "Font.h"
#include "ImageManager.h"
#include "SoftwareRasterizer.h"

Font::Font(std::string fontPath, int fontWidth, int fontHeight)
{
	std::string CharList = "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,;'~!@#$%^&*()_+-=\\/";

	FT_Error error;
	FT_GlyphSlot slot;

	error = FT_Init_FreeType(&mFTLib);
	if (error)
	{
		int x = 0;

	}

	error = FT_New_Face(mFTLib, fontPath.c_str(), 0, &mFTFace);

	slot = mFTFace->glyph;

	if (error)
	{
		int x = 0;
	}

	FT_Set_Char_Size(mFTFace, fontWidth * 64, fontHeight * 64, 100, 100);
	mCharWidth = fontWidth;
	
	//get char height of a standard letter A (should be the tallest character)
	FT_UInt heightindex = FT_Get_Char_Index(mFTFace, 'A');
	FT_Load_Glyph(mFTFace, heightindex, FT_LOAD_DEFAULT);
	mCharHeight = mFTFace->glyph->bitmap.rows;
	

	for (char& c : CharList)
	{
		FT_UInt index = FT_Get_Char_Index(mFTFace, c);

		error = FT_Load_Glyph(mFTFace, index, FT_LOAD_DEFAULT);

		if (error)
		{
			int x = 0;
			continue;
		}

		error = FT_Render_Glyph(mFTFace->glyph, FT_RENDER_MODE_NORMAL);
		if (error)
		{
			int x = 0;
			continue;
		}

		FT_Bitmap bmp = slot->bitmap;

		Bitmap* file = new Bitmap(bmp.width, bmp.rows, bmp.buffer);
		int key = ImageManager::AddImage(file);
		mCharImageMap.insert(std::make_pair(c, key));

		
	}

}

int Font::Draw(DWORD* video_mem,  int lpitch32, std::string outText, int xpos, int ypos, DWORD* color)
{
	FT_GlyphSlot slot = mFTFace->glyph;
	bool use_kerning = FT_HAS_KERNING(mFTFace);
	int previous = 0;
	int pen_x = xpos;
	int pen_y = ypos;

	for (char& c : outText)
	{

		FT_UInt glyph_index = FT_Get_Char_Index(mFTFace, c);


		/* retrieve kerning distance and move pen position */
		if (use_kerning && previous && glyph_index)
		{
			FT_Vector  delta;


			FT_Get_Kerning(mFTFace, previous, glyph_index,
				FT_KERNING_DEFAULT, &delta);

			pen_x += delta.x >> 6;
		}

		FT_Load_Glyph(mFTFace, glyph_index, FT_LOAD_DEFAULT);

		ImageManager::GetImage(GetCharKey(c))->Draw(video_mem, lpitch32, pen_x + slot->bitmap_left , pen_y - slot->bitmap_top + mCharHeight, color);

		pen_x += slot->advance.x >> 6;
		previous = glyph_index;
	}

	//return the length of the string we just drew in pixels
	return pen_x - xpos;


}

int Font::GetStringPixelLength(std::string str)
{

	FT_GlyphSlot slot = mFTFace->glyph;
	bool use_kerning = FT_HAS_KERNING(mFTFace);
	int previous = 0;
	int pen_x = 0;


	for (char& c : str)
	{

		FT_UInt glyph_index = FT_Get_Char_Index(mFTFace, c);

		/* retrieve kerning distance and move pen position */
		if (use_kerning && previous && glyph_index)
		{
			FT_Vector  delta;


			FT_Get_Kerning(mFTFace, previous, glyph_index,
				FT_KERNING_DEFAULT, &delta);

			pen_x += delta.x >> 6;
		}

		FT_Load_Glyph(mFTFace, glyph_index, FT_LOAD_DEFAULT);

		pen_x += slot->advance.x >> 6;
		previous = glyph_index;
	}

	return pen_x;
}


int Font::GetFontHieght() const
{
	return mCharHeight;
}

int Font::GetFontWidth() const
{
	return mCharWidth;
}


int Font::GetCharKey(char c)
{
	return mCharImageMap[c];
}

