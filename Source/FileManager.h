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

	bool Init(const char* argv0);
	bool CleanUp();

	bool AddDirectory(const char* path, const char* mount_point = nullptr);
	const char* GetBasePath();

	static pugi::xml_node& ConfigNode();

	bool SaveConfig() const;
	bool LoadConfig();
	bool LoadXML(const char* file, pugi::xml_document& doc);

	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* LoadRWops(const char* file) const;

public:

	static pugi::xml_document config;

private:

	std::string base_path;
};

#endif // __FILE_MANAGER_H__