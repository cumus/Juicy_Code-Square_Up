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
#include "FogOfWarManager.h"
#include "MeleeUnit.h"
#include "Gatherer.h"
#include "Tower.h"
#include "BaseCenter.h"
#include "RangedUnit.h"
#include "JuicyMath.h"
#include "Input.h"
#include "ParticleSystem.h"

#include <vector>

std::map<double, Behaviour*> Behaviour::b_map;
std::vector<double> Behaviour::enemiesInSight;
std::vector<double> Behaviour::selectableUnits;

Behaviour::Behaviour(Gameobject* go, UnitType t, UnitState starting_state, ComponentType comp_type) :
	Component(comp_type, go),
	type(t),
	current_state(starting_state)
{
	current_life = max_life = damage = 10;
	//attack_range = vision_range = 5.0f;
	dieDelay = 5.0f;
	deathFX = EDGE_FX; //temp
	rayCastTimer = 0;
	shoot = false;
	selectionPanel = nullptr;
	drawRanges = false;
	objective = nullptr;
	providesVisibility = true;
	visible = true;
	baseCollOffset = { 0,0 };
	visionCollOffset = { 0,0 };
	attackCollOffset = { 0,0 };
	selectionRect = {0,0,64,64};
	game_object->SetStatic(true);

	audio = new AudioSource(game_object);
	characteR = new AnimatedSprite(this);

	selection_highlight = new Sprite(go, App->tex.Load("Assets/textures/selectionMark.png"), { 0, 0, 64, 64 }, BACK_SCENE, { 0, -50, 1.f, 1.f });
	selection_highlight->SetInactive();

	mini_life_bar.Create(go);
	mini_life_bar.Hide();

	b_map.insert({ GetID(), this });

	Minimap::AddUnit(GetID(), t, game_object->GetTransform());

	//GetTilesInsideRadius();
	//CheckFoWMap();	
}

Behaviour::~Behaviour()
{
	b_map.erase(GetID());
	Minimap::RemoveUnit(GetID());
}

bool Behaviour::IsDestroyed()
{
	if (current_state == DESTROYED) return true;
	else return false;
}

void Behaviour::SetColliders()
{
	LOG("Set colliders");
	//Colliders
	pos = game_object->GetTransform()->GetGlobalPosition();
	vec s = game_object->GetTransform()->GetLocalScale();
	switch(type)
	{
		case GATHERER:
		case UNIT_MELEE:
		case UNIT_RANGED:
		case UNIT_SUPER:
		{
			bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, PLAYER_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			//visionColl = new Collider(game_object, { pos.x,pos.y,vision_range,vision_range }, TRIGGER, PLAYER_VISION_TAG, { 0,Map::GetBaseOffset(),0,0 });
			attackColl = new Collider(game_object, { pos.x,pos.y,attack_range,attack_range }, TRIGGER, PLAYER_ATTACK_TAG, { 0,Map::GetBaseOffset(),0,0 }, ATTACK_COLL_LAYER);
			//selColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, SELECTION_TAG, { 0,0,0,0 }, UNIT_SELECTION_LAYER);
			//selColl->SetPointsOffset({-20,-70}, {20,50}, {-10,-55}, {10,35});
			//selectableUnits.push_back(GetID());
			std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
			selectionOffset = { 10,-50 };
			selectionRect = { world.first + selectionOffset.first,world.second + selectionOffset.second,50,90 };
			break;
		}
		case ENEMY_MELEE:
		case ENEMY_RANGED:
		case ENEMY_SUPER:
		case ENEMY_SPECIAL:
		{
			bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, ENEMY_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			visionColl = new Collider(game_object, { pos.x,pos.y,vision_range,vision_range }, TRIGGER, ENEMY_VISION_TAG, { 0,Map::GetBaseOffset(),0,0 }, VISION_COLL_LAYER);
			attackColl = new Collider(game_object, { pos.x,pos.y,attack_range,attack_range }, TRIGGER, ENEMY_ATTACK_TAG, { 0,Map::GetBaseOffset(),0,0 }, ATTACK_COLL_LAYER);
			//selColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, ENEMY_TAG, { 0,0,0,0 }, UNIT_SELECTION_LAYER);
			//selColl->SetPointsOffset({ -20,-70 }, { 20,50 }, { -10,-55 }, { 10,35 });
			//selectableUnits.push_back(GetID());
			std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
			selectionOffset = { 10,-50 };
			selectionRect = { world.first + selectionOffset.first,world.second + selectionOffset.second,50,90 };
			break;
		}
		case BASE_CENTER:
		{
			bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, PLAYER_TAG, { 90,Map::GetBaseOffset() + 65,0,0 }, BODY_COLL_LAYER);
			//selColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, SELECTION_TAG, { 0,0,0,0 }, UNIT_SELECTION_LAYER);
			//selColl->SetPointsOffset({ 0,60 }, { 180,-25 }, { 50,120 }, { 130,-90 });
			//selectableUnits.push_back(GetID());
			std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
			selectionOffset = { 30,-100};
			selectionRect = { world.first + selectionOffset.first,world.second + selectionOffset.second,185,250 };
			break;
		}
		case TOWER:
		{
			//bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, BUILDING_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			//selColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, PLAYER_TAG, { 0,0,0,0 }, UNIT_SELECTION_LAYER);
			//selColl->SetPointsOffset({ -20,-70 }, { 20,50 }, { -10,-55 }, { 10,35 });
			//selectableUnits.push_back(GetID());
			break;
		}
		case BARRACKS:
		{
			//bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, BUILDING_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			//selColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, PLAYER_TAG, { 0,0,0,0 }, UNIT_SELECTION_LAYER);
			//selColl->SetPointsOffset({ -20,-70 }, { 20,50 }, { -10,-55 }, { 10,35 });
			//selectableUnits.push_back(GetID());
			break;
		}
		case LAB:
		{
			break;
		}
		case EDGE:
		{
			//bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, ENEMY_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			//selColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, ENEMY_TAG, { 0,0,0,0 }, UNIT_SELECTION_LAYER);
			//selColl->SetPointsOffset({-65, -20 }, { 40,65 }, { 10,-5 }, { -35,45 });
			//selectableUnits.push_back(GetID());
			std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
			selectionOffset = { -30,0 };
			selectionRect = { world.first + selectionOffset.first,world.second + selectionOffset.second,100,50 };
			break;
		}
		case CAPSULE:
		{
			//bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, TRIGGER, ENEMY_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
			selectionOffset = { 0,-50 };
			selectionRect = { world.first + selectionOffset.first,world.second + selectionOffset.second,60,100 };
			break;
		}
		case SPAWNER:
		{
			//bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, ENEMY_TAG, { 0,Map::GetBaseOffset(),0,0 }, BODY_COLL_LAYER);
			break;
		}
	}
}

