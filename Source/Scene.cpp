#include "Scene.h"
#include "Application.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "Render.h"
#include "Editor.h"

#include "Transform.h"
#include "Sprite.h"
#include "Behaviour.h"
#include "Edge.h"
#include "BaseCenter.h"
#include "Tower.h"
#include "AudioSource.h"
#include "Canvas.h"
#include "Minimap.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL_scancode.h"
#include "optick-1.3.0.0/include/optick.h"

#include <queue>
#include <vector>

Scene::Scene() : Module("scene")
{
	root.SetName("______root______");
}

Scene::~Scene()
{}

bool Scene::PreUpdate()
{
	root.PreUpdate();

	return true;
}

bool Scene::Update()
{
	bool ret = true;

	OPTICK_EVENT();

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, TEST);
		else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, INTRO);
		else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, MENU);
		else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, MAIN);
	}


	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		map.draw_walkability = !map.draw_walkability;

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		App->audio->PauseMusic(1.f);

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");

	// Swap map orientation
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) Map::SwapMapType();

	// Update gameobject hierarchy
	
	root.Update();

	int x, y;
	App->input->GetMousePosition(x, y);
	SDL_Rect cam = App->render->GetCameraRect();

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		Gameobject* audio_go = AddGameobject("AudioSource - son of root");
		audio_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		(new AudioSource(audio_go))->Play(HAMMER, -1);
		new Sprite(audio_go, id_mouse_tex, { 128, 0, 64, 64 });
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		Gameobject* unit_go = AddGameobject("Game Unit - son of root");
		unit_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		new B_Unit(unit_go, UNIT_MELEE, IDLE);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) //Edge
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
		if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
		{
			Gameobject* edge_go = AddGameobject("Edge resource node");
			edge_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

			new Edge(edge_go);
		}
		else
			LOG("Invalid spawn position");
	}
	if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) //Base_Center
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
		if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
		{
			Gameobject* base_go = AddGameobject("Base Center");
			base_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

			App->audio->PlayFx(B_BUILDED);
			new Base_Center(base_go);
		}
		else
			LOG("Invalid spawn position");
	}

	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN) //Defensive tower
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
		if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
		{
			Gameobject* tower_go = AddGameobject("Tower");
			tower_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

			App->audio->PlayFx(B_BUILDED);
			new Tower(tower_go);
		}
		else
			LOG("Invalid spawn position");
	}

	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{
		std::pair<int, int> mouseOnMap = Map::WorldToTileBase(x + cam.x, y + cam.y);
		destinationPath = iPoint(mouseOnMap.first, mouseOnMap.second);
		path = App->pathfinding.CreatePath(startPath, destinationPath,0);
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		std::pair<int, int> mouseOnMap = Map::WorldToTileBase(x + cam.x, y + cam.y);
		destinationPath = iPoint(mouseOnMap.first, mouseOnMap.second);
		path = App->pathfinding.CreatePath(startPath, destinationPath,1);
	}

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		std::pair<int, int> mouseOnMap = Map::WorldToTileBase(x + cam.x, y + cam.y);
		destinationPath = iPoint(mouseOnMap.first, mouseOnMap.second);
		path = App->pathfinding.CreatePath(startPath, destinationPath,2);
	}

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		App->pathfinding.DebugShowPaths();
	}

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		if (App->editor->selection)
		{
			App->editor->selection->Destroy();
		}
	}

	if (App->input->GetMouseButtonDown(2) == KEY_DOWN)
	{
		Gameobject* go = App->editor->selection;
		if (go) 
		{
			std::pair<float, float> mouseOnMap = Map::F_WorldToMap(float(x + cam.x), float(y + cam.y));
			Event::Push(ON_RIGHT_CLICK, go, mouseOnMap.first, mouseOnMap.second);
		}
	}

	return ret;
}

