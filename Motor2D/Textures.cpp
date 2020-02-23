#include "Textures.h"

#include "Application.h"
#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "SDL_image/include/SDL_image.h"

Textures::Textures()
{}

Textures::~Textures()
{}

// Called before quitting
void Textures::CleanUp()
{
	LOG("Freeing textures");

	for (std::list<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		SDL_DestroyTexture(*it);

	textures.clear();
}

// Load new texture from file path
SDL_Texture* const Textures::Load(const char* path)
{
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = IMG_Load(path);

	if(surface == nullptr)
	{
		LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

// Unload texture
bool Textures::UnLoad(SDL_Texture* texture)
{
	bool ret = false;

	for (std::list<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (*it == texture)
		{
			SDL_DestroyTexture(*it);
			textures.remove(*it);
			ret = true;
			break;
		}
	}

	return ret;
}

// Translate a surface into a texture
SDL_Texture* const Textures::LoadSurface(SDL_Surface* surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(App->render->renderer, surface);

	if(texture)
		textures.push_back(texture);
	else
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());

	return texture;
}

// Retrieve size of a texture
void Textures::GetSize(const SDL_Texture* texture, unsigned int& width, unsigned int& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, 0, 0, (int*) &width, (int*) &height);
}
