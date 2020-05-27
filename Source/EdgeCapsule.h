#ifndef __CAPSULE_H__
#define __CAPSULE_H__

#include "Behaviour.h"

class Capsule : public Behaviour
{
public:
	Capsule(Gameobject* go);
	~Capsule();
	
	
	void AfterDamageAction() override;
	void Update() override;
	void FreeWalkabilityTiles() override;
};

#endif 
