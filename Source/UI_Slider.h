#ifndef __UI_SLIDER_H__
#define __UI_SLIDER_H__

#include "UI_Elements.h"

class UI_Slider : public UI_Element
{
public:
	UI_Slider(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, RectF button_rect = { 0.0f, 0.0f, 1.0f, 1.0f }, int texture_id = -1, SDL_Rect section = { 0, 0, 0, 0 });
	~UI_Slider();

	void Draw() const override;

	UI_Slider* ToUiSlider() override;

public:
	
	int bar_texture_id;
	RectF button_rect;
	SDL_Color button_color;
	SDL_Rect section;
	bool apply_section = false;

};


#endif // !__UI_BUTTON_H__