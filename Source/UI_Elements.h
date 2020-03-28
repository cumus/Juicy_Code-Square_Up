#ifndef __UI_ELEMENTS_H__
#define __UI_ELEMENTS_H__

#include "EventListener.h"
#include "SDL/include/SDL_rect.h"

enum UI_Type
{
	BUTTON,
	BUTTON_IMAGE,
	IMAGE,
	TEXT,
	TEXT_BUTTON,
	SLIDER,
	SUB_MENU,

	MAX_UI_ELEMENTS
};

class EditorWindow;
class UI_Button;
class UI_Image;
class UI_Text;
class UI_TextButton;
class UI_ButtonImage;
class UI_SubMenu;
class UI_Slider;

class UI_Element : public EventListener
{
public:
	UI_Element(EditorWindow* window, UI_Type type, RectF rect = { 0.0f, 0.0f, 0.0f, 0.0f });
	virtual ~UI_Element();

	virtual void Draw() const;

	virtual UI_Button* ToUiButton() { return nullptr; }
	virtual UI_Image* ToUiImage() { return nullptr; }
	virtual UI_Text* ToUiText() { return nullptr; }
	virtual UI_TextButton* ToUiTextButton() { return nullptr; }
	virtual UI_ButtonImage* ToUiButtonImage() { return nullptr; }
	virtual UI_SubMenu* ToUiSubMenu() { return nullptr; }
	virtual UI_Slider* ToUiSlider() { return nullptr; }

	RectF GetTargetNormRect() const;
	SDL_Rect GetTargetRect() const;

protected:

	EditorWindow* window = nullptr;

public:

	RectF rect;
	bool mouse_inside = false;

private:

	UI_Type	type;
};


#endif // __UI_ELEMENTS_H__
