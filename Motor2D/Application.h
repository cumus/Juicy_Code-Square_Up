#ifndef __APP_H__
#define __APP_H__

#include <list>
#include <string>
#include "PugiXml\src\pugixml.hpp"
#include "Module.h"

// Modules
class Window;
class Input;
class Render;
class Audio;
class Scene;
class Map;
class FadeToBlack;
class Collisions;

// Independent Managers
class TimeManager;
class Textures;

class Application
{
public:

	Application(int argc, char* args[]);
	virtual ~Application();

	bool Init();
	int Update();
	bool CleanUp();

	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(std::list<std::string>& list_to_fill) const;

private:

	void AddModule(Module* module);

	void PrepareUpdate();
	void FinishUpdate();

	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	Window*			win;
	Input*			input;
	Render*			render;
	Audio*			audio;
	Scene*			scene;
	Map*			map;
	FadeToBlack*	fade;
	Collisions*		collisions;

	// Independent Managers
	TimeManager*	time;
	Textures*		tex;

private:

	std::list<Module*>	modules;

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