vec Behaviour::GetPos() { return pos; }

RectF Behaviour::GetSelectionRect() { return selectionRect; }

Collider* Behaviour::GetSelectionCollider()
{
	return selColl;
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
	//case IMPULSE: OnGetImpulse(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case BUILD_GATHERER:
	{
		AddUnitToQueue(GATHERER, e.data1.AsVec(), e.data2.AsFloat());
		break;
	}
	case BUILD_MELEE:
	{
		AddUnitToQueue(UNIT_MELEE, e.data1.AsVec(), e.data2.AsFloat());
		break;
	}
	case BUILD_RANGED:
	{
		AddUnitToQueue(UNIT_RANGED, e.data1.AsVec(), e.data2.AsFloat());
		break;
	}
	case BUILD_SUPER:
	{
		AddUnitToQueue(UNIT_SUPER, e.data1.AsVec(), e.data2.AsFloat());
		break;
	}
	case DO_UPGRADE: Upgrade(); break;
	case UPDATE_PATH: UpdatePath(e.data1.AsInt(), e.data2.AsInt()); break;
	case DRAW_RANGE: drawRanges = !drawRanges; break;
	case SHOW_SPRITE: ActivateSprites(); break;
	case HIDE_SPRITE: DesactivateSprites(); break;
	case CHECK_FOW: CheckFoWMap(e.data1.AsBool()); break;
	case ON_COLLISION:
		OnCollision(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
		break;
	case ON_COLL_ENTER:
		//OnCollisionEnter(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
		break;
	case ON_COLL_STAY:
		//OnCollisionStay(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
		break;
	case ON_COLL_EXIT:
		//OnCollisionExit(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
		break;
	}
}

void Behaviour::PreUpdate()
{
	pos = game_object->GetTransform()->GetGlobalPosition();
	if(providesVisibility) GetTilesInsideRadius(); 
}

void Behaviour::ActivateSprites()
{
	characteR->SetActive();
	//LOG("Show sprite");
}

void Behaviour::DesactivateSprites()
{
	characteR->SetInactive();
	//LOG("Hide sprite");
}

Collider* Behaviour::GetBodyCollider() 
{ 
	return bodyColl;
}

void Behaviour::CheckFoWMap(bool debug)
{
	//if (providesVisibility) ApplyMaskToTiles(GetTilesInsideRadius());	
	bool visible = FogOfWarManager::fogMap[int(pos.x)][int(pos.y)];//App->fogWar.CheckTileVisibility(iPoint(int(position.x), int(position.y)));
	if (!visible && !debug) { DesactivateSprites(); visible = false; }
	else{ ActivateSprites(); visible = true; }
}

std::vector<iPoint> Behaviour::GetTilesInsideRadius()
{
	std::vector<iPoint> ret;
	vec s = game_object->GetTransform()->GetLocalScale();
	//LOG("Scale X:%f/Y:%f",s.x,s.y);
	iPoint startingPos(int(pos.x + s.x*0.5 - vision_range),int(pos.y) - s.y*0.5 - vision_range);
	iPoint finishingPos(int(startingPos.x + (vision_range*2)), int(startingPos.y + (vision_range*2)));

	for (int x = startingPos.x; x < finishingPos.x; x++)
	{
		for (int y = startingPos.y; y < finishingPos.y; y++)
		{
			iPoint tilePos(x, y);
			if (tilePos.DistanceTo(iPoint(int(pos.x), int(pos.y))) <= vision_range*0.6)
			{
				if (App->fogWar.CheckFoWTileBoundaries(iPoint(tilePos.x,tilePos.y)))FogOfWarManager::fogMap[tilePos.x][tilePos.y] = true;
				//lastFog.push_back(tilePos);
			}
		}
	}
	return ret;
}


void Behaviour::Load(pugi::xml_node& node)
{
	current_life = node.attribute("current_life").as_int();
}

void Behaviour::Save(pugi::xml_node& node) const
{
	node.append_attribute("current_life").set_value(current_life);
	node.append_attribute("type").set_value((int)type);
}

void Behaviour::Selected()
{
	// Selection mark
	selection_highlight->SetActive();

	// Audio Fx
	audio->Play(SELECT);

	if (bar_go != nullptr) bar_go->SetActive();
	if (creation_bar_go != nullptr) creation_bar_go->SetActive();
	if (selectionPanel != nullptr) selectionPanel->SetActive();

	/*if (type == TOWER) {
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
	}*/
	
}

