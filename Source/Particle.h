#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Point.h"
#include "SDL/include/SDL.h"

#define MIN_LIFE_TO_INTERPOLATE 15

struct SDL_Texture;


class Particle
{
private:

	int life = 0;

	struct Vortex
	{
		fPoint pos = { 0.0f, 0.0f };
		float speed = 0.0f;
		float scale = 0.0f;
	} vortex;

	union ParticleInfo
	{
		struct ParticleState
		{
			int startLife;
			fPoint pos;
			fPoint startVel;
			fPoint endVel;
			fPoint currentVel;
			float currentSize, startSize, endSize;
			float ageRatio;
			float angle;
			double startRotSpeed;
			double currentRotSpeed;
			SDL_Rect pRect;
			SDL_Rect rectSize;
			SDL_Color startColor;
			SDL_Color endColor;
			SDL_BlendMode blendMode;
			bool vortexSensitive;
			float t;

			ParticleState() {}

		} pLive;
		Particle* next;
		ParticleInfo() {}
	} pState;

public:

	Particle();
	~Particle();

	// Initializes new generated particle
	void Init(fPoint pos, float startSpeed, float endSpeed, float angle, double rotSpeed, float startSize, float endSize, int life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode, bool vortexSensitive);

	// Generic methods
	void Update();
	bool Draw();
	bool IsAlive();

	// Particle pointer methods
	Particle* GetNext();
	void SetNext(Particle* next);

	// Given two colors interpolates linearly over time and returns the resulting color
	SDL_Color RgbInterpolation(SDL_Color startColor, float timeStep, SDL_Color endColor);

	float InterpolateBetweenRange(float min, float timeStep, float max);

	// Adds a vortex to the system
	void AddVortex(fPoint pos, float speed, float scale);

	/* Calculates particle position considering its velocity
	   and if there's a vortex in the system */
	void CalculateParticlePos();
};

#endif