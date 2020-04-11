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
#include "EnemyMeleeUnit.h"
#include "MeleeUnit.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL_scancode.h"
#include "optick-1.3.0.0/include/optick.h"

#include <queue>
#include <vector>

#include <sstream>
#include <string.h>

Scene::Scene() : Module("scene")
{
	root.SetName("root");
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
		fade_timer += App->time.GetDeltaTime();

		if (fading == FADE_OUT)
		{
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
			alpha = (fade_duration - fade_timer) / fade_duration * 255.f;

			if (fade_timer >= fade_duration)
				fading = NO_FADE;
		}

		if (fading != NO_FADE)
			App->render->DrawQuadNormCoords({ 0.f, 0.f, 1.f, 1.f }, { 0, 0, 0, unsigned char(alpha) }, true, FADE);
	}
	else
	{
		if (god_mode)
			GodMode();

		//Pause Game
		if ((test || level) && !placing_building)
		{
			if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && pause == false)
			{
				Event::Push(SCENE_PAUSE, App);
				PauseMenu();
				pause = true;
			}
			else if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && pause == true)
			{
				pause_canvas_go->Destroy();
				Event::Push(SCENE_PLAY, App);
				pause = false;
			}
		}
		
		//Mob Drop Print Updated Value
		if (text_mobdrop_value) {
			std::stringstream ss;
			ss << mob_drop;
			std::string temp_str = ss.str();
			//const char* t = (char*)temp_str.c_str();
			text_mobdrop_value->text->SetText(temp_str.c_str());
		}

		//Edge Print Updated Value
		if (text_edge_value) {
			std::stringstream ss1;
			ss1 << edge_value;
			std::string temp_str1 = ss1.str();
			//const char* t1 = (char*)temp_str1.c_str();
			text_edge_value->text->SetText(temp_str1.c_str());
		}

		if (placing_building)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			RectF cam = App->render->GetCameraRectF();
			std::pair<int, int> pos = Map::WorldToTileBase(float(x) + cam.x, float(y) + cam.y);
			placing_building->SetLocalPos(vec(float(pos.first), float(pos.second), 0.f));

			if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			{
				placing_building->GetGameobject()->Destroy();
				placing_building = nullptr;
			}
			else if (App->input->GetMouseButtonDown(0) == KEY_DOWN)
			{
				PlaceMode(placing_building->GetGameobject()->GetBehaviour()->GetType());
			}
		}
		else
		{
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
					if (it->second->GetType() == UNIT_MELEE || it->second->GetType() == GATHERER || it->second->GetType() == UNIT_RANGED || it->second->GetType() == BASE_CENTER || it->second->GetType() == TOWER)
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
				if (!group.empty()) groupSelect = true;
				break;
			}
			default:
				break;
			}

			//GROUP MOVEMENT//
			if (!groupSelect && group.empty() == false)
			{
				std::vector<Gameobject*>::iterator it;
				for (it = group.begin(); it != group.end(); ++it)
				{
					Event::Push(ON_UNSELECT, *it);
				}
				group.clear();
			}

			//SELECTION RIGHT CLICK//
			if (App->input->GetMouseButtonDown(2) == KEY_DOWN)
			{
				int x, y;
				App->input->GetMousePosition(x, y);
				RectF cam = App->render->GetCameraRectF();
				std::pair<float, float> mouseOnMap = Map::F_WorldToMap(float(x) + cam.x, float(y) + cam.y);

				if (groupSelect && !group.empty())//Move group selected
				{
					for (std::vector<Gameobject*>::iterator it = group.begin(); it != group.end(); ++it)
						Event::Push(ON_RIGHT_CLICK, *it, mouseOnMap.first, mouseOnMap.second);
				}
				else//Move one selected
				{
					Gameobject* go = App->editor->selection;
					if (go)
					{
						Event::Push(ON_RIGHT_CLICK, go, mouseOnMap.first, mouseOnMap.second);
						groupSelect = false;
					}
					else groupSelect = false;
				}
			}
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
	map.CleanUp();
	
	return true;
}

