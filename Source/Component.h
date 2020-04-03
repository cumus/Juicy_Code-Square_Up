#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "EventListener.h"

enum ComponentType
{
	COMP_NONE,
	TRANSFORM,
	SPRITE,
	AUDIO_SOURCE,

	// UI
	UI_GENERAL,
	UI_CANVAS,
	UI_IMAGE,
	UI_TEXT,
	UI_BUTTON,
	UI_MAX,

	// Behaviours
	BEHAVIOUR,
	B_MOVABLE,
	B_BUILDING,
	B_UNIT,

	//Units


	//Structures
	EDGE
};

class Transform;
class Gameobject;
class B_Unit;
class B_Movable;
class B_Building;
class Edge;
class UI_Component;

class Component : public EventListener
{
public:

	Component(ComponentType type, Gameobject* go);
	virtual ~Component() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	bool IsActive() const { return active; }

	void SetActive() { active = true; }
	void SetInactive() { active = false; }

	ComponentType GetType() const { return type; }
	Transform* AsTransform() const { return (Transform*)this; }
	B_Unit* AsBUnit() const { return (B_Unit*)this; }
	B_Movable* AsBMovable() const { return (B_Movable*)this; }
	B_Building* AsBBuilding() const { return (B_Building*)this; }
	Edge* AsEdge() const { return (Edge*)this; }
	UI_Component* AsUIComp() const { return (UI_Component*)this; }
	Gameobject* GetGameobject() const { return game_object;	}

	double GetID() const { return id; }

	bool operator==(Component* comp);

private:

	static double component_count;

	double id = -1;
	bool active = true;
	ComponentType type = COMP_NONE;

protected:

	Gameobject* game_object = nullptr;
};

#endif // __COMPONENT_H__