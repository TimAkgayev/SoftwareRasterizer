#include "Font.h"
#include "ImageManager.h"
#include "SoftwareRasterizer.h"

Font::Font(std::string fontPath, int fontSize)
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

	FT_Set_Char_Size(mFTFace, 0, fontSize * 64, 100, 100);
	mHeight = fontSize;
	

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

void Font::Draw(DWORD* video_mem,  int lpitch32, std::string outText, int xpos, int ypos, DWORD* color)
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

		int pen_x_offset = slot->bitmap_left;
		int pen_y_offset = 0;

		if (slot->bitmap_top < mHeight)
			pen_y_offset =  slot->bitmap_top;

		if (outText == "Sec MODE")
			int x = 0;

		ImageManager::GetImage(GetCharKey(c))->Draw(video_mem, lpitch32, pen_x + slot->bitmap_left , pen_y - slot->bitmap_top + mHeight, color);
	//	ImageManager::GetImage(GetCharKey(c))->Draw(video_mem, lpitch32, pen_x + pen_x_offset, pen_y + pen_y_offset, color);
	//	ImageManager::GetImage(GetCharKey(c))->Draw(video_mem, lpitch32, pen_x, pen_y, color);


		pen_x += slot->advance.x >> 6;
		previous = glyph_index;
	}


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
	return mHeight;
}

int Font::GetCharKey(char c)
{
	return mCharImageMap[c];
}

