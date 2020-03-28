#include "Application.h"
#include "UI_Image.h"
#include "Render.h"
#include "TextureManager.h"

UI_Image::UI_Image(EditorWindow* window, RectF rect, int texture_id, SDL_Rect section)
	: UI_Element(window, IMAGE, rect), texture_id(texture_id), section(section)
{}

UI_Image::~UI_Image()
{}

void UI_Image::Draw() const
{
	App->render->BlitNorm(texture_id, GetTargetNormRect(), apply_section ? &section : nullptr);
}