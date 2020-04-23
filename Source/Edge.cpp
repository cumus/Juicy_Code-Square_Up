#include "Edge.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"

Edge::Edge(Gameobject* go) : Behaviour(go, EDGE, NO_UPGRADE, B_EDGE)
{
	max_life = 100;
	current_life = max_life;
	damage = 0;
	dieDelay = 5.0f;
	//deathFX = EDGE_FX;
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}	
}

Edge::~Edge() 
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());
}

void Edge::OnRightClick(vec pos, vec modPos)
{
	OnDamage(3);
}


void Edge::AfterDamageAction()
{
	if (App->scene->tutorial_edge == true)
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 15);
		Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 1);
	}
	else
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 5);
		Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 1);
	}

	if (current_life <= 0)
		OnKill(type);
}

