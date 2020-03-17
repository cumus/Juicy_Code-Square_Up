#include "TextureManager.h"

#include "Application.h"
#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "optick-1.3.0.0/include/optick.h"

#include "SDL2_image-2.0.5/include/SDL_image.h"
#ifdef PLATFORMx86
#pragma comment( lib, "SDL2_image-2.0.5/lib/x86/SDL2_image.lib" )
#elif PLATFORMx64
#pragma comment( lib, "SDL2_image-2.0.5/lib/x64/SDL2_image.lib" )
#endif

void TextureManager::LoadConfig(bool empty_config)
{
	pugi::xml_node config = FileManager::ConfigNode();

	if (empty_config)
	{
		pugi::xml_node img_flags = config.append_child("textures").append_child("flags");
		img_flags.append_attribute("jpg").set_value(using_jpg);
		img_flags.append_attribute("png").set_value(using_png);
		img_flags.append_attribute("tif").set_value(using_tif);
		img_flags.append_attribute("webp").set_value(using_webp);
	}
	else
	{
		pugi::xml_node img_flags = config.child("textures").child("flags");
		using_jpg = img_flags.attribute("jpg").as_bool(using_jpg);
		using_png = img_flags.attribute("png").as_bool(using_png);
		using_tif = img_flags.attribute("tif").as_bool(using_tif);
		using_webp = img_flags.attribute("webp").as_bool(using_webp);
	}
}

void TextureManager::SaveConfig() const
{
	pugi::xml_node config = FileManager::ConfigNode();
	pugi::xml_node img_flags = config.child("textures").child("flags");
	img_flags.attribute("jpg").set_value(using_jpg);
	img_flags.attribute("png").set_value(using_png);
	img_flags.attribute("tif").set_value(using_tif);
	img_flags.attribute("webp").set_value(using_webp);
}

bool TextureManager::Init()
{
	bool ret;
	int flags = 0;
	if (using_jpg) flags |= IMG_INIT_JPG;
	if (using_png) flags |= IMG_INIT_PNG;
	if (using_tif) flags |= IMG_INIT_TIF;
	if (using_webp) flags |= IMG_INIT_WEBP;

	int init = IMG_Init(flags);
	if (ret = ((init & flags) == flags))
		LOG("SDL_IMG initialized.");
	else
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());

	return ret;
}

// Load texture from file path
int TextureManager::Load(const char* path)
{
	OPTICK_EVENT();

	int ret = -1;

	for (std::vector<TextureData>::const_iterator it = texture_data.cbegin(); it != texture_data.cend(); ++it)
	{
		if (it->source == path)
		{
			ret = it->id;
			LOG("Texture already loaded: %s", it->source);
		}
	}

	if (ret < 0)
	{
		SDL_Surface* surface = IMG_Load_RW(App->files.LoadRWops(path), 1);

		if (surface)
		{
			SDL_Texture* texture = SDL_CreateTextureFromSurface(App->render->GetSDLRenderer(), surface);

			if (texture)
			{
				TextureData data;
				SDL_QueryTexture(texture, 0, 0, &data.width, &data.height);
				data.source = path;
				data.id = textures.size();

				ret = data.id;
				texture_data.push_back(data);
				textures.push_back(texture);

				SDL_FreeSurface(surface);

				LOG("Loaded surface with path: %s", path);
			}
			else
				LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		}
		else
			LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}

	return ret;
}

void TextureManager::CleanUp()
{
	LOG("Freeing textures");

	texture_data.clear();

	for (std::vector<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		SDL_DestroyTexture(*it);

	textures.clear();

	IMG_Quit();
}

int TextureManager::LoadSurface(SDL_Surface* surface)
{
	int ret = -1;

	SDL_Texture* tex = SDL_CreateTextureFromSurface(App->render->GetSDLRenderer(), surface);

	if (tex != nullptr)
	{
		TextureData data;
		SDL_QueryTexture(tex, 0, 0, &data.width, &data.height);
		data.source = "From SDL_Surface";
		data.id = textures.size();

		ret = data.id;
		texture_data.push_back(data);
		textures.push_back(tex);
	}
	else
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());

	return ret;
}

bool TextureManager::GetTextureData(int id, TextureData& data) const
{
	bool ret;

	if (ret = (id >= 0 && id < int(textures.size())))
		data = texture_data[id];

	return ret;
}

SDL_Texture * TextureManager::GetTexture(int id) const
{
	SDL_Texture* ret = nullptr;

	if (id >= 0 && id < int(textures.size()))
		ret = textures[id];

	return ret;
}

TextureData::TextureData() :
	id(0),
	width(0),
	height(0),
	source("none")
{}

TextureData::TextureData(const TextureData& copy) :
	id(copy.id),
	width(copy.width),
	height(copy.height),
	source(copy.source)
{}
