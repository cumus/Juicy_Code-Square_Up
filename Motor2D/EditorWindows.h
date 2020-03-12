#ifndef __EDITOR_WINDOWS_H__
#define __EDITOR_WINDOWS_H__

#include "UI_Elements.h"
#include "SDL/include/SDL_rect.h"

enum KeyState;

enum Window_Sides
{
	NONE,
	SIDE_N,
	SIDE_W,
	SIDE_E,
	SIDE_S,
	CORNER_NW,
	CORNER_NE,
	CORNER_SW,
	CORNER_SE
};

class EditorWindow : public EventListener
{
public:

	EditorWindow(float x, float y, float w, float h);
	virtual ~EditorWindow() {}

	bool Update(float mouse_x, float mouse_y, KeyState mouse_left_button, bool sizing = false);
	void Draw(float width, float height, bool draw_border = false) const;
	void DrawBorders(float width, float height, float margin = 0.002f) const;

	SDL_Rect GetBorderN(float width, float height, float margin) const;
	SDL_Rect GetBorderW(float width, float height, float margin) const;
	SDL_Rect GetBorderE(float width, float height, float margin) const;
	SDL_Rect GetBorderS(float width, float height, float margin) const;

	RectF GetBorderN_Normalized(float margin) const;
	RectF GetBorderW_Normalized(float margin) const;
	RectF GetBorderE_Normalized(float margin) const;
	RectF GetBorderS_Normalized(float margin) const;

	virtual void DrawContent(SDL_Rect area) const {}

	static bool InsideRect(std::pair<float, float> p, RectF rect);

public:

	float x, y, w, h;
	unsigned int r, g, b, a;

	bool mouse_inside = false;
	Window_Sides hovering = NONE;
	bool dragging = false;

	std::vector<UI_Element*> elements;
};

class BarMenu : public EditorWindow
{
public:

	BarMenu(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~BarMenu() {}

	void DrawContent(SDL_Rect area) const override;
};

class PlayPauseWindow : public EditorWindow
{
public:

	PlayPauseWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~PlayPauseWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class HeriarchyWindow : public EditorWindow
{
public:

	HeriarchyWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~HeriarchyWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class PropertiesWindow : public EditorWindow
{
public:

	PropertiesWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~PropertiesWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class ConsoleWindow : public EditorWindow
{
public:

	ConsoleWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~ConsoleWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class ConfigWindow : public EditorWindow
{
public:

	ConfigWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~ConfigWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

#endif // __EDITOR_WINDOWS_H__