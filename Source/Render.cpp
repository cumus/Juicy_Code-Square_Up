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
	SetBackgroundColor({ 0, 0, 0, 255 });
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
			// Get render drawing color
			if (SDL_GetRenderDrawColor(renderer, &draw_color.r, &draw_color.g, &draw_color.b, &draw_color.a) == 0)
			{
				// Setup camera from viewport
				cam.x = cam.y = 0;
				cam.w = float(viewport.w);
				cam.h = float(viewport.h);

				ret = true;
			}
			else
				LOG("Could not Get Render Draw Color! SDL_SetRenderDrawBlendMode Error: %s\n", SDL_GetError());
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

		if (target_zoom > 2.0f)
		{
			zoom = 2.0f;
			Map::SetMapScale(zoom);
		}
		else if (target_zoom < 0.3f)
		{
			zoom = 0.3f;
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
	bool ret = true;

	// Render by layers
	for (int i = 0; i < MAX_LAYERS; ++i)
	{
		for (std::map<int, std::vector<RenderData>>::const_iterator it = layers[i].cbegin(); it != layers[i].cend() && ret; ++it)
		{
			// TODO: Check if map layers need sorting
			for (std::vector<RenderData>::const_iterator data = it->second.cbegin(); data != it->second.cend() && ret; ++data)
			{
				switch (data->type)
				{
				case RenderData::TEXTURE_FULL:
				{
					if (data->texture != nullptr)
						if (!(ret = SDL_RenderCopy(renderer, data->texture, nullptr, &data->rect) == 0))
							LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
					break;
				}
				case RenderData::TEXTURE_SECTION:
				{
					if (data->texture != nullptr)
						if (!(ret = SDL_RenderCopy(renderer, data->texture, &data->extra.section, &data->rect) == 0))
							LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
					break;
				}
				case RenderData::QUAD_FILLED:
				{
					if (!(ret = (SetDrawColor(data->extra.color) && SDL_RenderFillRect(renderer, &data->rect) == 0)))
						LOG("Cannot draw filled rect. SDL_RenderFillRect error: %s", SDL_GetError());
					break;
				}
				case RenderData::QUAD_EMPTY:
				{
					if (!(ret = (SetDrawColor(data->extra.color) && SDL_RenderDrawRect(renderer, &data->rect) == 0)))
						LOG("Cannot draw empty rect. SDL_RenderFillRect error: %s", SDL_GetError());
					break;
				}
				case RenderData::LINE:
				{
					if (!(ret = (SDL_RenderDrawLine(renderer, data->rect.x, data->rect.y, data->rect.w, data->rect.h) == 0)))
						LOG("Cannot draw line. SDL_RenderDrawLine error: %s", SDL_GetError());
					break;
				}
				case RenderData::CIRCLE:
				{
					if (ret = SetDrawColor(data->extra.color))
					{
						SDL_Point points[360];
						float factor = (float)M_PI / 180.0f;
						for (unsigned int i = 0; i < 360; ++i)
						{
							points[i].x = data->rect.x + int(float(data->rect.w) * cos(float(i) * factor));
							points[i].y = data->rect.y + int(float(data->rect.h) * sin(float(i) * factor));
						}

						if (!(ret = (SDL_RenderDrawPoints(renderer, points, 360) == 0)))
							LOG("Cannot draw circle. SDL_RenderDrawPoints error: %s", SDL_GetError());
					}

					break;
				}
				default:
					break;
				}
			}
		}
	}

	for (int i = 0; i < MAX_LAYERS; ++i)
		for (std::map<int, std::vector<RenderData>>::iterator it = layers[i].begin(); it != layers[i].end() && ret; ++it)
			it->second.clear();

	// Update Screen
	SDL_RenderPresent(renderer);

	return ret;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case WINDOW_SIZE_CHANGED:
	{
		cam.w = float(e.data1.AsInt());
		cam.h = float(e.data2.AsInt());
		break;
	}
	default:
		break;
	}
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

bool Render::SetDrawColor(SDL_Color color)
{
	bool ret = true;

	if (color.r != draw_color.r ||
		color.g != draw_color.g ||
		color.b != draw_color.b ||
		color.a != draw_color.a)
	{
		if (!(ret = (SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a) == 0)))
			LOG("Cannot set Render Draw Color. SDL_SetRenderDrawColor error: %s", SDL_GetError());
	}

	return ret;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

bool Render::Blit(int texture_id, int x, int y, const SDL_Rect* section, Layer layer, bool use_cam)
{
	bool ret;
	RenderData data(RenderData::TEXTURE_FULL);
	data.texture = App->tex.GetTexture(texture_id);

	if (ret = (data.texture != nullptr))
	{
		if (section != nullptr)
		{
			data.type = RenderData::TEXTURE_SECTION;
			data.extra.section = *section;
			data.rect.w = data.extra.section.w;
			data.rect.h = data.extra.section.h;
		}
		else
		{
			static TextureData tex_data;
			App->tex.GetTextureData(texture_id, tex_data);
			data.rect.w = tex_data.width;
			data.rect.h = tex_data.height;
		}

		if (use_cam)
		{
			data.rect.x = x - int(cam.x);
			data.rect.y = y - int(cam.y);
			data.rect.w = int(float(data.rect.w) * zoom);
			data.rect.h = int(float(data.rect.h) * zoom);
		}
		else
		{
			data.rect.x = x;
			data.rect.y = y;
		}

		layers[layer][data.rect.y].push_back(data);
	}
	else
		LOG("Cannot blit to screen. Invalid id %d", texture_id);

	return ret;
}

