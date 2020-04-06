#include "Edge.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"

Edge::Edge(Gameobject* go) : Behaviour(go, EDGE, FULL_LIFE, B_EDGE)
{
	current_life = max_life = 100;
	damage = 0;
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

void Edge::OnRightClick(float x, float y)
{
	OnDamage(3);
}

void Edge::OnDamage(int d)
{
	if (current_state != DESTROYED)
	{
		current_life -= d;

		LOG("Current life: %d", current_life);

		if (current_life <= 0)
			OnKill();
		else if (current_life >= max_life * 0.5f)
			current_state = FULL_LIFE;
		else
			current_state = HALF_LIFE;
	}
}

void Edge::OnKill()
{
	current_life = 0;
	current_state = DESTROYED;
	game_object->Destroy(5.0f);
}
