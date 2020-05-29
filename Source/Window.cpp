#include "Window.h"
#include "Application.h"
#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"
#include "optick-1.3.0.0/include/optick.h"

Window::Window() : Module("window")
{
	window = nullptr;
	screen_surface = nullptr;
}

// Destructor
Window::~Window()
{}

void Window::LoadConfig(bool empty_config)
{
	pugi::xml_node config = FileManager::ConfigNode();

	if (empty_config)
	{
		pugi::xml_node window_node = config.append_child(name);

		// Position
		pugi::xml_node pos = window_node.append_child("position");
		pos.append_attribute("x").set_value(rect.x);
		pos.append_attribute("y").set_value(rect.y);

		// Resolution
		pugi::xml_node res = window_node.append_child("resolution");
		res.append_attribute("width").set_value(rect.w);
		res.append_attribute("height").set_value(rect.h);

		// Window flags
		window_node.append_child("fullscreen").append_attribute("value").set_value(fullscreen);
		window_node.append_child("fullscreen_window").append_attribute("value").set_value(fullscreen_desktop);

		window_node.append_child("borderless").append_attribute("value").set_value(borderless);
		window_node.append_child("resizable").append_attribute("value").set_value(resizable);

		window_node.append_child("minimized").append_attribute("value").set_value(minimized);
		window_node.append_child("maximized").append_attribute("value").set_value(maximized);

		window_node.append_child("input_grabbed").append_attribute("value").set_value(input_grabbed);
		window_node.append_child("input_focus").append_attribute("value").set_value(input_focus);
		window_node.append_child("mouse_focus").append_attribute("value").set_value(mouse_focus);

		window_node.append_child("high_dpi").append_attribute("value").set_value(high_dpi);
		window_node.append_child("mouse_capture").append_attribute("value").set_value(mouse_capture);
		window_node.append_child("always_on_top").append_attribute("value").set_value(always_on_top);
		window_node.append_child("skip_taskbar").append_attribute("value").set_value(skip_taskbar);
	}
	else
	{
		pugi::xml_node window_node = config.child(name);

		// Position
		pugi::xml_node pos = window_node.child("position");

		// Resolution
		pugi::xml_node res = window_node.child("resolution");
		rect.w = res.attribute("width").as_int(rect.w);
		rect.h = res.attribute("height").as_int(rect.h);

		// Window flags
		fullscreen = false; // window_node.child("fullscreen").first_attribute().as_bool(fullscreen);
		fullscreen_desktop = false; // window_node.child("fullscreen_window").first_attribute().as_bool(fullscreen_desktop);

		borderless = window_node.child("borderless").first_attribute().as_bool(borderless);
		resizable = window_node.child("resizable").first_attribute().as_bool(resizable);

		minimized = window_node.child("minimized").first_attribute().as_bool(minimized);
		maximized = window_node.child("maximized").first_attribute().as_bool(maximized);

		input_grabbed = window_node.child("input_grabbed").first_attribute().as_bool(input_grabbed);
		input_focus = window_node.child("input_focus").first_attribute().as_bool(input_focus);
		mouse_focus = window_node.child("mouse_focus").first_attribute().as_bool(mouse_focus);

		high_dpi = window_node.child("high_dpi").first_attribute().as_bool(high_dpi);
		mouse_capture = window_node.child("mouse_capture").first_attribute().as_bool(mouse_capture);
		always_on_top = window_node.child("always_on_top").first_attribute().as_bool(always_on_top);
		skip_taskbar = window_node.child("skip_taskbar").first_attribute().as_bool(skip_taskbar);
	}
}

