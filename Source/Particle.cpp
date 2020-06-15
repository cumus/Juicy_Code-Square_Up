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
	animationSpeed = 0.01f;
	spriteNum = 0;
	type = typ;
	direction = { d.x - p.x,d.y - p.y};
	float normal = sqrt(pow(direction.x,2) + pow(direction.y,2));
	direction = {direction.x/normal,direction.y/normal};
	t = go->GetTransform();

	switch (type)
	{
		case ORANGE_PARTICLE:
			texID = App->tex.Load("textures/particle_shot.png");
			img = new Sprite(go, texID, { 0, 0, 30, 30 }, FRONT_SCENE, {0.0f,0.0f,1.0f,1.0f});
			animationSprites = 8;
			break;
		case PURPLE_PARTICLE:
			texID = App->tex.Load("textures/particle_shot.png");
			img = new Sprite(go, texID, { 0, 31, 30, 30 }, FRONT_SCENE, { 0.0f,0.0f,1.0f,1.0f });
			animationSprites = 8;
			break;
		case ENERGY_BALL_PARTICLE:
			texID = App->tex.Load("textures/Energy_Ball.png");
			img = new Sprite(go, texID, { 0, 0, 60, 60 }, FRONT_SCENE, { 0.0f,0.0f,1.0f,1.0f });
			animationSprites = 16;
			break;
	}	
}

Particle::~Particle()
{}


void Particle::Update()
{
	float d = (abs(t->GetGlobalPosition().x - destination.x))+(abs(t->GetGlobalPosition().y - destination.y));
	if (d > 1)
	{
		game_object->GetTransform()->MoveX(direction.x * speed * App->time.GetGameDeltaTime());//Move X
		game_object->GetTransform()->MoveY(direction.y * speed * App->time.GetGameDeltaTime());//Move Y

		if (animCounter > animationSpeed)
		{
			if (spriteNum < animationSprites) spriteNum++;
			else spriteNum = 0;

			switch (type)
			{
			case ORANGE_PARTICLE:
				img->SetSection({ 30 * spriteNum,0,30,30 });
				break;
			case PURPLE_PARTICLE:
				img->SetSection({ 30 * spriteNum,31,30,30 });
				break;
			case ENERGY_BALL_PARTICLE:
				img->SetSection({ 60 * spriteNum,0,60,60 });
				break;
			}
			animCounter = 0;
		}
		else
			animCounter += App->time.GetGameDeltaTime();
	}
	else
		alive = false;
}


vec Particle::GetPos() { return t->GetGlobalPosition(); }

bool Particle::IsAlive() { return alive; }

ParticleType Particle::GetType() { return type; }
