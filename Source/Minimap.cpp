#include "Minimap.h"
#include "Application.h"
#include "TextureManager.h"
#include "Render.h"
#include "Input.h"
#include "Transform.h"
#include "Behaviour.h"
#include "Sprite.h"
#include "Log.h"
#include "JuicyMath.h"

#include <string>

Minimap* Minimap::minimap = nullptr;

Minimap::Minimap(Gameobject* go) : UI_Component(go, go->GetUIParent(), UI_MINIMAP)
{
	minimap = this;
	mouse_moving = false;
	minimap_timer = 0.0f;
	minimap_redraw = 1.0f;

	// Load Map Data
	map_scale = 0.5f;
	map_size = Map::GetMapSize_I();
    tile_size = Map::GetTileSize_I();
	total_size = { int(float(map_size.first * tile_size.first) * map_scale), int(float(map_size.second * tile_size.second) * map_scale) };

	// Set UI_Component values
	target = { 1.f, 1.f, 0.035f, 0.035f };
	offset = { -total_size.first, -total_size.second };

	// Setup FoW Texture
	fow_texture = App->render->GetMinimap(total_size.first, total_size.second, map_scale);
	sections[FOW] = { 0, 0, total_size.first, total_size.second };

	// Set Border & Sections
	hud_texture = App->tex.Load("Assets/textures/minimap.png");
	sections[MINIMAP] = { 0, 0, 1338, 668 };
	border_texture = App->tex.Load("Assets/textures/hud-sprites.png");
	sections[BACKGROUND] = { 647, 733, 360, 181 };

	// Load Icons
	sections[ICON_ALLIED_UNIT]	= { 325, 81, 5, 5 };
	sections[ICON_ENEMY_UNIT]	= { 331, 866, 5, 5 };
	sections[ICON_BASE_CENTER]	= { 344, 73, 8, 13 };
	sections[ICON_TOWER]		= { 337, 77, 6, 9 };
	sections[ICON_BARRACKS]		= { 353, 77, 9, 9 };
	sections[ICON_EDGE]			= { 0, 0, 0, 0 };
	sections[ICON_SPAWNER]		= { 376, 75, 11, 10 };
}

Minimap::~Minimap()
{
	if (minimap == this)
		minimap = nullptr;
}

void Minimap::Update()
{
	if ((minimap_timer += App->time.GetGameDeltaTime()) > minimap_redraw)
	{
		minimap_timer = 0.0f;
		Event::Push(MINIMAP_UPDATE_TEXTURE, App->render);
	}
}

void Minimap::PostUpdate()
{
	ComputeOutputRect(float(sections[FOW].w), float(sections[FOW].h));

	// Minimap
	std::pair<float, float> scale = { float(output.w) / float(sections[MINIMAP].w), float(output.h) / float(sections[MINIMAP].h) };
	App->render->Blit_Scale(hud_texture, output.x, output.y, scale.first, scale.second, nullptr, HUD, false);

	// FoW
	scale = { float(output.w) / float(sections[FOW].w), float(output.h) / float(sections[FOW].h) };
	App->render->Blit_Scale(fow_texture, output.x, output.y, scale.first, scale.second, nullptr, HUD, false);

	scale.first *= map_scale;
	scale.second *= map_scale;

	// Border
	App->render->Blit_Scale(
		border_texture,
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
			border_texture,
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
		if (!Behaviour::IsHidden(id))
		{
			MinimapTexture icon;
			if (type <= UNIT_SUPER) icon = ICON_ALLIED_UNIT;
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
	}

	return ret;
}

void Minimap::RemoveUnit(double id)
{
	if (minimap)
		minimap->units.erase(id);
}