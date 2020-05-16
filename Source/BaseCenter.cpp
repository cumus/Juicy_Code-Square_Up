#include "BaseCenter.h"
#include "Application.h"
#include "Gameobject.h"
#include "Render.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "Input.h"
#include "SDL/include/SDL_scancode.h"
#include "Scene.h"
#include "Canvas.h"
#include "Sprite.h"

Gameobject* Base_Center::baseCenter = nullptr;

Base_Center::Base_Center(Gameobject* go) : BuildingWithQueue(go, BASE_CENTER, NO_UPGRADE, B_BASE_CENTER)
{
	Transform* t = game_object->GetTransform();

	max_life = 1000;
	current_life = max_life;
	lvl = 1;
	max_lvl = 5;
	vision_range = 20.0f;
	attack_range = 0;
	providesVisibility = true;

	create_bar();
	//bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();
	

	if (t)
	{
		//LOG("Set walkability");
		vec pos = t->GetGlobalPosition();
		//LOG("POS X:%f/Y:%f",pos.x,pos.y);
		for (int i = 0; i < t->GetLocalScaleX(); i++)
		{
			for (int a = 0; a < t->GetLocalScaleY(); a++)
			{
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 2, int(pos.y) + a - 1, false);
			}
		}
	}
	baseCenter = this->GetGameobject();
	SetColliders();
}

Base_Center::~Base_Center()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	b_map.erase(GetID());

	if (baseCenter == game_object)
		baseCenter = nullptr;
}


void Base_Center::FreeWalkabilityTiles()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}

	if (baseCenter == game_object)
		baseCenter = nullptr;
}

void Base_Center::Update()
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

	/*if (GetState() != DESTROYED)
	{
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(pos.x, pos.y, 0.5f), vision_range, out);//Get units in vision range
		float distance = 0;
		if (total_found > 0)//Check if found behaviours in range
		{
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED ||
					it->second->GetType() == ENEMY_SUPER || it->second->GetType() == ENEMY_SPECIAL) //Check if it is an emey
				{
					Behaviour::enemiesInSight.push_back(it->second->GetID());
					//LOG("Added to in sight");
				}
			}
		}
	}*/
}

void Base_Center::UpdateWalkabilityTiles()
{
	Transform* t = game_object->GetTransform();
	vec pos = t->GetGlobalPosition();
	//LOG("POS X:%f/Y:%f", pos.x, pos.y);
	for (int i = 0; i < t->GetLocalScaleX(); i++)
	{
		for (int a = 0; a < t->GetLocalScaleY(); a++)
		{
			App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 2, int(pos.y) + a - 1, false);
		}
	}
}

void Base_Center::AfterDamageAction()
{
	if (current_life <= 0)
		OnKill(type);
}


void Base_Center::Upgrade()
{
	if (lvl < max_lvl) {

		current_life += 50;
		max_life += 50;
		lvl += 1;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("BC LEVEL: %d", lvl);
		update_upgrades_ui();
		update_health_ui();

		switch (current_state)
		{
		case NO_UPGRADE:
			current_state = FIRST_UPGRADE;
			break;
		case FIRST_UPGRADE: current_state = SECOND_UPGRADE;
			break;
		}
	}
}

