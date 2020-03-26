#ifndef __BAR_MENU_WINDOW_H__
#define __BAR_MENU_WINDOW_H__

#include "EditorWindow.h"

class BarMenu : public EditorWindow
{
public:

	BarMenu(const RectF rect);
	~BarMenu();

	bool Init() override;
	void RecieveEvent(const Event& e) override;

public:

	enum Content
	{
		Menu,
		Team,
	};
};

#endif // __BAR_MENU_WINDOW_H__