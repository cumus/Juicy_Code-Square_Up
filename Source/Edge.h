#ifndef __EDGE_H__
#define __EDGE_H__

#include "Behaviour.h"

class Edge : public Behaviour
{
public:
	Edge(Gameobject* go);
	~Edge();

	void AfterDamageAction() override;
	void FreeWalkabilityTiles() override;
	void Update() override;
};

#endif // __EDGE_H__