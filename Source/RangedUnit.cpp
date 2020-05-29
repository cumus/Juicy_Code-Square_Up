#include "RangedUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "ParticleSystem.h"

RangedUnit::RangedUnit(Gameobject* go) : B_Unit(go, UNIT_RANGED, IDLE, B_RANGED)
{
	//Stats
	max_life = 100;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 15;
	attack_range = 13.0f;
	vision_range = 20.0f;
	providesVisibility = true;
	//SFX
	deathFX = RANGED_DIE_FX;
	attackFX = RANGED_ATK_FX;
	SetColliders();
}

RangedUnit::~RangedUnit() {}

void RangedUnit::UnitAttackType()
{
	attackPos = atkObj->GetPos();
	App->particleSys.AddParticle(pos,attackPos,8.0f, ORANGE_PARTICLE);
}