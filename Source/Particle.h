#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "SDL/include/SDL.h"
#include "Component.h"
#include "Vector3.h"

#define MIN_LIFE_TO_INTERPOLATE 15

class Sprite;

enum ParticleType
{
	GREEN_PARTICLE,
	RED_PARTICLE,
	ENERGY_BALL_PARTICLE,

	MAX_PARTICLES_TYPES,
};

class Particle : public Component
{
public:

	Particle (Gameobject* game_object,vec pos, vec destination,float speed, ParticleType t, ComponentType type = PARTICLE);
	~Particle();

	void Update();
	bool IsAlive();
	vec GetPos();
	ParticleType GetType();

private:

	vec destination;
	vec direction;
	std::pair<float, float> velocityMod;
	float speed;
	bool alive;
	int texID;
	Sprite* img;
	Transform* t;
	float animationSpeed;
	float animCounter;
	int spriteNum;
	int animationSprites;
	ParticleType type;
};

#endif