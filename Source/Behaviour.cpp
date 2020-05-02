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

#include <vector>

std::map<double, Behaviour*> Behaviour::b_map;
std::vector<double> Behaviour::enemiesInSight;

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
	drawRanges = false;
	attackObjective = nullptr;
	providesVisibility = true;
	visible = true;

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
	case IMPULSE: OnGetImpulse(e.data1.AsFloat(), e.data2.AsFloat()); break;
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
	case ON_COLL_ENTER:
		OnCollisionEnter(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
		break;
	case ON_COLL_STAY:
		OnCollisionStay(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
		break;
	case ON_COLL_EXIT:
		OnCollisionExit(*Component::ComponentsList[e.data1.AsDouble()]->AsCollider(), *Component::ComponentsList[e.data2.AsDouble()]->AsCollider());
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

void Behaviour::CheckFoWMap(bool debug)
{
	//if (providesVisibility) ApplyMaskToTiles(GetTilesInsideRadius());	
	bool visible = FogOfWarManager::fogMap[int(pos.x)][int(pos.y)];//App->fogWar.CheckTileVisibility(iPoint(int(position.x), int(position.y)));
	if (!visible && !debug) { DesactivateSprites(); visible = false; }
	else{ ActivateSprites(); visible = true; }
}

std::vector<iPoint> Behaviour::GetTilesInsideRadius()
{
	/*if (!lastFog.empty())
	{
		for (std::vector<iPoint>::const_iterator it = lastFog.cbegin(); it != lastFog.cend(); ++it)
		{
			FogOfWarManager::fogMap[it->x][it->y] = false;
		}
		lastFog.clear();
	}*/
	std::vector<iPoint> ret;
	/*int length = vision_range * 2;
	iPoint startingPos(int(pos.x-vision_range),int(pos.y-vision_range));
	iPoint finishingPos(startingPos.x+length, startingPos.y+length);

	//Creates a vector with all the tiles inside a bounding box delimited by the radius
	for (int i = startingPos.y; i < finishingPos.y; i++)
	{
		for (int j = startingPos.x; j < finishingPos.x; j++)
		{
			ret.push_back({ j,i });
		}
	}*/

	iPoint startingPos(int(pos.x - vision_range),int(pos.y - vision_range));
	iPoint finishingPos(int(startingPos.x + (vision_range*2)), int(startingPos.y + (vision_range*2)));

	for (int x = startingPos.x; x < finishingPos.x; x++)
	{
		for (int y = startingPos.y; y < finishingPos.y; y++)
		{
			iPoint tilePos(x, y);
			if (tilePos.DistanceTo(iPoint(int(pos.x), int(pos.y))) <= vision_range)
			{
				FogOfWarManager::fogMap[tilePos.x][tilePos.y] = true;
				//lastFog.push_back(tilePos);
			}
		}
	}


	return ret;
}

void Behaviour::ApplyMaskToTiles(std::vector<iPoint>tilesAffected)
{
	/*short precMask = App->fogWar.circleMasks[int(vision_range) - fow_MIN_CIRCLE_RADIUS][0];

	for (int i = 0; i < tilesAffected.size(); i++)
	{
		FoWDataStruct tileValue = App->fogWar.GetFoWTileState(tilesAffected[i]);

		//And (bitwise AND) them with the mask if the tile FoW values are not nullptr
		//To bitwise AND values you just simply do this: value1 &= value2 
		//the operation result will be stored in the variable on the left side. 
		//In this case you want to modify the fog and shroud values that you have requested above

		if (tileValue.tileFogBits != -1 && tileValue.tileShroudBits != -1)
		{
			tileValue->tileShroudBits &= precMask;
			tileValue->tileFogBits &= precMask;
		}
		precMask++;
	}*/
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
			mini_life_bar.Update(float(current_life) / float(max_life));

			if (current_life <= 0)
			{
				update_health_ui();
				OnKill(type);
			}
			else
			{
				mini_life_bar.Update(float(current_life) / float(max_life));

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
			break;
		}
		case ENEMY_RANGED:
		{
			Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 10);
			Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 10);
			Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
			break;
		}
		case ENEMY_SPECIAL:
		{
			Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 15);
			Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 15);
			Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
			break;
		}
		case ENEMY_SUPER:
		{
			Event::Push(UPDATE_STAT, App->scene, CURRENT_MOB_DROP, 20);
			Event::Push(UPDATE_STAT, App->scene, MOB_DROP_COLLECTED, 20);
			Event::Push(UPDATE_STAT, App->scene, UNITS_KILLED, 1);
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
	if(!tilesVisited.empty())
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
	atkDelay = 1.0;
	speed = 5;//MAX SPEED 60
	attack_range = 3.0f;
	damage = 5;
	deathFX = UNIT_DIES;
	attackFX = SELECT;

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
	inRange = false;
	msCount = 0;
	arriveDestination = false;
	current_state = IDLE;
	drawRanges = false;
	gotTile = false;

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
		IARangeCheck();
		if (attackObjective != nullptr && attackObjective->GetState() != DESTROYED) //Attack
		{
			//LOG("FOUND");
			CheckAtkRange();
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
				if (attackObjective->GetType() != DESTROYED)
				{
					//LOG("Do attack");
					DoAttack();
					UnitAttackType();
					Event::Push(DAMAGE, attackObjective, damage);
				}
				msCount = 0;
			}
		}
		else if (path != nullptr && !path->empty()) //Movement
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
	

		//Collisions
		CheckCollision();

		//Raycast
		if (shoot) ShootRaycast();
			
		//Draw vision and attack range
		if (drawRanges) DrawRanges();
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

