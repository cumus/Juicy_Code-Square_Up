#include "Application.h"
#include "UI_Text.h"
#include "Render.h"
#include "FontManager.h"

UI_Text::UI_Text(EditorWindow* window, RectF rect, int font_id, const char* text)
	: UI_Element(window, TEXT, rect), font_id(font_id), text(text)
{}

UI_Text::~UI_Text()
{}

bool UI_Text::Draw() const
{
	return App->render->Blit_TextSized(text, GetTargetRect(), font_id);
}