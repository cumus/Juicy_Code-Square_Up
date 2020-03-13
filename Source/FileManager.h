#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <string>

#include "PugiXml\src\pugixml.hpp"

struct SDL_RWops;

int close_sdl_rwops(SDL_RWops* rw);

class FileManager
{
public:

	FileManager();
	~FileManager();

	bool CleanUp();

	bool LoadConfig(pugi::xml_node& config);

	const char* GetBasePath();

	/*unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	unsigned int Save(const char* file, const char* buffer, unsigned int size) const;*/

private:

	pugi::xml_document config_file;

	const char* base_path = nullptr;
	const char* pref_dir = nullptr;
};

#endif // __FILE_MANAGER_H__