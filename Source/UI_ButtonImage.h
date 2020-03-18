#ifndef __UI_BUTTONIMAGE_H__
#define __UI_BUTTONIMAGE_H__

#include "UI_Elements.h"

class UI_ButtonImage : public UI_Element
{
public:
	UI_ButtonImage(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, int texture_id = -1, SDL_Rect section = { 0, 0, 0, 0 });
	~UI_ButtonImage();

	bool Draw() const override;

	UI_ButtonImage* ToUiButtonImage() override;

public:

	bool apply_section = false;
	SDL_Rect section;
	int texture_id;

};

#endif // !__UI_BUTTONIMAGE_H__