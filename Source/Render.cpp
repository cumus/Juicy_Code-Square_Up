#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Map.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "Defs.h"
#include "Log.h"

#include "optick-1.3.0.0/include/optick.h"
#include "SDL2_image-2.0.5/include/SDL_image.h"

Render::Render() : Module("renderer")
{
	background.r = background.g = background.b = 0;
	background.a = 255;
}

// Destructor
Render::~Render()
{}

void Render::LoadConfig(bool empty_config)
{
	pugi::xml_node config = FileManager::ConfigNode();
	if (empty_config)
	{
		pugi::xml_node render_flags = config.append_child(name).append_child("flags");
		render_flags.append_attribute("accelerated").set_value(accelerated);
		render_flags.append_attribute("vsync").set_value(vsync);
		render_flags.append_attribute("target_texture").set_value(target_texture);
	}
	else
	{
		pugi::xml_node render_flags = config.child(name).child("flags");
		accelerated = render_flags.attribute("accelerated").as_bool(accelerated);
		vsync = render_flags.attribute("vsync").as_bool(vsync);
		target_texture = render_flags.attribute("height").as_bool(target_texture);
	}
}

void Render::SaveConfig() const
{
	pugi::xml_node config = FileManager::ConfigNode();
	pugi::xml_node render_flags = config.child(name).child("flags");
	render_flags.attribute("accelerated").set_value(accelerated);
	render_flags.attribute("vsync").set_value(vsync);
	render_flags.attribute("target_texture").set_value(target_texture);
}

// Called before the first frame
bool Render::Init()
{
	bool ret = false;

	// Set loaded flags
	unsigned int flags = 0;
	if (accelerated) flags |= SDL_RENDERER_ACCELERATED;
	if (vsync) flags |= SDL_RENDERER_PRESENTVSYNC;
	if (target_texture) flags |= SDL_RENDERER_TARGETTEXTURE;

	// Create SDL rendering context
	renderer = SDL_CreateRenderer(App->win->GetWindow(), -1, flags);
	if (renderer)
	{
		SDL_RenderGetViewport(renderer, &viewport);

		// Add alpha blending
		if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) == 0)
		{
			// Setup camera from viewport
			cam.x = cam.y = 0;
			cam.w = float(viewport.w);
			cam.h = float(viewport.h);

			ret = true;
		}
		else
			LOG("Could not Set Render Draw Blend Mode to SDL_BLENDMODE_BLEND! SDL_Error: %s\n", SDL_GetError());
	}
	else
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());

	return ret;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	return (SDL_RenderClear(renderer) == 0);
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
			Map::SetMapScale(zoom);
		}
		else if (target_zoom < 0.5f)
		{
			zoom = 0.5f;
			Map::SetMapScale(zoom);
		}
		else if (wheel_motion != 0)
		{
			// Get Tile at mouse
			int x, y;
			App->input->GetMousePosition(x, y);
			std::pair<int, int> mouse_tile = Map::WorldToTileBase(cam.x + float(x), cam.y + float(y));
			std::pair<float, float> mouse_tile_f = { float(mouse_tile.first), float(mouse_tile.second) };

			// Get Tile at mouse pos - before and after zoom
			std::pair<float, float> tile_pos = Map::F_MapToWorld(mouse_tile_f.first, mouse_tile_f.second);
			Map::SetMapScale(zoom = target_zoom);
			std::pair<float, float> tile_pos_next = Map::F_MapToWorld(mouse_tile_f.first, mouse_tile_f.second);

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
	bool ret = false;

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

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, 0,nullptr, SDL_RendererFlip::SDL_FLIP_NONE) == 0)
			ret = true;
		else
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
	}
	else
		LOG("Cannot blit to screen. Invalid id %d");

	return ret;
}

bool Render::Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, const SDL_Rect* section, bool use_cam) const
{
	bool ret = false;

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

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) == 0)
			ret = true;
		else
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
	}
	else
		LOG("Cannot blit to screen. Invalid id %d");

	return ret;
}

// Blit to screen
bool Render::Blit_Rot(int texture_id, int x, int y, bool use_cam, const SDL_Rect* section, int flip, double angle, int pivot_x, int pivot_y) const
{
	bool ret = false;

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

		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_RendererFlip(flip)) == 0)
			ret = true;
		else
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
	}
	else
		LOG("Cannot blit to screen. Invalid id %d");

	return ret;
}

bool Render::BlitNorm(int texture_id, RectF rect, const SDL_Rect* section, bool draw_anyway) const
{
	bool ret = false;

	SDL_Texture* texture = App->tex.GetTexture(texture_id);

	if (texture != nullptr)
	{
		SDL_Rect target_rect = { int(cam.w * rect.x), int(cam.h * rect.y), int(cam.w * rect.w), int(cam.h * rect.h) };

		if (SDL_RenderCopyEx(renderer, texture, section, &target_rect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) == 0)
			ret = true;
		else
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
	}
	else if (draw_anyway)
	{
		ret = DrawQuadNormCoords(rect);
	}
	else
		LOG("Cannot blit to screen. Invalid id %d");

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

	// TODO: Release rendered text texture on changing text or removing component
	if (SDL_RenderCopyEx(renderer, App->fonts.RenderText(text, wrap_length, font_id, color.r, color.g, color.b, color.a), 0, &size, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
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