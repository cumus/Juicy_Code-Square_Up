#include "UI_Button.h"
#include "Application.h"
#include "Render.h"

UI_Button::UI_Button(EditorWindow* window, RectF rect)
	: UI_Element(window, BUTTON, rect)
{}

UI_Button::~UI_Button()
{}

void UI_Button::Draw() const
{
	App->render->DrawQuadNormCoords(GetTargetNormRect(), color, true, EDITOR);
}

UI_Button* UI_Button::ToUiButton()
{
	return this;
}
