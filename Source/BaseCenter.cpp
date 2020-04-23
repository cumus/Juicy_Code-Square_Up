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

Gameobject* Base_Center::baseCenter = nullptr;

Base_Center::Base_Center(Gameobject* go) : BuildingWithQueue(go, BASE_CENTER, NO_UPGRADE, B_BASE_CENTER)
{
	Transform* t = game_object->GetTransform();

	max_life = 1000;
	current_life = max_life;
	bc_lvl = 1;
	bc_max_lvl = 5;
	vision_range = 30.0f;
	create_bar();
	bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();
	create_creation_bar();
	creation_bar_go->SetInactive();

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

void Base_Center::Update()
{
	if (GetState() != DESTROYED)
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
	}
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
	if (bc_lvl < bc_max_lvl) {

		current_life += 50;
		max_life += 50;
		bc_lvl += 1;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("BC LEVEL: %d", bc_lvl);
		update_upgrades_ui();
		update_health_ui();
	}
}

void Base_Center::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("Assets/textures/buildPanelSample.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Main Base Build Panel");

	panel = new C_Image(selectionPanel);
	panel->target = { 0.9f, posY_panel, 1.0f, 1.0f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 0, 0, 119, 119 };
	panel->tex_id = panel_tex_ID;

	gatherer_btn = new C_Button(selectionPanel, Event(BUILD_GATHERER, this, spawnPoint, 5.0f));//Top left
	gatherer_btn->target = { 0.912f, posY_panel+0.02f, 0.7f, 0.7f };
	gatherer_btn->offset = { 0.0f, 0.0f };

	gatherer_btn->section[0] = { 142, 0, 62, 62 };
	gatherer_btn->section[1] = { 142, 130, 62, 62 };
	gatherer_btn->section[3] = { 142, 260, 62, 62 };
	gatherer_btn->section[2] = { 142, 260, 62, 62 };

	gatherer_btn->tex_id = panel_tex_ID;

	meleeUnit_btn = new C_Button(selectionPanel, Event(BUILD_MELEE, this, spawnPoint, 5.0f));//Top right
	meleeUnit_btn->target = { 0.95f, posY_panel+0.02f, 0.7f, 0.7f };
	meleeUnit_btn->offset = { 0.0f,0.0f };

	meleeUnit_btn->section[0] = { 207, 0, 62, 62 };
	meleeUnit_btn->section[1] = { 207, 130, 62, 62 };
	meleeUnit_btn->section[2] = { 207, 260, 62, 62 };
	meleeUnit_btn->section[3] = { 207, 260, 62, 62 };

	meleeUnit_btn->tex_id = panel_tex_ID;

	/*
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
	sa*/
}

void Base_Center::UpdatePanel()
{

}


