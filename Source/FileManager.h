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

	bool Init();
	bool CleanUp();

	bool AddDirectory(const char* path, const char* mount_point = nullptr);

	bool LoadConfig(pugi::xml_node& node);
	bool LoadXML(const char* file, pugi::xml_document& doc);

	const char* GetBasePath();

	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

private:

	pugi::xml_document	config;
	std::string base_path;
	std::string pref_dir;
};

#endif // __FILE_MANAGER_H__