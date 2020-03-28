#include "UI_SubMenu.h"
#include "UI_TextButton.h"
#include "Application.h"
#include "BarMenu.h"
#include "Editor.h"

#include <Windows.h>

UI_SubMenu::UI_SubMenu(const RectF rect) 
	: UI_Element(window, SUB_MENU, rect)
{
}

UI_SubMenu::~UI_SubMenu()
{
}

void UI_SubMenu::Draw() const
{
}

void UI_SubMenu::RecieveEvent(const Event& e)
{
	int id = e.data1.AsInt();
	if (id >= 0)
	{
		switch (BarMenu::Content(id))
		{
		case BarMenu::Menu:
		{
			switch (e.type)
			{
			case MOUSE_UP:
			{
				switch (Options(id))
				{
				case UI_SubMenu::Save:
					Event::Push(REQUEST_SAVE, App);
					break;
				case UI_SubMenu::Load:
					Event::Push(REQUEST_LOAD, App);
					break;
				case UI_SubMenu::GameObject:
					break;
				case UI_SubMenu::Exit:
					Event::Push(REQUEST_QUIT, App);
					break;
				}
				break;
			}
			break;
			}
			break;
		}
		case BarMenu::Team:
		{
			switch (e.type)
			{
			case MOUSE_UP:
			{
				switch (Options(id))
				{
				case UI_SubMenu::Repo:
					ShellExecute(0, 0, "https://github.com/PolGannau/Juicy-Code-Games_Project-2/", 0, 0, SW_SHOW);
					break;
				case UI_SubMenu::Wiki:
					ShellExecute(0, 0, "https://github.com/PolGannau/Juicy-Code-Games_Project-2/wiki", 0, 0, SW_SHOW);
					break;
				case UI_SubMenu::Web:
					ShellExecute(0, 0, "https://polgannau.github.io/Juicy-Code-Games_Project-2/", 0, 0, SW_SHOW);
					break;
				case UI_SubMenu::Release:
					ShellExecute(0, 0, "https://github.com/PolGannau/Juicy-Code-Games_Project-2/releases", 0, 0, SW_SHOW);
					break;
				}
				break;
			}
			break;
			}
			break;
		}
		break;
		}
	}
}

UI_SubMenu* UI_SubMenu::ToUiSubMenu()
{
	return this;
}