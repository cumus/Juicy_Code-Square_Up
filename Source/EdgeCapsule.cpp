#include "EdgeCapsule.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"
#include "Behaviour.h"

Edge_Capsule::Edge_Capsule(Gameobject* go) : Behaviour(go, EDGE_CAPSULE, NO_UPGRADE, B_CAPSULE)
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

Edge_Capsule::~Edge_Capsule()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());
}

void Edge_Capsule::AfterDamageAction()
{
	Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 100);
	Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 1);
	
	if (current_life <= 0)
		OnKill(type);
}

