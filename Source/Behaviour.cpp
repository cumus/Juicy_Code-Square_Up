#include "Behaviour.h"
#include "Application.h"
#include "TimeManager.h"
#include "PathfindingManager.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Sprite.h"
#include "AudioSource.h"
#include "Log.h"
#include "Vector3.h"
#include "Canvas.h"
#include "Scene.h"
#include "Audio.h"
#include "MeleeUnit.h"
#include "Gatherer.h"
#include "Tower.h"
#include "BaseCenter.h"
#include "RangedUnit.h"

std::map<double, Behaviour*> Behaviour::b_map;

Behaviour::Behaviour(Gameobject* go, UnitType t, UnitState starting_state, ComponentType comp_type) :
	Component(comp_type, go),
	type(t),
	current_state(starting_state)
{
	current_life = max_life = damage = 10;
	attack_range = vision_range = 5.0f;
	dieDelay = 2.0f;
	deathFX = EDGE_FX; //temp
	rayCastTimer = 0;
	shoot = false;
	selectionPanel = nullptr;

	current_state = IDLE;
	audio = new AudioSource(game_object);
	new AnimatedSprite(this);
	selection_highlight = new Sprite(go, App->tex.Load("Assets/textures/selectionMark.png"), { 0, 0, 64, 64 }, BACK_SCENE, { 0, -32, 1.f, 1.f });
	selection_highlight->SetInactive();

	b_map.insert({ GetID(), this });
}

Behaviour::~Behaviour()
{
	b_map.erase(GetID());
}

