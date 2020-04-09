#ifndef __MELEEUNIT_H__
#define __MELEEUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class MeleeUnit : public B_Unit
{
public:
	MeleeUnit(Gameobject* go);
	~MeleeUnit();
	// void OnDamage(int d);
	// void CheckSprite();
	// void OnKill();

};

#endif