void Window::SaveConfig() const
{
	pugi::xml_node config = FileManager::ConfigNode();
	pugi::xml_node window_node = config.child(name);

	// Position
	pugi::xml_node pos = window_node.child("position");
	pos.attribute("x").set_value(maximized ? -1 : rect.x);
	pos.attribute("y").set_value(maximized ? -1 : rect.y);

	// Resolution
	pugi::xml_node res = window_node.child("resolution");
	res.attribute("width").set_value(1280);
	res.attribute("height").set_value(720);

	// Window flags
	window_node.child("fullscreen").attribute("value").set_value(fullscreen);
	window_node.child("fullscreen_window").attribute("value").set_value(fullscreen_desktop);

	window_node.child("borderless").attribute("value").set_value(borderless);
	window_node.child("resizable").attribute("value").set_value(resizable);

	window_node.child("minimized").attribute("value").set_value(minimized);
	window_node.child("maximized").attribute("value").set_value(maximized);

	window_node.child("input_grabbed").attribute("value").set_value(input_grabbed);
	window_node.child("input_focus").attribute("value").set_value(input_focus);
	window_node.child("mouse_focus").attribute("value").set_value(mouse_focus);

	window_node.child("high_dpi").attribute("value").set_value(high_dpi);
	window_node.child("mouse_capture").attribute("value").set_value(mouse_capture);
	window_node.child("always_on_top").attribute("value").set_value(always_on_top);
	window_node.child("skip_taskbar").attribute("value").set_value(skip_taskbar);
}

// Called before render is available
bool Window::Init()
{
	OPTICK_EVENT();

	bool ret = false;

	if(SDL_Init(SDL_INIT_VIDEO) >= 0)
	{
		LOG("SDL_VIDEO initialized");

		// Setup flags
		Uint32 flags = SDL_WINDOW_SHOWN;
		if (fullscreen) flags |= (fullscreen_desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN);
		if (borderless) flags |= SDL_WINDOW_BORDERLESS;
		if (resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (minimized) flags |= SDL_WINDOW_MINIMIZED;
		if (maximized) flags |= SDL_WINDOW_MAXIMIZED;
		if (input_grabbed) flags |= SDL_WINDOW_INPUT_GRABBED;
		if (has_keyboard_focus = input_focus) flags |= SDL_WINDOW_INPUT_FOCUS;
		if (has_mouse_focus = mouse_focus) flags |= SDL_WINDOW_MOUSE_FOCUS;
		if (high_dpi) flags |= SDL_WINDOW_ALLOW_HIGHDPI;
		if (mouse_capture) flags |= SDL_WINDOW_MOUSE_CAPTURE;
		if (always_on_top) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (skip_taskbar) flags |= SDL_WINDOW_SKIP_TASKBAR;

		// Create window
		window = SDL_CreateWindow(
			App->GetTitle(),
			rect.x >= 0 ? rect.x : SDL_WINDOWPOS_UNDEFINED,
			rect.y >= 0 ? rect.y : SDL_WINDOWPOS_UNDEFINED,
			rect.w, rect.h, flags);

		if (window != nullptr)
		{
			display_index = SDL_GetWindowDisplayIndex(window);
			if (display_index >= 0)
			{
				// Reset rect
				SDL_GetWindowPosition(window, &rect.x, &rect.y);
				SDL_GetWindowSize(window, &rect.w, &rect.h);

				// Get render target surface
				screen_surface = SDL_GetWindowSurface(window);
				if (screen_surface != nullptr)
				{
					ret = true;
					LOG("Created SDL window & surface");
				}
				else
					LOG("Window surface missing! SDL_Error: %s\n", SDL_GetError());
			}
			else
				LOG("Error getting display index! SDL_Error: %s\n", SDL_GetError());
		}
		else
			LOG("Error creating window! SDL_Error: %s\n", SDL_GetError());
	}
	else
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());

	return ret;
}

bool Window::Update()
{
	/*int w, h;
	SDL_GetWindowSize(window, &w, &h);

	if (w != rect.w || h != rect.h)
		Event::Push(WINDOW_SIZE_CHANGED, this, w, h);*/

	return true;
}

// Called before quitting
bool Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window) SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return true;
}

