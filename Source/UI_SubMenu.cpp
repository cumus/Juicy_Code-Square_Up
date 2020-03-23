#include "UI_SubMenu.h"

UI_SubMenu::UI_SubMenu(const RectF rect)
	: UI_Element(window, SUB_MENU, rect)
{
}

UI_SubMenu::~UI_SubMenu()
{
}

bool UI_SubMenu::Draw() const
{
	return false;
}

UI_SubMenu* UI_SubMenu::ToUiSubMenu()
{
	return this;
}