void Behaviour::UnSelected()
{
	// Selection mark
	selection_highlight->SetInactive();

	if (bar_go != nullptr) bar_go->SetInactive();
	if (creation_bar_go != nullptr) creation_bar_go->SetInactive();
	if (selectionPanel != nullptr) selectionPanel->SetInactive();

	/*if (type == TOWER) {
		if (App->scene->building_bars_created > 0)
			App->scene->building_bars_created--;
	}*/
	
}


void Behaviour::OnDamage(int d)
{	
	if (current_state != DESTROYED && Scene::DamageAllowed())
	{
		//LOG("Got damage: %d", d);
		if (current_life > 0)
		{
			current_life -= d;

			// Lifebar
			mini_life_bar.Show();
			mini_life_bar.Update(float(current_life) / float(max_life), current_lvl);

			if (current_life <= 0)
			{
				update_health_ui();
				OnKill(type);
			}
			else
			{
				mini_life_bar.Update(float(current_life) / float(max_life), current_lvl);

				//LOG("Life: %d", current_life);
				update_health_ui();
				AfterDamageAction();
			}
		}
	}
}

void Behaviour::OnKill(const UnitType type)
{
	current_life = 0;
	current_state = DESTROYED;
	spriteState = DESTROYED;

	/*if (!selectableUnits.empty())
	{
		for (std::vector<double>::const_iterator it = selectableUnits.begin(); it != selectableUnits.end(); ++it)
		{
			if ((*it) == GetID()) selectableUnits.erase(it);
		}
	}*/

	// Lifebar
	mini_life_bar.Hide();

	audio->Play(deathFX);
	game_object->Destroy(dieDelay);
	if (bar_go)
		bar_go->Destroy(dieDelay);

	switch (type)
	{
	case UNIT_MELEE:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_MELEE_UNITS, -1);
		Event::Push(UPDATE_STAT, App->scene, UNITS_LOST, 1);
		break;
	}
	case UNIT_RANGED:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_RANGED_UNITS, -1);
		Event::Push(UPDATE_STAT, App->scene, UNITS_LOST, 1);
		break;
	}
	case GATHERER:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_GATHERER_UNITS, -1);
		Event::Push(UPDATE_STAT, App->scene, UNITS_LOST, 1);
		break;
	}
	case ENEMY_MELEE:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 5);
		Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 5);
		Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
		if (App->scene->GetStat(UNITS_KILLED) % 10 == 0) {
			Event::Push(UPDATE_STAT, App->scene, CURRENT_GOLD, 1);
			Event::Push(UPDATE_STAT, App->scene, GOLD_COLLECTED, 1);
			LOG("total gold value %d", App->scene->GetStat(CURRENT_GOLD));
		}
		break;
	}
	case ENEMY_RANGED:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 10);
		Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 10);
		Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
		if (App->scene->GetStat(UNITS_KILLED) % 10 == 0) {
			Event::Push(UPDATE_STAT, App->scene, CURRENT_GOLD, 1);
			Event::Push(UPDATE_STAT, App->scene, GOLD_COLLECTED, 1);
			LOG("total gold value %d", App->scene->GetStat(CURRENT_GOLD));
		}
		break;
	}
	case ENEMY_SPECIAL:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 15);
		Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 15);
		Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
		if (App->scene->GetStat(UNITS_KILLED) % 10 == 0) {
			Event::Push(UPDATE_STAT, App->scene, CURRENT_GOLD, 1);
			Event::Push(UPDATE_STAT, App->scene, GOLD_COLLECTED, 1);
			LOG("total gold value %d", App->scene->GetStat(CURRENT_GOLD));
		}
		break;
	}
	case ENEMY_SUPER:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 20);
		Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 20);
		Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
		if (App->scene->GetStat(UNITS_KILLED) % 10 == 0) {
			Event::Push(UPDATE_STAT, App->scene, CURRENT_GOLD, 1);
			Event::Push(UPDATE_STAT, App->scene, GOLD_COLLECTED, 1);
			LOG("total gold value %d", App->scene->GetStat(CURRENT_GOLD));
		}
		break;
	}
	case BASE_CENTER:
	{
		Event::Push(GAMEPLAY, App->scene, LOSE);
		break;
	}
	case SPAWNER:
	{
		Event::Push(UPDATE_STAT, App->scene, CURRENT_SPAWNERS, -1);
		break;
	}
	}
	if (!tilesVisited.empty())
	{
		for (std::vector<iPoint>::const_iterator it = tilesVisited.cbegin(); it != tilesVisited.cend(); ++it)
		{
			if (PathfindingManager::unitWalkability[it->x][it->y] != 0)
			{
				PathfindingManager::unitWalkability[it->x][it->y] = 0;
			}
		}
	}
	FreeWalkabilityTiles();
	b_map.erase(GetID());
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
	atkTime = 1.0;
	
	speed = 5;//MAX SPEED 60
	attack_range = 3.0f;
	vision_range = 10.0f;
	damage = 5;
	deathFX = UNIT_DIES;
	attackFX = SELECT;
	vision_range = 5.0f;

	//UnitInit();

	//Needed
	path = nullptr;
	next = false;
	move = false;
	nextTile.x = 0;
	nextTile.y = 0;
	positiveX = false;
	positiveY = false;
	dirX = 0;
	dirY = 0;
	atkTimer = 0.0f;
	inRange = false;
	inVision = false;
	arriveDestination = false;
	goingBase = false;
	new_state = IDLE;
	spriteState = IDLE;
	drawRanges = false;
	gotTile = false;
	game_object->SetStatic(false);
	calculating_path = false;
	atkObj = nullptr;
	chaseObj = nullptr;
	chasing = false;
	moveOrder = false;

	//Info for ranged units constructor
	/*vec pos = game_object->GetTransform()->GetGlobalPosition();
	shootPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	shootPos.first += 30.0f;
	shootPos.second += 20.0f;*/
}

