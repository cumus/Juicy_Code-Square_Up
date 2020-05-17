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

EnemyMeleeUnit::EnemyMeleeUnit(Gameobject* go) : B_Unit(go, ENEMY_MELEE, IDLE, B_ENEMY_MELEE)
{
	//Stats
	current_life = 75;
	max_life = current_life;
	atkTime = 1.25f;
	speed = 3.0f;
	damage = 8;
	attack_range = 3.0f;
	vision_range = 10.0f;
	inVision = false;
	inRange = false;
	base_found = false;
	arriveDestination = true;
	providesVisibility = false;
	new_state = IDLE;

	SetColliders();
	//SFX
	//deathFX = IA_MELEE_DIE_FX;
	//attackFX = IA_MELEE_ATK_FX;
	//characteR->SetInactive();
}

EnemyMeleeUnit::~EnemyMeleeUnit()
{
}

/*void EnemyMeleeUnit::SetColliders()
{
	//Colliders
	pos = game_object->GetTransform()->GetGlobalPosition();
	bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, ENEMY_TAG, { 0,Map::GetBaseOffset(),0,0 },BODY_COLL_LAYER);	
	//attackColl = new Collider(game_object, { pos.x,pos.y,attack_range,attack_range }, TRIGGER, ENEMY_ATTACK_TAG, { 0,Map::GetBaseOffset(),0,0 },ATTACK_COLL_LAYER);
	//visionColl = new Collider(game_object, { pos.x,pos.y,vision_range,vision_range }, TRIGGER, ENEMY_VISION_TAG, { 0,Map::GetBaseOffset(),0,0 },VISION_COLL_LAYER);
}*/

/*void EnemyMeleeUnit::UpdatePath(int x, int y)
{
	if (x >=0 && y >= 0)
	{
		Transform* t = game_object->GetTransform();
		vec pos = t->GetGlobalPosition();
		path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { x, y }, GetID());
		
		next = false;
		move = false;

		if (!tilesVisited.empty())
		{
			for (std::vector<iPoint>::const_iterator it = tilesVisited.cbegin(); it != tilesVisited.cend(); ++it)
			{
				if (PathfindingManager::unitWalkability[it->x][it->y] != 0.0f)
				{
					PathfindingManager::unitWalkability[it->x][it->y] = 0.0f;	
					//LOG("Clear tiles");
				}
			}
			tilesVisited.clear();
		}
	}
}*/


