#include "Tower.h"
#include "Application.h"
#include "Gameobject.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "MapContainer.h"
#include "Scene.h"
#include "Sprite.h"
#include "Canvas.h"

Tower::Tower(Gameobject* go, bool build_new) : Behaviour(go, TOWER, NO_UPGRADE, B_TOWER)
{
	max_life = 100;
	current_life = 1;
	attack_range = 13.0f;
	vision_range = 25.0f;
	damage = 20;
	ms_count = 0;
	atkDelay = 1.5;
	shoot = false;
	current_state = NO_UPGRADE;
	providesVisibility = true;
	spriteState = NO_UPGRADE;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();
	upgrade_tooltip->SetInactive();

	App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	SetColliders();
	mini_life_bar.Show();

	if (build_new)
		characteR->StartBuildEffect(5.0f);
}

Tower::~Tower()
{}

void Tower::FreeWalkabilityTiles()
{
	App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
}

void Tower::OnCollision(Collider selfCol, Collider col)
{
	if (current_state != DESTROYED
		&& selfCol.GetColliderTag() == PLAYER_ATTACK_TAG
		&& col.GetColliderTag() == ENEMY_TAG
		&& col.parentGo->GetBehaviour()->GetType() != EDGE
		&& col.parentGo->GetBehaviour()->GetType() != CAPSULE)
	{
		objective = col.parentGo->GetBehaviour();
	}
}

void Tower::Update()
{
	if (!active)
	{
		current_life = int(float(max_life) * characteR->GetBuildEffectProgress());
		
		if (current_life >= max_life)
		{
			current_life = max_life;
			active = true;
			mini_life_bar.Hide();

			std::stringstream ssLife;
			ssLife << "Life: ";
			ssLife << current_life;
			unitLife->text->SetText(ssLife.str().c_str());
		}

		mini_life_bar.Update(float(current_life) / float(max_life), current_lvl);
	}


	if (current_state != DESTROYED && active)
	{		
		if (objective != nullptr && objective->GetState() != DESTROYED)
			if (ms_count >= atkDelay) DoAttack();
		
		if (ms_count < atkDelay) ms_count += App->time.GetGameDeltaTime();
		
		//Draw vision and attack range
		if (drawRanges)
		{	
			vec pos = game_object->GetTransform()->GetGlobalPosition();
			std::pair<float, float> drawPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
			drawPos.first += 30.0f;
			drawPos.second += 30.0f;
			visionRange = { vision_range*23, vision_range*23 };
			atkRange = { attack_range*23, attack_range*23 };
			App->render->DrawCircle(drawPos, visionRange, { 10, 156, 18, 255 }, FRONT_SCENE, true);//Vision
			App->render->DrawCircle(drawPos, atkRange, { 255, 0, 0, 255 }, FRONT_SCENE, true);//Attack
		}
		objective = nullptr;
	}

	// Upgrade Tooltip Check
	if (upgrade_btn->state == 1 && upgrade_tooltip->IsActive() == false)
		upgrade_tooltip->SetActive();

	else if (upgrade_btn->state != 1 && upgrade_tooltip->IsActive() == true)
		upgrade_tooltip->SetInactive();
}

void Tower::Upgrade()
{
	if (App->scene->GetGearsCount() >= TOWER_UPGRADE_COST)
	{
		if (lvl < max_lvl)
		{
			App->scene->UpdateStat(int(CURRENT_MOB_DROP),int(-TOWER_UPGRADE_COST));
			lvl += 1;
			current_life += 25;
			max_life += 25;
			damage += 2;
			audio->Play(B_BUILDED);

			switch (current_state)
			{
			case NO_UPGRADE:
				current_state = FIRST_UPGRADE;
				spriteState = FIRST_UPGRADE;
				break;
			case FIRST_UPGRADE: 
				current_state = SECOND_UPGRADE;
				spriteState = SECOND_UPGRADE;
				break;
			}
		}
	}
}

void Tower::DoAttack()
{	
	App->particleSys.AddParticle(pos,objective->GetPos(),15.0f,ENERGY_BALL_PARTICLE);
	Event::Push(DAMAGE, objective, damage,GetType());
	ms_count = 0;
	objective = nullptr;
}

