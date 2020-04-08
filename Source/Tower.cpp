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
	current_life = max_life = 50;
	attack_range = 10;
	vision_range = 13;
	damage = 20;
	ms_count = 0;
	atkDelay = 1.5;
	shoot = false;
	current_state = FULL_LIFE;
	vec pos = game_object->GetTransform()->GetGlobalPosition();
	localPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	//localPos.first -= App->render->GetCameraRect().x;
	//localPos.second -= App->render->GetCameraRect().y;

	create_unit_bar();
	unit_bar_go->SetInactive();

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

		if (atkObj != nullptr && atkObj->GetState() != DESTROYED)
		{
			if (ms_count >= atkDelay)
			{
				DoAttack();
			}

		}

		if (ms_count < atkDelay)
		{
			ms_count += App->time.GetGameDeltaTime();
		}

		if (shoot)
		{
			rayCastTimer += App->time.GetGameDeltaTime();
			if (rayCastTimer < RAYCAST_TIME)
			{
				App->render->DrawLine(localPos, atkPos, { 34,191,255,255 }, SCENE, false);
			}
			else
			{
				shoot = false;
				rayCastTimer = 0;
				atkObj = nullptr;
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
			OnKill();
		else if (current_life >= max_life * 0.5f)
			current_state = FULL_LIFE;
		else
			current_state = HALF_LIFE;
	}
}


void Tower::Upgrade()
{
	if (t_lvl < t_max_lvl) {
		
		t_lvl += 1;
		max_life += 25;
		damage += 2;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("Tower LEVEL: %d", t_lvl);

		update_upgrades_ui();
		update_health_ui();
	}

}

void Tower::OnRightClick(float x, float y)
{
	Upgrade();
	//OnDamage(10);
}

void Tower::DoAttack()
{
	vec pos = atkObj->GetGameobject()->GetTransform()->GetGlobalPosition();
	localPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	//atkPos.first -= App->render->GetCameraRect().x;
	//atkPos.second -= App->render->GetCameraRect().y;
	Event::Push(DAMAGE, atkObj, damage);
	shoot = true;
	ms_count = 0;
}

void Tower::create_unit_bar() {

	App->scene->building_bars_created++;

	pos_y_HUD = 0.17 + 0.07 * App->scene->building_bars_created;

	unit_bar_text_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BUILDING BAR --------------------------------------

	unit_bar_go = App->scene->AddGameobject("Tower Bar", App->scene->hud_canvas_go);

	unit_bar = new C_Button(unit_bar_go, Event(REQUEST_QUIT, App));
	unit_bar->target = { 0.405f, pos_y_HUD, 1.3f, 1.2f };
	unit_bar->offset = { -388.0f, -35.0f };
	unit_bar->section = { 4, 76, 388, 35 };
	unit_bar->tex_id = unit_bar_text_id;

	//------------------------- BUILDING PORTRAIT --------------------------------------

	unit_portrait = new C_Image(unit_bar_go);
	unit_portrait->target = { 0.06f, pos_y_HUD - 0.003f, 0.5f, 0.37f };
	unit_portrait->offset = { -109.0f, -93.0f };
	unit_portrait->section = { 4, 291, 109, 93 };
	unit_portrait->tex_id = unit_bar_text_id;


	//------------------------- BUILDING TEXT --------------------------------------

	unit_text = new C_Text(unit_bar_go, "Tower");
	unit_text->target = { 0.07f, pos_y_HUD - 0.055f, 1.5f, 1.2f };

	//------------------------- BUILDING HEALTHBAR --------------------------------------

	unit_healthbar = new C_Image(unit_bar_go);
	unit_healthbar->target = { 0.338f, pos_y_HUD - 0.01f, 1.4f, 0.5f };
	unit_healthbar->offset = { -245.0f, -23.0f };
	unit_healthbar->section = { 56, 192, 245, 23 };
	unit_healthbar->tex_id = unit_bar_text_id;

	//------------------------- BUILDING HEALTH --------------------------------------


	unit_health = new C_Image(unit_bar_go);
	unit_health->target = { 0.338f, pos_y_HUD - 0.01f, 1.4f, 0.5f };
	unit_health->offset = { -245.0f, -23.0f };
	unit_health->section = { 57, 238, 245, 23 };
	unit_health->tex_id = unit_bar_text_id;
}

void Tower::update_health_ui() {

	unit_health->target = { (0.338f) - ((0.338f - 0.07f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.01f, 1.4f * (float(current_life) / float(max_life)), 0.5f };
}

void Tower::update_upgrades_ui() {

	C_Image* tower_upgrades = new C_Image(unit_bar_go);
	tower_upgrades->target = { 0.338f + (t_lvl - 1) * 0.014f, pos_y_HUD - 0.01f, 0.14f, 0.3f };
	tower_upgrades->offset = { -79.0f, -93.0f };
	tower_upgrades->section = { 398, 78, 79, 93 };
	tower_upgrades->tex_id = unit_bar_text_id;
}