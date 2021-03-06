#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "Component.h"
#include "SDL/include/SDL_rect.h"


#include <sstream>

class C_Canvas;

class UI_Component : public Component
{
public:

	UI_Component(Gameobject* go, UI_Component* parent, ComponentType type = UI_GENERAL);
	virtual ~UI_Component();

	void ComputeOutputRect(float width, float height);
	bool PointInsideOutputRect(int x, int y) const;

protected:

	static C_Canvas* canvas;
	UI_Component* parent;
	SDL_Rect output;

public:

	std::pair<float, float> offset;
	RectF target;
};

class C_Canvas : public UI_Component
{
public:

	C_Canvas(Gameobject* go);
	~C_Canvas();

	void PreUpdate() override;
	void PostUpdate() override;

	void RecieveEvent(const Event& e) override;

	static bool MouseOnUI();
	static Gameobject* GameObject();

protected:

	unsigned int hovered_childs;
	bool has_mouse_focus = false;
};

class C_Image : public UI_Component
{
public:

	C_Image(Gameobject* go);
	~C_Image();

	void PostUpdate() override;

public:

	int tex_id = -1;
	SDL_Rect section;
	SDL_Color color;
};

class RenderedText;

class C_Text : public UI_Component
{
public:

	C_Text(Gameobject* go, const char* text = "empty", int font_id = -1);
	~C_Text();

	void PostUpdate() override;

public:

	bool scale_to_fit = false;
	RenderedText* text = nullptr;
};

class C_Button : public UI_Component
{
public:

	C_Button(Gameobject* go, const Event& e);
	~C_Button();

	void PreUpdate() override;
	void PostUpdate() override;

public:

	Event event_triggered;
	bool trigger_while_pressed = false;
	bool clikable = true;
	int tex_id = -1;

	enum ButtonStates : int
	{
		BUTTON_IDLE,
		BUTTON_HOVERED,
		BUTTON_PRESSED,
		BUTTON_PRESSING,

		MAX_BUTTON_STATES
	} state;

	SDL_Rect section[MAX_BUTTON_STATES];
	SDL_Color color;

private:

	bool mouse_inside = false;
};

class C_Slider_Button : public UI_Component
{
public:

	C_Slider_Button(Gameobject* go, float min_x, float max_x, float value, EventType e, EventListener* lis);
	~C_Slider_Button();

	void PreUpdate() override;
	void PostUpdate() override;

public:

	float min_x, max_x;
	float value;

	EventType event_type;
	EventListener* lis;

	bool trigger_while_pressed = false;

	int tex_id = -1;

	enum SliderButtonStates : int
	{
		BUTTON_IDLE,
		BUTTON_HOVERED,
		BUTTON_PRESSED,
		BUTTON_PRESSING,

		MAX_BUTTON_STATES
	} state;

	SDL_Rect section[MAX_BUTTON_STATES];
	SDL_Color color;

private:

	bool mouse_inside = false;
};
#endif // __CANVAS_H__