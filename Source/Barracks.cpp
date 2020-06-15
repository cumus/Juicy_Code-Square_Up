#include "Barracks.h"
#include "Application.h"
#include "Gameobject.h"
#include "AudioSource.h"
#include "Transform.h"
#include "SDL/include/SDL_scancode.h"
#include "Scene.h"
#include "Canvas.h"
#include "Sprite.h"
#include "Log.h"
#include "MeleeUnit.h"

Barracks::Barracks(Gameobject* go, bool build_new) : BuildingWithQueue(go, BARRACKS, NO_UPGRADE, B_BARRACKS)
{
	Transform* t = game_object->GetTransform();

	max_life = 350;
	current_life = 1;
	buildQueue = 0;
	current_lvl = 1;
	max_lvl = 5;
	providesVisibility = true;
	vision_range = 15.0f;
	current_state = NO_UPGRADE;
	spriteState = NO_UPGRADE;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();
	melee_tooltip->SetInactive();
	ranged_tooltip->SetInactive();
	super_tooltip->SetInactive();
	upgrade_tooltip->SetInactive();

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
	mini_life_bar.Show();

	if (build_new)
		characteR->StartBuildEffect(10.0f);
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

void Barracks::Update()
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
	else
	{
		if (!build_queue.empty())
		{
			bool able_to_build = true;

			switch (build_queue.front().type)
			{
			case GATHERER:
				if (App->scene->GetStat(CURRENT_EDGE) <= GATHERER_COST)
				{
					able_to_build = false;
				}
				break;
			case UNIT_MELEE:
				if (App->scene->GetStat(CURRENT_EDGE) <= MELEE_COST)
				{
					able_to_build = false;
				}
				break;
			case UNIT_RANGED:
				if (App->scene->GetStat(CURRENT_EDGE) <= RANGED_COST)
				{
					able_to_build = false;
				}
				break;
			}

			if (!progress_bar->GetGameobject()->IsActive())
				progress_bar->GetGameobject()->SetActive();

			if (!icon->GetGameobject()->IsActive())
				icon->GetGameobject()->SetActive();

			float percent;
			if (able_to_build)
				percent = build_queue.front().Update();
			else
				percent = 1.0f;
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
				case GATHERER: icon->SetSection({ 121, 292, 43, 35 }); break;
				case UNIT_MELEE: icon->SetSection({ 121, 256, 48, 35 }); break;
				case UNIT_RANGED: icon->SetSection({ 231, 310, 35, 33 }); break;
				case UNIT_SUPER: icon->SetSection({ 121, 335, 35, 33 }); break;
				}

				SDL_Rect section = bar_section;
				section.w = int(float(section.w) * percent);
				progress_bar->SetSection(section);
			}
		}
	}

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

	// Upgrade Tooltip Check
	if (upgrade_btn->state == 1 && upgrade_tooltip->IsActive() == false)
		upgrade_tooltip->SetActive();

	else if (upgrade_btn->state != 1 && upgrade_tooltip->IsActive() == true)
		upgrade_tooltip->SetInactive();
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
			audio->Play(B_BUILDED);

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
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Barracks Build Panel");

	posY_panel = 0.81f;
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

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
	panel->section = { 208, 1034, 202, 114 };
	panel->tex_id = panel_tex_ID;

	//------------------------- MELEE TOOLTIP --------------------------------------

	melee_tooltip = App->scene->AddGameobject("Melee Tooltip", selectionPanel);

	C_Image* melee_tooltip_bg = new C_Image(melee_tooltip);
	melee_tooltip_bg->target = { 0.155f, -0.607, 2.5f, 2.5f };
	melee_tooltip_bg->offset = { 0.0f, 0.0f };
	melee_tooltip_bg->section = { 422, 376, 121, 40 };
	melee_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream melee_life;
	melee_life << "Life: ";
	melee_life << 100;

	C_Text* melee_tooltip_life = new C_Text(melee_tooltip, melee_life.str().c_str());
	melee_tooltip_life->target = { 0.38f, -0.507f, 1.0f , 1.0f };

	std::stringstream melee_range;
	melee_range << "Range: ";
	melee_range << 2;

	C_Text* melee_tooltip_range = new C_Text(melee_tooltip, melee_range.str().c_str());
	melee_tooltip_range->target = { 0.672f, -0.507, 1.0f , 1.0f };

	std::stringstream melee_damage;
	melee_damage << "Dmg: ";
	melee_damage << 5;

	C_Text* melee_tooltip_damage = new C_Text(melee_tooltip, melee_damage.str().c_str());
	melee_tooltip_damage->target = { 0.94f, -0.507, 1.0f , 1.0f };

	std::stringstream melee_description;
	melee_description << "Melee Unit:";

	C_Text* melee_tooltip_description = new C_Text(melee_tooltip, melee_description.str().c_str());
	melee_tooltip_description->target = { 0.28f, -0.327f, 1.0f , 1.0f };

	std::stringstream melee_info;
	melee_info << "Offensive ally unit";

	C_Text* melee_tooltip_info = new C_Text(melee_tooltip, melee_info.str().c_str());
	melee_tooltip_info->target = { 0.28f, -0.227f, 1.0f , 1.0f };

	//------------------------- MELEE BUTTON --------------------------------------

	Gameobject* melee_btn_go = App->scene->AddGameobject("Melee Button", selectionPanel);

	meleeUnit_btn = new C_Button(melee_btn_go, Event(BUILD_MELEE, this, spawnPoint, 5.0f));//First option from the right
	meleeUnit_btn->target = { -0.0235f, 0.027, 1.5f, 1.5f };
	meleeUnit_btn->offset = { 0.0f,0.0f };

	meleeUnit_btn->section[0] = { 1153, 54, 46, 46 };
	meleeUnit_btn->section[1] = { 1153, 3, 46, 46 };
	meleeUnit_btn->section[2] = { 1153, 105, 46, 46 };
	meleeUnit_btn->section[3] = { 1153, 105, 46, 46 };

	meleeUnit_btn->tex_id = panel_tex_ID;

	//------------------------- RANGED TOOLTIP --------------------------------------

	ranged_tooltip = App->scene->AddGameobject("Ranged Tooltip", selectionPanel);

	C_Image* ranged_tooltip_bg = new C_Image(ranged_tooltip);
	ranged_tooltip_bg->target = { 0.3885f, -0.63, 2.5f, 2.5f };
	ranged_tooltip_bg->offset = { 0.0f, 0.0f };
	ranged_tooltip_bg->section = { 422, 376, 121, 40 };
	ranged_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream ranged_life;
	ranged_life << "Life: ";
	ranged_life << 100;

	C_Text* ranged_tooltip_life = new C_Text(ranged_tooltip, ranged_life.str().c_str());
	ranged_tooltip_life->target = { 0.6135f, -0.53f, 1.0f , 1.0f };

	std::stringstream ranged_range;
	ranged_range << "Range: ";
	ranged_range << 13;

	C_Text* ranged_tooltip_range = new C_Text(ranged_tooltip, ranged_range.str().c_str());
	ranged_tooltip_range->target = { 0.9055f, -0.53f, 1.0f , 1.0f };

	std::stringstream ranged_damage;
	ranged_damage << "Dmg: ";
	ranged_damage << 15;

	C_Text* ranged_tooltip_damage = new C_Text(ranged_tooltip, ranged_damage.str().c_str());
	ranged_tooltip_damage->target = { 1.1735f, -0.53, 1.0f , 1.0f };

	std::stringstream ranged_description;
	ranged_description << "Ranged Unit:";

	C_Text* ranged_tooltip_description = new C_Text(ranged_tooltip, ranged_description.str().c_str());
	ranged_tooltip_description->target = { 0.5135f, -0.357f, 1.0f , 1.0f };

	std::stringstream ranged_info;
	ranged_info << "Offensive ally unit";

	C_Text* ranged_tooltip_info = new C_Text(ranged_tooltip, ranged_info.str().c_str());
	ranged_tooltip_info->target = { 0.5135f, -0.257f, 1.0f , 1.0f };

	//------------------------- RANGED BUTTON --------------------------------------
	
	Gameobject* rangedUnit_btn_go = App->scene->AddGameobject("Ranged Unit Button", selectionPanel);

	rangedUnit_btn = new C_Button(rangedUnit_btn_go, Event(BUILD_RANGED, this, spawnPoint, 5.0f));// Third option from the right
	rangedUnit_btn->target = { 0.21f, 0.004f, 1.5f, 1.5f };
	rangedUnit_btn->offset = { 0.0f, 0.0f };
	rangedUnit_btn->section[0] = { 1082, 730, 46, 46 };
	rangedUnit_btn->section[1] = { 1082, 679, 46, 46 };
	rangedUnit_btn->section[2] = { 1082, 781, 46, 46 };
	rangedUnit_btn->section[3] = { 1028, 781, 46, 46 };
	rangedUnit_btn->tex_id = panel_tex_ID;

	//------------------------- SUPER TOOLTIP --------------------------------------

	super_tooltip = App->scene->AddGameobject("Ranged Tooltip", selectionPanel);

	C_Image* super_tooltip_bg = new C_Image(super_tooltip);
	super_tooltip_bg->target = { 0.5885f, -0.414, 2.5f, 2.5f };
	super_tooltip_bg->offset = { 0.0f, 0.0f };
	super_tooltip_bg->section = { 422, 376, 121, 40 };
	super_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream super_life;
	super_life << "Life: ";
	super_life << 100;

	C_Text* super_tooltip_life = new C_Text(super_tooltip, super_life.str().c_str());
	super_tooltip_life->target = { 0.8155f, -0.314f, 1.0f , 1.0f };

	std::stringstream super_range;
	super_range << "Range: ";
	super_range << 13;

	C_Text* super_tooltip_range = new C_Text(super_tooltip, super_range.str().c_str());
	super_tooltip_range->target = { 1.1075f, -0.314f, 1.0f , 1.0f };

	std::stringstream super_damage;
	super_damage << "Dmg: ";
	super_damage << 15;

	C_Text* super_tooltip_damage = new C_Text(super_tooltip, super_damage.str().c_str());
	super_tooltip_damage->target = { 1.3755f, -0.314f, 1.0f , 1.0f };

	std::stringstream super_description;
	super_description << "Super Unit:";

	C_Text* super_tooltip_description = new C_Text(super_tooltip, super_description.str().c_str());
	super_tooltip_description->target = { 0.7155f, -0.141f, 1.0f , 1.0f };

	std::stringstream super_info;
	super_info << "Offensive ally unit";

	C_Text* super_tooltip_info = new C_Text(super_tooltip, super_info.str().c_str());
	super_tooltip_info->target = { 0.7155f, -0.041f, 1.0f , 1.0f };

	//------------------------- SUPER BUTTON --------------------------------------
	
	Gameobject* superUnit_btn_go = App->scene->AddGameobject("Super Unit Button", selectionPanel);

	superUnit_btn = new C_Button(superUnit_btn_go, Event(BUILD_SUPER, this, spawnPoint, 5.0f)); // Second option from the right
	superUnit_btn->target = { 0.45f, 0.22f, 1.5f, 1.5f };
	superUnit_btn->offset = { 0.0f, 0.0f };
	superUnit_btn->section[0] = { 1153, 730, 46, 46 };
	superUnit_btn->section[1] = { 1153, 679, 46, 46 };
	superUnit_btn->section[2] = { 1153, 781, 46, 46 };
	superUnit_btn->section[3] = { 1153, 781, 46, 46 };
	superUnit_btn->tex_id = panel_tex_ID;

	//-----------------------------UPGRADE TOOLTIP-------------------------------------------

	upgrade_tooltip = App->scene->AddGameobject("Upgrade Tooltip", selectionPanel);

	C_Image* upgrade_tooltip_bg = new C_Image(upgrade_tooltip);
	upgrade_tooltip_bg->target = { 0.5885f, -0.0015, 4.1f, 2.5f };
	upgrade_tooltip_bg->offset = { 0.0f, 0.0f };
	upgrade_tooltip_bg->section = { 514, 772, 87, 40 };
	upgrade_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream upgrade_description;
	upgrade_description << "Upgrade:";

	C_Text* upgrade_tooltip_description = new C_Text(upgrade_tooltip, upgrade_description.str().c_str());
	upgrade_tooltip_description->target = { 0.7535f, 0.2085f, 1.0f , 1.0f };

	std::stringstream upgrade_info;
	upgrade_info << "Upgrade the barrack to resist more enemy hits";

	C_Text* upgrade_tooltip_info = new C_Text(upgrade_tooltip, upgrade_info.str().c_str());
	upgrade_tooltip_info->target = { 0.7535f, 0.3085f, 1.0f , 1.0f };

	//------------------------- UPGRADE BUTTON --------------------------------------

	Gameobject* upgrade_btn_go = App->scene->AddGameobject("Upgrade Button", selectionPanel);

	upgrade_btn = new C_Button(upgrade_btn_go, Event(DO_UPGRADE, this->AsBehaviour()));//Last option from the right
	upgrade_btn->target = { 0.45f, 0.6325, 1.5f, 1.5f };
	upgrade_btn->offset = { 0.0f,0.0f };
	upgrade_btn->section[0] = { 1081, 54, 46, 46 };
	upgrade_btn->section[1] = { 1081, 3, 46, 46 };
	upgrade_btn->section[2] = { 1081, 105, 46, 46 };
	upgrade_btn->section[3] = { 1081, 105, 46, 46 };
	upgrade_btn->tex_id = panel_tex_ID;

	//Melee price
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost1 = new C_Image(prices);
	cost1->target = { 0.11f, 0.1f, 0.8f, 0.8f };
	cost1->offset = { 0, 0 };
	cost1->section = { 101, 13, 32, 31 };
	cost1->tex_id = App->tex.Load("textures/icons_price.png");

	//Ranged price
	C_Image* cost2 = new C_Image(prices);
	cost2->target = { 0.33f, 0.08f, 0.8f, 0.8f };
	cost2->offset = { 0, 0 };
	cost2->section = { 142, 13, 39, 31 };
	cost2->tex_id = App->tex.Load("textures/icons_price.png");

	//Super price
	C_Image* cost3 = new C_Image(prices);
	cost3->target = { 0.59f, 0.29f, 0.8f, 0.8f };
	cost3->offset = { 0, 0 };
	cost3->section = { 183, 12, 37, 32 };
	cost3->tex_id = App->tex.Load("textures/icons_price.png");

	//Upgrade
	C_Image* cost4 = new C_Image(prices);
	cost4->target = { 0.58f, 0.68f, 0.8f, 0.8f };
	cost4->offset = { 0, 0 };
	cost4->section = { 188, 52, 35, 33 };
	cost4->tex_id = App->tex.Load("textures/icons_price.png");

	std::stringstream ssLife;
	ssLife << "Life: ";
	ssLife << current_life;
	unitInfo = App->scene->AddGameobjectToCanvas("Unit info");
	unitLife = new C_Text(unitInfo, ssLife.str().c_str());//Text line
	unitLife->target = { 0.165f, 0.94f, 1.0f , 1.0f };
}


void Barracks::create_bar()
{
	pos_y_HUD = 0.17;

	bar_text_id = App->tex.Load("textures/Iconos_square_up.png");

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