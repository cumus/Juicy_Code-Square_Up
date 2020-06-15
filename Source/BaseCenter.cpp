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
	current_lvl = 1;
	max_lvl = 5;
	vision_range = 20.0f;
	attack_range = 0;
	providesVisibility = true;
	spriteState = NO_UPGRADE;
	current_state = NO_UPGRADE;
	active = true;
	create_bar();
	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();
	gatherer_tooltip->SetInactive();

	if (t)
	{
		vec pos = t->GetGlobalPosition();
		for (int i = 0; i < t->GetLocalScaleX(); i++)
			for (int a = 0; a < t->GetLocalScaleY(); a++)
				App->pathfinding.SetWalkabilityTile(int(pos.x) + i + 2, int(pos.y) + a - 1, false);
	}

	baseCenter = game_object;
	SetColliders();
}

Base_Center::~Base_Center()
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

	if (baseCenter == game_object)
		baseCenter = nullptr;
}


void Base_Center::FreeWalkabilityTiles()
{
	const Transform* t = game_object->GetTransform();
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
	pos = game_object->GetTransform()->GetGlobalPosition();

	if (providesVisibility)
		GetTilesInsideRadius();

	if (!build_queue.empty())
	{
		bool able_to_build = true;

		switch (build_queue.front().type)
		{
		case GATHERER:
			if (App->scene->GetStat(CURRENT_EDGE) <= GATHERER_COST)
				able_to_build = false;

			break;
		case UNIT_MELEE:
			if (App->scene->GetStat(CURRENT_EDGE) <= MELEE_COST)
				able_to_build = false;

			break;
		case UNIT_RANGED:
			if (App->scene->GetStat(CURRENT_EDGE) <= RANGED_COST)
				able_to_build = false;

			break;
		}

		if (!progress_bar->GetGameobject()->IsActive())
			progress_bar->GetGameobject()->SetActive();

		if (!icon->GetGameobject()->IsActive())
			icon->GetGameobject()->SetActive();

		float percent = able_to_build ? build_queue.front().Update() : 1.0f;
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

	mini_life_bar.Update(float(current_life) / float(max_life), current_lvl);

	// Gatherer Tooltip Check
	if (gatherer_btn->state == 1 && gatherer_tooltip->IsActive() == false)
		gatherer_tooltip->SetActive();

	else if (gatherer_btn->state != 1 && gatherer_tooltip->IsActive() == true)
		gatherer_tooltip->SetInactive();
}

void Base_Center::Upgrade()
{
	if (App->scene->GetGearsCount() >= BASE_UPGRADE_COST)
	{
		if (current_lvl < max_lvl)
		{
			App->scene->UpdateStat(int(CURRENT_MOB_DROP), int(-BASE_UPGRADE_COST));
			current_life += 50;
			max_life += 50;
			current_lvl += 1;
			audio->Play(B_BUILDED);
			update_health_ui();

			switch (current_state)
			{
			case NO_UPGRADE:
				spriteState = FIRST_UPGRADE;
				current_state = FIRST_UPGRADE;
				break;
			case FIRST_UPGRADE:
				spriteState = SECOND_UPGRADE;
				current_state = SECOND_UPGRADE;
				break;
			}
		}
	}
}

void Base_Center::CreatePanel()
{
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

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
	panel->section = { 208, 1034, 202, 114 };
	panel->tex_id = panel_tex_ID;
	
	//------------------------- GATHERER TOOLTIP --------------------------------------
	
	gatherer_tooltip = App->scene->AddGameobject("Gatherer Tooltip", selectionPanel);

	C_Image* gatherer_tooltip_bg = new C_Image(gatherer_tooltip);
	gatherer_tooltip_bg->target = { 0.155f, -0.607f, 2.5f, 2.5f };
	gatherer_tooltip_bg->offset = { 0.0f, 0.0f };
	gatherer_tooltip_bg->section = { 422, 376, 121, 40 };
	gatherer_tooltip_bg->tex_id = panel_tex_ID;

	std::stringstream gather_life;
	gather_life << "Life: ";
	gather_life << 20;

	C_Text* gatherer_tooltip_life = new C_Text(gatherer_tooltip, gather_life.str().c_str());
	gatherer_tooltip_life->target = { 0.38f, -0.507f, 1.0f , 1.0f };

	std::stringstream gather_range;
	gather_range << "Range: ";
	gather_range << 2;

	C_Text * gatherer_tooltip_range = new C_Text(gatherer_tooltip, gather_range.str().c_str());
	gatherer_tooltip_range->target = { 0.672f, -0.507, 1.0f , 1.0f };

	std::stringstream gather_damage;
	gather_damage << "Dmg: ";
	gather_damage << 10;

	C_Text* gatherer_tooltip_damage = new C_Text(gatherer_tooltip, gather_damage.str().c_str());
	gatherer_tooltip_damage->target = { 0.94f, -0.507, 1.0f , 1.0f };

	std::stringstream gather_info;
	gather_info << "Ally unit capable of mining and building";

	C_Text* gatherer_tooltip_info = new C_Text(gatherer_tooltip, gather_info.str().c_str());
	gatherer_tooltip_info->target = { 0.28f, -0.227f, 1.0f , 1.0f };

	//------------------------- GATHERER BUTTON --------------------------------------
	
	Gameobject* gatherer_btn_go = App->scene->AddGameobject("Gatherer Button", selectionPanel);

	gatherer_btn = new C_Button(gatherer_btn_go, Event(BUILD_GATHERER, this, spawnPoint, 5.0f));//First option from the right
	gatherer_btn->target = { -0.0235f, 0.027, 1.5f, 1.5f };
	gatherer_btn->offset = { 0.0f, 0.0f };

	gatherer_btn->section[0] = { 1081, 226, 46, 46 };
	gatherer_btn->section[1] = { 1081, 175, 46, 46 };
	gatherer_btn->section[2] = { 1081, 277, 46, 46 };
	gatherer_btn->section[3] = { 1081, 277, 46, 46 };

	gatherer_btn->tex_id = panel_tex_ID;

	//-----------------------------CAPSULE BUTTON-------------------------------------------

	Gameobject* capsule_btn_go = App->scene->AddGameobject("Capsule Button", selectionPanel);

	capsule_button = new C_Button(capsule_btn_go, Event(BUILD_CAPSULE, this, spawnPoint, 5.0f));//First option from the right
	capsule_button->target = { 0.21f, 0.004f, 1.5f, 1.5f };
	capsule_button->offset = { 0.0f, 0.0f };

	capsule_button->section[0] = { 503,249,46,46 };
	capsule_button->section[1] = { 503,198,46,46 };
	capsule_button->section[2] = { 503,300,46,46 };
	capsule_button->section[3] = { 503,300,46,46 };

	capsule_button->tex_id = panel_tex_ID;

	//-----------------------------UPGRADE BUTTON-------------------------------------------

	Gameobject* upgrade_btn_go = App->scene->AddGameobject("Upgrade Button", selectionPanel);

	upgrade_btn = new C_Button(upgrade_btn_go, Event(DO_UPGRADE, this->AsBehaviour()));//Last option from the right
	upgrade_btn->target = { 0.45f, 0.6325, 1.5f, 1.5f };
	upgrade_btn->offset = { 0.0f,0.0f };

	upgrade_btn->section[0] = { 1081, 54, 46, 46 };
	upgrade_btn->section[1] = { 1081, 3, 46, 46 };
	upgrade_btn->section[2] = { 1081, 105, 46, 46 };
	upgrade_btn->section[3] = { 1081, 105, 46, 46 };

	upgrade_btn->tex_id = panel_tex_ID;

	//Gatherer price
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost1 = new C_Image(prices);
	cost1->target = { 0.11f, 0.1f, 0.8f, 0.8f };
	cost1->offset = { 0, 0 };
	cost1->section = { 59, 13, 33, 31 };
	cost1->tex_id = App->tex.Load("textures/icons_price.png");

	//Capsule price
	C_Image* cost2 = new C_Image(prices);
	cost2->target = { 0.33f, 0.08f, 0.8f, 0.8f };
	cost2->offset = { 0, 0 };
	cost2->section = { 14, 14, 32, 29 };
	cost2->tex_id = App->tex.Load("textures/icons_price.png");

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


void Base_Center::create_bar() {

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

void Base_Center::update_health_ui()
{
	green_health->section.w = 439 * float(current_life) / float(max_life);
}