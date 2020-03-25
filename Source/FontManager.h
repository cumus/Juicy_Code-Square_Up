#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__

#include "SDL/include/SDL_pixels.h"

#include <string>
#include <vector>

struct SDL_Texture;
struct _TTF_Font;
struct TextureData;

struct FontData
{
	FontData();
	FontData(const FontData& copy);

	int id, size;
	std::string source;
};

class RenderedText
{
public:
	RenderedText(const char* content = "sample text", int font_id = -1, SDL_Color color = { 0, 0, 0, 255 }, unsigned int wrap_length = 1000u);
	~RenderedText();

	const char* GetText() const;
	void SetText(const char* t);

	SDL_Texture* GetTexture();

	bool GetSize(int& width, int& height) const;

private:

	bool BlitTexture();

private:

	bool needs_redraw = true;

	std::string text;
	int font_id = -1;

	TextureData* texture = nullptr;

	int width = -1;
	int height = -1;
	unsigned int wrap_length = 1000u;
	SDL_Color color = { 0, 0, 0, 255 };
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
	
public:

	std::vector<FontData> fonts_data;
	std::vector<_TTF_Font*> fonts;
};

#endif // __FONT_MANAGER_H__