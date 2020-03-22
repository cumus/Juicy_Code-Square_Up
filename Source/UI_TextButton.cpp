#include "Application.h"
#include "UI_TextButton.h"
#include "Render.h"
#include "FontManager.h"

UI_TextButton::UI_TextButton(EditorWindow* window, RectF rect, const char* text, int font_id)
	: UI_Element(window, TEXT, rect), font_id(font_id), text(text)
{}

UI_TextButton::~UI_TextButton()
{}

bool UI_TextButton::Draw() const
{

	App->render->DrawQuadNormCoords(GetTargetNormRect(), color);

	App->render->Blit_TextSized(text, GetTargetRect(), font_id);

	return true;
}

UI_TextButton* UI_TextButton::ToUiTextButton()
{
	return this;
}