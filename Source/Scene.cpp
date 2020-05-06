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
#include "RangedUnit.h"
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
bool Scene::no_damage = true;
bool Scene::draw_collisions = false;
int Scene::player_stats[MAX_PLAYER_STATS];

Scene::Scene() : Module("scene")
{
	root.SetName("root");
}

Scene::~Scene()
{}

bool Scene::Start()
{
	ResetScene();
	return true;
}

bool Scene::PreUpdate()
{
	root.PreUpdate();

	//Fog of war
	//if (fogLoaded) App->fogWar.Update();
	/*if (Behaviour::enemiesInSight.empty() != false)
	{
		cacheEnemies = Behaviour::enemiesInSight;
		Behaviour::enemiesInSight.clear();
		//LOG("Not empty");
		for (std::vector<double>::const_iterator it = cacheEnemies.cbegin(); it != cacheEnemies.cend(); ++it)
		{
			Behaviour* go = Behaviour::b_map[*it]->GetGameobject()->GetBehaviour();
			if (go->GetState() != DESTROYED) { Event::Push(SHOW_SPRITE, go); LOG("Sent event"); }
		}
	}*/
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
	else if (placing_building)
	{
		UpdateBuildingMode();
	}
	else
	{
		UpdatePause();

		if (!paused_scene && !C_Canvas::MouseOnUI() && !App->editor->MouseOnEditor())
			UpdateSelection();
	}

	return true;
}

bool Scene::PostUpdate()
{
	root.PostUpdate();
	map.Draw();
	// Timed Scene Changing
	if (scene_change_timer >= 0.f)
	{
		scene_change_timer -= App->time.GetGameDeltaTime();
		if (scene_change_timer <= 0.f)
		{
			scene_change_timer = -1.0f;
			Event::Push(SCENE_CHANGE, this, next_scene, 2.f);
		}
	}
	//Fog of war
	if(fogLoaded) App->fogWar.DrawFoWMap();
	/*for (std::vector<double>::const_iterator it = cacheEnemies.cbegin(); it != cacheEnemies.cend(); ++it)
	{
		//Behaviour* go = Behaviour::b_map[*it]->GetGameobject()->GetBehaviour();
		//if (go->GetState() != DESTROYED) Event::Push(HIDE_SPRITE, go);
		cacheEnemies.clear();
	}*/
	return true;
}

bool Scene::CleanUp()
{
	ResetScene();
	
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
		if (fading != FADE_OUT)
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
	case PLACE_BUILDING:
	{
		imgPreview->SetActive();
		placing_building = true;
		buildType = e.data1.AsInt();
		switch (buildType)
		{
		case TOWER:
			buildingImage->SetSection({ 0, 3, 217, 177 });
			imgPreview->GetTransform()->SetScale({ 0.9f,0.9f,0.9f });
			//LOG("Tower");
			break;
		case BARRACKS:
			buildingImage->SetSection({ 217, 3, 217, 177 });
			imgPreview->GetTransform()->SetScale({ 1.75f,1.75f,1.75f });
			//LOG("Barracks");
			break;
		case BASE_CENTER: 
			buildingImage->SetSection({ 434, 3, 217, 177 });
			imgPreview->GetTransform()->SetScale({ 1.9f,1.9f,1.9f });
			//LOG("Base center");
			break;
		default:
			buildingImage->SetSection({ 0, 3, 217, 177 });
			imgPreview->GetTransform()->SetScale({ 0.9f,0.9f,0.9f });

			//LOG("Default");
			break;
		}
		
		break;
	}
	case UPDATE_STAT:
	{
		UpdateStat(e.data1.AsInt(), e.data2.AsInt());
		break;
	}
	case GAMEPLAY:
	{
		OnEventStateMachine(GameplayState(e.data1.AsInt()));
		break;
	}
	case SPAWN_UNIT:
	{
		SpawnBehaviour(e.data1.AsInt(), e.data2.AsVec());
		break;
	}
	case SET_INACTIVE:
	{
		not_go->SetInactive();
		break;
	}
	case RESUME:
	{
		pause_background_go->SetInactive();
		paused_scene = false;
		break;
	}
	case BUTTON_EVENT:
	{
		switch (e.data1.AsInt())
		{
		case::SCENE_CHANGE:
		{
			Event::Push(SCENE_CHANGE, this, e.data2.AsInt(), 2.0f);
			break;
		}
		case::TOGGLE_FULLSCREEN:
		{
			Event::Push(TOGGLE_FULLSCREEN, App->win);
			break;
		}
		case::RESUME:
		{
			Event::Push(SCENE_PLAY, App);
			pause_background_go->SetInactive();
			paused_scene = false;
			break;
		}
		case::REQUEST_QUIT:
		{
			Event::Push(REQUEST_QUIT, App);
			break;
		}
		case::REQUEST_SAVE:
		{

			break;
		}
		case::REQUEST_LOAD:
		{

			break;
		}
		}
		Event::Push(PLAY_FX, App->audio, int(SELECT), 0);
		break;
	}
	/*case NEW_BEHAVIOUR:

		switch (e.type)
		{
		case UNIT_MELEE:
			App->scene->current_melee_units += 1;
			App->scene->melee_units_created += 1;
			break;
		case UNIT_RANGED:
			App->scene->current_ranged_units += 1;
			App->scene->ranged_units_created += 1;
			break;
		case GATHERER:
			App->scene->current_gatherer_units += 1;
			App->scene->gatherer_units_created += 1;
			break;
		case BARRACKS:
			App->scene->tutorial_barrack += 1;
			break;
		case TOWER:
			App->scene->tutorial_tower += 1;
			break;
		}
		break;*/
	default:
		break;
	}
}

void Scene::LoadTestScene()
{
	LoadMainScene();
	/*OPTICK_EVENT();

	map.Load("Assets/maps/iso.tmx");

	LoadMainHUD();

	god_mode = true;
	App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-buzzkiller.ogg");
	id_mouse_tex = App->tex.Load("Assets/textures/meta.png");

	// Build mode
	Gameobject* builder = AddGameobjectToCanvas("Building Mode");
	C_Button* base = new C_Button(builder, Event(PLACE_BUILDING, this, int(BASE_CENTER)));
	C_Button* tower = new C_Button(builder, Event(PLACE_BUILDING, this, int(TOWER)));
	C_Button* edge = new C_Button(builder, Event(PLACE_BUILDING, this, int(EDGE)));
	base->target = { 0.5f, 1.f, 1.f , 1.f };
	tower->target = { 0.6f, 1.f, 1.f , 1.f };
	edge->target = { 0.7f, 1.f, 1.f , 1.f };
	base->offset = tower->offset = edge->offset ={ -40.f, -80.f };
	for (int i = 0; i < 4; i++)base->section[i] = tower->section[i] = edge->section[i] = { 0, 0, 80, 80 };*/
}

