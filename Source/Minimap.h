#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "Canvas.h"
#include "Point.h"
#include "Map.h"
#include "SDL/include/SDL.h"
#include "TimeManager.h"
#include "MapContainer.h"

struct _Point
{
	SDL_Color	color;
	SDL_Rect	rect;
};

struct _Sprite
{
	SDL_Surface*	sprite_img;
	SDL_Rect		section;
};

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

	void AddToMinimap(SDL_Rect rect, SDL_Color color);
	void Draw_Sprite(SDL_Surface* img, int x, int y);

private:

	void Update() override;

private:

	SDL_Surface*	base_image;
	SDL_Renderer*	renderer;
	SDL_Texture*	map_texture;
	SDL_Color		camera_color;
	SDL_Rect		minimap_camera;
	Map				map;

	SDL_Rect		ally;
	SDL_Rect		enemy;
	SDL_Rect		building;

	int		minimap_width;
	int		minimap_height;
	int		window_width;
	int		window_height;
	bool	mouse_inside;

public:

	std::list<_Point> point_queue;
	std::list<_Sprite> sprite_queue;
};


#endif // !__MINIMAP_H__