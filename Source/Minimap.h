#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "Canvas.h"
#include "Point.h"
#include "Map.h"
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

class Minimap :public C_Image
{
public:

	Minimap(Gameobject* go);
	~Minimap();

	bool CreateMinimap();

private:

	void Update() override;
	bool Awake(pugi::xml_node& config);

	iPoint WorldToMinimap(int x, int y);
	iPoint ScreenToMinimapToWorld(int x, int y);

private:

	SDL_Renderer*	renderer;
	SDL_Texture*	map_texture;
	iPoint			pos;
	Corner			corner;
	SDL_Rect		minimap_camera;
	SDL_Color		camera_color;
	Timer			ptimer;
	Map				map;

	int		width;
	int		height;
	int		map_width;
	int		map_height;
	int		margin;
	int		window_width;
	int		window_height;
	float	scale;
};
#endif // !__MINIMAP_H__