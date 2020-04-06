#include "MeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"



MeleeUnit::MeleeUnit(Gameobject* go) : B_Unit(go, ENEMY_MELEE, IDLE, B_UNIT)
{
	current_life = 100;
	//damage = 2;
}

MeleeUnit::~MeleeUnit()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());
}

void MeleeUnit::OnDamage(int d)
{
	if (current_state != DESTROYED)
	{
		current_life -= d;

		LOG("Current life: %d", current_life);

		if (current_life <= 0)
			OnKill();

	}
}

void MeleeUnit::CheckSprite()
{
	switch (current_state)
	{
	case IDLE:

		break;
	case MOVING_E:

		break;
	case MOVING_W:

		break;
	case MOVING_S:

		break;
	case MOVING_N:

		break;
	case MOVING_SE:

		break;
	case MOVING_NE:

		break;
	case MOVING_SW:

		break;
	case MOVING_NW:

		break;
	case ATTACKING_E:

		break;
	case ATTACKING_W:

		break;
	case ATTACKING_S:

		break;
	case ATTACKING_N:

		break;
	case ATTACKING_SE:

		break;
	case ATTACKING_NE:

		break;
	case ATTACKING_SW:

		break;
	case ATTACKING_NW:

		break;
	}
}

void MeleeUnit::OnKill()
{
	current_life = 0;
	current_state = DESTROYED;
	game_object->Destroy(5.0f);
}

