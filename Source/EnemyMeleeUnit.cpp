#include "EnemyMeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "BaseCenter.h"

#include <vector>

EnemyMeleeUnit::EnemyMeleeUnit(Gameobject* go) : B_Unit(go, ENEMY_MELEE, IDLE, B_UNIT)
{
	//Stats
	current_life = 75;
	max_life = current_life;
	atkDelay = 1.25;
	speed = 3.0f;
	damage = 8;
	attack_range = 2.0f;
	vision_range = 10.0f;
	going_base = false;
	going_enemy = false;
	base_found = false;
	arriveDestination = true;
	scanTimer = 0.0f;
	scanTime = 3.0f;

	//SFX
	//deathFX = IA_MELEE_DIE_FX;
	//attackFX = IA_MELEE_ATK_FX;
}

EnemyMeleeUnit::~EnemyMeleeUnit()
{
}

void EnemyMeleeUnit::UpdatePath(int x, int y)
{
	if (x >=0 && y >= 0)
	{
		Transform* t = game_object->GetTransform();
		vec pos = t->GetGlobalPosition();
		path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { x, y }, GetID());

		for (std::vector<iPoint>::const_iterator it = tilesVisited.cbegin(); it != tilesVisited.cend(); ++it)
		{
			if (PathfindingManager::unitWalkability[nextTile.x][nextTile.y] != 0)
			{
				PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0;
			}
		}
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
		if (total_found > 0 /*&& scanTimer > scanTime*/)//Check if found behaviours in range
		{
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (it->second->GetType() != ENEMY_MELEE && it->second->GetType() != ENEMY_RANGED && it->second->GetType() != EDGE &&
					it->second->GetType() != ENEMY_SUPER && it->second->GetType() != ENEMY_SPECIAL && it->second->GetType() != SPAWNER) //Check if not enemy unit
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
		else //Not found
		{
			attackObjective = nullptr;
			scanTimer += App->time.GetGameDeltaTime();
		}

		if (attackObjective != nullptr)//Check if there is a valid objective
		{
			//LOG("Valid objective");
			going_base = false;
			attackPos = attackObjective->GetGameobject()->GetTransform()->GetGlobalPosition();
			//LOG("Distance to enemy: %f", game_object->GetTransform()->DistanceTo(attackPos));
			if (game_object->GetTransform()->DistanceTo(attackPos) > attack_range)
			{
				//LOG("Path to enemy");
				if (arriveDestination)
				{
					Transform* t = attackObjective->GetGameobject()->GetTransform();
					destPos.first = int(t->GetGlobalPosition().x)-1;
					destPos.second = int(t->GetGlobalPosition().y)-1;
					Event::Push(UPDATE_PATH, this->AsBehaviour(), int(t->GetGlobalPosition().x - 1), int(t->GetGlobalPosition().y - 1));
					arriveDestination = false;
					//LOG("repath");
				}
				else
				{
					vec localPos = game_object->GetTransform()->GetGlobalPosition();
					std::pair<int, int> Pos(int(localPos.x), int(localPos.y));
					//LOG("Pos X:%d/Y:%d", Pos.first, Pos.second);
					//LOG("DestPos X:%d/Y:%d", destPos.first, destPos.second);
					if (Pos.first <= destPos.first+1 && Pos.first >= destPos.first -1 && Pos.second >= destPos.second-1 && Pos.second <= destPos.second+1) arriveDestination = true;
				}
				attackObjective = nullptr;				
			}			
		}
		else
		{
			if (!going_base && Base_Center::baseCenter != nullptr)//If no valid objective and not going to base, set path to base
			{
				//LOG("Path to base");
				vec centerPos = Base_Center::baseCenter->GetTransform()->GetGlobalPosition();
				Event::Push(UPDATE_PATH, this->AsBehaviour(), int(centerPos.x) - 1, int(centerPos.y) - 1);
				going_base = true;
				arriveDestination = true;
				//LOG("Move to base");
			}
		}
	}
}