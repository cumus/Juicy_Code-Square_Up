#include "EdgeCapsule.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"
#include "Behaviour.h"

Capsule::Capsule(Gameobject* go) : Behaviour(go, CAPSULE, NO_UPGRADE, B_CAPSULE)
{
	max_life = 1;
	current_life = max_life;
	damage = 0;
	dieDelay = 5.0f;
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}
}

Capsule::~Capsule()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());
}

void Capsule::AfterDamageAction()
{
	if (App->scene->capsule_content == true) {
		Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 100);
		Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 100);
	}
	else {
		vec pos = game_object->GetTransform()->GetGlobalPosition();

		for (int i = 0; i < 10; i++) {
			Event(SPAWN_UNIT, App->scene, UNIT_MELEE, pos - 5 + i);
		}
	}

	if (current_life <= 0)
		OnKill(type);
}