bool Scene::PostUpdate()
{
	root.PostUpdate();

	map.Draw();

	SDL_Rect cam_rect = App->render->GetCameraRect();
	SDL_Rect rect = { 0, 0, 64, 64 };

	std::pair<int, int> render_pos = Map::I_MapToWorld(startPath.x, startPath.y);
	App->render->Blit(id_mouse_tex, render_pos.first, render_pos.second, &rect, DEBUG_MAP);

	if (path != nullptr && !path->empty())
	{
		for (std::vector<iPoint>::const_iterator it = path->cbegin(); it != path->cend(); ++it)
		{
			std::pair<int, int> render_pos = Map::I_MapToWorld(it->x, it->y);
			App->render->Blit(id_mouse_tex, render_pos.first, render_pos.second, &rect, DEBUG_MAP);
		}
	}

	// Debug Pointer Info on Window Title
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	std::pair<int, int> map_coordinates = Map::WorldToTileBase(cam_rect.x + mouse_x, cam_rect.y + mouse_y);

	// Editor Selection
	Gameobject* sel = App->editor->selection;

	// Log onto window title
	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Zoom: %0.2f, Mouse: %dx%d, Tile: %dx%d, Selection: %s",
		App->time.GetLastFPS(),
		App->render->GetZoom(),
		mouse_x, mouse_y,
		map_coordinates.first, map_coordinates.second,
		sel != nullptr ? sel->GetName() : "none selected");

	App->win->SetTitle(tmp_str);

	return true;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	
	return true;
}

void Scene::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case SCENE_PLAY:
		Event::Push(ON_PLAY, &root, e.data1);
		break;
	case SCENE_PAUSE:
		Event::Push(ON_PAUSE, &root, e.data1);
		break;
	case SCENE_STOP:
		Event::Push(ON_STOP, &root, e.data1);
		break;
	case SCENE_CHANGE:
		map.CleanUp();
		App->audio->UnloadFx();
		App->audio->PauseMusic(1.f);
		App->editor->SetSelection(nullptr, false);
		root.RemoveChilds();
		Event::PumpAll();
		ChangeToScene(Scenes(e.data1.AsInt()));
		break;
	default:
		break;
	}
}

bool Scene::LoadTestScene()
{
	OPTICK_EVENT();
	// Play sample track
	bool ret = App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");

	// Load mouse debug texture for identifying tiles
	if (ret)
	{
		id_mouse_tex = App->tex.Load("textures/meta.png");
		ret = (id_mouse_tex != -1);
	}

	if (ret && map.Load("maps/iso.tmx"))
	{
		// HUD
		Gameobject* canvas_go = AddGameobject("Canvas", &root);
		C_Canvas* canv = new C_Canvas(canvas_go);
		canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };

		Gameobject* img_go = AddGameobject("Image", canvas_go);
		C_Image* img = new C_Image(img_go);
		img->target = { 1.f, 1.f, 0.5f, 0.5f };
		img->offset = { -1199.f, -674.f };
		img->section = { 0, 0, 1199, 674 };
		img->tex_id = App->tex.Load("textures/goku.png");

		Gameobject* text1_go = AddGameobject("Text 1", canvas_go);
		C_Text* text1 = new C_Text(text1_go, "Componente texto sin ajustar");
		text1->target = { 0.4f, 0.2f, 1.f, 1.f };

		Gameobject* text2_go = AddGameobject("Text 2", canvas_go);
		C_Text* text2 = new C_Text(text2_go, "Componente texto ajustado");
		text2->target = { 0.4f, 0.4f, 1.f, 1.f };
		text2->scale_to_fit = true;

		Gameobject* button_go = AddGameobject("Quit Button", canvas_go);
		C_Button* button = new C_Button(button_go, Event(REQUEST_QUIT, App));
		button->target = { 1.f, 0.4f, 1.f, 1.f };
		button->offset = { -101.f, 0.f };
		button->section = { 359, 114, 101, 101 };
		button->tex_id = App->tex.Load("textures/icons.png");

		Gameobject* minimap_go = AddGameobject("Minimap", canvas_go);
		Minimap* minimap = new Minimap(minimap_go);
		minimap->target = { 0.f, 0.6f, 0.4f, 0.4f };
		minimap->CreateMinimap();
		minimap->SetActive();
	}

	return ret;
}

