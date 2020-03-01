#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "TextureManager.h"
#include "Defs.h"
#include "Log.h"

#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

Render::Render() : Module("renderer")
{
	background.r = background.g = background.b = background.a = 0;
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Init Image library");

	int img_flags = IMG_INIT_PNG;
	int init = IMG_Init(img_flags);

	if ((init & img_flags) != img_flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}
	else
	{
		// load flags
		flags = SDL_RENDERER_ACCELERATED;
		if (config.child("vsync").attribute("value").as_bool(true))
		{
			flags |= SDL_RENDERER_PRESENTVSYNC;
			LOG("Using vsync");
		}
	}

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	bool ret = true;

	LOG("Create SDL rendering context");
	renderer = SDL_CreateRenderer(App->win->window, -1, flags);
	if (renderer)
	{
		SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderGetViewport(renderer, &viewport);

		// setup camera
		cam_x = cam_y = 0;
		cam_w = float(viewport.w);
		cam_h = float(viewport.h);
	}
	else
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update()
{
	return true;
}

bool Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	return true;
}

// Load Game State
bool Render::Load(pugi::xml_node& data)
{
	cam_x = data.child("camera").attribute("x").as_float();
	cam_y = data.child("camera").attribute("y").as_float();

	return true;
}

// Save Game State
bool Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = cam_x;
	cam.append_attribute("y") = cam_y;

	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

bool Render::Blit(int texture_id, int x, int y, const SDL_Rect* section, bool use_cam) const
{
	bool ret = true;

	SDL_Texture* texture = App->tex->GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect rect;

		if (use_cam)
		{
			rect.x = x - int(cam_x);
			rect.y = y - int(cam_y);
		}
		else
		{
			rect.x = x;
			rect.y = y;
		}

		if (section)
		{
			rect.w = section->w;
			rect.h = section->h;
		}
		else
		{
			static TextureData tex_data;
			App->tex->GetTextureData(texture_id, tex_data);
			rect.w = tex_data.width;
			rect.h = tex_data.height;
		}

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, 0,nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}
	else
	{
		LOG("Cannot blit to screen. Invalid id %d");
		ret = false;
	}

	return ret;
}

bool Render::Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, bool use_cam, const SDL_Rect* section) const
{
	bool ret = true;

	SDL_Texture* texture = App->tex->GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect rect;

		if (use_cam)
		{
			rect.x = x - int(cam_x);
			rect.y = y - int(cam_y);
		}
		else
		{
			rect.x = x;
			rect.y = y;
		}

		if (section)
		{
			rect.w = section->w;
			rect.h = section->h;
		}
		else
		{
			static TextureData tex_data;
			App->tex->GetTextureData(texture_id, tex_data);
			rect.w = tex_data.width;
			rect.h = tex_data.height;
		}

		if (scale_x != 1.00f || scale_y != 1.00f)
		{
			rect.x -= int((float(rect.w) * scale_x) - float(rect.w) * 0.5f);
			rect.y -= int((float(rect.h) * scale_y) - float(rect.h) * 0.5f);

			rect.w = int(float(rect.w) * scale_x);
			rect.h = int(float(rect.h) * scale_y);
		}

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}
	else
	{
		LOG("Cannot blit to screen. Invalid id %d");
		ret = false;
	}

	return ret;
}

// Blit to screen
bool Render::Blit_Rot(int texture_id, int x, int y, bool use_cam, const SDL_Rect* section, int flip, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;

	SDL_Texture* texture = App->tex->GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect rect;

		if (use_cam)
		{
			rect.x = x - int(cam_x);
			rect.y = y - int(cam_y);
		}
		else
		{
			rect.x = x;
			rect.y = y;
		}

		if (section)
		{
			rect.w = section->w;
			rect.h = section->h;
		}
		else
		{
			static TextureData tex_data;
			App->tex->GetTextureData(texture_id, tex_data);
			rect.w = tex_data.width;
			rect.h = tex_data.height;
		}

		SDL_Point* p = nullptr;
		SDL_Point pivot;

		if (pivot_x != INT_MAX && pivot_y != INT_MAX)
		{
			pivot.x = pivot_x;
			pivot.y = pivot_y;
			p = &pivot;
		}

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_RendererFlip(flip)) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}
	else
	{
		LOG("Cannot blit to screen. Invalid id %d");
		ret = false;
	}

	return ret;
}

bool Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x -= int(cam_x);
		rec.y -= int(cam_y);
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	//unsigned int scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (use_camera)
		result = SDL_RenderDrawLine(
			renderer,
			int(cam_x) + x1, // * scale,
			int(cam_y) + y1, // * scale,
			int(cam_x) + x2, // * scale,
			int(cam_y) + y2); // * scale);
	else
		result = SDL_RenderDrawLine(
			renderer,
			x1, // * scale,
			y1, // * scale,
			x2, // * scale,
			y2); // * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	//unsigned int scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(unsigned int i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}