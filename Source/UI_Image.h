#ifndef _UI_IMAGE_H_
#define _UI_IMAGE_H_

#include "UI_Elements.h"

class UI_Image : public UI_Element
{
public:
	UI_Image(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, int texture_id = -1, SDL_Rect section = { 0, 0, 0, 0 });
	~UI_Image();

	bool Draw() const override;

public:

	bool apply_section = false;
	SDL_Rect section;
	int texture_id;
};

#endif // !_UI_IMAGE_H_
