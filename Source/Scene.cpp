#include "Scene.h"
#include "Application.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "Render.h"
#include "Map.h"
#include "Editor.h"
#include "MapContainer.h"
#include "TimeManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Defs.h"
#include "Log.h"

#include "Optick/include/optick.h"

#include <math.h>
#include <queue>

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
	bool ret = true;

	OPTICK_EVENT();

	// Load sample maps
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		ret = App->map->LoadFromFile("iso.tmx");
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		ret = App->map->LoadFromFile("level1.tmx");
	}

	if (go1 != nullptr && go2 != nullptr)
	{
		Transform* t1 = go1->GetTransform();
		Transform* t2 = go2->GetTransform();

		if (t1 != nullptr && t2 != nullptr)
		{
			// Rotate go2 around go1
			time += App->time.GetDeltaTime() * 5.00f;
			t2->SetX(sin(time) * 2.0f);
			t2->SetY(cos(time) * 2.0f);

			// Shift acceleration
			float moveSpeed = 4.000f * App->time.GetDeltaTime();
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
				moveSpeed *= 5.000f;

			// Move go1
			if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)	t1->MoveX(-moveSpeed);
			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)	t1->MoveX(moveSpeed);
			if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)		t1->MoveY(-moveSpeed);
			if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)	t1->MoveY(moveSpeed);
		}
	}

	// Swap map orientation
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) App->map->SwapMapType();

	// Update gameobject hierarchy
	root.Update();

	return ret;
}

bool Scene::PostUpdate()
{
	root.PostUpdate();

	// Debug Pointer Info on Window Title
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	SDL_Rect cam_rect = App->render->GetCameraRect();
	std::pair<int, int> map_coordinates = App->map->WorldToTileBase(cam_rect.x + mouse_x, cam_rect.y + mouse_y);

	// Debug gameobject transforms
	vec go1_pos;
	vec go2_pos;

	if (go1 != nullptr && go2 != nullptr)
	{
		go1_pos = go1->GetTransform()->GetGlobalPosition();
		go2_pos = go2->GetTransform()->GetGlobalPosition();
	}

	// Editor Selection
	Gameobject* sel = App->editor->selection;

	// Log onto window title
	static char tmp_str[220];
	sprintf_s(tmp_str, 220, "FPS: %d, Zoom: %0.2f, Mouse: %dx%d, Tile: %dx%d, g1 { %d, %d, %d }, g2 { %d, %d, %d }, Selection: %s",
		App->time.GetLastFPS(),
		App->render->GetZoom(),
		mouse_x, mouse_y,
		map_coordinates.first, map_coordinates.second,
		(int)go1_pos.x, (int)go1_pos.y, (int)go1_pos.z,
		(int)go2_pos.x, (int)go2_pos.y, (int)go2_pos.z,
		sel != nullptr ? sel->GetName().c_str() : "none selected");

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
	App->audio->PlayMusic("audio/Music/alexander-nakarada-buzzkiller.ogg");

	// Remove fps cap
	App->time.SetMaxFPS(0);

	// Load mouse debug texture for identifying tiles
	id_mouse_tex = App->tex.Load("textures/meta.png");

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

Gameobject* Scene::RaycastSelect()
{
	Gameobject* ret = nullptr;
	std::queue<Gameobject*> queue;
	std::vector<Gameobject*> root_childs = root.GetChilds();
	
	for (std::vector<Gameobject*>::iterator it = root_childs.begin(); it != root_childs.end(); ++it)
		queue.push(*it);

	if (!queue.empty())
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		SDL_Rect cam_rect = App->render->GetCameraRect();
		std::pair<float, float> map_coordinates = App->map->WorldToTileBase(cam_rect.x + mouse_x, cam_rect.y + mouse_y);

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
