#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "Canvas.h"
#include "SDL/include/SDL_rect.h"
#include <map>

class Transform;

enum MinimapTexture : int
{
	MINIMAP,
	FOW,
	BORDER,
	ICON_ALLIED_UNIT,
	ICON_ENEMY_UNIT,
	ICON_BASE_CENTER,
	ICON_TOWER,
	ICON_BARRACKS,
	ICON_EDGE,
	ICON_SPAWNER,
	MAX_MINIMAP_TEXTURES
};

class Minimap : public UI_Component
{
public:

	Minimap(Gameobject* go);
	~Minimap();

	void Update() override;
	void PostUpdate() override;

	static Minimap* Get();
	static void SwapTexture(int id);

private:

	inline bool GetSectionIndex(int type, MinimapTexture &index);

public:

	bool draw_units_always = false;

private:

	static Minimap* minimap;
	bool mouse_moving;

	// Map Data
	float map_scale;
	std::pair<int, int> map_size;
	std::pair<int, int> tile_size;
	std::pair<int, int> total_size;
	float minimap_timer, minimap_redraw;

	// Textures
	int fow_texture;
	int hud_texture;
	int border_texture;

	SDL_Rect sections[MAX_MINIMAP_TEXTURES];
};

#endif // !__MINIMAP_H__