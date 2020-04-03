#ifndef __MINIMAPWINDOW_H__
#define __MINIMAPWINDOW_H__

#include "EditorWindow.h"
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

class MinimapWindow :public EditorWindow
{
public:

	MinimapWindow(const RectF rect);
	~MinimapWindow();

	bool Init() override;

private:

	void _Update() override;
	bool Awake(pugi::xml_node& config);

	bool CreateMinimap();
	iPoint WorldToMinimap(int x, int y);
	iPoint ScreenToMinimapToWorld(int x, int y);

private:

	SDL_Texture* map_texture;
	iPoint pos;
	Corner corner;
	SDL_Rect minimap_camera;
	Timer ptimer;

	int width;
	int height;
	float scale;
	int map_width;
	int map_height;
	int margin;
};
#endif // !__MINIMAPWINDOW_H__