void B_Unit::Update()
{	
	if (!providesVisibility) CheckFoWMap();
	if (current_state != DESTROYED)
	{
		if (inRange) //ATTACK
		{
			//LOG("In range");
			if (type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER || type == ENEMY_SPECIAL)
			{
				if (atkTimer > atkTime)
				{
					if (atkObj != nullptr && !atkObj->GetState() != DESTROYED) //Attack
					{
						DoAttack();
						Event::Push(DAMAGE, atkObj, damage);
						LOG("Do attack");
					}
					atkObj = nullptr;
					atkTimer = 0;
				}
			}
			else
			{
				if (!moveOrder)
				{
					if (atkTimer > atkTime)
					{
						if (atkObj != nullptr && !atkObj->GetState() != DESTROYED) //ATTACK
						{
							DoAttack();						
							Event::Push(DAMAGE, atkObj, damage);
						}
						atkObj = nullptr;
						atkTimer = 0;
					}
				}
			}
		}
		else
		{
			//LOG("Not in range");
			if (type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER || type == ENEMY_SPECIAL)
			{
				if (inVision)//CHASE
				{
					//LOG("In vision");
					if (chaseObj != nullptr && !chasing)
					{
						//LOG("Start player chase");
						vec pos = chaseObj->GetPos();
						Event::Push(UPDATE_PATH, this->AsBehaviour(), int(pos.x), int(pos.y));
						chasing = true;
						//move = true;
						goingBase = false;
					}
				}
				else
				{
					if (Base_Center::baseCenter != nullptr && !goingBase && !chasing)//GO TO BASE
					{
						//LOG("Path to base");
						goingBase = true;
						vec centerPos = Base_Center::baseCenter->GetTransform()->GetGlobalPosition();
						Event::Push(UPDATE_PATH, this->AsBehaviour(), int(centerPos.x), int(centerPos.y));
						//LOG("Move to base");	
					}
				}
			}
		}

		if (moveOrder)
		{
			if (chaseObj != nullptr && !chasing)
			{
				vec goToPos = chaseObj->GetPos();
				Event::Push(UPDATE_PATH, this->AsBehaviour(), int(goToPos.x)-1, int(goToPos.y));
				chasing = true;
			}

			/*if (chasing)
			{
				if (path !=  nullptr && path->empty())
				{
					chasing = false;
				}
			}*/
		}

		if (path != nullptr && !path->empty()) CheckPathTiles();

		if (move && PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == GetID())
		{
			calculating_path = false;
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

			if (App->pathfinding.ValidTile(int(pos.x), int(pos.y)) == false)
			{
				game_object->GetTransform()->MoveX(-dirX * speed * App->time.GetGameDeltaTime());//Move x back
				game_object->GetTransform()->MoveY(-dirY * speed * App->time.GetGameDeltaTime());//Move y back
			}		

			ChangeState();
			CheckDirection(actualPos);
			if (path->empty())
			{
				moveOrder = false;
				move = false;
				chasing = false;
				if(!inRange) spriteState = IDLE;
			}
		}

		//Move selection rect
		std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
		selectionRect.x = world.first + selectionOffset.first;
		selectionRect.y = world.second + selectionOffset.second;
	
		
		/*
		LOG("Current state %d",current_state);
		switch(new_state)
		{
			case IDLE:
			{				
				//LOG("state IDLE");
				spriteState = IDLE;
				if (inRange && !moveOrder)
				{
					new_state = ATTACKING;
				}
				else
				{
					if (inVision && type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER || type == ENEMY_SPECIAL)
					{
						new_state = CHASING;
					}
					else
					{
						if(chaseObj != nullptr) new_state = CHASING;
						else
						{
							if (path != nullptr && !path->empty())
							{
								new_state = MOVING;
							}
							else
							{
								if (type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER || type == ENEMY_SPECIAL)
								{
									new_state = BASE;
								}
								else new_state = IDLE;
								move = false;
							}
							objective = nullptr;
						}
					}
				}

				current_state = IDLE;
				break;
			}
			case MOVING:
			{
				//LOG("state MOVING");	
				CheckPathTiles();

				if (move && PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == GetID() && !inRange)
				{
					spriteState = MOVING;
					calculating_path = false;
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
				}

				if (inRange) new_state = ATTACKING;
				else if (inVision && type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER || type == ENEMY_SPECIAL) new_state = CHASING;

				if (path->empty()) 
				{
					new_state = IDLE; 
					moveOrder = false;
				}

				current_state = MOVING;
				break;
			}
			case ATTACKING:
			{
				//LOG("state ATTACK");	
				spriteState = ATTACKING;
				if (atkTimer > atkTime)
				{
					if (atkObj != nullptr && !atkObj->BeingDestroyed()) //Attack
					{
						DoAttack();
						UnitAttackType();
						Event::Push(DAMAGE, atkObj->GetBehaviour(), damage);
					}
					atkObj = nullptr;
					atkTimer = 0;
					new_state = IDLE;
				}
				else if(!inRange || moveOrder) new_state = IDLE;
			
				current_state = ATTACKING;
				break;
			}
			case CHASING:
			{
				//LOG("state CHASING");
				if (chaseObj != nullptr && !calculating_path && !chasing)
				{
					vec pos = chaseObj->GetTransform()->GetGlobalPosition();
					Event::Push(UPDATE_PATH, this->AsBehaviour(), int(pos.x), int(pos.y));
					chasing = true;
				}

				if (inRange) new_state = ATTACKING;
				else if (path != nullptr && !path->empty())
				{
					CheckPathTiles();

					if (move && PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == GetID() && !inRange)
					{
						calculating_path = false;
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

						if (path->size() < 3)
						{
							if (chaseObj != nullptr && !calculating_path)
							{
								vec pos = chaseObj->GetTransform()->GetGlobalPosition();
								Event::Push(UPDATE_PATH, this->AsBehaviour(), int(pos.x), int(pos.y));
							}
						}
					}
				}
				else new_state = IDLE;

				if (path->empty())
				{
					new_state = IDLE;
					moveOrder = false;
					chasing = false;
				}
				current_state = CHASING;
				break;
			}
			case BASE:
			{
				if (Base_Center::baseCenter != nullptr && !calculating_path)
				{
					//LOG("Path to base");
					vec centerPos = Base_Center::baseCenter->GetTransform()->GetGlobalPosition();
					Event::Push(UPDATE_PATH, this->AsBehaviour(), int(centerPos.x) - 1, int(centerPos.y) - 1);
					//LOG("Move to base");	
					new_state = IDLE;
				}
				else new_state = IDLE;
				current_state = BASE;
				break;
			}
		}
		*/

		if (atkTimer < atkTime)
		{
			atkTimer += App->time.GetGameDeltaTime();
		}

		//Raycast
		if (shoot) ShootRaycast();
			
		//Draw vision and attack range
		if (drawRanges) DrawRanges();

		atkObj = nullptr;
		chaseObj = nullptr;
		inRange = false;
		inVision = false;
	}
}

