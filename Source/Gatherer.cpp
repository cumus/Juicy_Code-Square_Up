#include "Gatherer.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "BaseCenter.h"


Gatherer::Gatherer(Gameobject* go) : B_Unit(go, GATHERER, IDLE, B_GATHERER)
{
	atkTime = 2.0f;
	speed = 3;
	damage = 10;
	current_life = max_life = 20;
	attack_range = 2.0f;
	vision_range = 20.0f;
	providesVisibility = true;
	deathFX = GATHERER_DIE_FX;
	attackFX = GATHERER_ATK_FX;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();
	barrack_tooltip->SetInactive();
	tower_tooltip->SetInactive();
	lab_tooltip->SetInactive();
	
	baseCollOffset = {0,-30};
	SetColliders();
}

Gatherer::~Gatherer(){}

void Gatherer::Update()
{
	if (providesVisibility) GetTilesInsideRadius();
	else CheckFoWMap();

	if (current_state != DESTROYED)
	{
		if (moveOrder)
			atkObj = nullptr;

		if (atkObj != nullptr) //ATTACK
		{
			if (type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER)//IA
			{
				if (atkTimer > atkTime)
				{
					if (!atkObj->IsDestroyed()) //Attack
					{
						DoAttack();
						Event::Push(DAMAGE, atkObj, damage, GetType());
					}
					else
						atkObj = nullptr;

					atkTimer = 0;
				}
			}
			else if (atkTimer > atkTime) //PLAYER
			{
				if (!atkObj->IsDestroyed()) //ATTACK
				{
					DoAttack();
					Event::Push(DAMAGE, atkObj, damage, GetType());
				}
				else
					atkObj = nullptr;

				atkTimer = 0;
			}
		}
		else//CHECK IF MOVES
		{
			if (type == ENEMY_MELEE || type == ENEMY_RANGED || type == ENEMY_SUPER) //IA
			{
				if (chaseObj != nullptr)//CHASE
				{
					if (!chasing)
					{
						vec pos = chaseObj->GetPos();
						Event::Push(UPDATE_PATH, this->AsBehaviour(), int(pos.x), int(pos.y));
						chasing = true;
						goingBase = false;
					}
				}
				else if (Base_Center::baseCenter != nullptr && !goingBase && !chasing)//GO TO BASE
				{
					goingBase = true;
					vec centerPos = Base_Center::baseCenter->GetTransform()->GetGlobalPosition();
					Event::Push(UPDATE_PATH, this->AsBehaviour(), int(centerPos.x + 2), int(centerPos.y + 2));
				}


			}

			if (moveOrder && (chaseObj != nullptr && !chaseObj->IsDestroyed() && !chasing))
			{
				vec goToPos = chaseObj->GetPos();
				Event::Push(UPDATE_PATH, this->AsBehaviour(), int(goToPos.x), int(goToPos.y));
				chasing = true;
			}

			if (path != nullptr && !path->empty())
				CheckPathTiles();

			if (move && PathfindingManager::unitWalkability[nextTile.x][nextTile.y] == GetID())
			{
				calculating_path = false;
				nonMovingCounter = 0.0f;
				fPoint actualPos = { pos.x, pos.y };
				iPoint tilePos = { int(pos.x), int(pos.y) };

				if (nextTile.x > tilePos.x)
					dirX = 1;
				else if (nextTile.x < tilePos.x)
					dirX = -1;
				else
					dirX = 0;

				if (nextTile.y > tilePos.y)
					dirY = 1;
				else if (nextTile.y < tilePos.y)
					dirY = -1;
				else
					dirY = 0;

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
					spriteState = IDLE;

					if (chaseObj != nullptr && chaseObj->IsDestroyed())
						chaseObj = nullptr;
				}
			}
			else if (nonMovingCounter > 1.0f)
				spriteState = IDLE;
			else
				nonMovingCounter += App->time.GetGameDeltaTime();
		}

		//Move selection rect
		std::pair<float, float> world = Map::F_MapToWorld(pos.x, pos.y);
		selectionRect.x = world.first + selectionOffset.first;
		selectionRect.y = world.second + selectionOffset.second;

		if (atkTimer < atkTime)
			atkTimer += App->time.GetGameDeltaTime();

		if (atkObj != nullptr && atkObj->IsDestroyed())
		{
			spriteState = IDLE;
			atkObj = nullptr;
		}

		if (chaseObj != nullptr && chaseObj->IsDestroyed())
		{
			chaseObj = nullptr;
			chasing = false;
			goingBase = false;
			spriteState = IDLE;
		}

		//Check position for non walkable
		if (App->pathfinding.ValidTile(int(pos.x), int(pos.y)) == false)
		{
			if (!foundPoint)
			{
				bool free = false;
				int distance = 1;
				int maxIterations = 100;
				iPoint startPoint = { int(pos.x),int(pos.y) };
				do
				{
					if (App->pathfinding.ValidTile(startPoint.x + distance, startPoint.y))
					{
						free = true;
						startPoint = { int(pos.x) + (distance + 2), int(pos.y) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x - distance, startPoint.y))
					{
						free = true;
						startPoint = { int(pos.x) - (distance + 2), int(pos.y) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x, startPoint.y + distance))
					{
						free = true;
						startPoint = { int(pos.x), int(pos.y) + (distance + 2) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x, startPoint.y - distance))
					{
						free = true;
						startPoint = { int(pos.x), int(pos.y) - (distance + 2) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x + distance, startPoint.y + distance))
					{
						free = true;
						startPoint = { int(pos.x) + (distance + 2), int(pos.y) + (distance + 2) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x - distance, startPoint.y + distance))
					{
						free = true;
						startPoint = { int(pos.x) - (distance + 2),int(pos.y) + (distance + 2) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x + distance, startPoint.y - distance))
					{
						free = true;
						startPoint = { int(pos.x) + (distance + 2), int(pos.y) - (distance + 2) };
					}
					else if (App->pathfinding.ValidTile(startPoint.x - distance, startPoint.y - distance))
					{
						free = true;
						startPoint = { int(pos.x) - (distance + 2), int(pos.y) - (distance + 2) };
					}
					distance++;

				} while (!free && distance < maxIterations);

				if (free)
				{
					foundPoint = true;
					freePos = { float(startPoint.x),float(startPoint.y) };
					direction = { freePos.x - pos.x,freePos.y - pos.y };
					float normal = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
					direction = { direction.x / normal,direction.y / normal };
				}
			}
			else
			{
				float d = (abs(pos.x - freePos.x)) + (abs(pos.y - freePos.y));
				if (d > 0.5f)
				{
					game_object->GetTransform()->MoveX(direction.x * 5 * App->time.GetGameDeltaTime());//Move X
					game_object->GetTransform()->MoveY(direction.y * 5 * App->time.GetGameDeltaTime());//Move Y
				}
				else
				{
					foundPoint = false;
				}
			}
		}

		//Draw vision and attack range
		if (drawRanges) DrawRanges();

		if (visible)
		{
			if (current_life < max_life)
				mini_life_bar.Show();
		}
		else  mini_life_bar.Hide();
	}

	// Barrack Tooltip Check
	if (barracks_btn->state == 1 && barrack_tooltip->IsActive() == false)
		barrack_tooltip->SetActive();
	else if (barracks_btn->state != 1 && barrack_tooltip->IsActive() == true)
		barrack_tooltip->SetInactive();
	
	// Tower Tooltip Check
	if (tower_btn->state == 1 && tower_tooltip->IsActive() == false)
		tower_tooltip->SetActive();
	else if (tower_btn->state != 1 && tower_tooltip->IsActive() == true)
		tower_tooltip->SetInactive();
	
	// Lab Tooltip Check

	if (labBtn->state == 1 && lab_tooltip->IsActive() == false)
		lab_tooltip->SetActive();
	else if (labBtn->state != 1 && lab_tooltip->IsActive() == true)
		lab_tooltip->SetInactive();
}