void B_Unit::CheckDirection(fPoint actualPos)
{
	if (dirX == 1 && dirY == 1)
	{
		if (actualPos.x >= nextTile.x && actualPos.y >= nextTile.y)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == -1 && dirY == -1)
	{
		if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == -1 && dirY == 1)
	{
		if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 1 && dirY == -1)
	{
		if (actualPos.x >= nextTile.x && actualPos.y <= nextTile.y)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 0 && dirY == -1)
	{
		if (actualPos.y <= nextTile.y)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 0 && dirY == 1)
	{
		if (actualPos.y >= nextTile.y)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 1 && dirY == 0)
	{
		if (actualPos.x >= nextTile.x)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == -1 && dirY == 0)
	{
		if (actualPos.x <= nextTile.x)
		{
			path->erase(path->begin());
			next = false;
			PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
			//LOG("Arrive");
		}
	}
	else if (dirX == 0 && dirY == 0)
	{
		path->erase(path->begin());
		next = false;
		PathfindingManager::unitWalkability[nextTile.x][nextTile.y] = 0.0f;
		//LOG("Arrive");
	}
}

void B_Unit::CheckCollision()
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
}

void B_Unit::CheckAtkRange()
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
}

void B_Unit::ShootRaycast()
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
	std::pair<int, int> Pos(int(pos.x),int(pos.y));
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
				else if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED || it->second->GetType() == SPAWNER)//Temporal
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
		else
		{
			if (modPos.x != -1 && modPos.y != -1) path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(modPos.x), int(modPos.y) }, GetID());
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

void Behaviour::Lifebar::Create(Gameobject* parent)
{
	int hud_id = App->tex.Load("Assets/textures/Iconos_square_up.png");
	go = new Gameobject("life_bar", parent);
	new Sprite(go, hud_id, { 275, 698, 30, 4 }, FRONT_SCENE, { 3.f, -35.f, 2.f, 2.f }, { 255, 0, 0, 255});
	green_bar = new Sprite(new Gameobject("GreenBar", go), hud_id, starting_section = { 276, 703, 28, 5 }, FRONT_SCENE, { 4.f, -34.f, 2.f, 2.f }, { 0, 255, 0, 255 });
	Update(1.0f);
}

void Behaviour::Lifebar::Show()
{
	go->SetActive();
}

void Behaviour::Lifebar::Hide()
{
	go->SetInactive();
}

void Behaviour::Lifebar::Update(float life)
{
	green_bar->SetSection({ starting_section.x, starting_section.y, int(float(starting_section.w) * life), starting_section.h });
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
		switch (type)
		{
		case GATHERER:
			new Sprite(icon, App->tex.Load("Assets/textures/Iconos_square_up.png"), { 75, 458, 43, 42 }, FRONT_SCENE, { -0.f, -50.f, 0.2f, 0.2f });
			break;
		case UNIT_MELEE:
			new Sprite(icon, App->tex.Load("Assets/textures/Iconos_square_up.png"), { 22, 463, 48, 35 }, FRONT_SCENE, { -0.f, -50.f, 0.2f, 0.2f });
			break;
		case UNIT_RANGED:
			new Sprite(icon, App->tex.Load("Assets/textures/Iconos_square_up.png"), { 22, 463, 48, 35 }, FRONT_SCENE, { -0.f, -50.f, 0.2f, 0.2f });
			break;
		}
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
}

void BuildingWithQueue::Update()
{
	if (!build_queue.empty())
	{
		if (!progress_bar->GetGameobject()->IsActive())
			progress_bar->GetGameobject()->SetActive();

		float percent = build_queue.front().Update();
		if (percent >= 1.0f)
		{
			Event::Push(SPAWN_UNIT, App->scene, build_queue.front().type, build_queue.front().pos);
			build_queue.front().transform->GetGameobject()->Destroy();
			build_queue.pop_front();

			if (build_queue.empty())
				progress_bar->GetGameobject()->SetInactive();
		}
		else
		{
			SDL_Rect section = bar_section;
			section.w = int(float(section.w) * percent);
			progress_bar->SetSection(section);
		}
	}
}

void BuildingWithQueue::AddUnitToQueue(UnitType type, vec pos, float time)
{
	if (true /* TODO: has enough resources*/)
	{
		QueuedUnit unit(type, game_object, pos, time);
		unit.transform->SetY(build_queue.size());
		build_queue.push_back(unit);
	}
}