void B_Unit::OnCollision(Collider selfCol, Collider col)
{
	if (!col.parentGo->GetBehaviour()->GetState() != DESTROYED)
	{
		if (selfCol.GetColliderTag() == PLAYER_ATTACK_TAG)
		{
			//LOG("Atk");
			if (col.GetColliderTag() == ENEMY_TAG)
			{
				//LOG("Eenmy unit in attack range");
				inRange = true;
				atkObj = col.parentGo->GetBehaviour();
			}
		}

		if (selfCol.GetColliderTag() == ENEMY_ATTACK_TAG)
		{
			//LOG("Atk");
			if (col.GetColliderTag() == PLAYER_TAG)
			{
				//LOG("Player unit in attack range");
				inRange = true;
				atkObj = col.parentGo->GetBehaviour();
			}
		}

		if (selfCol.GetColliderTag() == ENEMY_VISION_TAG)
		{
			//LOG("Enemy vision");
			if (col.GetColliderTag() == PLAYER_TAG)
			{
				//LOG("Player unit in vision");
				inVision = true;
				if (chaseObj == nullptr) chaseObj = col.parentGo->GetBehaviour();
			}
		}
	}
}

void B_Unit::OnCollisionEnter(Collider selfCol, Collider col)
{
	 if (!col.parentGo->GetBehaviour()->GetState() != DESTROYED)
	 {
		 if (selfCol.GetColliderTag() == PLAYER_ATTACK_TAG)
		 {
			 //LOG("Atk");
			 //LOG("Coll tag :%d", selfCol.GetColliderTag());
			 //LOG("Coll tag :%d", col.GetColliderTag());
			 if (col.GetColliderTag() == ENEMY_TAG)
			 {
				 LOG("Eenmy unit in attack range");
				 inRange = true;
				 if (atkObj == nullptr) atkObj = col.parentGo->GetBehaviour();
			 }
		 }

		 if (selfCol.GetColliderTag() == ENEMY_ATTACK_TAG)
		 {
			 //LOG("Atk");
			 //LOG("Coll tag :%d", selfCol.GetColliderTag());
			 //LOG("Coll tag :%d", col.GetColliderTag());
			 if (col.GetColliderTag() == PLAYER_TAG)
			 {
				 //LOG("Player unit in attack range");
				 inRange = true;
				 if (atkObj == nullptr) atkObj = col.parentGo->GetBehaviour();
			 }
		 }

		 if (selfCol.GetColliderTag() == ENEMY_VISION_TAG)
		 {
			 //LOG("Enemy vision");
			 //LOG("Coll tag :%d", selfCol.GetColliderTag());
			 //LOG("Coll tag :%d", col.GetColliderTag());
			 if (col.GetColliderTag() == PLAYER_TAG)
			 {
				 //LOG("Player unit in vision");
				 inVision = true;
				 if (chaseObj == nullptr) chaseObj = col.parentGo->GetBehaviour();
			 }
		 }
	 }
}

void B_Unit::OnCollisionStay(Collider selfCol, Collider col)
{
	if (!col.parentGo->GetBehaviour()->GetState() != DESTROYED)
	{
		if (selfCol.GetColliderTag() == PLAYER_ATTACK_TAG)
		{
			//LOG("Atk stay");
			//LOG("Coll tag :%d", selfCol.GetColliderTag());
			//LOG("Coll tag :%d", col.GetColliderTag());
			if (col.GetColliderTag() == ENEMY_TAG)
			{
				//LOG("Eenmy unit in attack range stay");
				inRange = true;
				if (atkObj == nullptr) atkObj = col.parentGo->GetBehaviour();
			}
		}

		if (selfCol.GetColliderTag() == ENEMY_ATTACK_TAG)
		{
			//LOG("Atk stay");
			//LOG("Coll tag :%d", selfCol.GetColliderTag());
			//LOG("Coll tag :%d", col.GetColliderTag());
			if (col.GetColliderTag() == PLAYER_TAG)
			{
				//LOG("Player unit in attack range stay");
				inRange = true;
				if (atkObj == nullptr) atkObj = col.parentGo->GetBehaviour();
			}
		}

		if (selfCol.GetColliderTag() == ENEMY_VISION_TAG)
		{
			//LOG("Enemy vision");
			//LOG("Coll tag :%d", selfCol.GetColliderTag());
			//LOG("Coll tag :%d", col.GetColliderTag());
			if (col.GetColliderTag() == PLAYER_TAG)
			{
				//LOG("Player unit in vision");
				inVision = true;
				chaseObj = col.parentGo->GetBehaviour();
			}
		}

	}
}

