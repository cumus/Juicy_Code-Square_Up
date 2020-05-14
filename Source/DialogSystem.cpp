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
	question = false;
	dStep = 0;
	end = false;
	CreateScreenUI();
	UpdateScreenUI();
	return true;
}


bool DialogSystem::Update()
{
	bool ret = true;
	if (!end)
	{
		if (question)
		{
			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				UpdateScreenUI(OPTION_A);
			}
			else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			{
				UpdateScreenUI(OPTION_B);
			}
			else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			{
				UpdateScreenUI(OPTION_C);
			}
		}
		else
		{
			if (App->input->GetKey(SDL_SCANCODE_SPACE))
			{
				UpdateScreenUI();
			}
		}
	}

	if (end)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE))
		{
			retail->SetInactive();
			txt->SetInactive();
			txtOptionA->SetInactive();
			txtOptionB->SetInactive();
			txtOptionC->SetInactive();
		}
	}
	return ret;
}

void DialogSystem::CreateScreenUI()
{
	dialogGo = App->scene->AddGameobjectToCanvas("Dialog");

	retail = new C_Image(dialogGo);
	retail->target = { 1.f, 1.f, 1.f, 1.f };
	retail->offset = { 50.0f, 50.0f };
	retail->section = { 40, 450, 945, 266 };
	retail->tex_id = App->tex.Load("Assets/textures/fullscreen.png");


	txt = new C_Text(dialogGo, "");//Text line
	txt->target = { 0.047f, 0.628f, 0.9f , 0.9f };

	txtOptionA = new C_Text(dialogGo, "");//Option A
	txtOptionA->target = { 0.047f, 0.628f, 0.9f , 0.9f };

	txtOptionB = new C_Text(dialogGo, "");//Option B
	txtOptionB->target = { 0.047f, 0.628f, 0.9f , 0.9f };

	txtOptionC = new C_Text(dialogGo, "");//Option C
	txtOptionC->target = { 0.047f, 0.628f, 0.9f , 0.9f };
}

void DialogSystem::UpdateScreenUI(Option answer)
{
	switch (dStep)
	{
	case 0:
		txt->text->SetText("Press space to see next text.");
		dStep += 1;
		break;
	case 1:
		txt->text->SetText("Now choose one option:");
		txtOptionA->text->SetText("1-Option A.");
		txtOptionB->text->SetText("2-Option B.");
		txtOptionC->text->SetText("3-Option C.");
		question = true;
		dStep += 1;
		break;
	case 2:
		txtOptionA->text->SetText("");
		txtOptionB->text->SetText("");
		txtOptionC->text->SetText("");
		switch (answer)
		{
		case OPTION_A:
			txt->text->SetText("This is option A.");
			question = false;
			dStep += 1;
			break;
		case OPTION_B:
			txt->text->SetText("This is option B.");
			question = false;
			dStep += 1;
			break;
		case OPTION_C:
			txt->text->SetText("This is option C.");
			question = false;
			dStep += 1;
			break;
		}
		break;
	case 3:
		txt->text->SetText("Want to finish?");
		txtOptionA->text->SetText("1- Yes.");
		txtOptionB->text->SetText("2- No.");
		question = true;
		dStep += 1;
		break;
	case 4:
		txtOptionA->text->SetText("");
		txtOptionB->text->SetText("");
		switch (answer)
		{
		case OPTION_A:
			txt->text->SetText("Dialog end.");
			question = false;
			end = true;
			break;
		case OPTION_B:
			dStep = CHOOSE_OPTION;
			question = false;
			UpdateScreenUI();
			break;
		}
		break;
	}
}


bool DialogSystem::CleanUp()
{
	return true;
}