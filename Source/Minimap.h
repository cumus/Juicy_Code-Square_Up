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

	Map		map;

	SDL_Color	camera_color;
	SDL_Color	unit_color;
	SDL_Rect	minimap_camera;
	SDL_Rect	background_rect;
	RectF		background_ouput;

	int		background_tex;
	int		window_width;
	int		window_height;
	bool	mouse_inside;
	bool	map_charged;

public:

	std::list<Gameobject*> object_queue;
};


#endif // !__MINIMAP_H__