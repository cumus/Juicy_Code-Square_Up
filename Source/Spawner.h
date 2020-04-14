#ifndef __SPAWNER_H__
#define __SPAWNER_H__

#include "Behaviour.h"

class Spawner : public Behaviour
{
public:
	Spawner(Gameobject* go);
	~Spawner();

	void SpawnMelee(float x, float y);
	void SpawnRanged(float x, float y);
	void SpawnSuper(float x, float y);
	void SpawnSpecial(float x, float y);
};

#endif // __SPAWNER_H__