void B_Unit::OnCollisionExit(Collider selfCol, Collider col)
{
	if (!col.parentGo->GetBehaviour()->GetState() != DESTROYED)
	{
		if (selfCol.GetColliderTag() == PLAYER_ATTACK_TAG)
		{
			if (col.GetColliderTag() == ENEMY_TAG)
			{
				inRange = false;
			}
		}

		/*if (selfCol.GetColliderTag() == PLAYER_VISION_TAG)
		{
			if (col.GetColliderTag() == ENEMY_TAG)
			{
				inVision = false;
			}
		}*/

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
	}
}


void B_Unit::UpdatePath(int x, int y)
{
	if (x >= 0 && y >= 0)
	{
		Transform* t = game_object->GetTransform();
		path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { x, y }, GetID());

		calculating_path = true;
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
}

void B_Unit::CheckPathTiles()
{
	if (!next)
	{
		if (PathfindingManager::unitWalkability[nextTile.x][nextTile.y] != 0.0f) PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == 0;
		nextTile = path->front();
		next = true;
		move = true;
		gotTile = false;
		//LOG("Next tile");
	}
	else
	{
		if (PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == 0.0f)
		{
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = GetID();
			//LOG("ID: %f",GetID());
			tilesVisited.push_back(nextTile);
			gotTile = true;
			//LOG("Tile found");
		}
		else if (!gotTile)
		{

			//LOG("Pos X:%d/Y:%d", int(pos.x), int(pos.y));
			//LOG("Tile X:%d/Y:%d", nextTile.x, nextTile.y);
			iPoint cell = App->pathfinding.CheckEqualNeighbours(iPoint(int(pos.x), int(pos.y)), nextTile);
			//LOG("Cell X:%d/Y:%d", cell.x, cell.y);
			if (cell.x != -1 && cell.y != -1)
			{
				//LOG("Tile ok");
				nextTile = cell;
			}
		}
	}
}

void B_Unit::ChangeState()
{
	//Change state to change sprite
	if (dirX == 1 && dirY == 1)//S
	{
		spriteState = MOVING_S;
	}
	else if (dirX == -1 && dirY == -1)//N
	{
		spriteState = MOVING_N;
	}
	else if (dirX == 1 && dirY == -1)//E
	{
		spriteState = MOVING_E;
	}
	else if (dirX == -1 && dirY == 1)//W
	{
		spriteState = MOVING_W;
	}
	else if (dirX == 0 && dirY == 1)//SW
	{
		spriteState = MOVING_SW;
	}
	else if (dirX == 1 && dirY == 0)//SE
	{
		spriteState = MOVING_SE;
	}
	else if (dirX == 0 && dirY == -1)//NE
	{
		spriteState = MOVING_NE;
	}
	else if (dirX == -1 && dirY == 0)//NW
	{
		spriteState = MOVING_NW;
	}
	/*else if (dirX == 0 && dirY == 0)
	{
		current_state = IDLE;
	}*/
}

