#include "FileManager.h"
#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"
#include "physfs-3.0.2/include/physfs.h"

#ifdef DEBUG
#ifdef PLATFORMx86
#pragma comment( lib, "physfs-3.0.2/x86/DebugData/physfs.lib" )
#elif PLATFORMx64
#pragma comment( lib, "physfs-3.0.2/x64/DebugData/physfs.lib" )
#endif
#else
#ifdef PLATFORMx86
#pragma comment( lib, "physfs-3.0.2/x86/ReleaseData/physfs.lib" )
#elif PLATFORMx64
#pragma comment( lib, "physfs-3.0.2/x64/ReleaseData/physfs.lib" )
#endif
#endif

pugi::xml_document FileManager::config;

FileManager::FileManager()
{}

FileManager::~FileManager()
{}

bool FileManager::Init(const char* argv0)
{
	char* path = SDL_GetBasePath();

	bool ret = (PHYSFS_init(path) != 0);

	if (ret)
	{
		base_path = path;
		ret = AddDirectory(path, NULL);
	}
	else
		LOG("Error Initializing PhysFS: %s", PHYSFS_getLastError());

	SDL_free(path);

	return ret;
}

bool FileManager::CleanUp()
{
	config.reset();
	return (PHYSFS_deinit() != 0);
}

bool FileManager::AddDirectory(const char* path, const char* mount_point)
{
	bool ret = (PHYSFS_mount(path, mount_point, 1) != 0);

	if (!ret)
		LOG("PhysFS error while adding a path (%s): %s\n", path, PHYSFS_getLastError());

	return ret;
}

const char* FileManager::GetBasePath()
{
	return base_path.c_str();
}

pugi::xml_node FileManager::ConfigNode()
{
	return config.first_child();
}

bool FileManager::SaveConfig() const
{
	bool ret = config.save_file((base_path + "config.xml").c_str(), "\t", 1u, pugi::encoding_utf8);

	if (!ret)
		LOG("Error saving new config.xml file.");

	return ret;
}

bool FileManager::LoadConfig()
{
	bool ret = LoadXML("config.xml", config);

	if(!ret)
		config.append_child("config");
	else
		LOG("Engine Configuration config.xml found and ready to load.");

	return ret;
}

bool FileManager::LoadXML(const char* file, pugi::xml_document& doc)
{
	bool ret = false;

	char* buffer;
	unsigned int size = Load(file, &buffer);

	if (size > 0u)
	{
		pugi::xml_parse_result result = doc.load_buffer(buffer, size);

		if (!(ret = result))
			LOG("Could not load %s - pugi error: %s", file, result.description());

		DEL_ARRAY(buffer);
	}

	return ret;
}

unsigned int FileManager::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != NULL)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[(unsigned int)size];
			PHYSFS_sint64 read = PHYSFS_read(fs_file, *buffer, 1, (PHYSFS_sint32)size);
			if (read != size)
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				DEL_ARRAY(buffer);
			}
			else
				ret = (unsigned int)read;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

SDL_RWops* FileManager::LoadRWops(const char* file) const
{
	char* buffer;
	int size = Load(file, &buffer);

	if (size > 0)
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size);
		if (r != nullptr)
			r->close = close_sdl_rwops;

		return r;
	}
	else
		return NULL;
}

int close_sdl_rwops(SDL_RWops* rw)
{
	DEL_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}