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

	bool MouseOnEditor() const;
	void ToggleEditorVisibility();

public:

	BarMenu* bar_menu = nullptr;
	PlayPauseWindow* play_pause = nullptr;
	HierarchyWindow* hierarchy = nullptr;
	PropertiesWindow* properties = nullptr;
	ConsoleWindow* console = nullptr;
	ConfigWindow* config = nullptr;

private:

	bool hide_windows = true;
	bool sizing = false;
	int editing_window = -1;
	unsigned int mouse_over_windows = 0u;
	bool has_mouse_focus = false;

	std::vector<EditorWindow*> windows;
};

#endif // __EDITOR_H__