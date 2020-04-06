#ifndef __BASECENTER_H__
#define __BASECENTER_H__

#include "Behaviour.h"

class Base_Center : public Behaviour
{
public:
	Base_Center(Gameobject* go);
	~Base_Center();


	void OnDamage(int damage) override;
	void OnKill() override;
};

#endif // __BASECENTER_H__