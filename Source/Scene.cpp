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
#include "Spawner.h"
#include "Barracks.h"
#include "JuicyMath.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL_scancode.h"
#include "optick-1.3.0.0/include/optick.h"

#include <queue>
#include <vector>

#include <sstream>
#include <string.h>
#include <time.h>


bool Scene::god_mode = false;
bool Scene::no_damage = false;

Scene::Scene() : Module("scene")
{
	root.SetName("root");
	baseCenterPos.first = -1;
	baseCenterPos.second = -1;
	spawnPoints.push_back(vec(20,20,0.5));
	spawnPoints.push_back(vec(30, 20, 0.5));
	spawnPoints.push_back(vec(20, 30, 0.5));
	currentSpawns = 0;
	maxSpawns = 200;
	spawnCounter = 0;
	cooldownSpawn = 5.0f;
	last_cam_pos = { 0,0 };
	minimap = nullptr;
	groupSelect = false;
	pause_background_go = nullptr;
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

	UpdateStateMachine();

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
			god_mode = !god_mode;

	if (god_mode)
			GodMode();

	UpdateSpawner();

	if (fading != NO_FADE)
	{
		UpdateFade();
	}
	else
	{
		
		UpdateHUD();

		if (placing_building)
		{
			UpdateBuildingMode();
		}
		else
		{
			UpdatePause();
			UpdateSelection();
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
		if (fading == NO_FADE)
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
	case BASE_DESTROYED: //Lose condition
		ChangeToScene(END);
		break;
	case MOB_DROP: 
		mob_drop += e.data1.AsInt();
		break;
	case GAMEPLAY:
	{
		OnEventStateMachine(GameplayState(e.data1.AsInt()));
	}
	default:
		break;
	}
}

bool Scene::LoadTestScene()
{
	OPTICK_EVENT();

	god_mode = true;

	// Play sample track
	bool ret = App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-buzzkiller.ogg");

	// Load mouse debug texture for identifying tiles
	if (ret)
	{
		id_mouse_tex = App->tex.Load("Assets/textures/meta.png");
		ret = (id_mouse_tex != -1);
	}

	if (ret) map.Load("Assets/maps/iso.tmx");
	
	test = true;

	int icons_text_id = App->tex.Load("Assets/textures/Iconos_square_up.png");

	building_bars_created = 0;
	int current_melee_units = 0;
	int melee_units_created = 0;
	int current_ranged_units = 0;
	int ranged_units_created = 0;
	int current_gatherer_units = 0;
	int gatherer_units_created = 0;
	

	//------------------------- HUD CANVAS --------------------------------------

	hud_canvas_go = AddGameobject("HUD Canvas", &root);

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


	// Unit icon (Melee Unit)
	Gameobject* melee_counter_go = AddGameobjectToCanvas("Melee Unit Counter");

	C_Image* melee_counter_box = new C_Image(melee_counter_go);
	melee_counter_box->target = { 0.153f, 0.64f, 0.55f , 1.f };
	melee_counter_box->offset = { -345.f, -45.f };
	melee_counter_box->section = { 17, 509, 345, 45 };
	melee_counter_box->tex_id = icons_text_id;

	C_Image* melee_counter_icon = new C_Image(melee_counter_go);
	melee_counter_icon->target = { 0.047f, 0.628f, 0.9f , 0.9f };
	melee_counter_icon->offset = { -48.f, -35.f };
	melee_counter_icon->section = { 22, 463, 48, 35 };
	melee_counter_icon->tex_id = icons_text_id;

	text_current_melee_units = new C_Text(melee_counter_go, "0");
	text_current_melee_units->target = { 0.049f, 0.587f, 1.6f, 1.6f };

	C_Text* melee_diagonal = new C_Text(melee_counter_go, "/");
	melee_diagonal->target = { 0.088f, 0.587f, 1.6f, 1.6f };

	text_melee_units_created = new C_Text(melee_counter_go, "0");
	text_melee_units_created->target = { 0.099f, 0.587f, 1.6f, 1.6f };

	// Unit icon (Gatherer Unit)
	Gameobject* gatherer_counter_go = AddGameobjectToCanvas("Gatherer Unit Counter");

	C_Image* gatherer_counter_box = new C_Image(gatherer_counter_go);
	gatherer_counter_box->target = { 0.153f, 0.72f, 0.55f , 1.f };
	gatherer_counter_box->offset = { -345.f, -45.f };
	gatherer_counter_box->section = { 17, 509, 345, 45 };
	gatherer_counter_box->tex_id = icons_text_id;

	C_Image* gatherer_counter_icon = new C_Image(gatherer_counter_go);
	gatherer_counter_icon->target = { 0.041f, 0.708f, 0.9f , 0.9f };
	gatherer_counter_icon->offset = { -48.f, -35.f };
	gatherer_counter_icon->section = { 75, 458, 48, 35 };
	gatherer_counter_icon->tex_id = icons_text_id;

	text_current_gatherer_units = new C_Text(gatherer_counter_go, "0");
	text_current_gatherer_units->target = { 0.049f, 0.667f, 1.6f, 1.6f };

	C_Text* gatherer_diagonal = new C_Text(gatherer_counter_go, "/");
	gatherer_diagonal->target = { 0.088f, 0.667f, 1.6f, 1.6f };

	text_gatherer_units_created = new C_Text(gatherer_counter_go, "0");
	text_gatherer_units_created->target = { 0.099f, 0.667f, 1.6f, 1.6f };

	// Unit icon (Ranged Unit)
	Gameobject* ranged_counter_go = AddGameobjectToCanvas("Ranged Unit Counter");

	C_Image* ranged_counter_box = new C_Image(ranged_counter_go);
	ranged_counter_box->target = { 0.153f, 0.80f, 0.55f , 1.f };
	ranged_counter_box->offset = { -345.f, -45.f };
	ranged_counter_box->section = { 17, 509, 345, 45 };
	ranged_counter_box->tex_id = icons_text_id;

	/*
	C_Image* ranged_counter_icon = new C_Image(ranged_counter_go);
	ranged_counter_icon->target = { 0.047f, 0.788f, 0.9f , 0.9f };
	ranged_counter_icon->offset = { -48.f, -35.f };
	ranged_counter_icon->section = { 22, 463, 48, 35 };
	ranged_counter_icon->tex_id = icons_text_id;
	*/

	text_current_ranged_units = new C_Text(ranged_counter_go, "0");
	text_current_ranged_units->target = { 0.049f, 0.747f, 1.6f, 1.6f };

	C_Text* ranged_diagonal = new C_Text(ranged_counter_go, "/");
	ranged_diagonal->target = { 0.088f, 0.747f, 1.6f, 1.6f };

	text_ranged_units_created = new C_Text(ranged_counter_go, "0");
	text_ranged_units_created->target = { 0.099f, 0.747f, 1.6f, 1.6f };


	//Resources
	Gameobject* resource_counter_go = AddGameobjectToCanvas("Resources");
	C_Image* img = new C_Image(resource_counter_go);
	img->target = { 0.1f, 1.f, 1.f , 1.f };
	img->offset = { -119.f, -119.f };
	img->section = { 22, 333, 119, 119 };
	img->tex_id = icons_text_id;

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
	text_mobdrop->target = { 0.45f, 0.8f, 1.f, 1.f };
	
	Gameobject* resources_value_2_go = AddGameobject("Mob Drop Value", resource_counter_go);
	text_mobdrop_value = new C_Text(resources_2_go, "0");
	text_mobdrop_value->target = { 0.65f, 0.4f, 1.f, 1.f };
	
	//Minimap
	if (minimap != nullptr)
	{
		Gameobject* minimap_go = AddGameobjectToCanvas("Minimap");
		minimap = new Minimap(minimap_go);
	}

	// Build mode
	Gameobject* builder = AddGameobjectToCanvas("Building Mode");
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

	tutorial_barrack = 0;
	
	lore_go = AddGameobjectToCanvas("lore");
	C_Image* lore = new C_Image(lore_go);
	C_Button* next = new C_Button(lore_go, Event(GAMEPLAY, this, CAM_MOVEMENT));

	lore->target = { 0.66f, 0.95f, 0.6f, 0.6f };
	lore->offset = { -640.f, -985.f };
	lore->section = { 0, 0, 640, 985 };
	lore->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

	next->target = { 0.51f, 0.3f, 0.3f, 0.3f };
	next->offset = { -525.f, 200.f };
	next->section = { 0, 0, 1070, 207 };
	next->tex_id = App->tex.Load("Assets/textures/button.png");

	map.Load("Assets/maps/iso.tmx");

	//Minimap
	Gameobject* minimap_go = AddGameobjectToCanvas("Minimap");
	minimap = new Minimap(minimap_go);

	std::pair<int, int> position = Map::WorldToTileBase(float(450.0f), float(450.0f));
	if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
	{
		Gameobject* base_go = AddGameobjectToCanvas("Base Center");
		base_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		//App->audio->PlayFx(B_BUILDED);
		new Base_Center(base_go);
		baseCenterPos.first = base_go->GetTransform()->GetGlobalPosition().x;
		baseCenterPos.second = base_go->GetTransform()->GetGlobalPosition().y;
		for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)//Update paths 
		{
			Event::Push(UPDATE_PATH, it->second, baseCenterPos.first - 1, baseCenterPos.second - 1);
		}
	}

	return  App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-buzzkiller.ogg");
}

