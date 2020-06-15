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


Lab::Lab(Gameobject* go, bool build_new) : Behaviour(go, LAB, NO_UPGRADE, B_LAB)
{
	Transform* t = game_object->GetTransform();

	max_life = 200;
	current_life = 1;
	current_lvl = 1;
	max_lvl = 5;
	vision_range = 15.0f;
	attack_range = 0;
	providesVisibility = true;
	spriteState = NO_UPGRADE;
	current_state = NO_UPGRADE;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();
	gatherer_tooltip->SetInactive();
	melee_tooltip->SetInactive();
	ranged_tooltip->SetInactive();
	super_tooltip->SetInactive();

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
	mini_life_bar.Show();

	if (build_new)
		characteR->StartBuildEffect(15.0f);
}

Lab::~Lab()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		for (int i = 0; i < t->GetLocalScaleX(); i++)
			for (int a = 0; a < t->GetLocalScaleY(); a++)
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 2, int(pos.y) + a - 1, false);
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
	if (!active)
	{
		current_life = int(float(max_life) * characteR->GetBuildEffectProgress());

		if (current_life >= max_life)
		{
			current_life = max_life;
			active = true;
			mini_life_bar.Hide();

			std::stringstream ssLife;
			ssLife << "Life: ";
			ssLife << current_life;
			unitLife->text->SetText(ssLife.str().c_str());
		}

		mini_life_bar.Update(float(current_life) / float(max_life), current_lvl);
	}

	// Gatherer Tooltip Check
	if (gatherer_btn->state == 1 && gatherer_tooltip->IsActive() == false)
		gatherer_tooltip->SetActive();
	else if (gatherer_btn->state != 1 && gatherer_tooltip->IsActive() == true)
		gatherer_tooltip->SetInactive();


	// Melee Tooltip Check
	if (meleeUnit_btn->state == 1 && melee_tooltip->IsActive() == false)
		melee_tooltip->SetActive();
	else if (meleeUnit_btn->state != 1 && melee_tooltip->IsActive() == true)
		melee_tooltip->SetInactive();

	

	// Ranged Tooltip Check
	if (rangedUnit_btn->state == 1 && ranged_tooltip->IsActive() == false)
		ranged_tooltip->SetActive();
	else if (rangedUnit_btn->state != 1 && ranged_tooltip->IsActive() == true)
		ranged_tooltip->SetInactive();

	

	// Super Tooltip Check
	if (superUnit_btn->state == 1 && super_tooltip->IsActive() == false)
		super_tooltip->SetActive();
	else if (superUnit_btn->state != 1 && super_tooltip->IsActive() == true)
		super_tooltip->SetInactive();

}


