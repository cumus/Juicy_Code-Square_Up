#include "Sprite.h"
#include "Application.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Transform.h"

Sprite::Sprite(Gameobject* go) : Component(SPRITE, go)
{
	r = g = b = a = 250;

	if (go) go->AddComponent(this);
}

Sprite::~Sprite()
{}

void Sprite::PostUpdate()
{
	std::pair<float, float> cam = { App->render->cam_x, App->render->cam_y };

	if (game_object)
	{
		Transform* t = game_object->GetTransform();
		if (t)
		{
			vec pos = t->GetGlobalPosition();
			std::pair<float, float> map_pos = App->map->F_MapToWorld(pos.x, pos.y);

			if (tex_id >= 0)
				App->render->Blit(tex_id, int(map_pos.first - App->render->cam_x), int(map_pos.second - App->render->cam_y), 1, 1, &section);
			else
				App->render->DrawQuad({ int(map_pos.first - App->render->cam_x), int(map_pos.second - App->render->cam_y), section.w, section.h }, r, g, b, a);
		}
	}
}
