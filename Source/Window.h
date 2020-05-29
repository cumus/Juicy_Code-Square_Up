#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

#include "SDL/include/SDL_rect.h"

#include <string>

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();
	~Window();

	void LoadConfig(bool empty_config) override;
	void SaveConfig() const override;

	bool Init() override;
	bool Update() override;
	bool CleanUp() override;

	void RecieveEvent(const Event& e) override;

	void SetTitle(const char* new_title);
	SDL_Window* GetWindow() const;
	void GetWindowSize(int& width, int& height) const;

private:

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr; //The surface contained by the window

private:

	std::string	title;
	SDL_Rect rect = { -1, -1, 1280, 720 };
	SDL_Rect prev_rect;

	int display_index = -1;

	bool showing = true;
	bool has_mouse_focus = false;
	bool has_keyboard_focus = false;

	// Flags
	bool fullscreen			= false;
	bool fullscreen_desktop = false;

	bool borderless			= false;
	bool resizable			= false;

	bool minimized			= false;
	bool maximized			= false;

	bool input_grabbed		= false;
	bool input_focus		= false;
	bool mouse_focus		= false;

	bool high_dpi			= true;
	bool mouse_capture		= false;
	bool always_on_top		= false;
	bool skip_taskbar		= false;
};

#endif // __WINDOW_H__