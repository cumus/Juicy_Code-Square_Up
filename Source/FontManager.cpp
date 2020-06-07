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
{}

FontManager::~FontManager()
{}

bool FontManager::Init()
{
	bool ret = (TTF_Init() == 0);

	if (ret)
	{
		LOG("SDL_ttf initialized!");

		// Load default font
		ret = (
			Load("fonts/OpenSans-Regular.ttf") >= 0 &&
			Load("fonts/OpenSans-Regular.ttf", 56) >= 0) &&
			Load("fonts/BEBAS__.ttf") >= 0;
	}
	else
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

	return ret;
}

bool FontManager::CleanUp()
{
	for (std::vector<TTF_Font*>::iterator item = fonts.begin(); item != fonts.end(); item++)
		TTF_CloseFont(*item);

	fonts.clear();
	fonts_data.clear();

	TTF_Quit();

	return true;
}

int FontManager::Load(const char* path, int size)
{
	int ret = -1;

	if (path != nullptr)
	{
		for (std::vector<FontData>::const_iterator it = fonts_data.cbegin(); it != fonts_data.cend(); ++it)
		{
			if (it->source == path && it->size == size)
			{
				ret = it->id;
				LOG("Font already loaded: %s, size: %d", path, size);
			}
		}

		if (ret < 0)
		{
			TTF_Font* font = TTF_OpenFontRW(App->files.LoadRWops(path), 1, size);

			if (font != nullptr)
			{
				FontData data;
				ret = data.id = fonts.size();
				data.source = path;
				data.size = size;

				fonts_data.push_back(data);
				fonts.push_back(font);

				LOG("Loaded TTF font size %d with path: %s", size, path);
			}
			else
				LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
		}
	}

	return ret;
}

_TTF_Font* FontManager::GetFont(int id) const
{
	_TTF_Font* ret = nullptr;

	if (!fonts.empty())
	{
		if (id < 0 || id >= int(fonts.size()))
			id = 0;

		ret = fonts[id];
	}

	return ret;
}

FontData::FontData() : id(-1), size(-1), source("none")
{}

FontData::FontData(const FontData& copy) : id(copy.id), size(copy.size), source(copy.source)
{}

RenderedText::RenderedText(const char* content, int font_id, SDL_Color color, unsigned int wrap_length) :
	text(content), wrap_length(wrap_length), color(color), texture(nullptr)
{
	texture = App->tex.CreateEmpty();
	needs_redraw = true;
	BlitTexture();
}

RenderedText::~RenderedText()
{
	if (texture != nullptr)
		App->tex.Remove(texture->id);
}

const char* RenderedText::GetText() const
{
	return text.c_str();
}

void RenderedText::SetText(const char* t)
{
	if (t != nullptr && text != t)
	{
		text = t;
		needs_redraw = true;
	}
}

SDL_Texture* RenderedText::GetTexture()
{
	SDL_Texture* ret = nullptr;

	if (texture != nullptr)
	{
		if (needs_redraw)
			BlitTexture();

		ret = texture->texture;
	}

	return ret;
}

bool RenderedText::GetSize(int& w, int& h) const
{
	bool ret;

	if (ret = (width > 0 && height > 0))
	{
		w = width;
		h = height;
	}

	return ret;
}

bool RenderedText::BlitTexture()
{
	_TTF_Font* font = App->fonts.GetFont(font_id);
	if (font != nullptr)
	{
		if (TTF_SizeText(font, text.c_str(), &width, &height) == 0)
		{
			SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, wrap_length);

			if (surface != nullptr)
			{
				if (texture->ReloadSurface(surface))
					needs_redraw = false;
				else
					LOG("Unable to reload texture from text surface!", TTF_GetError());

				SDL_FreeSurface(surface);
			}
			else
				LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
			LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
		LOG("Unable to render text! No fonts loaded.");

	return !needs_redraw;
}