void Scene::LoadMainScene()
{
	OPTICK_EVENT();

	map.Load("Assets/maps/iso.tmx");

	LoadMainHUD();

	fogLoaded = App->fogWar.Init();

	Event::Push(MINIMAP_MOVE_CAMERA, App->render, float(800), float(2900));

	//Minimap
	new Minimap(AddGameobjectToCanvas("Minimap"));
	//Event::Push(ON_PAUSE, &root);
	not_go = AddGameobjectToCanvas("lore");
	not = new C_Image(not_go);
	next = new C_Button(not_go, Event(GAMEPLAY, this, CAM_MOVEMENT));

	not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
	not->offset = { -183.f, -1044.f };
	not->section = { 0, 0, 983, 644 };
	not->tex_id = App->tex.Load("Assets/textures/tuto/lore-not.png");

	next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
	next->offset = { 500.f, -317.f };

	next->section[0] = { 0, 0, 309, 37 };
	next->section[1] = { 0, 44, 309, 37 };
	next->section[2] = { 0, 88, 309, 37 };
	next->section[3] = { 0, 88, 309, 37 };

	next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

	skip = new C_Button(not_go, Event(GAMEPLAY, this, SPAWNER_STATE));

	skip->target = { 0.74f, 0.726f, 0.4f, 0.4f };
	skip->offset = { 100.f, -317.f };

	skip->section[0] = { 0, 0, 309, 37 };
	skip->section[1] = { 0, 44, 309, 37 };
	skip->section[2] = { 0, 88, 309, 37 };
	skip->section[3] = { 0, 88, 309, 37 };

	skip->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

	std::pair<int, int> position = Map::WorldToTileBase(float(1400.0f), float(3250.0f));
	if (App->pathfinding.CheckWalkabilityArea(position, vec(1.0f)))
	{
		Gameobject* base_go = AddGameobject("Base Center");
		base_go->GetTransform()->SetLocalPos({ float(position.first), float(position.second), 0.0f });
		base_go->GetTransform()->ScaleX(4.0f);
		base_go->GetTransform()->ScaleY(4.0f);
		//App->audio->PlayFx(B_BUILDED);
		new Base_Center(base_go);
		std::pair<int, int> baseCenterPos = {
			base_go->GetTransform()->GetGlobalPosition().x,
			baseCenterPos.second = base_go->GetTransform()->GetGlobalPosition().y};
		for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)//Update paths 
		{
			Event::Push(UPDATE_PATH, it->second, baseCenterPos.first - 1, baseCenterPos.second - 1);
		}
	}

	imgPreview = AddGameobject("Builder image");
	buildingImage = new Sprite(imgPreview, App->tex.Load("Assets/textures/buildPreview.png"), { 0, 3, 217, 177 }, FRONT_SCENE, {-60.0f,-100.0f,1.0f,1.0f});
	imgPreview->SetInactive();

	SpawnBehaviour(EDGE, edge_pos1);
	SpawnBehaviour(EDGE, edge_pos2);
	SpawnBehaviour(EDGE, edge_pos3);
	SpawnBehaviour(EDGE, edge_pos4);
	SpawnBehaviour(EDGE, edge_pos5);
	SpawnBehaviour(EDGE, edge_pos6);
	SpawnBehaviour(EDGE, edge_pos7);
	SpawnBehaviour(EDGE, edge_pos8);
	SpawnBehaviour(EDGE, edge_pos9);
}

void Scene::LoadIntroScene()
{
	OPTICK_EVENT();
	
	App->audio->PlayFx(LOGO);

	scene_change_timer = 10.f;
	next_scene = MENU;

	C_Button* background = new C_Button(AddGameobjectToCanvas("Background"), Event(SCENE_CHANGE, this, MENU, 2.f));
	for (int i = 0; i < 4; i++)background->section[i] = { 0, 0, 1920, 1080 };
	background->color = { 255, 255, 255, 255 };

	C_Image* logo = new C_Image(AddGameobjectToCanvas("Team logo"));
	logo->target = { 0.5f, 0.5f, 0.5f, 0.5f };
	logo->offset = { -300.f, -400.f };
	logo->section = { 0, 0, 499, 590 };
	logo->tex_id = App->tex.Load("Assets/textures/team-logo2.png");
}

void Scene::LoadMenuScene()
{
	OPTICK_EVENT();

	App->audio->PlayMusic("Assets/audio/Music/alexander-nakarada-curiosity.ogg") && App->audio->PlayFx(TITLE);

	//------------------------- BACKGROUND --------------------------------------

	C_Image* background = new C_Image(AddGameobjectToCanvas("Background"));
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1280.f, -720.f };
	background->section = { 0, 0, 1280, 720 };
	background->tex_id = App->tex.Load("Assets/textures/background.png");

	//------------------------- LOGO --------------------------------------

	C_Image* g_logo = new C_Image(AddGameobjectToCanvas("Game logo"));
	g_logo->target = { 0.6f, 0.6f, 0.6f, 0.6f };
	g_logo->offset = { -1255.f, -500.f };
	g_logo->section = { 0, 0, 1070, 207 };
	g_logo->tex_id = App->tex.Load("Assets/textures/game-logo.png");
	
	//------------------------- START --------------------------------------

	Gameobject* start_go = AddGameobjectToCanvas("Start Button");
	
	C_Button* start = new C_Button(start_go, Event(BUTTON_EVENT, this, SCENE_CHANGE, MAIN));
	start->target = { 0.f, 0.67f, .67f, .67f };
	start->offset = { 25.f, -253.f };

	start->section[0] = { 0, 0, 470, 90 };
	start->section[1] = { 0, 101, 470, 90 };
	start->section[2] = { 0, 202, 470, 90 };
	start->section[3] = { 0, 202, 470, 90 };

	start->tex_id = App->tex.Load("Assets/textures/new-game.png");

	//------------------------- FULLSCREEN --------------------------------------

	Gameobject* fullscreen_go = AddGameobjectToCanvas("Fullscreen Button");

	C_Button* fullscreen = new C_Button(fullscreen_go, Event(BUTTON_EVENT, this, TOGGLE_FULLSCREEN));
	fullscreen->target = { 0.f, 0.65f, .55f, .55f };
	fullscreen->offset = { 30.f, -157.f };

	fullscreen->section[0] = { 0, 0, 470, 90 };
	fullscreen->section[1] = { 0, 101, 470, 90 };
	fullscreen->section[2] = { 0, 202, 470, 90 };
	fullscreen->section[3] = { 0, 202, 470, 90 };

	fullscreen->tex_id = App->tex.Load("Assets/textures/fullscreen.png");

	//------------------------- QUIT --------------------------------------

	Gameobject* quit_go = AddGameobjectToCanvas("Quit Button");

	C_Button* quit = new C_Button(quit_go, Event(BUTTON_EVENT, this, REQUEST_QUIT));
	quit->target = { 0.f, 0.65f, .55f, .55f };
	quit->offset = { 30.f, -55.f };

	quit->section[0] = { 0, 0, 470, 90 };
	quit->section[1] = { 0, 101, 470, 90 };
	quit->section[2] = { 0, 202, 470, 90 };
	quit->section[3] = { 0, 202, 470, 90 };

	quit->tex_id = App->tex.Load("Assets/textures/quit.png");
}

