#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();
	~Window();

	bool Awake(pugi::xml_node&) override;
	bool CleanUp() override;

	void SetTitle(const char* new_title);
	void GetWindowSize(int& width, int& height) const;

public:

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr; //The surface contained by the window

private:

	std::string		title;

	unsigned int	width;
	unsigned int	height;
};

#endif // __WINDOW_H__