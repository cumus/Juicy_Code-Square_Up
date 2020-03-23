#ifndef __UI_SUBMENU_H__
#define __UI_SUBMENU_H__

#include "UI_Elements.h"

class UI_SubMenu :public UI_Element
{
public:

	UI_SubMenu(const RectF rect);
	~UI_SubMenu();

	bool Draw() const override;

	UI_SubMenu* ToUiSubMenu() override;
};
#endif // !__UI_SUBMENU_H__
