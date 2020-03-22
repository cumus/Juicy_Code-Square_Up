#include "Application.h"
#include "UI_Text.h"
#include "Render.h"
#include "FontManager.h"

UI_Text::UI_Text(EditorWindow* window, RectF rect, const char* text, int font_id)
	: UI_Element(window, TEXT, rect), text(text), font_id(font_id)
{}

UI_Text::~UI_Text()
{}

bool UI_Text::Draw() const
{
	return App->render->Blit_TextSized(text.c_str(), GetTargetRect(), font_id);
}

UI_Text* UI_Text::ToUiText()
{
	return this;
}
