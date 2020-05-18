#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Sprite.h"
#include "SDL/include/SDL.h"
#include "Vector3.h"

#define MIN_LIFE_TO_INTERPOLATE 15


class Particle : public Component
{
public:

	Particle (Gameobject* game_object,vec pos, vec destination,float speed,bool player, ComponentType type = PARTICLE);
	~Particle();

	void Update();
	bool IsAlive();
	vec GetPos();

private:

	vec destination;
	vec direction;
	std::pair<float, float> velocityMod;
	float speed;
	bool alive;
	Gameobject* goParent;
	Sprite* img;
	Transform* t;
	float animationSpeed;
	float animCounter;
	bool player;
	int spriteNum;
};

#endif