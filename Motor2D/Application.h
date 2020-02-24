#ifndef __APP_H__
#define __APP_H__

#include "EventListener.h"
#include "Module.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>

// Modules
class Input;
class Window;
class Audio;
class Map;
class Scene;
class Collisions;
class FadeToBlack;
class Render;

// Independent Managers
class TimeManager;
class Textures;

class Application : public EventListener
{
public:

	Application(int argc, char* args[]);
	virtual ~Application();

	bool Init();
	int Update();
	bool CleanUp();

	void RecieveEvent(const Event& e) override;

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
	Input*			input;
	Window*			win;
	Audio*			audio;
	Map*			map;
	Scene*			scene;
	Collisions*		collisions;
	FadeToBlack*	fade;
	Render*			render;

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
	bool				want_to_quit;

	std::string			load_game;
	mutable std::string	save_game;
};

extern Application* App;

#endif // __APP_H__