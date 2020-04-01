#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "PathfindingManager.h"

//Unit types
enum UnitType
{
	//////Movable units//////
	UNKNOWN = 0,
	USER_MELEE,
	USER_RANGED,
	USER_SUPER,
	USER_SPECIAL,//Gatherer
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
	IA_SPAWN
};


class Gameobject;

class Behaviour : public Component 
{
public:

	Behaviour(Gameobject* go, ComponentType type = BEHAVIOUR);
	virtual ~Behaviour() {}

public:
	double ID=0;
	int startingLife = 10;//Temporal value
	int currentLife=startingLife;
	int damage = 5;//Temporal value
	bool selected = false;
	bool canAttackUnits = true; //For gatherer set false
	UnitType type = UNKNOWN;
};

class B_Movable: public Behaviour
{
public:

	B_Movable(Gameobject* go, ComponentType type = B_MOVABLE) : Behaviour(go, type) {}
	virtual ~B_Movable() {}
	void Update() override;
	void RecieveEvent(const Event& e) override;
	

public:
	
	float speed = 2;
	float aux_speed = speed;
	//float angle;
	std::vector<iPoint>* path = nullptr;
	//float range;

	iPoint nextTile;
	bool next = false;
	bool move = false;
	bool positiveX = false;
	bool positiveY = false;
};

class B_Building : public Behaviour
{
public:

	B_Building(Gameobject* go, ComponentType type = B_BUILDING) : Behaviour(go, type) {}
	virtual ~B_Building() {}

	
public:

};

class B_Unit : public B_Movable
{
public:

	B_Unit(Gameobject* go, ComponentType type = B_UNIT) : B_Movable(go, type) {}
	virtual ~B_Unit() {}


public:

};


#endif // __BEHAVIOUR_H__