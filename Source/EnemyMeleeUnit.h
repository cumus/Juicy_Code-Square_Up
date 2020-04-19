#ifndef __ENEMYMELEEUNIT_H__
#define __ENEMYMELEEUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class EnemyMeleeUnit : public B_Unit
{
public:
	EnemyMeleeUnit(Gameobject* go);
	~EnemyMeleeUnit();
	void UpdatePath(int x,int y) override;
	void IARangeCheck() override;

protected:
	std::pair<int, int> destPos;
	bool going_base;
	bool going_enemy;
	bool base_found;
	Gameobject* baseCenter;
	float scanTimer, scanTime;
};

#endif