bool Render::Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, const SDL_Rect* section, Layer layer, bool use_cam)
{
	bool ret;
	RenderData data(RenderData::TEXTURE_FULL);
	data.texture = App->tex.GetTexture(texture_id);

	if (ret = (data.texture != nullptr))
	{
		if (section != nullptr)
		{
			data.type = RenderData::TEXTURE_SECTION;
			data.extra.section = *section;
			data.rect.w = section->w;
			data.rect.h = section->h;
		}
		else
		{
			static TextureData tex_data;
			App->tex.GetTextureData(texture_id, tex_data);
			data.rect.w = tex_data.width;
			data.rect.h = tex_data.height;
		}

		if (use_cam)
		{
			data.rect.x = x - int(cam.x);
			data.rect.y = y - int(cam.y);
			data.rect.w = int(float(data.rect.w) * zoom * scale_x);
			data.rect.h = int(float(data.rect.h) * zoom * scale_y);
		}
		else
		{
			data.rect.x = x;
			data.rect.y = y;
			data.rect.w = int(float(data.rect.w) * scale_x);
			data.rect.h = int(float(data.rect.h) * scale_y);
		}

		layers[layer][data.rect.y].push_back(data);
	}
	else
		LOG("Cannot blit to screen. Invalid id %d", texture_id);

	return ret;
}

/*
bool Render::Blit_Rot(int texture_id, int x, int y, bool use_cam, const SDL_Rect* section, int flip, double angle, int pivot_x, int pivot_y)
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
*/

bool Render::BlitNorm(int texture_id, RectF rect, const SDL_Rect* section, Layer layer)
{
	bool ret;
	RenderData data(RenderData::TEXTURE_FULL);
	data.texture = App->tex.GetTexture(texture_id);

	if (ret = (data.texture != nullptr))
	{
		data.rect = { int(cam.w * rect.x), int(cam.h * rect.y), int(cam.w * rect.w), int(cam.h * rect.h) };

		if (section != nullptr)
		{
			data.extra.section = *section;
			data.type = RenderData::TEXTURE_SECTION;
		}

		layers[layer][data.rect.y].push_back(data);
	}
	else
		LOG("Cannot blit to screen. Invalid id %d", texture_id);

	return ret;
}

bool Render::Blit_Text(RenderedText* rendered_text, int x, int y, Layer layer)
{
	bool ret = false;
	if (rendered_text != nullptr)
	{
		int width, height;
		if (ret = (rendered_text->GetSize(width, height)))
		{
			RenderData data(RenderData::TEXTURE_FULL);
			data.texture = rendered_text->GetTexture();
			data.rect = { x, y, width, height };

			layers[layer][data.rect.y].push_back(data);
		}
		else
			LOG("Cannot blit text. Invalid text size");
	}
	else
		LOG("Cannot blit text. Invalid RenderedText");

	return ret;
}

bool Render::Blit_TextSized(RenderedText* rendered_text, SDL_Rect size, Layer layer)
{
	bool ret;

	if (ret = (rendered_text != nullptr))
	{
		RenderData data(RenderData::TEXTURE_FULL);
		data.texture = rendered_text->GetTexture();
		data.rect = size;

		layers[layer][data.rect.y].push_back(data);
	}
	else
		LOG("Cannot blit text. Invalid RenderedText");

	return ret;
}

void Render::DrawQuad(const SDL_Rect rect, const SDL_Color color, bool filled, Layer layer, bool use_camera)
{
	RenderData data(filled ? RenderData::QUAD_FILLED : RenderData::QUAD_EMPTY);
	data.texture = nullptr;
	data.rect = rect;
	data.extra.color = color;

	if (use_camera)
	{
		data.rect.x -= int(cam.x);
		data.rect.y -= int(cam.y);
	}

	layers[layer][data.rect.y].push_back(data);
}

void Render::DrawQuadNormCoords(RectF rect, const SDL_Color color, bool filled, Layer layer)
{
	RenderData data(filled ? RenderData::QUAD_FILLED : RenderData::QUAD_EMPTY);
	data.texture = nullptr;
	data.rect = { int(rect.x * cam.w), int(rect.y * cam.h), int(rect.w * cam.w), int(rect.h * cam.h) };
	data.extra.color = color;

	layers[layer][data.rect.y].push_back(data);
}

void Render::DrawLine(const std::pair<int, int> a, const std::pair<int, int> b, const SDL_Color color, Layer layer, bool use_camera)
{
	RenderData data(RenderData::LINE);
	data.texture = nullptr;
	data.rect = { a.first, a.second, b.first, b.second };
	data.extra.color = color;

	if (use_camera)
	{
		data.rect.x -= int(cam.x);
		data.rect.y -= int(cam.y);
		data.rect.w -= int(cam.x);
		data.rect.h -= int(cam.y);
	}

	layers[layer][data.rect.y].push_back(data);
}

void Render::DrawCircle(const SDL_Rect rect, const SDL_Color color, Layer layer, bool use_camera)
{
	RenderData data(RenderData::CIRCLE);
	data.texture = nullptr;
	data.rect = rect;
	data.extra.color = color;

	if (use_camera)
	{
		data.rect.x -= int(cam.x);
		data.rect.y -= int(cam.y);
	}

	layers[layer][data.rect.y].push_back(data);
}

Render::RenderData::RenderData(Type t) :
	type(t),
	texture(nullptr),
	rect({0, 0, 0, 0})
{
	extra.section = rect;
}

Render::RenderData::RenderData(const RenderData& copy) :
	type(copy.type),
	texture(copy.texture),
	rect(copy.rect)
{
	if (type <= TEXTURE_SECTION)
		extra.section = copy.extra.section;
	else if (type > TEXTURE_SECTION)
		extra.color = copy.extra.color;
}
