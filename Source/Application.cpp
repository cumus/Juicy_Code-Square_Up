#include "Application.h"
#include "Input.h"
#include "Window.h"
#include "Minimap.h"
#include "Audio.h"
#include "Map.h"
#include "Scene.h"
#include "Editor.h"
#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"
#include "optick-1.3.0.0/include/optick.h"

#ifdef DEBUG
#ifdef PLATFORMx86
#pragma comment( lib, "optick-1.3.0.0/x86/DebugData/OptickCore.lib" )
#elif PLATFORMx64
#pragma comment( lib, "optick-1.3.0.0/x64/DebugData/OptickCore.lib" )
#endif
#else
#ifdef PLATFORMx86
#pragma comment( lib, "optick-1.3.0.0/x86/ReleaseData/OptickCore.lib" )
#elif PLATFORMx64
#pragma comment( lib, "optick-1.3.0.0/x64/ReleaseData/OptickCore.lib" )
#endif
#endif

Application::Application(int argc, char* args[]) : argc(argc), args(args)
{
	want_to_save = want_to_load = want_to_quit = false;

	// Modules
	modules.push_back(input = new Input());
	modules.push_back(win = new Window());
	modules.push_back(audio = new Audio());
	modules.push_back(scene = new Scene());
	modules.push_back(editor = new Editor());
	modules.push_back(render = new Render());
}

Application::~Application()
{
	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend(); ++it)
		DEL(*it);

	modules.clear();
}

// Called before the first frame
bool Application::Init()
{
	bool ret = false;
	if (files.Init(args[0]))
	{
		ret = true;

		// Load Engine Configuration
		bool config_loaded = files.LoadConfig();
		LoadAllConfig(!config_loaded);

		if (!config_loaded) files.SaveConfig();

		// Pre-Initialize Independent Manager Systems
		if (ret) ret = time.Init();
		if (ret) ret = tex.Init();
		if (ret) ret = fonts.Init();
		
		// Initialize Modules
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		{
			(*it)->SetActive(ret = (*it)->Init());
			if (ret)
				LOG("Initialized module: %s.", (*it)->GetName());
			else
				LOG("Error initializing module: %s.", (*it)->GetName());
		}

		if (ret) ret = pathfinding.Init();

		// Start Modules
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		{
			if (ret = (*it)->Start())
				LOG("Started module: %s.", (*it)->GetName());
			else
				LOG("Error starting module: %s.", (*it)->GetName());
		}

		// Load Intro scene
		if (ret)
		{
			state = STOPED;
			time.SetMaxFPS(60);

			Event::Push(SCENE_CHANGE, scene, INTRO, 0.f);
			Event::PumpAll();
			Event::Push(SCENE_PLAY, this);
		}
	}
	else
		LOG("Error initializing file system.");

	return ret;
}

// Called each loop iteration
int Application::Update()
{
	OPTICK_FRAME("MainThread");

	if (want_to_quit)
	{
		SaveConfig();
		return 0; // closing app
	}

	PrepareUpdate();

	static std::list<Module*>::iterator it;
	static bool no_error = true;

	fogWar.Update();//Pre update

	OPTICK_CATEGORY("PreUpdate Application", Optick::Category::GameLogic);
	for (it = modules.begin(); it != modules.end() && no_error; ++it)
		if (!(no_error = (*it)->PreUpdate()))
			LOG("Module %s encuntered an error during PreUpdate!", (*it)->GetName());

	OPTICK_CATEGORY("Update Application", Optick::Category::GameLogic);
	for (it = modules.begin(); it != modules.end() && no_error; ++it)
		if (!(no_error = (*it)->Update()))
			LOG("Module %s encuntered an error during Update!", (*it)->GetName());

	particleSys.Update();
	collSystem.Update();

	OPTICK_CATEGORY("PostUpdate Application", Optick::Category::GameLogic);
	for (it = modules.begin(); it != modules.end() && no_error; ++it)
		if (!(no_error = (*it)->PostUpdate()))
			LOG("Module %s encuntered an error during PostUpdate!", (*it)->GetName());

	if (!no_error)
		return -1; // error

	FinishUpdate();

	return 1; // continue
}

