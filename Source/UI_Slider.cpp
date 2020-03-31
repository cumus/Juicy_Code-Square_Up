#include "UI_Slider.h"
#include "Application.h"
#include "Render.h"
#include "FontManager.h"

#include <sstream>
#include <string.h>


UI_Slider::UI_Slider(EditorWindow* window, RectF rect, RectF button_rect, int texture_id, SDL_Rect section, int font_id)
	: UI_Element(window, SLIDER, rect), bar_texture_id(texture_id), button_rect(button_rect), section(section), font_id(font_id)
{
	target = GetTargetRect();
	targetf = GetTargetNormRect();
	Set_Value();
	text = new RenderedText("  ", font_id);
}

UI_Slider::~UI_Slider()
{
  DEL(text)
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

	ss << value;

	std::string temp_str = ss.str();

	const char* t = (char*)temp_str.c_str();

	text->SetText(t);

	if (scale_to_fit)
		App->render->Blit_Text(text, target.x, target.y + target.h * 2, EDITOR);
	else
		App->render->Blit_TextSized(text, {
			target.x,
			target.y + target.h * 2,
			target.w / 5,
			target.h
			}, EDITOR);
}

void UI_Slider::Set_Value()
{
	value = (int)((button_rect.x - 1.00f) * targetf.x / (targetf.w) * 100.00f) + 1.00f;

	if (value <= 0)
		value = 0;
	else if (value >= 100)
	    value = 100;
}

void UI_Slider::Set_Button(int v)
{
	if (v <= 0)
		v = 0;
	else if (value >= 100)
		v = 100;

	value = v;
	
	button_rect.x = (value - 1.00f) * (targetf.w) / (targetf.x * 100.0f) + 1.00f;
}

UI_Slider* UI_Slider::ToUiSlider()
{
	return this;
}