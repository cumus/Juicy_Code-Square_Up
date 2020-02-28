#include "Scene.h"
#include "Application.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "Render.h"
#include "Map.h"
#include "MapContainer.h"
#include "TimeManager.h"
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

	float moveSpeed = 200.000f * App->time->GetDeltaTime();
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) moveSpeed *= 5.000f;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) App->render->cam_x -= moveSpeed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) App->render->cam_x += moveSpeed;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) App->render->cam_y -= moveSpeed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) App->render->cam_y += moveSpeed;

	Transform* t1 = go1->GetTransform();
	Transform* t2 = go2->GetTransform();

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) t1->MoveX(-moveSpeed);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) t1->MoveX(moveSpeed);
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) t1->MoveY(-moveSpeed);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) t1->MoveY(moveSpeed);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)	t2->MoveX(-1.f);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)	t2->MoveX(1.f);
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)		t2->MoveY(-1.f);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)	t2->MoveY(1.f);

	/*if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");*/

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) App->map->SwapMapType();

	App->map->Draw();

	// Debug Pointer Info on Window Title
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);

	std::pair<int, int> map_coordinates = App->map->I_WorldToMap(
		int(App->render->cam_x) + mouse_x,
		int(App->render->cam_y) + mouse_y);

	const MapContainer* map = App->map->GetMap();

	vec go1_pos = t1->GetGlobalPosition();
	vec go2_pos = t2->GetGlobalPosition();

	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Mouse: %dx%d, Map size: %dx%d, Tile: %dx%d, g1:%d-%d-%d, g2:%d-%d-%d",
		App->time->GetLastFPS(),
		mouse_x, mouse_y,
		map->width, map->height,
		map_coordinates.first, map_coordinates.second,
		(int)go1_pos.x, (int)go1_pos.y, (int)go1_pos.z,
		(int)go2_pos.x, (int)go2_pos.y, (int)go2_pos.z);

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

	// Remove fps cap
	App->time->SetMaxFPS(0);

	return App->map->LoadFromFile("level1.tmx");
}

Gameobject * Scene::AddGameobject(const char * name, Gameobject * parent)
{
	return new Gameobject(name, parent != nullptr ? parent : &root);
}