void Application::PrepareUpdate()
{
	time.UpdateDeltaTime();
}

void Application::FinishUpdate()
{
	if (want_to_save)
	{
		scene->SaveGameNow();
		want_to_save = false;
	}

	if(want_to_load)
	{
		scene->LoadGameNow();
		want_to_load = false;
	}

	if (state == TICKING)
	{
		time.PauseGameTimer();
		Event::Push(SCENE_PAUSE, scene);
		Event::PumpAll();
		state = PAUSED;
	}

	int extra_ms = time.ManageFrameTimers();

	if (extra_ms < 0) // uncapped fps
	{
		pathfinding.IteratePaths(1);

		while (Event::RemainingEvents() > 0)
			Event::Pump();
	}
	else
	{
		Timer timer;

		// Update pathfindnig
		pathfinding.IteratePaths(extra_ms);

		// Pump events in extra_ms timespan
		while (extra_ms > timer.ReadI() && Event::RemainingEvents() > 0)
			Event::Pump();

		// Delay capped FPS
		if (extra_ms - timer.ReadI() > 0)
			time.Delay(extra_ms - timer.Read());
	}
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	if (ret)
	{
		fogWar.CleanUp();
		collSystem.Clear();
		particleSys.CleanUp();
		tex.CleanUp();

		ret = (fonts.CleanUp() && files.CleanUp());
	}

	return ret;
}

void Application::RecieveEvent(const Event & e)
{
	switch (e.type)
	{
	case REQUEST_LOAD:
		LOG("App load event");
		want_to_load = true;
		break;
	case REQUEST_SAVE:
		LOG("App save event");
		want_to_save = true;
		break;
	case REQUEST_QUIT:
		LOG("App quit event requested");
		want_to_quit = true;
		break;
	case WINDOW_QUIT:
		LOG("App window quit event requested");
		want_to_quit = true;
		break;
	case SCENE_PLAY:
		time.StartGameTimer();
		Event::Push(SCENE_PLAY, scene, int(state));
		Event::Push(SCENE_PLAY, audio, int(state));
		Event::PumpAll();
		state = PLAYING;
		break;
	case SCENE_PAUSE:
		time.PauseGameTimer();
		Event::Push(SCENE_PAUSE, scene, int(state));
		Event::Push(SCENE_PAUSE, audio, int(state));
		Event::PumpAll();
		state = PAUSED;
		break;
	case SCENE_TICK:
		time.StartGameTimer();
		Event::Push(SCENE_PLAY, scene, int(state));
		Event::Push(SCENE_PLAY, audio, int(state));
		Event::PumpAll();
		state = TICKING;
		break;
	case SCENE_STOP:
		time.StopGameTimer();
		Event::Push(SCENE_STOP, scene, int(state));
		Event::Push(SCENE_STOP, audio, int(state));
		Event::PumpAll();
		state = STOPED;
		break;
	default:
		break;
	}
}

GameState Application::GetState() const
{
	return state;
}

int Application::GetArgc() const
{
	return argc;
}

const char* Application::GetArgv(int index) const
{
	return (index > 0 && index < argc) ? args[index] : nullptr;
}

const char* Application::GetTitle() const
{
	return title;
}

const char* Application::GetOrganization() const
{
	return organization;
}

void Application::SetTitleAndOrg(const char* t, const char* org)
{
	title = t;
	organization = org;
}

void Application::LoadAllConfig(bool empty_config)
{
	pugi::xml_node config = files.ConfigNode();

	title = "Square Up";
	organization = "UPC";

	if (empty_config)
	{
		pugi::xml_node app_config = config.append_child("app");
		app_config.append_attribute("title").set_value(title);
		app_config.append_attribute("organization").set_value(organization);
	}
	else
	{
		pugi::xml_node app_config = config.child("app");
		title = app_config.attribute("title").as_string(title);
		organization = app_config.attribute("organization").as_string(organization);
	}

	// Call Managers
	tex.LoadConfig(empty_config);

	// Call Modules
	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->LoadConfig(empty_config);
}