void Scene::LoadEndScene()
{
	OPTICK_EVENT();

	App->audio->PlayMusic(win ?
		"Assets/audio/Music/alexander-nakarada-early-probe-eats-the-dust.ogg" :
		"Assets/audio/Music/alexander-nakarada-inter7ude.ogg");

	//------------------------- BACKGROUND --------------------------------------

	Gameobject* background_go = AddGameobjectToCanvas("Background");

	C_Image* background = new C_Image(background_go);
	background->target = { 1.f, 1.f, 1.f, 1.f };
	background->offset = { -1280.f, -720.f };
	background->section = { 0, 0, 1280, 720 };
	background->tex_id = App->tex.Load(win ? "Assets/textures/back-win.png" : "Assets/textures/back-lose.png");

	C_Button* background_btn = new C_Button(background_go, Event(BUTTON_EVENT, this, SCENE_CHANGE, MENU));
	background_btn->target = { 1.f, 1.f, 1.f, 1.f };
	background_btn->offset = { -1920.f, -1080.f };
	for (int i = 0; i < 4; i++)background_btn->section[i] = { 0, 0, 1920, 1080 };

	//------------------------- WIN/LOSE --------------------------------------
	if (win)
	{
		C_Image* win = new C_Image(AddGameobjectToCanvas("Background"));
		win->target = { 0.66f, 0.25f, 0.8f, 0.8f };
		win->offset = { -605.f, -100.f };
		win->section = { 0, 0, 693, 100 };
		win->tex_id = App->tex.Load("Assets/textures/youwin.png");
	}
	else
	{
		C_Image* lose = new C_Image(AddGameobjectToCanvas("Background"));
		lose->target = { 0.66f, 0.25f, 0.8f, 0.8f };
		lose->offset = { -605.f, -100.f };
		lose->section = { 0, 0, 693, 100 };
		lose->tex_id = App->tex.Load("Assets/textures/youlose.png");
	}

	/*//------------------------- BACK --------------------------------------

	C_Image* back = new C_Image(AddGameobjectToCanvas("Background"));
	back->target = { 0.68f, 0.9f, 0.6f, 0.65f };
	back->offset = { -783.f, -735.f };
	back->section = { 0, 0, 783, 735 };
	back->tex_id = App->tex.Load("Assets/textures/back.png");*/

	//------------------------- TIME --------------------------------------

	C_Image* time = new C_Image(AddGameobjectToCanvas("Time"));
	time->target = { 0.66f, 0.37f, 0.6f, 0.65f };
	time->offset = { -693.f, -100.f };
	time->section = { 0, 0, 693, 100 };
	time->tex_id = App->tex.Load(win ? "Assets/textures/wtime.png" : "Assets/textures/ltime.png");

	//------------------------- EDGE --------------------------------------

	C_Image* edge = new C_Image(AddGameobjectToCanvas("edge"));
	edge->target = { 0.66f, 0.49f, 0.6f, 0.65f };
	edge->offset = { -693.f, -100.f };
	edge->section = { 0, 0, 693, 100 };
	edge->tex_id = App->tex.Load(win ? "Assets/textures/wedge.png" : "Assets/textures/ledge.png");

	//------------------------- UNITS CREATED --------------------------------------

	C_Image* units_c = new C_Image(AddGameobjectToCanvas("created"));
	units_c->target = { 0.66f, 0.61f, 0.6f, 0.65f };
	units_c->offset = { -693.f, -100.f };
	units_c->section = { 0, 0, 693, 100 };
	units_c->tex_id = App->tex.Load(win ? "Assets/textures/wunits_c.png" : "Assets/textures/lunits_c.png");

	//------------------------- UNITS LOST --------------------------------------

	C_Image* units_l = new C_Image(AddGameobjectToCanvas("lost"));
	units_l->target = { 0.66f, 0.73f, 0.6f, 0.65f };
	units_l->offset = { -693.f, -100.f };
	units_l->section = { 0, 0, 693, 100 };
	units_l->tex_id = App->tex.Load(win ? "Assets/textures/wunits_l.png" : "Assets/textures/lunits_l.png");

	//------------------------- UNITS KILLED --------------------------------------

	C_Image* units_k = new C_Image(AddGameobjectToCanvas("killed"));
	units_k->target = { 0.66f, 0.85f, 0.6f, 0.65f };
	units_k->offset = { -693.f, -100.f };
	units_k->section = { 0, 0, 693, 100 };
	units_k->tex_id = App->tex.Load(win ? "Assets/textures/wunits_k.png" : "Assets/textures/lunits_k.png");
}

void Scene::LoadMainHUD()
{
	int icons_text_id = App->tex.Load("Assets/textures/Iconos_square_up.png");

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

	hud_texts[CURRENT_MELEE_UNITS] = new C_Text(melee_counter_go, "0");
	hud_texts[CURRENT_MELEE_UNITS]->target = { 0.049f, 0.587f, 1.6f, 1.6f };

	C_Text* melee_diagonal = new C_Text(melee_counter_go, "/");
	melee_diagonal->target = { 0.088f, 0.587f, 1.6f, 1.6f };

	hud_texts[TOTAL_MELEE_UNITS] = new C_Text(melee_counter_go, "0");
	hud_texts[TOTAL_MELEE_UNITS]->target = { 0.099f, 0.587f, 1.6f, 1.6f };

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

	hud_texts[CURRENT_GATHERER_UNITS] = new C_Text(gatherer_counter_go, "0");
	hud_texts[CURRENT_GATHERER_UNITS]->target = { 0.049f, 0.667f, 1.6f, 1.6f };

	C_Text* gatherer_diagonal = new C_Text(gatherer_counter_go, "/");
	gatherer_diagonal->target = { 0.088f, 0.667f, 1.6f, 1.6f };

	hud_texts[TOTAL_GATHERER_UNITS] = new C_Text(gatherer_counter_go, "0");
	hud_texts[TOTAL_GATHERER_UNITS]->target = { 0.099f, 0.667f, 1.6f, 1.6f };

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

	hud_texts[CURRENT_RANGED_UNITS] = new C_Text(ranged_counter_go, "0");
	hud_texts[CURRENT_RANGED_UNITS]->target = { 0.049f, 0.747f, 1.6f, 1.6f };

	C_Text* ranged_diagonal = new C_Text(ranged_counter_go, "/");
	ranged_diagonal->target = { 0.088f, 0.747f, 1.6f, 1.6f };

	hud_texts[TOTAL_RANGED_UNITS] = new C_Text(ranged_counter_go, "0");
	hud_texts[TOTAL_RANGED_UNITS]->target = { 0.099f, 0.747f, 1.6f, 1.6f };

	//Resources
	Gameobject* resource_counter_go = AddGameobjectToCanvas("Resources");
	C_Image* img = new C_Image(resource_counter_go);
	img->target = { 0.1f, 1.f, 1.f , 1.f };
	img->offset = { -119.f, -119.f };
	img->section = { 22, 333, 119, 119 };
	img->tex_id = icons_text_id;

	//Edge
	Gameobject* resources_go = AddGameobject("Text Edge", resource_counter_go);
	C_Text* text_edge = new C_Text(resources_go, "Edge");
	text_edge->target = { 0.1f, 0.1f, 1.f, 1.f };

	Gameobject* resources_value_go = AddGameobject("Mob Drop Value", resource_counter_go);
	hud_texts[CURRENT_EDGE] = new C_Text(resources_go, "0");
	hud_texts[CURRENT_EDGE]->target = { 0.1f, 0.4f, 1.f, 1.f };

	//MobDrop
	Gameobject* resources_2_go = AddGameobject("Text Mob Drop", resource_counter_go);
	C_Text* text_mobdrop = new C_Text(resources_2_go, "Mob Drop");
	text_mobdrop->target = { 0.45f, 0.8f, 1.f, 1.f };

	Gameobject* resources_value_2_go = AddGameobject("Mob Drop Value", resource_counter_go);
	hud_texts[CURRENT_MOB_DROP] = new C_Text(resources_2_go, "0");
	hud_texts[CURRENT_MOB_DROP]->target = { 0.65f, 0.4f, 1.f, 1.f };

	//Minimap
	new Minimap(AddGameobjectToCanvas("Minimap"));
}

