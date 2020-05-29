#include "Tower.h"
#include "Application.h"
#include "Render.h"
#include "Gameobject.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "MapContainer.h"
#include "Scene.h"
#include "Canvas.h"

Tower::Tower(Gameobject* go) : Behaviour(go, TOWER, NO_UPGRADE, B_TOWER)
{
	max_life = 75;
	current_life = max_life;
	attack_range = 13.0f;
	vision_range = 25.0f;
	damage = 20;
	ms_count = 0;
	atkDelay = 1.5;
	shoot = false;
	current_state = NO_UPGRADE;
	providesVisibility = true;
	spriteState = NO_UPGRADE;

	// create_bar();
	// bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();

	/*Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}*/
	SetColliders();
}

Tower::~Tower()
{}

void Tower::FreeWalkabilityTiles()
{
	/*Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}*/
}

void Tower::OnCollision(Collider selfCol, Collider col)
{
	if (current_state != DESTROYED)
	{
		if (selfCol.GetColliderTag() == PLAYER_ATTACK_TAG)
		{
			//LOG("Atk");
			if (col.GetColliderTag() == ENEMY_TAG)
			{				
				if (col.parentGo->GetBehaviour()->GetType() != EDGE && col.parentGo->GetBehaviour()->GetType() != CAPSULE) objective = col.parentGo->GetBehaviour();
				//LOG("Eenmy unit in attack range");				
			}
		}
	}	
}

void Tower::Update()
{
	if (current_state != DESTROYED)
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
			LOG("LIFE AFTER UPGRADE: %d", max_life);
			LOG("Tower LEVEL: %d", lvl);

			update_upgrades_ui();
			update_health_ui();
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
	panel_tex_ID = App->tex.Load("Assets/textures/hud-sprites.png");

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
	panel->section = { 163, 343, 202, 114 };
	panel->tex_id = panel_tex_ID;

	Gameobject* upgrade_btn_go = App->scene->AddGameobject("Upgrade Button", selectionPanel);

	upgrade_btn = new C_Button(upgrade_btn_go, Event(DO_UPGRADE, this->AsBehaviour()));//Last option from the right
	upgrade_btn->target = { 0.4190f, 0.6075, 1.5f, 1.5f };
	upgrade_btn->offset = { 0.0f,0.0f };

	upgrade_btn->section[0] = { 1075, 51, 56, 49 };
	upgrade_btn->section[1] = { 1075, 0, 56, 49 };
	upgrade_btn->section[2] = { 1075, 102, 56, 49 };
	upgrade_btn->section[3] = { 1075, 102, 56, 49 };

	upgrade_btn->tex_id = panel_tex_ID;

	//Upgrade
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost4 = new C_Image(prices);
	cost4->target = { 0.58f, 0.68f, 0.8f, 0.8f };
	cost4->offset = { 0, 0 };
	cost4->section = { 229, 51, 34, 35 };
	cost4->tex_id = App->tex.Load("Assets/textures/icons_price.png");

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

	pos_y_HUD = 0.17 + 0.1 * 4;// App->scene->building_bars_created;

	bar_text_id = App->tex.Load("Assets/textures/Iconos_square_up.png");

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

void Tower::update_health_ui() {

//	health->target = { (0.345f - (0.345f - 0.058f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.018f, 1.68f * (float(current_life) / float(max_life)), 0.75f };
}

void Tower::update_upgrades_ui() {

//	upgrades->section = { 16 + 36 * (App->scene->t_lvl - 1), 806, 33, 33 };

}