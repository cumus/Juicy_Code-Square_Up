#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "Module.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "TimeManager.h"
#include "MapContainer.h"

enum class Corner
{
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

class Minimap :public Module
{
public:

	Minimap();
	~Minimap();

	bool Start() override;

	bool CreateMinimap();

private:

	bool PostUpdate() override;
	bool Awake(pugi::xml_node& config);

	iPoint WorldToMinimap(int x, int y);
	iPoint ScreenToMinimapToWorld(int x, int y);

private:

	SDL_Texture*	map_texture;
	iPoint			pos;
	Corner			corner;
	SDL_Rect		minimap_camera;
	Timer			ptimer;
	SDL_Renderer*	renderer;

	int		width;
	int		height;
	float	scale;
	int		map_width;
	int		map_height;
	int		margin;
};
#endif // !__MINIMAP_H__