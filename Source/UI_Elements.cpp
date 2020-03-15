#include "UI_Elements.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"

void UI_Element::SetLocalPos(iPoint pos) { localPos = pos; }

bool UI_Element::OnHover()
{
	iPoint mouse;
	SDL_Rect camera;
	camera = App->render->GetCameraRect();

	bool ret = false;
	App->input->GetMousePosition(mouse.x, mouse.y);
	mouse.x -= camera.x;
	mouse.y -= camera.y;

	if ((mouse.x > rect.x) && (mouse.x < rect.x + rect.w) && (mouse.y > rect.y) && (mouse.y < rect.y + rect.h)) ret = true;

	return ret;
}

iPoint UI_Element::GetScreenPos() const
{
	iPoint pos;
	pos.x = rect.x;
	pos.y = rect.y;
	return pos;
}

iPoint UI_Element::GetLocalPos() const
{
	iPoint pos;
	if (parent != nullptr)
	{
		pos.x = rect.x - parent->GetScreenPos().x;
		pos.y = rect.y - parent->GetScreenPos().y;
	}
	else
	{
		pos.x = rect.x;
		pos.y = rect.y;
	}
	return pos;
}

SDL_Rect UI_Element::GetScreenRect() const { return rect; }

SDL_Rect UI_Element::GetLocalRect() const
{
	SDL_Rect local_rect = rect;
	if (parent != nullptr)
	{
		local_rect.x = rect.x - parent->GetScreenPos().x;
		local_rect.y = rect.y - parent->GetScreenPos().y;
	}
	return local_rect;
}