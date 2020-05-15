#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "EventListener.h"

enum ComponentType
{
	COMP_NONE = 0,
	TRANSFORM,
	SPRITE,
	ANIM_SPRITE,
	AUDIO_SOURCE,

	// UI
	UI_GENERAL,
	UI_CANVAS,
	UI_IMAGE,
	UI_TEXT,
	UI_BUTTON,
	UI_MINIMAP,
	UI_MAX,

	// Behaviours
	BEHAVIOUR,
	B_MELEE_UNIT,
	B_GATHERER,
	B_RANGED,
	B_ENEMY_MELEE,
	B_EDGE,
	B_CAPSULE,
	B_BASE_CENTER,
	B_TOWER,
	B_SPAWNER,
	B_BARRACKS,
	MAX_BEHAVIOUR,

	MAX_TYPES
};

class Gameobject;
class Transform;
class Behaviour;
class UI_Component;

class Component : public EventListener
{
public:

	Component(ComponentType type, Gameobject* go);
	virtual ~Component() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	bool IsActive() const;

	void SetActive() { active = true; }
	void SetInactive() { active = false; }

	ComponentType GetType() const { return type; }
	Transform* AsTransform() const { return (Transform*)this; }
	Behaviour* AsBehaviour() const { return (Behaviour*)this; }
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