#include "Application.h"
#include "UI_TextButton.h"
#include "Render.h"
#include "FontManager.h"

UI_TextButton::UI_TextButton(EditorWindow* window, RectF rect, const char* t, int font_id)
	: UI_Element(window, TEXT, rect)
{
	text = new RenderedText(t, font_id);
}

UI_TextButton::~UI_TextButton()
{
	DEL(text);
}

bool UI_TextButton::Draw() const
{
	bool ret;

	if (ret = App->render->DrawQuadNormCoords(GetTargetNormRect(), color))
	{
		SDL_Rect rect = GetTargetRect();
		ret = (scale_to_fit ?
			App->render->Blit_Text(text, rect.x, rect.y) :
			App->render->Blit_TextSized(text, rect));
	}

	return ret;
}

UI_TextButton* UI_TextButton::ToUiTextButton()
{
	return this;
}