bool Scene::LoadMainScene()
{
	OPTICK_EVENT();
	//------------------------- HUD CANVAS --------------------------------------

	hud_canvas_go = AddGameobject("HUD Canvas", &root);
	C_Canvas* hud_canv = new C_Canvas(hud_canvas_go);
	hud_canv->target = { 0.3f, 0.3f, 0.4f, 0.4f };

	// HUD Creation Bars Test // These should be created when a unit, building or the main base are created 
	                          // instead of when the main scene loads


	// Main Base Creation Bar

	const char* base_name = "Main Base";

	float base_health_percentatge = 50.0f;

	create_base_bar(base_name, &base_health_percentatge);

	// Building Creation Bar

	const char* building_name = "Tower";

    const char* building_type = "defensive";

	float bulding_health_percentatge = 25.0f;

	int upgrades = 2;

	create_building_bar(building_name, building_type, &bulding_health_percentatge, &upgrades);

	create_building_bar(building_name, building_type, &bulding_health_percentatge, &upgrades);

	create_building_bar(building_name, building_type, &bulding_health_percentatge, &upgrades);

	const char* unit_name = "Unit";

	const char* unit_type = "melee";

	float unit_health_percentatge = 100.0f;

	const char* action = "defense";

	create_unit_bar(unit_name, unit_type, &unit_health_percentatge, action);

	create_unit_bar(unit_name, unit_type, &unit_health_percentatge, action);

	create_unit_bar(unit_name, unit_type, &unit_health_percentatge, action);

	return map.Load("maps/iso.tmx");
}

bool Scene::LoadIntroScene()
{
	OPTICK_EVENT();
	// Play sample track
	bool ret = App->audio->PlayFx(LOGO);

	// Add a canvas
	Gameobject* canvas_go = AddGameobject("Canvas", &root);
	C_Canvas* canv = new C_Canvas(canvas_go);
	canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };

	Gameobject* background_go = AddGameobject("Background", canvas_go);

	C_Image* background = new C_Image(background_go);
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1920.f, -1080.f };
	background->section = { 0, 0, 1920, 1080 };
	background->tex_id = App->tex.Load("textures/white.png");
	
	Gameobject* logo_go = AddGameobject("Team logo", canvas_go);

	C_Image* logo = new C_Image(logo_go);
	logo->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	logo->offset = { -300.f, -400.f };
	logo->section = { 0, 0, 499, 590 };
	logo->tex_id = App->tex.Load("textures/team-logo2.png");

	return ret;
	
}

bool Scene::LoadMenuScene()
{
	OPTICK_EVENT();
	// Play sample track
	bool ret = App->audio->PlayMusic("audio/Music/alexander-nakarada-early-probe-eats-the-dust.ogg");

	//------------------------- CANVAS --------------------------------------
	Gameobject* canvas_go = AddGameobject("Canvas", &root);
	C_Canvas* canv = new C_Canvas(canvas_go);
	canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };

	//------------------------- BACKGROUND --------------------------------------

	Gameobject* background_go = AddGameobject("Background", canvas_go);

	C_Image* background = new C_Image(background_go);
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1920.f, -1080.f };
	background->section = { 0, 0, 1920, 1080 };
	background->tex_id = App->tex.Load("textures/white.png");

	//------------------------- LOGO --------------------------------------

	Gameobject* g_logo_go = AddGameobject("Game logo", canvas_go);

	C_Image* g_logo = new C_Image(g_logo_go);
	g_logo->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	g_logo->offset = { -525.f, -500.f };
	g_logo->section = { 0, 0, 1070, 207 };
	g_logo->tex_id = App->tex.Load("textures/game-logo.png");

	Gameobject* start_go = AddGameobject("Start Button", canvas_go);
	
	//------------------------- START --------------------------------------
	
	C_Button* start = new C_Button(start_go, Event(SCENE_CHANGE, this, MAIN));
	start->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	start->offset = { -525.f, -100.f };
	start->section = { 0, 0, 1070, 207 };
	start->tex_id = App->tex.Load("textures/button.png");

	Gameobject* start_txt_go = AddGameobject("Start Button", start_go);
	
	C_Text* start_txt = new C_Text(start_txt_go, "PLAY");
	start_txt->target = { 0.48f, 0.45f, 1.f, 1.f };
	start_txt->scale_to_fit = true;

	//------------------------- QUIT --------------------------------------

	Gameobject* quit_go = AddGameobject("Quit Button", canvas_go);

	C_Button* quit = new C_Button(quit_go, Event(REQUEST_QUIT, App));
	quit->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	quit->offset = { -525.f, 200.f };
	quit->section = { 0, 0, 1070, 207 };
	quit->tex_id = App->tex.Load("textures/button.png");

	Gameobject* quit_txt_go = AddGameobject("Start Button", quit_go);

	C_Text* quit_txt = new C_Text(quit_txt_go, "QUIT");
	quit_txt->target = { 0.48f, 0.45f, 1.f, 1.f };
	quit_txt->scale_to_fit = true;

	return ret;
}