void Scene::UpdateFade()
{
	float alpha;

	if (just_triggered_change)
		just_triggered_change = false;
	else
		fade_timer += App->time.GetDeltaTime();

	if (fading == FADE_OUT)
	{
		alpha = fade_timer / fade_duration * 255.f;

		if (fade_timer >= fade_duration)
		{
			ChangeToScene(next_scene);
			Event::Push(SCENE_PLAY, App);
			fading = FADE_IN;
			just_triggered_change = true;
			fade_timer = 0.0f;
		}
	}
	else if (fading == FADE_IN)
	{
		alpha = JMath::Cap((fade_duration - fade_timer) / fade_duration * 255.f, 1.f, 254.f);

		if (fade_timer >= fade_duration)
			fading = NO_FADE;
	}

	if (fading != NO_FADE)
		App->render->DrawQuadNormCoords({ 0.f, 0.f, 1.f, 1.f }, { 0, 0, 0, unsigned char(alpha) }, true, FADE);
}

void Scene::UpdateStat(int stat, int count)
{
	player_stats[stat] += count;

	if (stat < EDGE_COLLECTED && hud_texts[stat])
	{
		std::stringstream ss;
		ss << player_stats[stat];
		hud_texts[stat]->text->SetText(ss.str().c_str());
	}
}

void Scene::UpdateBuildingMode()
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		placing_building = false;
		buildType = -1;
		imgPreview->SetInactive();
	}
	else if (App->input->GetMouseButtonDown(0) == KEY_DOWN)
	{
		if (buildType != -1)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			RectF cam = App->render->GetCameraRectF();
			std::pair<int, int> pos = Map::WorldToTileBase(float(x) + cam.x, float(y) + cam.y);
			Transform* t = SpawnBehaviour(buildType, vec(pos.first, pos.second));
		}
	}
	else
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		RectF cam = App->render->GetCameraRectF();
		std::pair<int, int> pos = Map::WorldToTileBase(float(x) + cam.x, float(y) + cam.y);
		imgPreview->GetTransform()->SetLocalPos(vec(float(pos.first), float(pos.second), 0.f));
	}
}

void Scene::UpdatePause()
{
	//Pause Game
	if (OnMainScene() && App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (!pause_background_go)
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

			C_Button* resume = new C_Button(resume_go, Event(BUTTON_EVENT, this, RESUME));
			resume->target = { 0.51f, 0.3f, 0.6f, 0.6f };
			resume->offset = { -250.f, -80.f };

			resume->section[0] = { 0, 0, 470, 90 };
			resume->section[1] = { 0, 101, 470, 90 };
			resume->section[2] = { 0, 202, 470, 90 };
			resume->section[3] = { 0, 202, 470, 90 };

			resume->tex_id = App->tex.Load("Assets/textures/resume.png");

			//------------------------- FULLSCREEN -----------------------------------------

			Gameobject* fullscreen_go = AddGameobject("resume Button", pause_background_go);

			C_Button* fullscreen = new C_Button(fullscreen_go, Event(BUTTON_EVENT, this, TOGGLE_FULLSCREEN));
			fullscreen->target = { 0.51f, 0.3f, 0.6f, 0.6f };
			fullscreen->offset = { -250.f, 70.f };

			fullscreen->section[0] = { 0, 0, 470, 90 };
			fullscreen->section[1] = { 0, 101, 470, 90 };
			fullscreen->section[2] = { 0, 202, 470, 90 };
			fullscreen->section[3] = { 0, 202, 470, 90 };

			fullscreen->tex_id = App->tex.Load("Assets/textures/fullscreen.png");

			/*//------------------------- SAVE --------------------------------------

			Gameobject* save_go = AddGameobject("save button", pause_background_go);

			C_Button* save = new C_Button(save_go, Event(SCENE_CHANGE, this, MAIN));
			save->target = { 0.51f, 0.3f, 0.3f, 0.3f };
			save->offset = { -525.f, 200.f };
			for (int i = 0; i < 4; i++)save->section[i] = { 0, 0, 1070, 207 };
			save->tex_id = App->tex.Load("textures/button.png");

			C_Button* save_fx = new C_Button(save_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
			save_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
			save_fx->offset = { -525.f, 200.f };
			for (int i = 0; i < 4; i++)save_fx->section[i] = { 0, 0, 1070, 207 };

			//------------------------- LOAD --------------------------------------

			Gameobject* load_go = AddGameobject("load Button", pause_background_go);

			C_Button* load = new C_Button(load_go, Event(SCENE_CHANGE, this, MAIN));
			load->target = { 0.51f, 0.3f, 0.3f, 0.3f };
			load->offset = { -525.f, 500.f };
			for (int i = 0; i < 4; i++)load->section[i] = { 0, 0, 1070, 207 };
			load->tex_id = App->tex.Load("textures/button.png");

			C_Button* load_fx = new C_Button(load_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
			load_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
			load_fx->offset = { -525.f, 500.f };
			for (int i = 0; i < 4; i++)load_fx->section[i] = { 0, 0, 1070, 207 };

			//------------------------- OPTIONS --------------------------------------

			Gameobject* options_go = AddGameobject("options Button", pause_background_go);

			C_Button* options = new C_Button(options_go, Event(SCENE_CHANGE, this, MAIN));
			options->target = { 0.51f, 0.3f, 0.3f, 0.3f };
			options->offset = { -525.f, 800.f };
			for (int i = 0; i < 4; i++)options->section[i] = { 0, 0, 1070, 207 };
			options->tex_id = App->tex.Load("textures/button.png");

			C_Button* options_fx = new C_Button(options_go, Event(PLAY_FX, App->audio, int(SELECT), 0));
			options_fx->target = { 0.51f, 0.3f, 0.3f, 0.3f };
			options_fx->offset = { -525.f, 800.f };
			for (int i = 0; i < 4; i++)options_fx->section[i] = { 0, 0, 1070, 207 };*/

			//------------------------- MAIN MENU --------------------------------------

			Gameobject* main_menu_go = AddGameobject("main menu Button", pause_background_go);

			C_Button* main_menu = new C_Button(main_menu_go, Event(BUTTON_EVENT, this, SCENE_CHANGE, MENU));
			main_menu->target = { 0.51f, 0.3f, 0.6f, 0.6f };
			main_menu->offset = { -250.f, 220.f };

			main_menu->section[0] = { 0, 0, 470, 90 };
			main_menu->section[1] = { 0, 101, 470, 90 };
			main_menu->section[2] = { 0, 202, 470, 90 };
			main_menu->section[3] = { 0, 202, 470, 90 };

			main_menu->tex_id = App->tex.Load("Assets/textures/main-menu.png");
		}

		if (paused_scene)
		{
			App->time.StartGameTimer();
			Event::Push(SCENE_PLAY, App->audio);
			Event::Push(ON_PLAY, &root);
			pause_background_go->SetInactive();
		}
		else
		{
			App->time.StopGameTimer();
			Event::Push(SCENE_PAUSE, App->audio);
			Event::Push(ON_PAUSE, &root);
			pause_background_go->SetActive();
		}

		paused_scene = !paused_scene;
	}
}

