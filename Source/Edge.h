#ifndef __EDGE_H__
#define __EDGE_H__

#include "Behaviour.h"

class Edge : public Behaviour
{
public:
	Edge(Gameobject* go);
	~Edge();

	void OnRightClick(float x, float y) override;
	void OnDamage(int damage) override;
	void OnKill() override;
};

#endif // __EDGE_H__