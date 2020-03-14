#include "FontManager.h"
#include "Application.h"
#include "TextureManager.h"
#include "Log.h"

#include "SDL/include/SDL.h"

#include "SDL2_ttf-2.0.15/include/SDL_ttf.h"
#ifdef PLATFORMx86
#pragma comment( lib, "SDL2_ttf-2.0.15/lib/x86/SDL2_ttf.lib" )
#elif PLATFORMx64
#pragma comment( lib, "SDL2_ttf-2.0.15/lib/x64/SDL2_ttf.lib" )
#endif

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

bool FontManager::Init()
{
	bool ret = (TTF_Init() == 0);

	if (!ret) LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

	return ret;
}

bool FontManager::CleanUp()
{
	for (std::vector<TTF_Font*>::iterator item = fonts.begin(); item != fonts.cend(); item++)
		TTF_CloseFont(*item);

	fonts.clear();

	TTF_Quit();

	return true;
}

_TTF_Font* const FontManager::Load(const char* path, int size)
{
	TTF_Font* font = nullptr;

	if (path != nullptr)
	{
		font = TTF_OpenFontRW(App->files.Load(path), 1, size);

		if (font != nullptr)
			fonts.push_back(font);
		else
			LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}

	return font;
}

SDL_Texture* FontManager::RenderText(const char* text, unsigned int wrap_length, int font_id, int r, int g, int b, int a)
{
	SDL_Texture* ret = nullptr;

	if (!fonts.empty())
	{
		if (font_id < 0 || font_id > fonts.size())
			font_id = 0;

		_TTF_Font* font = fonts[font_id];

		if (font != nullptr)
		{
			SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, { unsigned char(r), unsigned char(g), unsigned char(b), unsigned char(a) }, wrap_length);

			if (surface != nullptr)
			{
				ret = App->tex.GetTexture(App->tex.LoadSurface(surface));
				SDL_FreeSurface(surface);
			}
			else
				LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
	}
	else
		LOG("Unable to render text! No fonts loaded.");

	return ret;
}

bool FontManager::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	if (font != nullptr && TTF_SizeText(font, text, & width, & height) == 0)
		ret = true;
	else
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());

	return ret;
}