void Scene::UpdateSelection()
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

				if (posToWorld.first > groupStart.x && posToWorld.first < mouseExtend.x) //Right
				{
					if (posToWorld.second > groupStart.y && posToWorld.second < mouseExtend.y)//Up
					{
						group.push_back(it->second->GetGameobject());
						if (it->second->GetState() != DESTROYED) Event::Push(ON_SELECT, it->second->GetGameobject());
					}
					else if (posToWorld.second < groupStart.y && posToWorld.second > mouseExtend.y)//Down
					{
						group.push_back(it->second->GetGameobject());
						if(it->second->GetState() != DESTROYED) Event::Push(ON_SELECT, it->second->GetGameobject());
					}
				}
				else if (posToWorld.first < groupStart.x && posToWorld.first > mouseExtend.x)//Left
				{
					if (posToWorld.second > groupStart.y && posToWorld.second < mouseExtend.y)//Up
					{
						group.push_back(it->second->GetGameobject());
						if (it->second->GetState() != DESTROYED) Event::Push(ON_SELECT, it->second->GetGameobject());
					}
					else if (posToWorld.second < groupStart.y && posToWorld.second > mouseExtend.y)//Down
					{
						group.push_back(it->second->GetGameobject());
						if (it->second->GetState() != DESTROYED) Event::Push(ON_SELECT, it->second->GetGameobject());
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
		if (App->input->GetMouseButtonDown(0))
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			SDL_Rect cam = App->render->GetCameraRect();
			for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
			{
				if (it->second->GetType() == UNIT_MELEE || it->second->GetType() == GATHERER || it->second->GetType() == UNIT_RANGED
					|| it->second->GetType() == BASE_CENTER || it->second->GetType() == TOWER || it->second->GetType() == BARRACKS
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
		if (App->pathfinding.ValidTile(int(mouseOnMap.first), int(mouseOnMap.second)))
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
				if (selection) Event::Push(ON_RIGHT_CLICK, selection, vec(mouseOnMap.first, mouseOnMap.second, 0.5f), vec(-1, -1, -1));					
				groupSelect = false;
				group.clear();
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
				if (random < MELEE_RATE) Event::Push(SPAWN_UNIT, this, ENEMY_MELEE, pos);
				else if (random < (MELEE_RATE + RANGED_RATE)) Event::Push(SPAWN_UNIT, this, ENEMY_RANGED, pos);
				else if (random < (MELEE_RATE + RANGED_RATE + SUPER_RATE)) Event::Push(SPAWN_UNIT, this, ENEMY_SUPER, pos);
				else  Event::Push(SPAWN_UNIT, this, ENEMY_SPECIAL, pos);
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
		
		distance = JMath::Distance(last_cam_pos,current_cam_pos);

		if (distance > last_distance) {
			total_distance += distance - last_distance;
		}
		else if (last_distance > distance) {
			total_distance += last_distance - distance;
		}
		last_distance = distance;
		if (total_distance >= 500.0f && r_c_comprobation) {
			r_c_comprobation = false;
			//LOG("camera dist %f ", total_distance);
			Event::Push(GAMEPLAY, this, R_CLICK_MOVEMENT);
		}
		LOG("camera dist %f ", total_distance);

		break;
	case R_CLICK_MOVEMENT:
		if (App->input->GetMouseButtonDown(2) == KEY_DOWN /*&& App->scene->selection == gather_go*/) {

			tutorial_clicks++;
			if (tutorial_clicks == 10) Event::Push(GAMEPLAY, this, EDGE_STATE);

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
		LOG("barrack number %d", player_stats[CURRENT_BARRACKS]);
		if (player_stats[CURRENT_BARRACKS] >= 1) {

			Event::Push(GAMEPLAY, this, MELEE);
		}

		break;

	case MELEE:

		if (player_stats[CURRENT_MELEE_UNITS] == 1) {

			Event::Push(GAMEPLAY, this, ENEMY);
		}

		break;
	case ENEMY:

		if (player_stats[UNITS_KILLED] == 1) {

			Event::Push(GAMEPLAY, this, MELEE_ATK);
		}

		break;
	case MELEE_ATK:

		if (player_stats[CURRENT_MOB_DROP] > 0) Event::Push(GAMEPLAY, this, MOBDROP);

		break;
	case MOBDROP:


		break;
	case BUILD:

		if (player_stats[CURRENT_TOWERS] == 1) {

			Event::Push(GAMEPLAY, this, UPGRADE);
		}

		break;
	case UPGRADE:

		if (t_lvl > 1) Event::Push(GAMEPLAY, this, TOWER_ATK);

		break;

	case TOWER_ATK:


		break;

	case SPAWNER_STATE:

		if (player_stats[CURRENT_SPAWNERS] == 0) Event::Push(GAMEPLAY, this, WIN);
		break;

	case WIN:

		if(!endScene)Event::Push(GAMEPLAY, this, WIN);
		break;

	case LOSE:

		if(!endScene)Event::Push(GAMEPLAY, this, LOSE);
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


		distance = 0.0f;
		last_distance = 0.0f;
		total_distance = 0.0f;

		last_cam_pos = App->render->GetCameraCenter();

		not_go->SetInactive();
		LOG("CAM MOVEMENT STATE");
		not_go = AddGameobjectToCanvas("cam_mov");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(SCENE_PLAY, this, App));
		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/cam-not.png");

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };

		not_inactive->section[0] = { 0, 0, 309, 37 };
		not_inactive->section[1] = { 0, 44, 309, 37 };
		not_inactive->section[2] = { 0, 88, 309, 37 };
		not_inactive->section[3] = { 0, 88, 309, 37 };

		not_inactive->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");



		current_state = CAM_MOVEMENT;
		break;
	case R_CLICK_MOVEMENT:
	{
		not_go->SetInactive();
		LOG("R CLICK MOVEMENT STATE");
		not_go = AddGameobjectToCanvas("R_click_mov");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(SCENE_PLAY, App));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/movement-not.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		not_inactive->section[0] = { 0, 0, 309, 37 };
		not_inactive->section[1] = { 0, 44, 309, 37 };
		not_inactive->section[2] = { 0, 88, 309, 37 };
		not_inactive->section[3] = { 0, 88, 309, 37 };
		not_inactive->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");



		std::pair<float, float> current_cam_pos_t = Map::F_WorldToMap(current_cam_pos.first, current_cam_pos.second);


		gather_go = AddGameobject("Tutorial Gatherer");
		gather_go->GetTransform()->SetLocalPos({ current_cam_pos_t.first, current_cam_pos_t.second,0.0f });

		new Gatherer(gather_go);

		current_state = R_CLICK_MOVEMENT;
		break;
	}
	case EDGE_STATE:
	{
		not_go->SetInactive();
		LOG("EDGE STATE");
		not_go = AddGameobjectToCanvas("edge_go");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(SCENE_PLAY, App));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/edge-not.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		not_inactive->section[0] = { 0, 0, 309, 37 };
		not_inactive->section[1] = { 0, 44, 309, 37 };
		not_inactive->section[2] = { 0, 88, 309, 37 };
		not_inactive->section[3] = { 0, 88, 309, 37 };

		std::pair<float, float> current_cam_pos_t = Map::F_WorldToMap(current_cam_pos.first, current_cam_pos.second);

		edge_t_go = AddGameobject("Tutorial Gatherer");
		edge_t_go->GetTransform()->SetLocalPos({ current_cam_pos_t.first, current_cam_pos_t.second,0.0f });

		new Edge(edge_t_go);


		current_state = EDGE_STATE;
	}
	break;
	case BASE_CENTER_STATE:
	{
		not_go->SetInactive();
		tutorial_edge = false;
		LOG("BASE CENTER STATE");

		not_go = AddGameobjectToCanvas("base_center_go");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(GAMEPLAY, this, RESOURCES));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/base-not.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		/*not_inactive = new C_Button(not_go, Event(GAMEPLAY, this, SET_INACTIVE));

		not_inactive->target = { 0.74f, 0.726f, 0.6f, 0.6f };
		not_inactive->offset = { -309.f, 37.f };
		not_inactive->section = { 0, 0, 309, 37 };*/

		current_state = BASE_CENTER_STATE;
	}
	break;
	case RESOURCES:
		not_go->SetInactive();
		LOG("RESOURCES STATE");
		not_go = AddGameobjectToCanvas("resources_state_go");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(GAMEPLAY, this, BARRACKS_STATE));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/barracks-not.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		/*not_inactive = new C_Button(not_go, Event(GAMEPLAY, this, SET_INACTIVE));

		not_inactive->target = { 0.74f, 0.726f, 0.6f, 0.6f };
		not_inactive->offset = { -309.f, 37.f };
		not_inactive->section = { 0, 0, 309, 37 };*/

		//Explain Edge and what it is used for
		current_state = RESOURCES;

		break;
	case BARRACKS_STATE:
		not_go->SetInactive();
		LOG("BARRACKS STATE");

		//BUILD BARRACKS -> NEXT STATE
		current_state = BARRACKS_STATE;

		break;

	case MELEE:
		//barracks_state_go->SetInactive();
		LOG("MELEE STATE");
		not_go = AddGameobjectToCanvas("melee_go");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(SCENE_PLAY, App));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/melee-not.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		for (int i = 0; i < 4; i++)not_inactive->section[i] = { 0, 0, 309, 37 };

		//new MeleeUnit(melee_go);
		current_state = MELEE;
		//BUILD MELEE -> NEXT STATE

		break;
	case ENEMY:
	{
		not_go->SetInactive();
		LOG("ENEMY STATE");
		not_go = AddGameobjectToCanvas("enemy_go");
		not = new C_Image(not_go);
		next = new C_Button(not_go, Event(GAMEPLAY, this, MELEE_ATK));

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/tuto/enemy-not.png");

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		for (int i = 0; i < 4; i++)not_inactive->section[i] = { 0, 0, 309, 37 };

		std::pair<float, float> current_cam_pos_t = Map::F_WorldToMap(current_cam_pos.first, current_cam_pos.second);


		enemy_go = AddGameobject("Tutorial Enemy");
		enemy_go->GetTransform()->SetLocalPos({ current_cam_pos_t.first, current_cam_pos_t.second,0.0f });

		new EnemyMeleeUnit(enemy_go);
		current_state = ENEMY;
		break;
	}
	case MELEE_ATK:
		not_go->SetInactive();
		LOG("MELEE ATK STATE");
		current_state = MELEE_ATK;
		break;

	case MOBDROP:
		not_go->SetInactive();
		LOG("not STATE");
		not_go = AddGameobjectToCanvas("mobdrop_go");
		not = new C_Image(not_go);

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/mobdrop-not.png");

		next = new C_Button(not_go, Event(GAMEPLAY, this, BUILD));
		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		for (int i = 0; i < 4; i++)not_inactive->section[i] = { 0, 0, 309, 37 };

		current_state = MOBDROP;

		break;
	case BUILD:
		not_go->SetInactive();
		LOG("BUILD STATE");
		not_go = AddGameobjectToCanvas("build_go");
		not = new C_Image(not_go);

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/build-not.png");

		next = new C_Button(not_go, Event(SCENE_PLAY, App));

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		for (int i = 0; i < 4; i++)not_inactive->section[i] = { 0, 0, 309, 37 };

		current_state = BUILD;

		break;
	case UPGRADE:
		not_go->SetInactive();

		LOG("UPGRADE STATE");
		not_go = AddGameobjectToCanvas("upgrade_go");
		not = new C_Image(not_go);

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/upgrade-not.png");

		next = new C_Button(not_go, Event(SCENE_PLAY, App));

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		for (int i = 0; i < 4; i++)not_inactive->section[i] = { 0, 0, 309, 37 };

		current_state = UPGRADE;

		break;
	case TOWER_STATE:
		not_go->SetInactive();

		LOG("TOWER STATE");
		current_state = TOWER_STATE;

		break;
	case TOWER_ATK:
		not_go->SetInactive();
		LOG("TOWER ATK STATE");
		not_go = AddGameobjectToCanvas("tower_atk_go");
		not = new C_Image(not_go);

		not->target = { 0.75f, 0.8f, 0.4f, 0.4f };
		not->offset = { -183.f, -1044.f };
		not->section = { 0, 0, 983, 644 };
		not->tex_id = App->tex.Load("Assets/textures/toweratk-not.png");

		next = new C_Button(not_go, Event(GAMEPLAY, this, SPAWNER_STATE));

		next->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		next->offset = { 500.f, -317.f };

		next->section[0] = { 0, 0, 309, 37 };
		next->section[1] = { 0, 44, 309, 37 };
		next->section[2] = { 0, 88, 309, 37 };
		next->section[3] = { 0, 88, 309, 37 };

		next->tex_id = App->tex.Load("Assets/textures/tuto/not-button.png");

		not_inactive = new C_Button(not_go, Event(SET_INACTIVE, this, MAIN));

		not_inactive->target = { 0.74f, 0.726f, 0.4f, 0.4f };
		not_inactive->offset = { 500.f, -317.f };
		for (int i = 0; i < 4; i++)not_inactive->section[i] = { 0, 0, 309, 37 };

		current_state = TOWER_ATK;


		break;
	case SPAWNER_STATE:
		not_go->SetInactive();
		LOG("SPAWNER STATE");

		SpawnBehaviour(SPAWNER, spawner_pos1);
		SpawnBehaviour(SPAWNER, spawner_pos2);
		SpawnBehaviour(SPAWNER, spawner_pos3);

		current_state = SPAWNER_STATE;
		break;

	case WIN:
		//Kill 200 units
		current_state = WIN;
		win = true;
		endScene = true;
		LoadEndScene();

		break;
	case LOSE:
		//Base center destroyed
		current_state = LOSE;
		win = false;
		endScene = true;
		LoadEndScene();

		break;
	default:
		break;
	}
}

