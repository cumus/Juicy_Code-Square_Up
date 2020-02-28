#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "EventListener.h"

enum ComponentType
{
	NONE,
	TRANSFORM
};

class Transform;
class Gameobject;

class Component : public EventListener
{
public:

	Component(ComponentType type, Gameobject* go) : type(type), game_object(go) {}
	virtual ~Component() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	bool IsActive() const { return active; }

	void SetActive() { active = true; }
	void SetInactive() { active = false; }

	ComponentType GetType() const { return type; }
	Transform* AsTransform() const { return (Transform*)this; }

private:

	bool active = true;
	ComponentType type = NONE;

protected:

	Gameobject* game_object = nullptr;
};

#endif // __COMPONENT_H__