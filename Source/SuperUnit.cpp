#include "SuperUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "ParticleSystem.h"

SuperUnit::SuperUnit(Gameobject* go) : B_Unit(go, UNIT_SUPER, IDLE, B_RANGED)
{
	//Stats
	max_life = 100;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 15;
	attack_range = 10.0f;
	vision_range = 15.0f;
	providesVisibility = true;
	//SFX
	deathFX = SUPER_DIE_FX;
	attackFX = SUPER_ATK_FX;
	SetColliders();
}

SuperUnit::~SuperUnit() {}

void SuperUnit::UnitAttackType()
{
	attackPos = atkObj->GetPos();
	App->particleSys.AddParticle(pos, attackPos, 1.0f, ORANGE_PARTICLE);
}