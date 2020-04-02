#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "PathfindingManager.h"
#include "Sprite.h"

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

enum BuildingState
{
	FULL,
	HALF,
	DESTROYED
};

class Gameobject;

class Behaviour : public Component 
{
public:

	Behaviour(Gameobject* go, ComponentType type = BEHAVIOUR);
	virtual ~Behaviour() {}

public:
	double ID;
	int startingLife;
	int currentLife;
	int damage;
	bool selected;
	bool canAttackUnits; //For gatherer set false and non defensive structures
	UnitType unit;
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
	std::vector<iPoint>* path = nullptr;

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
	void Init(int life, int damage, bool attackUnits, UnitType type);	
	void GotDamaged(int dmg);
	void Repair(int heal);
	void CheckState();
	virtual void SetTexture() {}
	virtual void CheckSprite() {}
	virtual void BuildingAction() {}
	
public:
	BuildingState currentState;
	int textureID;
	Sprite* building;
};

class B_Unit : public B_Movable
{
public:

	B_Unit(Gameobject* go, ComponentType type = B_UNIT) : B_Movable(go, type) {}
	virtual ~B_Unit() {}


public:
	int textureID;
};


#endif // __BEHAVIOUR_H_