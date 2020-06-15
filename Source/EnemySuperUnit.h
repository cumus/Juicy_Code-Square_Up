#ifndef __ENEMYSUPERUNIT_H__
#define __ENEMYSUPERUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class EnemySuperUnit : public B_Unit
{
public:
	EnemySuperUnit(Gameobject* go);
	~EnemySuperUnit();

	void UnitAttackType() override;
};

#endif#