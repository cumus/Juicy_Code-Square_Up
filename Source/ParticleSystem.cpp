#include "ParticleSystem.h"
#include "Application.h"
#include "Scene.h"
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
	if (!particles.empty())
	{
		for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
		{
			if ((*it)->IsAlive())
			{
				(*it)->Update();
				aliveCache.push_back(*it);
			}
			else deathCache.push_back(*it);			
		}
	}

	if (!deathCache.empty())
	{
		for (std::vector<Particle*>::iterator it = deathCache.begin(); it != deathCache.end(); ++it)
		{
			(*it)->GetGameobject()->Destroy();
			//LOG("Destroy particle");
		}
		deathCache.clear();
	}
	particles = aliveCache;
	aliveCache.clear();
}

void ParticleSystem::CleanUp()
{
	if (!particles.empty())
	{
		for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
		{
			(*it)->GetGameobject()->Destroy();
		}
		particles.clear();
	}

	for (std::vector<Particle*>::iterator it = aliveCache.begin(); it != aliveCache.end(); ++it) delete *it;
	
	for (std::vector<Particle*>::iterator it = deathCache.begin(); it != deathCache.end(); ++it) delete* it;
	
	aliveCache.clear();
	deathCache.clear();
}

void ParticleSystem::AddParticle(vec p, vec dest, float speed, ParticleType t)
{
	if (particles.size() < MAX_PARTICLES)
	{
		//LOG("Create new particle");
		Gameobject* part = App->scene->AddGameobject("Particle");
		part->GetTransform()->SetLocalPos(p);
		Particle* particle = new Particle(part, p, dest, speed, t);
		particles.push_back(particle);
		//particlesID++;
	}
}
