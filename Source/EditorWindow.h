#ifndef __EDITOR_WINDOW_H__
#define __EDITOR_WINDOW_H__

#include "EventListener.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_pixels.h"

enum WindowPos
{
	INSIDE,
	OUTSIDE,
	SIDE_N,
	SIDE_W,
	SIDE_E,
	SIDE_S,
	CORNER_NW,
	CORNER_NE,
	CORNER_SW,
	CORNER_SE
};

struct WindowState
{
	bool mouse_inside = false;
	WindowPos hovering = OUTSIDE;
	bool dragging = false;
};

enum KeyState;
class UI_Element;

class EditorWindow : public EventListener
{
public:

	EditorWindow(const RectF rect, SDL_Color color = { 250, 250, 250, 220 });
	virtual ~EditorWindow();

	virtual bool Init() { return true; }
	virtual void CleanUp();

	const WindowState Update(float mouse_x, float mouse_y, KeyState mouse_left_button, bool sizing);

	void Draw(bool draw_border = false) const;
	void DrawBorders() const;

	RectF GetBorderN_Norm() const;
	RectF GetBorderW_Norm() const;
	RectF GetBorderE_Norm() const;
	RectF GetBorderS_Norm() const;

private:

	virtual void _Update() {}

	inline void MouseDrag_N(float mouse_x, float mouse_y);
	inline void MouseDrag_W(float mouse_x, float mouse_y);
	inline void MouseDrag_E(float mouse_x, float mouse_y);
	inline void MouseDrag_S(float mouse_x, float mouse_y);

public:

	RectF rect;
	SDL_Color color;

	static float margin;
	static float min_size;

protected:

	WindowState state;
	std::vector<UI_Element*> elements;
};

#endif // __EDITOR_WINDOW_H__