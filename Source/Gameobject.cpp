#include "Gameobject.h"
#include "Application.h"
#include "Scene.h"
#include "Audio.h"
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

	if (death_timer > 0.f)
	{
		death_timer -= App->time.GetGameDeltaTime();

		if (death_timer <= 0.f)
			Destroy();
	}

	UpdateRemoveQueue();

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

bool Gameobject::IsActive() const
{
	return active && (parent ? parent->IsActive() : true);
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
	case ON_UNSELECT:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_UNSELECT, *component);
		break;
	}
	case ON_DESTROY:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			Event::Push(ON_DESTROY, *component);

		for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
			Event::Push(ON_DESTROY, *child);

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
	case ON_COLL_ENTER:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_COLL_ENTER, *component,
					Cvar(e.data1),
					Cvar(e.data2));
		break;
	}
	case ON_COLL_EXIT:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_COLL_EXIT, *component,
					Cvar(e.data1),
					Cvar(e.data2));
		break;
	}
	case ON_COLL_STAY:
	{
		for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
			if ((*component)->IsActive())
				Event::Push(ON_COLL_STAY, *component,
					Cvar(e.data1),
					Cvar(e.data2));
		break;
	}
	}
}

UI_Component* Gameobject::GetUIParent() const
{
	UI_Component* ret = nullptr;

	if (parent)
		ret = parent->GetUI();

	return ret;
}

void Gameobject::RecursiveFillHierarchy(float deepness, std::vector<std::pair<float, Gameobject*>>& container)
{
	container.push_back({ deepness, this });

	for (std::vector<Gameobject*>::iterator child = childs.begin(); child != childs.end(); ++child)
		(*child)->RecursiveFillHierarchy(++deepness, container);
}

void Gameobject::AddComponent(Component* comp)
{
	if (comp)
	{
		const ComponentType type = comp->GetType();
		if (type == TRANSFORM)
			transform = comp->AsTransform();
		else if (type >= UI_GENERAL && type < UI_MAX)
			ui = comp->AsUIComp();
		else if (type >= BEHAVIOUR && type < MAX_BEHAVIOUR)
			behaviour = comp->AsBehaviour();
		else if (type >= COLLIDER && type < MAX_BEHAVIOUR)
			collider = comp->AsCollider();

		components.push_back(comp);
	}
}

void Gameobject::RemoveChilds()
{
	for (std::vector<Gameobject*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
	{
		Event::Push(ON_DESTROY, *it);
		go_to_remove.push((*it)->id);
	}
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

bool Gameobject::Destroy(float ms)
{
	bool ret = true;
	toDestroy = true;
	App->collSystem.ProcessRemovals();
	if ((death_timer = ms) <= 0.f)
		ret = (parent != nullptr && parent->RemoveChild(this));

	return ret;
}

void Gameobject::UpdateRemoveQueue()
{
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
				if (App->scene->selection == *it)
					App->scene->SetSelection(nullptr, false);

				DEL(*it);
				childs.erase(it);
				break;
			}
		}

		go_to_remove.pop();
	}
}

void Gameobject::AddNewChild(Gameobject * child)
{
	if (child != nullptr)
	{
		childs.push_back(child);

		if (child->transform)
			child->transform->SetParent(transform);
	}
}
