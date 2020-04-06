#pragma once
#ifndef __TOWER_H__
#define __TOWER_H__

#include "Behaviour.h"

class Tower : public Behaviour
{
public:
	Tower(Gameobject* go);
	~Tower();


	void OnDamage(int damage) override;
	void OnKill() override;
};

#endif // __TOWER_H__