#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"
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
	bool CleanUp() override;

	void SetTitle(const char* new_title);
	SDL_Window* GetWindow() const;
	void GetWindowSize(int& width, int& height) const;

private:

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr; //The surface contained by the window

private:

	std::string	title;

	int width = 1080;
	int height = 720;
	bool fullscreen = false;
	bool borderless = false;
	bool resizable = true;
	bool fullscreen_window = false;
};

#endif // __WINDOW_H__