void Window::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case WINDOW_SHOW:
	{
		if (!showing)
			showing = true;

		break;
	}
	case WINDOW_HIDDEN:
	{
		if (showing)
			showing = false;

		break;
	}
	case WINDOW_EXPOSED:
	{
		if (!showing)
			showing = true;

		break;
	}
	case WINDOW_MOVED:
	{
		rect.x = e.data1.AsInt();
		rect.y = e.data2.AsInt();
		break;
	}
	case WINDOW_SIZE_CHANGED:
	{
		rect.w = e.data1.AsInt();
		rect.h = e.data2.AsInt();
		Event::Push(WINDOW_SIZE_CHANGED, App->render, rect.w, rect.h);

		break;
	}
	case WINDOW_MINIMIZED:
	{
		minimized = true;
		maximized = false;

		SDL_GetWindowPosition(window, &rect.x, &rect.y);
		SDL_GetWindowSize(window, &rect.w, &rect.h);

		Event::Push(WINDOW_SIZE_CHANGED, App->render, rect.w, rect.h);

		break;
	}
	case WINDOW_MAXIMIZED:
	{
		minimized = false;
		maximized = true;
		fullscreen_desktop = true;

		SDL_GetWindowPosition(window, &rect.x, &rect.y);
		SDL_GetWindowSize(window, &rect.w, &rect.h);

		Event::Push(WINDOW_SIZE_CHANGED, App->render, rect.w, rect.h);

		break;
	}
	case WINDOW_RESTORED:
	{
		minimized = false;
		maximized = false;
		fullscreen_desktop = false;

		SDL_GetWindowPosition(window, &rect.x, &rect.y);
		SDL_GetWindowSize(window, &rect.w, &rect.h);

		LOG("Window restored: { %d, %d, %d, %d }", rect.w, rect.h);
		Event::Push(WINDOW_SIZE_CHANGED, App->render, rect.w, rect.h);

		break;
	}
	case WINDOW_ENTER:
	{
		if (!has_mouse_focus)
			has_mouse_focus = true;

		break;
	}
	case WINDOW_LEAVE:
	{
		if (has_mouse_focus)
			has_mouse_focus = false;

		break;
	}
	case WINDOW_FOCUS_GAINED:
	{
		if (!has_keyboard_focus)
			has_keyboard_focus = true;

		break;
	}
	case WINDOW_FOCUS_LEAVE:
	{
		if (has_keyboard_focus)
			has_keyboard_focus = false;

		break;
	}
	case WINDOW_SET_SIZE:
	{
		SDL_SetWindowSize(window, rect.w = e.data1.AsInt(), rect.h = e.data2.AsInt());
		Event::Push(WINDOW_SIZE_CHANGED, App->render, rect.w, rect.h);

		break;
	}
	case WINDOW_SET_POSITION:
	{
		SDL_SetWindowSize(window, rect.x = e.data1.AsInt(), rect.y = e.data2.AsInt());
	}
	case TOGGLE_FULLSCREEN:
	{
		if (fullscreen = !fullscreen)
		{
			prev_rect = rect;

			int w, h;
			SDL_GetWindowMaximumSize(window, &w, &h);
			if (!w || !h)
			{
				SDL_DisplayMode mode;
				SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(window), &mode);
				SDL_SetWindowMaximumSize(window, w = mode.w, h = mode.h);
			}

			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			Event::Push(WINDOW_SET_SIZE, this, w, h);
		}
		else
		{
			SDL_SetWindowFullscreen(window, 0);
			Event::Push(WINDOW_SET_SIZE, this, prev_rect.w, prev_rect.h);
		}

		break;
	}
	default:
		break;
	}
}

// Set new window title
void Window::SetTitle(const char* new_title)
{
	title = new_title;
	SDL_SetWindowTitle(window, new_title);
}

SDL_Window* Window::GetWindow() const
{
	return window;
}

void Window::GetWindowSize(int& w, int& h) const
{
	w = rect.w;
	h = rect.h;
}