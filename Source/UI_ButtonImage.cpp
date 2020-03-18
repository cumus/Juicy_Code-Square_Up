#include "UI_ButtonImage.h"
#include "Application.h"
#include "TextureManager.h"
#include "Render.h"

UI_ButtonImage::UI_ButtonImage(EditorWindow* window, RectF rect, int texture_id, SDL_Rect section)
	: UI_Element(window, BUTTON_IMAGE, rect), section(section)
{}

UI_ButtonImage::~UI_ButtonImage()
{}

bool UI_ButtonImage::Draw() const
{
	return App->render->BlitNorm(texture_id, GetTargetNormRect(), apply_section ? &section : nullptr);
}

UI_ButtonImage* UI_ButtonImage::ToUiButtonImage()
{
	return this;
}