void Scene::ResetScene()
{
	scene_change_timer = -1.f;
	next_scene = EMPTY;

	for (int i = 0; i < EDGE_COLLECTED; ++i)
		hud_texts[i] = nullptr;

	for (int i = 0; i < MAX_PLAYER_STATS; ++i)
		player_stats[i] = 0;

	pause_background_go = nullptr;
	groupSelect = false;
	group.clear();

	spawnPoints.clear();
	spawnPoints.push_back(vec(20, 20, 0.5));
	spawnPoints.push_back(vec(30, 20, 0.5));
	spawnPoints.push_back(vec(20, 30, 0.5));

	currentSpawns = 0;
	maxSpawns = 200;
	spawnCounter = 0;
	cooldownSpawn = 5.0f;
	last_cam_pos = { 0.0f,0.0f };
	total_distance = 0;

	App->collSystem.Clear();
	map.CleanUp();
	App->fogWar.CleanUp();
	App->audio->UnloadFx();
	App->audio->StopMusic(1.f);
	SetSelection(nullptr, false);
	root.RemoveChilds();
	Event::PumpAll();
	fogLoaded = false;
	root.UpdateRemoveQueue();
}

void Scene::ChangeToScene(SceneType scene)
{
	ResetScene();

	switch (current_scene = scene)
	{
	case TEST: LoadTestScene(); break;
	case INTRO: LoadIntroScene(); break;
	case MENU: LoadMenuScene(); break;
	case MAIN: LoadMainScene(); break;
	case MAIN_FROM_SAFE: break;
	case END: LoadEndScene(); break;
	case CREDITS: break;
	default: break;
	}
}

