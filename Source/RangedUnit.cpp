#include "RangedUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"

RangedUnit::RangedUnit(Gameobject* go) : B_Unit(go, UNIT_RANGED, IDLE, B_RANGED)
{
	//Stats
	max_life = 100;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 15;
	attack_range = 10.0f;
	vision_range = 15.0f;
	providesVisibility = true;
	//SFX
	//deathFX = RANGED_DIE_FX;
	//attackFX = RANGED_ATK_FX;
	//SetColliders();
}

RangedUnit::~RangedUnit()
{
	/*Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());*/
}

void RangedUnit::UnitAttackType()
{
	vec pos = game_object->GetTransform()->GetGlobalPosition();
	shootPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	shootPos.first += 30.0f;
	shootPos.second += 20.0f;

	pos = objective->GetTransform()->GetGlobalPosition();
	atkObj = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	atkObj.first += 30.0f;
	atkObj.second += 20.0f;
	shoot = true;
}