void Base_Center::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("Assets/textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Main Base Build Panel");

	base_icon = new C_Image(selectionPanel);
	base_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	base_icon->offset = { 0.0f, 0.0f };
	base_icon->section = { 544, 651, 104, 81 };
	base_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 163, 343, 202, 114 };
	panel->tex_id = panel_tex_ID;


	gatherer_btn = new C_Button(selectionPanel, Event(BUILD_GATHERER, this, spawnPoint, 5.0f));//First option from the right
	gatherer_btn->target = { -0.0125f, 0.7635, 1.5f, 1.5f };
	gatherer_btn->offset = { 0.0f, 0.0f };

	gatherer_btn->section[0] = { 1075, 223, 56, 49 };
	gatherer_btn->section[1] = { 1075, 172, 56, 49 };
	gatherer_btn->section[2] = { 1075, 274, 56, 49 };
	gatherer_btn->section[3] = { 1075, 274, 56, 49 };


	gatherer_btn->tex_id = panel_tex_ID;
	/*
	meleeUnit_btn = new C_Button(selectionPanel, Event(BUILD_MELEE, this, spawnPoint, 5.0f));//Top right
	meleeUnit_btn->target = { 0.95f, posY_panel+0.02f, 0.7f, 0.7f };
	meleeUnit_btn->offset = { 0.0f,0.0f };

	meleeUnit_btn->section[0] = { 207, 0, 62, 62 };
	meleeUnit_btn->section[1] = { 207, 130, 62, 62 };
	meleeUnit_btn->section[2] = { 207, 260, 62, 62 };
	meleeUnit_btn->section[3] = { 207, 260, 62, 62 };

	meleeUnit_btn->tex_id = panel_tex_ID;

	
	rangedUnit_btn = new C_Button(selectionPanel, Event(BUILD_RANGED, this, spawnPoint, 5.0f));//Bottom left
	rangedUnit_btn->target = { 0.912f, posY_panel+0.085f, 0.7f, 0.7f };
	rangedUnit_btn->offset = { 0.0f, 0.0f };

	rangedUnit_btn->section[0] = { 142, 65, 62, 62 };
	rangedUnit_btn->section[1] = { 142, 195, 62, 62 };
	rangedUnit_btn->section[2] = { 142, 325, 62, 62 };
	rangedUnit_btn->section[3] = { 142, 325, 62, 62 };

	rangedUnit_btn->tex_id = panel_tex_ID;

	superUnit_btn = new C_Button(selectionPanel, Event(BUILD_SUPER, this, spawnPoint, 5.0f));//Bottom right
	superUnit_btn->target = { 0.95f, posY_panel+0.085f, 0.7f, 0.7f };
	superUnit_btn->offset = { 0.0f, 0.0f };

	superUnit_btn->section[0] = { 207, 65, 62, 62 };
	superUnit_btn->section[1] = { 207, 195, 62, 62 };
	superUnit_btn->section[2] = { 207, 325, 62, 62 };
	superUnit_btn->section[3] = { 207, 325, 62, 62 };

	superUnit_btn->tex_id = panel_tex_ID;
	*/
	upgrade_btn = new C_Button(selectionPanel, Event(DO_UPGRADE, this->AsBehaviour()));//Last option from the right
	upgrade_btn->target = { 0.0990f, 0.9075, 1.5f, 1.5f };
	upgrade_btn->offset = { 0.0f,0.0f };

	upgrade_btn->section[0] = { 1075, 51, 56, 49 };
	upgrade_btn->section[1] = { 1075, 0, 56, 49 };
	upgrade_btn->section[2] = { 1075, 102, 56, 49 };
	upgrade_btn->section[3] = { 1075, 102, 56, 49 };

	upgrade_btn->tex_id = panel_tex_ID;
}

void Base_Center::UpdatePanel()
{

}


void Base_Center::create_bar() {

	bar_text_id = App->tex.Load("Assets/textures/hud-sprites.png");

	//------------------------- BASE BAR --------------------------------------

	bar_go = App->scene->AddGameobjectToCanvas("Main Base Bar");

	//------------------------- BASE HEALTH BOARDER ---------------------------

	health_boarder = new C_Image(bar_go);
	health_boarder->target = { 0.32f, 0.01f, 1.f, 0.9f };
	health_boarder->section = { 0, 772, 454, 44 };
	health_boarder->tex_id = bar_text_id;

	//------------------------- BASE RED HEALTH -------------------------------

	Gameobject* red_health_go = App->scene->AddGameobject("red bar health", bar_go);

	red_health = new C_Image(red_health_go);
	red_health->target = { 0.018f, 0.06f, 1.f, 0.9f };
	red_health->section = { 163, 733, 438, 38 };
	red_health->tex_id = bar_text_id;

	//------------------------- BASE GREEN HEALTH -----------------------------

	Gameobject* green_health_go = App->scene->AddGameobject("green bar health", bar_go);

	green_health = new C_Image(green_health_go);
	green_health->target = { 0.018f, 0.06f, 1.f, 0.9f };
	green_health->section = { 0, 817, 439, 38 };
	green_health->tex_id = bar_text_id;

	//------------------------- BASE UPGRADES ---------------------------------

	/*upgrades = new C_Image(bar_go);
	upgrades->target = { 0.44f, pos_y_HUD - 0.02f, 1.3f, 1.3f };
	upgrades->offset = { -33.0f, -33.0f };
	upgrades->section = { 16, 806, 33, 33 };
	upgrades->tex_id = bar_text_id;*/

}

void Base_Center::update_health_ui() {

	green_health->section.w = 439 * float(current_life) / float(max_life);

}

void Base_Center::update_upgrades_ui() {

	upgrades->section = { 16 + 36 * (lvl - 1), 806, 33, 33 };

}

void Base_Center::create_creation_bar() {

	
	
}

void Base_Center::update_creation_bar() {


	//gatherer_creation_bar_completed->target = { (0.97f) - (0.155f) * (1.0f - float(gatherer_timer) / float(CREATION_TIME)), 0.42f, 0.9f * (float(gatherer_timer) / float(CREATION_TIME)), 0.4f };
	//melee_creation_bar_completed->target = { (0.97f) - (0.155f) * (1.0f - float(melee_timer) / float(CREATION_TIME)), 0.47f, 0.9f * (float(melee_timer) / float(CREATION_TIME)), 0.4f };
	//ranged_creation_bar_completed->target = { 0.97f - (0.155f) * (1.0f - float(ranged_timer / CREATION_TIME)), 0.52f, 0.9f * (float(ranged_timer) / float(CREATION_TIME)), 0.4f };

}