#include "Minimap.h"
#include "Application.h"
#include "TextureManager.h"
#include "MeleeUnit.h"
#include "EnemyMeleeUnit.h"
#include "UI_Image.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "Editor.h"
#include "Map.h"
#include "Input.h"
#include "Log.h"
#include "JuicyMath.h"

#include <string>

Minimap::Minimap(Gameobject* go) :
	C_Image(go)
{
	LOG("Starting minimap");

	//renderer = App->render->GetSDLRenderer();
	//map_texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(App->win->GetWindow()), SDL_TEXTUREACCESS_TARGET, 1.05F * width, 1.05F * height);

	ally = { 0, 0, 0, 0 };
	enemy = { 0, 0, 0, 0 };
	building = { 0, 0, 0, 0 };

	minimap_camera = { 0, 0, 4, 4 };
	camera_color = { 255, 255, 255, 255 };
	tex_id = App->tex.Load("textures/darkimg.png");

	App->win->GetWindowSize(window_width, window_height);

	//SDL_SetRenderTarget(renderer, map_texture);
	CreateMinimap();
	//SDL_SetRenderTarget(renderer, NULL);
}

Minimap::~Minimap()
{
}

void Minimap::Update()
{
	//App->render->Blit((int)map_texture, pos.x, pos.y, NULL);

	SDL_Rect camera_getter = App->render->GetCameraRect();

	App->render->DrawQuad(output, { 255, 0, 0, 255 }, false, EDITOR, false);

	//App->render->DrawQuad( { camera_getter.x, camera_getter.y, (int)(camera_getter.w * scale), (int)(camera_getter.h * scale) }, { 255,255,255,255 }, false, EDITOR, false);

	//From camera to minimap camera (SDL_Rect)
	float tile_width, tile_height;
	map.GetTileSize_F(tile_width, tile_height);

	float map_width, map_height;
	map.GetMapSize(map_width, map_height);

	float map_pixelwidth = map_width * tile_width;
	float map_pixelheight = map_height * tile_height;

	float scale_x, scale_y;
	scale_x = output.w / map_pixelwidth;
	scale_y = output.h / map_pixelheight;

	minimap_camera.x = camera_getter.x;
	minimap_camera.y = camera_getter.y;
	minimap_camera.w = camera_getter.w * scale_x;
	minimap_camera.h = camera_getter.h * scale_y;
				 
	minimap_camera.x = minimap_camera.x * scale_x;
	minimap_camera.y = minimap_camera.y * scale_y;

	minimap_camera.x += output.x + output.w / 2;
	minimap_camera.y += output.y;

	App->render->DrawQuad(minimap_camera, camera_color, false, EDITOR, false);
//-------------------------------------------------------------------------

	if (App->input->GetMouseButtonDown(0) == KeyState::KEY_REPEAT)
	{
		int x, y;
		App->input->GetMousePosition(x, y);

		if (mouse_inside = JMath::PointInsideRect(x, y, output))
		{
			x -= output.x;
			y -= output.y;

			App->render->cam.x = (x / scale_x) - map_pixelwidth / 2 - App->render->cam.w / 2;
			App->render->cam.y = y / scale_y - App->render->cam.h / 2;
		}
	}


}

bool Minimap::CreateMinimap()
{
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
