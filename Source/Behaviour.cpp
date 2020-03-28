#include "Behaviour.h"
#include "Application.h"
#include "TimeManager.h"
#include "Gameobject.h"
#include "PathfindingManager.h"
#include "Log.h"


Behaviour::Behaviour(Gameobject* go, ComponentType type) : Component(type, go) {

	if (go != nullptr) go->AddComponent(this);

}


void B_Movable::RecieveEvent(const Event& e)
{
	
	switch (e.type)
	{
	case ON_SELECT: {

		life -= 5;
		LOG("Life after taking damage is: %d", life);

		if (life <= 0)
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
		LOG("10");
		path = App->pathfinding.CreatePath(origin, destination);
		LOG("Path length: %d", path->size());

		break;
	}
	default:
		break;
	}
	
};

void B_Movable::Update()
{
		
	if (path != nullptr && !path->empty()) {
	
		if (first_tile == false) {
			
			pathbegin = path->front();
			//pathbegin.x += 0.5;
			//pathbegin.y += 0.5;
			first_tile = true;
			path->erase(path->begin());
			move = true;
			LOG("X: %d, Y: %d", pathbegin.x, pathbegin.y);
			LOG("X: %f, Y: %f", game_object->GetTransform()->GetGlobalPosition().x, game_object->GetTransform()->GetGlobalPosition().y);

		}

		if (game_object->GetTransform()->GetGlobalPosition().x == pathbegin.x && game_object->GetTransform()->GetGlobalPosition().y == pathbegin.y) {
			first_tile = false;
		}
		
		
	}
	
	else
	{
		move = false;
	}

	if (move)
	{
		LOG("Path registered");
		//iPoint pathend = path->back();
		
		if (pathbegin.x > game_object->GetTransform()->GetGlobalPosition().x) {
			//define speed properly depending on tile position in respect to the object
			game_object->GetTransform()->MoveX(+speed * App->time.GetDeltaTime());
			LOG("1");
		}

		else
		{
			game_object->GetTransform()->MoveX(-speed * App->time.GetDeltaTime());
			LOG("2");
		}
				

		if (pathbegin.y > game_object->GetTransform()->GetGlobalPosition().y) {
			//define speed properly depending on tile position in respect to the object
			game_object->GetTransform()->MoveY(+speed * App->time.GetDeltaTime());
			LOG("3");
		}

		else
		{
			game_object->GetTransform()->MoveY(-speed * App->time.GetDeltaTime());
			LOG("4");
		}
		

	}
	
}
