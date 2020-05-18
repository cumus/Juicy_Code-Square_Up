#include "Particle.h"
#include "Gameobject.h"
#include "Application.h"
#include "TextureManager.h"
#include "Transform.h"
#include "Sprite.h"
#include "Vector3.h"
#include "Log.h"


Particle::Particle(Gameobject* go,vec p, vec d, float s, bool ply, ComponentType type) : Component(type, go)
{
	destination = d;
	speed = s;
	goParent = go;
	alive = true;

	animCounter = 0;
	animationSpeed = 0.8f;
	spriteNum = 0;
	player = ply;
	if(player) img = new Sprite(go, App->tex.Load("Assets/textures/ally-enemy-shot.png"), { 0, 0, 30, 30 }, BACK_SCENE, { 0, -50, 1.f, 1.f });
	else img = new Sprite(go, App->tex.Load("Assets/textures/ally-enemy-shot.png"), { 0, 32, 30, 30 }, BACK_SCENE, { 0, -50, 1.f, 1.f });
	direction = {abs(p.x- destination.x),abs(p.y- destination.y),0};
	velocityMod = { direction.x / direction.y, direction.y/direction.x };
	t = go->GetTransform();
}

Particle::~Particle()
{}


void Particle::Update()
{
	float d = (t->GetGlobalPosition().x - destination.x)+(t->GetGlobalPosition().y - destination.y);
	if (d > 1)
	{
		game_object->GetTransform()->MoveX(velocityMod.first * speed * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(velocityMod.second * speed * App->time.GetGameDeltaTime());//Move y

		if (animCounter < animationSpeed)
		{
			if (spriteNum < 8) spriteNum++;
			else spriteNum = 0;

			if(player) img->SetSection({30 * spriteNum,0,30,30});
			else img->SetSection({30 * spriteNum,32,30,30});
			animCounter = 0;
		}
		else
		{
			animCounter += App->time.GetGameDeltaTime();
		}
	}
	else
	{
		img->SetInactive();
		alive = false;
	}
}


vec Particle::GetPos() { return goParent->GetTransform()->GetGlobalPosition(); }

bool Particle::IsAlive() { return alive; }