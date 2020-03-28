#include "UI_Slider.h"
#include "Application.h"
#include "Render.h"

UI_Slider::UI_Slider(EditorWindow* window, RectF rect, RectF button_rect, int texture_id, SDL_Rect section)
	: UI_Element(window, SLIDER, rect), bar_texture_id(texture_id), button_rect(button_rect), section(section)
{}

UI_Slider::~UI_Slider()
{}

void UI_Slider::Draw() const
{
	App->render->BlitNorm(bar_texture_id, GetTargetNormRect(), apply_section ? &section : nullptr);

	RectF target = GetTargetNormRect();
	App->render->DrawQuadNormCoords({
		button_rect.x * target.x,
		button_rect.y * target.y,
		button_rect.w * target.w,
		button_rect.h * target.h }, button_color);
}

UI_Slider* UI_Slider::ToUiSlider()
{
	return this;
}