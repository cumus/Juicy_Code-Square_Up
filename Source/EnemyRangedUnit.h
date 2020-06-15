#ifndef __ENEMYRANGEDUNIT_H__
#define __ENEMYRANGEDUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class EnemyRangedUnit : public B_Unit
{
public:
	EnemyRangedUnit(Gameobject* go);
	~EnemyRangedUnit();

	void UnitAttackType() override;
};

#endif#