void Scene::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case SCENE_PLAY:
	{
		Event::Push(ON_PLAY, &root);
		break;
	}
	case SCENE_PAUSE:
		Event::Push(ON_PAUSE, &root);
		break;
	case SCENE_STOP:
		Event::Push(ON_STOP, &root);
		break;
	case SCENE_CHANGE:
	{
		if ((fade_duration = e.data2.AsFloat()) != 0.f)
		{
			next_scene = SceneType(e.data1.AsInt());
			fade_duration = e.data2.AsFloat();

			fade_timer = 0.f;
			if (fade_duration < 0)
			{
				fade_duration *= -1.0f;
				fading = FADE_IN;
			}
			else
				fading = FADE_OUT;
		}
		else
		{
			ChangeToScene(SceneType(e.data1.AsInt()));
		}
		break; }
	case RESOURCE: 
		edge_value += e.data1.AsInt();
		LOG("Current resources: %d",edge_value);
		break;
	case PLACE_BUILDING:
	{
		PlaceMode(e.data1.AsInt());
		break;
	}
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
	
	test = true;

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

	//Resources
	Gameobject* resource_counter_go = AddGameobject("Resources", canvas_go);
	C_Image* img = new C_Image(resource_counter_go);
	img->target = { 0.1f, 1.f, 1.f , 1.f };
	img->offset = { -119.f, -119.f };
	img->section = { 22, 333, 119, 119 };
	img->tex_id = App->tex.Load("textures/Iconos_square_up.png");

	//Edge
	resources_go = AddGameobject("Text Edge", resource_counter_go);
	C_Text* text_edge = new C_Text(resources_go, "Edge");
	text_edge->target = { 0.1f, 0.1f, 1.f, 1.f };
	
	Gameobject* resources_value_go = AddGameobject("Mob Drop Value", resource_counter_go);
	text_edge_value = new C_Text(resources_go, "100");
	text_edge_value->target = { 0.1f, 0.4f, 1.f, 1.f };

	//MobDrop
	resources_2_go = AddGameobject("Text Mob Drop", resource_counter_go);
	C_Text* text_mobdrop = new C_Text(resources_2_go, "Mob Drop");
	text_mobdrop->target = { 0.5f, 0.1f, 1.f, 1.f };
	
	Gameobject* resources_value_2_go = AddGameobject("Mob Drop Value", resource_counter_go);
	text_mobdrop_value = new C_Text(resources_2_go, "0");
	text_mobdrop_value->target = { 0.5f, 0.4f, 1.f, 1.f };
	
	//Minimap
	Gameobject* minimap_go = AddGameobject("Minimap", canvas_go);
	minimap = new Minimap(minimap_go);

	// Build mode
	Gameobject* builder = AddGameobject("Building Mode", canvas_go);
	C_Button* base = new C_Button(builder, Event(PLACE_BUILDING, this, int(BASE_CENTER)));
	C_Button* tower = new C_Button(builder, Event(PLACE_BUILDING, this, int(TOWER)));
	C_Button* edge = new C_Button(builder, Event(PLACE_BUILDING, this, int(EDGE)));
	base->target = { 0.5f, 1.f, 1.f , 1.f };
	tower->target = { 0.6f, 1.f, 1.f , 1.f };
	edge->target = { 0.7f, 1.f, 1.f , 1.f };
	base->offset = tower->offset = edge->offset ={ -40.f, -80.f };
	base->section = tower->section = edge->section = { 0, 0, 80, 80 };

	return ret;
}

bool Scene::LoadMainScene()
{
	OPTICK_EVENT();

	level = true;

	building_bars_created = 0;
	unit_bars_created = 0;

	//------------------------- HUD CANVAS --------------------------------------

	hud_canvas_go = AddGameobject("HUD Canvas", &root);
	C_Canvas* hud_canv = new C_Canvas(hud_canvas_go);
	hud_canv->target = { 0.3f, 0.3f, 0.4f, 0.4f };


	return map.Load("maps/iso.tmx") && App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");
}

