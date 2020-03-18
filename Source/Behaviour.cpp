#include "Behaviour.h"
#include "Gameobject.h"
#include "Log.h"


Behaviour::Behaviour(Gameobject* go, ComponentType type) : Component(type, go) {

	if (go != nullptr) go->AddComponent(this);

}


void B_Unit::RecieveEvent(const Event& e)
{
	
	switch (e.type)
	{
	case ON_SELECT: {
		
		life -= damage;
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
	default:
		break;
	}
	
};
