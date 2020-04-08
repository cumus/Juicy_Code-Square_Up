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



Base_Center::Base_Center(Gameobject* go) : Behaviour(go, BASE_CENTER, FULL_LIFE, B_BASE_CENTER)
{
	 max_life = 100;
	 current_life = max_life;
	create_unit_bar();
	unit_bar_go->SetInactive();
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}
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
}


void Base_Center::AfterDamageAction()
{
	if (current_life <= 0)
		OnKill();
	else if (current_life >= max_life * 0.5f)
		current_state = FULL_LIFE;
	else
		current_state = HALF_LIFE;
}


void Base_Center::Upgrade()
{
	if (bc_lvl < bc_max_lvl) {

		max_life += 50;
		bc_lvl += 1;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("BC LEVEL: %d", bc_lvl);
		update_upgrades_ui();
		update_health_ui();
	}
}


void Base_Center::OnRightClick(float x, float y)
{
	//Upgrade();
	SpawnUnit(x,y);
}

void Base_Center::SpawnUnit(float x,float y) 
{		
	Gameobject* unit_go = App->scene->AddGameobject("Melee Unit - son of root");
	unit_go->GetTransform()->SetLocalPos({x, y, 0.0f });
	new B_Unit(unit_go, UNIT_MELEE, IDLE);
	
}

void Base_Center::create_unit_bar() {

	pos_y_HUD = 0.17;

	unit_bar_text_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE BAR --------------------------------------

	unit_bar_go = App->scene->AddGameobject("Main Base Bar", App->scene->hud_canvas_go);

	unit_bar = new C_Button(unit_bar_go, Event(REQUEST_QUIT, App));
	unit_bar->target = { 0.5f, pos_y_HUD, 1.3f, 1.2f };
	unit_bar->offset = { -482.0f, -44.0f };
	unit_bar->section = { 4, 7, 482, 44 };
	unit_bar->tex_id = unit_bar_text_id;

	//------------------------- BASE PORTRAIT --------------------------------------

	unit_portrait = new C_Image(unit_bar_go);
	unit_portrait->target = { 0.06f, pos_y_HUD - 0.003f, 0.5f, 0.5f };
	unit_portrait->offset = { -109.0f, -89.0f };
	unit_portrait->section = { 126, 295, 109, 95 };
	unit_portrait->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE TEXT --------------------------------------

	unit_text = new C_Text(unit_bar_go, "Base");
	unit_text->target = { 0.07f, pos_y_HUD - 0.07f, 1.5f, 1.5f };

	//------------------------- BASE HEALTHBAR --------------------------------------

	unit_healthbar = new C_Image(unit_bar_go);
	unit_healthbar->target = { 0.415f, pos_y_HUD - 0.01f, 1.8f, 0.7f };
	unit_healthbar->offset = { -245.0f, -23.0f };
	unit_healthbar->section = { 56, 192, 245, 23 };
	unit_healthbar->tex_id = unit_bar_text_id;

	//------------------------- BASE HEALTH --------------------------------------

	unit_health = new C_Image(unit_bar_go);
	unit_health->target = { 0.415f, pos_y_HUD - 0.01f, 1.8f, 0.7f };
	unit_health->offset = { -245.0f, -23.0f };
	unit_health->section = { 57, 238, 245, 23 };
	unit_health->tex_id = unit_bar_text_id;
}

void Base_Center::update_health_ui() {

	unit_health->target = { (0.415f) - ((0.415f - 0.07f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.01f, 1.8f * (float(current_life) / float(max_life)), 0.7f };

}

void Base_Center::update_upgrades_ui() {

	C_Image* main_base_upgrade = new C_Image(unit_bar_go);
	main_base_upgrade->target = { 0.415f + (bc_lvl - 1) * 0.018f, pos_y_HUD - 0.01f, 0.18f, 0.4f };
	main_base_upgrade->offset = { -79.0f, -93.0f };
	main_base_upgrade->section = { 398, 78, 79, 93 };
	main_base_upgrade->tex_id = unit_bar_text_id;

}