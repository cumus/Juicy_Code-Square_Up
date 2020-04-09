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



Base_Center::Base_Center(Gameobject* go) : Behaviour(go, BASE_CENTER, FULL_LIFE, B_BASE_CENTER)
{
	Transform* t = game_object->GetTransform();

	max_life = 100;
	current_life = max_life;
	buildQueue = 0;
	bc_lvl = 1;
	bc_max_lvl = 5;
	spawnPointX = t->GetLocalPos().x - 1;
	spawnPointY = t->GetLocalPos().y - 1;

	create_bar();
	bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();

	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}
}

Base_Center::~Base_Center()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
	b_map.erase(GetID());
}



void Base_Center::AfterDamageAction()
{
	if (current_life <= 0)
		OnKill();
	else if (current_life >= max_life * 0.5f)
		current_state = FULL_LIFE;
	else
		current_state = HALF_LIFE;
}


void Base_Center::Upgrade()
{
	if (bc_lvl < bc_max_lvl) {

		max_life += 50;
		bc_lvl += 1;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("BC LEVEL: %d", bc_lvl);
		update_upgrades_ui();
		update_health_ui();
	}
}

void Base_Center::CreatePanel()
{
	posY_panel = 0.3f;
	panel_tex_ID = App->tex.Load("textures/buildPanelSample.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobject("Main Base Build Panel", App->scene->hud_canvas_go);

	panel = new C_Image(bar_go);
	panel->target = { 0.06f, pos_y_HUD - 0.003f, 0.5f, 0.5f };
	panel->offset = { -109.0f, -89.0f };
	panel->section = { 0, 0, 119, 119 };
	panel->tex_id = panel_tex_ID;

	gatherer_btn = new C_Button(selectionPanel, Event(BUILD_GATHERER, this->game_object, spawnPointX, spawnPointY));
	gatherer_btn->target = { 0.5f, posY_panel, 1.3f, 1.2f };
	gatherer_btn->offset = { -482.0f, -44.0f };
	gatherer_btn->section = { 121, 38, 38, 38 };
	gatherer_btn->tex_id = panel_tex_ID;

	/*meleeUnit_btn = new C_Button(selectionPanel, Event(BUILD_MELEE, this->game_object, spawnPointX, spawnPointY));
	meleeUnit_btn->target = { 0.5f, posY_panel, 1.3f, 1.2f };
	meleeUnit_btn->offset = { -482.0f, -44.0f };
	meleeUnit_btn->section = { 121, 0, 38, 38 };
	meleeUnit_btn->tex_id = panel_tex_ID;*/

	/*rangedUnit_btn = new C_Button(selectionPanel, Event(BUILD_RANGED, this->game_object, spawnPointX, spawnPointY));
	rangedUnit_btn->target = { 0.5f, posY_panel, 1.3f, 1.2f };
	rangedUnit_btn->offset = { -482.0f, -44.0f };
	rangedUnit_btn->section = { 121, 0, 38, 38 };
	rangedUnit_btn->tex_id = panel_tex_ID;*/

	/*superUnit_btn = new C_Button(selectionPanel, Event(BUILD_SUPER, this->game_object, spawnPointX, spawnPointY));
	superUnit_btn->target = { 0.5f, posY_panel, 1.3f, 1.2f };
	superUnit_btn->offset = { -482.0f, -44.0f };
	superUnit_btn->section = { 162, 38, 38, 38 };
	superUnit_btn->tex_id = panel_tex_ID;*/
}

void Base_Center::UpdatePanel()
{

}

void Base_Center::OnRightClick(float x, float y)
{
	//Upgrade();
	SpawnUnit(x,y);
}

void Base_Center::SpawnUnit(float x,float y) 
{		
	Gameobject* unit_go = App->scene->AddGameobject("Melee Unit - son of root");
	unit_go->GetTransform()->SetLocalPos({x, y, 0.0f });
	new B_Unit(unit_go, UNIT_MELEE, IDLE);
	
}

void Base_Center::create_bar() {

	pos_y_HUD = 0.17;

	bar_text_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE BAR --------------------------------------

	bar_go = App->scene->AddGameobject("Main Base Bar", App->scene->hud_canvas_go);

	bar = new C_Button(bar_go, Event(REQUEST_QUIT, App));
	bar->target = { 0.5f, pos_y_HUD, 1.3f, 1.2f };
	bar->offset = { -482.0f, -44.0f };
	bar->section = { 4, 7, 482, 44 };
	bar->tex_id = bar_text_id;

	//------------------------- BASE PORTRAIT --------------------------------------

	portrait = new C_Image(bar_go);
	portrait->target = { 0.06f, pos_y_HUD - 0.003f, 0.5f, 0.5f };
	portrait->offset = { -109.0f, -89.0f };
	portrait->section = { 126, 295, 109, 95 };
	portrait->tex_id = App->tex.Load("textures/creation_bars_test.png");

	//------------------------- BASE TEXT --------------------------------------

	text = new C_Text(bar_go, "Base");
	text->target = { 0.07f, pos_y_HUD - 0.07f, 1.5f, 1.5f };

	//------------------------- BASE HEALTHBAR --------------------------------------

	healthbar = new C_Image(bar_go);
	healthbar->target = { 0.415f, pos_y_HUD - 0.01f, 1.8f, 0.7f };
	healthbar->offset = { -245.0f, -23.0f };
	healthbar->section = { 56, 192, 245, 23 };
	healthbar->tex_id = bar_text_id;

	//------------------------- BASE HEALTH --------------------------------------

	health = new C_Image(bar_go);
	health->target = { 0.415f, pos_y_HUD - 0.01f, 1.8f, 0.7f };
	health->offset = { -245.0f, -23.0f };
	health->section = { 57, 238, 245, 23 };
	health->tex_id = bar_text_id;
}

void Base_Center::update_health_ui() {

	health->target = { (0.415f) - ((0.415f - 0.07f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.01f, 1.8f * (float(current_life) / float(max_life)), 0.7f };

}

void Base_Center::update_upgrades_ui() {

	C_Image* main_base_upgrade = new C_Image(bar_go);
	main_base_upgrade->target = { 0.415f + (bc_lvl - 1) * 0.018f, pos_y_HUD - 0.01f, 0.18f, 0.4f };
	main_base_upgrade->offset = { -79.0f, -93.0f };
	main_base_upgrade->section = { 398, 78, 79, 93 };
	main_base_upgrade->tex_id = bar_text_id;

}