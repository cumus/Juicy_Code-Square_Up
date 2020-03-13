#include "FileManager.h"
#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

FileManager::FileManager()
{}

FileManager::~FileManager()
{}

bool FileManager::CleanUp()
{
	return true;
}

bool FileManager::LoadConfig(pugi::xml_node& config)
{
	std::string config_path = GetBasePath();
	config_path += "config.xml";

	pugi::xml_parse_result result = config_file.load_file(config_path.c_str());

	if (result)
		config = config_file.child("config");
	else
		LOG("Could not load %s - pugi error: %s", config_path.c_str(), result.description());

	return result;

	/*const char* org = "Juicy Code Games";
	const char* app = "Square Up";
	pref_dir = SDL_GetPrefPath(org, app);*/
}

const char* FileManager::GetBasePath()
{
	if (base_path == nullptr)
		base_path = SDL_GetBasePath();

	return base_path;
}
