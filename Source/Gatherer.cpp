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
	vision_range = 20.0f;
	dieDelay = 5.0f;
	providesVisibility = true;
	//deathFX = GATHERER_DIE_FX;
	//attackFX = GATHERER_ATK_FX;

	CreatePanel();
	selectionPanel->SetInactive();
	baseCollOffset = {0,-30};
	SetColliders();
}

Gatherer::~Gatherer(){}

void Gatherer::CreatePanel()
{
	panel_tex_ID = App->tex.Load("Assets/textures/buildPanelSample.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Gatherer Build Panel");
	btnTower = App->scene->AddGameobject("Gatherer buttons", selectionPanel);
	btnBarrack = App->scene->AddGameobject("Btn Barrack build ", selectionPanel);
	btnBaseCenter = App->scene->AddGameobject("Btn Base build", selectionPanel);

	panel = new C_Image(selectionPanel);
	panel->target = { 0.155f, 0.81f, 0.8f, 0.8f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 0, 0, 140, 139 };
	panel->tex_id = panel_tex_ID;

	barracks_btn = new C_Button(btnBarrack, Event(PLACE_BUILDING, App->scene, int(BARRACKS)));//Top left
	barracks_btn->target = { 0.1f, 0.08f, 0.7f, 0.7f };
	barracks_btn->offset = { 0.0f, 0.0f };

	barracks_btn->section[0] = { 337, 0, 62, 62 };
	barracks_btn->section[1] = { 337, 130, 62, 62 };
	barracks_btn->section[2] = { 337, 260, 62, 62 };
	barracks_btn->section[3] = { 337, 260, 62, 62 };

	barracks_btn->tex_id = panel_tex_ID;

	tower_btn = new C_Button(btnTower, Event(PLACE_BUILDING, App->scene, int(TOWER)));//Top right
	tower_btn->target = { 0.55f, 0.08f, 0.7f, 0.7f };
	tower_btn->offset = { 0.0f, 0.0f };

	tower_btn->section[0] = { 272, 65, 62, 62 };
	tower_btn->section[1] = { 272, 195, 62, 62 };
	tower_btn->section[2] = { 272, 325, 62, 62 };
	tower_btn->section[3] = { 272, 325, 62, 62 };

	tower_btn->tex_id = panel_tex_ID;

	baseBtn = new C_Button(btnBaseCenter, Event(PLACE_BUILDING, App->scene, int(BASE_CENTER)));//Bottom right
	baseBtn->target = { 0.55f, 0.55f, 0.7f, 0.7f };
	baseBtn->offset = { 0.0f, 0.0f };

	baseBtn->section[0] = { 272, 0, 62, 62 };
	baseBtn->section[1] = { 272, 130, 62, 62 };
	baseBtn->section[2] = { 272, 260, 62, 62 };
	baseBtn->section[3] = { 272, 260, 62, 62 };

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