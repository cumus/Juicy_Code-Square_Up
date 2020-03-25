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

private:

	enum Content
	{
		Archive,
		Team,
		Exit,
		Save,
		Load,
		GameObject,
		Web,
		Wiki,
		Release,
		Repository
	};
};

#endif // __BAR_MENU_WINDOW_H__