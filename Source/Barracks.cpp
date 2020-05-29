#include "Barracks.h"
#include "Application.h"
#include "Gameobject.h"
#include "Render.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Input.h"
#include "SDL/include/SDL_scancode.h"
#include "Scene.h"
#include "Canvas.h"
#include "Barracks.h"
#include "Log.h"


Barracks::Barracks(Gameobject* go) : BuildingWithQueue(go, BARRACKS, NO_UPGRADE, B_BARRACKS)
{
	Transform* t = game_object->GetTransform();

	max_life = 350;
	current_life = max_life;
	buildQueue = 0;
	current_lvl = 1;
	max_lvl = 5;
	providesVisibility = true;
	vision_range = 15.0f;
	current_state = NO_UPGRADE;
	spriteState = NO_UPGRADE;

	//create_bar();
	//bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();

	if (t)
	{
		vec pos = t->GetGlobalPosition();
		vec scale = t->GetGlobalScale();
		for (int i = 0; i < scale.x; i++)
		{
			for (int a = 0; a < scale.y; a++)
			{				
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 3, int(pos.y) + a - 2, false);
			}
		}
	}
	SetColliders();
}

Barracks::~Barracks()
{}

void Barracks::FreeWalkabilityTiles()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		vec scale = t->GetGlobalScale();
		for (int i = 0; i < scale.x; i++)
		{
			for (int a = 0; a < scale.y; a++)
			{
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 3, int(pos.y) + a - 2, true);
			}
		}
	}
}


