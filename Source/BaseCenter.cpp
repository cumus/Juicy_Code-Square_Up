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
	spawnPointX = t->GetLocalPos().x;
	spawnPointY = t->GetLocalPos().y + 1;

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
		OnKill(type);
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
	posY_panel = 0.8f;
	panel_tex_ID = App->tex.Load("textures/buildPanelSample.png");

	//------------------------- BASE PANEL --------------------------------------

	selectionPanel = App->scene->AddGameobject("Main Base Build Panel", App->scene->hud_canvas_go);

	panel = new C_Image(selectionPanel);
	panel->target = { 0.9f, posY_panel, 1.0f, 1.0f };
	panel->offset = { 0.0f, 0.0f };
	panel->section = { 0, 0, 119, 119 };
	panel->tex_id = panel_tex_ID;

	gatherer_btn = new C_Button(selectionPanel, Event(BUILD_GATHERER, this->AsBehaviour(), spawnPointX, spawnPointY));//Top left
	gatherer_btn->target = { 0.912f, posY_panel+0.02f, 1.0f, 1.0f };
	gatherer_btn->offset = { 0.0f, 0.0f };
	gatherer_btn->section = { 121, 38, 38, 38 };
	gatherer_btn->tex_id = panel_tex_ID;

	meleeUnit_btn = new C_Button(selectionPanel, Event(BUILD_MELEE, this->AsBehaviour(), spawnPointX, spawnPointY));//Top right
	meleeUnit_btn->target = { 0.95f, posY_panel+0.02f, 1.0f, 1.0f };
	meleeUnit_btn->offset = { 0.0f,0.0f };
	meleeUnit_btn->section = { 121, 0, 38, 38 };
	meleeUnit_btn->tex_id = panel_tex_ID;

	rangedUnit_btn = new C_Button(selectionPanel, Event(BUILD_RANGED, this->AsBehaviour(), spawnPointX, spawnPointY));//Bottom left
	rangedUnit_btn->target = { 0.912f, posY_panel+0.085f, 1.0f, 1.0f };
	rangedUnit_btn->offset = { 0.0f, 0.0f };
	rangedUnit_btn->section = { 161, 0, 38, 38 };
	rangedUnit_btn->tex_id = panel_tex_ID;

	superUnit_btn = new C_Button(selectionPanel, Event(BUILD_SUPER, this->AsBehaviour(), spawnPointX, spawnPointY));//Bottom right
	superUnit_btn->target = { 0.95f, posY_panel+0.085f, 1.0f, 1.0f };
	superUnit_btn->offset = { 0.0f, 0.0f };
	superUnit_btn->section = { 162, 38, 38, 38 };
	superUnit_btn->tex_id = panel_tex_ID;
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

	bar_text_id = App->tex.Load("textures/Iconos_square_up.png");

	//------------------------- BASE BAR --------------------------------------

	bar_go = App->scene->AddGameobject("Main Base Bar", App->scene->hud_canvas_go);

	bar = new C_Image(bar_go);
	bar->target = { 0.5f, pos_y_HUD, 1.3f, 1.2f };
	bar->offset = { -455.0f, -62.0f };
	bar->section = { 17, 561, 455, 62 };
	bar->tex_id = bar_text_id;

	//------------------------- BASE PORTRAIT --------------------------------------

	portrait = new C_Image(bar_go);
	portrait->target = { 0.08f, pos_y_HUD - 0.013f, 0.28f, 0.28f };
	portrait->offset = { -166.0f, -185.0f };
	portrait->section = { 0, 0, 166, 185 };
	portrait->tex_id = App->tex.Load("textures/base_center.png");

	//------------------------- BASE TEXT --------------------------------------

	text = new C_Text(bar_go, "Base");
	text->target = { 0.09f, pos_y_HUD - 0.085f, 1.5f, 1.5f };

	//------------------------- BASE RED HEALTH --------------------------------------

	red_health = new C_Image(bar_go);
	red_health->target = { 0.42f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	red_health->offset = { -220.0f, -20.0f };
	red_health->section = { 39, 729, 220, 20 };
	red_health->tex_id = bar_text_id;

	//------------------------- BASE HEALTH --------------------------------------

	health = new C_Image(bar_go);
	health->target = { 0.42f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	health->offset = { -220.0f, -20.0f };
	health->section = { 39, 749, 220, 20 };
	health->tex_id = bar_text_id;

	//------------------------- BASE HEALTH BOARDER --------------------------------------

	health_boarder = new C_Image(bar_go);
	health_boarder->target = { 0.42f, pos_y_HUD - 0.02f, 1.92f, 1.0f };
	health_boarder->offset = { -220.0f, -20.0f };
	health_boarder->section = { 39, 707, 220, 20 };
	health_boarder->tex_id = bar_text_id;

	upgrades = new C_Image(bar_go);
	upgrades->target = { 0.47f, pos_y_HUD - 0.02f, 1.3f, 1.3f };
	upgrades->offset = { -33.0f, -33.0f };
	upgrades->section = { 543, 706, 33, 33 };
	upgrades->tex_id = bar_text_id;

}

void Base_Center::update_health_ui() {

	health->target = { (0.42f) - ((0.42f - 0.094f) * (1.0f - float(current_life) / float(max_life))), pos_y_HUD - 0.02f, 1.92f * (float(current_life) / float(max_life)), 1.0f };

}

void Base_Center::update_upgrades_ui() {

	upgrades->section = { 543 + 36 * (bc_lvl - 1), 706, 33, 33 };

}