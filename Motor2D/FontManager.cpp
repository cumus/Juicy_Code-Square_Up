#include "FontManager.h"
#include "Application.h"
#include "TextureManager.h"
#include "Log.h"

//#include "SDL/include/SDL.h"
#include "SDL2_ttf-2.0.15/include/SDL_ttf.h"
//#pragma comment( lib, "SDL2_ttf-2.0.15/lib/x86/SDL2_ttf.lib" )

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

bool FontManager::Init()
{
	/*bool ret = (TTF_Init() == 0);

	if (!ret) LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

	return ret;*/
	return true;
}

bool FontManager::CleanUp()
{
	/*for (std::vector<TTF_Font*>::iterator item = fonts.begin(); item != fonts.cend(); item++)
		TTF_CloseFont(*item);

	fonts.clear();
	TTF_Quit();*/

	return true;
}

_TTF_Font* const FontManager::Load(const char* path, int size)
{
	// TODO: Add SDL_RWops support
	TTF_Font* font = nullptr; // TTF_OpenFontRW(App->fs->Load(path), 1, size);

	if (font != nullptr)
		fonts.push_back(font);
	else
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());

	return font;
}

SDL_Texture* FontManager::RenderText(const char* text, int new_line, int r, int g, int b, int a, _TTF_Font* font)
{
	SDL_Texture* ret = nullptr;

	/*if (font != nullptr)
	{
		SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, color, new_line);

		if (surface != nullptr)
		{
			ret = App->tex->GetTexture(App->tex->LoadSurface(surface));
			SDL_FreeSurface(surface);
		}
		else
			LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}*/

	return ret;
}

bool FontManager::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	/*if (font != nullptr && TTF_SizeText(font, text, & width, & height) == 0)
		ret = true;
	else
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());*/

	return ret;
}
