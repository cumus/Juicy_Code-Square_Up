#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "Particle.h"
#include "Vector3.h"
#include "Gameobject.h"

#include <vector>

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Start();
	void Update();
	void CleanUp();
	void Draw();
	void AddParticle(vec pos,vec dest,float speed, ParticleType t);
	void DestroyParticle();

private:
	std::vector<Particle> particles;
	int particlesID;
};

#endif