#include "Gameobject.h"
#include "Application.h"
#include "Editor.h"
#include "Defs.h"
#include "Transform.h"

#include "optick-1.3.0.0/include/optick.h"

double Gameobject::go_count = 0;

Gameobject::Gameobject(const char* n, Gameobject* p) : id(++go_count), name(n), parent(p)
{
	transform = new Transform(this);

	if (parent != nullptr)
		parent->AddNewChild(this);
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
				if (App->editor->selection == *it)
					App->editor->selection = nullptr;

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
	case ON_PLAY:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_PLAY, *component);

		for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
			if ((*child)->active)
				Event::Push(ON_PLAY, *child);
		break;
	}
	case ON_PAUSE:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_PAUSE, *component);

		for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
			if ((*child)->active)
				Event::Push(ON_PAUSE, *child);
		break;
	}
	case ON_STOP:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_STOP, *component);

		for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
			if ((*child)->active)
				Event::Push(ON_STOP, *child);
		break;
	}
	case ON_SELECT:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_SELECT, *component);
		break;
	}
	case ON_RIGHT_CLICK:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_RIGHT_CLICK, *component, e.data1, e.data2);

		break;
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

UI_Component* Gameobject::GetUIParent() const
{
	if (parent)
		for (std::vector<Component*>::const_iterator it = parent->components.begin(); it != parent->components.end(); ++it)
			if ((*it)->GetType() > UI_GENERAL && (*it)->GetType() < UI_MAX)
				return (*it)->AsUIComp();

	return nullptr;
}

B_Movable* Gameobject::GetBMovable()
{
	if (bMovable != nullptr)
		return bMovable;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == B_MOVABLE)
			return bMovable = (*it)->AsBMovable();

	return nullptr;
}

const B_Movable* Gameobject::GetBMovable() const
{
	if (bMovable != nullptr)
		return bMovable;

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == B_MOVABLE)
			return (*it)->AsBMovable();

	return nullptr;
}

B_Unit* Gameobject::GetBUnit()
{
	if (bunit != nullptr)
		return bunit;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == B_UNIT)
			return bunit = (*it)->AsBUnit();

	return nullptr;
}

const B_Unit* Gameobject::GetBUnit() const
{
	if (bunit != nullptr)
		return bunit;

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == B_UNIT)
			return (*it)->AsBUnit();

	return nullptr;
}



void Gameobject::RecursiveFillHierarchy(float deepness, std::vector<std::pair<float, Gameobject*>>& container)
{
	container.push_back({ deepness, this });

	for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
		(*child)->RecursiveFillHierarchy(++deepness, container);
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

	if (child != nullptr)
	{
		for (std::vector<Gameobject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
		{
			if (child == *it)
			{
				Event::Push(ON_DESTROY, child);
				go_to_remove.push(child->id);
				ret = true;
				break;
			}
		}
	}

	return ret;
}

bool Gameobject::RemoveComponent(Component* comp)
{
	bool ret = false;

	if (comp != nullptr)
	{
		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		{
			if (comp == (*it))
			{
				Event::Push(ON_DESTROY, comp);
				comp_to_remove.push(comp->GetID());
				ret = true;
				break;
			}
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

void Gameobject::AddNewChild(Gameobject * child)
{
	if (child != nullptr)
	{
		childs.push_back(child);
		child->transform->SetParent(transform);
	}
}
