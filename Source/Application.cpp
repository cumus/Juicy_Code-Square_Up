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

#include "Optick/include/optick.h"

#ifdef DEBUG
#ifdef PLATFORMx86
#pragma comment( lib, "Optick/libx86/OptickCore_debug.lib" )
#elif PLATFORMx64
#pragma comment( lib, "Optick/libx64/OptickCore_debug.lib" )
#endif
#else
#ifdef PLATFORMx86
#pragma comment( lib, "Optick/libx86/OptickCore_release.lib" )
#elif PLATFORMx64
#pragma comment( lib, "Optick/libx64/OptickCore_release.lib" )
#endif
#endif

Application::Application(int argc, char* args[]) : argc(argc), args(args)
{
	want_to_save = want_to_load = want_to_quit = false;

	// Modules
	AddModule(input = new Input());
	AddModule(win = new Window());
	AddModule(audio = new Audio());
	AddModule(map = new Map());
	AddModule(scene = new Scene());
	AddModule(editor = new Editor());
	AddModule(render = new Render());
}

Application::~Application()
{
	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend(); ++it)
		DEL(*it);

	modules.clear();
}

void Application::AddModule(Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before the first frame
bool Application::Init()
{
	bool ret = files.Init();
	pugi::xml_node config;

	if (ret && files.LoadConfig(config))
	{
		if (!config.empty())
		{
			// App Config
			pugi::xml_node app_config = config.child("app");
			title = app_config.child("title").child_value();
			organization = app_config.child("organization").child_value();

			// Awake Modules
			for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
				ret = (*it)->Awake(config.child((*it)->name));

			// Initialize Independent Managers
			ret = fonts.Init();
		}
		else
		{
			LOG("Empty config file.");

			title = "unknown";
			organization = "unknown";

			for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
				ret = (*it)->Awake(config.append_child((*it)->name));
		}

		// Start Modules
		for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
			ret = (*it)->Start();
	}

	if (ret)
		ret = scene->LoadTestScene();

	return ret;
}

// Called each loop iteration
int Application::Update()
{
	OPTICK_FRAME("MainThread");

	if (want_to_quit)
		return 0; // closing app

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
	if(want_to_save)
		SavegameNow();

	if(want_to_load)
		LoadGameNow();

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
	default:
		break;
	}
}

// ---------------------------------------
int Application::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* Application::GetArgv(int index) const
{
	return index < argc ? args[index] : nullptr;
}

// ---------------------------------------
const char* Application::GetTitle() const
{
	return title;
}

// ---------------------------------------
const char* Application::GetOrganization() const
{
	return organization;
}

void Application::SetTitleAndOrg(const char* t, const char* org)
{
	title = t;
	organization = org;
}

// Load / Save
void Application::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
	//load_game.create("%s%s", fs->GetSaveDirectory(), file);
}

// ---------------------------------------
void Application::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	//save_game.create(file);
}

// ---------------------------------------
void Application::GetSaveGames(std::list<std::string>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool Application::LoadGameNow()
{
	bool ret = true;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.c_str());

	if(result)
	{
		LOG("Loading Game State from %s...", load_game);

		root = data.child("game_state");

		std::list<Module*>::iterator it;
		for (it = modules.begin(); it != modules.end() && ret; ++it)
			ret = (*it)->Load(root.child((*it)->name));

		want_to_load = false;
		data.reset();

		if(ret) LOG("...finished loading");
		else LOG("...loading process interrupted with error on module %s", (*it)->name);
	}
	else
	{
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game, result.description());
		ret = false;
	}

	return ret;
}

bool Application::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game);

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	std::list<Module*>::const_iterator it;
	for (it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Save(root.append_child((*it)->name));

	if (ret)
	{
		data.save_file(save_game.c_str());
		data.reset();
		want_to_save = false;

		LOG("... finished saving", save_game);
	}
	else
		LOG("Save process halted from an error in module %s", (*it)->name);

	return ret;
}