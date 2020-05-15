#ifndef __EDGE_CAPSULE_H__
#define __EDGE_CAPSULE_H__

#include "Behaviour.h"

class Edge_Capsule : public Behaviour
{
public:
	Edge_Capsule(Gameobject* go);
	~Edge_Capsule();

	void AfterDamageAction() override;
};

#endif 
