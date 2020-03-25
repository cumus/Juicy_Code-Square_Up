#include "UI_SubMenu.h"
#include "UI_TextButton.h"
#include "Application.h"
#include "Editor.h"
#include "BarMenu.h"

UI_SubMenu::UI_SubMenu(const RectF rect)
	: UI_Element(window, SUB_MENU, rect)
{
}

UI_SubMenu::~UI_SubMenu()
{
}

bool UI_SubMenu::Draw() const
{
	return true;
}

UI_SubMenu* UI_SubMenu::ToUiSubMenu()
{
	return this;
}

void UI_SubMenu::ContentGetter(const int id)
{
	/*switch (Content(id))
	{
	case UI_SubMenu::Archive:
	{
		elements.push_back(new UI_TextButton(this, { 0.000f, 0.0f, 0.03f, 1.0f }, "Exit")); //0
		elements.push_back(new UI_TextButton(this, { 0.040f, 0.0f, 0.03f, 1.0f }, "Save")); //1
		elements.push_back(new UI_TextButton(this, { 0.080f, 0.0f, 0.03f, 1.0f }, "Load")); //2
		elements.push_back(new UI_TextButton(this, { 0.120f, 0.0f, 0.10f, 1.0f }, "GameObject")); //3
		break;
	}
	case UI_SubMenu::Team:
	{
		elements.push_back(new UI_TextButton(this, { 0.230f, 0.0f, 0.03f, 1.0f }, "Web")); //4
		elements.push_back(new UI_TextButton(this, { 0.270f, 0.0f, 0.03f, 1.0f }, "Wiki")); //5
		elements.push_back(new UI_TextButton(this, { 0.310f, 0.0f, 0.07f, 1.0f }, "Release")); //6
		elements.push_back(new UI_TextButton(this, { 0.390f, 0.0f, 0.09f, 1.0f }, "Repository")); //7
		break;
	}
	break;
	}*/
}