bool Scene::ChangeToScene(Scenes scene)
{
	bool ret = false;

	switch (scene)
	{
	case TEST:
		ret = LoadTestScene();
		break;
	case INTRO:
		ret = LoadIntroScene();
		break;
	case MENU:
		ret = LoadMenuScene();
		break;
	case MAIN:
		ret = LoadMainScene();
		break;
	case MAIN_FROM_SAFE:
		break;
	case END:
		break;
	case CREDITS:
		break;
	default:
		break;
	}

	return ret;
}

void Scene::create_base_bar(const char* name, float* health) {
	
	float pos_y = 0.17;
	
	//------------------------- BASE BAR --------------------------------------

	Gameobject* main_base_bar_go = AddGameobject("Main Base Bar", hud_canvas_go);
	C_Button* main_base_bar = new C_Button(main_base_bar_go, Event(REQUEST_QUIT, App));
	main_base_bar->target = { 0.5f, pos_y, 1.3f, 1.2f };
	main_base_bar->offset = { -482.0f, -44.0f };
	main_base_bar->section = { 4, 7, 482, 44 };
	main_base_bar->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE PORTRAIT --------------------------------------

	Gameobject* main_base_portrait_go = AddGameobject("Main Base Portrait", hud_canvas_go);
	C_Image* main_base_portrait = new C_Image(main_base_portrait_go);
	main_base_portrait->target = { 0.06f, pos_y - 0.003f, 0.5f, 0.5f };
	main_base_portrait->offset = { -109.0f, -89.0f };
	main_base_portrait->section = { 126, 295, 109, 95 };
	main_base_portrait->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE TEXT --------------------------------------

	Gameobject* main_base_text_go = AddGameobject("Main Base Text", hud_canvas_go);
	C_Text* main_base_text = new C_Text(main_base_text_go, name);
	main_base_text->target = { 0.07f, pos_y - 0.07f, 1.5f, 1.5f };

	//------------------------- BASE HEALTHBAR --------------------------------------

	Gameobject* main_base_healthbar_go = AddGameobject("Main Base healthbar", hud_canvas_go);
	C_Image* main_base_healthbar = new C_Image(main_base_healthbar_go);
	main_base_healthbar->target = { 0.415f, pos_y - 0.01f, 1.8f, 0.7f };
	main_base_healthbar->offset = { -245.0f, -23.0f };
	main_base_healthbar->section = { 56, 192, 245, 23 };
	main_base_healthbar->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE HEALTH --------------------------------------

	Gameobject* main_base_health_go = AddGameobject("Main Base health", hud_canvas_go);
	C_Image* main_base_health = new C_Image(main_base_health_go);
	main_base_health->target = { (0.415f) - ((0.415f - 0.07f) * (1.0f - *health / 100.0f)), pos_y - 0.01f, 1.8f * (*health/100), 0.7f }; 
	main_base_health->offset = { -245.0f, -23.0f };
	main_base_health->section = { 57, 238, 245, 23 };
	main_base_health->tex_id = App->tex.Load("textures/creation_bars_test.png");

}

