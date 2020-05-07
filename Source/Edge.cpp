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
	providesVisibility = false;
	//deathFX = EDGE_FX;
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}	

	//SetColliders();
}

Edge::~Edge() 
{
	
}

/*void Edge::SetColliders()
{
	//Collider
	pos = game_object->GetTransform()->GetGlobalPosition();
	bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, BUILDING_TAG);
}*/

void Edge::FreeWalkabilityTiles()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
}


void Edge::Update()
{
	CheckFoWMap();
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

