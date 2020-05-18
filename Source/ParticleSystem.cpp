#include "ParticleSystem.h"
#include "Particle.h"
#include "Vector3.h"

#include <vector>

ParticleSystem::ParticleSystem()
{}

ParticleSystem::~ParticleSystem()
{}

void ParticleSystem::Start()
{
	particlesID = 0;
}

void ParticleSystem::Update()
{
	for (std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		if (it->IsAlive()) it->Update();
		else
		{
			it->GetGameobject()->Destroy();
			particles.erase(it);
		}
	}
}

void ParticleSystem::CleanUp()
{
	for (std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		it->GetGameobject()->Destroy();
	}
	particles.clear();
}

void ParticleSystem::Draw()
{

}

void ParticleSystem::AddParticle(Gameobject* parent, vec p, vec dest, float speed, bool player)
{
	Gameobject* part = new Gameobject("Particle",parent);
	new Particle(part,p,dest,speed, player);
	//particlesID++;
}

void ParticleSystem::DestroyParticle()
{

}