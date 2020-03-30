#include "Sprite.h"
#include "Application.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Transform.h"

#include "optick-1.3.0.0/include/optick.h"

Sprite::Sprite(Gameobject* go) : Component(SPRITE, go)
{
	color = { 255, 255, 255, 255 };
}

Sprite::~Sprite()
{}

void Sprite::PostUpdate()
{
	OPTICK_EVENT();

	if (game_object)
	{
		Transform* t = game_object->GetTransform();
		if (t)
		{
			vec pos = t->GetGlobalPosition();
			std::pair<float, float> map_pos = Map::F_MapToWorld(pos.x, pos.y);

			if (tex_id >= 0)
				App->render->Blit(tex_id, int(map_pos.first), int(map_pos.second), &section);
			else
				App->render->DrawQuad({ int(map_pos.first), int(map_pos.second), section.w, section.h }, color);
		}
	}
}
