#include "Application.h"
#include "Input.h"
#include "Canvas.h"
#include "DialogSystem.h"
#include "Scene.h"
#include "Log.h"

DialogSystem::DialogSystem()
{

}

DialogSystem::~DialogSystem()
{}

bool DialogSystem::Start()
{
	dStep = 0;
	end = false;
	CreateScreenUI();
	UpdateScreenUI();
	return true;
}


bool DialogSystem::Update()
{
	if (!end)
	{	
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			UpdateScreenUI();
		}		
		return true;
	}
	else
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			queen->SetInactive();
			soldier->SetInactive();
			retail->SetInactive();
			//portrait->SetInactive();
			units->SetInactive();
			buildings->SetInactive();
			resources->SetInactive();
			capsule->SetInactive();
			enemies->SetInactive();
			gathedge->SetInactive();
			gold->SetInactive();
			edge->SetInactive();
			gear->SetInactive();
			txt->SetInactive();
			txt2->SetInactive();
			txt3->SetInactive();
			space->SetInactive();
			return false;
		}
		else return true;
	}
}

void DialogSystem::CreateScreenUI()
{
	dialogGo = App->scene->AddGameobjectToCanvas("Dialog");

	/*portrait = new C_Image(dialogGo);
	portrait->target = { 0.165f, 0.63f, 0.37, 0.45f };
	portrait->offset = { 0, 0 };
	portrait->section = { 562, 0, 500, 600 };
	portrait->tex_id = App->tex.Load("textures/hud-sprites.png");*/

	queen = new C_Image(dialogGo);
	queen->target = { 0.175f, 0.6f, 0.2f, 0.2f };
	queen->offset = { 0, 0 };
	queen->section = { 7, 3, 793, 1447 };
	queen->tex_id = App->tex.Load("textures/queen.png");

	soldier = new C_Image(dialogGo);
	soldier->target = { 0.175f, 0.66f, 0.2f, 0.2f };
	soldier->offset = { 0, 0 };
	soldier->section = { 6, 2, 788, 1245 };
	soldier->tex_id = App->tex.Load("textures/soldier.png");

	retail = new C_Image(dialogGo);
	retail->target = { 0.3f, 0.75f, 2.5f, 2.5f };
	retail->offset = { 0, 0 };
	retail->section = { 712, 915, 232, 77 };
	retail->tex_id = App->tex.Load("textures/hud-sprites.png");
	
	units = new C_Image(dialogGo);
	units->target = { 0.5f, 0.7f, 0.6f, 0.6f };
	units->offset = { 0, 0 };
	units->section = { 0, 9, 452, 160 };
	units->tex_id = App->tex.Load("textures/tutomages.png");

	buildings = new C_Image(dialogGo);
	buildings->target = { 0.5f, 0.7f, 0.5f, 0.5f };
	buildings->offset = { 0, 0 };
	buildings->section = { 24, 191, 592, 184 };
	buildings->tex_id = App->tex.Load("textures/tutomages.png");

	resources = new C_Image(dialogGo);
	resources->target = { 0.57f, 0.8f, 0.6f, 0.6f };
	resources->offset = { 0, 0 };
	resources->section = { 556, 36, 301, 145 };
	resources->tex_id = App->tex.Load("textures/tutomages.png");

	capsule = new C_Image(dialogGo);
	capsule->target = { 0.67f, 0.774f, 0.7f, 0.7f };
	capsule->offset = { 0, 0 };
	capsule->section = { 34, 633, 66, 226 };
	capsule->tex_id = App->tex.Load("textures/tutomages.png");

	gold = new C_Image(dialogGo);
	gold->target = { 0.6f, 0.89f, 0.7f, 0.7f };
	gold->offset = { 0, 0 };
	gold->section = { 561, 742, 55, 90 };
	gold->tex_id = App->tex.Load("textures/tutomages.png");

	edge = new C_Image(dialogGo);
	edge->target = { 0.6f, 0.85f, 0.7f, 0.7f };
	edge->offset = { 0, 0 };
	edge->section = { 171, 713, 217, 134 };
	edge->tex_id = App->tex.Load("textures/tutomages.png");

	gear = new C_Image(dialogGo);
	gear->target = { 0.6f, 0.89f, 0.7f, 0.7f };
	gear->offset = { 0, 0 };
	gear->section = {427, 745, 93, 87};
	gear->tex_id = App->tex.Load("textures/tutomages.png");

	enemies = new C_Image(dialogGo);
	enemies->target = { 0.45f, 0.87f, 0.7f, 0.7f };
	enemies->offset = { 0, 0 };
	enemies->section = { 109, 435, 239, 145 };
	enemies->tex_id = App->tex.Load("textures/tutomages.png");

	gathedge = new C_Image(dialogGo);
	gathedge->target = { 0.43f, 0.87f, 0.6f, 0.6f };
	gathedge->offset = { 0, 0 };
	gathedge->section = { 426, 419, 335, 168 };
	gathedge->tex_id = App->tex.Load("textures/tutomages.png");

	txt = new C_Text(dialogGo, " ");//Text line
	txt->target = { 0.31f, 0.80f, 2.0f , 2.0f };
	txt2 = new C_Text(dialogGo, " ");//Text line
	txt2->target = { 0.31f, 0.84f, 2.0f , 2.0f };
	txt3 = new C_Text(dialogGo, " ");//Text line
	txt3->target = { 0.31f, 0.88f, 2.0f , 2.0f };
	space = new C_Text(dialogGo, " ");//Text line
	space->target = { 0.31f, 0.95f, 1.0f , 1.0f };
}