bool Scene::LoadIntroScene()
{
	OPTICK_EVENT();
	// Play sample track
	bool ret = App->audio->PlayFx(LOGO);

	level = false;
	test = false;

	// Add a canvas
	Gameobject* canvas_go = AddGameobject("Canvas", &root);
	C_Canvas* canv = new C_Canvas(canvas_go);
	canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };

	Gameobject* background_go = AddGameobject("Background", canvas_go);

	C_Button* background = new C_Button(background_go, Event(SCENE_CHANGE, this, MENU, 2.f));
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
	bool ret = App->audio->PlayMusic("audio/Music/alexander-nakarada-curiosity.ogg") && App->audio->PlayFx(TITLE);

	level = false;
	test = false;

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
	start->tex_id = App->tex.Load("textures/play.png");

	C_Button* start_fx = new C_Button(start_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	start_fx->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	start_fx->offset = { -525.f, -100.f };
	start_fx->section = { 0, 0, 1070, 207 };

	//------------------------- QUIT --------------------------------------

	Gameobject* quit_go = AddGameobject("Quit Button", canvas_go);

	C_Button* quit = new C_Button(quit_go, Event(REQUEST_QUIT, App));
	quit->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	quit->offset = { -525.f, 200.f };
	quit->section = { 0, 0, 1070, 207 };
	quit->tex_id = App->tex.Load("textures/quit.png");

	C_Button* quit_fx = new C_Button(quit_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	quit_fx->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	quit_fx->offset = { -525.f, 200.f };
	quit_fx->section = { 0, 0, 1070, 207 };

	return ret;
}

bool Scene::LoadEndScene()
{
	OPTICK_EVENT();

	bool ret;

	level = false;
	test = false;

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

	//------------------------- WIN/LOSE --------------------------------------
	if (win)
	{
		Gameobject* win_go = AddGameobject("Background", canvas_go);

		C_Image* win = new C_Image(win_go);
		win->target = { 0.58f, 0.2f, 0.5f, 0.5f };
		win->offset = { -442.f, -117.f };
		win->section = { 0, 0, 442, 117 };
		win->tex_id = App->tex.Load("textures/youwin.png");
	}
	else
	{
		Gameobject* lose_go = AddGameobject("Background", canvas_go);

		C_Image* lose = new C_Image(lose_go);
		lose->target = { 0.59f, 0.2f, 0.5f, 0.5f };
		lose->offset = { -495.f, -117.f };
		lose->section = { 0, 0, 495, 117 };
		lose->tex_id = App->tex.Load("textures/youlose.png");
	}

	//------------------------- BACK --------------------------------------

	Gameobject* back_go = AddGameobject("Background", canvas_go);

	C_Image* back = new C_Image(back_go);
	back->target = { 0.68f, 0.9f, 0.6f, 0.65f };
	back->offset = { -783.f, -735.f };
	back->section = { 0, 0, 783, 735 };
	back->tex_id = App->tex.Load("textures/back.png");

	//------------------------- TIME --------------------------------------

	Gameobject* time_go = AddGameobject("Time", canvas_go);

	C_Image* time = new C_Image(time_go);
	time->target = { 0.66f, 0.37f, 0.6f, 0.65f };
	time->offset = { -693.f, -100.f };
	time->section = { 0, 0, 693, 100 };
	if (win) time->tex_id = App->tex.Load("textures/wtime.png");
	else time->tex_id = App->tex.Load("textures/ltime.png");

	//------------------------- EDGE --------------------------------------

	Gameobject* edge_go = AddGameobject("edge", canvas_go);

	C_Image* edge = new C_Image(edge_go);
	edge->target = { 0.66f, 0.49f, 0.6f, 0.65f };
	edge->offset = { -693.f, -100.f };
	edge->section = { 0, 0, 693, 100 };
	if (win) edge->tex_id = App->tex.Load("textures/wedge.png");
	else edge->tex_id = App->tex.Load("textures/ledge.png");

	//------------------------- UNITS CREATED --------------------------------------

	Gameobject* units_c_go = AddGameobject("created", canvas_go);

	C_Image* units_c = new C_Image(units_c_go);
	units_c->target = { 0.66f, 0.61f, 0.6f, 0.65f };
	units_c->offset = { -693.f, -100.f };
	units_c->section = { 0, 0, 693, 100 };
	if (win) units_c->tex_id = App->tex.Load("textures/wunits_c.png");
	else units_c->tex_id = App->tex.Load("textures/lunits_c.png");

	//------------------------- UNITS LOST --------------------------------------

	Gameobject* units_l_go = AddGameobject("lost", canvas_go);

	C_Image* units_l = new C_Image(units_l_go);
	units_l->target = { 0.66f, 0.73f, 0.6f, 0.65f };
	units_l->offset = { -693.f, -100.f };
	units_l->section = { 0, 0, 693, 100 };
	if (win) units_l->tex_id = App->tex.Load("textures/wunits_l.png");
	else units_l->tex_id = App->tex.Load("textures/lunits_l.png");

	//------------------------- UNITS KILLED --------------------------------------

	Gameobject* units_k_go = AddGameobject("killed", canvas_go);

	C_Image* units_k = new C_Image(units_k_go);
	units_k->target = { 0.66f, 0.85f, 0.6f, 0.65f };
	units_k->offset = { -693.f, -100.f };
	units_k->section = { 0, 0, 693, 100 };
	if (win) units_k->tex_id = App->tex.Load("textures/wunits_k.png");
	else units_k->tex_id = App->tex.Load("textures/lunits_k.png");

	if (win) ret = App->audio->PlayMusic("audio/Music/alexander-nakarada-early-probe-eats-the-dust.ogg");
	else ret = App->audio->PlayMusic("audio/Music/alexander-nakarada-inter7ude.ogg");

	return ret;
}

bool Scene::PauseMenu()
{
	bool ret = true;
	//------------------------- CANVAS --------------------------------------

	pause_canvas_go = AddGameobject("Canvas", &root);
	C_Canvas* canv = new C_Canvas(pause_canvas_go);
	canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };

	//------------------------- BACKGROUND -----------------------------------

	Gameobject* background_go = AddGameobject("Background", pause_canvas_go);

	C_Image* background = new C_Image(background_go);
	background->target = { 0.66f, 0.95f, 0.6f, 0.6f };
	background->offset = { -640.f, -985.f };
	background->section = { 0, 0, 640, 985 };
	background->tex_id = App->tex.Load("textures/pause-bg.png");

	//------------------------- RESUME -----------------------------------------

	Gameobject* resume_go = AddGameobject("resume Button", pause_canvas_go);

	C_Button* resume = new C_Button(resume_go, Event(SCENE_PLAY, this, App));
	resume->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	resume->offset = { -525.f, -100.f };
	resume->section = { 0, 0, 1070, 207 };
	resume->tex_id = App->tex.Load("textures/button.png");

	C_Button* resume_fx = new C_Button(resume_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	resume_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	resume_fx->offset = { -525.f, -100.f };
	resume_fx->section = { 0, 0, 1070, 207 };

	/*//------------------------- SAVE --------------------------------------

	Gameobject* save_go = AddGameobject("save button", pause_canvas_go);

	C_Button* save = new C_Button(save_go, Event(SCENE_CHANGE, this, MAIN));
	save->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	save->offset = { -525.f, 200.f };
	save->section = { 0, 0, 1070, 207 };
	save->tex_id = App->tex.Load("textures/button.png");

	C_Button* save_fx = new C_Button(save_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	save_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	save_fx->offset = { -525.f, 200.f };
	save_fx->section = { 0, 0, 1070, 207 };

	//------------------------- LOAD --------------------------------------

	Gameobject* load_go = AddGameobject("load Button", pause_canvas_go);

	C_Button* load = new C_Button(load_go, Event(SCENE_CHANGE, this, MAIN));
	load->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	load->offset = { -525.f, 500.f };
	load->section = { 0, 0, 1070, 207 };
	load->tex_id = App->tex.Load("textures/button.png");

	C_Button* load_fx = new C_Button(load_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	load_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	load_fx->offset = { -525.f, 500.f };
	load_fx->section = { 0, 0, 1070, 207 };

	//------------------------- OPTIONS --------------------------------------

	Gameobject* options_go = AddGameobject("options Button", pause_canvas_go);

	C_Button* options = new C_Button(options_go, Event(SCENE_CHANGE, this, MAIN));
	options->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	options->offset = { -525.f, 800.f };
	options->section = { 0, 0, 1070, 207 };
	options->tex_id = App->tex.Load("textures/button.png");

	C_Button* options_fx = new C_Button(options_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	options_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	options_fx->offset = { -525.f, 800.f };
	options_fx->section = { 0, 0, 1070, 207 };*/

	//------------------------- MAIN MENU --------------------------------------

	Gameobject* main_menu_go = AddGameobject("main menu Button", pause_canvas_go);

	C_Button* main_menu = new C_Button(main_menu_go, Event(SCENE_CHANGE, this, MENU));
	main_menu->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	main_menu->offset = { -525.f, 200.f  };
	main_menu->section = { 0, 0, 1070, 207 };
	main_menu->tex_id = App->tex.Load("textures/button.png");

	C_Button* main_menu_fx = new C_Button(main_menu_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	main_menu_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	main_menu_fx->offset = { -525.f, 200.f };
	main_menu_fx->section = { 0, 0, 1070, 207 };

	return ret;
}

/*bool Scene::DestroyPauseMenu()
{
	bool ret = true;

	pause_canvas_go->Destroy();

	return true;
}*/

bool Scene::ChangeToScene(SceneType scene)
{
	text_mobdrop_value = nullptr;
	text_edge_value = nullptr;

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
		ret = LoadEndScene();
		break;
	case CREDITS:
		break;
	default:
		break;
	}

	return ret;
}

void Scene::PlaceMode(int type)
{
	placing_building = nullptr;
	Gameobject* go = nullptr;

	switch (UnitType(type))
	{
	case BASE_CENTER: new Base_Center(go = AddGameobject("Base Center")); break;
	case TOWER: new Tower(go = AddGameobject("Tower")); break;
	case WALL: break;
	case BARRACKS: break;
	case LAB: break;
	case EDGE: new Edge(go = AddGameobject("Tower")); break;
	default: break;
	}

	if (go)
		placing_building = go->GetTransform();
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
					{
						ret = go;
						groupSelect = false;
					}

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

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, TEST, 0.f);
		else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, INTRO, 2.f);
		else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, MENU, 2.f);
		else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, MAIN, 2.f);
		else if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
			Event::Push(SCENE_CHANGE, this, END, 2.f);
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

		minimap->AddToMinimap(unit_go);

		new MeleeUnit(unit_go);
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

		new EnemyMeleeUnit(unit_go);
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
		if (edge_value > 10)
		{
			std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
			if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
			{
				Gameobject* gather_go = AddGameobject("Gatherer unit");
				gather_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

				new Gatherer(gather_go);
				edge_value -= 10;
				LOG("Current resources: %d", edge_value);
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

		if (App->editor->selection != nullptr)
		{
			App->editor->selection->Destroy();
		}
	}


	/////Temp
	/*if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		App->input->GetMousePosition(atkPos.first, atkPos.second);
		pos.first = 10;
		pos.second = 10;
		shoot = true;
	}

	if (shoot)
	{
		rayCastTimer += App->time.GetDeltaTime();
		if (rayCastTimer < RAYCAST_TIME)
		{
			App->render->DrawLine(pos, atkPos, { 0,0,255,255 }, SCENE,false);
		}
		else
		{
			shoot = false;
			rayCastTimer = 0;
		}
	}*/
	/////

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
