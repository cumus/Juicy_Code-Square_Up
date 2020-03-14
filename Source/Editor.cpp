#include "Editor.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "EditorWindows.h"
#include "Defs.h"

#include "SDL/include/SDL_scancode.h"

Editor::Editor() : Module("editor")
{}

Editor::~Editor()
{}

bool Editor::Awake(pugi::xml_node&)
{
	// Load window positions from config
	AddWindow(bar_menu = new BarMenu({ 0.0f, 0.0f, 1.0f, 0.02f }));
	AddWindow(play_pause = new PlayPauseWindow({ 0.3f, 0.05f, 0.4f, 0.05f }));

	AddWindow(hierarchy = new HeriarchyWindow({ 0.0f, 0.05f, 0.2f, 0.5f }));
	AddWindow(properties = new PropertiesWindow({ 0.8f, 0.05f, 0.2f, 0.5f }));

	AddWindow(console = new ConsoleWindow({ 0.0f, 0.7f, 0.4f, 0.3f }));
	AddWindow(config = new ConfigWindow({ 0.7f, 0.6f, 0.3f, 0.4f }));

	return true;
}

bool Editor::Update()
{
	mouse_over_windows = 0u;
	KeyState mouse_left_button = App->input->GetMouseButtonDown(0);
	bool sizing = (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT);

	if (!hide_windows)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		RectF cam = App->render->GetCameraRectF();
		float mouse_x = float(x) / cam.w;
		float mouse_y = float(y) / cam.h;

		if (editing_window < 0)
		{
			int count = 0;
			for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
			{
				if ((*it)->CheckIfEditing(mouse_x, mouse_y, mouse_left_button))
				{
					editing_window = count;
					break;
				}

				++count;
			}
		}
		else 
		{
			int count = 0;
			for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
			{
				if (count == editing_window)
				{
					if (!(*it)->CheckIfEditing(mouse_x, mouse_y, mouse_left_button))
						editing_window = -1;
				}
				else
					(*it)->CheckIfEditing(mouse_x, mouse_y, KEY_IDLE);

				++count;
			}
		}

		for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
			if ((*it)->Update(mouse_x, mouse_y, mouse_left_button))
				mouse_over_windows++;
	}

	// Select Gameobject
	if (mouse_left_button == KEY_DOWN && !sizing && mouse_over_windows == 0u)
		selection = App->scene->RaycastSelect();

	return true;
}

bool Editor::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		hide_windows = !hide_windows;

	if (!hide_windows)
	{
		bool draw_border = (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT);

		for (std::vector<EditorWindow*>::const_reverse_iterator it = windows.rbegin(); it != windows.rend(); ++it)
			(*it)->Draw(draw_border);
	}

	return true;
}

bool Editor::CleanUp()
{
	for (std::vector<EditorWindow*>::reverse_iterator it = windows.rbegin(); it != windows.rend(); ++it)
		DEL(*it);

	windows.clear();

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
