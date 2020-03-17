#include "Window.h"
#include "Application.h"
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

		// Resolution
		pugi::xml_node res = window_node.append_child("resolution");
		res.append_attribute("width").set_value(width);
		res.append_attribute("height").set_value(height);

		// Window flags
		window_node.append_child("fullscreen").append_attribute("value").set_value(fullscreen);
		window_node.append_child("borderless").append_attribute("value").set_value(borderless);
		window_node.append_child("resizable").append_attribute("value").set_value(resizable);
		window_node.append_child("fullscreen_window").append_attribute("value").set_value(fullscreen_window);
	}
	else
	{
		pugi::xml_node window_node = config.child(name);

		// Resolution
		pugi::xml_node res = window_node.child("resolution");
		width = res.attribute("width").as_int(width);
		height = res.attribute("height").as_int(height);

		// Window flags
		fullscreen = window_node.child("fullscreen").first_attribute().as_bool(fullscreen);
		borderless = window_node.child("borderless").first_attribute().as_bool(borderless);
		resizable = window_node.child("resizable").first_attribute().as_bool(resizable);
		fullscreen_window = window_node.child("fullscreen_window").first_attribute().as_bool(fullscreen_window);
	}
}

void Window::SaveConfig() const
{
	pugi::xml_node config = FileManager::ConfigNode();
	pugi::xml_node window_node = config.child(name);

	// Resolution
	pugi::xml_node res = window_node.child("resolution");
	res.attribute("width").set_value(width);
	res.attribute("height").set_value(height);

	// Window flags
	window_node.child("fullscreen").attribute("value").set_value(fullscreen);
	window_node.child("borderless").attribute("value").set_value(borderless);
	window_node.child("resizable").attribute("value").set_value(resizable);
	window_node.child("fullscreen_window").attribute("value").set_value(fullscreen_window);
}

// Called before render is available
bool Window::Init()
{
	OPTICK_EVENT();

	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) >= 0)
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;

		if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
		if (borderless) flags |= SDL_WINDOW_BORDERLESS;
		if (resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (fullscreen_window) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		window = SDL_CreateWindow(
			"App",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width, height, flags);

		if (window)
		{
			screen_surface = SDL_GetWindowSurface(window);
		}
		else
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
	}
	else
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
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
	w = width;
	h = height;
}