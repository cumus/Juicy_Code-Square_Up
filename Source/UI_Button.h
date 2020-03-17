#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "UI_Elements.h"

class UI_Button : public UI_Element
{
public:
	UI_Button(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f });
	~UI_Button();

	bool Draw() const override;

	UI_Button* ToUiButton() override;

public:

	SDL_Color color;
};


#endif // !__UI_BUTTON_H__
