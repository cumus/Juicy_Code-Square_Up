#ifndef __UI_ELEMENTS_H__
#define __UI_ELEMENTS_H__

#include "EventListener.h"
#include "SDL/include/SDL_rect.h"

enum UI_Type
{
	BUTTON,
	IMAGE,
	TEXT,
	TEXT_BUTTON,
	SLIDER,
	MAX_UI_ELEMENTS
};

class EditorWindow;

class UI_Element : public EventListener
{
public:
	UI_Element(EditorWindow* window, UI_Type type, RectF rect = { 0.0f, 0.0f, 0.0f, 0.0f });
	virtual ~UI_Element();

	virtual bool Draw() const;

	RectF GetTargetNormRect() const;

protected:

	EditorWindow* window = nullptr;

public:

	RectF rect;

private:

	UI_Type	type;
};


#endif // __UI_ELEMENTS_H__
