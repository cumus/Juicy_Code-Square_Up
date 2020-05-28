#include "Lab.h"
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


Lab::Lab(Gameobject* go) : Behaviour(go, LAB, NO_UPGRADE, B_LAB)
{
	Transform* t = game_object->GetTransform();

	max_life = 200;
	current_life = max_life;
	current_lvl = 1;
	max_lvl = 5;
	vision_range = 10.0f;
	attack_range = 0;
	providesVisibility = true;

	create_bar();
	//bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();


	if (t)
	{
		vec pos = t->GetGlobalPosition();
		for (int i = 0; i < t->GetLocalScaleX(); i++)
		{
			for (int a = 0; a < t->GetLocalScaleY(); a++)
			{
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 2, int(pos.y) + a - 1, false);
			}
		}
	}
	SetColliders();
}

Lab::~Lab()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		for (int i = 0; i < t->GetLocalScaleX(); i++)
		{
			for (int a = 0; a < t->GetLocalScaleY(); a++)
			{
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 2, int(pos.y) + a - 1, false);
			}
		}
	}

	b_map.erase(GetID());
}


void Lab::FreeWalkabilityTiles()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
}

void Lab::Update()
{
	mini_life_bar.Update(float(current_life) / float(max_life), current_lvl);
}


void Lab::Upgrade()
{
	if (current_lvl < max_lvl) {

		current_life += 50;
		max_life += 50;
		current_lvl += 1;
		App->audio->PlayFx(B_BUILDED);
		//LOG("LIFE AFTER UPGRADE: %d", max_life);
		//LOG("BC LEVEL: %d", current_lvl);
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

void Lab::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("Assets/textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------
	selectionPanel = App->scene->AddGameobjectToCanvas("Lab Panel");

	lab_icon = new C_Image(selectionPanel);
	lab_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	lab_icon->offset = { 0.0f, 0.0f };
	lab_icon->section = { 754, 651, 104, 81 };
	lab_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 163, 343, 202, 114 };
	panel->tex_id = panel_tex_ID;

	Gameobject* gUnit = App->scene->AddGameobject("Gatherer Button", selectionPanel);

	gatherer_btn = new C_Button(gUnit, Event(UPGRADE_GATHERER, App->scene, int(BARRACKS)));//First option from the right
	gatherer_btn->target = { -0.0535f, -0.007, 1.5f, 1.5f };
	gatherer_btn->offset = { 0.0f, 0.0f };

	gatherer_btn->section[0] = { 1147, 223, 56, 49 };
	gatherer_btn->section[1] = { 1147, 172, 56, 49 };
	gatherer_btn->section[2] = { 1147, 274, 56, 49 };
	gatherer_btn->section[3] = { 1147, 274, 56, 49 };

	gatherer_btn->tex_id = panel_tex_ID;

	Gameobject* mUnit_btn_go = App->scene->AddGameobject("Melee Button", selectionPanel);

	meleeUnit_btn = new C_Button(mUnit_btn_go, Event(UPGRADE_MELEE, App->scene, int(TOWER)));//Second option from the right
	meleeUnit_btn->target = { 0.18f, -0.024, 1.5f, 1.5f };
	meleeUnit_btn->offset = { 0.0f, 0.0f };

	meleeUnit_btn->section[0] = { 1075, 395, 56, 49 };
	meleeUnit_btn->section[1] = { 1075, 344, 56, 49 };
	meleeUnit_btn->section[2] = { 1075, 446, 56, 49 };
	meleeUnit_btn->section[3] = { 1075, 446, 56, 49 };

	meleeUnit_btn->tex_id = panel_tex_ID;

	Gameobject* rUnit_btn_go = App->scene->AddGameobject("Ranged Button", selectionPanel);

	rangedUnit_btn = new C_Button(rUnit_btn_go, Event(UPGRADE_RANGED, App->scene, int(LAB)));//Third option from the right
	rangedUnit_btn->target = { 0.38f, 0.20f, 1.5f, 1.5f };
	rangedUnit_btn->offset = { 0.0f, 0.0f };

	rangedUnit_btn->section[0] = { 1147, 395, 56, 49 };
	rangedUnit_btn->section[1] = { 1147, 344, 56, 49 };
	rangedUnit_btn->section[2] = { 1147, 446, 56, 49 };
	rangedUnit_btn->section[3] = { 1147, 446, 56, 49 };

	rangedUnit_btn->tex_id = panel_tex_ID;

	//TODO:
	//Super upgrade button
}


void Lab::create_bar() {

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

void Lab::update_health_ui() {
	green_health->section.w = 439 * float(current_life) / float(max_life);
}