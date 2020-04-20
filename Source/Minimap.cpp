#include "Minimap.h"
#include "Application.h"
#include "TextureManager.h"
#include "Render.h"
#include "Input.h"
#include "Transform.h"
#include "Behaviour.h"
#include "Log.h"
#include "JuicyMath.h"

#include <string>

Minimap* Minimap::minimap = nullptr;

Minimap::Minimap(Gameobject* go) : UI_Component(go, go->GetUIParent(), UI_MINIMAP)
{
	minimap = this;

	// Setup Minimap
	std::pair<int, int> map_size = Map::GetMapSize_I();
	std::pair<int, int> tile_size = Map::GetTileSize_I();
	std::pair<int, int> total_size = { map_size.first * tile_size.first, map_size.second * tile_size.second };
	minimap_texture = App->render->GetMinimap(total_size.first, total_size.second);

	// Load Icons
	hud_texture = App->tex.Load("Assets/textures/Iconos_square_up.png");
	sections[BACKGROUND]		= { 585, 656, 384, 214 };
	sections[MINIMAP]			= { 0, 0, total_size.first, total_size.second };
	sections[ICON_ALLIED_UNIT]	= { 494, 866, 5, 5 };
	sections[ICON_ENEMY_UNIT]	= { 502, 866, 5, 5 };
	sections[ICON_BASE_CENTER]	= { 519, 858, 8, 13 };
	sections[ICON_TOWER]		= { 510, 862, 6, 9 };
	sections[ICON_BARRACKS]		= { 530, 862, 9, 9 };
	sections[ICON_EDGE]			= { 0, 0, 0, 0 };
	sections[ICON_SPAWNER]		= { 542, 862, 11, 9 };

	// Set UI_Component values
	target = { 1.f, 0.f, 0.03f, 0.03f };
	offset = { -total_size.first, 0 };
}

Minimap::~Minimap()
{
	if (minimap == this)
		minimap = nullptr;
}

void Minimap::PostUpdate()
{
	ComputeOutputRect(float(sections[MINIMAP].w), float(sections[MINIMAP].h));

	// Minimap
	App->render->DrawQuad(output, { 0, 0, 0, 255 }, true, HUD, false);
	std::pair<float, float> scale = { float(output.w) / float(sections[MINIMAP].w), float(output.h) / float(sections[MINIMAP].h) };
	App->render->Blit_Scale(minimap_texture, output.x, output.y, scale.first, scale.second, nullptr, HUD, false);

	// Border
	App->render->Blit_Scale(
		hud_texture,
		output.x, output.y,
		float(output.w) / float(sections[BACKGROUND].w),
		float(output.h) / float(sections[BACKGROUND].h),
		&sections[BACKGROUND], HUD, false);

	// Draw Camera Rect
	RectF cam = App->render->GetCameraRectF();
	SDL_Rect cam_rect = {
		int(float(output.x) + (float(output.w) * 0.5f) + (cam.x * scale.first)),
		int(float(output.y) + (cam.y * scale.second)),
		int(cam.w * scale.first),
		int(cam.h * scale.second) };
	App->render->DrawQuad(cam_rect, { 255, 255, 255, 255 }, false, HUD, false);

	// Move camera
	KeyState mouse = App->input->GetMouseButtonDown(0);
	if (C_Canvas::MouseOnUI() && mouse)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		if (JMath::PointInsideRect(x, y, output))
		{
			mouse_moving = (mouse == KEY_DOWN || (mouse == KEY_REPEAT && mouse_moving));

			if (mouse_moving)
				Event::Push(MINIMAP_MOVE_CAMERA, App->render,
					((float(x - output.x) - (float(output.w) * 0.5f)) / scale.first) - (cam.w / 2.0f),
					(float(y - output.y) / scale.second) - (cam.h / 2.0f));
		}
	}

	// Draw Units
	for (std::map<double, std::pair<MinimapTexture, Transform*>>::const_iterator it = units.cbegin(); it != units.cend(); ++it)
	{
		SDL_Rect icon_section = sections[it->second.first];
		std::pair<float, float> world_pos = Map::F_MapToWorld(it->second.second->GetGlobalPosition());

		App->render->Blit(
			hud_texture,
			output.x + int((float(output.w * 0.5f) + (scale.first * world_pos.first) - (float(icon_section.w) * 0.5f))),
			output.y + int((scale.second * world_pos.second) - (float(icon_section.h) * 0.5f)),
			&icon_section, HUD, false);
	}
}

bool Minimap::AddUnit(double id, int type, Transform* unit)
{
	bool ret = false;
	if (minimap)
	{
		MinimapTexture icon;
		if (type <= UNIT_SPECIAL) icon = ICON_ALLIED_UNIT;
		else if (type <= ENEMY_SPECIAL) icon = ICON_ENEMY_UNIT;
		else
		{
			switch (UnitType(type))
			{
			case BASE_CENTER: { icon = ICON_BASE_CENTER; break; }
			case TOWER: { icon = ICON_TOWER; break; }
			case BARRACKS: { icon = ICON_BARRACKS; break; }
			case EDGE: { icon = ICON_EDGE; break; }
			case SPAWNER: { icon = ICON_SPAWNER; break; }
			}
		}

		minimap->units.insert({ id, { icon, unit } });
		ret = true;
	}

	return ret;
}

void Minimap::RemoveUnit(double id)
{
	if (minimap)
		minimap->units.erase(id);
}