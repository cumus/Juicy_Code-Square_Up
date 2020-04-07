#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"
#include <string>

class Transform;
class Behaviour;
class UI_Component;

class Gameobject : public EventListener
{
public:

	Gameobject(const char* name = "unknown", Gameobject* parent = nullptr);
	Gameobject(const Gameobject& copy);
	~Gameobject();

	void PreUpdate();
	void Update();
	void PostUpdate();

	bool IsActive() const { return active; }
	void SetActive() { active = true; }
	void SetInactive() { active = false; }

	void RecieveEvent(const Event& e) override;

	double GetID() const { return id; }
	bool operator==(Gameobject* go) { return go && id == go->id; }

	const char* GetName() const { return name.c_str(); };
	void SetName(const char* str) { name = str; }

	Transform*			GetTransform() { return transform; }
	Behaviour*			GetBehaviour() { return behaviour; }

	const Transform*	GetTransform() const { return transform; }
	const Behaviour*	GetBehaviour() const { return behaviour; }

	std::vector<Gameobject*>& GetChilds() { return childs; }

	UI_Component* GetUIParent() const;

	void RecursiveFillHierarchy(float deepness, std::vector<std::pair<float, Gameobject*>>& container);
	void AddComponent(Component* comp);
	void RemoveChilds();
	bool RemoveChild(Gameobject* child);
	bool RemoveComponent(Component* comp);
	bool Destroy(float ms = 0.f);


private:

	void AddNewChild(Gameobject* child);

private:

	static double go_count;

	double id;
	bool active = true;
	std::string name;
	float death_timer = -1;

	std::vector<Component*> components;
	std::vector<Gameobject*> childs;

	std::queue<double> comp_to_remove;
	std::queue<double> go_to_remove;

	Gameobject* parent = nullptr;
	Transform* transform = nullptr;
	Behaviour* behaviour = nullptr;
};

#endif // __GAMEOBJECT_H__