void DialogSystem::UpdateScreenUI(Option answer)
{
	switch (dStep)
	{
	case 0:
		queen->SetInactive();
		units->SetInactive();
		buildings->SetInactive();
		resources->SetInactive();
		capsule->SetInactive();
		enemies->SetInactive();
		gathedge->SetInactive();
		gold->SetInactive();
		edge->SetInactive();
		gear->SetInactive();
		txt2->text->SetText("    SOLDIERS, YOUR MISSION IS IMPORTANT.");
		space->text->SetText("     [PRESS SPACE]");
		dStep += 1;
		break;
	case 1:
		txt->text->SetText(" WE NEED YOU TO DESTROY THE PORTALS FROM");
		txt2->text->SetText(" WHICH THE KILLIAN INVADERS ARE COMING!");
		dStep += 1;
		break;
	case 2:	
		queen->SetActive();
		soldier->SetInactive();
		txt->text->SetText("    We know this is almost a suicide mission,");
		txt2->text->SetText("    sending you alone to the enemy backline.");
		dStep += 1;
		break;
	case 3:
		txt->SetInactive();
		txt2->text->SetText("    But all of our tools are at your disposal.");
		dStep += 1;
		break;
	case 4:
		buildings->SetActive();
		txt2->text->SetText("    Blueprints for you to build your base...");
		dStep += 1;
		break;
	case 5:
		buildings->SetInactive();
		queen->SetInactive();
		soldier->SetActive();
		units->SetActive();
		txt->SetInactive();
		txt2->text->SetText("    AND UNITS TO WAGE WAR!!");
		dStep += 1;
		break;
	case 6:
		soldier->SetInactive();
		queen->SetActive();
		units->SetInactive();
		txt2->text->SetText("    Yeah whatever...");
		dStep += 1;
		break;
	case 7:
		resources->SetActive();
		txt2->text->SetText("    Use resources carefully!");
		dStep += 1;
		break;
	case 8:
		resources->SetInactive();
		edge->SetActive();
		txt->SetActive();
		txt->text->SetText(" Edge can be used to build units and buildings,");
		txt2->text->SetText(" and can be harvested from within the map.");
		dStep += 1;
		break;
	case 9:
		edge->SetInactive();
		gear->SetActive();
		txt->text->SetText(" Gears can be obtained by killing enemies,");
		txt2->text->SetText(" and can be used to upgrade both buildings");
		txt3->text->SetText(" and units.");
		dStep += 1;
		break;
	case 10:
		gear->SetInactive();
		gold->SetActive();
		txt3->SetInactive();
		txt->text->SetText(" Enemies might drop gold somethimes, which");
		txt2->text->SetText(" you can use to purchase support capsules");
		txt3->text->SetText(" from yout base center.");
		dStep += 1;
		break;
	case 11:
		capsule->SetActive();
		gold->SetInactive();
		txt->text->SetText(" Some capsules might still remain from ");
		txt2->text->SetText(" past operations, they can contain edge ");
		txt3->text->SetText(" or allied units previously sent here.");
		dStep += 1;
		break;
	case 12:
		queen->SetInactive();
		soldier->SetActive();
		capsule->SetInactive();
		txt3->SetInactive();
		txt->text->SetText(" AND BE CAREFUL, WE'VE DETECTED SOME ");
		txt2->text->SetText(" SEISMIC ACTIVITY IN YOUR AREA,");
		txt3->text->SetText(" PREPARE FOR EARTHQUAKES!! ");
		dStep += 1;
		break;
	case 13:
		enemies->SetActive();
		txt->text->SetText(" DEFEND YOUR BASE CENTER AND");
		txt2->text->SetText(" DESTROY THE ENEMY PORTALS TO SUCCEED.");
		txt3->SetInactive();
		dStep += 1;
		break;
	case 14:
		queen->SetActive();
		soldier->SetInactive();
		gathedge->SetActive();
		enemies->SetInactive();
		txt->text->SetText(" Begin by collecting enough resources to ");
		txt2->text->SetText(" build a good foundation with your gatherer.");
		dStep += 1;
		break;
	case 15:
		txt3->SetActive();
		txt->text->SetText(" Select your Gatherer and find an Edge vein,");
		txt2->text->SetText(" then click on it with the gatherer selected");
		txt3->text->SetText(" to get edge.");
		end = true;
		break;
	}
}


bool DialogSystem::CleanUp()
{	
	dialogGo->Destroy();
	return true;
}