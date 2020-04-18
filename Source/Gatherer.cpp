#include "Gatherer.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"


Gatherer::Gatherer(Gameobject* go) : B_Unit(go, GATHERER, IDLE, B_UNIT)
{
	atkDelay = 2.0;
	speed = 3;
	damage = 5;
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
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("Assets/textures/buildPanelSample.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobjectToCanvas("Gatherer Build Panel");

	panel = new C_Image(selectionPanel);
	panel->target = { 0.9f, posY_panel, 1.0f, 1.0f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 0, 0, 119, 119 };
	panel->tex_id = panel_tex_ID;

	barracks_btn = new C_Button(selectionPanel, Event(PLACE_BUILDING, App->scene, int(BARRACKS)));//Top left
	barracks_btn->target = { 0.912f, posY_panel + 0.02f, 1.0f, 1.0f };
	barracks_btn->offset = { 0.0f, 0.0f };
	barracks_btn->section = { 121, 38, 38, 38 };
	barracks_btn->tex_id = panel_tex_ID;

	tower_btn = new C_Button(selectionPanel, Event(PLACE_BUILDING, App->scene, int(TOWER)));//Top right
	tower_btn->target = { 0.95f, posY_panel + 0.02f, 1.0f, 1.0f };
	tower_btn->offset = { 0.0f,0.0f };
	tower_btn->section = { 121, 0, 38, 38 };
	tower_btn->tex_id = panel_tex_ID;

	/*wall_btn = new C_Button(selectionPanel, Event(PLACE_BUILDING,  App->scene, spawnPointX, spawnPointY));//Bottom left
	wall_btn->target = { 0.912f, posY_panel + 0.085f, 1.0f, 1.0f };
	wall_btn->offset = { 0.0f, 0.0f };
	wall_btn->section = { 161, 0, 38, 38 };
	wall_btn->tex_id = panel_tex_ID;*/

	baseBtn = new C_Button(selectionPanel, Event(PLACE_BUILDING,  App->scene, int(BASE_CENTER)));//Bottom right
	baseBtn->target = { 0.95f, posY_panel + 0.085f, 1.0f, 1.0f };
	baseBtn->offset = { 0.0f, 0.0f };
	baseBtn->section = { 162, 38, 38, 38 };
	baseBtn->tex_id = panel_tex_ID;
}

void Gatherer::UpdatePanel()
{

}