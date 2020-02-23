#include "Scene.h"
#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "FadetoBlack.h"

Scene::Scene() : Module("scene")
{}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	count = config.child("count").attribute("value").as_int();
	levelNum = config.child("levelNum").attribute("value").as_int();
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	/*if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");*/

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	
	std::string title = "Map:";
	title += App->map->data.width;
	title += "x";
	title += App->map->data.height;
	title += " Tiles : ";
	title += App->map->data.tile_width;
	title += "x";
	title += App->map->data.tile_height;
	title += " Tilesets : ";
	title += (0 + App->map->data.tilesets.size());
	title += " Tile : ";
	title += map_coordinates.x;
	title += ", ";
	title += map_coordinates.y;

	App->win->SetTitle(title.c_str());
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	
	return true;
}