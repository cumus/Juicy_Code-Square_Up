#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "Module.h"
#include <vector>

class EditorWindow;
class BarMenu;
class PlayPauseWindow;
class HierarchyWindow;
class PropertiesWindow;
class ConsoleWindow;
class ConfigWindow;
class Gameobject;

class Editor : public Module
{
public:

	Editor();
	~Editor();

	bool Init() override;
	bool Start() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	bool MouseOnWindow() const;

public:

	BarMenu* bar_menu = nullptr;
	PlayPauseWindow* play_pause = nullptr;
	HierarchyWindow* hierarchy = nullptr;
	PropertiesWindow* properties = nullptr;
	ConsoleWindow* console = nullptr;
	ConfigWindow* config = nullptr;

	Gameobject* selection = nullptr;

private:

	bool hide_windows = false;
	bool sizing = false;
	int editing_window = -1;
	unsigned int mouse_over_windows = 0u;

	std::vector<EditorWindow*> windows;
};

#endif // __EDITOR_H__