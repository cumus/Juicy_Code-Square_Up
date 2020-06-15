#include "RangedUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "ParticleSystem.h"

RangedUnit::RangedUnit(Gameobject* go) : B_Unit(go, UNIT_RANGED, IDLE, B_RANGED)
{
	//Stats
	max_life = 100;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 15;
	attack_range = 13.0f;
	vision_range = 20.0f;
	providesVisibility = true;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();

	//SFX
	deathFX = RANGED_DIE_FX;
	attackFX = RANGED_ATK_FX;
	SetColliders();
}

RangedUnit::~RangedUnit() {}

void RangedUnit::UnitAttackType()
{
	attackPos = atkObj->GetPos();
	App->particleSys.AddParticle(pos,attackPos,8.0f, ORANGE_PARTICLE);
	audio->Play(attackFX);
}

void RangedUnit::CreatePanel()
{
	panel_tex_ID = App->tex.Load("textures/Hud_Sprites.png");

	//------------------------- BASE PANEL --------------------------------------
	selectionPanel = App->scene->AddGameobjectToCanvas("Ranged info Panel");

	ranged_icon = new C_Image(selectionPanel);
	ranged_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	ranged_icon->offset = { 0.0f, 0.0f };
	ranged_icon->section = { 859, 651, 104, 81 };
	ranged_icon->tex_id = panel_tex_ID;

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