bool Scene::LoadIntroScene()
{
	OPTICK_EVENT();
	// Play sample track
	bool ret = App->audio->PlayFx(LOGO);

	level = false;
	test = false;

	// Add a canvas

	Gameobject* background_go = AddGameobjectToCanvas("Background");

	C_Button* background = new C_Button(background_go, Event(SCENE_CHANGE, this, MENU, 2.f));
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1920.f, -1080.f };
	background->section = { 0, 0, 1920, 1080 };
	background->tex_id = App->tex.Load("Assets/textures/white.png");
	
	Gameobject* logo_go = AddGameobjectToCanvas("Team logo");

	C_Image* logo = new C_Image(logo_go);
	logo->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	logo->offset = { -300.f, -400.f };
	logo->section = { 0, 0, 499, 590 };
	logo->tex_id = App->tex.Load("Assets/textures/team-logo2.png");

	return ret;
	
}

bool Scene::LoadMenuScene()
{
	OPTICK_EVENT();
	// Play sample track
	bool ret = App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-curiosity.ogg") && App->audio->PlayFx(TITLE);

	level = false;
	test = false;

	//------------------------- CANVAS --------------------------------------
	/*Gameobject* canvas_go = AddGameobject("Canvas", &root);
	C_Canvas* canv = new C_Canvas(canvas_go);
	canv->target = { 0.6f, 0.6f, 0.4f, 0.4f };*/

	//------------------------- BACKGROUND --------------------------------------

	Gameobject* background_go = AddGameobjectToCanvas("Background");

	C_Image* background = new C_Image(background_go);
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1920.f, -1080.f };
	background->section = { 0, 0, 1920, 1080 };
	background->tex_id = App->tex.Load("Assets/textures/white.png");

	//------------------------- LOGO --------------------------------------

	Gameobject* g_logo_go = AddGameobjectToCanvas("Game logo");

	C_Image* g_logo = new C_Image(g_logo_go);
	g_logo->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	g_logo->offset = { -525.f, -500.f };
	g_logo->section = { 0, 0, 1070, 207 };
	g_logo->tex_id = App->tex.Load("Assets/textures/game-logo.png");
	
	//------------------------- START --------------------------------------

	Gameobject* start_go = AddGameobjectToCanvas("Start Button");
	
	C_Button* start = new C_Button(start_go, Event(SCENE_CHANGE, this, MAIN, 2.f));
	start->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	start->offset = { -525.f, -100.f };
	start->section = { 0, 0, 1070, 207 };
	start->tex_id = App->tex.Load("Assets/textures/play.png");

	C_Button* start_fx = new C_Button(start_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
	start_fx->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	start_fx->offset = { -525.f, -100.f };
	start_fx->section = { 0, 0, 1070, 207 };

	//------------------------- QUIT --------------------------------------

	Gameobject* quit_go = AddGameobjectToCanvas("Quit Button");

	C_Button* quit = new C_Button(quit_go, Event(REQUEST_QUIT, App));
	quit->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	quit->offset = { -525.f, 200.f };
	quit->section = { 0, 0, 1070, 207 };
	quit->tex_id = App->tex.Load("Assets/textures/quit.png");

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

	//------------------------- BACKGROUND --------------------------------------

	Gameobject* background_go = AddGameobjectToCanvas("Background");

	C_Image* background = new C_Image(background_go);
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1920.f, -1080.f };
	background->section = { 0, 0, 1920, 1080 };
	background->tex_id = App->tex.Load("Assets/textures/white.png");
	C_Button* background_btn = new C_Button(background_go, Event(SCENE_CHANGE, this, MENU));
	background_btn->target = { 1.f, 1.f, 1.f, 1.f };
	background_btn->offset = { -1920.f, -1080.f };
	background_btn->section = { 0, 0, 1920, 1080 };

	//------------------------- WIN/LOSE --------------------------------------
	if (win)
	{
		Gameobject* win_go = AddGameobjectToCanvas("Background");

		C_Image* win = new C_Image(win_go);
		win->target = { 0.58f, 0.2f, 0.5f, 0.5f };
		win->offset = { -442.f, -117.f };
		win->section = { 0, 0, 442, 117 };
		win->tex_id = App->tex.Load("Assets/textures/youwin.png");
	}
	else
	{
		Gameobject* lose_go = AddGameobjectToCanvas("Background");

		C_Image* lose = new C_Image(lose_go);
		lose->target = { 0.59f, 0.2f, 0.5f, 0.5f };
		lose->offset = { -495.f, -117.f };
		lose->section = { 0, 0, 495, 117 };
		lose->tex_id = App->tex.Load("Assets/textures/youlose.png");
	}

	//------------------------- BACK --------------------------------------

	Gameobject* back_go = AddGameobjectToCanvas("Background");

	C_Image* back = new C_Image(back_go);
	back->target = { 0.68f, 0.9f, 0.6f, 0.65f };
	back->offset = { -783.f, -735.f };
	back->section = { 0, 0, 783, 735 };
	back->tex_id = App->tex.Load("Assets/textures/back.png");

	//------------------------- TIME --------------------------------------

	Gameobject* time_go = AddGameobjectToCanvas("Time");

	C_Image* time = new C_Image(time_go);
	time->target = { 0.66f, 0.37f, 0.6f, 0.65f };
	time->offset = { -693.f, -100.f };
	time->section = { 0, 0, 693, 100 };
	if (win) time->tex_id = App->tex.Load("Assets/textures/wtime.png");
	else time->tex_id = App->tex.Load("Assets/textures/ltime.png");

	//------------------------- EDGE --------------------------------------

	Gameobject* edge_go = AddGameobjectToCanvas("edge");

	C_Image* edge = new C_Image(edge_go);
	edge->target = { 0.66f, 0.49f, 0.6f, 0.65f };
	edge->offset = { -693.f, -100.f };
	edge->section = { 0, 0, 693, 100 };
	if (win) edge->tex_id = App->tex.Load("Assets/textures/wedge.png");
	else edge->tex_id = App->tex.Load("Assets/textures/ledge.png");

	//------------------------- UNITS CREATED --------------------------------------

	Gameobject* units_c_go = AddGameobjectToCanvas("created");

	C_Image* units_c = new C_Image(units_c_go);
	units_c->target = { 0.66f, 0.61f, 0.6f, 0.65f };
	units_c->offset = { -693.f, -100.f };
	units_c->section = { 0, 0, 693, 100 };
	if (win) units_c->tex_id = App->tex.Load("Assets/textures/wunits_c.png");
	else units_c->tex_id = App->tex.Load("Assets/textures/lunits_c.png");

	//------------------------- UNITS LOST --------------------------------------

	Gameobject* units_l_go = AddGameobjectToCanvas("lost");

	C_Image* units_l = new C_Image(units_l_go);
	units_l->target = { 0.66f, 0.73f, 0.6f, 0.65f };
	units_l->offset = { -693.f, -100.f };
	units_l->section = { 0, 0, 693, 100 };
	if (win) units_l->tex_id = App->tex.Load("Assets/textures/wunits_l.png");
	else units_l->tex_id = App->tex.Load("Assets/textures/lunits_l.png");

	//------------------------- UNITS KILLED --------------------------------------

	Gameobject* units_k_go = AddGameobjectToCanvas("killed");

	C_Image* units_k = new C_Image(units_k_go);
	units_k->target = { 0.66f, 0.85f, 0.6f, 0.65f };
	units_k->offset = { -693.f, -100.f };
	units_k->section = { 0, 0, 693, 100 };
	if (win) units_k->tex_id = App->tex.Load("Assets/textures/wunits_k.png");
	else units_k->tex_id = App->tex.Load("Assets/textures/lunits_k.png");

	if (win) ret = App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-early-probe-eats-the-dust.ogg");
	else ret = App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-inter7ude.ogg");

	return ret;
}

