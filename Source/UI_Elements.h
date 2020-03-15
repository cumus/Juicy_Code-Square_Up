#ifndef __UI_ELEMENTS_H__
#define __UI_ELEMENTS_H__

#include "EventListener.h"
#include "Point.h"
#include "SDL/include/SDL.h"

class Module;

enum FocusEvent {
	FOCUS_IN,
	FOCUS_OUT,
	FOCUSED,
	CLICKED
};

enum UI_Type
{
	BUTTON,
	IMAGE,
	TEXT,
	TEXT_BUTTON,
	SLIDER,
	MAX_UI_ELEMENTS
};

class UI_Element : public EventListener
{
public:
	UI_Element() {};
	~UI_Element() {};

	virtual void Init() {}
	virtual bool Input() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool CleanUp() { return true; };

	virtual bool Draw() { return true; }
	virtual void HandleFocusEvent(FocusEvent event) {}

	SDL_Rect GetScreenRect() const;
	SDL_Rect GetLocalRect() const;
	iPoint GetScreenPos() const;
	iPoint GetLocalPos() const;
	void SetLocalPos(iPoint new_pos);
	bool OnHover();

public:
	iPoint	localPos;
	iPoint	screenPos;
	UI_Type	type;
	FocusEvent focus_event;
	Module* callback = nullptr;
	UI_Element* parent = nullptr;
	SDL_Rect rect;
	SDL_Texture* texture = nullptr;

	bool draggable;
	bool interactable;
	bool isStatic;
	bool to_delete;
};


#endif // __UI_ELEMENTS_H__
