#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <vector>
#include <map>
#include <string>

struct SDL_Texture;
struct SDL_Surface;
struct SDL_Renderer;

struct TextureData
{
	TextureData();
	TextureData(const TextureData& copy);
	~TextureData();

	void ClearTexture();
	bool ReloadSurface(SDL_Surface* surface);

	static int texture_count;

	int id;
	int width;
	int height;
	std::string source;
	SDL_Texture* texture;
	bool reloaded;
};

class TextureManager
{
public:

	TextureManager();
	~TextureManager();

	void LoadConfig(bool empty_config);
	void SaveConfig() const;

	bool Init();
	void CleanUp();

	int Load(const char* path, bool reload = false, short r = 255, short g = 255, short b = 255, short a = 255);
	int LoadSurface(SDL_Surface* surface);
	TextureData* CreateEmpty();
	int CreateEmptyTexture(SDL_Renderer* renderer, int width, int height, const char* source = "undefined");

	bool Remove(int id);

	bool GetTextureData(int id, TextureData& data) const;
	SDL_Texture* GetTexture(int id) const;
	TextureData* GetDataPtr(int id);
	void SetTextureAlpha(int id, int alpha);

private:

	bool using_jpg = false;
	bool using_png = true;
	bool using_tif = false;
	bool using_webp = false;

	std::map<int, TextureData> textures;
};

#endif // __TEXTURE_MANAGER_H__