void B_Unit::CheckDirection(fPoint actualPos)
{
	if (dirX == 1 && dirY == 1)
	{
		if (actualPos.x >= nextTile.x && actualPos.y >= nextTile.y)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == -1 && dirY == -1)
	{
		if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == -1 && dirY == 1)
	{
		if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 1 && dirY == -1)
	{
		if (actualPos.x >= nextTile.x && actualPos.y <= nextTile.y)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 0 && dirY == -1)
	{
		if (actualPos.y <= nextTile.y)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 0 && dirY == 1)
	{
		if (actualPos.y >= nextTile.y)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 1 && dirY == 0)
	{
		if (actualPos.x >= nextTile.x)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == -1 && dirY == 0)
	{
		if (actualPos.x <= nextTile.x)
		{
			if (!path->empty()) path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 0 && dirY == 0)
	{
		if(!path->empty()) path->erase(path->begin());
		next = false;
		PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
		//LOG("Arrive");
	}

	if (path->empty()) new_state = IDLE;
}

/*void B_Unit::CheckCollision()
{
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
				if (!move) Event::Push(IMPULSE, it->second->AsBehaviour(), -separationSpd.x / 2, -separationSpd.y / 2);
				else Event::Push(IMPULSE, it->second->AsBehaviour(), -separationSpd.x, -separationSpd.y);
			}
		}
	}
}*/

/*void B_Unit::CheckAtkRange()
{
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
}*/

void B_Unit::ShootRaycast()
{
	rayCastTimer += App->time.GetGameDeltaTime();
	if (rayCastTimer < RAYCAST_TIME)
	{
		App->render->DrawLine(shootPos, atkObjPos, { 34,191,255,255 }, FRONT_SCENE, true);
	}
	else
	{
		shoot = false;
		rayCastTimer = 0;
	}
}

void B_Unit::DrawRanges()
{
	std::pair<float, float> localPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	localPos.first += 30.0f;
	localPos.second += 30.0f;
	visionRange = { vision_range * 23, vision_range * 23 };
	atkRange = { attack_range * 23, attack_range * 23 };
	App->render->DrawCircle(localPos, visionRange, { 10, 156, 18, 255 }, FRONT_SCENE, true);//Vision
	App->render->DrawCircle(localPos, atkRange, { 255, 0, 0, 255 }, FRONT_SCENE, true);//Attack
}

void B_Unit::DoAttack()
{
	UnitAttackType();
	std::pair<int, int> Pos(int(pos.x),int(pos.y));
	vec objPos = atkObj->GetPos();
	std::pair<int,int> atkPos(int(objPos.x), int(objPos.y));
	arriveDestination = true;

	audio->Play(attackFX);
	if (atkPos.first == Pos.first && atkPos.second < Pos.second)//N
	{
		spriteState = ATTACKING_N;
	}
	else if (atkPos.first == Pos.first && atkPos.second > Pos.second)//S
	{
		spriteState = ATTACKING_S;
	}
	else if (atkPos.first < Pos.first && atkPos.second == Pos.second)//W
	{
		spriteState = ATTACKING_W;
	}
	else if (atkPos.first > Pos.first && atkPos.second == Pos.second)//E
	{
		spriteState = ATTACKING_E;
	}
	else if (atkPos.first < Pos.first && atkPos.second > Pos.second)//SW
	{
		spriteState = ATTACKING_NW;
	}
	else if (atkPos.first > Pos.first && atkPos.second > Pos.second)//
	{
		spriteState = ATTACKING_SW;
	}
	else if (atkPos.first < Pos.first && atkPos.second < Pos.second)//
	{
		spriteState = ATTACKING_NE;
	}
	else if (atkPos.first > Pos.first && atkPos.second < Pos.second)//
	{
		spriteState = ATTACKING_SE;
	}
}

void B_Unit::OnDestroy()
{
	App->pathfinding.DeletePath(GetID());
}

void B_Unit::OnRightClick(vec posClick, vec movPos)
{
	chaseObj = false;
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();		
		next = false;
		move = false;
		moveOrder = true;
		audio->Play(HAMMER);

		SDL_Rect cam = App->render->GetCameraRect();
		int x, y;
		App->input->GetMousePosition(x, y);
		x += cam.x;
		y += cam.y;

		for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
		{
			if (GetType() == GATHERER)
			{
				if ( it->second->GetType() == EDGE || it->second->GetType() == CAPSULE)
				{
					RectF coll = (*it).second->GetSelectionRect();
					if (float(x) > coll.x&& float(x) < coll.x + coll.w && float(y) > coll.y&& float(y) < coll.y + coll.h)
					{
						chaseObj = (*it).second;
						break;
					}
				}
			}
			else
			{
				if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED || it->second->GetType() == ENEMY_SUPER
					|| it->second->GetType() == ENEMY_SPECIAL || it->second->GetType() == EDGE || it->second->GetType() == CAPSULE
					|| it->second->GetType() == SPAWNER)
				{
					RectF coll = (*it).second->GetSelectionRect();
					if (float(x) > coll.x&& float(x) < coll.x + coll.w && float(y) > coll.y&& float(y) < coll.y + coll.h)
					{
						chaseObj = (*it).second;
						break;
					}
				}
			}
		
		
		/*std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(posClick.x, posClick.y, 0.5f), 1.5f, out);
		float distance = 0;
		if (total_found > 0)
		{
			LOG("Unit cliked");
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (GetType() == GATHERER)
				{
					if (it->second->GetType() == EDGE || it->second->GetType() == CAPSULE)
					{
						if (distance == 0)//Chose closest
						{
							chaseObj = it->second;
							distance = it->first;
						}
						else
						{
							if (it->first < distance)
							{
								distance = it->first;
								chaseObj = it->second;
							}
						}
					}
				}
				else if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED || it->second->GetType() == SPAWNER || it->second->GetType() == CAPSULE)//Temporal
				{
					if (distance == 0)//Closest distance
					{
						chaseObj = it->second;
						distance = it->first;
					}
					else
					{
						if (it->first < distance)
						{
							distance = it->first;
							chaseObj = it->second;
						}
					}
				}*/
			
			//path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(posClick.x-1), int(posClick.y-1) }, GetID());
			if (!tilesVisited.empty())
			{
				for (std::vector<iPoint>::const_iterator it = tilesVisited.cbegin(); it != tilesVisited.cend(); ++it)
				{
					if (PathfindingManager::unitWalkability[it->x][it->y] != 0)
					{
						PathfindingManager::unitWalkability[it->x][it->y] = 0;
					}
				}
				tilesVisited.clear();
			}
		}
		if(chaseObj == nullptr)
		{
			if (movPos.x != -1 && movPos.y != -1) path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(movPos.x), int(movPos.y) }, GetID());
			else path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(posClick.x), int(posClick.y) }, GetID());

			if (!tilesVisited.empty())
			{
				for (std::vector<iPoint>::const_iterator it = tilesVisited.cbegin(); it != tilesVisited.cend(); ++it)
				{
					if (PathfindingManager::unitWalkability[it->x][it->y] != 0)
					{
						PathfindingManager::unitWalkability[it->x][it->y] = 0;
					}
				}
				tilesVisited.clear();
			}
		}
		App->particleSys.AddParticle(pos, {movPos.x,movPos.y}, 0.5f, true);
	} 
}
 
/*void B_Unit::OnGetImpulse(float x, float y)
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
}*/

