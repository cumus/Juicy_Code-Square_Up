#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include <list>

struct SDL_Texture;
struct SDL_Surface;

class Textures
{
public:

	Textures();
	~Textures();

	void CleanUp();

	// Load Texture
	SDL_Texture* const	Load(const char* path);
	SDL_Texture* const	LoadSurface(SDL_Surface* surface);
	bool				UnLoad(SDL_Texture* texture);

	void GetSize(const SDL_Texture* texture, unsigned int& width, unsigned int& height) const;

public:

	std::list<SDL_Texture*>	textures;
};

#endif // __TEXTURES_H__