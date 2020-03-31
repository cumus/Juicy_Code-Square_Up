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

public:

	static C_Canvas* canvas;
	UI_Component* parent;
	SDL_Rect output;

	float offset_x, offset_y;
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

private:

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

#endif // __CANVAS_H__