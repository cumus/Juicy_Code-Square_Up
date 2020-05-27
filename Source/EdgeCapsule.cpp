#include "EdgeCapsule.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"

Capsule::Capsule(Gameobject* go) : Behaviour(go, CAPSULE, POSE, B_CAPSULE)
{
	max_life = 1;
	current_life = max_life;
	damage = 0;
	dieDelay = 5.0f;
	spriteState = POSE;
	current_state = POSE;
	providesVisibility = true;
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}

	SetColliders();
}

Capsule::~Capsule()
{
	
}


void Capsule::FreeWalkabilityTiles()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
}

void Capsule::Update()
{
	CheckFoWMap();
}

void Capsule::AfterDamageAction()
{
	spriteState = OPEN;
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

