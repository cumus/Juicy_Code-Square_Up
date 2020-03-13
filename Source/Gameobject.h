#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"
#include "Transform.h"
#include <string>

class Gameobject : public EventListener
{
public:

	Gameobject(const char* name = "unknown", Gameobject* parent = nullptr);
	Gameobject(const Gameobject& copy);
	~Gameobject();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void RecieveEvent(const Event& e) override;

	std::string GetName() const;
	void SetName(const char* name);

	Transform* GetTransform();
	const Transform* GetTransform() const;

	std::vector<Gameobject*>& GetChilds();

	void AddComponent(Component* comp);

private:

	Transform* AddNewChild(Gameobject* child);

private:

	bool active = true;
	std::string name;

	std::vector<Component*> components;
	std::vector<Gameobject*> childs;

	Gameobject* parent = nullptr;
	Transform* transform = nullptr;
};

#endif // __GAMEOBJECT_H__