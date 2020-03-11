#include "Editor.h"
#include "EditorWindows.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"

Editor::Editor() : Module("editor")
{
}

Editor::~Editor()
{
}

bool Editor::Awake(pugi::xml_node&)
{
	// Load window positions from config
	AddWindow(bar_menu = new BarMenu(0.0f, 0.0f, 1.0f, 0.02f));
	AddWindow(play_pause = new PlayPauseWindow(0.3f, 0.05f, 0.4f, 0.05f));

	AddWindow(hierarchy = new HeriarchyWindow(0.0f, 0.05f, 0.2f, 0.5f));
	AddWindow(properties = new PropertiesWindow(0.8f, 0.05f, 0.2f, 0.5f));

	AddWindow(console = new ConsoleWindow(0.0f, 0.7f, 0.4f, 0.3f));
	AddWindow(config = new ConfigWindow(0.7f, 0.6f, 0.3f, 0.4f));

	return true;
}

bool Editor::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		draw = !draw;

	if (draw)
	{
		SDL_Rect cam = App->render->GetCameraRect();

		for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
			(*it)->Draw(float(cam.w), float(cam.h));
	}

	return true;
}

void Editor::AddWindow(EditorWindow* window)
{
	windows.push_back(window);
}
