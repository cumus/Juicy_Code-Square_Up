#include "Input.h"
#include "Event.h"
#include "Application.h"
#include "Window.h"
#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

Input::Input() : Module("input")
{
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

Input::~Input()
{}

bool Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	// Update SDL event queue
	SDL_PumpEvents();

	// Keyboard
	const Uint8* keys = SDL_GetKeyboardState(0);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	// Mouse
	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	// Events SDL
	static SDL_Event e;
	while(SDL_PollEvent(&e) != 0)
	{
		switch(e.type)
		{
			case SDL_QUIT:
				Event::Push(REQUEST_QUIT, App);
			break;

			case SDL_WINDOWEVENT:
				switch(e.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN: // Window has been hidden
					case SDL_WINDOWEVENT_MINIMIZED: // Window has been minimized

						Event::Push(WINDOW_HIDE, App);
						break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN: // Window has been shown
					case SDL_WINDOWEVENT_EXPOSED: // Window has been exposed and should be redrawn
					case SDL_WINDOWEVENT_MAXIMIZED: // Window has been maximized

						Event::Push(WINDOW_SHOW, App);
						break;

					case SDL_WINDOWEVENT_MOVED: // Window has been moved to data1, data2

						Event::Push(WINDOW_MOVED, App->win, Cvar(e.window.data1), Cvar(e.window.data2));
						break;

					case SDL_WINDOWEVENT_RESIZED: // Window has been resized to data1xdata2
					//case SDL_WINDOWEVENT_SIZE_CHANGED: // The window size has changed, either as a result of an API call or through the system or user changing the window size
					//case SDL_WINDOWEVENT_RESTORED: // Window has been restored to normal size and position
						Event::Push(WINDOW_SIZE_CHANGED, App->win, Cvar(e.window.data1), Cvar(e.window.data2));
						break;

					case SDL_WINDOWEVENT_ENTER: // Window has gained mouse focus
					case SDL_WINDOWEVENT_FOCUS_GAINED: // Window has gained keyboard focus
						Event::Push(WINDOW_FOCUS, App->win);
						break;

					case SDL_WINDOWEVENT_LEAVE: // Window has lost mouse focus
					case SDL_WINDOWEVENT_FOCUS_LOST: // Window has lost keyboard focus
						Event::Push(WINDOW_FOCUS_LEAVE, App->win);
						break;

					case SDL_WINDOWEVENT_CLOSE:
						Event::Push(WINDOW_QUIT, App);
						break;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[e.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[e.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
				mouse_motion_x = e.motion.xrel;
				mouse_motion_y = e.motion.yrel;
				mouse_x = e.motion.x;
				mouse_y = e.motion.y;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
		}
	}

	// Own Events
	Event::PumpAll();

	return true;
}

bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

/*KeyState Input::GetKey(int id) const
{
	return keyboard[id];
}

KeyState Input::GetMouseButtonDown(int id) const
{
	return mouse_buttons[id - 1];
}*/

void Input::GetMousePosition(int& x, int& y) const
{
	x = mouse_x;
	y = mouse_y;
}

void Input::GetMouseMotion(int& x, int& y) const
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}