bool Scene::OnMainScene() const
{
	return current_scene == MAIN;
}

Transform* Scene::SpawnBehaviour(int type, vec pos)
{
	//LOG("Spawn");
	Transform* ret = nullptr;
	Gameobject* behaviour = nullptr;

	switch (UnitType(type))
	{
	case GATHERER:
	{
		//LOG("Edge %d", player_stats[CURRENT_EDGE]);
		if ((player_stats[CURRENT_EDGE] - GATHERER_COST) >= 0)
		{
			behaviour = AddGameobject("Gatherer");
			new Gatherer(behaviour);
			UpdateStat(CURRENT_GATHERER_UNITS, 1);
			UpdateStat(TOTAL_GATHERER_UNITS, 1);
			UpdateStat(CURRENT_EDGE,-GATHERER_COST);
		}
		else
		{
			LOG("Not enough resources! :(");
		}
		break;
	}
	case UNIT_MELEE:
	{
		if ((player_stats[CURRENT_EDGE] - MELEE_COST) >= 0)
		{
			behaviour = AddGameobject("Unit melee");
			new MeleeUnit(behaviour);
			UpdateStat(CURRENT_MELEE_UNITS, 1);
			UpdateStat(TOTAL_MELEE_UNITS, 1);
			UpdateStat(CURRENT_EDGE, -MELEE_COST);
		}
		else
		{
			LOG("Not enough resources! :(");
		}
		break;
	}
	case UNIT_RANGED: 
		if ((player_stats[CURRENT_EDGE] - RANGED_COST) >= 0)
		{
			behaviour = AddGameobject("Ranged unit");
			new RangedUnit(behaviour);
			UpdateStat(CURRENT_RANGED_UNITS, 1);
			UpdateStat(TOTAL_RANGED_UNITS, 1);
			UpdateStat(CURRENT_EDGE, -RANGED_COST);
		}
		else
		{
			LOG("Not enough resources! :(");
		}
		break;		
	case UNIT_SUPER: 
		if ((player_stats[CURRENT_EDGE] - SUPER_COST) >= 0)
		{
			behaviour = AddGameobject("Super unit");
			new RangedUnit(behaviour);
			UpdateStat(CURRENT_RANGED_UNITS, 1);
			UpdateStat(TOTAL_RANGED_UNITS, 1);
			UpdateStat(CURRENT_EDGE, - SUPER_COST);
		}
		else
		{
			LOG("Not enough resources! :(");
		}
		break;
	case ENEMY_MELEE:
	{
		behaviour = AddGameobject("Enemy Melee");
		new EnemyMeleeUnit(behaviour);
		break;
	}
	case ENEMY_RANGED: 
		behaviour = AddGameobject("Enemy Melee");
		new EnemyMeleeUnit(behaviour); ///Temporal
		break;
	case ENEMY_SUPER: 
		behaviour = AddGameobject("Enemy Melee");
		new EnemyMeleeUnit(behaviour); ///Temporal
		break;
	case ENEMY_SPECIAL: 
		behaviour = AddGameobject("Enemy Melee");
		new EnemyMeleeUnit(behaviour); ///Temporal
		break;
	case BASE_CENTER:
	{		
		if ((player_stats[CURRENT_EDGE] - 20) >= 0)
		{
			std::pair<int, int> thisPos(pos.x,pos.y);
			if(App->pathfinding.CheckWalkabilityArea(thisPos, vec(4.0f, 4.0f, 1.0f)))
			{
				behaviour = AddGameobject("Base Center");
				behaviour->GetTransform()->SetLocalPos(pos);
				behaviour->GetTransform()->ScaleX(4.0f);
				behaviour->GetTransform()->ScaleY(4.0f);
				new Base_Center(behaviour);
				UpdateStat(CURRENT_EDGE, -20);
				//Update paths
				for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
					Event::Push(UPDATE_PATH, it->second, pos.x - 1, pos.y - 1);
			}	
			else LOG("Can't place building");			
		}
		else LOG("Not enough resources! :(");		
		break;
	}
	case TOWER:
	{
		if ((player_stats[CURRENT_EDGE] - TOWER_COST) >= 0)
		{
			std::pair<int, int> thisPos(pos.x, pos.y);
			if (App->pathfinding.CheckWalkabilityArea(thisPos, vec(1.0f,1.0f,1.0f)))
			{
				behaviour = AddGameobject("Tower");
				behaviour->GetTransform()->SetLocalPos(pos);
				behaviour->GetTransform()->ScaleX(1.0f);
				behaviour->GetTransform()->ScaleY(1.0f);
				new Tower(behaviour);
				UpdateStat(CURRENT_TOWERS, 1);
				UpdateStat(TOTAL_TOWERS, 1);
				UpdateStat(CURRENT_EDGE, -TOWER_COST);
				//Update paths
				for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
					Event::Push(UPDATE_PATH, it->second, pos.x - 1, pos.y - 1);
			}
			else LOG("Can't place building");			
		}
		else LOG("Not enough resources! :(");		
		break;
	}
	case WALL: break;
	case BARRACKS:
	{			
		if ((player_stats[CURRENT_EDGE] - BARRACKS_COST) >= 0)
		{
			std::pair<int, int> thisPos(pos.x, pos.y);
			if (App->pathfinding.CheckWalkabilityArea(thisPos,vec(6.0f, 6.0f, 1.0f)))
			{
				behaviour = AddGameobject("Barracks");
				behaviour->GetTransform()->SetLocalPos(pos);
				behaviour->GetTransform()->ScaleX(6.0f);
				behaviour->GetTransform()->ScaleY(6.0f);
				new Barracks(behaviour);
				UpdateStat(CURRENT_BARRACKS, 1);
				UpdateStat(TOTAL_BARRACKS, 1);
				UpdateStat(CURRENT_EDGE, -BARRACKS_COST);

				//Update paths
				for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
					Event::Push(UPDATE_PATH, it->second, pos.x - 1, pos.y - 1);
			}
			else LOG("Can't place building");			
		}
		else LOG("Not enough resources! :(");
		break;
	}
	case LAB: break;
	case EDGE:
	{
		behaviour = AddGameobject("Edge");
		new Edge(behaviour);
		break;
	}
	case SPAWNER:
	{
		behaviour = AddGameobject("Spawner");
		new Spawner(behaviour);
		UpdateStat(CURRENT_SPAWNERS, 1);
		break;
	}
	default: break;
	}

	if (behaviour)
	{
		ret = behaviour->GetTransform();
		ret->SetLocalPos(pos);
	}

	return ret;
}

