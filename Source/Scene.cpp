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
		App->audio->PauseMusic();

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
	}

	return ret;
}

bool Scene::LoadMainScene()
{
	OPTICK_EVENT();
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