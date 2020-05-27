#ifndef __CAPSULE_H__
#define __CAPSULE_H__

#include "Behaviour.h"

class Capsule : public Behaviour
{
public:
	Capsule(Gameobject* go);
	~Capsule();
	
	
	void AfterDamageAction(UnitType from) override;
	void Update() override;

public:
	bool gives_edge;
};

#endif 