void Scene::UpdateFade()
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

void Scene::UpdateHUD()
{
	//Current Melee Units Updated Value
	if (text_current_melee_units) {
		std::stringstream ss;
		ss << current_melee_units;
		std::string temp_str = ss.str();
		text_current_melee_units->text->SetText(temp_str.c_str());
	}

	//Melee Units Created Updated Value
	if (text_melee_units_created) {
		std::stringstream ss;
		ss << melee_units_created;
		std::string temp_str = ss.str();
		text_melee_units_created->text->SetText(temp_str.c_str());
	}

	//Current Gatherer Units Updated Value
	if (text_current_gatherer_units) {
		std::stringstream ss;
		ss << current_gatherer_units;
		std::string temp_str = ss.str();
		text_current_gatherer_units->text->SetText(temp_str.c_str());
	}

	//Gatherer Units Created Updated Value
	if (text_gatherer_units_created) {
		std::stringstream ss;
		ss << gatherer_units_created;
		std::string temp_str = ss.str();
		text_gatherer_units_created->text->SetText(temp_str.c_str());
	}


	//Current Ranged Units Updated Value
	if (text_current_ranged_units) {
		std::stringstream ss;
		ss << current_ranged_units;
		std::string temp_str = ss.str();
		text_current_ranged_units->text->SetText(temp_str.c_str());
	}

	//Ranged Units Created Updated Value
	if (text_ranged_units_created) {
		std::stringstream ss;
		ss << ranged_units_created;
		std::string temp_str = ss.str();
		text_ranged_units_created->text->SetText(temp_str.c_str());
	}

	//Mob Drop Print Updated Value
	if (text_mobdrop_value) {
		std::stringstream ss;
		ss << mob_drop;
		std::string temp_str = ss.str();
		text_mobdrop_value->text->SetText(temp_str.c_str());
	}

	//Edge Print Updated Value
	if (text_edge_value) {
		std::stringstream ss1;
		ss1 << edge_value;
		std::string temp_str1 = ss1.str();
		text_edge_value->text->SetText(temp_str1.c_str());
	}
}