bool Scene::DamageAllowed()
{
	return !god_mode || (god_mode && no_damage);
}

bool Scene::DrawCollisions()
{
	return god_mode && draw_collisions;
}

int Scene::GetStat(int stat)
{
	return player_stats[stat];
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
			if ((*it)->GetBehaviour()->GetState() != DESTROYED) Event::Push(ON_UNSELECT, *it);
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
				if (call_unselect && selection->GetBehaviour()->GetState() != DESTROYED)
					Event::Push(ON_UNSELECT, selection);

				Event::Push(ON_SELECT, go);
			}
		}
		else
			Event::Push(ON_SELECT, go);
	}
	else if (selection != nullptr && call_unselect && selection->GetBehaviour()->GetState() != DESTROYED)
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
	//		   #: Spawn Units
	// LCTRL + #: Spawn Structures
	bool pressing_lctrl = (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT);
	int x, y;
	App->input->GetMousePosition(x, y);
	SDL_Rect cam = App->render->GetCameraRect();
	std::pair<int, int> position = Map::WorldToTileBase(float(x + cam.x), float(y + cam.y));
	int max = pressing_lctrl ? MAX_UNIT_TYPES - BASE_CENTER : BASE_CENTER;
	for (int i = 0; i < max; ++i)
		if (App->input->GetKey(SDL_SCANCODE_1 + i) == KEY_DOWN)
			Event::Push(SPAWN_UNIT, this, (pressing_lctrl ? BASE_CENTER : 0) + i, vec(float(position.first), float(position.second)));

	// LALT + #: Change Scene
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
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

	// F1: Show/Hide Editor Windows
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->editor->ToggleEditorVisibility();

	// F2: Show/Hide Map Walkability
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		//map.draw_walkability = !map.draw_walkability;
		App->pathfinding.DebugWalkability();
	}
		

	// F3: Toggle Music Playing
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->audio->MusicIsPlaying() ?
			App->audio->StopMusic(1.f) :
			App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");
	}

	// F4: Toggle Allowed Damage
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		no_damage = !no_damage;

	// F5: Toggle Collision & Path Drawing
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->pathfinding.DebugShowPaths();
		//draw_collisions = !draw_collisions;
		App->collSystem.SetDebug();
	}

	// F6: Toggle Zoom Locked
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->render->ToggleZoomLocked();

	// F7: Toggle Collision & Path Drawing
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		Event::Push(TOGGLE_FULLSCREEN, App->win);

	// F8: Toggle draw unit vision and attack range
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
			Event::Push(DRAW_RANGE, it->second);
	}

	// SPACE: Swap map orientation
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) Map::SwapMapType();

	// DEL: Remove Selected Gameobject/s
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		if (!group.empty())
		{
			for (std::vector<Gameobject*>::iterator it = group.begin(); it != group.end(); ++it)
				(*it)->Destroy();
							
			groupSelect = false;
			group.clear();
			App->audio->PlayFx(UNIT_DIES);
		}
		
		if (selection)
		{
			selection->Destroy();
			SetSelection();
			App->audio->PlayFx(UNIT_DIES);
		}
	}

	// Update window title
	std::pair<int, int> map_coordinates = Map::WorldToTileBase(cam.x + x, cam.y + y);
	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Zoom: %0.2f, Mouse: %dx%d, Tile: %dx%d, Selection: %s",
		App->time.GetLastFPS(),
		App->render->GetZoom(),
		x, y,
		map_coordinates.first, map_coordinates.second,
		selection != nullptr ? selection->GetName() : (groupSelect ? "Group selection" : "None selected"));
	App->win->SetTitle(tmp_str);

	// Arrow Keys: Increase/Decrease Resources
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		UpdateStat(CURRENT_EDGE, 50);
		UpdateStat(EDGE_COLLECTED, 50);
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		int quantity = -(player_stats[CURRENT_EDGE] < 50 ? player_stats[CURRENT_EDGE] : 50);
		UpdateStat(CURRENT_EDGE, quantity);
		UpdateStat(EDGE_COLLECTED, quantity);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		UpdateStat(CURRENT_MOB_DROP, 20);
		UpdateStat(MOB_DROP_COLLECTED, 20);
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		int quantity = -(player_stats[CURRENT_MOB_DROP] < 20 ? player_stats[CURRENT_MOB_DROP] : 20);
		UpdateStat(CURRENT_MOB_DROP, quantity);
		UpdateStat(MOB_DROP_COLLECTED, quantity);
	}
}

void Scene::ToggleGodMode()
{
	if (god_mode)
	{
		App->win->SetTitle("Square Up");
		App->audio->PlayFx(UNIT_DIES);
	}

	god_mode = !god_mode;
	App->fogWar.debugMode = !App->fogWar.debugMode;
}
