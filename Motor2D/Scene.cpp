#include "Scene.h"
#include "Application.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "Render.h"
#include "Map.h"
#include "MapContainer.h"
#include "TimeManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Defs.h"
#include "Log.h"
#include <math.h>

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
	Transform* t1 = go1->GetTransform();
	Transform* t2 = go2->GetTransform();

	// Rotate go2 around go1
	time += App->time->GetDeltaTime() * 5.00f;
	t2->SetX(sin(time) * 2.0f);
	t2->SetY(cos(time) * 2.0f);

	// Shift acceleration
	float moveSpeed = 4.000f * App->time->GetDeltaTime();
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		moveSpeed *= 5.000f;

	// Move go1
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)	t1->MoveX(-moveSpeed);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)	t1->MoveX(moveSpeed);
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)		t1->MoveY(-moveSpeed);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)	t1->MoveY(moveSpeed);

	// Move camera
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) App->render->cam_x -= moveSpeed * 50.f;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) App->render->cam_x += moveSpeed * 50.f;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) App->render->cam_y -= moveSpeed * 50.f;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) App->render->cam_y += moveSpeed * 50.f;

	// Swap map orientation
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) App->map->SwapMapType();

	// Update gameobject hierarchy
	root.Update();

	return true;
}

bool Scene::PostUpdate()
{
	root.PostUpdate();

	// Debug Pointer Info on Window Title
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	std::pair<int, int> map_coordinates = App->map->I_WorldToMap(int(App->render->cam_x) + mouse_x, int(App->render->cam_y) + mouse_y);

	// Debug gameobject transforms
	vec go1_pos = go1->GetTransform()->GetGlobalPosition();
	vec go2_pos = go2->GetTransform()->GetGlobalPosition();

	// Log onto window title
	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Mouse: %dx%d, Tile: %dx%d, g1 { %d, %d, %d }, g2 { %d, %d, %d }",
		App->time->GetLastFPS(),
		mouse_x, mouse_y,
		map_coordinates.first, map_coordinates.second,
		(int)go1_pos.x, (int)go1_pos.y, (int)go1_pos.z,
		(int)go2_pos.x, (int)go2_pos.y, (int)go2_pos.z);

	App->win->SetTitle(tmp_str);

	return true;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	
	return true;
}

bool Scene::LoadTestScene()
{
	// Play sample track
	//App->audio->PlayMusic("audio/music/lvl1bgm.ogg");

	// Remove fps cap
	App->time->SetMaxFPS(0);

	// Load mouse debug texture for identifying tiles
	id_mouse_tex = App->tex->Load("textures/meta.png");

	// Run test content
	go1 = AddGameobject("g1 - son of root", &root);
	go2 = AddGameobject("g2 - son of g1", go1);

	Sprite* s1 = new Sprite(go1);
	s1->tex_id = id_mouse_tex;
	s1->section = { 0, 0, 64, 64 };

	Sprite* s2 = new Sprite(go2);
	s2->tex_id = id_mouse_tex;
	s2->section = { 64, 0, 64, 64 };

	return id_mouse_tex != -1;
}

Gameobject * Scene::AddGameobject(const char * name, Gameobject * parent)
{
	return new Gameobject(name, parent != nullptr ? parent : &root);
}
