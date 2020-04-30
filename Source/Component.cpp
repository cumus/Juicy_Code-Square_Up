#include "Component.h"
#include "Gameobject.h"

double Component::component_count = 0;
std::map<double, Component*> Component::ComponentsList;

Component::Component(ComponentType type, Gameobject* go) : id(++component_count), type(type), game_object(go)
{
	if (go != nullptr)
	{
		go->AddComponent(this);
		ComponentsList.emplace(id,this);
	}
}

bool Component::IsActive() const
{
	return active && game_object->IsActive();
}

bool Component::operator==(Component * comp)
{
	return comp != nullptr && id == comp->id;
}
