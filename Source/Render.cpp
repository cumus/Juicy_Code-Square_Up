#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Map.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "Defs.h"
#include "Log.h"

#include "Optick/include/optick.h"

#include "SDL2_image-2.0.5/include/SDL_image.h"
#ifdef PLATFORMx86
#pragma comment( lib, "SDL2_image-2.0.5/lib/x86/SDL2_image.lib" )
#elif PLATFORMx64
#pragma comment( lib, "SDL2_image-2.0.5/lib/x64/SDL2_image.lib" )
#endif

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
		cam.x = cam.y = 0;
		cam.w = float(viewport.w);
		cam.h = float(viewport.h);
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
	OPTICK_EVENT();

	// Zoom
	int wheel_motion = App->input->GetMouseWheelMotion();
	if (wheel_motion != 0)
	{
		float target_zoom = zoom + float(wheel_motion) * 0.05f;

		if (target_zoom > 4.0f)
		{
			zoom = 4.0f;
			App->map->SetMapScale(zoom);
		}
		else if (target_zoom < 0.5f)
		{
			zoom = 0.5f;
			App->map->SetMapScale(zoom);
		}
		else if (wheel_motion != 0)
		{
			// Get Tile at mouse
			int x, y;
			App->input->GetMousePosition(x, y);
			std::pair<int, int> mouse_tile = App->map->WorldToTileBase(int(cam.x) + x, int(cam.y) + y);
			x = int(mouse_tile.first);
			y = int(mouse_tile.second);

			// Get Tile at mouse pos - before and after zoom
			std::pair<float, float> tile_pos = App->map->F_MapToWorld(x, y);
			App->map->SetMapScale(zoom = target_zoom);
			std::pair<float, float> tile_pos_next = App->map->F_MapToWorld(x, y);

			// Displace camera - keep mouse at same tile
			cam.x += (tile_pos_next.first - tile_pos.first);
			cam.y += (tile_pos_next.second - tile_pos.second);
		}
	}

	// Move camera
	float moveSpeed = 200.000f * App->time.GetDeltaTime() / zoom;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) moveSpeed *= 5.000f;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) cam.x -= moveSpeed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) cam.x += moveSpeed;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) cam.y -= moveSpeed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) cam.y += moveSpeed;

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
	cam.x = data.child("camera").attribute("x").as_float();
	cam.y = data.child("camera").attribute("y").as_float();

	return true;
}

// Save Game State
bool Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node camera = data.append_child("camera");

	camera.append_attribute("x") = cam.x;
	camera.append_attribute("y") = cam.y;

	return true;
}

SDL_Renderer* Render::GetSDLRenderer() const
{
	return renderer;
}

SDL_Rect Render::GetCameraRect() const
{
	return { int(cam.x), int(cam.y), int(cam.w), int(cam.h) };
}

RectF Render::GetCameraRectF() const
{
	return cam;
}

float Render::GetZoom() const
{
	return zoom;
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

	SDL_Texture* texture = App->tex.GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect rect;
		if (section)
		{
			rect.w = section->w;
			rect.h = section->h;
		}
		else
		{
			static TextureData tex_data;
			App->tex.GetTextureData(texture_id, tex_data);
			rect.w = tex_data.width;
			rect.h = tex_data.height;
		}
		if (use_cam)
		{
			rect.x = x - int(cam.x);
			rect.y = y - int(cam.y);
			rect.w = int(float(rect.w) * zoom);
			rect.h = int(float(rect.h) * zoom);
		}
		else
		{
			rect.x = x;
			rect.y = y;
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

bool Render::Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, const SDL_Rect* section, bool use_cam) const
{
	bool ret = true;

	SDL_Texture* texture = App->tex.GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect rect;

		if (section)
		{
			rect.w = section->w;
			rect.h = section->h;
		}
		else
		{
			static TextureData tex_data;
			App->tex.GetTextureData(texture_id, tex_data);
			rect.w = tex_data.width;
			rect.h = tex_data.height;
		}

		if (use_cam)
		{
			rect.x = x - int(cam.x);
			rect.y = y - int(cam.y);
			rect.w = int(float(rect.w) * zoom * scale_x);
			rect.h = int(float(rect.h) * zoom * scale_y);
		}
		else
		{
			rect.x = x;
			rect.y = y;
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

	SDL_Texture* texture = App->tex.GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect rect;

		if (use_cam)
		{
			rect.x = x - int(cam.x);
			rect.y = y - int(cam.y);
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
			App->tex.GetTextureData(texture_id, tex_data);
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

bool Render::Blit_Text(const char* text, int x, int y, int font_id, SDL_Color color, unsigned int wrap_length) const
{
	bool ret = true;

	int width, height;
	if (App->fonts.CalcSize(text, width, height, font_id))
	{
		SDL_Rect rect = { x, y, width, height };

		if (SDL_RenderCopyEx(renderer, App->fonts.RenderText("Square UP!", wrap_length, font_id, color.r, color.g, color.b, color.a), 0, &rect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}

	return ret;
}

bool Render::Blit_TextSized(const char* text, SDL_Rect size, int font_id, SDL_Color color, unsigned int wrap_length) const
{
	bool ret = true;

	if (SDL_RenderCopyEx(renderer, App->fonts.RenderText("Square UP!", wrap_length, font_id, color.r, color.g, color.b, color.a), 0, &size, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawQuad(const SDL_Rect rect, SDL_Color color, bool filled, bool use_camera) const
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	SDL_Rect dim(rect);
	if(use_camera)
	{
		dim.x -= int(cam.x);
		dim.y -= int(cam.y);
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &dim) : SDL_RenderDrawRect(renderer, &dim);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawQuadNormCoords(RectF rect, SDL_Color color, bool filled) const
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	SDL_Rect dim = { int(rect.x * cam.w), int(rect.y * cam.h), int(rect.w * cam.w), int(rect.h * cam.h) };

	int result = (filled) ? SDL_RenderFillRect(renderer, &dim) : SDL_RenderDrawRect(renderer, &dim);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(const SDL_Point a, const SDL_Point b, SDL_Color color, bool use_camera) const
{
	bool ret = true;
	//unsigned int scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	int result = -1;

	if (use_camera)
		result = SDL_RenderDrawLine(renderer, a.x - int(cam.x), a.y - int(cam.y), b.x - int(cam.x), b.y - int(cam.y));
	else
		result = SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, float radius, SDL_Color color, bool use_camera) const
{
	bool ret = true;

	if (use_camera)
	{
		x -= int(cam.x);
		y -= int(cam.y);
	}

	SDL_Point points[360];
	float factor = (float)M_PI / 180.0f;
	for(unsigned int i = 0; i < 360; ++i)
	{
		points[i].x = x + int(radius * cos(i * factor));
		points[i].y = y + int(radius * sin(i * factor));
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	if(SDL_RenderDrawPoints(renderer, points, 360) != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}