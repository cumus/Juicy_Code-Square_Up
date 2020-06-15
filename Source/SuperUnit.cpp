#include "SuperUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"
#include "ParticleSystem.h"

SuperUnit::SuperUnit(Gameobject* go) : B_Unit(go, UNIT_SUPER, IDLE, B_RANGED)
{
	//Stats
	max_life = 100;
	current_life = max_life;
	atkTime = 2.0;
	speed = 3;
	damage = 15;
	attack_range = 16.0f;
	vision_range = 23.0f;
	providesVisibility = true;

	CreatePanel();
	selectionPanel->SetInactive();
	unitInfo->SetInactive();

	//SFX
	deathFX = SUPER_DIE_FX;
	attackFX = SUPER_ATK_FX;
	SetColliders();
}

SuperUnit::~SuperUnit() {}

void SuperUnit::UnitAttackType()
{
	attackPos = atkObj->GetPos();
	App->particleSys.AddParticle(pos, attackPos, 8.0f, ORANGE_PARTICLE);
	audio->Play(attackFX);
}

void SuperUnit::CreatePanel()
{
	panel_tex_ID = App->tex.Load("textures/hud-sprites.png");

	//------------------------- BASE PANEL --------------------------------------
	selectionPanel = App->scene->AddGameobjectToCanvas("Super info Panel");

	super_icon = new C_Image(selectionPanel);
	super_icon->target = { 0.0f, 0.832f, 1.5f, 1.5f };
	super_icon->offset = { 0.0f, 0.0f };
	super_icon->section = { 964, 651, 104, 81 };
	super_icon->tex_id = panel_tex_ID;

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