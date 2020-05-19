#include "ParticleSystem.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Particle.h"
#include "Vector3.h"
#include "Log.h"

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

void ParticleSystem::AddParticle(vec p, vec dest, float speed, bool player)
{
	LOG("Create new particle");
	Gameobject* part = new Gameobject("Particle");
	part->GetTransform()->SetLocalPos(p);
	new Particle(part,p,dest,speed, player);
	//particlesID++;
}

void ParticleSystem::DestroyParticle()
{

}