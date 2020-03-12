#include "Editor.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "EditorWindows.h"

Editor::Editor() : Module("editor")
{}

Editor::~Editor()
{}

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

bool Editor::Update()
{
	if (!hide_windows)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		SDL_Rect cam = App->render->GetCameraRect();
		float mouse_x = float(x) / float(cam.w);
		float mouse_y = float(y) / float(cam.h);

		KeyState mouse_left_button = App->input->GetMouseButtonDown(0);
		bool sizing = (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT);

		mouse_over_windows = 0u;

		for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
			if ((*it)->Update(mouse_x, mouse_y, mouse_left_button, sizing))
				mouse_over_windows++;

		// Select Gameobject
		if (mouse_left_button == KEY_DOWN && !sizing && mouse_over_windows == 0u)
			selection = App->scene->RaycastSelect();
	}

	return true;
}

bool Editor::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		hide_windows = !hide_windows;

	if (!hide_windows)
	{
		SDL_Rect cam = App->render->GetCameraRect();
		bool draw_border = (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT);

		for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
			(*it)->Draw(float(cam.w), float(cam.h), draw_border);
	}

	return true;
}

bool Editor::MouseOnWindow() const
{
	return mouse_over_windows > 0u;
}

void Editor::AddWindow(EditorWindow* window)
{
	windows.push_back(window);
}
