#ifndef __UI_SUBMENU_H__
#define __UI_SUBMENU_H__

#include "UI_Elements.h"

class UI_SubMenu :public UI_Element
{
public:

	UI_SubMenu(const RectF rect);
	~UI_SubMenu();

	bool Draw() const override;
	 
	void RecieveEvent(const Event& e) override;

	UI_SubMenu* ToUiSubMenu() override;

private:
	enum Options
	{
		Save,
		Load,
		GameObject,
		Exit,
		Repo,
		Wiki,
		Web,
		Release
	};

	std::vector<UI_Element*> elements;
};
#endif // !__UI_SUBMENU_H__