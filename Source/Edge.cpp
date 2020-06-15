#include "Edge.h"
#include "Application.h"
#include "Gameobject.h"
#include "Log.h"

Edge::Edge(Gameobject* go) : Behaviour(go, EDGE, NO_UPGRADE, B_EDGE)
{
	max_life = 300;
	current_life = max_life;
	damage = 0;
	dieDelay = 3.0f;
	providesVisibility = false;
	deathFX = EDGE_FX;
	SetColliders();
	App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
}

Edge::~Edge() {}

void Edge::Update()
{
	CheckFoWMap();
}

void Edge::FreeWalkabilityTiles()
{
	App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
}

void Edge::AfterDamageAction(UnitType from)
{
	if (current_life < max_life / 2)
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 4);
		Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 4);
	}
	else
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_EDGE, 7);
		Event::Push(UPDATE_STAT, App->scene, EDGE_COLLECTED, 7);
	}
}