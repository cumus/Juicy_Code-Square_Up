#ifndef _UI_TEXTBUTTON_H_
#define _UI_TEXTBUTTON_H_

#include "UI_Elements.h"


class UI_TextButton : public UI_Element
{
public:
	UI_TextButton(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, int font_id = -1, const char* text = nullptr);
	~UI_TextButton();

	bool Draw() const override;
	
	UI_TextButton* ToUiTextButton() override;

public:

	SDL_Color color;
	const char* text;
	int font_id;

};
#endif // !_UI_TEXT_H_