void Behaviour::Lifebar::Create(Gameobject* parent)
{
	int hud_id = App->tex.Load("Assets/textures/Iconos_square_up.png");
	go = new Gameobject("life_bar", parent);
	new Sprite(go, hud_id, { 275, 698, 30, 4 }, FRONT_SCENE, { 2.f, -35.f, 2.f, 2.f }, { 255, 0, 0, 255 });
	green_bar = new Sprite(new Gameobject("GreenBar", go), hud_id, life_starting_section = { 276, 703, 28, 5 }, FRONT_SCENE, { 3.f, -34.f, 2.f, 2.f }, { 0, 255, 0, 255 });
	upgrades = new Sprite(new Gameobject("Upgrades", go), hud_id, upgrades_starting_section = { 0, 0, 0, 0 }, FRONT_SCENE, { 168.f, -184.f, 0.4f, 0.4f });
	Update(1.0f, 0);
}

void Behaviour::Lifebar::Show()
{
	go->SetActive();
}

void Behaviour::Lifebar::Hide()
{
	go->SetInactive();
}

void Behaviour::Lifebar::Update(float life, int lvl)
{
	green_bar->SetSection({ life_starting_section.x, life_starting_section.y, int(float(life_starting_section.w) * life), life_starting_section.h });

	if (lvl <= 0)
		upgrades->SetSection({ 0,0,0,0 });

	else
		upgrades->SetSection({ 16 + 36 * (lvl - 1), 806, 33, 33 });
}

// Queued Unit

SDL_Rect BuildingWithQueue::bar_section;

BuildingWithQueue::QueuedUnit::QueuedUnit(UnitType type, Gameobject* go, vec pos, float time) :
	type(type), pos(pos), time(time), current_time(time)
{
	if (go)
	{

		Gameobject* icon = App->scene->AddGameobject("Queued Unit", go);
		transform = icon->GetTransform();
		
	}
	else
		transform = nullptr;
}

BuildingWithQueue::QueuedUnit::QueuedUnit(const QueuedUnit& copy) :
	type(copy.type), pos(copy.pos), time(copy.time), current_time(copy.current_time), transform(copy.transform)
{}

float BuildingWithQueue::QueuedUnit::Update()
{
	return (time - (current_time -= App->time.GetGameDeltaTime())) / time;
}

BuildingWithQueue::BuildingWithQueue(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type) : Behaviour(go, type, starting_state, comp_type)
{
	spawnPoint = game_object->GetTransform()->GetLocalPos();
	int texture_id = App->tex.Load("Assets/textures/Iconos_square_up.png");
	Gameobject* back_bar = App->scene->AddGameobject("Creation Bar", game_object);
	new Sprite(back_bar, texture_id, { 41, 698, 216, 16 }, FRONT_SCENE, { 0.f, 13.f, 0.29f, 0.2f });
	progress_bar = new Sprite(back_bar, texture_id, bar_section = { 41, 721, 216, 16 }, FRONT_SCENE, { 0.f, 13.f, 0.29f, 0.2f });
	back_bar->SetInactive();

	Gameobject* unit_icon = App->scene->AddGameobject("Unit Icon", game_object);

	icon = new Sprite(unit_icon, texture_id, { 0, 0, 0, 0 }, FRONT_SCENE, { -50.f, 0.f, 0.2f, 0.2f });

	unit_icon->SetInactive();
}

void BuildingWithQueue::Update()
{
	if (!build_queue.empty())
	{
		if (!progress_bar->GetGameobject()->IsActive())
			progress_bar->GetGameobject()->SetActive();

		if (!icon->GetGameobject()->IsActive())
			icon->GetGameobject()->SetActive();

		float percent = build_queue.front().Update();
		if (percent >= 1.0f)
		{
			Event::Push(SPAWN_UNIT, App->scene, build_queue.front().type, build_queue.front().pos);
			build_queue.front().transform->GetGameobject()->Destroy();
			build_queue.pop_front();

			if (build_queue.empty())
				progress_bar->GetGameobject()->SetInactive();
				icon->GetGameobject()->SetInactive();
		}
		else
		{
			switch (build_queue.front().type)
			{
			case GATHERER:
				icon->SetSection({ 75, 458, 48, 35 });
				break;
			case UNIT_MELEE:
				icon->SetSection({ 22, 463, 48, 35 });
				break;
			case UNIT_RANGED:
				icon->SetSection({ 22, 463, 48, 35 });
				break;
			}

			SDL_Rect section = bar_section;
			section.w = int(float(section.w) * percent);
			progress_bar->SetSection(section);
		}
	}
}

void BuildingWithQueue::AddUnitToQueue(UnitType type, vec pos, float time)
{
	switch (type)
	{
	case GATHERER:
	{
		if (App->scene->GetStat(CURRENT_EDGE) >= GATHERER_COST)
		{
			QueuedUnit unit(type, game_object, pos, time);
			unit.transform->SetY(build_queue.size());
			build_queue.push_back(unit);
		}
		break;
	}
	case UNIT_MELEE:
	{
		if (App->scene->GetStat(CURRENT_EDGE) >= MELEE_COST)
		{
			QueuedUnit unit(type, game_object, pos, time);
			unit.transform->SetY(build_queue.size());
			build_queue.push_back(unit);
		}
		break;
	}
	case UNIT_RANGED:
	{
		if (App->scene->GetStat(CURRENT_EDGE) >= RANGED_COST)
		{
			QueuedUnit unit(type, game_object, pos, time);
			unit.transform->SetY(build_queue.size());
			build_queue.push_back(unit);
		}
		break;
	}
	case UNIT_SUPER:
	{
		if (App->scene->GetStat(CURRENT_EDGE) >= SUPER_COST)
		{
			QueuedUnit unit(type, game_object, pos, time);
			unit.transform->SetY(build_queue.size());
			build_queue.push_back(unit);
		}
		break;
	}
	}
}
