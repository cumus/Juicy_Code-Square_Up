#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(unsigned int& width, unsigned int& height) const;

	// Retrieve window scale
	unsigned int GetScale() const;

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	unsigned int	width;
	unsigned int	height;

private:

	std::string		title;
	unsigned int	scale;
};

#endif // __WINDOW_H__