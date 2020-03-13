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
class Gameobject;

class Editor : public Module
{
public:

	Editor();
	~Editor();

	bool Awake(pugi::xml_node&) override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	bool MouseOnWindow() const;

public:

	BarMenu* bar_menu = nullptr;
	PlayPauseWindow* play_pause = nullptr;
	HeriarchyWindow* hierarchy = nullptr;
	PropertiesWindow* properties = nullptr;
	ConsoleWindow* console = nullptr;
	ConfigWindow* config = nullptr;

	Gameobject* selection = nullptr;

private:

	void AddWindow(EditorWindow* window);

private:

	bool hide_windows = false;
	unsigned int mouse_over_windows = 0u;

	std::vector<EditorWindow*> windows;
};

#endif // __EDITOR_H__