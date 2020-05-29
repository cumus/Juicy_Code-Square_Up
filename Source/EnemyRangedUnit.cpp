#include "EnemyRangedUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "ParticleSystem.h"

EnemyRangedUnit::EnemyRangedUnit(Gameobject* go) : B_Unit(go, ENEMY_RANGED, IDLE, B_RANGED)
{
	//Stats
	max_life = 35;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 10;
	attack_range = 10.0f;
	vision_range = 15.0f;
	providesVisibility = false;
	//SFX
	deathFX = IA_RANGED_DIE_FX;
	attackFX = RANGED_ATK_FX;
	SetColliders();
}

EnemyRangedUnit::~EnemyRangedUnit() {}

void EnemyRangedUnit::UnitAttackType()
{
	attackPos = atkObj->GetPos();
	App->particleSys.AddParticle(pos, attackPos, 8.0f, PURPLE_PARTICLE);
	App->audio->PlayFx(attackFX);
}