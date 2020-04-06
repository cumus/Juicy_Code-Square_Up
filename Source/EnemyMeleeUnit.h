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
	void OnDamage(int d);
	void CheckSprite();
	void OnKill();

protected:

	int EM_lvl = 1;
	int EM_max_lvl = 5;
	int EM_damage = 5;
	int attack_speed = 1;

};

#endif
