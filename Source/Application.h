#ifndef __APP_H__
#define __APP_H__

#include "EventListener.h"
#include "Module.h"

#include "FileManager.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "FontManager.h"
#include "PathfindingManager.h"
#include "FogOfWarManager.h"
#include "CollisionSystem.h"
#include "DialogSystem.h"
#include "ParticleSystem.h"

#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>

// Modules
class Input;
class Window;
class Audio;
class Scene;
class Editor;
class Render;


// Independent Managers
class FileManager;
class TimeManager;
class TextureManager;
class FontManager;
class PathfindingManager;
class FogOfWarManager;
class CollisionSystem;
class DialogSystem;
class ParticleSystem;

enum GameState : int
{
	STOPED,
	PLAYING,
	PAUSED,
	TICKING
};

class Application : public EventListener
{
public:

	Application(int argc, char* args[]);
	virtual ~Application();

	bool Init();
	int Update();
	bool CleanUp();

	void RecieveEvent(const Event& e) override;

	GameState GetState() const;
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void SetTitleAndOrg(const char* title, const char* org);

	void StressTest();

private:

	void PrepareUpdate();
	void FinishUpdate();

	void LoadAllConfig(bool empty_config);
	void SaveConfig() const;

public:

	// Modules
	Input*		input;
	Window*		win;
	Audio*		audio;
	Scene*		scene;
	Editor*		editor;
	Render*		render;

	// Independent Managers
	FileManager		files;
	TimeManager		time;
	TextureManager	tex;
	FontManager		fonts;
	PathfindingManager pathfinding;
	FogOfWarManager fogWar;
	CollisionSystem collSystem;
	DialogSystem   dialogSys;
	ParticleSystem particleSys;

private:

	// Module container
	std::list<Module*> modules;

	// Execution arguments
	int		argc = -1;
	char**	args = nullptr;

	// Game State
	GameState state;

	// Config values
	const char*	title = nullptr;
	const char*	organization = nullptr;

	// Event controls
	bool want_to_save = false;
	bool want_to_load = false;
	bool want_to_quit = false;
};

extern Application* App;

#endif // __APP_H__