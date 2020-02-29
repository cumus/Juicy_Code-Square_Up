#include "TextureManager.h"

#include "Application.h"
#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "SDL_image/include/SDL_image.h"

void TextureManager::CleanUp()
{
	LOG("Freeing textures");

	sprites.clear();

	for (std::vector<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		SDL_DestroyTexture(*it);

	textures.clear();
}

// Load texture from file path
int TextureManager::Load(const char* path)
{
	int ret = -1;

	for (std::vector<Sprite>::iterator it = sprites.begin(); it != sprites.end(); ++it)
	{
		if (it->source == path)
		{
			ret = it->id;
			LOG("Texture already loaded: %s", it->source);
		}
	}

	if (ret < 0)
	{
		SDL_Surface* surface = IMG_Load(path);

		if (surface)
		{
			SDL_Texture* texture = SDL_CreateTextureFromSurface(App->render->renderer, surface);

			if (texture)
			{
				Sprite sprite;
				SDL_QueryTexture(texture, 0, 0, &sprite.width, &sprite.height);
				sprite.source = path;
				sprite.id = textures.size();

				ret = sprite.id;
				sprites.push_back(sprite);
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

bool TextureManager::GetSprite(int id, Sprite& spite) const
{
	bool ret;

	if (ret = (id >= 0 && id < textures.size()))
		spite = sprites[id];

	return ret;
}

SDL_Texture * TextureManager::GetTexture(int id) const
{
	SDL_Texture* ret = nullptr;

	if (id >= 0 && id < textures.size())
		ret = textures[id];

	return ret;
}

Sprite::Sprite() :
	id(0),
	width(0),
	height(0),
	source("none")
{}

Sprite::Sprite(const Sprite& copy) :
	id(copy.id),
	width(copy.width),
	height(copy.height),
	source(copy.source)
{}
