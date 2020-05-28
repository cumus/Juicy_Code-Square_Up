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
	dieDelay = 5.0f;
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
	labBtn->target = { 0.38f, 0.20f, 1.5f, 1.5f };
	labBtn->offset = { 0.0f, 0.0f };

	labBtn->section[0] = { 1147, 395, 56, 49 };
	labBtn->section[1] = { 1147, 344, 56, 49 };
	labBtn->section[2] = { 1147, 446, 56, 49 };
	labBtn->section[3] = { 1147, 446, 56, 49 };

	labBtn->tex_id = panel_tex_ID;

	/*
	Gameobject* wall_btn_go = App->scene->AddGameobject("Wall Button", selectionPanel);

	wall_btn = new C_Button(wall_btn_go, Event(PLACE_BUILDING,  App->scene,int(WALL)));//Last option from the right
	wall_btn->target = { 0.4190f, 0.6075, 1.5f, 1.5f };
	wall_btn->offset = { 0.0f, 0.0f };
	wall_btn->section = { 161, 0, 38, 38 };
	wall_btn->tex_id = panel_tex_ID;*/
}

void Gatherer::UpdatePanel()
{

}