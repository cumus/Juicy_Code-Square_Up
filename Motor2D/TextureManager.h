#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <vector>
#include <string>

struct SDL_Texture;
struct SDL_Surface;

struct TextureData
{
	TextureData();
	TextureData(const TextureData& copy);

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

	int LoadSurface(SDL_Surface* surface);

	bool GetTextureData(int id, TextureData& data) const;
	SDL_Texture* GetTexture(int id) const;

private:

	std::vector<TextureData> texture_data;
	std::vector<SDL_Texture*> textures;
};

#endif // __TEXTURE_MANAGER_H__