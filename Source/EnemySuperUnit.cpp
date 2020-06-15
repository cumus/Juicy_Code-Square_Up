#include "EnemySuperUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "ParticleSystem.h"

EnemySuperUnit::EnemySuperUnit(Gameobject* go) : B_Unit(go, ENEMY_SUPER, IDLE, B_RANGED)
{
	//Stats
	max_life = 75;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 15;
	attack_range = 10.0f;
	vision_range = 15.0f;
	providesVisibility = false;

	//SFX
	deathFX = IA_SUPER_DIE_FX;
	attackFX = IA_SUPER_ATK_FX;
	SetColliders();
}

EnemySuperUnit::~EnemySuperUnit()
{}

void EnemySuperUnit::UnitAttackType()
{
	attackPos = atkObj->GetPos();
	App->particleSys.AddParticle(pos, attackPos, 8.0f, PURPLE_PARTICLE);
}