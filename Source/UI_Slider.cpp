#include "UI_Slider.h"
#include "Application.h"
#include "Render.h"

#include <sstream>
#include <string.h>


UI_Slider::UI_Slider(EditorWindow* window, RectF rect, RectF button_rect, int texture_id, SDL_Rect section, int font_id)
	: UI_Element(window, SLIDER, rect), bar_texture_id(texture_id), button_rect(button_rect), section(section), font_id(font_id)
{
	target = GetTargetRect();
	targetf = GetTargetNormRect();
	Setvalue();
	
}

UI_Slider::~UI_Slider()
{

}

void UI_Slider::Draw() const
{

	App->render->BlitNorm(bar_texture_id, targetf, apply_section ? &section : nullptr, EDITOR);
 
	App->render->DrawQuadNormCoords({
		button_rect.x * targetf.x,
		button_rect.y * targetf.y,
		button_rect.w * targetf.w,
		button_rect.h * targetf.h }, button_color, true, EDITOR);

	std::stringstream ss;

	char* t;

	ss << value;
    
	std::string temp_str = ss.str();
	
	t = (char*)temp_str.c_str();

	RenderedText* text = new RenderedText(t, font_id);

	if (scale_to_fit)
		App->render->Blit_Text(text, target.x, target.y + target.h * 2, EDITOR);
	else
		App->render->Blit_TextSized(text, {
			target.x,
			target.y + target.h * 2,
			target.w / 5,
			target.h
			}, EDITOR);

	DEL(text);
	
}

void UI_Slider::Setvalue()
{
	value = (int)((button_rect.x + -1.00f) / (targetf.w / rect.w) * 100.0f);

	if (value <= 0)
		value = 0;
	else if (value >= 100)
	    value = 100;
}

UI_Slider* UI_Slider::ToUiSlider()
{
	return this;
}