#include "Gatherer.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"


Gatherer::Gatherer(Gameobject* go) : B_Unit(go, GATHERER, IDLE, B_GATHERER)
{
	atkTime = 2.0f;
	speed = 3;
	damage = 10;
	current_life = max_life = 20;
	attack_range = 2.0f;
	vision_range = 20.0f;
	providesVisibility = true;
	deathFX = GATHERER_DIE_FX;
	attackFX = GATHERER_ATK_FX;

	CreatePanel();
	selectionPanel->SetInactive();
	baseCollOffset = {0,-30};
	SetColliders();
}

Gatherer::~Gatherer(){}

void Gatherer::CreatePanel()
{
	panel_tex_ID = App->tex.Load("Assets/textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------
	selectionPanel = App->scene->AddGameobjectToCanvas("Gatherer Build Panel");
	
	gatherer_icon = new C_Image(selectionPanel);
	gatherer_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	gatherer_icon->offset = { 0.0f, 0.0f };
	gatherer_icon->section = { 754, 651, 104, 81 };
	gatherer_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 163, 343, 202, 114 };
	panel->tex_id = panel_tex_ID;

	Gameobject* barracks_btn_go = App->scene->AddGameobject("Barracks Button", selectionPanel);

	barracks_btn = new C_Button(barracks_btn_go, Event(PLACE_BUILDING, App->scene, int(BARRACKS)));//First option from the right
	barracks_btn->target = { -0.0535f, -0.007, 1.5f, 1.5f };
	barracks_btn->offset = { 0.0f, 0.0f };

	barracks_btn->section[0] = { 1147, 223, 56, 49 };
	barracks_btn->section[1] = { 1147, 172, 56, 49 };
	barracks_btn->section[2] = { 1147, 274, 56, 49 };
	barracks_btn->section[3] = { 1147, 274, 56, 49 };

	barracks_btn->tex_id = panel_tex_ID;

	Gameobject* tower_btn_go = App->scene->AddGameobject("Tower Button", selectionPanel);

	tower_btn = new C_Button(tower_btn_go, Event(PLACE_BUILDING, App->scene, int(TOWER)));//Second option from the right
	tower_btn->target = { 0.18f, -0.024, 1.5f, 1.5f };
	tower_btn->offset = { 0.0f, 0.0f };

	tower_btn->section[0] = { 1075, 395, 56, 49 };
	tower_btn->section[1] = { 1075, 344, 56, 49 };
	tower_btn->section[2] = { 1075, 446, 56, 49 };
	tower_btn->section[3] = { 1075, 446, 56, 49 };

	tower_btn->tex_id = panel_tex_ID;

	Gameobject* lab_btn_go = App->scene->AddGameobject("Lab Button", selectionPanel);

	labBtn = new C_Button(lab_btn_go, Event(PLACE_BUILDING, App->scene, int(LAB)));//Third option from the right
	labBtn->target = { 0.4f, 0.22f, 1.5f, 1.5f };
	labBtn->offset = { 0.0f, 0.0f };

	labBtn->section[0] = { 503,95,46,46 };
	labBtn->section[1] = { 503,44,46,46 };
	labBtn->section[2] = { 503,146,46,46 };
	labBtn->section[3] = { 503,146,46,46 };

	labBtn->tex_id = panel_tex_ID;

	//Tower price
	Gameobject* prices = App->scene->AddGameobject("Prices", selectionPanel);;
	C_Image* cost1 = new C_Image(prices);
	cost1->target = { 0.11f, 0.1f, 0.8f, 0.8f };
	cost1->offset = { 0, 0 };
	cost1->section = { 225, 13, 35, 32 };
	cost1->tex_id = App->tex.Load("Assets/textures/icons_price.png");
	//Barracks price
	C_Image* cost2 = new C_Image(prices);
	cost2->target = { 0.33f, 0.08f, 0.8f, 0.8f };
	cost2->offset = { 0, 0 };
	cost2->section = { 268, 14, 35, 31 };
	cost2->tex_id = App->tex.Load("Assets/textures/icons_price.png");
	//Lab price
	C_Image* cost3 = new C_Image(prices);
	cost3->target = { 0.52f, 0.29f, 0.8f, 0.8f };
	cost3->offset = { 0, 0 };
	cost3->section = { 268, 14, 35, 31 };
	cost3->tex_id = App->tex.Load("Assets/textures/icons_price.png");
}