void Base_Center::create_bar() {

	pos_y_HUD = 0.17;

	bar_text_id = App->tex.Load("Assets/textures/Iconos_square_up.png");

	//------------------------- BASE BAR --------------------------------------

	bar_go = App->scene->AddGameobjectToCanvas("Main Base Bar");

	bar = new C_Image(bar_go);
	bar->target = { 0.465f, pos_y_HUD, 1.3f, 1.2f };
	bar->offset = { -455.0f, -62.0f };
	bar->section = { 17, 561, 455, 62 };
	bar->tex_id = bar_text_id;

	//------------------------- BASE PORTRAIT --------------------------------------

	portrait = new C_Image(bar_go);
	portrait->target = { 0.045f, pos_y_HUD - 0.015f, 1.0f, 1.0f };
	portrait->offset = { -34.0f, -47.0f };
	portrait->section = { 132, 456, 34, 47 };
	portrait->tex_id = bar_text_id;

	//------------------------- BASE TEXT --------------------------------------

	text = new C_Text(bar_go, "Base");
	text->target = { 0.055f, pos_y_HUD - 0.085f, 1.5f, 1.5f };

	//------------------------- BASE RED HEALTH --------------------------------------

	red_health = new C_Image(bar_go);
	red_health->target = { 0.385f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	red_health->offset = { -220.0f, -20.0f };
	red_health->section = { 39, 696, 220, 20 };
	red_health->tex_id = bar_text_id;

	//------------------------- BASE HEALTH --------------------------------------

	health = new C_Image(bar_go);
	health->target = { 0.385f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	health->offset = { -220.0f, -20.0f };
	health->section = { 39, 719, 220, 20 };
	health->tex_id = bar_text_id;

	//------------------------- BASE HEALTH BOARDER --------------------------------------

	health_boarder = new C_Image(bar_go);
	health_boarder->target = { 0.385f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	health_boarder->offset = { -220.0f, -20.0f };
	health_boarder->section = { 39, 744, 220, 20 };
	health_boarder->tex_id = bar_text_id;

	//------------------------- BASE UPGRADES --------------------------------------

	upgrades = new C_Image(bar_go);
	upgrades->target = { 0.44f, pos_y_HUD - 0.02f, 1.3f, 1.3f };
	upgrades->offset = { -33.0f, -33.0f };
	upgrades->section = { 16, 806, 33, 33 };
	upgrades->tex_id = bar_text_id;

}

void Base_Center::update_health_ui() {

	health->target = { (0.385f) - ((0.385f - 0.059f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.02f, 1.92f * (float(current_life) / float(max_life)), 1.0f };

}

void Base_Center::update_upgrades_ui() {

	upgrades->section = { 16 + 36 * (bc_lvl - 1), 806, 33, 33 };

}

void Base_Center::create_creation_bar() {

	bar_text_id = App->tex.Load("Assets/textures/Iconos_square_up.png");

	creation_bar_go = App->scene->AddGameobjectToCanvas("Creation Bar");

	creation_bar = new C_Image(creation_bar_go);
	creation_bar->target = { 0.99f, 0.55f, 0.6f, 0.2f };
	creation_bar->offset = { -499.0f, -599.0f };
	creation_bar->section = { 527, 35, 499, 599 };
	creation_bar->tex_id = bar_text_id;

	gatherer_icon = new C_Image(creation_bar_go);
	gatherer_icon->target = { 0.8f, 0.44f, 1.f, 1.f };
	gatherer_icon->offset = { -48.f, -35.f };
	gatherer_icon->section = { 75, 458, 48, 35 };
	gatherer_icon->tex_id = bar_text_id;

	melee_icon = new C_Image(creation_bar_go);
	melee_icon->target = { 0.804f, 0.49f, 0.8f, 0.8f };
	melee_icon->offset = { -48.f, -35.f };
	melee_icon->section = { 22, 463, 48, 35 };
	melee_icon->tex_id = bar_text_id;

	/*
	ranged_icon = new C_Image(creation_bar_go);
	ranged_icon->target = { 0.804f, 0.54f, 0.8f, 0.8f };
	ranged_icon->offset = { -48.f, -35.f };
	ranged_icon->section = { 22, 463, 48, 35 };
	ranged_icon->tex_id = bar_text_id;
	*/

	gahterer_creation_bar = new C_Image(creation_bar_go);
	gahterer_creation_bar->target = { 0.97f, 0.42f,  0.9f, 0.4f };
	gahterer_creation_bar->offset = { -220.0f, -20.0f };
	gahterer_creation_bar->section = { 39, 696, 220, 20 };
	gahterer_creation_bar->tex_id = bar_text_id;

	gatherer_creation_bar_completed = new C_Image(creation_bar_go);
	gatherer_creation_bar_completed->offset = { -220.0f, -20.0f };
	gatherer_creation_bar_completed->section = { 39, 719, 220, 20 };
	gatherer_creation_bar_completed->tex_id = bar_text_id;

	gatherer_creation_bar_boarder = new C_Image(creation_bar_go);
	gatherer_creation_bar_boarder->target = { 0.97f, 0.42f, 0.9f, 0.4f };
	gatherer_creation_bar_boarder->offset = { -220.0f, -20.0f };
	gatherer_creation_bar_boarder->section = { 39, 744, 220, 20 };
	gatherer_creation_bar_boarder->tex_id = bar_text_id;

	melee_creation_bar = new C_Image(creation_bar_go);
	melee_creation_bar->target = { 0.97f, 0.47f, 0.9f, 0.4f };
	melee_creation_bar->offset = { -220.0f, -20.0f };
	melee_creation_bar->section = { 39, 696, 220, 20 };
	melee_creation_bar->tex_id = bar_text_id;

	melee_creation_bar_completed = new C_Image(creation_bar_go);
	melee_creation_bar_completed->offset = { -220.0f, -20.0f };
	melee_creation_bar_completed->section = { 39, 719, 220, 20 };
	melee_creation_bar_completed->tex_id = bar_text_id;

	melee_creation_bar_boarder = new C_Image(creation_bar_go);
	melee_creation_bar_boarder->target = { 0.97f, 0.47f, 0.9f, 0.4f };
	melee_creation_bar_boarder->offset = { -220.0f, -20.0f };
	melee_creation_bar_boarder->section = { 39, 744, 220, 20 };
	melee_creation_bar_boarder->tex_id = bar_text_id;

	ranged_creation_bar = new C_Image(creation_bar_go);
	ranged_creation_bar->target = { 0.97f, 0.52f, 0.9f, 0.4f };
	ranged_creation_bar->offset = { -220.0f, -20.0f };
	ranged_creation_bar->section = { 39, 696, 220, 20 };
	ranged_creation_bar->tex_id = bar_text_id;

	ranged_creation_bar_completed = new C_Image(creation_bar_go);
	ranged_creation_bar_completed->offset = { -220.0f, -20.0f };
	ranged_creation_bar_completed->section = { 39, 719, 220, 20 };
	ranged_creation_bar_completed->tex_id = bar_text_id;

	ranged_creation_bar_boarder = new C_Image(creation_bar_go);
	ranged_creation_bar_boarder->target = { 0.97f, 0.52f, 0.9f, 0.4f };
	ranged_creation_bar_boarder->offset = { -220.0f, -20.0f };
	ranged_creation_bar_boarder->section = { 39, 744, 220, 20 };
	ranged_creation_bar_boarder->tex_id = bar_text_id;
	
}

void Base_Center::update_creation_bar() {


	//gatherer_creation_bar_completed->target = { (0.97f) - (0.155f) * (1.0f - float(gatherer_timer) / float(CREATION_TIME)), 0.42f, 0.9f * (float(gatherer_timer) / float(CREATION_TIME)), 0.4f };
	//melee_creation_bar_completed->target = { (0.97f) - (0.155f) * (1.0f - float(melee_timer) / float(CREATION_TIME)), 0.47f, 0.9f * (float(melee_timer) / float(CREATION_TIME)), 0.4f };
	//ranged_creation_bar_completed->target = { 0.97f - (0.155f) * (1.0f - float(ranged_timer / CREATION_TIME)), 0.52f, 0.9f * (float(ranged_timer) / float(CREATION_TIME)), 0.4f };

}