/*void EnemyMeleeUnit::Update()
{
	if (!providesVisibility) CheckFoWMap();
	if (current_state != DESTROYED)
	{
		Transform* t = game_object->GetTransform();
		if (t)
		{
			vec pos = t->GetGlobalPosition();

			switch (new_state)
			{
			case IDLE:
			{
				if (inRange)
				{
					new_state = ATTACKING;
				}
				else if (inVision)
				{
					new_state = CHASING;
				}
				else
				{
					new_state = BASE;
					objective = nullptr;
				}
				current_state = IDLE;
				break;
			}
			case BASE:
			{
				if (Base_Center::baseCenter != nullptr)
				{
					if (new_state != current_state)
					{
						//LOG("Path to base");
						vec centerPos = Base_Center::baseCenter->GetTransform()->GetGlobalPosition();
						Event::Push(UPDATE_PATH, this->AsBehaviour(), int(centerPos.x) - 1, int(centerPos.y) - 1);
						//going_base = true;
						//arriveDestination = true;
						//LOG("Move to base");	
					}

					if (inRange) new_state = ATTACKING;
					else if (inVision) new_state = CHASING;
				}
				else new_state = IDLE;
				current_state = BASE;
				break;
			}
			case CHASING:
			{
				//LOG("Valid objective");
				//going_base = false;
				attackPos = objective->GetTransform()->GetGlobalPosition();
				//LOG("Distance to enemy: %f", game_object->GetTransform()->DistanceTo(attackPos));
				if (game_object->GetTransform()->DistanceTo(attackPos) > attack_range)
				{
					//LOG("Path to enemy");
					if (arriveDestination)
					{
						Transform* t = objective->GetTransform();
						destPos.first = int(t->GetGlobalPosition().x) - 1;
						destPos.second = int(t->GetGlobalPosition().y) - 1;
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
						if (Pos.first <= destPos.first + 1 && Pos.first >= destPos.first - 1 && Pos.second >= destPos.second - 1 && Pos.second <= destPos.second + 1) arriveDestination = true;
						//LOG("on destination");
					}
					objective = nullptr;
				}
				else
				{
					new_state = ATTACKING;
				}
				current_state = CHASING;
				break;
			}
			case ATTACKING:
			{
				if (atkTimer < atkTime)
				{
					atkTimer += App->time.GetGameDeltaTime();
				}
				else
				{
					if (!objective->BeingDestroyed())
					{
						//LOG("Do attack");
						DoAttack();
						UnitAttackType();
						Event::Push(DAMAGE, objective->GetBehaviour(), damage);
					}
					new_state = IDLE;
					objective = nullptr;
					atkTimer = 0;
				}
				current_state = ATTACKING;
				break;
			}
			}
			if (path != nullptr && !path->empty() && !inRange) //Movement
			{
				//LOG("moving");		
				CheckPathTiles();
			}
			else
			{
				move = false;
				//arriveDestination = true;
				current_state = IDLE;
			}

			//LOG("Tile ID: %f", PathfindingManager::unitWalkability[nextTile.x][nextTile.y]);
			if (move && PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == GetID())
			{
				//LOG("move");
				fPoint actualPos = { pos.x, pos.y };

				iPoint tilePos = { int(pos.x), int(pos.y) };
				if (nextTile.x > tilePos.x)
				{
					dirX = 1;
				}
				else if (nextTile.x < tilePos.x)
				{
					dirX = -1;
				}
				else dirX = 0;

				if (nextTile.y > tilePos.y)
				{
					dirY = 1;
				}
				else if (nextTile.y < tilePos.y)
				{
					dirY = -1;
				}
				else dirY = 0;

				game_object->GetTransform()->MoveX(dirX * speed * App->time.GetGameDeltaTime());//Move x
				game_object->GetTransform()->MoveY(dirY * speed * App->time.GetGameDeltaTime());//Move y

				ChangeState();
				CheckDirection(actualPos);
				//App->fogWar.MapNeedsUpdate();
			}

			//Raycast
			if (shoot) ShootRaycast();

			//Draw vision and attack range
			if (drawRanges) DrawRanges();
		}
	}
}*/

/*void EnemyMeleeUnit::OnCollisionEnter(Collider selfCol, Collider col)
{
	//LOG("Got coll");
	//LOG("Coll tag :%d", selfCol.GetColliderTag());
	//LOG("Coll tag :%d", col.GetColliderTag());
	if (selfCol.GetColliderTag() == ENEMY_ATTACK_TAG)
	{
		//LOG("Atk");
		//LOG("Coll tag :%d", selfCol.GetColliderTag());
		//LOG("Coll tag :%d", col.GetColliderTag());
		if (col.GetColliderTag() == PLAYER_TAG)
		{
			LOG("Player unit in attack range");
			inRange = true;
			//inVision = false;
			if (objective == nullptr) objective = col.GetGameobject();
		}
	}

	if (selfCol.GetColliderTag() == ENEMY_VISION_TAG)
	{
		//LOG("Vision");
		//LOG("Coll tag :%d", selfCol.GetColliderTag());
		//LOG("Coll tag :%d", col.GetColliderTag());
		if (col.GetColliderTag() == PLAYER_TAG)
		{
			LOG("Player unit in vision");
			//inRange = false;
			inVision = true;
			if (attackObjective == nullptr) objective = col.GetGameobject();
		}
	}	
}*/

/*void EnemyMeleeUnit::OnCollisionExit(Collider selfCol, Collider col)
{

	if (selfCol.GetColliderTag() == ENEMY_ATTACK_TAG)
	{
		if (col.GetColliderTag() == PLAYER_TAG)
		{
			inRange = false;
		}
	}

	if (selfCol.GetColliderTag() == ENEMY_VISION_TAG)
	{
		if (col.GetColliderTag() == PLAYER_TAG)
		{
			inVision = false;
		}
	}
}*/