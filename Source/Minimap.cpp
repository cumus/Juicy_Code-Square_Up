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
	scale = ((float)width / (float)map_width);
	height = map_height * scale;
	map_width = 100;
	minimap_camera = { 0, 0, 4, 4 };
	camera_color = { 255, 0, 0, 255 };
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
	//iPoint minimap_camera_position = camera_getter.x, camera_getter.y);


	//minimap_camera.x = minimap_camera_position.x;
	//minimap_camera.y = minimap_camera_position.y;

	App->render->DrawQuad(output, camera_color, false, EDITOR, false);

	App->render->DrawQuad( { camera_getter.x, camera_getter.y, (int)(camera_getter.w * scale), (int)(camera_getter.h * scale) }, { 255,255,255,255 }, false, EDITOR, false);

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
