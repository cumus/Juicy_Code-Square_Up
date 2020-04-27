#include "MeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"

MeleeUnit::MeleeUnit(Gameobject* go) : B_Unit(go, UNIT_MELEE, IDLE, B_MELEE_UNIT)
{
	//Stats	
	max_life = 100;
	current_life = max_life;
	atkDelay = 1.0;
	speed = 3;
	damage = 5;
	attack_range = 2.0f;
	vision_range = 10.0f;
	providesVisibility = true;
	//SFX
	deathFX = MELEE_DIE_FX;
	attackFX = MELEE_ATK_FX;
}

MeleeUnit::~MeleeUnit()
{
	
}


void MeleeUnit::IARangeCheck()
{
	if (GetState() != DESTROYED)
	{
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(pos.x, pos.y, 0.5f), vision_range, out);//Get units in vision range
		float distance = 0;
		if (total_found > 0)//Check if found behaviours in range
		{
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED ||
					it->second->GetType() == ENEMY_SUPER || it->second->GetType() == ENEMY_SPECIAL) //Check if it is an emey
				{
					Behaviour::enemiesInSight.push_back(it->second->GetID());
					//LOG("Added to in sight");
				}
			}
		}
	}
}