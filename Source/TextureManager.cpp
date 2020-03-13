#include "TextureManager.h"

#include "Application.h"
#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "Optick/include/optick.h"

#include "SDL2_image-2.0.5/include/SDL_image.h"

void TextureManager::CleanUp()
{
	LOG("Freeing textures");

	texture_data.clear();

	for (std::vector<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		SDL_DestroyTexture(*it);

	textures.clear();
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

// Load texture from file path
int TextureManager::Load(const char* path, bool relative_path)
{
	OPTICK_EVENT();

	int ret = -1;

	std::string tex_path;
	if (relative_path)
		tex_path = App->files.GetBasePath();

	tex_path += path;

	for (std::vector<TextureData>::iterator it = texture_data.begin(); it != texture_data.end(); ++it)
	{
		if (it->source == path)
		{
			ret = it->id;
			LOG("Texture already loaded: %s", it->source);
		}
	}

	if (ret < 0)
	{
		SDL_Surface* surface = IMG_Load(tex_path.c_str());

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

				LOG("     Loaded surface with path: %s", path);
			}
			else
				LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		}
		else
			LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}

	return ret;
}

bool TextureManager::GetTextureData(int id, TextureData& data) const
{
	bool ret;

	if (ret = (id >= 0 && id < textures.size()))
		data = texture_data[id];

	return ret;
}

SDL_Texture * TextureManager::GetTexture(int id) const
{
	SDL_Texture* ret = nullptr;

	if (id >= 0 && id < textures.size())
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
