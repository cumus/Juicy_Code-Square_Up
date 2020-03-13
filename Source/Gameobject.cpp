#include "Gameobject.h"
#include "Defs.h"
#include "Transform.h"

#include "Optick/include/optick.h"

Gameobject::Gameobject(const char* n, Gameobject* p)
{
	name = n;

	components.push_back(transform = new Transform(this));

	if (p != nullptr)
		transform->SetParent(*p->AddNewChild(this));
}

Gameobject::Gameobject(const Gameobject& copy) :
	active(copy.active),
	name(copy.name),
	components(copy.components),
	childs(copy.childs),
	parent(copy.parent),
	transform(copy.transform)
{}

Gameobject::~Gameobject()
{
	for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
		DEL(*child);

	transform = nullptr;

	for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
		DEL(*component);
}

void Gameobject::PreUpdate()
{
	for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
		if ((*component)->IsActive())
			(*component)->PreUpdate();

	for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
		if ((*child)->active)
			(*child)->PreUpdate();
}

void Gameobject::Update()
{
	OPTICK_EVENT();

	for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
		if ((*component)->IsActive())
			(*component)->Update();

	for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
		if ((*child)->active)
			(*child)->Update();
}

void Gameobject::PostUpdate()
{
	for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
		if ((*component)->IsActive())
			(*component)->PostUpdate();

	for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
		if ((*child)->active)
			(*child)->PostUpdate();
}

void Gameobject::RecieveEvent(const Event & e)
{
	switch (e.type)
	{
	case TRANSFORM_MODIFIED:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(TRANSFORM_MODIFIED, *component, e.data1, e.data2);

		for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
			if ((*child)->active)
				Event::Push(PARENT_TRANSFORM_MODIFIED, *child, e.data1, e.data2);
		break;

	}
	case PARENT_TRANSFORM_MODIFIED:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(PARENT_TRANSFORM_MODIFIED, *component,
					Cvar(e.data1),
					Cvar(e.data2));
		break;

	}
	}
}

std::string Gameobject::GetName() const
{
	return name;
}

Transform * Gameobject::GetTransform()
{
	if (transform != nullptr)
		return transform;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == TRANSFORM)
			return transform = (*it)->AsTransform();

	return nullptr;
}

const Transform* Gameobject::GetTransform() const
{
	if (transform != nullptr)
		return transform;

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == TRANSFORM)
			return (*it)->AsTransform();

	return nullptr;
}

std::vector<Gameobject*>& Gameobject::GetChilds()
{
	return childs;
}

void Gameobject::SetName(const char * n)
{
	name = n;
}

void Gameobject::AddComponent(Component* comp)
{
	if (comp)
		components.push_back(comp);
}

Transform* Gameobject::AddNewChild(Gameobject * child)
{
	Transform* ret = nullptr;

	if (child != nullptr)
	{
		childs.push_back(child);
		ret = child->GetTransform();
	}

	return ret;
}
