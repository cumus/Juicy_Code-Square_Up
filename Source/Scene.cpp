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
#include "Gatherer.h"

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

	root.Update();

	if (fading != NO_FADE)
	{
		float alpha;

		if (fading == FADE_OUT)
		{
			fade_timer += App->time.GetDeltaTime();
			alpha = fade_timer / fade_duration * 255.f;

			if (fade_timer >= fade_duration)
			{
				ChangeToScene(next_scene);
				Event::Push(SCENE_PLAY, App);
				fading = FADE_IN;
				fade_timer = 0.f;
			}
		}
		else if (fading == FADE_IN)
		{
			fade_timer += App->time.GetDeltaTime();
			alpha = (fade_duration - fade_timer) / fade_duration * 255.f;

			if (fade_timer >= fade_duration)
			{
				fading = NO_FADE;
			}
		}

		if (fading != NO_FADE)
			App->render->DrawQuadNormCoords({ 0.f, 0.f, 1.f, 1.f }, { 0, 0, 0, unsigned char(alpha) }, true, FADE);
	}
	else
	{
		if (god_mode)
			GodMode();

		//GROUP SELECTION//
		switch (App->input->GetMouseButtonDown(0))
		{
		case KEY_DOWN:
		{
			App->input->GetMousePosition(groupStart.x, groupStart.y);
			break;
		}
		case KEY_REPEAT:
		{
			App->input->GetMousePosition(mouseExtend.x, mouseExtend.y);
			App->render->DrawQuad({ groupStart.x, groupStart.y, mouseExtend.x - groupStart.x, mouseExtend.y - groupStart.y }, { 0, 200, 0, 100 }, false, SCENE, false);
			App->render->DrawQuad({ groupStart.x, groupStart.y, mouseExtend.x - groupStart.x, mouseExtend.y - groupStart.y }, { 0, 200, 0, 50 }, true, SCENE, false);
			break;
		}
		case KEY_UP:
		{
			SDL_Rect cam = App->render->GetCameraRect();
			for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
			{
				if (it->second->GetType() == UNIT_MELEE || it->second->GetType() == GATHERER || it->second->GetType() == UNIT_RANGED)
				{
					vec pos = it->second->GetGameobject()->GetTransform()->GetGlobalPosition();
					std::pair<float, float> posToWorld = Map::F_MapToWorld(pos.x, pos.y, pos.z);
					posToWorld.first -= cam.x;
					posToWorld.second -= cam.y;

					if (posToWorld.first > groupStart.x && posToWorld.first < mouseExtend.x) //Right
					{
						if (posToWorld.second > groupStart.y && posToWorld.second < mouseExtend.y)//Up
						{
							group.push_back(it->second->GetGameobject());
							Event::Push(ON_SELECT, it->second->GetGameobject());
						}
						else if (posToWorld.second < groupStart.y && posToWorld.second > mouseExtend.y)//Down
						{
							group.push_back(it->second->GetGameobject());
							Event::Push(ON_SELECT, it->second->GetGameobject());
						}
					}
					else if (posToWorld.first < groupStart.x && posToWorld.first > mouseExtend.x)//Left
					{
						if (posToWorld.second > groupStart.y && posToWorld.second < mouseExtend.y)//Up
						{
							group.push_back(it->second->GetGameobject());
							Event::Push(ON_SELECT, it->second->GetGameobject());
						}
						else if (posToWorld.second < groupStart.y && posToWorld.second > mouseExtend.y)//Down
						{
							group.push_back(it->second->GetGameobject());
							Event::Push(ON_SELECT, it->second->GetGameobject());
						}
					}
				}
			}

			if (!group.empty())
				groupSelect = true;

			break;
		}
		default:
			break;
		}
	}

	return true;
}

bool Scene::PostUpdate()
{
	root.PostUpdate();
	map.Draw();

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
	{
		if ((fade_duration = e.data2.AsFloat()) > 0.f)
		{
			next_scene = SceneType(e.data1.AsInt());
			fading = FADE_OUT;
			fade_timer = 0.f;
		}
		else
		{
			ChangeToScene(SceneType(e.data1.AsInt()));
			Event::Push(ON_PLAY, &root);
		}
		break; }
	case RESOURCE: 
		resources += e.data1.AsInt();
		LOG("Current resources: %d",resources);
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

	if (ret) map.Load("maps/iso.tmx");
	
	building_bars_created = 0;
	unit_bars_created = 0;

	//------------------------- HUD CANVAS --------------------------------------

	hud_canvas_go = AddGameobject("HUD Canvas", &root);
	C_Canvas* hud_canv = new C_Canvas(hud_canvas_go);
	hud_canv->target = { 0.3f, 0.3f, 0.4f, 0.4f };


	// HUD
	Gameobject* canvas_go = AddGameobject("Canvas", &root);
	C_Canvas* canv = new C_Canvas(canvas_go);
	canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };
	/*
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
	button->tex_id = App->tex.Load("textures/icons.png");*/

	Gameobject* minimap_go = AddGameobject("Minimap", canvas_go);
	Minimap* minimap = new Minimap(minimap_go);
	minimap->target = { 0.f, 1.f, 0.2f, 0.2f };
	minimap->offset = { 0, -1080 };
	minimap->section = { 0, 0, 1920, 1080 };
	minimap->CreateMinimap();


	return ret;
}

