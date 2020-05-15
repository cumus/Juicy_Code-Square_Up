#ifndef __UNIT_CAPSULE_H__
#define __UNIT_CAPSULE_H__

#include "Behaviour.h"

class Unit_Capsule : public Behaviour
{
public:
	Unit_Capsule(Gameobject* go);
	~Unit_Capsule();

	void AfterDamageAction() override;
};

#endif