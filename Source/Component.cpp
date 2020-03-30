#include "Component.h"
#include "Gameobject.h"

double Component::component_count = 0;

Component::Component(ComponentType type, Gameobject* go) : id(++component_count), type(type), game_object(go)
{
	if (go != nullptr)
		go->AddComponent(this);
}

bool Component::operator==(Component * comp)
{
	return comp != nullptr && id == comp->id;
}
