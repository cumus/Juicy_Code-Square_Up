#ifndef _UI_TEXT_H_
#define _UI_TEXT_H_

#include "UI_Elements.h"


class UI_Text : public UI_Element
{
public:
	UI_Text(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, int font_id = -1, const char* text = nullptr);
	~UI_Text();

	bool Draw() const override;

public:

	const char* text;
	int font_id;
};
#endif // !_UI_TEXT_H_