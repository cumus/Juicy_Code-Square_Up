#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "Canvas.h"
#include "Point.h"
#include "Map.h"
#include "SDL/include/SDL.h"
#include "TimeManager.h"
#include "Map.h"

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

	void AddToMinimap(Gameobject* object, SDL_Color color);

private:

	void Update() override;

private:

	SDL_Surface*	base_image;
	SDL_Renderer*	renderer;
	SDL_Texture*	map_texture;
	SDL_Color		camera_color;
	SDL_Color		unit_color;
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
	bool	map_charged;

public:

	std::list<Gameobject*> object_queue;
};


#endif // !__MINIMAP_H__