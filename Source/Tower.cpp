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
	vec pos = game_object->GetTransform()->GetGlobalPosition();
	localPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	localPos.first += 30.0f;
	localPos.second += 20.0f;

	create_bar();
	bar_go->SetInactive();

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
			if (ms_count >= atkDelay) DoAttack();			
		}

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
				Event::Push(DAMAGE, atkObj, damage);
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
	atkPos.first = 0;
	atkPos.second = 0;
	vec pos = atkObj->GetGameobject()->GetTransform()->GetGlobalPosition();
	atkPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
	atkPos.first += 30.0f;
	atkPos.second += 20.0f;
	shoot = true;
	ms_count = 0;
	LOG("LX: %f / LY:%f", localPos.first, localPos.second);
	LOG("OX: %f / OY:%f", atkPos.first,atkPos.second);
}

void Tower::create_bar() {

	App->scene->building_bars_created++;

	pos_y_HUD = 0.17 + 0.07 * App->scene->building_bars_created;

	bar_text_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BUILDING BAR --------------------------------------

	bar_go = App->scene->AddGameobject("Tower Bar", App->scene->hud_canvas_go);

	bar = new C_Button(bar_go, Event(REQUEST_QUIT, App));
	bar->target = { 0.405f, pos_y_HUD, 1.3f, 1.2f };
	bar->offset = { -388.0f, -35.0f };
	bar->section = { 4, 76, 388, 35 };
	bar->tex_id = bar_text_id;

	//------------------------- BUILDING PORTRAIT --------------------------------------

	portrait = new C_Image(bar_go);
	portrait->target = { 0.06f, pos_y_HUD - 0.003f, 0.5f, 0.37f };
	portrait->offset = { -109.0f, -93.0f };
	portrait->section = { 4, 291, 109, 93 };
	portrait->tex_id = bar_text_id;


	//------------------------- BUILDING TEXT --------------------------------------

	text = new C_Text(bar_go, "Tower");
	text->target = { 0.07f, pos_y_HUD - 0.055f, 1.5f, 1.2f };

	//------------------------- BUILDING HEALTHBAR --------------------------------------

	healthbar = new C_Image(bar_go);
	healthbar->target = { 0.338f, pos_y_HUD - 0.01f, 1.4f, 0.5f };
	healthbar->offset = { -245.0f, -23.0f };
	healthbar->section = { 56, 192, 245, 23 };
	healthbar->tex_id = bar_text_id;

	//------------------------- BUILDING HEALTH --------------------------------------


	health = new C_Image(bar_go);
	health->target = { 0.338f, pos_y_HUD - 0.01f, 1.4f, 0.5f };
	health->offset = { -245.0f, -23.0f };
	health->section = { 57, 238, 245, 23 };
	health->tex_id = bar_text_id;
}

void Tower::update_health_ui() {

	health->target = { (0.338f) - ((0.338f - 0.07f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.01f, 1.4f * (float(current_life) / float(max_life)), 0.5f };
}

void Tower::update_upgrades_ui() {

	C_Image* tower_upgrades = new C_Image(bar_go);
	tower_upgrades->target = { 0.338f + (t_lvl - 1) * 0.014f, pos_y_HUD - 0.01f, 0.14f, 0.3f };
	tower_upgrades->offset = { -79.0f, -93.0f };
	tower_upgrades->section = { 398, 78, 79, 93 };
	tower_upgrades->tex_id = bar_text_id;
}