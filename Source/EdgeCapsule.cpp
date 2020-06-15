#include "EdgeCapsule.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"

Capsule::Capsule(Gameobject* go) : Behaviour(go, CAPSULE, POSE, B_CAPSULE)
{
	max_life = 100;
	current_life = max_life;
	damage = 0;
	dieDelay = 2.0f;
	spriteState = POSE;
	current_state = POSE;
	providesVisibility = false;
	gives_edge = true;

	App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	SetColliders();
}

Capsule::~Capsule()
{}

void Capsule::Update()
{
	CheckFoWMap();
}

void Capsule::FreeWalkabilityTiles()
{
	App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
}

void Capsule::AfterDamageAction(UnitType from)
{
	current_life = 1;
	spriteState = OPEN;
	if (from != EARTHQUAKE)
	{
		if (gives_edge)
		{
			Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 100);
			Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 100);
		}
		else 
		{
			vec pos = game_object->GetTransform()->GetGlobalPosition();

			for (int i = 0; i < 10; i++)
			{
				Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 15);
				Event::Push(SPAWN_UNIT, App->scene, UNIT_MELEE, pos - 5 + i);
				
			}
		}
	}
}