void Gatherer::CreatePanel()
{
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Gatherer Build Panel");

	gatherer_icon = new C_Image(selectionPanel);
	gatherer_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	gatherer_icon->offset = { 0.0f, 0.0f };
	gatherer_icon->section = { 754, 651, 104, 81 };
	gatherer_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 208, 1034, 202, 114 };
	panel->tex_id = panel_tex_ID;

	//------------------------- BARRACKS TOOLTIP --------------------------------------

	barrack_tooltip = App->scene->AddGameobject("Barrack Tooltip", selectionPanel);

	C_Image* barrack_tooltip_bg = new C_Image(barrack_tooltip);
	barrack_tooltip_bg->target = { 0.155f, -0.607f, 2.5f, 2.5f };
	barrack_tooltip_bg->offset = { 0.0f, 0.0f };
	barrack_tooltip_bg->section = { 456, 417, 87, 40 };
	barrack_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream barrack_life;
	barrack_life << "Life: ";
	barrack_life << 350;

	C_Text* barrack_tooltip_life = new C_Text(barrack_tooltip, barrack_life.str().c_str());
	barrack_tooltip_life->target = { 0.38f, -0.507f, 1.0f , 1.0f };

	std::stringstream barrack_description;
	barrack_description << "Barrack:";

	C_Text* barrack_tooltip_description = new C_Text(barrack_tooltip, barrack_description.str().c_str());
	barrack_tooltip_description->target = { 0.28f, -0.327f, 1.0f , 1.0f };

	std::stringstream barrack_info;
	barrack_info << "Bulding to create new units";

	C_Text* barrack_tooltip_info = new C_Text(barrack_tooltip, barrack_info.str().c_str());
	barrack_tooltip_info->target = { 0.28f, -0.227f, 1.0f , 1.0f };

	//------------------------- BARRACKS BUTTON --------------------------------------

	Gameobject* barracks_btn_go = App->scene->AddGameobject("Barracks Button", selectionPanel);

	barracks_btn = new C_Button(barracks_btn_go, Event(PLACE_BUILDING, App->scene, int(BARRACKS)));//First option from the right
	barracks_btn->target = { -0.0235f, 0.027f, 1.5f, 1.5f };
	barracks_btn->offset = { 0.0f, 0.0f };

	barracks_btn->section[0] = { 1153, 226, 46, 46 };
	barracks_btn->section[1] = { 1153, 175, 46, 46 };
	barracks_btn->section[2] = { 1153, 277, 46, 46 };
	barracks_btn->section[3] = { 1153, 277, 46, 46 };

	barracks_btn->tex_id = panel_tex_ID;

	//------------------------- TOWER TOOLTIP --------------------------------------

	tower_tooltip = App->scene->AddGameobject("Tower Tooltip", selectionPanel);

	C_Image* tower_tooltip_bg = new C_Image(tower_tooltip);
	tower_tooltip_bg->target = { 0.3885f, -0.63f, 2.5f, 2.5f };
	tower_tooltip_bg->offset = { 0.0f, 0.0f };
	tower_tooltip_bg->section = { 422, 376, 121, 40 };
	tower_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream tower_life;
	tower_life << "Life: ";
	tower_life << 75;

	C_Text* tower_tooltip_life = new C_Text(tower_tooltip, tower_life.str().c_str());
	tower_tooltip_life->target = { 0.6135f, -0.53f, 1.0f , 1.0f };

	std::stringstream tower_range;
	tower_range << "Range: ";
	tower_range << 13;

	C_Text* tower_tooltip_range = new C_Text(tower_tooltip, tower_range.str().c_str());
	tower_tooltip_range->target = { 0.9055f, -0.53f, 1.0f , 1.0f };

	std::stringstream tower_damage;
	tower_damage << "Dmg: ";
	tower_damage << 20;

	C_Text* tower_tooltip_damage = new C_Text(tower_tooltip, tower_damage.str().c_str());
	tower_tooltip_damage->target = { 1.1735f, -0.53, 1.0f , 1.0f };

	std::stringstream tower_description;
	tower_description << "Tower:";

	C_Text* tower_tooltip_description = new C_Text(tower_tooltip, tower_description.str().c_str());
	tower_tooltip_description->target = { 0.5135f, -0.357f, 1.0f , 1.0f };

	std::stringstream tower_info;
	tower_info << "Offensive building";

	C_Text* tower_tooltip_info = new C_Text(tower_tooltip, tower_info.str().c_str());
	tower_tooltip_info->target = { 0.5135f, -0.257f, 1.0f , 1.0f };

	//------------------------- TOWER BUTTON --------------------------------------

	Gameobject* tower_btn_go = App->scene->AddGameobject("Tower Button", selectionPanel);

	tower_btn = new C_Button(tower_btn_go, Event(PLACE_BUILDING, App->scene, int(TOWER)));//Second option from the right
	tower_btn->target = { 0.21f, 0.004, 1.5f, 1.5f };
	tower_btn->offset = { 0.0f, 0.0f };

	tower_btn->section[0] = { 1081, 398, 46, 46 };
	tower_btn->section[1] = { 1081, 347, 46, 46 };
	tower_btn->section[2] = { 1081, 449, 46, 46 };
	tower_btn->section[3] = { 1081, 449, 46, 46 };

	tower_btn->tex_id = panel_tex_ID;

	//------------------------- LAB TOOLTIP --------------------------------------

	lab_tooltip = App->scene->AddGameobject("Lab Tooltip", selectionPanel);

	C_Image* lab_tooltip_bg = new C_Image(lab_tooltip);
	lab_tooltip_bg->target = { 0.5885f, -0.414f, 2.5f, 2.5f };
	lab_tooltip_bg->offset = { 0.0f, 0.0f };
	lab_tooltip_bg->section = { 456, 417, 87, 40 };
	lab_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream lab_life;
	lab_life << "Life: ";
	lab_life << 200;

	C_Text* lab_tooltip_life = new C_Text(lab_tooltip, lab_life.str().c_str());
	lab_tooltip_life->target = { 0.8155f, -0.314f, 1.0f , 1.0f };

	std::stringstream lab_description;
	lab_description << "Lab:";

	C_Text* lab_tooltip_description = new C_Text(lab_tooltip, lab_description.str().c_str());
	lab_tooltip_description->target = { 0.7155f, -0.141f, 1.0f , 1.0f };

	std::stringstream lab_info;
	lab_info << "Building to upgrade ally units";

	C_Text* lab_tooltip_info = new C_Text(lab_tooltip, lab_info.str().c_str());
	lab_tooltip_info->target = { 0.7155f, -0.041f, 1.0f , 1.0f };

	//------------------------- LAB BUTTON --------------------------------------

	Gameobject* lab_btn_go = App->scene->AddGameobject("Lab Button", selectionPanel);

	labBtn = new C_Button(lab_btn_go, Event(PLACE_BUILDING, App->scene, int(LAB)));//Third option from the right
	labBtn->target = { 0.44f, 0.22f, 1.5f, 1.5f };
	labBtn->offset = { 0.0f, 0.0f };

	labBtn->section[0] = { 503,95,46,46 };
	labBtn->section[1] = { 503,44,46,46 };
	labBtn->section[2] = { 503,146,46,46 };
	labBtn->section[3] = { 503,146,46,46 };

	labBtn->tex_id = panel_tex_ID;

	//Tower price
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost1 = new C_Image(prices);
	cost1->target = { 0.33f, 0.08f, 0.8f, 0.8f };
	cost1->offset = { 0, 0 };
	cost1->section = { 225, 13, 35, 32 };
	cost1->tex_id = App->tex.Load("textures/icons_price.png");

	//Barracks price
	C_Image* cost2 = new C_Image(prices);
	cost2->target = { 0.11f, 0.1f, 0.8f, 0.8f };
	cost2->offset = { 0, 0 };
	cost2->section = { 268, 14, 35, 31 };
	cost2->tex_id = App->tex.Load("textures/icons_price.png");

	//Lab price
	C_Image* cost3 = new C_Image(prices);
	cost3->target = { 0.59f, 0.29f, 0.8f, 0.8f };
	cost3->offset = { 0, 0 };
	cost3->section = { 268, 14, 35, 31 };
	cost3->tex_id = App->tex.Load("textures/icons_price.png");

	std::stringstream ssLife;
	std::stringstream ssDamage;
	ssLife << "Life: ";
	ssLife << current_life;
	ssDamage << "Damage: ";
	ssDamage << damage;
	unitInfo = App->scene->AddGameobjectToCanvas("Unit info");
	unitLife = new C_Text(unitInfo, ssLife.str().c_str());//Text line
	unitLife->target = { 0.165f, 0.94f, 1.0f , 1.0f };
	unitDamage = new C_Text(unitInfo, ssDamage.str().c_str());//Text line
	unitDamage->target = { 0.165f, 0.96f, 1.0f , 1.0f };
}
