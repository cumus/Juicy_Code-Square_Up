#ifndef __RANGEDUNIT_H__
#define __RANGEDUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class RangedUnit : public B_Unit
{
public:
	RangedUnit(Gameobject* go);
	~RangedUnit();
	void UnitAttackType() override;
};

#endif#
