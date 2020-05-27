#ifndef __SPAWNER_H__
#define __SPAWNER_H__

#include "Behaviour.h"

#define MELEE_RATE 30
#define RANGED_RATE 30
#define SUPER_RATE 30
#define SPECIAL_RATE 10

class Spawner : public Behaviour
{
public:
	Spawner(Gameobject* go);
	~Spawner();

	void Update() override;
	void ToggleSpawn(bool toggle) { shoot = toggle; }
	void ResetSpawner();
	void ChangeValues(int spawns,float cooldown, int spawnPoints);
	

	

private:
	float ms_counter,cooldown;
	int maxSpawns, currentSpawns;
};

#endif // __SPAWNER_H__