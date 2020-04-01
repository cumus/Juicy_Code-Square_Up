#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "PathfindingManager.h"
#include "Defs.h"

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