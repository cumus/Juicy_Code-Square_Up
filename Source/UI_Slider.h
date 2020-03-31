#ifndef __UI_SLIDER_H__
#define __UI_SLIDER_H__

#include "UI_Elements.h"

class RenderedText;

class UI_Slider : public UI_Element
{
public:
	UI_Slider(EditorWindow* window, RectF rect = { 0.0f, 0.0f, 1.0f, 1.0f }, RectF button_rect = { 0.0f, 0.0f, 1.0f, 1.0f }, int texture_id = -1, SDL_Rect section = { 0, 0, 0, 0 }, int font_id = -1);
	~UI_Slider();

	void Draw() const override;

	void Set_Value();
	void Set_Button(int value);

	UI_Slider* ToUiSlider() override;


public:
	
	RectF button_rect;
	int bar_texture_id;
	SDL_Color button_color;
	SDL_Rect section;
	bool apply_section = false;
	int font_id;
	SDL_Rect target;
	RectF targetf;
	float value;
	bool scale_to_fit = false;
	RenderedText* text = nullptr;

};


#endif // !__UI_BUTTON_H__