void Tower::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

	//------------------------- TOWER PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Tower Build Panel");

	tower_icon = new C_Image(selectionPanel);
	tower_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	tower_icon->offset = { 0.0f, 0.0f };
	tower_icon->section = { 439, 651, 104, 81 };
	tower_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 208, 1034, 202, 114 };
	panel->tex_id = panel_tex_ID;

	Gameobject* upgrade_btn_go = App->scene->AddGameobject("Upgrade Button", selectionPanel);

	//-----------------------------UPGRADE TOOLTIP-------------------------------------------

	upgrade_tooltip = App->scene->AddGameobject("Upgrade Tooltip", selectionPanel);

	C_Image* upgrade_tooltip_bg = new C_Image(upgrade_tooltip);
	upgrade_tooltip_bg->target = { 0.5885f, -0.0015, 4.1f, 2.5f };
	upgrade_tooltip_bg->offset = { 0.0f, 0.0f };
	upgrade_tooltip_bg->section = { 514, 772, 87, 40 };
	upgrade_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream upgrade_description;
	upgrade_description << "Upgrade:";

	C_Text* upgrade_tooltip_description = new C_Text(upgrade_tooltip, upgrade_description.str().c_str());
	upgrade_tooltip_description->target = { 0.7535f, 0.1585f, 1.0f , 1.0f };

	std::stringstream upgrade_info;
	upgrade_info << "Upgrade the tower to resist more enemy hits";

	C_Text* upgrade_tooltip_info = new C_Text(upgrade_tooltip, upgrade_info.str().c_str());
	upgrade_tooltip_info->target = { 0.7535f, 0.2585f, 1.0f , 1.0f };

	std::stringstream upgrade_info_2;
	upgrade_info_2 << "and deal more damage";

	C_Text* upgrade_tooltip_info_2 = new C_Text(upgrade_tooltip, upgrade_info_2.str().c_str());
	upgrade_tooltip_info_2->target = { 0.7535f, 0.3585f, 1.0f , 1.0f };

	//-----------------------------UPGRADE BUTTON-------------------------------------------

	upgrade_btn = new C_Button(upgrade_btn_go, Event(DO_UPGRADE, this->AsBehaviour()));//Last option from the right
	upgrade_btn->target = { 0.45f, 0.6325, 1.5f, 1.5f };
	upgrade_btn->offset = { 0.0f,0.0f };

	upgrade_btn->section[0] = { 1081, 54, 46, 46 };
	upgrade_btn->section[1] = { 1081, 3, 46, 46 };
	upgrade_btn->section[2] = { 1081, 105, 46, 46 };
	upgrade_btn->section[3] = { 1081, 105, 46, 46 };

	upgrade_btn->tex_id = panel_tex_ID;

	//Upgrade
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost4 = new C_Image(prices);
	cost4->target = { 0.58f, 0.68f, 0.8f, 0.8f };
	cost4->offset = { 0, 0 };
	cost4->section = { 229, 51, 34, 35 };
	cost4->tex_id = App->tex.Load("textures/icons_price.png");

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

void Tower::create_bar() {

	pos_y_HUD = 0.17 + 0.1 * 4;

	bar_text_id = App->tex.Load("textures/Iconos_square_up.png");

	//------------------------- TOWER BAR --------------------------------------

	bar_go = App->scene->AddGameobjectToCanvas("Tower Bar");

	bar = new C_Image(bar_go);
	bar->target = { 0.41f, pos_y_HUD, 1.3f, 1.2f };
	bar->offset = { -401.0f, -52.0f };
	bar->section = { 17, 634, 401, 52 };
	bar->tex_id = bar_text_id;

	//------------------------- TOWER PORTRAIT --------------------------------------

	portrait = new C_Image(bar_go);
	portrait->target = { 0.04f, pos_y_HUD - 0.014f, 1.0f, 1.0f };
	portrait->offset = { -25.0f, -40.0f };
	portrait->section = { 184, 462, 25, 40 };
	portrait->tex_id = bar_text_id;


	//------------------------- TOWER TEXT --------------------------------------

	text = new C_Text(bar_go, "Tower");
	text->target = { 0.058f, pos_y_HUD - 0.073f, 1.5f, 1.2f };

	//------------------------- TOWER RED HEALTH --------------------------------------

	red_health = new C_Image(bar_go);
	red_health->target = { 0.345f, pos_y_HUD - 0.018f, 1.68f, 0.75f };
	red_health->offset = { -220.0f, -20.0f };
	red_health->section = { 39, 696, 220, 20 };
	red_health->tex_id = bar_text_id;

	//------------------------- TOWER HEALTH --------------------------------------

	green_health = new C_Image(bar_go);
	green_health->target = { 0.345f, pos_y_HUD - 0.018f, 1.68f, 0.75f };
	green_health->offset = { -220.0f, -20.0f };
	green_health->section = { 39, 719, 220, 20 };
	green_health->tex_id = bar_text_id;

	//------------------------- TOWER HEALTH BOARDER --------------------------------------

	health_boarder = new C_Image(bar_go);
	health_boarder->target = { 0.345f, pos_y_HUD - 0.018f, 1.68f, 0.75f };
	health_boarder->offset = { -220.0f, -20.0f };
	health_boarder->section = { 39, 744, 220, 20 };
	health_boarder->tex_id = bar_text_id;

	//------------------------- TOWER UPGRADES --------------------------------------

	upgrades = new C_Image(bar_go);
	upgrades->target = { 0.385f, pos_y_HUD - 0.018f, 1.1f, 1.1f };
	upgrades->offset = { -33.0f, -33.0f };
	upgrades->section = { 16, 806, 33, 33 };
	upgrades->tex_id = bar_text_id;
}