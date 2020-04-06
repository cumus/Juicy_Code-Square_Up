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

	for (std::vector<EditorWindow*>::iterator it = windows.begin(); it != windows.end() && ret; ++it)
		ret = (*it)->Init();

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

	// Select Gameobject
	if (mouse_left_button == KEY_DOWN && mouse_over_windows == 0u && !sizing)
	{
		SetSelection(App->scene->MouseClickSelect(x, y));


		/*Gameobject* prev = selection;

		if (selection != nullptr)
		{
			if (prev != nullptr)
			{
				if (prev != selection)
				{
					Event::Push(ON_UNSELECT, prev);
					Event::Push(ON_SELECT, selection);
				}
			}
			else
				Event::Push(ON_SELECT, selection);
		}
		else if (prev != nullptr)
			Event::Push(ON_UNSELECT, prev);*/

		//selectedUnits.push_back(App->scene->MouseClickSelect(x, y));

		/*if (selectedUnits.empty() == false)
		{
			for (std::vector<Gameobject*>::const_iterator it = selectedUnits.begin(); it != selectedUnits.end(); ++it)
			{
				if((*it) != nullptr) Event::Push(UNSELECTED, *it);
			}
			LOG("Previous units diselected");
			selectedUnits.clear();
		}			
		selectedUnits.push_back(App->scene->MouseClickSelect(x, y));
		LOG("Saved selection");
		Event::Push(SELECTED, selectedUnits[0]);*/	
	}

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
	{
		(*it)->CleanUp();
		DEL(*it);
	}

	windows.clear();

	return true;
}

bool Editor::MouseOnWindow() const
{
	return mouse_over_windows > 0u;
}

void Editor::SetSelection(Gameobject* go, bool call_unselect)
{
	if (go != nullptr)
	{
		if (selection != nullptr)
		{
			if (selection != go)
			{
				if (call_unselect)
					Event::Push(ON_UNSELECT, selection);

				Event::Push(ON_SELECT, go);
			}
		}
		else
			Event::Push(ON_SELECT, go);
	}
	else if (selection != nullptr && call_unselect)
		Event::Push(ON_UNSELECT, selection);

	selection = go;
}