bool Scene::LoadMainScene()
{
	OPTICK_EVENT();

	building_bars_created = 0;
	unit_bars_created = 0;

	//------------------------- HUD CANVAS --------------------------------------

	hud_canvas_go = AddGameobject("HUD Canvas", &root);
	C_Canvas* hud_canv = new C_Canvas(hud_canvas_go);
	hud_canv->target = { 0.3f, 0.3f, 0.4f, 0.4f };


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
	
	//------------------------- START --------------------------------------

	Gameobject* start_go = AddGameobject("Start Button", canvas_go);
	
	C_Button* start = new C_Button(start_go, Event(SCENE_CHANGE, this, MAIN, 2.f));
	start->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	start->offset = { -525.f, -100.f };
	start->section = { 0, 0, 1070, 207 };
	start->tex_id = App->tex.Load("textures/button.png");

	C_Button* start_fx = new C_Button(start_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	start_fx->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	start_fx->offset = { -525.f, -100.f };
	start_fx->section = { 0, 0, 1070, 207 };

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

	C_Button* quit_fx = new C_Button(quit_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	quit_fx->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	quit_fx->offset = { -525.f, 200.f };
	quit_fx->section = { 0, 0, 1070, 207 };

	Gameobject* quit_txt_go = AddGameobject("Start Button", quit_go);

	C_Text* quit_txt = new C_Text(quit_txt_go, "QUIT");
	quit_txt->target = { 0.48f, 0.45f, 1.f, 1.f };
	quit_txt->scale_to_fit = true;

	return ret;
}

bool Scene::ChangeToScene(SceneType scene)
{
	map.CleanUp();
	App->audio->UnloadFx();
	App->audio->PauseMusic(1.f);
	App->editor->SetSelection(nullptr, false);
	root.RemoveChilds();
	Event::PumpAll();

	bool ret = false;
	switch (current_scene = scene)
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

void Scene::GodMode()
{
	int x, y;
	App->input->GetMousePosition(x, y);
	SDL_Rect cam = App->render->GetCameraRect();

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, TEST);
		else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, INTRO, 2.f);
		else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, MENU, 2.f);
		else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, MAIN, 2.f);
	}


	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		map.draw_walkability = !map.draw_walkability;

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		App->audio->PauseMusic(1.f);

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");

	// Swap map orientation
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) Map::SwapMapType();

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		Gameobject* audio_go = AddGameobject("AudioSource - Hammer Loop");
		audio_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		(new AudioSource(audio_go))->Play(HAMMER, -1);
		new Sprite(audio_go, id_mouse_tex, { 128, 0, 64, 64 });
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)//Melee unit
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		Gameobject* unit_go = AddGameobject("Ally Melee unit");
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

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)// Enemy melee
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		Gameobject* unit_go = AddGameobject("Enemy Melee unit");
		unit_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		new B_Unit(unit_go, ENEMY_MELEE, IDLE);
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

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN) //Gatherer
	{
		if (resources > 10)
		{
			std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
			if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
			{
				Gameobject* gather_go = AddGameobject("Gatherer unit");
				gather_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

				new Gatherer(gather_go);
				resources -= 10;
				LOG("Current resources: %d", resources);
			}
			else
				LOG("Invalid spawn position");
		}
		
	}

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		App->pathfinding.DebugShowPaths();
	}

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{

		if (!group.empty()) {
			std::vector<Gameobject*>::iterator it;
			for (it = group.begin(); it != group.end(); ++it)
			{
				(*it)->Destroy();

			}
			groupSelect = false;
		}

		if (App->editor->selection)
		{
			App->editor->selection->Destroy();
		}
	}

	if (App->input->GetMouseButtonDown(2) == KEY_DOWN)
	{
		if (groupSelect && group.empty() == false)//Move group selected
		{
			std::vector<Gameobject*>::iterator it;
			it = group.begin();
			for (it = group.begin(); it != group.end(); ++it)
			{
				std::pair<float, float> mouseOnMap = Map::F_WorldToMap(float(x + cam.x), float(y + cam.y));
				Event::Push(ON_RIGHT_CLICK, *it, mouseOnMap.first, mouseOnMap.second);
			}
		}
		else//Move one selected
		{
			Gameobject* go = App->editor->selection;
			if (go)
			{
				std::pair<float, float> mouseOnMap = Map::F_WorldToMap(float(x + cam.x), float(y + cam.y));
				Event::Push(ON_RIGHT_CLICK, go, mouseOnMap.first, mouseOnMap.second);
				groupSelect = false;
			}
			else groupSelect = false;
		}
	}
	if (!groupSelect && group.empty() == false)
	{
		std::vector<Gameobject*>::iterator it;
		for (it = group.begin(); it != group.end(); ++it)
		{
			Event::Push(ON_UNSELECT, *it);
		}
		group.clear();
	}

	// Update window title
	std::pair<int, int> map_coordinates = Map::WorldToTileBase(cam.x + x, cam.y + y);
	Gameobject* sel = App->editor->selection;

	// Log onto window title
	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Zoom: %0.2f, Mouse: %dx%d, Tile: %dx%d, Selection: %s",
		App->time.GetLastFPS(),
		App->render->GetZoom(),
		x, y,
		map_coordinates.first, map_coordinates.second,
		sel != nullptr ? sel->GetName() : "none selected");

	App->win->SetTitle(tmp_str);
}
