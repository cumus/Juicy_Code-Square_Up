#ifndef __CAPSULE_H__
#define __CAPSULE_H__

#include "Behaviour.h"

class Capsule : public Behaviour
{
public:
	Capsule(Gameobject* go);
	~Capsule();
	void Update() override;
	void AfterDamageAction() override;
};

#endif 
