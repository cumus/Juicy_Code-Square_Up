#include "BarMenu.h"
#include "Application.h"
#include "UI_TextButton.h"

BarMenu::BarMenu(const RectF rect) : EditorWindow(rect)
{}

BarMenu::~BarMenu()
{}

bool BarMenu::Init()
{
	elements.push_back(new UI_TextButton(this, { 0.00f, 0.0f, 0.03f, 1.0f }, "Menu")); //0
	elements.push_back(new UI_TextButton(this, { 0.04f, 0.0f, 0.03f, 1.0f }, "Team")); //1
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
			case BarMenu::Menu:
				break;
			case BarMenu::Team:
				break;
			}
			break;
		}
		break;
		}
	}
}