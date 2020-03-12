#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__

#include "SDL\include\SDL_pixels.h"
#include <vector>

struct SDL_Texture;
struct _TTF_Font;

class FontManager
{
public:

	FontManager();
	~FontManager();

	bool Init();
	bool CleanUp();

	_TTF_Font* const Load(const char* path, int size = 12);

	SDL_Texture* RenderText(const char* text, int new_line, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	std::vector<_TTF_Font*> fonts;
};

#endif // __FONT_MANAGER_H__