#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__

#include <string>
#include <vector>

struct SDL_Texture;
struct _TTF_Font;

struct FontData
{
	FontData();
	FontData(const FontData& copy);

	int id, size;
	std::string source;
};

class FontManager
{
public:

	FontManager();
	~FontManager();

	bool Init();
	bool CleanUp();

	int Load(const char* path, int size = 12);

	_TTF_Font* GetFont(int id = -1) const;

	SDL_Texture* RenderText(const char* text, unsigned int wrap_length, int font_id = -1, int r = 0, int g = 0, int b = 0, int a = 256)  const;

	bool CalcSize(const char* text, int& width, int& height, int font_id = -1) const;
	
public:

	std::vector<FontData> fonts_data;
	std::vector<_TTF_Font*> fonts;
};

#endif // __FONT_MANAGER_H__