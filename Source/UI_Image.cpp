#include "Application.h"
#include "UI_Image.h"
#include "Render.h"
#include "TextureManager.h"

UI_Image::UI_Image() {

}

UI_Image::UI_Image(Module* _callback) 
{
	callback = _callback;
	to_delete = false;
}

void UI_Image::Init(iPoint pos, SDL_Rect _section)
{
	screenPos = pos;
	section = _section;
	rect.x = screenPos.x;
	rect.y = screenPos.y;
	rect.w = section.w;
	rect.h = section.h;
	if (parent != nullptr)
	{
		localPos.x = screenPos.x - parent->screenPos.x;
		localPos.y = screenPos.y - parent->screenPos.y;
	}
}

bool UI_Image::Update(float dt)
{
	bool ret = true;
	if (parent != nullptr)
	{
		screenPos.x = parent->screenPos.x + localPos.x;
		screenPos.y = parent->screenPos.y + localPos.y;
	}
	
	SDL_Rect camera;
	camera = App->render->GetCameraRect();

	rect.x = screenPos.x - camera.x;
	rect.y = screenPos.y - camera.y;
	return ret;
}

bool UI_Image::Draw() 
{
	App->render->Blit(texture_id, rect.x, rect.y, &section);
	return true;
}

bool UI_Image::CleanUp()
{
	bool ret = true;
	return ret;
}