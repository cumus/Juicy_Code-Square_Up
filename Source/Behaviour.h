#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "Point.h"
#include <vector>
#include <map>

enum UnitType
{
	EDGE,
	UNIT_MELEE,
	ENEMY_MELEE,
	BASE_CENTER,
	TOWER,

	/*USER_GATHERER,
	USER_RANGED,
	USER_SUPER,
	USER_SPECIAL,
	IA_MELEE,
	IA_RANGED,
	IA_SUPER,
	IA_SPECIAL,

	//////Structures//////
	TOWN_HALL,
	LAB,
	BARRACKS,
	RANGED_TURRET,
	NEAR_TURRET,
	RESOURCE,
	IA_SPAWN*/
};

enum UnitState
{
	// Unit
	IDLE = 0,
	MOVING_N,
	MOVING_S,
	MOVING_W,
	MOVING_E,
	MOVING_NE,
	MOVING_NW,
	MOVING_SE,
	MOVING_SW,
	ATTACKING_N,
	ATTACKING_S,
	ATTACKING_W,
	ATTACKING_E,
	ATTACKING_NE,
	ATTACKING_NW,
	ATTACKING_SE,
	ATTACKING_SW,
	DEAD,

	// Building
	BUILDING,
	FULL_LIFE,
	HALF_LIFE,
	DESTROYED
};

class Sprite;
class AudioSource;

class Behaviour : public Component 
{
public:
	Behaviour(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = BEHAVIOUR);
	virtual ~Behaviour() {}

	void RecieveEvent(const Event& e) override;

	virtual void Selected();
	virtual void UnSelected();
	virtual void OnRightClick(float x, float y) {}
	virtual void OnDamage(int damage);
	virtual void OnKill();

	UnitType GetType() const { return type; }
	UnitState* GetStatePtr() { return &current_state; }

	unsigned int GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const;

protected:

	static std::map<double, Behaviour*> b_map;

	UnitType type;
	UnitState current_state;

	// Stats
	int max_life, current_life, damage;
	float attack_range, vision_range;

	// Complementary components
	AudioSource* audio;
	Sprite* selection_highlight;
};

class B_Unit : public Behaviour
{
public:

	B_Unit(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = B_UNIT);
	virtual ~B_Unit() {}

	void Update() override;
	void OnRightClick(float x, float y) override;
	
protected:

	float speed = 2;
	float aux_speed = speed;
	std::vector<iPoint>* path = nullptr;
	iPoint nextTile;
	bool next = false;
	bool move = false;
	bool positiveX = false;
	bool positiveY = false;
};

#endif // __BEHAVIOUR_H_