void Scene::create_building_bar(const char* name, const char* type, float* health, int *upgrades) {
	
	building_bars_created++;

	float pos_y = 0.17 + 0.07 * building_bars_created;

	//------------------------- BUILDING BAR --------------------------------------

	Gameobject* building_bar_go = AddGameobject("Building Bar", hud_canvas_go);
	C_Button* building_bar = new C_Button(building_bar_go, Event(REQUEST_QUIT, App));
	building_bar->target = { 0.405f, pos_y, 1.3f, 1.2f };
	building_bar->offset = { -388.0f, -35.0f };
	building_bar->section = { 4, 76, 388, 35 };
	building_bar->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BUILDING PORTRAIT --------------------------------------

    // Gameobject* building_portrait_go = AddGameobject("Building Portrait", hud_canvas_go);
	C_Image* building_portrait = new C_Image(building_bar_go);
    building_portrait->target = { 0.06f, pos_y - 0.003f, 0.5f, 0.37f };
    building_portrait->offset = { -109.0f, -93.0f };
	if (type == "defensive")
	    building_portrait->section = { 4, 291, 109, 93 };
	/* 
	else if (type == "ofensive") 
		 building_portrait->section = { __, __, 109, 93 };
	}
	   
	else if (type == "spawner")
	{
		building_portrait->section = { __, __, 109, 93 };
	}
	*/

	building_portrait->tex_id = App->tex.Load("textures/creation_bars_test.png");
	

	//------------------------- BUILDING TEXT --------------------------------------

	Gameobject* building_text_go = AddGameobject("Builidng Text", hud_canvas_go);
	C_Text* building_text = new C_Text(building_text_go, name);
	building_text->target = { 0.07f, pos_y - 0.055f, 1.5f, 1.2f };

	//------------------------- BUILDING HEALTHBAR --------------------------------------

	Gameobject* builiding_healthbar_go = AddGameobject("Builidng Healthbar", hud_canvas_go);
	C_Image* building_helathbar = new C_Image(builiding_healthbar_go);
	building_helathbar->target = { 0.338f, pos_y - 0.01f, 1.4f, 0.5f };
	building_helathbar->offset = { -245.0f, -23.0f };
	building_helathbar->section = { 56, 192, 245, 23 };
	building_helathbar->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BUILDING HEALTH --------------------------------------

	Gameobject* building_health_go = AddGameobject("Builidng Health", hud_canvas_go);
	C_Image* building_health = new C_Image(building_health_go);
	building_health->target = { (0.338f) - ((0.338f - 0.07f) * (1.0f - *health / 100.0f)), pos_y - 0.01f, 1.4f * (*health / 100), 0.5f };
	building_health->offset = { -245.0f, -23.0f };
	building_health->section = { 57, 238, 245, 23 };
	building_health->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BUILDING UPGRADES --------------------------------------

	for (int i = 0; i <= *upgrades; i++) {
		if (i >= 1) {
			Gameobject* building_upgrades_go = AddGameobject("Building Upgrade", hud_canvas_go);
			C_Image* building_upgrades = new C_Image(building_upgrades_go);
			building_upgrades->target = { 0.338f + i * 0.02f, pos_y - 0.01f, 0.3f, 0.3f };
			building_upgrades->offset = { -79.0f, -93.0f };
			building_upgrades->section = { 398, 78, 79, 93 };
			building_upgrades->tex_id = App->tex.Load("textures/creation_bars_test.png");
		}

		else
			;
	}
	
}

