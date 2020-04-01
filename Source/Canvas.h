#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "Component.h"
#include "SDL/include/SDL_rect.h"

class C_Canvas;

class UI_Component : public Component
{
public:

	UI_Component(Gameobject* go, UI_Component* parent, ComponentType type = UI_GENERAL);
	virtual ~UI_Component();

	void ComputeOutputRect(float width, float height);

public:

	static C_Canvas* canvas;
	UI_Component* parent;
	SDL_Rect output;

	std::pair<float, float> offset;
	RectF target;
};

class C_Canvas : public UI_Component
{
public:

	C_Canvas(Gameobject* go);
	~C_Canvas();

	void PostUpdate() override;

	void RecieveEvent(const Event& e) override;

	std::pair<float, float> GetScale() const;

public:

	bool playing = false;
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

	int tex_id = -1;
	SDL_Rect section;

private:

	bool mouse_inside = false;
};

#endif // __CANVAS_H__