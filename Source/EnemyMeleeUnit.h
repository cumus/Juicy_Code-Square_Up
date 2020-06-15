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

protected:
	bool base_found;
	Gameobject* baseCenter;
	
};

#endif
