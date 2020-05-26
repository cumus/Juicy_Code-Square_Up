#include "EnemyMeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"

#include <vector>

EnemyMeleeUnit::EnemyMeleeUnit(Gameobject* go) : B_Unit(go, ENEMY_MELEE, IDLE, B_ENEMY_MELEE)
{
	//Stats
	current_life = 75;
	max_life = current_life;
	atkTime = 1.25f;
	speed = 3.0f;
	damage = 8;
	attack_range = 2.0f;
	vision_range = 10.0f;
	base_found = false;
	arriveDestination = true;
	providesVisibility = false;
	new_state = IDLE;

	SetColliders();
	//SFX
	deathFX = IA_MELEE_DIE_FX;
	attackFX = IA_MELEE_ATK_FX;
	//characteR->SetInactive();
}

EnemyMeleeUnit::~EnemyMeleeUnit() {}