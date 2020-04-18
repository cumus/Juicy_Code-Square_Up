#include "Editor.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "EditorWindow.h"
#include "BarMenu.h"
#include "PlayPauseWindow.h"
#include "HierarchyWindow.h"
#include "PropertiesWindow.h"
#include "ConsoleWindow.h"
#include "ConfigWindow.h"
#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL_scancode.h"

Editor::Editor() : Module("editor")
{}

Editor::~Editor()
{
	if (!windows.empty())
		CleanUp();
}

bool Editor::Init()
{
	//TODO: Load/Save window positions from config

	//windows.push_back(bar_menu = new BarMenu({ 0.0f, 0.0f, 1.0f, 0.02f }));
	windows.push_back(play_pause = new PlayPauseWindow({ 0.4f, 0.05f, 0.2f, 0.05f }));

	windows.push_back(hierarchy = new HierarchyWindow({ 0.02f, 0.05f, 0.2f, 0.9f }));
	//windows.push_back(properties = new PropertiesWindow({ 0.8f, 0.05f, 0.2f, 0.4f }));

	//windows.push_back(console = new ConsoleWindow({ 0.0f, 0.7f, 0.4f, 0.3f }));
	//windows.push_back(config = new ConfigWindow({ 0.7f, 0.6f, 0.3f, 0.4f }));


	return !windows.empty();
}

bool Editor::Start()
{
	bool ret = true;

	int window_count = 0;
	for (std::vector<EditorWindow*>::iterator it = windows.begin(); it != windows.end() && ret; ++it, ++window_count)
		if (!(ret = (*it)->Init()))
			LOG("Error loading window %d", window_count);

	return ret;
}

bool Editor::Update()
{
	mouse_over_windows = 0u;
	int x, y;
	App->input->GetMousePosition(x, y);
	KeyState mouse_left_button = App->input->GetMouseButtonDown(0);
	sizing = App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT;

	if (!hide_windows)
	{
		RectF cam = App->render->GetCameraRectF();
		float mouse_x = float(x) / cam.w;
		float mouse_y = float(y) / cam.h;

		if (sizing)
		{
			if (editing_window < 0)
			{
				int count = 0;
				for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
				{
					if (((*it)->Update(mouse_x, mouse_y, mouse_left_button, sizing).dragging))
					{
						editing_window = count;
						break;
					}

					++count;
				}
			}
			else if (!windows[editing_window]->Update(mouse_x, mouse_y, mouse_left_button, sizing).dragging)
				editing_window = -1;
		}
		else
		{
			editing_window = -1;

			for (std::vector<EditorWindow*>::const_iterator it = windows.begin(); it != windows.end(); ++it)
				if ((*it)->Update(mouse_x, mouse_y, mouse_left_button, sizing).mouse_inside)
					mouse_over_windows++;
		}
	}

	return true;
}

bool Editor::CleanUp()
{
	for (std::vector<EditorWindow*>::reverse_iterator it = windows.rbegin(); it != windows.rend(); ++it)
	{
		(*it)->CleanUp();
		DEL(*it);
	}

	windows.clear();

	return true;
}

bool Editor::MouseOnWindow() const
{
	return !hide_windows && (mouse_over_windows > 0u || sizing);
}

bool Editor::Draw()
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