#ifndef _UI_TEXT_H_
#define _UI_TEXT_H_

#include "UI_Elements.h"

#include <string>

class UI_Text : public UI_Element
{
public:
	UI_Text(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, const char* text = nullptr, int font_id = -1);
	~UI_Text();

	bool Draw() const override;

	UI_Text* ToUiText() override;

public:

	std::string text;
	int font_id;
};
#endif // !_UI_TEXT_H_