void Application::SaveConfig() const
{
	// Save App config
	pugi::xml_node config = files.ConfigNode();
	pugi::xml_node app_config = config.child("app");
	app_config.attribute("title").set_value(title);
	app_config.attribute("organization").set_value(organization);

	// Call Managers
	tex.SaveConfig();

	// Call Modules
	for (std::list<Module*>::const_iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->SaveConfig();

	// Save changes
	files.SaveConfig();
}

void Application::StressTest()
{
	// HUD
	tex.Load("textures/intro-sprite.png");
	tex.Load("textures/game-logo.png");
	tex.Load("textures/hud-sprites.png");
	tex.Load("textures/icons_price.png");
	tex.Load("textures/Iconos_square_up.png");
	tex.Load("textures/selectionMark.png");
	tex.Load("textures/icons.png");
	tex.Load("textures/Mouse.png");
	tex.Load("textures/background.png");
	tex.Load("textures/game-logo.png");
	tex.Load("textures/new-game.png");
	tex.Load("textures/resume.png");
	tex.Load("textures/options.png");
	tex.Load("textures/quit.png");
	tex.Load("textures/BaseAnim.png");
	tex.Load("textures/background2.png");
	tex.Load("textures/options_title.png");
	tex.Load("textures/fullscreen.png");
	tex.Load("textures/button3.png");
	tex.Load("textures/music-volume.png");
	tex.Load("textures/sfx-volume.png");
	tex.Load("textures/main-menu.png");
	tex.Load("textures/back-win.png");
	tex.Load("textures/youwin.png");
	tex.Load("textures/wcontinue.png");
	tex.Load("textures/back-lose.png");
	tex.Load("textures/youlose.png");
	tex.Load("textures/lcontinue.png");
	tex.Load("textures/pause-bg.png");
	tex.Load("textures/save.png");
	tex.Load("textures/load.png");
	tex.Load("textures/victory.png");
	tex.Load("textures/defeat.png");

	// Pathfinding
	App->tex.Load("textures/meta.png");

	// Dialogue & Tutorial
	tex.Load("textures/queen.png");
	tex.Load("textures/soldier.png");
	tex.Load("textures/tutomages.png");
	tex.Load("textures/tuto/skip-button.png");
	tex.Load("textures/tuto/cam-not.png");
	tex.Load("textures/tuto/not-button.png");
	tex.Load("textures/tuto/lure-queen-not.png");

	// FoW
	tex.Load("textures/fogTiles60.png");
	tex.Load("textures/fogTiles.png");

	// Map
	tex.Load("maps/isometric_grass_and_water.png");
	tex.Load("maps/Tileset_Map.png");

	// Minimap
	tex.Load("textures/minimap.png");

	// Particles
	tex.Load("textures/particle_shot.png");
	tex.Load("textures/energyBall.png");

	// Buildmode
	tex.Load("textures/buildPreview.png");

	// Units
	tex.Load("textures/BaseCenter.png");
	tex.Load("textures/Tower.png");
	tex.Load("textures/Edge.png");
	tex.Load("textures/Capsule.png");
	tex.Load("textures/lab.png");
	tex.Load("textures/Barracks.png");
	tex.Load("textures/SpawnEnemy.png");
	tex.Load("textures/Unit_Melee.png");
	tex.Load("textures/Enemy_Melee.png");
	tex.Load("textures/Enemy_Ranged.png");
	tex.Load("textures/Enemy_Super_Temp.png");
	tex.Load("textures/Unit_Gatherer.png");
	tex.Load("textures/Unit_Ranged.png");
	tex.Load("textures/Unit_Super.png");

	// Audios
	for (int i = 0; i <= 20; ++i)
		audio->LoadFx(Audio_FX(i));
}
