#include "Application.h"
#include "Input.h"
#include "Window.h"
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

		// Initialize Modules
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		{
			(*it)->SetActive(ret = (*it)->Init());
			if (ret)
				LOG("Initialized module: %s.", (*it)->GetName());
			else
				LOG("Error initializing module: %s.", (*it)->GetName());
		}

		// Post-Initialize Independent Manager Systems
		if (ret) ret = fonts.Init();

		// Start Modules
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		{
			if (ret = (*it)->Start())
				LOG("Started module: %s.", (*it)->GetName());
			else
				LOG("Error starting module: %s.", (*it)->GetName());
		}

		// Load sample scene
		if (ret)
		{
			state = STOPED;

			if (ret = scene->LoadTestScene())
				LOG("Loaded Scene");
			else
				LOG("Error loading Scene");
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

	OPTICK_CATEGORY("PreUpdate Application", Optick::Category::GameLogic);
	for (it = modules.begin(); it != modules.end() && no_error; ++it)
		no_error = (*it)->PreUpdate();

	OPTICK_CATEGORY("Update Application", Optick::Category::GameLogic);
	for (it = modules.begin(); it != modules.end() && no_error; ++it)
		no_error = (*it)->Update();

	OPTICK_CATEGORY("PostUpdate Application", Optick::Category::GameLogic);
	for (it = modules.begin(); it != modules.end() && no_error; ++it)
		no_error = (*it)->PostUpdate();

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
	/*(want_to_save)
		SavegameNow();

	if(want_to_load)
		LoadGameNow();*/

	if (state == TICKING)
	{
		time.PauseGameTimer();
		Event::Push(SCENE_PAUSE, scene);
		Event::PumpAll();
		state = PAUSED;
	}

	int extra_ms = time.ManageFrameTimers();

	// uncapped fps
	if (extra_ms < 0)
	{
		while (Event::RemainingEvents() > 0)
			Event::Pump();
	}
	else
	{
		// Pump events in extra_ms timespan
		Timer timer;
		while (extra_ms > timer.ReadI() && Event::RemainingEvents() > 0)
			Event::Pump();

		// Delay capped FPS
		if (extra_ms - timer.ReadI() > 0)
			time.Delay(extra_ms - timer.Read());
	}
}

// Called before quitting
bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	if (ret)
	{
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
		Event::PumpAll();
		state = PLAYING;
		break;
	case SCENE_PAUSE:
		time.PauseGameTimer();
		Event::Push(SCENE_PAUSE, scene, int(state));
		Event::PumpAll();
		state = PAUSED;
		break;
	case SCENE_TICK:
		time.StartGameTimer();
		Event::Push(SCENE_PLAY, scene, int(state));
		Event::PumpAll();
		state = TICKING;
		break;
	case SCENE_STOP:
		time.StopGameTimer();
		Event::Push(SCENE_STOP, scene, int(state));
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
