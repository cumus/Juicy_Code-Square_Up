#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"
#include "Transform.h"
#include <string>

class Gameobject : public EventListener
{
public:

	Gameobject(const char* name = "unknown", Gameobject* parent = nullptr);
	~Gameobject();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void RecieveEvent(const Event& e) override;

	Transform* GetTransform();
	Transform* GetTransform() const;

	void SetName(const char* name);

private:

	Transform* AddNewChild(Gameobject* child);

private:

	bool active = true;
	std::string name = "unknown";

	std::vector<Component*> components;
	std::vector<Gameobject*> childs;

	Gameobject* parent = nullptr;
	Transform* transform = nullptr;
};

#endif // __GAMEOBJECT_H__