#include "Particle.h"
#include "Gameobject.h"
#include "Application.h"
#include "TextureManager.h"
#include "Transform.h"
#include "Sprite.h"
#include "Vector3.h"
#include "Log.h"


Particle::Particle(Gameobject* go,vec p, vec d, float s, ParticleType typ, ComponentType cT) : Component(cT, go)
{
	destination = d;
	speed = s;
	alive = true;

	animCounter = 0;
	animationSpeed = 0.8f;
	spriteNum = 0;
	type = typ;
	direction = { abs(p.x - destination.x),abs(p.y - destination.y),0 };
	velocityMod = { direction.x / direction.y, direction.y / direction.x };
	t = go->GetTransform();

	
	switch (type)
	{
	case GREEN_PARTICLE:
		texID = App->tex.Load("Assets/textures/particle_shot.png");
		img = new Sprite(go, texID, { 0, 0, 30, 31 }, FRONT_SCENE, {0.0f,0.0f,1.0f,1.0f});
		animationSprites = 8;
		break;
	case RED_PARTICLE:
		texID = App->tex.Load("Assets/textures/particle_shot.png");
		img = new Sprite(go, texID, { 0, 32, 30, 31 }, FRONT_SCENE, { 0.0f,0.0f,1.0f,1.0f });
		animationSprites = 8;
		break;
	case ENERGY_BALL_PARTICLE:
		texID = App->tex.Load("Assets/textures/energyBall.png");
		img = new Sprite(go, texID, { 0, 0, 59, 60 }, FRONT_SCENE, { 0.0f,0.0f,1.0f,1.0f });
		animationSprites = 16;
		break;
	}	
}

Particle::~Particle()
{}


void Particle::Update()
{
	float d = (t->GetGlobalPosition().x - destination.x)+(t->GetGlobalPosition().y - destination.y);
	if (d > 1)
	{
		//LOG("move particle");
		//LOG("part pos X:%f/Y:%f", t->GetGlobalPosition().x, t->GetGlobalPosition().y);
		game_object->GetTransform()->MoveX(velocityMod.first * speed * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(velocityMod.second * speed * App->time.GetGameDeltaTime());//Move y

		if (animCounter < animationSpeed)
		{
			if (spriteNum < animationSprites) spriteNum++;
			else spriteNum = 0;

			switch (type)
			{
			case GREEN_PARTICLE:
				img->SetSection({ 30 * spriteNum,0,30,30 });
				break;
			case RED_PARTICLE:
				img->SetSection({ 30 * spriteNum,32,30,30 });
				break;
			case ENERGY_BALL_PARTICLE:
				img->SetSection({ 59 * spriteNum,0,59,60 });
				break;
			}
			animCounter = 0;
		}
		else
		{
			animCounter += App->time.GetGameDeltaTime();
		}
	}
	else
	{
		LOG("Img inactive");
		//img->SetInactive();
		//alive = false;
	}
}


vec Particle::GetPos() { return t->GetGlobalPosition(); }

bool Particle::IsAlive() { return alive; }

ParticleType Particle::GetType() { return type; }