void Behaviour::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_PLAY: break;
	case ON_PAUSE: break;
	case ON_STOP: break;
	case ON_SELECT: Selected(); break;
	case ON_UNSELECT: UnSelected(); break;
	case ON_DESTROY: OnDestroy(); break;
	case ON_RIGHT_CLICK: OnRightClick(e.data1.AsVec(), e.data2.AsVec()); break;
	case DAMAGE: OnDamage(e.data1.AsInt()); break;
	case IMPULSE: OnGetImpulse(e.data1.AsFloat(),e.data2.AsFloat()); break;
	case BUILD_GATHERER: BuildGatherer(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case BUILD_RANGED: BuildRanged(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case BUILD_MELEE: BuildMelee(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case BUILD_SUPER: BuildSuper(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case DO_UPGRADE: Upgrade(); break;
	case UPDATE_PATH: UpdatePath(e.data1.AsInt(),e.data2.AsInt()); break;
	}
}

void Behaviour::Selected()
{
	selection_highlight->SetActive();
	//App->audio->PlayFx(SELECT);
	audio->Play(SELECT);
	if (bar_go != nullptr) bar_go->SetActive();
	if (selectionPanel != nullptr) selectionPanel->SetActive();
	if (type == TOWER) {
		if (App->scene->building_bars_created < 4)
			App->scene->building_bars_created++;

		pos_y_HUD = 0.17 + 0.1 * App->scene->building_bars_created;
		bar->target.y = pos_y_HUD;
		portrait->target.y = pos_y_HUD - 0.014f;
		text->target.y = pos_y_HUD - 0.073f;
		red_health->target.y = pos_y_HUD - 0.018f;
		health->target.y = pos_y_HUD - 0.018f;
		health_boarder->target.y = pos_y_HUD - 0.018f;
		upgrades->target.y = pos_y_HUD - 0.018f;
	}
	
}

void Behaviour::UnSelected()
{
	selection_highlight->SetInactive();
	if (bar_go != nullptr) bar_go->SetInactive();
	if (selectionPanel != nullptr) selectionPanel->SetInactive();
	if (type == TOWER) {
		if (App->scene->building_bars_created > 0)
			App->scene->building_bars_created--;
	}
	
}

void Behaviour::BuildGatherer(float x, float y)
{
	Gameobject* gather_go = App->scene->AddGameobject("Gatherer");
	gather_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new Gatherer(gather_go);
	//resources -= 10;
}

void Behaviour::BuildMelee(float x, float y)
{
	Gameobject* melee_go = App->scene->AddGameobject("Melee unit");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new MeleeUnit(melee_go);
}

void Behaviour::BuildRanged(float x, float y)
{
	Gameobject* ranged_go = App->scene->AddGameobject("Ranged unit");
	ranged_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new RangedUnit(ranged_go);
}

void Behaviour::BuildSuper(float x, float y)
{

}

void Behaviour::BuildTower(float x, float y)
{
	Gameobject* tower_go = App->scene->AddGameobject("Defense tower");
	tower_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new Tower(tower_go);
}

void Behaviour::BuildCenter(float x, float y)
{
	Gameobject* center_go = App->scene->AddGameobject("Base center");
	center_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new Base_Center(center_go);
}

void Behaviour::BuildWall(float x, float y) 
{

}

void Behaviour::BuildLab(float x, float y) 
{

}

void Behaviour::BuildBarrack(float x, float y) 
{

}

void Behaviour::OnDamage(int d)
{
	//LOG("Got damage: %d",d);
	if (current_state != DESTROYED && GetType() != SPAWNER)
	{
		if (current_life <= 0) {
			OnKill(type);			
		}
		else current_life -= d;

		LOG("Life: %d", current_life);
		update_health_ui();
		AfterDamageAction();
	}
}

void Behaviour::OnKill(const UnitType type)
{
	current_life = 0;
	LOG("Unit killed");
	
	switch (type)
	{
	case ENEMY_MELEE: App->scene->mob_drop += 5; App->scene->units_killed += 1; LOG("Player Mob Drop Value: %i", App->scene->mob_drop); break;
	case ENEMY_RANGED: App->scene->mob_drop += 10; App->scene->units_killed += 1; LOG("Player Mob Drop Value: %i", App->scene->mob_drop); break;
	case ENEMY_SPECIAL: App->scene->mob_drop += 15; App->scene->units_killed += 1; LOG("Player Mob Drop Value: %i", App->scene->mob_drop); break;
	case ENEMY_SUPER: App->scene->mob_drop += 20; App->scene->units_killed += 1; LOG("Player Mob Drop Value: %i", App->scene->mob_drop); break;
	case BASE_CENTER: Event::Push(GAMEPLAY, this, LOSE); break;
	}
	LOG("Debug Mob Drop Value: %i", App->scene->mob_drop);

	current_state = DESTROYED;
	//App->audio->PlayFx(deathFX);
	audio->Play(deathFX);
	game_object->Destroy(dieDelay);
	if (bar_go) {
		bar_go->Destroy(dieDelay);
	}
}

unsigned int Behaviour::GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const
{
	unsigned int ret = 0;

	for (std::map<double, Behaviour*>::iterator it = b_map.begin(); it != b_map.end(); ++it)
	{
		if (it->first != GetID())
		{
			Transform* t = it->second->game_object->GetTransform();
			if (t)
			{
				float d = t->DistanceTo(pos);
				if (d < dist)
				{
					ret++;
					res.insert({ d, it->second });
				}
			}
		}
	}

	return ret;
}




///////////////////////////
// UNIT BEHAVIOUR
///////////////////////////


B_Unit::B_Unit(Gameobject* go, UnitType t, UnitState s, ComponentType comp_type) :
	Behaviour(go, t, s, comp_type)
{
	//Depending on unit
	atkDelay = 1.0;
	speed = 5;//MAX SPEED 60
	attack_range = 3.0f;
	damage = 5;
	deathFX = UNIT_DIES;
	attackFX = SELECT;

	//Needed
	path = nullptr;
	nextTile;
	next = false;
	move = false;
	positiveX = false;
	positiveY = false;
	dirX = 0;
	dirY = 0;
	inRange = false;
	attackObjective = nullptr;
	msCount = 0;
	arriveDestination = false;
	current_state = IDLE;

	//Info for ranged units constructor
	/*vec pos = game_object->GetTransform()->GetGlobalPosition();
	shootPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	shootPos.first += 30.0f;
	shootPos.second += 20.0f;*/

	switch (t) {
	case UNIT_MELEE:
		App->scene->current_melee_units += 1;
		App->scene->melee_units_created += 1;
		break;
	case UNIT_RANGED:
		App->scene->current_ranged_units += 1;
		App->scene->ranged_units_created += 1;
		break;
	case GATHERER:
		App->scene->current_gatherer_units += 1;
		App->scene->gatherer_units_created += 1;
		break;
	}
}

void B_Unit::Update()
{	
	vec pos = game_object->GetTransform()->GetGlobalPosition();
	if (current_state != DESTROYED)
	{
		IARangeCheck();
		if (attackObjective != nullptr && attackObjective->GetState() != DESTROYED) //Attack
		{
			//LOG("FOUND");
			attackPos = attackObjective->GetGameobject()->GetTransform()->GetGlobalPosition();
			float d = game_object->GetTransform()->DistanceTo(attackPos);
			//LOG("Distance 1:%f",d);
			if (d <= attack_range) //Arriba izquierda
			{
				inRange = true;
				//LOG("In range");
			}
			attackPos.x += attackObjective->GetGameobject()->GetTransform()->GetLocalScaleX();
			attackPos.y += attackObjective->GetGameobject()->GetTransform()->GetLocalScaleY();
			d = game_object->GetTransform()->DistanceTo(attackPos);
			//LOG("Distance 2:%f", d);
			if (d <= attack_range)//Abajo derecha
			{
				inRange = true;
				//LOG("In range");
			}

			attackPos.x -= attackObjective->GetGameobject()->GetTransform()->GetLocalScaleX();
			d = game_object->GetTransform()->DistanceTo(attackPos);
			//LOG("Distance 3:%f", d);
			if (d <= attack_range)//Abajo izquierda
			{
				inRange = true;
				//LOG("In range");
			}

			attackPos.x += attackObjective->GetGameobject()->GetTransform()->GetLocalScaleX();
			attackPos.y -= attackObjective->GetGameobject()->GetTransform()->GetLocalScaleY();
			d = game_object->GetTransform()->DistanceTo(attackPos);
			//LOG("Distance 4:%f", d);
			if (d <= attack_range)//Arriba derecha
			{
				inRange = true;
				//LOG("In range");
			}
			//LOG("%d",inRange);
		}
		else
		{
			attackObjective = nullptr;
			inRange = false;
			//LOG("Not in attack range");
		}

		if (msCount < atkDelay)
		{
			msCount += App->time.GetGameDeltaTime();
		}

		if (inRange)
		{
			//LOG("Unit in range");
			if (msCount >= atkDelay)
			{
				//LOG("Do attack");
				DoAttack();
				UnitAttackType();
				Event::Push(DAMAGE, attackObjective, damage);
				msCount = 0;
			}
		}
		else if (path != nullptr && !path->empty()) //Movement
		{
			//LOG("moving");
			fPoint actualPos = { pos.x, pos.y };

			if (!next)
			{
				nextTile = path->front();
				next = true;
				move = true;
			}

			if (dirX == 1 && dirY == 1)
			{
				if (actualPos.x >= nextTile.x && actualPos.y >= nextTile.y)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == -1 && dirY == -1)
			{
				if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == -1 && dirY == 1)
			{
				if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == 1 && dirY == -1)
			{
				if (actualPos.x >= nextTile.x && actualPos.y <= nextTile.y)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == 0 && dirY == -1)
			{
				if (actualPos.y <= nextTile.y)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == 0 && dirY == 1)
			{
				if (actualPos.y >= nextTile.y)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == 1 && dirY == 0)
			{
				if (actualPos.x >= nextTile.x)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == -1 && dirY == 0)
			{
				if (actualPos.x <= nextTile.x)
				{
					path->erase(path->begin());
					next = false;
				}
			}
			else if (dirX == 0 && dirY == 0)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else
		{
			move = false;
			//arriveDestination = true;
			current_state = IDLE;
		}

		if (move)
		{
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

			//Change state to change sprite
			if (dirX == 1 && dirY == 1)//S
			{
				current_state = MOVING_S;
			}
			else if (dirX == -1 && dirY == -1)//N
			{
				current_state = MOVING_N;
			}
			else if (dirX == 1 && dirY == -1)//E
			{
				current_state = MOVING_E;
			}
			else if (dirX == -1 && dirY == 1)//W
			{
				current_state = MOVING_W;
			}
			else if (dirX == 0 && dirY == 1)//SW
			{
				current_state = MOVING_SW;
			}
			else if (dirX == 1 && dirY == 0)//SE
			{
				current_state = MOVING_SE;
			}
			else if (dirX == 0 && dirY == -1)//NE
			{
				current_state = MOVING_NE;
			}
			else if (dirX == -1 && dirY == 0)//NW
			{
				current_state = MOVING_NW;
			}	
			/*else if (dirX == 0 && dirY == 0)
			{
				current_state = IDLE;
			}*/
		}

		//Colision check
		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(pos, 1.4f, out);
		if (total_found > 0)
		{
			fPoint pos(0, 0);
			pos.x = game_object->GetTransform()->GetGlobalPosition().x;
			pos.y = game_object->GetTransform()->GetGlobalPosition().y;
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				vec otherPos = it->second->GetGameobject()->GetTransform()->GetGlobalPosition();
				fPoint separationSpd(0, 0);
				separationSpd.x = pos.x - otherPos.x;
				separationSpd.y = pos.y - otherPos.y;
				if (it->second->GetState() != DESTROYED)
				{
					if(!move) Event::Push(IMPULSE, it->second->AsBehaviour(), -separationSpd.x/2 , -separationSpd.y/2);
					else Event::Push(IMPULSE, it->second->AsBehaviour(), -separationSpd.x, -separationSpd.y);
				}
			}
		}

		//Raycast
		if (shoot)
		{
			rayCastTimer += App->time.GetGameDeltaTime();
			if (rayCastTimer < RAYCAST_TIME)
			{
				App->render->DrawLine(shootPos, atkObj, { 34,191,255,255 }, FRONT_SCENE, true);
			}
			else
			{
				shoot = false;
				rayCastTimer = 0;
			}
		}
	}
}

void B_Unit::DoAttack()
{
	vec localPos = game_object->GetTransform()->GetGlobalPosition();
	std::pair<int, int> Pos(int(localPos.x),int(localPos.y));
	vec objPos = attackObjective->GetGameobject()->GetTransform()->GetGlobalPosition();
	std::pair<int,int> atkPos(int(objPos.x), int(objPos.y));
	arriveDestination = true;
	//LOG("Pos X:%d/Y:%d", Pos.first, Pos.second);
	//LOG("Atkpos X:%d/Y:%d", atkPos.first, atkPos.second);

	audio->Play(attackFX);
	if (atkPos.first == Pos.first && atkPos.second < Pos.second)//N
	{
		current_state = ATTACKING_N;
	}
	else if (atkPos.first == Pos.first && atkPos.second > Pos.second)//S
	{
		current_state = ATTACKING_S;
	}
	else if (atkPos.first < Pos.first && atkPos.second == Pos.second)//W
	{
		current_state = ATTACKING_W;
	}
	else if (atkPos.first > Pos.first && atkPos.second == Pos.second)//E
	{
		current_state = ATTACKING_E;
	}
	else if (atkPos.first < Pos.first && atkPos.second > Pos.second)//SW
	{
		current_state = ATTACKING_NW;
	}
	else if (atkPos.first > Pos.first && atkPos.second > Pos.second)//
	{
		current_state = ATTACKING_SW;
	}
	else if (atkPos.first < Pos.first && atkPos.second < Pos.second)//
	{
		current_state = ATTACKING_NE;
	}
	else if (atkPos.first > Pos.first && atkPos.second < Pos.second)//
	{
		current_state = ATTACKING_SE; 
	}
}

void B_Unit::OnDestroy()
{
	App->pathfinding.DeletePath(GetID());
	switch (type) {
	case UNIT_MELEE:
		App->scene->current_melee_units -= 1;
		break;
	case UNIT_RANGED:
		App->scene->current_ranged_units -= 1;
		break;
	case GATHERER:
		App->scene->current_gatherer_units -= 1;
		break;
	}
}

void B_Unit::OnRightClick(vec posClick, vec modPos)
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();		
		next = false;
		move = false;

		audio->Play(HAMMER);

		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(posClick.x, posClick.y, 0.5f), 1.5f, out);
		float distance = 0;
		if (total_found > 0)
		{
			LOG("Unit cliked");
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (GetType() == GATHERER)
				{
					if (it->second->GetType() == EDGE)
					{
						if (distance == 0)//Chose closest
						{
							attackObjective = it->second;
							distance = it->first;
						}
						else
						{
							if (it->first < distance)
							{
								distance = it->first;
								attackObjective = it->second;
							}
						}

					}
				}
				else if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED)//Temporal
				{
					if (distance == 0)//Closest distance
					{
						attackObjective = it->second;
						distance = it->first;
					}
					else
					{
						if (it->first < distance)
						{
							distance = it->first;
							attackObjective = it->second;
						}
					}

				}
			}
			path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(posClick.x-1), int(posClick.y-1) }, GetID());
		}
		else
		{
			if (modPos.x != -1 && modPos.y != -1) path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(modPos.x), int(modPos.y) }, GetID());
			else path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(posClick.x), int(posClick.y) }, GetID());

			attackObjective = nullptr;
		}
	}
}
 
void B_Unit::OnGetImpulse(float x, float y)
{
	float tempX = game_object->GetTransform()->GetGlobalPosition().x;
	float tempY = game_object->GetTransform()->GetGlobalPosition().y;
	if (App->pathfinding.ValidTile(int(tempX), int(tempY)) == false)
	{
		game_object->GetTransform()->MoveX(-6 * x * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(-6 * y * App->time.GetGameDeltaTime());//Move y
	}
	else
	{		
		game_object->GetTransform()->MoveX(6 * x * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(6 * y * App->time.GetGameDeltaTime());//Move y				
	}
}

