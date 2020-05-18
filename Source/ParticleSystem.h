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
	void AddParticle(Gameobject* parent,vec pos,vec dest,float speed,bool player);
	void DestroyParticle();

private:
	std::vector<Particle> particles;
	int particlesID;
};

#endif