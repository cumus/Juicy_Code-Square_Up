#include "Scene.h"
#include "Application.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "Render.h"
#include "Map.h"
#include "MapContainer.h"
#include "FadetoBlack.h"
#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module("scene")
{}

Scene::~Scene()
{}

bool Scene::Awake(pugi::xml_node& config)
{
	return true;
}

bool Scene::Start()
{
	return true;
}

bool Scene::PreUpdate()
{
	root.PreUpdate();

	return true;
}

bool Scene::Update()
{
	root.Update();

	Transform* t1 = go1->GetTransform();
	Transform* t2 = go2->GetTransform();

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) t1->MoveX(-1.f);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) t1->MoveX(1.f);
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) t1->MoveY(1.f);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) t1->MoveY(-1.f);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)	t2->MoveX(-1.f);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)	t2->MoveX(1.f);
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)		t2->MoveY(1.f);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)	t2->MoveY(-1.f);


	/*if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");*/

	App->map->Draw();

	// Debug Pointer Info on Window Title
	int x, y;
	App->input->GetMousePosition(x, y);
	std::pair<int, int> map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	const MapContainer* map = App->map->GetMap();
	vec go1_pos = t1->GetGlobalPosition();
	vec go2_pos = t2->GetGlobalPosition();

	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "Mouse: %dx%d, Map size: %dx%d, Tile size: %dx%d, Tilesets: %d, Tile: %dx%d, g1:%f:%f:%f, g2:%f:%f:%f",
		x, y,
		map->width, map->height,
		map->tile_width, map->tile_height,
		map->tilesets.size(),
		map_coordinates.first, map_coordinates.second,
		go1_pos.x, go1_pos.y, go1_pos.z,
		go2_pos.x, go2_pos.y, go2_pos.z);

	App->win->SetTitle(tmp_str);

	return true;
}

bool Scene::PostUpdate()
{
	root.PostUpdate();

	return true;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	
	return true;
}

bool Scene::LoadTestScene()
{
	// Run test content
	go1 = AddGameobject("sample name", &root);
	go2 = AddGameobject("son", go1);

	App->audio->PlayMusic("audio/music/lvl1bgm.ogg");

	return App->map->LoadFromFile("level1.tmx");
}

Gameobject * Scene::AddGameobject(const char * name, Gameobject * parent)
{
	return new Gameobject(name, parent != nullptr ? parent : &root);
}
