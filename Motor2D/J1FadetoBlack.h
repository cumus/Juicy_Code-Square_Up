#ifndef __J1FADETOBLACK_H
#define __J1FADETOBLACK_H


#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

class j1FadeToBlack : public j1Module
{
public:

	j1FadeToBlack();
	~j1FadeToBlack();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool FadeTo(float time = 1.0f);
	bool FadeFrom(float time = 1.0f);
	bool Fading() const;
private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
	j1Module* to_enable = nullptr;
	j1Module* to_disable = nullptr;
};


#endif
