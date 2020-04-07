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
	current_life = max_life = 100;
}

Base_Center::~Base_Center()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
}


void Base_Center::OnDamage(int d)
{
	if (current_state != DESTROYED)
	{
		current_life -= d;

		LOG("Current life: %d", current_life);

		update_health_ui();

		if (current_life <= 0)
			OnKill();
		else if (current_life >= max_life * 0.5f)
			current_state = FULL_LIFE;
		else
			current_state = HALF_LIFE;
	}
}

void Base_Center::OnKill()
{
	App->audio->PlayFx(B_DESTROYED);
	current_state = DESTROYED;
	game_object->Destroy(1.0f);
	main_base_bar_go->Destroy(1.0f);

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
	Gameobject* unit_go = App->scene->AddGameobject("Game Unit - son of root");
	unit_go->GetTransform()->SetLocalPos({x, y, 0.0f });
	new B_Unit(unit_go, UNIT_MELEE, IDLE);
	
}

void Base_Center::create_unit_bar() {

	pos_y = 0.17;

	base_bar_text_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE BAR --------------------------------------

	main_base_bar_go = App->scene->AddGameobject("Main Base Bar", App->scene->hud_canvas_go);

	main_base_bar = new C_Button(main_base_bar_go, Event(REQUEST_QUIT, App));
	main_base_bar->target = { 0.5f, pos_y, 1.3f, 1.2f };
	main_base_bar->offset = { -482.0f, -44.0f };
	main_base_bar->section = { 4, 7, 482, 44 };
	main_base_bar->tex_id = base_bar_text_id;

	//------------------------- BASE PORTRAIT --------------------------------------

	main_base_portrait = new C_Image(main_base_bar_go);
	main_base_portrait->target = { 0.06f, pos_y - 0.003f, 0.5f, 0.5f };
	main_base_portrait->offset = { -109.0f, -89.0f };
	main_base_portrait->section = { 126, 295, 109, 95 };
	main_base_portrait->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE TEXT --------------------------------------

	main_base_text = new C_Text(main_base_bar_go, "Base");
	main_base_text->target = { 0.07f, pos_y - 0.07f, 1.5f, 1.5f };

	//------------------------- BASE HEALTHBAR --------------------------------------

	main_base_healthbar = new C_Image(main_base_bar_go);
	main_base_healthbar->target = { 0.415f, pos_y - 0.01f, 1.8f, 0.7f };
	main_base_healthbar->offset = { -245.0f, -23.0f };
	main_base_healthbar->section = { 56, 192, 245, 23 };
	main_base_healthbar->tex_id = base_bar_text_id;

	//------------------------- BASE HEALTH --------------------------------------

	main_base_health = new C_Image(main_base_bar_go);
	main_base_health->target = { 0.415f, pos_y - 0.01f, 1.8f, 0.7f };
	main_base_health->offset = { -245.0f, -23.0f };
	main_base_health->section = { 57, 238, 245, 23 };
	main_base_health->tex_id = base_bar_text_id;
}

void Base_Center::update_health_ui() {

	main_base_health->target = { (0.415f) - ((0.415f - 0.07f) * (1.0f - float(current_life) / float(max_life))), pos_y - 0.01f, 1.8f * (float(current_life) / float(max_life)), 0.7f };

}

void Base_Center::update_upgrades_ui() {

	C_Image* main_base_upgrade = new C_Image(main_base_bar_go);
	main_base_upgrade->target = { 0.415f + (bc_lvl - 1) * 0.018f, pos_y - 0.01f, 0.18f, 0.4f };
	main_base_upgrade->offset = { -79.0f, -93.0f };
	main_base_upgrade->section = { 398, 78, 79, 93 };
	main_base_upgrade->tex_id = base_bar_text_id;

}