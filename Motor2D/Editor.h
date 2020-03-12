#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "Module.h"
#include <vector>

class EditorWindow;
class BarMenu;
class PlayPauseWindow;
class HeriarchyWindow;
class PropertiesWindow;
class ConsoleWindow;
class ConfigWindow;

class Editor : public Module
{
public:

	Editor();
	~Editor();

	bool Awake(pugi::xml_node&) override;
	bool Update() override;
	bool PostUpdate() override;

	bool MouseOnWindow() const;

public:

	BarMenu* bar_menu = nullptr;
	PlayPauseWindow* play_pause = nullptr;
	HeriarchyWindow* hierarchy = nullptr;
	PropertiesWindow* properties = nullptr;
	ConsoleWindow* console = nullptr;
	ConfigWindow* config = nullptr;

private:

	void AddWindow(EditorWindow* window);

private:

	std::vector<EditorWindow*> windows;
	bool hide_windows = false;
	unsigned int mouse_over_windows = 0u;
};

#endif // __EDITOR_H__