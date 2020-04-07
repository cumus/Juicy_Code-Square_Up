#ifndef __GATHERER_H__
#define __GATHERER_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class Gatherer : public B_Unit
{
public:
	Gatherer(Gameobject* go);
	~Gatherer();
};

#endif