#include "Behaviour.h"
#include "Application.h"
#include "TimeManager.h"
#include "Gameobject.h"
#include "PathfindingManager.h"
#include "Log.h"



Behaviour::Behaviour(Gameobject* go, ComponentType type) : Component(type, go)
{
	ID = GetID();
	startingLife = 10;
	currentLife = startingLife;
	damage = 1;
	selected = false;
	canAttackUnits = true;
	unit = UNKNOWN;
}


void B_Movable::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_SELECT: {

		currentLife -= 5;		

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
		next = false;
		move = false;
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
		else if (!positiveX && !positiveY)
		{
			if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (!positiveX && positiveY)
		{
			if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (positiveX && !positiveY)
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
		iPoint tilePos = { int(pos.x), int(pos.y) };

		if (nextTile.x > tilePos.x) 
		{
			game_object->GetTransform()->MoveX(+speed * App->time.GetGameDeltaTime());
			//game_object->GetTransform()->MoveX(+speed * App->time.GetDeltaTime());
			positiveX = true;
		}
		else
		{
			game_object->GetTransform()->MoveX(-speed * App->time.GetGameDeltaTime());
			//game_object->GetTransform()->MoveX(-speed * App->time.GetDeltaTime());
			positiveX = false;			
		}				

		if (nextTile.y > tilePos.y) 
		{
			game_object->GetTransform()->MoveY(+speed * App->time.GetGameDeltaTime());
			//game_object->GetTransform()->MoveY(+speed * App->time.GetDeltaTime());
			positiveY = true;
		}
		else
		{
			game_object->GetTransform()->MoveY(-speed * App->time.GetGameDeltaTime());
			//game_object->GetTransform()->MoveY(-speed * App->time.GetDeltaTime());
			positiveY = false;
		}	
	}	
}

///////////////////////////
// BUILDING BEHAVIOUR
///////////////////////////


void B_Building::Init(int life, int dmg, bool attackUnits, UnitType type)
{
	startingLife = life;
	damage = dmg;
	canAttackUnits = attackUnits;
	this->unit = type;
	currentState = FULL;
	CheckSprite();
}

void B_Building::GotDamaged(int dmg)
{
	if (currentState != DESTROYED)
	{
		currentLife -= dmg;
		CheckState();
	}
}

void B_Building::Repair(int heal)
{
	if (currentState != DESTROYED)
	{
		currentLife += heal;
		if (currentLife > startingLife) { currentLife = startingLife; }
		CheckState();
	}
}

void B_Building::CheckState()
{
	if (currentLife < (startingLife / 2))
	{
		if (currentLife > 0) currentState = HALF;
		else currentState = DESTROYED;
	}
	else currentState = FULL;
	LOG("State: %d", currentState);
	CheckSprite();
}



///////////////////////////
// UNIT BEHAVIOUR
///////////////////////////

