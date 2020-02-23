#ifndef __APP_H__
#define __APP_H__

#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>
#include "Module.h"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class Map;
class FadeToBlack;
class Collisions;

class Application
{
public:

	Application(int argc, char* args[]);
	virtual ~Application();

	bool Awake();
	bool Start();
	bool Update();
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(std::list<std::string>& list_to_fill) const;

private:

	void PrepareUpdate();
	void FinishUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	Window*			win;
	Input*			input;
	Render*			render;
	Textures*		tex;
	Audio*			audio;
	Scene*			scene;
	Map*			map;
	FadeToBlack*	fade;
	Collisions*		collisions;

private:
	std::list<Module*>	modules;
	uint				frames;
	float				dt;
	int					argc;
	char**				args;

	const char*			title = nullptr;
	const char*			organization = nullptr;

	mutable bool		want_to_save;
	bool				want_to_load;
	std::string			load_game;
	mutable std::string	save_game;
};

extern Application* App;

#endif // __APP_H__