#include "Behaviour.h"
#include "Application.h"
#include "TimeManager.h"
#include "Gameobject.h"
#include "PathfindingManager.h"
#include "Log.h"
#include "Defs.h"



Behaviour::Behaviour(Gameobject* go, ComponentType type) : Component(type, go),ID(GetID())
{}


void B_Movable::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_SELECT: {

		currentLife -= 5;
		LOG("Life after taking damage is: %d", currentLife);

		if (currentLife <= 0)
		{
			if (game_object->Destroy())
				LOG("Destroying GO: %s", game_object->GetName());
			else
				LOG("Error destroying GO: %s", game_object->GetName());
		}

		break;
	}
	case ON_RIGHT_CLICK: {

		vec pos = game_object->GetTransform()->GetGlobalPosition();
		iPoint origin = { int(pos.x), int(pos.y) };
		iPoint destination = { e.data1.AsInt(), e.data2.AsInt() };
		path = App->pathfinding.CreatePath(origin, destination,ID);
		//LOG("Path length: %d", path->size());
		break;
	}
	default:
		break;
	}
	
};

void B_Movable::Update()
{	
	if (path != nullptr && !path->empty()) 
	{	
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		fPoint actualPos = { pos.x, pos.y };		
		
		if(!next) 
		{		
			nextTile = path->front();
			//LOG("Tile coords X:%d, Y:%d",nextTile.x,nextTile.y);
			next = true;
			move = true;
			//LOG("X: %d, Y: %d", pathbegin.x, pathbegin.y);
			//LOG("X: %f, Y: %f", game_object->GetTransform()->GetGlobalPosition().x, game_object->GetTransform()->GetGlobalPosition().y);
		}

		if (positiveX && positiveY)
		{
			if (actualPos.x >= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}

		if (!positiveX && !positiveY)
		{
			if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}

		if (!positiveX && positiveY)
		{
			if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}

		if (positiveX && !positiveY)
		{
			if (actualPos.x >= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
	}	
	else
	{
		move = false;
	}

	if (move)
	{
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		fPoint tilePos = { pos.x, pos.y };

		if (nextTile.x > tilePos.x) 
		{
			//define speed properly depending on tile position in respect to the object
			game_object->GetTransform()->MoveX(+speed * App->time.GetGameDeltaTime());
			positiveX = true;
		}
		else
		{
			game_object->GetTransform()->MoveX(+speed * App->time.GetGameDeltaTime());
			positiveY = false;			
		}				

		if (nextTile.y > tilePos.y) 
		{
			//define speed properly depending on tile position in respect to the object
			game_object->GetTransform()->MoveX(+speed * App->time.GetGameDeltaTime());
			positiveY = true;
		}
		else
		{
			game_object->GetTransform()->MoveX(+speed * App->time.GetGameDeltaTime());
			positiveY = false;
		}	
		
	}	
}
