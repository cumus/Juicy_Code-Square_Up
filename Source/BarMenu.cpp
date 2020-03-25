#include "BarMenu.h"
#include "Application.h"
#include "UI_TextButton.h"

#include <Windows.h>

BarMenu::BarMenu(const RectF rect) : EditorWindow(rect)
{}

BarMenu::~BarMenu()
{}

bool BarMenu::Init()
{
	elements.push_back(new UI_TextButton(this, { 0.00f, 0.0f, 0.04f, 1.0f }, "Archive")); //0
	elements.push_back(new UI_TextButton(this, { 0.05f, 0.0f, 0.03f, 1.0f }, "Team")); //1
	elements.push_back(new UI_TextButton(this, { 0.09f, 0.0f, 0.03f, 1.0f }, "Exit")); //2
	elements.push_back(new UI_TextButton(this, { 0.13f, 0.0f, 0.03f, 1.0f }, "Save")); //3
	elements.push_back(new UI_TextButton(this, { 0.17f, 0.0f, 0.03f, 1.0f }, "Load")); //4
	elements.push_back(new UI_TextButton(this, { 0.21f, 0.0f, 0.10f, 1.0f }, "GameObject")); //5
	elements.push_back(new UI_TextButton(this, { 0.32f, 0.0f, 0.03f, 1.0f }, "Web")); //6
	elements.push_back(new UI_TextButton(this, { 0.36f, 0.0f, 0.03f, 1.0f }, "Wiki")); //7
	elements.push_back(new UI_TextButton(this, { 0.40f, 0.0f, 0.07f, 1.0f }, "Release")); //8
	elements.push_back(new UI_TextButton(this, { 0.48f, 0.0f, 0.09f, 1.0f }, "Repository")); //9

	return !elements.empty();
}


void BarMenu::RecieveEvent(const Event& e)
{
	int id = e.data1.AsInt();
	if (id >= 0)
	{
		switch (e.type)
		{
		case HOVER_IN:
		{
			elements[id]->ToUiTextButton()->color = { 255, 255, 255, 255 };
			break;
		}
		case HOVER_OUT:
		{
			elements[id]->ToUiTextButton()->color = { 150, 150, 150, 0 };
			break;
		}
		case MOUSE_DOWN:
		{
			elements[id]->ToUiTextButton()->color = { 150, 150, 150, 150 };
			break;
		}
		case MOUSE_REPEAT:
		{
			elements[id]->ToUiTextButton()->color = { 150, 150, 150, 150 };
			break;
		}
		case MOUSE_UP:
		{
			elements[id]->ToUiTextButton()->color = { 50, 50, 50, 255 };

			switch (Content(id))
			{
			case BarMenu::Archive:
				break;
			case BarMenu::Team:
				break;
			case BarMenu::Exit:
				Event::Push(REQUEST_QUIT, App);
				break;
			case BarMenu::Save:
				Event::Push(REQUEST_SAVE, App);
				break;
			case BarMenu::Load:
				Event::Push(REQUEST_LOAD, App);
				break;
			case BarMenu::GameObject:
				break;
			case BarMenu::Web:
				break;
			case BarMenu::Wiki:
				ShellExecute(0, 0, "https://github.com/PolGannau/Juicy-Code-Games_Project-2/wiki", 0, 0, SW_SHOW);
				break;
			case BarMenu::Release:
				ShellExecute(0, 0, "https://github.com/PolGannau/Juicy-Code-Games_Project-2/releases", 0, 0, SW_SHOW);
				break;
			case BarMenu::Repository:
				ShellExecute(0, 0, "https://polgannau.github.io/Juicy-Code-Games_Project-2/", 0, 0, SW_SHOW);
				break;
			default:
				break;
			}
			break;
		}
		}
	}
}

