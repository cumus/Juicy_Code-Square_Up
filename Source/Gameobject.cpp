#include "Gameobject.h"
#include "Defs.h"
#include "Transform.h"

#include "optick-1.3.0.0/include/optick.h"

double Gameobject::go_count = 0;

Gameobject::Gameobject(const char* n, Gameobject* p) : id(++go_count), name(n)
{
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

	while (!comp_to_remove.empty())
	{
		double comp_id = comp_to_remove.front();

		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			if (comp_id == (*it)->GetID())
			{
				DEL(*it);
				components.erase(it);
				break;
			}
		}

		comp_to_remove.pop();
	}

	while (!go_to_remove.empty())
	{
		double go_id = go_to_remove.front();

		for (std::vector<Gameobject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			if (go_id == (*it)->id)
			{
				DEL(*it);
				childs.erase(it);
				break;
			}
		}

		go_to_remove.pop();
	}


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
	case ON_SELECT:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_SELECT, *component);
	}
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

const char* Gameobject::GetName() const
{
	return name.c_str();
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

bool Gameobject::RemoveChild(Gameobject* child)
{
	bool ret = false;

	for (std::vector<Gameobject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		if (id == (*it)->id)
		{
			go_to_remove.push(child->id);
			ret = true;
			break;
		}
	}

	return ret;
}

bool Gameobject::RemoveComponent(Component* comp)
{
	bool ret = false;

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if (comp == (*it))
		{
			comp_to_remove.push(comp->GetID());
			ret = true;
			break;
		}
	}

	return ret;
}

bool Gameobject::Destroy()
{
	return parent != nullptr && parent->RemoveChild(this);
}

double Gameobject::GetID() const
{
	return id;
}

bool Gameobject::operator==(Gameobject* go)
{
	return go != nullptr && id == go->id;
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
