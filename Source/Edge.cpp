#include "Edge.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"

Edge::Edge(Gameobject* go) : Behaviour(go, EDGE, FULL_LIFE, B_EDGE)
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
	Event::Push(RESOURCE, App->scene, 5);
	if (current_life <= 0)
		OnKill(type);
	else if (current_life >= max_life * 0.5f)
		current_state = FULL_LIFE;
	else
		current_state = HALF_LIFE;
}

