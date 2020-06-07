#include "MeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Collider.h"
#include "Log.h"

MeleeUnit::MeleeUnit(Gameobject* go) : B_Unit(go, UNIT_MELEE, IDLE, B_MELEE_UNIT)
{
	//Stats	
	max_life = 100;
	current_life = max_life;
	atkTime = 1.0f;
	speed = 3;
	damage = 5;
	attack_range = 2.0f;
	vision_range = 20.0f;
	providesVisibility = true;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();

	//SFX
	deathFX = MELEE_DIE_FX;
	attackFX = MELEE_ATK_FX;
	SetColliders();

}

MeleeUnit::~MeleeUnit() {}

void MeleeUnit::CreatePanel()
{
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------
	selectionPanel = App->scene->AddGameobjectToCanvas("Melee info Panel");

	melee_icon = new C_Image(selectionPanel);
	melee_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	melee_icon->offset = { 0.0f, 0.0f };
	melee_icon->section = { 439, 569, 104, 81 };
	melee_icon->tex_id = panel_tex_ID;

	panel = new C_Image(selectionPanel);
	panel->target = { 0.0f, 0.764f, 1.5f, 1.5f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 163, 343, 202, 114 };
	panel->tex_id = panel_tex_ID;

	std::stringstream ssLife;
	std::stringstream ssDamage;
	ssLife << "Life: ";
	ssLife << current_life;
	ssDamage << "Damage: ";
	ssDamage << damage;
	unitInfo = App->scene->AddGameobjectToCanvas("Unit info");
	unitLife = new C_Text(unitInfo, ssLife.str().c_str());//Text line
	unitLife->target = { 0.165f, 0.94f, 1.0f , 1.0f };
	unitDamage = new C_Text(unitInfo, ssDamage.str().c_str());//Text line
	unitDamage->target = { 0.165f, 0.96f, 1.0f , 1.0f };
}