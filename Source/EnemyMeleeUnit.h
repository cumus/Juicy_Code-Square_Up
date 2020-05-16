#ifndef __ENEMYMELEEUNIT_H__
#define __ENEMYMELEEUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class EnemyMeleeUnit : public B_Unit
{
public:
	EnemyMeleeUnit(Gameobject* go);
	~EnemyMeleeUnit();
	//void UpdatePath(int x,int y) override;
	//void IARangeCheck() override;
	//void OnCollisionEnter(Collider selfCol, Collider col) override;
	//void OnCollisionStay(Collider selfCol, Collider col) override;
	//void OnCollisionExit(Collider selfCol, Collider col) override;
	//void SetColliders();
	//void Update() override;

protected:
	bool base_found;
	Gameobject* baseCenter;
	
};

#endif