void Scene::create_unit_bar(const char* name, const char* type, float* health, const char* action) {

	unit_bars_created++;

	float pos_y = 0.5 + 0.07 * unit_bars_created;

	//------------------------- UNIT BAR --------------------------------------

	Gameobject* unit_bar_go = AddGameobject("Unit Bar", hud_canvas_go);
	C_Button* unit_bar = new C_Button(unit_bar_go, Event(REQUEST_QUIT, App));
	unit_bar->target = { 0.365f, pos_y, 1.3f, 1.2f };
	unit_bar->offset = { -349.0f, -32.0f };
	unit_bar->section = { 4, 135, 349, 32 };
	unit_bar->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- UNIT PORTRAIT --------------------------------------

	Gameobject* unit_portrait_go = AddGameobject("Unit Portrait", hud_canvas_go);
	C_Image* unit_portrait = new C_Image(unit_portrait_go);
	unit_portrait->target = { 0.06f, pos_y - 0.001f, 0.45f, 0.38f };
	unit_portrait->offset = { -109.0f, -93.0f };
	if (type == "melee")
	{
		unit_portrait->section = { 349, 185, 109, 93 };
	}
	
	/*
	else if(type == "ranged")
	{
		building_portrait->section = { __, __, 109, 93 };
	}
	*/

	/*
	else if(type == "gatherer")
	{
		building_portrait->section = { __, __, 109, 93 };
	}
	*/

	/*
	else if(type == "tank")
	{
		building_portrait->section = { __, __, 109, 93 };
	}
	*/

	unit_portrait->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- UNIT TEXT --------------------------------------

	Gameobject* building_text_go = AddGameobject("Unit Text", hud_canvas_go);
	C_Text* building_text = new C_Text(building_text_go, name);
	building_text->target = { 0.07f, pos_y - 0.05f, 1.2f, 1.2f };

	//------------------------- UNIT HEALTHBAR --------------------------------------

	Gameobject* unit_healthbar_go = AddGameobject("Unit Healthbar", hud_canvas_go);
	C_Image* unit_helathbar = new C_Image(unit_healthbar_go);
	unit_helathbar->target = { 0.31f, pos_y - 0.007f, 1.255f, 0.4f };
	unit_helathbar->offset = { -245.0f, -23.0f };
	unit_helathbar->section = { 56, 192, 245, 23 };
	unit_helathbar->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- UNIT HEALTH --------------------------------------

	Gameobject* building_health_go = AddGameobject("Unit Health", hud_canvas_go);
	C_Image* building_health = new C_Image(building_health_go);
	building_health->target = { (0.31f) - ((0.31f - 0.07f) * (1.0f - *health / 100.0f)), pos_y - 0.007f, 1.255f * (*health / 100), 0.4f };
	building_health->offset = { -245.0f, -23.0f };
	building_health->section = { 57, 238, 245, 23 };
	building_health->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- UNIT ACTION --------------------------------------

	Gameobject* unit_action_go = AddGameobject("Unit Action", hud_canvas_go);
	C_Image* unit_action = new C_Image(unit_action_go);
	unit_action->target = { 0.35f, pos_y - 0.005f, 0.4f, 0.325f };
	
	if (action == "attack")
	{
		unit_action->offset = { -85.0f, -93.0f };
		unit_action->section = { 266, 291, 85, 93 };
	}

	else if(action == "defense")
	{
		unit_action->offset = { -85.0f, -93.0f };
		unit_action->section = { 378, 291, 85, 93 };
	}
	

	/*
	else if(type == "gather")
	{
		building_portrait->section = { __, __, 109, 93 };
	}
	
	*/

	unit_action->tex_id = App->tex.Load("textures/creation_bars_test.png");

}

Gameobject* Scene::GetRoot()
{
	return &root;
}

const Gameobject* Scene::GetRoot() const
{
	return &root;
}

Gameobject * Scene::AddGameobject(const char * name, Gameobject * parent)
{
	return new Gameobject(name, parent != nullptr ? parent : &root);
}

Gameobject* Scene::MouseClickSelect(int mouse_x, int mouse_y)
{
	Gameobject* ret = nullptr;
	std::queue<Gameobject*> queue;
	std::vector<Gameobject*> root_childs = root.GetChilds();
	
	for (std::vector<Gameobject*>::iterator it = root_childs.begin(); it != root_childs.end(); ++it)
		queue.push(*it);

	if (!queue.empty())
	{
		SDL_Rect cam_rect = App->render->GetCameraRect();
		std::pair<float, float> map_coordinates = Map::WorldToTileBase(cam_rect.x + mouse_x, cam_rect.y + mouse_y);

		while (!queue.empty())
		{
			// TODO: Quadtree
			Gameobject* go = queue.front();
			if (go != nullptr)
			{
				// Get transform
				Transform* t = go->GetTransform();
				if (t != nullptr)
				{
					// Check intersection
					if (t->Intersects(map_coordinates))
						ret = go;

					// Push childs
					std::vector<Gameobject*> go_childs = go->GetChilds();
					for (std::vector<Gameobject*>::iterator it = go_childs.begin(); it != go_childs.end(); ++it)
						queue.push(*it);
				}
			}

			queue.pop();
		}
	}

	return ret;
}