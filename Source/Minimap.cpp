#include "Minimap.h"
#include "Application.h"
#include "TextureManager.h"
#include "UI_Image.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "Editor.h"
#include "Map.h"
#include "Log.h"

#include <string>

Minimap::Minimap(Gameobject* go) :
	C_Image(go)
{
	LOG("Starting minimap");

	//renderer = App->render->GetSDLRenderer();
	//map_texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(App->win->GetWindow()), SDL_TEXTUREACCESS_TARGET, 1.05F * width, 1.05F * height);
	map_height = 200;
	scale = (width / (float)map_width);
	height = map_height * scale;
	map_width = 100;
	minimap_camera = { 0, 0, 4, 4 };
	tex_id = App->tex.Load("textures/white.png");
	test_rect = { App->render->cam.x, App->render->cam.y, App->render->cam.w * scale, App->render->cam.h * scale };

	App->win->GetWindowSize(window_width, window_height);

	switch (corner)
	{
	case Corner::TOP_LEFT:
	{
		pos.x = margin;
		pos.y = margin;
		break;
	}
	case Corner::TOP_RIGHT:
	{
		pos.x = window_width - width - margin;
		pos.y = margin;
		break;
	}
	case Corner::BOTTOM_LEFT:
	{
		pos.x = margin;
		pos.y = window_height - height - margin;
		break;
	}
	case Corner::BOTTOM_RIGHT:
	{
		pos.x = window_width - width - margin;
		pos.y = window_height - height - margin;
		break;
	}
	break;
	}

	//SDL_SetRenderTarget(renderer, map_texture);
	CreateMinimap();
	//SDL_SetRenderTarget(renderer, NULL);
}

Minimap::~Minimap()
{
}

bool Minimap::Awake(pugi::xml_node& config)
{
	width = config.attribute("width").as_int();
	std::string corner_string = std::string(config.attribute("corner").as_string());
	margin = config.attribute("margin").as_int();

	if (corner_string == "top_left")
		corner = Corner::TOP_LEFT;

	if (corner_string == "top_right")
		corner = Corner::TOP_RIGHT;

	if (corner_string == "bottom_left")
		corner = Corner::BOTTOM_LEFT;

	if (corner_string == "bottom_right")
		corner = Corner::TOP_RIGHT;

	return true;
}

void Minimap::Update()
{
	//App->render->DrawQuad(section, { 0, 0, 0, 255 }, true);

	//App->render->Blit((int)map_texture, pos.x, pos.y, NULL);

	iPoint minimap_camera_position = WorldToMinimap(test_rect.x, test_rect.y);
	minimap_camera.x = minimap_camera_position.x;
	minimap_camera.y = minimap_camera_position.y;
	App->render->DrawQuad(minimap_camera, { 255, 0, 0, 255 }, true, SCENE, false);

	/*SDL_Rect rect = { 0, 0, 0, 0 };
	iPoint rect_position = WorldToMinimap(-App->render->cam.x, -App->render->cam.y);
	App->render->DrawQuad({ rect_position.x, rect_position.y, (int)(App->render->cam.w * scale), (int)(App->render->cam.h * scale) }, { 0, 0, 0, 255 }, false, SCENE, false);
	
	if (test_rect.y < 250)
		going_up = false;
	if (test_rect.y > 550)
		going_up = true;

	if (going_up)
		test_rect.y--;
	else
		test_rect.y++;*/

}

bool Minimap::CreateMinimap()
{
	PERF_START(ptimer);
	int half_width = map_width * 0.5F;

	App->render->DrawQuad(section, {0, 0, 0, 255}, true);

	/*for (std::vector<MapLayer*>::const_iterator item = map.layers.begin(); item != map.layers.end(); ++item)
	{
		MapLayer* layer = *item;

		if (layer->GetProperty("Nodraw") != 0)
			continue;

		int half_width = map_width * 0.5F;

		for (int y = 0; y < map.height; ++y)
		{
			for (int x = 0; x < map.width; ++x)
			{
				unsigned int tile_id = layer->GetID(x, y);
				if (tile_id > 0)
				{
					SDL_Rect section;
					int text_id;
					if (map.GetRectAndTexId(tile_id, section, text_id))
					{
						std::pair<int, int> pos = map.I_MapToWorld(x, y);
						iPoint position = App->render->WorldToScreen(pos.first, pos.second);
						App->render->Blit(text_id, position.x + half_width, position.y, &section);
					}
				}
			}
		}
	}*/

	return true;
}

iPoint Minimap::WorldToMinimap(int x, int y)
{

	iPoint minimap_position;

	minimap_position.x = pos.x + width * 0.5F + x * scale;
	minimap_position.y = pos.y + y * scale;

	return minimap_position;
	
}

iPoint Minimap::ScreenToMinimapToWorld(int x, int y)
{

	iPoint minimap_pos;

	minimap_pos.x = (x - pos.x - width * 0.5F) / scale;
	minimap_pos.y = (y - pos.y) / scale;

	return minimap_pos;
}