void Lab::Upgrade()
{
	if (App->scene->GetGearsCount() >= LAB_UPGRADE_COST)
	{
		if (current_lvl < max_lvl)
		{
			App->scene->UpdateStat(int(CURRENT_MOB_DROP), int(-LAB_UPGRADE_COST));
			current_life += 50;
			max_life += 50;
			current_lvl += 1;
			audio->Play(B_BUILDED);
			green_health->section.w = 439 * float(current_life) / float(max_life);

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

void Lab::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Lab Panel");

	lab_icon = new C_Image(selectionPanel);
	lab_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	lab_icon->offset = { 0.0f, 0.0f };
	lab_icon->section = { 456, 487, 104, 81 };
	lab_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 208, 1034, 202, 114 };
	panel->tex_id = panel_tex_ID;

	//-----------------------------GATHERER TOOLTIP-------------------------------------------

	gatherer_tooltip = App->scene->AddGameobject("Gatherer Tooltip", selectionPanel);

	C_Image* gatherer_tooltip_bg = new C_Image(gatherer_tooltip);
	gatherer_tooltip_bg->target = { 0.155f, -0.607f, 4.1f, 2.5f };
	gatherer_tooltip_bg->offset = { 0.0f, 0.0f };
	gatherer_tooltip_bg->section = { 514, 772, 87, 40 };
	gatherer_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream gatherer_description;
	gatherer_description << "Upgrade Gatherer:";

	C_Text* gatherer_tooltip_description = new C_Text(gatherer_tooltip, gatherer_description.str().c_str());
	gatherer_tooltip_description->target = { 0.32f, -0.447f, 1.0f , 1.0f };

	std::stringstream gatherer_info;
	gatherer_info << "Upgrade the gatherer unit to resist more enemy hits,";

	C_Text* gatherer_tooltip_info = new C_Text(gatherer_tooltip, gatherer_info.str().c_str());
	gatherer_tooltip_info->target = { 0.32f, -0.347f, 1.0f , 1.0f };

	std::stringstream gatherer_info_2;
	gatherer_info_2 << "deal more damage and have more range";

	C_Text* gatherer_tooltip_info_2 = new C_Text(gatherer_tooltip, gatherer_info_2.str().c_str());
	gatherer_tooltip_info_2->target = { 0.32f, -0.247f, 1.0f , 1.0f };

	//-----------------------------GATHERER BUTTON-------------------------------------------

	Gameobject* gUnit = App->scene->AddGameobject("Gatherer Button", selectionPanel);

	gatherer_btn = new C_Button(gUnit, Event(UPGRADE_GATHERER, App->scene));//First option from the right
	gatherer_btn->target = { -0.0535f, -0.007, 1.5f, 1.5f };
	gatherer_btn->offset = { 0.0f, 0.0f };

	gatherer_btn->section[0] = { 1075, 223, 52, 48 };
	gatherer_btn->section[1] = { 1075, 172, 52, 48 };
	gatherer_btn->section[2] = { 1075, 274, 52, 48 };
	gatherer_btn->section[3] = { 1075, 274, 52, 48 };

	gatherer_btn->tex_id = panel_tex_ID;

	//-----------------------------MELEE UNIT TOOLTIP-------------------------------------------

	melee_tooltip = App->scene->AddGameobject("Melee Tooltip", selectionPanel);

	C_Image* melee_tooltip_bg = new C_Image(melee_tooltip);
	melee_tooltip_bg->target = { 0.3885f, -0.63f, 4.1f, 2.5f };
	melee_tooltip_bg->offset = { 0.0f, 0.0f };
	melee_tooltip_bg->section = { 514, 772, 87, 40 };
	melee_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream melee_description;
	melee_description << "Upgrade Melee Unit:";

	C_Text* melee_tooltip_description = new C_Text(melee_tooltip, melee_description.str().c_str());
	melee_tooltip_description->target = { 0.5535f, -0.47f, 1.0f , 1.0f };

	std::stringstream melee_info;
	melee_info << "Upgrade the melee unit to resist more enemy hits,";

	C_Text* melee_tooltip_info = new C_Text(melee_tooltip, melee_info.str().c_str());
	melee_tooltip_info->target = { 0.5535f, -0.37f, 1.0f , 1.0f };

	std::stringstream melee_info_2;
	melee_info_2 << "deal more damage and have more range";

	C_Text* melee_tooltip_info_2 = new C_Text(melee_tooltip, melee_info_2.str().c_str());
	melee_tooltip_info_2->target = { 0.5535f, -0.27f, 1.0f , 1.0f };

	//-----------------------------MELEE UNIT BUTTON-------------------------------------------

	Gameobject* mUnit_btn_go = App->scene->AddGameobject("Melee Button", selectionPanel);

	meleeUnit_btn = new C_Button(mUnit_btn_go, Event(UPGRADE_MELEE, App->scene));//Second option from the right
	meleeUnit_btn->target = { 0.21f, 0.004, 1.5f, 1.5f };
	meleeUnit_btn->offset = { 0.0f, 0.0f };

	meleeUnit_btn->section[0] = { 1153, 54, 46, 46 };
	meleeUnit_btn->section[1] = { 1153, 3, 46, 46 };
	meleeUnit_btn->section[2] = { 1153, 105, 46, 46 };
	meleeUnit_btn->section[3] = { 1153, 105, 46, 46 };

	meleeUnit_btn->tex_id = panel_tex_ID;

	//-----------------------------RANGED UNIT TOOLTIP-------------------------------------------

	ranged_tooltip = App->scene->AddGameobject("Ranged Tooltip", selectionPanel);

	C_Image* ranged_tooltip_bg = new C_Image(ranged_tooltip);
	ranged_tooltip_bg->target = { 0.5885f, -0.414, 4.1f, 2.5f };
	ranged_tooltip_bg->offset = { 0.0f, 0.0f };
	ranged_tooltip_bg->section = { 514, 772, 87, 40 };
	ranged_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream ranged_description;
	ranged_description << "Upgrade Ranged Unit:";

	C_Text* ranged_tooltip_description = new C_Text(ranged_tooltip, ranged_description.str().c_str());
	ranged_tooltip_description->target = { 0.7535f, -0.254f, 1.0f , 1.0f };

	std::stringstream ranged_info;
	ranged_info << "Upgrade the ranged unit to resist more enemy hits,";

	C_Text* ranged_tooltip_info = new C_Text(ranged_tooltip, ranged_info.str().c_str());
	ranged_tooltip_info->target = { 0.7535f, -0.154f, 1.0f , 1.0f };

	std::stringstream ranged_info_2;
	ranged_info_2 << "deal more damage and have more range";

	C_Text* ranged_tooltip_info_2 = new C_Text(ranged_tooltip, ranged_info_2.str().c_str());
	ranged_tooltip_info_2->target = { 0.7535f, -0.054f, 1.0f , 1.0f };

	//-----------------------------RANGED UNIT BUTTON-------------------------------------------

	Gameobject* rUnit_btn_go = App->scene->AddGameobject("Ranged Button", selectionPanel);

	rangedUnit_btn = new C_Button(rUnit_btn_go, Event(UPGRADE_RANGED, App->scene));//Third option from the right
	rangedUnit_btn->target = { 0.44f, 0.22f, 1.5f, 1.5f };
	rangedUnit_btn->offset = { 0.0f, 0.0f };

	rangedUnit_btn->section[0] = { 1081, 730, 46, 46 };
	rangedUnit_btn->section[1] = { 1081, 679, 46, 46 };
	rangedUnit_btn->section[2] = { 1081, 781, 46, 46 };
	rangedUnit_btn->section[3] = { 1081, 781, 46, 46 };

	rangedUnit_btn->tex_id = panel_tex_ID;

	//-----------------------------SUPER UNIT TOOLTIP-------------------------------------------

	super_tooltip = App->scene->AddGameobject("Super Tooltip", selectionPanel);

	C_Image* super_tooltip_bg = new C_Image(super_tooltip);
	super_tooltip_bg->target = { 0.5885f, -0.0015, 4.1f, 2.5f };
	super_tooltip_bg->offset = { 0.0f, 0.0f };
	super_tooltip_bg->section = { 514, 772, 87, 40 };
	super_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream super_description;
	super_description << "Upgrade Super Unit:";

	C_Text* super_tooltip_description = new C_Text(super_tooltip, super_description.str().c_str());
	super_tooltip_description->target = { 0.7535f, 0.1585f, 1.0f , 1.0f };

	std::stringstream super_info;
	super_info << "Upgrade the super unit to resist more enemy hits,";

	C_Text* super_tooltip_info = new C_Text(super_tooltip, super_info.str().c_str());
	super_tooltip_info->target = { 0.7535f, 0.2585f, 1.0f , 1.0f };

	std::stringstream super_info_2;
	super_info_2 << "deal more damage and have more rangedx";

	C_Text* super_tooltip_info_2 = new C_Text(super_tooltip, super_info_2.str().c_str());
	super_tooltip_info_2->target = { 0.7535f, 0.3585f, 1.0f , 1.0f };

	//-----------------------------SUPER UNIT BUTTON-------------------------------------------

	Gameobject* sUnit_btn_go = App->scene->AddGameobject("Super Button", selectionPanel);

	superUnit_btn = new C_Button(sUnit_btn_go, Event(UPGRADE_SUPER, App->scene));//Last option from the right
	superUnit_btn->target = { 0.45f, 0.6325, 1.5f, 1.5f };
	superUnit_btn->offset = { 0.0f, 0.0f };

	superUnit_btn->section[0] = { 1153, 730, 46, 46 };
	superUnit_btn->section[1] = { 1153, 679, 46, 46 };
	superUnit_btn->section[2] = { 1153, 781, 46, 46 };
	superUnit_btn->section[3] = { 1153, 781, 46, 46 };

	superUnit_btn->tex_id = panel_tex_ID;

	//Gatherer price
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost1 = new C_Image(prices);
	cost1->target = { 0.11f, 0.1f, 0.8f, 0.8f };
	cost1->offset = { 0, 0 };
	cost1->section = { 17, 50, 32, 32 };
	cost1->tex_id = App->tex.Load("textures/icons_price.png");

	//Melee price
	C_Image* cost2 = new C_Image(prices);
	cost2->target = { 0.33f, 0.08f, 0.8f, 0.8f };
	cost2->offset = { 0, 0 };
	cost2->section = { 60, 51, 36, 33 };
	cost2->tex_id = App->tex.Load("textures/icons_price.png");

	//Ranged price
	C_Image* cost3 = new C_Image(prices);
	cost3->target = { 0.59f, 0.29f, 0.8f, 0.8f };
	cost3->offset = { 0, 0 };
	cost3->section = { 104, 52, 33, 33 };
	cost3->tex_id = App->tex.Load("textures/icons_price.png");

	//Super price
	C_Image* cost4 = new C_Image(prices);
	cost4->target = { 0.58f, 0.68f, 0.8f, 0.8f };
	cost4->offset = { 0, 0 };
	cost4->section = { 147, 52, 38, 33 };
	cost4->tex_id = App->tex.Load("textures/icons_price.png");

	std::stringstream ssLife;
	ssLife << "Life: ";
	ssLife << current_life;
	unitInfo = App->scene->AddGameobjectToCanvas("Unit info");
	unitLife = new C_Text(unitInfo, ssLife.str().c_str());//Text line
	unitLife->target = { 0.165f, 0.94f, 1.0f , 1.0f };
}


void Lab::create_bar() {

	bar_text_id = App->tex.Load("textures/hud-sprites.png");

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
}