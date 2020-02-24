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
bool Scene::Update()
{
	/*if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");*/

	App->map->Draw();

	// Debug Pointer Info on Window Title
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	MapData* data = &App->map->data;
	static char tmp_str[120];
	sprintf_s(tmp_str, 120, "Mouse: %dx%d, Map: %dx%d, Tiles: %dx%d, Tilesets: %d, Tile: %dx%d",
		x, y,
		data->width, data->height,
		data->tile_width, data->tile_height,
		data->tilesets.size(),
		map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(tmp_str);

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