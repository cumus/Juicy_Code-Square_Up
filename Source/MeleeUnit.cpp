#include "MeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"

MeleeUnit::MeleeUnit(Gameobject* go) : B_Unit(go, UNIT_MELEE, IDLE, B_UNIT)
{
	//Stats	
	max_life = 100;
	current_life = max_life;
	atkDelay = 1.0;
	speed = 3;
	damage = 5;
	attack_range = 2.0f;
	vision_range = 10.0f;

	//SFX
	//deathFX = MELEE_DIE_FX;
	//attackFX = MELEE_ATK_FX;
}

MeleeUnit::~MeleeUnit()
{
	
}
