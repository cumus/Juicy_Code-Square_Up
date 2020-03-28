#ifndef _UI_TEXT_H_
#define _UI_TEXT_H_

#include "UI_Elements.h"

#include <string>

class RenderedText;

class UI_Text : public UI_Element
{
public:
	UI_Text(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, const char* text = nullptr, int font_id = -1);
	~UI_Text();

	void Draw() const override;

	UI_Text* ToUiText() override;

public:

	bool scale_to_fit = false;
	RenderedText* text = nullptr;
};
#endif // !_UI_TEXT_H_