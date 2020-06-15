#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "Particle.h"
#include "Vector3.h"
#include "Gameobject.h"

#include <vector>
#include <list>

#define MAX_PARTICLES 1000

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Start();
	void Update();
	void CleanUp();
	void AddParticle(vec pos,vec dest,float speed, ParticleType t);

private:

	std::vector<Particle*> particles;
	std::vector<Particle*> aliveCache;
	std::vector<Particle*> deathCache;
	int particlesID;
};

#endif