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
			retail->SetInactive();
			txt->SetInactive();
			soldier->SetInactive();
			queen->SetInactive();
			return false;
		}
		else return true;
	}
}

void DialogSystem::CreateScreenUI()
{
	dialogGo = App->scene->AddGameobjectToCanvas("Dialog");

	queen = new C_Image(dialogGo);
	queen->target = { 0.6f, 0.2f, 0.4f, 0.4f };
	queen->offset = { 0, 0 };
	queen->section = { 7, 3, 793, 1447 };
	queen->tex_id = App->tex.Load("Assets/textures/queen.png");

	soldier = new C_Image(dialogGo);
	soldier->target = { 0.1f, 0.32f, 0.4f, 0.4f };
	soldier->offset = { 0, 0 };
	soldier->section = { 6, 2, 788, 1245 };
	soldier->tex_id = App->tex.Load("Assets/textures/soldier.png");

	retail = new C_Image(dialogGo);
	retail->target = { 0.3f, 0.75f, 2.0f, 2.0f };
	retail->offset = { 0, 0 };
	retail->section = { 712, 915, 232, 77 };
	retail->tex_id = App->tex.Load("Assets/textures/hud-sprites.png");

	txt = new C_Text(dialogGo, "");//Text line
	txt->target = { 0.36f, 0.84f, 1.5f , 1.5f };
}

void DialogSystem::UpdateScreenUI(Option answer)
{
	switch (dStep)
	{
	case 0:
		txt->text->SetText("QUEEN!! WE ARE BEING INVADED!!");
		dStep += 1;
		break;
	case 1:
		txt->text->SetText("Now choose one option:");
		dStep += 1;
		break;
	case 2:	
		txt->text->SetText("This is option C.");
		dStep += 1;
		break;
	case 3:
		txt->text->SetText("Want to finish?");
		dStep += 1;
		break;
	case 4:
		txt->text->SetText("Dialog end.");
		end = true;
		break;
	}
}


bool DialogSystem::CleanUp()
{
	return true;
}