#ifndef __SUPERUNIT_H__
#define __SUPERUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class SuperUnit : public B_Unit
{
public:
	SuperUnit(Gameobject* go);
	~SuperUnit();
	void UnitAttackType() override;
};

#endif#