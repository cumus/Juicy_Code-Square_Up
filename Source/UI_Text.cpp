#include "Application.h"
#include "UI_Text.h"
#include "Render.h"
#include "FontManager.h"

UI_Text::UI_Text(EditorWindow* window, RectF rect, const char* t, int font_id)
	: UI_Element(window, TEXT, rect)
{
	text = new RenderedText(t, font_id);
}

UI_Text::~UI_Text()
{
	DEL(text);
}

bool UI_Text::Draw() const
{
	SDL_Rect rect = GetTargetRect();
	return scale_to_fit ? App->render->Blit_Text(text, rect.x, rect.y) : App->render->Blit_TextSized(text, rect);
}

UI_Text* UI_Text::ToUiText()
{
	return this;
}
