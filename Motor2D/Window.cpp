#include "Window.h"
#include "SDL/include/SDL.h"
#include "Defs.h"
#include "Log.h"
#include "Application.h"

Window::Window() : Module("window")
{
	window = nullptr;
	screen_surface = nullptr;
}

// Destructor
Window::~Window()
{}

// Called before render is available
bool Window::Awake(pugi::xml_node& config)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) >= 0)
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;
		bool fullscreen = config.child("fullscreen").attribute("value").as_bool(false);
		bool borderless = config.child("borderless").attribute("value").as_bool(false);
		bool resizable = config.child("resizable").attribute("value").as_bool(false);
		bool fullscreen_window = config.child("fullscreen_window").attribute("value").as_bool(false);

		width = config.child("resolution").attribute("width").as_int(640);
		height = config.child("resolution").attribute("height").as_int(480);
		scale = config.child("resolution").attribute("scale").as_int(1);

		if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
		if (borderless) flags |= SDL_WINDOW_BORDERLESS;
		if (resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (fullscreen_window) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		window = SDL_CreateWindow(
			App->GetTitle(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width, height, flags);

		if (window != NULL)
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
	if(window != NULL) SDL_DestroyWindow(window);

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

void Window::GetWindowSize(uint& w, uint& h) const
{
	w = width;
	h = height;
}

uint Window::GetScale() const
{
	return scale;
}