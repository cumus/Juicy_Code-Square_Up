#ifndef __EDGE_H__
#define __EDGE_H__

#include "Behaviour.h"

class Edge : public Behaviour
{
public:
	Edge(Gameobject* go);
	~Edge();

	void OnRightClick(vec pos, vec modPos) override;
	void AfterDamageAction() override;
};

#endif // __EDGE_H__