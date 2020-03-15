#ifndef __EDITOR_WINDOWS_H__
#define __EDITOR_WINDOWS_H__

#include "EventListener.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_pixels.h"

enum Window_Sides
{
	SIDE_NONE,
	SIDE_N,
	SIDE_W,
	SIDE_E,
	SIDE_S,
	CORNER_NW,
	CORNER_NE,
	CORNER_SW,
	CORNER_SE
};

enum KeyState;
class UI_Element;

class EditorWindow : public EventListener
{
public:

	EditorWindow(const RectF rect, SDL_Color color = { 250, 250, 250, 220 });
	virtual ~EditorWindow();

	bool CheckIfEditing(float mouse_x, float mouse_y, KeyState mouse_left_button);
	bool Update(float mouse_x, float mouse_y, KeyState mouse_left_button);
	void Draw(bool draw_border = false) const;
	void DrawBorders() const;

	RectF GetBorderN_Norm() const;
	RectF GetBorderW_Norm() const;
	RectF GetBorderE_Norm() const;
	RectF GetBorderS_Norm() const;

	virtual void DrawContent() const {}

private:

	inline void MouseDrag_N(float mouse_x, float mouse_y);
	inline void MouseDrag_W(float mouse_x, float mouse_y);
	inline void MouseDrag_E(float mouse_x, float mouse_y);
	inline void MouseDrag_S(float mouse_x, float mouse_y);

public:

	RectF rect;
	SDL_Color color;

	bool mouse_inside = false;
	Window_Sides hovering = SIDE_NONE;
	bool dragging = false;

	static float margin;
	static float min_size;

	std::vector<UI_Element*> elements;
};

class BarMenu : public EditorWindow
{
public:

	BarMenu(const RectF rect) : EditorWindow(rect) {}
	~BarMenu() {}

	void DrawContent() const override;
};

class PlayPauseWindow : public EditorWindow
{
public:

	PlayPauseWindow(const RectF rect) : EditorWindow(rect) {}
	~PlayPauseWindow() {}

	void DrawContent() const override;
};

class HeriarchyWindow : public EditorWindow
{
public:

	HeriarchyWindow(const RectF rect) : EditorWindow(rect) {}
	~HeriarchyWindow() {}

	void DrawContent() const override;
};

class PropertiesWindow : public EditorWindow
{
public:

	PropertiesWindow(const RectF rect) : EditorWindow(rect) {}
	~PropertiesWindow() {}

	void DrawContent() const override;
};

class ConsoleWindow : public EditorWindow
{
public:

	ConsoleWindow(const RectF rect) : EditorWindow(rect) {}
	~ConsoleWindow() {}

	void DrawContent() const override;
};

class ConfigWindow : public EditorWindow
{
public:

	ConfigWindow(const RectF rect) : EditorWindow(rect) {}
	~ConfigWindow() {}

	void DrawContent() const override;
};

#endif // __EDITOR_WINDOWS_H__