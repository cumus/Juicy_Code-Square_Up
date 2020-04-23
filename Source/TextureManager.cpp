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

int TextureData::texture_count = 0;

TextureData::TextureData() :
	id(++texture_count),
	width(0),
	height(0),
	source("none"),
	texture(nullptr)
{}

TextureData::TextureData(const TextureData& copy) :
	id(copy.id),
	width(copy.width),
	height(copy.height),
	source(copy.source),
	texture(copy.texture)
{}

TextureData::~TextureData()
{}

void TextureData::ClearTexture()
{
	if (texture != nullptr)
	{
		width = height = 0;
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}

bool TextureData::ReloadSurface(SDL_Surface* surface)
{
	bool ret = false;

	ClearTexture();

	texture = SDL_CreateTextureFromSurface(App->render->GetSDLRenderer(), surface);
	if (texture != nullptr)
	{
		if (!(ret = (SDL_QueryTexture(texture, 0, 0, &width, &height) == 0)))
			LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());

	return ret;
}

TextureManager::TextureManager()
{}

TextureManager::~TextureManager()
{
	if (!textures.empty())
		CleanUp();
}

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

void TextureManager::CleanUp()
{
	LOG("Freeing textures");

	for (std::map<int, TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
		it->second.ClearTexture();

	textures.clear();

	IMG_Quit();
}

// Load texture from file path
int TextureManager::Load(const char* path)
{
	OPTICK_EVENT();

	int ret = -1;
	for (std::map<int, TextureData>::const_iterator it = textures.cbegin(); it != textures.cend(); ++it)
	{
		if (it->second.source == path)
		{
			ret = it->second.id;
			LOG("Texture already loaded: %s", it->second.source);
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
				data.texture = texture;
				textures.insert({ ret = data.id, data });

				LOG("Loaded surface with path: %s", path);
			}
			else
				LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());

			SDL_FreeSurface(surface);
		}
		else
			LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}

	return ret;
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
		data.texture = tex;
		textures.insert({ ret = data.id, data });
	}
	else
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());

	return ret;
}

TextureData* TextureManager::CreateEmpty()
{
	TextureData data;
	return &textures.insert({ data.id, data }).first->second;
}

int TextureManager::CreateEmptyTexture(SDL_Renderer* r, int width, int height, const char* source)
{
	int ret = -1;
	TextureData data;

	if ((data.texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, data.width = width, data.height = height)) != nullptr)
	{
		/*if (SDL_SetTextureBlendMode(data.texture, SDL_BLENDMODE_BLEND) == 0)
		{*/
			data.source = source;
			textures.insert({ ret = data.id, data });
		/*}
		else
			LOG("Unable to SDL_SetTextureBlendMode to SDL_BLENDMODE_BLEND! SDL Error: %s\n", SDL_GetError());*/
	}
	else
		LOG("Unable to create texture ! SDL Error: %s\n", SDL_GetError());
		

	return ret;
}

bool TextureManager::Remove(int id)
{
	bool ret = false;

	std::map<int, TextureData>::iterator it = textures.find(id);

	if (ret = (it != textures.end()))
		textures.erase(it);

	return ret;
}

bool TextureManager::GetTextureData(int id, TextureData& data) const
{
	bool ret = false;

	std::map<int, TextureData>::const_iterator it = textures.find(id);

	if (ret = (it != textures.end()))
		data = it->second;

	return ret;
}

SDL_Texture * TextureManager::GetTexture(int id) const
{
	SDL_Texture* ret = nullptr;

	std::map<int, TextureData>::const_iterator it = textures.find(id);

	if (it != textures.cend())
		ret = it->second.texture;

	return ret;
}

TextureData* TextureManager::GetDataPtr(int id)
{
	TextureData* ret = nullptr;

	std::map<int, TextureData>::iterator it = textures.find(id);

	if (it != textures.end())
		ret = &it->second;

	return ret;
}
