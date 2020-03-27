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
	case ON_MOVEMENT: {

		vec pos = game_object->GetTransform()->GetGlobalPosition();
		iPoint origin = {int(pos.x), int(pos.y)};
		iPoint destination = { e.data1.AsInt(), e.data2.AsInt() };
		path = App->pathfinding.CreatePath(origin , destination/*,ID*/);
		if(path != nullptr) LOG("Path length: %d", path->size());

		break;
	}
	default:
		break;
	}
	
};

void B_Movable::Update()
{
	game_object->GetTransform()->MoveX(speed * App->time.GetDeltaTime());
	game_object->GetTransform()->MoveY(speed * App->time.GetDeltaTime());
		
}
