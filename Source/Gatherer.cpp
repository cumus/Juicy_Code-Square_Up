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
	atkDelay = 2.0;
	speed = 3;
	damage = 10;
	current_life = max_life = 20;
	attack_range = 2.0f;
	vision_range = 10.0f;
	dieDelay = 5.0f;
	//deathFX = GATHERER_DIE_FX;
	//attackFX = GATHERER_ATK_FX;

	CreatePanel();
	selectionPanel->SetInactive();

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

	barracks_btn = new C_Button(selectionPanel, Event(PLACE_BUILDING, App->scene, int(BARRACKS)));//Top left
	barracks_btn->target = { -0.0165f, 0.7625, 1.5f, 1.5f };
	barracks_btn->offset = { 0.0f, 0.0f };

	barracks_btn->section[0] = { 1064, 0, 56, 49 };
	barracks_btn->section[1] = { 1122, 0, 56, 49 };
	barracks_btn->section[2] = { 1180, 0, 56, 49 };
	barracks_btn->section[3] = { 1180, 0, 56, 49 };

	barracks_btn->tex_id = panel_tex_ID;

	tower_btn = new C_Button(selectionPanel, Event(PLACE_BUILDING, App->scene, int(TOWER)));//Top right
	tower_btn->target = { 0.0385f, 0.7575, 1.5f, 1.5f };
	tower_btn->offset = { 0.0f, 0.0f };

	tower_btn->section[0] = { 1064, 51, 56, 49 };
	tower_btn->section[1] = { 1122, 51, 56, 49 };
	tower_btn->section[2] = { 1180, 51, 56, 49 };
	tower_btn->section[3] = { 1180, 51, 56, 49 };

	tower_btn->tex_id = panel_tex_ID;

	baseBtn = new C_Button(selectionPanel, Event(PLACE_BUILDING, App->scene, int(BASE_CENTER)));//Bottom right
	baseBtn->target = { 0.085f, 0.8095f, 1.5f, 1.5f };
	baseBtn->offset = { 0.0f, 0.0f };

	baseBtn->section[0] = { 1064, 153, 56, 49 };
	baseBtn->section[1] = { 1122, 153, 56, 49 };
	baseBtn->section[2] = { 1180, 153, 56, 49 };
	baseBtn->section[3] = { 1180, 153, 56, 49 };

	baseBtn->tex_id = panel_tex_ID;


	/*wall_btn = new C_Button(selectionPanel, Event(PLACE_BUILDING,  App->scene, spawnPointX, spawnPointY));//Bottom left
	wall_btn->target = { 0.912f, posY_panel + 0.085f, 1.0f, 1.0f };
	wall_btn->offset = { 0.0f, 0.0f };
	wall_btn->section = { 161, 0, 38, 38 };
	wall_btn->tex_id = panel_tex_ID;*/

}

void Gatherer::UpdatePanel()
{

}