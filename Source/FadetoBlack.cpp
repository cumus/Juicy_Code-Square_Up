#include "FadeToBlack.h"
#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Log.h"
#include "Defs.h"

#include "optick-1.3.0.0/include/optick.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

#include <math.h>

FadeToBlack::FadeToBlack() : Module("FadeToBlack")
{}

FadeToBlack::~FadeToBlack()
{}

bool FadeToBlack::Start()
{
	LOG("Starting Fade.");

	screen = { 0, 0, 0, 0 };
	App->win->GetWindowSize(screen.w, screen.h);

	return (screen.w != 0 && screen.h != 0);
}

bool FadeToBlack::Update(float dt)
{

	//OPTICK_EVENT("fade_to_black_update");
	//OPTICK_THREAD("FadeToBlackUpdate");

	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	SDL_SetRenderDrawColor(App->render->GetSDLRenderer(), 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->GetSDLRenderer(), &screen);

	return true;
}

bool FadeToBlack::CleanUp()
{
	return true;
}

bool FadeToBlack::FadeTo(float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		ret = true;
	}

	return ret;
}
bool FadeToBlack::FadeFrom(float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_from_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		ret = true;
	}

	return ret;
}

bool FadeToBlack::Fading() const
{
	return current_step != fade_step::none;
}