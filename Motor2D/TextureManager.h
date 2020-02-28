#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <vector>

struct SDL_Texture;

struct Sprite
{
	Sprite();
	Sprite(const Sprite& copy);

	int id;
	int width;
	int height;
	std::string source;
};

class TextureManager
{
public:

	TextureManager() {}
	~TextureManager() {}

	int Load(const char* path);
	void CleanUp();

	bool GetSprite(int id, Sprite& spite) const;
	SDL_Texture* GetTexture(int id) const;

private:

	std::vector<Sprite>	sprites;
	std::vector<SDL_Texture*> textures;
};

#endif // __TEXTURE_MANAGER_H__