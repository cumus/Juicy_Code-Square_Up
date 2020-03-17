#include "UI_Button.h"
#include "Application.h"
#include "Render.h"

bool UI_Button::Draw() const
{
	return App->render->DrawQuadNormCoords(GetTargetNormRect(), { 0, 0, 0, 255 });
}
