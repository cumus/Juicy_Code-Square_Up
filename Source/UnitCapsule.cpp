#include "UnitCapsule.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"
#include "Behaviour.h"

Unit_Capsule::Unit_Capsule(Gameobject* go) : Behaviour(go, UNIT_CAPSULE, NO_UPGRADE, B_CAPSULE)
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

Unit_Capsule::~Unit_Capsule()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());
}

void Unit_Capsule::AfterDamageAction()
{
	
	//Event(BUILD_MELEE, this, 5.0f));

	if (current_life <= 0)
		OnKill(type);
}