void Scene::UpdateBuildingMode()
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		placing_building->GetGameobject()->Destroy();
		placing_building = nullptr;
	}
	else if (App->input->GetMouseButtonDown(0) == KEY_DOWN)
	{
		PlaceMode(placing_building->GetGameobject()->GetBehaviour()->GetType());
	}
	else
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		RectF cam = App->render->GetCameraRectF();
		std::pair<int, int> pos = Map::WorldToTileBase(float(x) + cam.x, float(y) + cam.y);
		placing_building->SetLocalPos(vec(float(pos.first), float(pos.second), 0.f));
	}
}

void Scene::UpdatePause()
{
	//Pause Game
	if ((test || level) && App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (pause)
		{
			pause_background_go->SetInactive();
			Event::Push(SCENE_PLAY, App);
			pause = false;
		}
		else
		{
			Event::Push(SCENE_PAUSE, App);
			PauseMenu();
			pause = true;
		}
	}
}

void Scene::UpdateSelection()
{
	//LOG("Canvas: %d", C_Canvas::MouseOnUI());
	//LOG("Editor: %d", App->editor->MouseOnWindow());
	if (!C_Canvas::MouseOnUI() && !App->editor->MouseOnEditor())
	{		
		if (App->input->GetMouseButtonDown(0))
		{
			//GROUP SELECTION//
			switch (App->input->GetMouseButtonDown(0))
			{
			case KEY_DOWN:
			{
				App->input->GetMousePosition(groupStart.x, groupStart.y);
				SetSelection(nullptr, true);
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

						if (posToWorld.first > groupStart.x&& posToWorld.first < mouseExtend.x) //Right
						{
							if (posToWorld.second > groupStart.y&& posToWorld.second < mouseExtend.y)//Up
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
							if (posToWorld.second > groupStart.y&& posToWorld.second < mouseExtend.y)//Up
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

			//UNIT SELECTION//
			if (!groupSelect)
			{
				int x, y;
				App->input->GetMousePosition(x, y);
				SDL_Rect cam = App->render->GetCameraRect();
				for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
				{
					if (it->second->GetType() == UNIT_MELEE || it->second->GetType() == GATHERER || it->second->GetType() == UNIT_RANGED
						|| it->second->GetType() == BASE_CENTER || it->second->GetType() == TOWER || it->second->GetType() == BARRACKS || it->second->GetType() == UNIT_SPECIAL
						|| it->second->GetType() == UNIT_SUPER)
					{
						std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
						vec pos = it->second->GetGameobject()->GetTransform()->GetGlobalPosition();

						if (int(pos.x) == position.first && int(pos.y) == position.second) //Right
						{
							SetSelection(it->second->GetGameobject(), true);
							//Event::Push(ON_SELECT, it->second->GetGameobject());
							break;
						}
					}
				}
			}
		}
		//SELECTION RIGHT CLICK//
		if (App->input->GetMouseButtonDown(2) == KEY_DOWN)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			RectF cam = App->render->GetCameraRectF();
			std::pair<float, float> mouseOnMap = Map::F_WorldToMap(float(x) + cam.x, float(y) + cam.y);
			if (App->pathfinding.ValidTile(int(mouseOnMap.first),int(mouseOnMap.second)))
			{
				std::pair<float, float> modPos;
				modPos.first = mouseOnMap.first;
				modPos.second = mouseOnMap.second;

				if (groupSelect && !group.empty())//Move group selected
				{
					bool incX = false;
					for (std::vector<Gameobject*>::iterator it = group.begin(); it != group.end(); ++it)
					{
						while (App->pathfinding.ValidTile(int(modPos.first), int(modPos.second)) == false)
						{
							if (incX)
							{
								modPos.first++;
								incX = false;
							}
							else
							{
								modPos.second++;
								incX = true;
							}
						}
						Event::Push(ON_RIGHT_CLICK, *it, vec(mouseOnMap.first, mouseOnMap.second, 0.5f), vec(modPos.first, modPos.second, 0.5f));
						if (incX)
						{
							modPos.first++;
							incX = false;
						}
						else
						{
							modPos.second++;
							incX = true;
						}
					}
				}
				else//Move one selected
				{
					if (selection)
					{
						Event::Push(ON_RIGHT_CLICK, selection, vec(mouseOnMap.first, mouseOnMap.second, 0.5f), vec(-1, -1, -1));
						groupSelect = false;
					}
					else groupSelect = false;
				}
			}
		}
	}
}

void Scene::UpdateSpawner()
{
	//////TEMPORAL/////
	if (activateSpawn && spawnCounter >= cooldownSpawn)
	{
		for (int a = 0; a < spawnPoints.size(); a++)
		{
			vec pos = spawnPoints[a];
			bool incX = false;
			for (int i = 0; i < 5; i++)
			{
				if (incX)
				{
					pos.x++;
					incX = false;
				}
				else
				{
					pos.y++;
					incX = true;
				}

				int random = std::rand() % 100 + 1;
				if (random < MELEE_RATE) //Spawn melee
				{
					SpawnMeleeIA(pos.x, pos.y);
				}
				else if (random < (MELEE_RATE + RANGED_RATE)) //Spawn ranged
				{
					SpawnRangedIA(pos.x, pos.y);
				}
				else if (random < (MELEE_RATE + RANGED_RATE + SUPER_RATE)) //Spawn super
				{

					SpawnSuperIA(pos.x, pos.y);
				}
				else //Spawn special
				{
					SpawnSpecialIA(pos.x, pos.y);
				}
				currentSpawns++;
				LOG("Spawned one");
			}
		}
		spawnCounter = 0;
		LOG("End ");
		//std::srand(time(NULL));
	}
	else
	{
		spawnCounter += App->time.GetGameDeltaTime();
	}
	///////////////////
}

void Scene::UpdateStateMachine()
{
	switch (current_state)
	{
	case LORE:

		break;
	case CAM_MOVEMENT:

		current_cam_pos = App->render->GetCameraCenter();
		distance = JMath::Distance(last_cam_pos, current_cam_pos);

		if (distance > last_distance) {
			total_distance += distance - last_distance;
		}
		else if (last_distance > distance) {
			total_distance += last_distance - distance;
		}
		last_distance = distance;
		if (total_distance >= 500.0f && r_c_comprobation) {
			r_c_comprobation = false;
			Event::Push(GAMEPLAY, this, R_CLICK_MOVEMENT);
		}
		LOG("camera dist %f ", total_distance);

		break;
	case R_CLICK_MOVEMENT:
		if(App->input->GetMouseButtonDown(2) == KEY_DOWN /*&& App->scene->selection == gather_go*/){
			
			tutorial_clicks++;
			if(tutorial_clicks == 10) Event::Push(GAMEPLAY, this, EDGE_STATE);
			
			LOG("Clicks %d", tutorial_clicks);

		}
		
		break;
	case EDGE_STATE:

		if (edge_t_go != nullptr && edge_t_go->GetBehaviour()->GetState() == DESTROYED) {

			edge_t_go = nullptr;
			Event::Push(GAMEPLAY, this, BASE_CENTER_STATE);
		}
		
		break;
	case BASE_CENTER_STATE:
		


		break;
	case RESOURCES:

		break;

	case BARRACKS_STATE:
		LOG("babb %d", tutorial_barrack);
		if (tutorial_barrack == 1) {
			
			Event::Push(GAMEPLAY, this, MELEE);
		}

		break;
	
	case MELEE:

		if (current_melee_units == 1) {

			Event::Push(GAMEPLAY, this, ENEMY);
		}

		break;
	case ENEMY:

		

		break;
	case MELEE_ATK:

		if (mobdrop != 0) Event::Push(GAMEPLAY, this, MOBDROP);

		break;
	case MOBDROP:


		break;
	case BUILD:
	
		if (tutorial_tower == 1) {

			Event::Push(GAMEPLAY, this, UPGRADE);
		}

		break;
	case UPGRADE:


		break;
	case TOWER_STATE:


		break;
	case TOWER_ATK:


		break;

	case WIN:

		if (units_killed >= 200) {
			Event::Push(GAMEPLAY, this, WIN);
		}

		break;
	case LOSE:

		Event::Push(GAMEPLAY, this, LOSE);

		break;

	default:
		break;

	
	}
}

void Scene::OnEventStateMachine(GameplayState state)
{
	switch (state)
	{
		//------------------STATE MACHINE CASES-----------------------
	case CAM_MOVEMENT:
		lore_go->SetInactive();
		LOG("CAM MOVEMENT STATE");
		cam_mov_go = AddGameobjectToCanvas("cam_mov");
		cam_mov = new C_Image(cam_mov_go);
		next = new C_Button(cam_mov_go, Event(SCENE_PLAY, this, R_CLICK_MOVEMENT));

		cam_mov->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		cam_mov->offset = { -640.f, -985.f };
		cam_mov->section = { 0, 0, 640, 985 };
		cam_mov->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		next->target = { 0.60f, 0.35f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		current_state = CAM_MOVEMENT;
		break;
	case R_CLICK_MOVEMENT:
	{
		cam_mov_go->SetInactive();
		//cam_mov_go->Destroy();
		LOG("R CLICK MOVEMENT STATE");
		R_click_mov_go = AddGameobjectToCanvas("R_click_mov");
		R_click_mov = new C_Image(R_click_mov_go);
		next = new C_Button(R_click_mov_go, Event(SCENE_PLAY, this, MAIN));

		R_click_mov->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		R_click_mov->offset = { -640.f, -985.f };
		R_click_mov->section = { 0, 0, 640, 985 };
		R_click_mov->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		next->target = { 0.70f, 0.45f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		std::pair<float, float> current_cam_pos_t = Map::F_WorldToMap(current_cam_pos.first, current_cam_pos.second);


		gather_go = AddGameobject("Tutorial Gatherer");
		gather_go->GetTransform()->SetLocalPos({ current_cam_pos_t.first, current_cam_pos_t.second,0.0f });

		//minimap->AddToMinimap(unit_go, { 0,255,0,255 });

		new Gatherer(gather_go);

		current_state = R_CLICK_MOVEMENT;
		break;
	}
	case EDGE_STATE:
	{
		R_click_mov_go->SetInactive();
		LOG("EDGE STATE");
		edge_go = AddGameobjectToCanvas("edge_go");
		edge = new C_Image(edge_go);
		
		edge->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		edge->offset = { -640.f, -985.f };
		edge->section = { 0, 0, 1070, 207 };
		edge->tex_id = App->tex.Load("Assets/textures/button.png");


		std::pair<float, float> current_cam_pos_t = Map::F_WorldToMap(current_cam_pos.first, current_cam_pos.second);

		
		edge_t_go = AddGameobject("Tutorial Gatherer");
		edge_t_go->GetTransform()->SetLocalPos({ current_cam_pos_t.first, current_cam_pos_t.second,0.0f });

		new Edge(edge_t_go);


		current_state = EDGE_STATE;
	}
		break;
	case BASE_CENTER_STATE:
	{
		edge_go->SetInactive();
		LOG("BASE CENTER STATE");

		base_center_go = AddGameobjectToCanvas("base_center_go");
		base_center = new C_Image(base_center_go);
		next = new C_Button(base_center_go, Event(GAMEPLAY, this, RESOURCES));

		base_center->target = { 0.50f, 0.85f, 0.6f, 0.6f };
		base_center->offset = { -640.f, -985.f };
		base_center->section = { 0, 0, 1070, 207 };
		base_center->tex_id = App->tex.Load("Assets/textures/button.png");
				
		next->target = { 0.70f, 0.45f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");


		current_state = BASE_CENTER_STATE;
	}
		break;
	case RESOURCES:
		base_center_go->SetInactive();
		LOG("RESOURCES STATE");
		resources_state_go = AddGameobjectToCanvas("resources_state_go");
		resources = new C_Image(resources_state_go);
		next = new C_Button(resources_state_go, Event(GAMEPLAY, this, BARRACKS_STATE));

		resources->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		resources->offset = { -640.f, -985.f };
		resources->section = { 0, 0, 640, 200 };
		resources->tex_id = App->tex.Load("Assets/textures/button.png");

		next->target = { 0.75f, 0.45f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		//Explain Edge and what it is used for
		current_state = RESOURCES;

		break;
	case BARRACKS_STATE:
		resources_state_go->SetInactive();
		LOG("BARRACKS STATE");
		barracks_state_go = AddGameobjectToCanvas("barracks_state_go");
		barracks_state = new C_Image(barracks_state_go);
		next = new C_Button(barracks_state_go, Event(SCENE_PLAY, this, MAIN));

		barracks_state->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		barracks_state->offset = { -640.f, -985.f };
		barracks_state->section = { 0, 0, 640, 300 };
		barracks_state->tex_id = App->tex.Load("Assets/textures/button.png");
	
		next->target = { 0.60f, 0.45f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		//BUILD BARRACKS -> NEXT STATE
		current_state = BARRACKS_STATE;

		break;
	
	case MELEE:
		barracks_state_go->SetInactive();
		LOG("MELEE STATE");
		melee_go = AddGameobjectToCanvas("melee_go");
		melee = new C_Image(melee_go);
		next = new C_Button(melee_go, Event(SCENE_PLAY, this, MAIN));

		melee->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		melee->offset = { -640.f, -985.f };
		melee->section = { 0, 0, 640, 985 };
		melee->tex_id = App->tex.Load("Assets/textures/button.png");

		next->target = { 0.80f, 0.35f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		//new MeleeUnit(melee_go);
		current_state = MELEE;
		//BUILD MELEE -> NEXT STATE

		break;
	case ENEMY:
		melee_go->SetInactive();
		LOG("ENEMY STATE");
		enemy_go = AddGameobjectToCanvas("enemy_go");
		enemy = new C_Image(enemy_go);
		new EnemyMeleeUnit(enemy_go);

		enemy->target = { 0.30f, 0.45f, 0.3f, 0.3f };
		enemy->offset = { -525.f, 200.f };
		enemy->section = { 0, 0, 640, 985 };
		enemy->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		next = new C_Button(melee_go, Event(GAMEPLAY, this, MELEE_ATK));
		next->target = { 0.30f, 0.45f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		break;
	case MELEE_ATK:
		enemy_go->SetInactive();
		LOG("MELEE ATK STATE");

		break;
	/*case ENEMY_ATK:
		enemy_atk_go = AddGameobjectToCanvas("enemy_atk_go");
		enemy_atk = new C_Image(enemy_atk_go);

		enemy_atk->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		enemy_atk->offset = { -640.f, -985.f };
		enemy_atk->section = { 0, 0, 640, 985 };
		enemy_atk->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		break;*/
	case MOBDROP:
		melee_atk_go->SetInactive();
		LOG("MOBDROP STATE");
		mobdrop_go = AddGameobjectToCanvas("mobdrop_go");
		mobdrop = new C_Image(mobdrop_go);

		mobdrop->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		mobdrop->offset = { -740.f, -285.f };
		mobdrop->section = { 0, 0, 640, 985 };
		mobdrop->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		next = new C_Button(melee_go, Event(SCENE_PLAY, this, MAIN));
		next->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		next->offset = { -740.f, -285.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		break;
	case BUILD:
		mobdrop_go->SetInactive();
		LOG("BUILD STATE");
		build_go = AddGameobjectToCanvas("build_go");
		build = new C_Image(build_go);

		build->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		build->offset = { -640.f, -985.f };
		build->section = { 0, 0, 640, 985 };
		build->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		next = new C_Button(barracks_state_go, Event(SCENE_PLAY, this, MAIN));

		next->target = { 0.60f, 0.45f, 0.3f, 0.3f };
		next->offset = { -525.f, 200.f };
		next->section = { 0, 0, 1070, 207 };
		next->tex_id = App->tex.Load("Assets/textures/button.png");

		new Tower(build_go);

		break;
	case UPGRADE:

		LOG("UPGRADE STATE");
		upgrade_go = AddGameobjectToCanvas("upgrade_go");
		upgrade = new C_Image(upgrade_go);

		upgrade->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		upgrade->offset = { -640.f, -985.f };
		upgrade->section = { 0, 0, 640, 985 };
		upgrade->tex_id = App->tex.Load("Assets/textures/pause-bg.png");


		break;
	case TOWER_STATE:

		LOG("TOWER STATE");
		tower_state_go = AddGameobjectToCanvas("tower_state_go");
		tower_state = new C_Image(tower_state_go);

		tower_state->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		tower_state->offset = { -640.f, -985.f };
		tower_state->section = { 0, 0, 640, 985 };
		tower_state->tex_id = App->tex.Load("Assets/textures/pause-bg.png");


		break;
	/*case TOWER_ATK:

		LOG("TOWER ATK STATE");
		tower_atk_go = AddGameobjectToCanvas("tower_atk_go");
		tower_atk = new C_Image(tower_atk_go);

		tower_atk->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		tower_atk->offset = { -640.f, -985.f };
		tower_atk->section = { 0, 0, 640, 985 };
		tower_atk->tex_id = App->tex.Load("Assets/textures/pause-bg.png");


		break;*/

	case WIN:
		//Kill 200 units

		win = true;
		LoadEndScene();

		break;
	case LOSE:
		//Base center destroyed

		win = false;
		LoadEndScene();

		break;
	default:
		break;
	}
}

void Scene::PauseMenu()
{
	if (pause_background_go == nullptr)
	{
		//------------------------- BACKGROUND -----------------------------------

		pause_background_go = AddGameobjectToCanvas("Background");

		C_Image* background = new C_Image(pause_background_go);
		background->target = { 0.66f, 0.95f, 0.6f, 0.6f };
		background->offset = { -640.f, -985.f };
		background->section = { 0, 0, 640, 985 };
		background->tex_id = App->tex.Load("Assets/textures/pause-bg.png");

		//------------------------- RESUME -----------------------------------------

		Gameobject* resume_go = AddGameobject("resume Button", pause_background_go);

		C_Button* resume = new C_Button(resume_go, Event(SCENE_PLAY, this, App));
		resume->target = { 0.51f, 0.3f, 0.3f, 0.3f };
		resume->offset = { -525.f, -100.f };
		resume->section = { 0, 0, 1070, 207 };
		resume->tex_id = App->tex.Load("Assets/textures/button.png");

		C_Button* resume_fx = new C_Button(resume_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
		resume_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
		resume_fx->offset = { -525.f, -100.f };
		resume_fx->section = { 0, 0, 1070, 207 };

		/*//------------------------- SAVE --------------------------------------

		Gameobject* save_go = AddGameobject("save button", pause_background_go);

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

		Gameobject* load_go = AddGameobject("load Button", pause_background_go);

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

		Gameobject* options_go = AddGameobject("options Button", pause_background_go);

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

		Gameobject* main_menu_go = AddGameobject("main menu Button", pause_background_go);

		C_Button* main_menu = new C_Button(main_menu_go, Event(SCENE_CHANGE, this, MENU));
		main_menu->target = { 0.51f, 0.3f, 0.3f, 0.3f };
		main_menu->offset = { -525.f, 200.f };
		main_menu->section = { 0, 0, 1070, 207 };
		main_menu->tex_id = App->tex.Load("Assets/textures/button.png");

		C_Button* main_menu_fx = new C_Button(main_menu_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
		main_menu_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
		main_menu_fx->offset = { -525.f, 200.f };
		main_menu_fx->section = { 0, 0, 1070, 207 };

		pause_background_go->SetInactive();
	}
	if (pause_background_go->IsActive()) pause_background_go->SetInactive();
	else pause_background_go->SetActive();
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
	SetSelection(nullptr, false);
	root.RemoveChilds();
	Event::PumpAll();
	root.UpdateRemoveQueue();

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
	case BASE_CENTER: new Base_Center(go = AddGameobject("Base Center")); 
		baseCenterPos.first = go->GetTransform()->GetGlobalPosition().x;
		baseCenterPos.second = go->GetTransform()->GetGlobalPosition().y;
		break;
	case TOWER: new Tower(go = AddGameobject("Tower")); break;
	case WALL: break;
	case BARRACKS: new Barracks(go = AddGameobject("Barracks")); break; 
	case LAB: break;
	case EDGE: new Edge(go = AddGameobject("Edge")); break;
	default: break;
	}

	if (go)
	{
		placing_building = go->GetTransform();
		for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)//Update paths 
		{
			Event::Push(UPDATE_PATH, it->second,baseCenterPos.first - 1,baseCenterPos.second - 1);
		}		
	}
}

void Scene::SpawnMeleeIA(float x, float y)
{
	Gameobject* unit_go = AddGameobject("Enemy Melee unit");
	unit_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(unit_go);
}

void Scene::SpawnRangedIA(float x, float y)
{
	Gameobject* unit_go = AddGameobject("Enemy Melee unit");
	unit_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(unit_go);
}
void Scene::SpawnSuperIA(float x, float y)
{
	Gameobject* unit_go = AddGameobject("Enemy Melee unit");
	unit_go->GetTransform()->SetLocalPos({ x,y, 0.0f });

	new EnemyMeleeUnit(unit_go);
}

void Scene::SpawnSpecialIA(float x, float y)
{
	Gameobject* unit_go = AddGameobject("Enemy Melee unit");
	unit_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(unit_go);
}

bool Scene::DamageAllowed()
{
	return god_mode && no_damage;
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

Gameobject* Scene::AddGameobjectToCanvas(const char* name)
{
	Gameobject* canvas_go = C_Canvas::GameObject();

	if (!canvas_go)
	{
		canvas_go = new Gameobject("Canvas", &root);
		new C_Canvas(canvas_go);
	}

	return new Gameobject(name, canvas_go);
}

void Scene::SetSelection(Gameobject* go, bool call_unselect)
{
	if (App->scene->group.empty() == false)
	{
		for (std::vector<Gameobject*>::iterator it = App->scene->group.begin(); it != App->scene->group.end(); ++it)
		{
			Event::Push(ON_UNSELECT, *it);
		}
		group.clear();
		groupSelect = false;
	}

	if (go != nullptr)
	{
		if (selection != nullptr)
		{
			if (selection != go)
			{
				if (call_unselect)
					Event::Push(ON_UNSELECT, selection);

				Event::Push(ON_SELECT, go);
			}
		}
		else
			Event::Push(ON_SELECT, go);
	}
	else if (selection != nullptr && call_unselect)
		Event::Push(ON_UNSELECT, selection);

	selection = go;
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


	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->editor->ToggleEditorVisibility();

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		map.draw_walkability = !map.draw_walkability;

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		App->audio->PauseMusic(1.f);

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		no_damage = !no_damage;

	// Swap map orientation
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) Map::SwapMapType();

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)//Barracks
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		Gameobject* barrack_go = AddGameobject("Barracks building");
		barrack_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		new Barracks(barrack_go);
	}

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

		//minimap->AddToMinimap(unit_go, { 0,255,0,255 });

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
			for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)//Update paths 
			{
				Event::Push(UPDATE_PATH, it->second, baseCenterPos.first - 1, baseCenterPos.second - 1);
			}
		}
		else
			LOG("Invalid spawn position");
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)// Enemy melee
	{
		std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));

		/*Gameobject* unit_go = AddGameobject("Enemy Melee unit");
		unit_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

		new EnemyMeleeUnit(unit_go);*/
		SpawnMeleeIA(float(position.first), float(position.second));
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
			baseCenterPos.first = base_go->GetTransform()->GetGlobalPosition().x;
			baseCenterPos.second = base_go->GetTransform()->GetGlobalPosition().y;
			for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)//Update paths 
			{
				Event::Push(UPDATE_PATH, it->second, baseCenterPos.first - 1, baseCenterPos.second - 1);
			}
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
			for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)//Update paths 
			{
				Event::Push(UPDATE_PATH, it->second, baseCenterPos.first - 1, baseCenterPos.second - 1);
			}
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
				//minimap->AddToMinimap(gather_go, { 0,0,255,255 });

				new Gatherer(gather_go);
				edge_value -= 10;
				LOG("Current resources: %d", edge_value);
			}
			else
				LOG("Invalid spawn position");
		}
		
	}

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) //Spawner
	{
		if (activateSpawn) activateSpawn = false;
		else activateSpawn = true;
		/*std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
		if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
		{
			Gameobject* spawner_go = AddGameobject("Enemy spawner");
			spawner_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });

			new Spawner(spawner_go);
		}
		else
			LOG("Invalid spawn position");*/
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

		if (selection)
			selection->Destroy();
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

	// Log onto window title
	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Zoom: %0.2f, Mouse: %dx%d, Tile: %dx%d, Selection: %s",
		App->time.GetLastFPS(),
		App->render->GetZoom(),
		x, y,
		map_coordinates.first, map_coordinates.second,
		selection != nullptr ? selection->GetName() : "none selected");

	App->win->SetTitle(tmp_str);
}
