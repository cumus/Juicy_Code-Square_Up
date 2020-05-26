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
	animationSpeed = 1.5f;
	spriteNum = 0;
	type = typ;
	direction = { d.x - p.x,d.y - p.y};
	float normal = sqrt(pow(direction.x,2) + pow(direction.y,2));
	direction = {direction.x/normal,direction.y/normal};
	//velocityMod = {1,1};//{ direction.x / direction.y, direction.y / direction.x };
	//LOG("Direction X:%f/Y:%f",direction.x,direction.y);
	//LOG("Velocity mod X:%f/Y:%f", velocityMod.first, velocityMod.second);
	t = go->GetTransform();

	
	switch (type)
	{
		case ORANGE_PARTICLE:
			texID = App->tex.Load("Assets/textures/particle_shot.png");
			img = new Sprite(go, texID, { 0, 0, 30, 31 }, FRONT_SCENE, {0.0f,0.0f,1.0f,1.0f});
			animationSprites = 8;
			break;
		case PURPLE_PARTICLE:
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
	float d = (abs(t->GetGlobalPosition().x - destination.x))+(abs(t->GetGlobalPosition().y - destination.y));
	if (d > 1)
	{
		/*if(destination.x > t->GetGlobalPosition().x) game_object->GetTransform()->MoveX(velocityMod.first * speed * App->time.GetGameDeltaTime());//Move x
		else game_object->GetTransform()->MoveX(-velocityMod.first * speed * App->time.GetGameDeltaTime());//Move x

		if (destination.y > t->GetGlobalPosition().y) game_object->GetTransform()->MoveY(velocityMod.second * speed * App->time.GetGameDeltaTime());//Move y
		else game_object->GetTransform()->MoveY(-velocityMod.second * speed * App->time.GetGameDeltaTime());//Move y
		*/
		game_object->GetTransform()->MoveX(direction.x * speed * App->time.GetGameDeltaTime());//Move X
		game_object->GetTransform()->MoveY(direction.y * speed * App->time.GetGameDeltaTime());//Move Y


		if (animCounter < animationSpeed)
		{
			if (spriteNum < animationSprites) spriteNum++;
			else spriteNum = 0;

			switch (type)
			{
			case ORANGE_PARTICLE:
				img->SetSection({ 30 * spriteNum,0,30,31 });
				break;
			case PURPLE_PARTICLE:
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
		//LOG("Img inactive");
		//img->SetInactive();
		alive = false;
	}
}


vec Particle::GetPos() { return t->GetGlobalPosition(); }

bool Particle::IsAlive() { return alive; }

ParticleType Particle::GetType() { return type; }
