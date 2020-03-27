#include "UI_Slider.h"
#include "Application.h"
#include "Render.h"

UI_Slider::UI_Slider(EditorWindow* window, RectF rect, RectF button_rect, int texture_id, SDL_Rect section)
	: UI_Element(window, SLIDER, rect), bar_texture_id(texture_id), button_rect(button_rect), section(section)
{}

UI_Slider::~UI_Slider()
{}

bool UI_Slider::Draw() const
{
	bool ret = true;

	if (ret = App->render->BlitNorm(bar_texture_id, GetTargetNormRect(), apply_section ? &section : nullptr))
	{
		float x = button_rect.x * GetTargetNormRect().x;
		float y = button_rect.y * GetTargetNormRect().y;
		float w = button_rect.w * GetTargetNormRect().w;
		float h = button_rect.h * GetTargetNormRect().h;

		App->render->DrawQuadNormCoords({ x, y, w, h }, button_color);
	}

	return ret;
}

UI_Slider* UI_Slider::ToUiSlider()
{
	return this;
}