#ifndef __BASECENTER_H__
#define __BASECENTER_H__

#include "Behaviour.h"

class Base_Center : public Behaviour
{
public:
	Base_Center(Gameobject* go);
	~Base_Center();

	void SpawnUnit(float x, float y);
	void OnRightClick(float x, float y) override;
	void Upgrade();
	void OnDamage(int damage) override;
	void OnKill() override;

protected:

	int bc_lvl = 1;
	int bc_max_lvl = 5;
	
};

#endif // __BASECENTER_H__