void Barracks::Upgrade()
{
	if (App->scene->GetGearsCount() >= BARRACKS_UPGRADE_COST)
	{
		if (current_lvl < max_lvl)
		{
			App->scene->UpdateStat(int(CURRENT_MOB_DROP), int(-BARRACKS_UPGRADE_COST));
			current_life += 50;
			max_life += 50;
			current_lvl += 1;
			App->audio->PlayFx(B_BUILDED);
			LOG("LIFE AFTER UPGRADE: %d", max_life);
			LOG("BC LEVEL: %d", current_lvl);
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

void Barracks::CreatePanel()
{
	posY_panel = 0.81f;
	panel_tex_ID = App->tex.Load("Assets/textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Barracks Build Panel");

	posY_panel = 0.81f;
	panel_tex_ID = App->tex.Load("Assets/textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Barracks Build Panel");

	barrack_icon = new C_Image(selectionPanel);
	barrack_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	barrack_icon->offset = { 0.0f, 0.0f };
	barrack_icon->section = { 649, 651, 104, 81 };
	barrack_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 163, 343, 202, 114 };
	panel->tex_id = panel_tex_ID;

	Gameobject* melee_btn_go = App->scene->AddGameobject("Melee Button", selectionPanel);

	meleeUnit_btn = new C_Button(melee_btn_go, Event(BUILD_MELEE, this, spawnPoint, 5.0f));//First option from the right
	meleeUnit_btn->target = { -0.0535f, -0.007, 1.5f, 1.5f };
	meleeUnit_btn->offset = { 0.0f,0.0f };

	meleeUnit_btn->section[0] = { 1147, 51, 56, 49 };
	meleeUnit_btn->section[1] = { 1147, 0, 56, 49 };
	meleeUnit_btn->section[2] = { 1147, 102, 56, 49 };
	meleeUnit_btn->section[3] = { 1147, 102, 56, 49 };

	meleeUnit_btn->tex_id = panel_tex_ID;

	Gameobject* rangedUnit_btn_go = App->scene->AddGameobject("Ranged Unit Button", selectionPanel);

	rangedUnit_btn = new C_Button(rangedUnit_btn_go, Event(BUILD_RANGED, this, spawnPoint, 5.0f));// Third option from the right
	rangedUnit_btn->target = { 0.18f, -0.024, 1.5f, 1.5f };
	rangedUnit_btn->offset = { 0.0f, 0.0f };

	rangedUnit_btn->section[0] = { 1076, 727, 56, 49 };
	rangedUnit_btn->section[1] = { 1076, 676, 56, 49 };
	rangedUnit_btn->section[2] = { 1076, 775, 56, 49 };
	rangedUnit_btn->section[3] = { 1076, 775, 56, 49 };

	rangedUnit_btn->tex_id = panel_tex_ID;
	
	Gameobject* superUnit_btn_go = App->scene->AddGameobject("Super Unit Button", selectionPanel);

	superUnit_btn = new C_Button(superUnit_btn_go, Event(BUILD_SUPER, this, spawnPoint, 5.0f)); // Second option from the right
	superUnit_btn->target = { 0.38f, 0.20f, 1.5f, 1.5f };
	superUnit_btn->offset = { 0.0f, 0.0f };

	superUnit_btn->section[0] = { 1147, 727, 56, 49 };
	superUnit_btn->section[1] = { 1147, 676, 56, 49 };
	superUnit_btn->section[2] = { 1147, 775, 56, 49 };
	superUnit_btn->section[3] = { 1147, 775, 56, 49 };

	superUnit_btn->tex_id = panel_tex_ID;

	Gameobject* upgrade_btn_go = App->scene->AddGameobject("Upgrade Button", selectionPanel);

	upgrade_btn = new C_Button(upgrade_btn_go, Event(DO_UPGRADE, this->AsBehaviour()));//Last option from the right
	upgrade_btn->target = { 0.4190f, 0.6075, 1.5f, 1.5f };
	upgrade_btn->offset = { 0.0f,0.0f };

	upgrade_btn->section[0] = { 1075, 51, 56, 49 };
	upgrade_btn->section[1] = { 1075, 0, 56, 49 };
	upgrade_btn->section[2] = { 1075, 102, 56, 49 };
	upgrade_btn->section[3] = { 1075, 102, 56, 49 };

	upgrade_btn->tex_id = panel_tex_ID;

	//Melee price
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost1 = new C_Image(prices);
	cost1->target = { 0.11f, 0.1f, 0.8f, 0.8f };
	cost1->offset = { 0, 0 };
	cost1->section = { 101, 13, 32, 31 };
	cost1->tex_id = App->tex.Load("Assets/textures/icons_price.png");
	//Ranged price
	C_Image* cost2 = new C_Image(prices);
	cost2->target = { 0.33f, 0.08f, 0.8f, 0.8f };
	cost2->offset = { 0, 0 };
	cost2->section = { 142, 13, 39, 31 };
	cost2->tex_id = App->tex.Load("Assets/textures/icons_price.png");
	//Super price
	C_Image* cost3 = new C_Image(prices);
	cost3->target = { 0.52f, 0.29f, 0.8f, 0.8f };
	cost3->offset = { 0, 0 };
	cost3->section = { 183, 12, 37, 32 };
	cost3->tex_id = App->tex.Load("Assets/textures/icons_price.png");
}


void Barracks::create_bar() {

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
	portrait->target = { 0.042f, pos_y_HUD - 0.01f, 1.0f, 1.0f };
	portrait->offset = { -34.0f, -47.0f };
	portrait->section = { 218, 468, 45, 34 };
	portrait->tex_id = bar_text_id;

	//------------------------- BASE TEXT --------------------------------------

	text = new C_Text(bar_go, "Barracks");
	text->target = { 0.055f, pos_y_HUD - 0.085f, 1.5f, 1.5f };

	//------------------------- BASE RED HEALTH --------------------------------------

	red_health = new C_Image(bar_go);
	red_health->target = { 0.385f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	red_health->offset = { -220.0f, -20.0f };
	red_health->section = { 39, 696, 220, 20 };
	red_health->tex_id = bar_text_id;

	//------------------------- BASE HEALTH --------------------------------------

	green_health = new C_Image(bar_go);
	green_health->target = { 0.385f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	green_health->offset = { -220.0f, -20.0f };
	green_health->section = { 39, 719, 220, 20 };
	green_health->tex_id = bar_text_id;

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