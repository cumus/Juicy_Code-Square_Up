#include "EnemyMeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"


EnemyMeleeUnit::EnemyMeleeUnit(Gameobject* go) : B_Unit(go, ENEMY_MELEE, IDLE, B_UNIT)
{
	//Stats
	current_life = 75;
	max_life = current_life;
	atkDelay = 1.25;
	speed = 4;
	damage = 7;
	attack_range = 2.0f;
	vision_range = 10.0f;
	going_base = false;
	going_enemy = false;

	if (App->scene->baseCenterPos.first != -1)
	{
		UpdatePath(App->scene->baseCenterPos.first, App->scene->baseCenterPos.first);
	}

	//SFX
	//deathFX = IA_MELEE_DIE_FX;
	//attackFX = IA_MELEE_ATK_FX;
}

EnemyMeleeUnit::~EnemyMeleeUnit()
{
	/*Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());*/
}

void EnemyMeleeUnit::UpdatePath(int x, int y)
{
	if (x != -1 && y != -1)
	{
		Transform* t = game_object->GetTransform();
		vec pos = t->GetGlobalPosition();
		path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { x, y }, GetID());
	}
}


void EnemyMeleeUnit::IARangeCheck()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		next = false;
		move = false;

		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(pos.x, pos.y, 0.5f), vision_range, out);//Get units in vision range
		float distance = 0;
		if (total_found > 0)
		{
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (it->second->GetType() != ENEMY_MELEE && it->second->GetType() != ENEMY_RANGED &&
					 it->second->GetType() != ENEMY_SUPER && it->second->GetType() != ENEMY_SPECIAL && it->second->GetType() != SPAWNER )//Check if not enemy unit
				{
					if (distance == 0)//Not set closest unit yet
					{
						attackObjective = it->second;
						distance = it->first;
					}
					else
					{
						if (it->first < distance)//Update closest unit
						{
							distance = it->first;
							attackObjective = it->second;
						}
					}

				}
			}
		}
		else
		{
			attackObjective = nullptr;
		}

		if (attackObjective != nullptr)//Check if there is a valid objective
		{
			attackPos = attackObjective->GetGameobject()->GetTransform()->GetGlobalPosition();
			if (game_object->GetTransform()->DistanceTo(attackPos) > attack_range)
			{
				//if (!going_enemy)
				//{
					Transform* t = attackObjective->GetGameobject()->GetTransform();
					Event::Push(UPDATE_PATH, this->AsBehaviour(), int(t->GetGlobalPosition().x), int(t->GetGlobalPosition().y));
				//	going_enemy = true;
				//}
				
			}
			/*else
			{
				going_enemy = true;
			}*/
		}
		else
		{
			if (!going_base)//If no valid objective and not going to base, set path to base
			{
				Event::Push(UPDATE_PATH, this->AsBehaviour(), App->scene->baseCenterPos.first, App->scene->baseCenterPos.second);
				going_base = true;
			}
		}
	}
}

/*void EnemyMeleeUnit::CheckSprite()
{
	switch (current_state)
		{
		case IDLE:

			break;
		case MOVING_E:

			break;
		case MOVING_W:

			break;
		case MOVING_S:

			break;
		case MOVING_N:

			break;
		case MOVING_SE:

			break;
		case MOVING_NE:

			break;
		case MOVING_SW:

			break;
		case MOVING_NW:

			break;
		case ATTACKING_E:

			break;
		case ATTACKING_W:

			break;
		case ATTACKING_S:

			break;
		case ATTACKING_N:

			break;
		case ATTACKING_SE:

			break;
		case ATTACKING_NE:

			break;
		case ATTACKING_SW:

			break;
		case ATTACKING_NW:

			break;
		}
}*/

	