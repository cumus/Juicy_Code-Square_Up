#include "UI_Button.h"
#include "Application.h"
#include "Render.h"

UI_Button::UI_Button(EditorWindow* window, RectF rect)
	: UI_Element(window, BUTTON, rect)
{}

UI_Button::~UI_Button()
{}

bool UI_Button::Draw() const
{
	return App->render->DrawQuadNormCoords(GetTargetNormRect(), { 0, 0, 0, 255 });
}
