#ifndef _UI_TEXTBUTTON_H_
#define _UI_TEXTBUTTON_H_

#include "UI_Elements.h"

class RenderedText;

class UI_TextButton : public UI_Element
{
public:
	UI_TextButton(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, const char* text = nullptr, int font_id = -1);
	~UI_TextButton();

	bool Draw() const override;
	
	UI_TextButton* ToUiTextButton() override;

public:

	SDL_Color color;
	bool scale_to_fit = false;
	RenderedText* text = nullptr;

};
#endif // !_UI_TEXT_H_
