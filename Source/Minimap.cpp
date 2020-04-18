#include "Minimap.h"
#include "Application.h"
#include "TextureManager.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "Editor.h"
#include "Map.h"
#include "Input.h"
#include "Log.h"
#include "JuicyMath.h"
#include "Transform.h"
#include "Behaviour.h"

#include <string>

Minimap::Minimap(Gameobject* go) :
	C_Image(go)
{
	LOG("Starting minimap");

	background_rect = { 585, 656, 384, 216 };
	minimap_camera = { 0, 0, 0, 0 };
	camera_color = { 255, 255, 255, 255 };
	unit_color = {0,0,255,255};
	minimapSel = false;
	background_tex = App->tex.Load("Assets/textures/Iconos_square_up.png");

	App->win->GetWindowSize(window_width, window_height);

	target = { 1.f, 0.f, 0.03f, 0.03f };
	std::pair<int, int> map_size = Map::GetMapSize_I();
	std::pair<int, int> tile_size = Map::GetTileSize_I();
	section = { 0, 0, map_size.first * tile_size.first, map_size.second * tile_size.second };
	offset = { -section.w, 0 };
	tex_id = App->render->GetMinimap(section.w, section.h);
	Event::Push(UPDATE_MINIMAP_TEXTURE, App->render);
}

Minimap::~Minimap()
{
}

void Minimap::Update()
{
	SDL_Rect camera_getter = App->render->GetCameraRect();
	background_output = { (float)output.x, (float)output.y, (float)output.w, (float)output.h };

	if (output.w != 0 && output.h != 0)
	{
		scalex = (background_rect.w / background_output.w) * (background_output.w / background_rect.w) * (background_output.w / background_rect.w);
		scaley = (background_rect.h / background_output.h) * (background_output.h / background_rect.h) * (background_output.h / background_rect.h);
	}


	App->render->Blit_Scale(background_tex, output.x, output.y, scalex, scaley, &background_rect, HUD, false);

	App->render->DrawQuad(output, { 255, 0, 0, 255 }, false, EDITOR, false);


	//-------------------------------------------------------------------------------------
		//From map to minimap viewport
	std::pair<float, float> tile_size = Map::GetTileSize_F();
	std::pair<float, float> map_size = Map::GetMapSize_F();

	float map_pixelwidth = map_size.first * tile_size.first;
	float map_pixelheight = map_size.second * tile_size.second;

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

	//-------------------------------------------------------------------------------------

		//From minimap to map drag viewport
	if (App->input->GetMouseButtonDown(0) == KeyState::KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		if (mouse_inside = JMath::PointInsideRect(x, y, output))
		{
			minimapSel = true;
		}
	}

	if (App->input->GetMouseButtonDown(0) == KeyState::KEY_REPEAT && minimapSel)
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

	if (App->input->GetMouseButtonDown(0) == KeyState::KEY_UP && minimapSel) minimapSel = false;

	//-------------------------------------------------------------------------------------

		//Units tu minimap (with coloured rect)
		/*for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); it++)
		{
			if (it->second->AsBehaviour()->GetType() == UNIT_MELEE || it->second->AsBehaviour()->GetType() == GATHERER)
			{
				LOG("Show unit");
				SDL_Rect representation;
				vec unit_pos = it->second->GetGameobject()->GetTransform()->GetGlobalPosition();
				std::pair<float, float> world_pos = map.F_MapToWorld(unit_pos);

				representation.x = scale_x * world_pos.first - 1;
				representation.y = scale_y * world_pos.second - 1;
				representation.w = 2;
				representation.h = 2;

				representation.x += output.x + output.w / 2;
				representation.y += output.y;

				App->render->DrawQuad(representation, unit_color, true, EDITOR, false);

			}
		}*/

	for (std::list<Gameobject*>::iterator it = object_queue.begin(); it != object_queue.end(); it++)
	{

		SDL_Rect representation;
		vec unit_pos = (*it)->GetTransform()->GetGlobalPosition();
		std::pair<float, float> world_pos = map.F_MapToWorld(unit_pos);

		representation.x = scale_x * world_pos.first - 1;
		representation.y = scale_y * world_pos.second - 1;
		representation.w = 2;
		representation.h = 2;

		representation.x += output.x + output.w / 2;
		representation.y += output.y;
		App->render->DrawQuad(representation, {0,255,0,255}, true, EDITOR, false);
	}

}

void Minimap::AddToMinimap(Gameobject* object, SDL_Color color)
{
	//unit_color = color;
	object_queue.push_back(object);
}