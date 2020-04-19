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

Tower::Tower(Gameobject* go) : Behaviour(go, TOWER, FULL_LIFE, B_TOWER)
{
	max_life = 50;
	current_life = max_life;
	attack_range = 20;
	vision_range = 25;
	damage = 20;
	ms_count = 0;
	atkDelay = 1.5;
	shoot = false;
	current_state = FULL_LIFE;
	atkObj = nullptr;

	create_bar();
	bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();

	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}
	
}

Tower::~Tower()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
	b_map.erase(GetID());
}

void Tower::Update()
{
	if (current_state != DESTROYED)
	{
		if (!shoot)
		{
			std::map<float, Behaviour*> inRange;
			int found = GetBehavioursInRange(game_object->GetTransform()->GetGlobalPosition(), attack_range, inRange);
			float d = 0;
			atkObj = nullptr;
			if (found > 0)
			{
				for (std::map<float, Behaviour*>::iterator it = inRange.begin(); it != inRange.end(); ++it)
				{
					if ((it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED) && it->second->GetState() != DESTROYED)
					{
						if (d == 0)
						{
							atkObj = it->second;
							d = it->first;
						}
						else
						{
							if (it->first < d)
							{
								atkObj = it->second;
								d = it->first;
							}
						}
					}
				}
			}
		}

		if (atkObj != nullptr) 
			if(atkObj->GetState() != DESTROYED) 
				if (ms_count >= atkDelay) DoAttack();
		

		if (ms_count < atkDelay) ms_count += App->time.GetGameDeltaTime();
		
		if (shoot)
		{
			rayCastTimer += App->time.GetGameDeltaTime();
			if (rayCastTimer < RAYCAST_TIME)
			{
				App->render->DrawLine(localPos, atkPos, { 34,191,255,255 }, FRONT_SCENE, true);
			}
			else
			{
				shoot = false;
				rayCastTimer = 0;
			}
		}
	}
}

void Tower::AfterDamageAction()
{
	if (current_state != DESTROYED)
	{
		update_health_ui();
		if (current_life <= 0)
			OnKill(type);
		else if (current_life >= max_life * 0.5f)
			current_state = FULL_LIFE;
		else
			current_state = HALF_LIFE;
	}
}


void Tower::Upgrade()
{
	if (t_lvl < t_max_lvl) 
	{
		t_lvl += 1;
		current_life += 25;
		max_life += 25;
		damage += 2;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("Tower LEVEL: %d", t_lvl);

		update_upgrades_ui();
		update_health_ui();
	}
}

void Tower::OnRightClick(vec pos, vec modPos)
{
	Upgrade();
	//OnDamage(10);
}

void Tower::DoAttack()
{
	atkPos.first = 0;
	atkPos.second = 0;
	vec pos = atkObj->GetGameobject()->GetTransform()->GetGlobalPosition();
	atkPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	atkPos.first += 30.0f;
	atkPos.second += 20.0f;

	pos = game_object->GetTransform()->GetGlobalPosition();
	localPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	localPos.first += 30.0f;
	localPos.second += 20.0f;

	Event::Push(DAMAGE, atkObj, damage);

	shoot = true;
	ms_count = 0;
	atkObj = nullptr;
}

void Tower::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("Assets/textures/buildPanelSample.png");

	//------------------------- TOWER PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Tower Panel");


	upgrade_btn = new C_Button(selectionPanel, Event(DO_UPGRADE, this->AsBehaviour()));//Top left
	upgrade_btn->target = { 0.912f, posY_panel + 0.02f, 1.0f, 1.0f };
	upgrade_btn->offset = { 0.0f, 0.0f };
	upgrade_btn->section = { 121, 38, 38, 38 };
	upgrade_btn->tex_id = panel_tex_ID;
}

void Tower::UpdatePanel()
{

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

	health = new C_Image(bar_go);
	health->target = { 0.345f, pos_y_HUD - 0.018f, 1.68f, 0.75f };
	health->offset = { -220.0f, -20.0f };
	health->section = { 39, 719, 220, 20 };
	health->tex_id = bar_text_id;

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

	health->target = { (0.345f - (0.345f - 0.058f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.018f, 1.68f * (float(current_life) / float(max_life)), 0.75f };
}

void Tower::update_upgrades_ui() {

	upgrades->section = { 16 + 36 * (t_lvl - 1), 806, 33, 33 };

}