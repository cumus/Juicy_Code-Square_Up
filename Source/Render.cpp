#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Audio.h"
#include "Map.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "JuicyMath.h"
#include "Defs.h"
#include "Log.h"

#include "optick-1.3.0.0/include/optick.h"
#include "SDL2_image-2.0.5/include/SDL_image.h"

std::pair<float, float> Render::target_res = { 1280.f, 720.f };
std::pair<float, float> Render::res_ratio;

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
		// Add alpha blending
		if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) == 0)
		{
			// Get render drawing color
			if (SDL_GetRenderDrawColor(renderer, &draw_color.r, &draw_color.g, &draw_color.b, &draw_color.a) == 0)
			{
				// Setup camera & viewport
				SDL_RenderGetViewport(renderer, &viewport);
				cam = { 0.f, 0.f, float(viewport.w), float(viewport.h) };
				SetupViewPort(16.0f / 9.0f);

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
	return (SetDrawColor(background) && SDL_RenderClear(renderer) == 0);
}

bool Render::Update()
{
	OPTICK_EVENT();

	// Zoom
	if (zoom_allowed)
	{
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
	}

	// Move camera
	bool moved = false;
	float moveSpeed = 200.000f * App->time.GetDeltaTime() / zoom;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) { moveSpeed *= 5.000f; moved = true; }
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {cam.x -= moveSpeed; moved = true;}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {cam.x += moveSpeed; moved = true;}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {cam.y -= moveSpeed; moved = true;}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {cam.y += moveSpeed; moved = true;}

	if (moved) Event::Push(CAMERA_MOVED, App->audio);
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
				/*if (data->camera)
				{				
					iPoint pos = ConvertIsoTo2D(iPoint(data->rect.x,data->rect.y));
					iPoint tempCam = CamToIsometric();
					//LOG("Camera X:%f/Y:%f/W:%f/H:%f",cam.x,cam.y,cam.w,cam.h);
					//LOG("Blit X:%d/Y:%d/W:%d/H:%d", data->rect.x, data->rect.y, data->rect.w, data->rect.h);
					if (data->rect.x < tempCam.x || (data->rect.w) > (tempCam.x+ cam.w) || (data->rect.y) < tempCam.y || (data->rect.h) > (tempCam.y+cam.h))
					{
						continue;
					}
				}*/
				
				
				switch (data->type)
				{
				case RenderData::TEXTURE_FULL:
				{
					if (data->texture != nullptr)
						if (!(ret = SDL_RenderCopy(renderer, data->texture, nullptr, &data->rect) == 0)) {}
					//LOG("Cannot blit texture to screen. SDL_RenderCopy error: %s", SDL_GetError());

					break;
				}
				case RenderData::TEXTURE_SECTION:
				{
					if (data->texture != nullptr)
						if (!(ret = SDL_RenderCopy(renderer, data->texture, &data->extra.section, &data->rect) == 0)) {}
					//LOG("Cannot blit texture section to screen. SDL_RenderCopy error: %s", SDL_GetError());

					break;
				}
				case RenderData::QUAD_FILLED:
				{
					SetDrawColor(data->extra.color);
					if (!(ret = (SDL_RenderFillRect(renderer, &data->rect) == 0)))
						LOG("Cannot draw filled rect. SDL_RenderFillRect error: %s", SDL_GetError());

					break;
				}
				case RenderData::QUAD_EMPTY:
				{
					SetDrawColor(data->extra.color);
					if (!(ret = (SDL_RenderDrawRect(renderer, &data->rect) == 0)))
						LOG("Cannot draw empty rect. SDL_RenderFillRect error: %s", SDL_GetError());

					break;
				}
				case RenderData::LINE:
				{
					SetDrawColor(data->extra.color);
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
		for (std::map<int, std::vector<RenderData>>::iterator it = layers[i].begin(); it != layers[i].end(); ++it)
			it->second.clear();

	// Update Screen
	SDL_RenderPresent(renderer);

	return ret;
}

iPoint Render::ConvertIsoTo2D(iPoint point)
{
	iPoint temp;
	temp.x = (2 * point.y + point.x) * 0.5;
	temp.y = (2 * point.y - point.x) * 0.5;
	return temp;
}

iPoint Render::CamToIsometric()
{
	iPoint tmpPoint;

	tmpPoint.x=cam.x;
	tmpPoint.y = cam.y;

	tmpPoint.x = tmpPoint.x * 2;

	if (tmpPoint.x < 0) {
		tmpPoint.x = abs((int)tmpPoint.x);
	}
	else if (tmpPoint.x > 0)
	{
		tmpPoint.x = -abs((int)tmpPoint.x);
	}

	tmpPoint = ConvertIsoTo2D(tmpPoint);

	return tmpPoint;
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
		SetupViewPort(16.0f / 9.0f);
		Event::Push(MINIMAP_UPDATE_TEXTURE, this);
		break;
	}
	case MINIMAP_UPDATE_TEXTURE:
	{
		RenderMinimap();
		break;
	}
	case MINIMAP_MOVE_CAMERA:
	{
		cam.x = e.data1.AsFloat();
		cam.y = e.data2.AsFloat();
		Event::Push(CAMERA_MOVED, App->audio);
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

void Render::ToggleZoomLocked()
{
	zoom_allowed = !zoom_allowed;
}

std::pair<float, float> Render::GetCameraCenter() const
{
	return { cam.x + (cam.w * 0.5f), cam.y + (cam.h * 0.5f) };
}

bool Render::InsideCam(float x, float y) const
{
	return JMath::PointInsideRect(x, y, cam);
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

bool Render::RenderMinimap()
{
	bool ret = false;
	TextureData data;

	if (App->tex.GetTextureData(minimap_texture, data))
	{
		int minimap_half_width = int(float(data.width) * 0.5f);
		if (SDL_SetRenderTarget(renderer, data.texture) == 0)
		{
			std::vector<std::pair<SDL_Rect, SDL_Rect>> rects;
			Map::SetMapScale(minimap_scale);
			SDL_Texture* tex = Map::GetMapC()->GetFullMap(rects);
			if (ret = (tex && !rects.empty()))
			{
				for (std::vector<std::pair<SDL_Rect, SDL_Rect>>::const_iterator it = rects.cbegin(); it != rects.cend() && ret; ++it)
				{
					SDL_Rect r = it->second;
					r.x += minimap_half_width;

					if (!(ret = SDL_RenderCopy(renderer, tex, &it->first, &r) == 0))
						LOG("Cannot blit to minimap texture. SDL_RenderCopy error: %s", SDL_GetError());
				}
			}
			else
				LOG("Map error drawing minimap");

			Map::SetMapScale(zoom);
			SDL_SetRenderTarget(renderer, nullptr);
		}
		else
			LOG("Error setting minimap render target. SDL_SetRenderTarget error: %s", SDL_GetError());
	}
	else
		LOG("Error retrieving minimap texture (id = %d)", minimap_texture);

	return ret;
}

inline void Render::AddToLayer(Layer layer, const RenderData& data)
{
	int pos = 0;

	if (layer < Layer::HUD && layer > Layer::DEBUG_MAP)
		pos = data.rect.y + data.rect.h;

	layers[layer][pos].push_back(data);
}

int Render::GetMinimap(int width, int height, float scale, bool trigger_event)
{
	if (minimap_texture < 0)
	{
		// Setup Minimap
		minimap_scale = scale;
		minimap_texture = App->tex.CreateEmptyTexture(renderer, width, height);
	}

	trigger_event ? Event::Push(MINIMAP_UPDATE_TEXTURE, this) : RenderMinimap();

	return minimap_texture;
}

void Render::SetupViewPort(float aspect_ratio)
{
	// Crop Cam's Aspect Ratio
	float target_height = cam.w / aspect_ratio;
	if (target_height != cam.h)
	{
		if (target_height > cam.h)
		{
			viewport = { int((cam.w - (cam.h * aspect_ratio)) * 0.5f) , 0, int(cam.h * aspect_ratio), int(cam.h) };
			cam.w = cam.h * aspect_ratio;
		}
		else
		{
			viewport = { 0, int((cam.h - target_height) * 0.5f), int(cam.w), int(target_height) };
			cam.h = target_height;
		}
	}
	else
	{
		viewport = { 0, 0, int(cam.w), int(cam.h) };
	}

	// Setup Viewport
	SDL_RenderSetViewport(renderer, &viewport);
	res_ratio = { cam.w / target_res.first, cam.h / target_res.second };
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

std::pair<int, int> Render::GetViewPortOffset()
{
	return { viewport.x, viewport.y };
}

std::pair<float, float> Render::GetResRatio()
{
	return res_ratio;
}

bool Render::Blit(int texture_id, int x, int y, const SDL_Rect* section, Layer layer, bool use_cam)
{
	bool ret;
	RenderData data(RenderData::TEXTURE_FULL);
	data.texture = App->tex.GetTexture(texture_id);
	data.camera = use_cam;

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

		AddToLayer(layer, data);
	}
	//else
		//LOG("Cannot blit to screen. Invalid id %d", texture_id);

	return ret;
}

bool Render::Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, const SDL_Rect* section, Layer layer, bool use_cam)
{
	bool ret;
	RenderData data(RenderData::TEXTURE_FULL);
	data.texture = App->tex.GetTexture(texture_id);
	data.camera = use_cam;

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

		AddToLayer(layer, data);
	}
	else
		//LOG("Cannot blit to screen. Invalid id %d", texture_id);

	return ret;
}

bool Render::BlitNorm(int texture_id, RectF rect, const SDL_Rect* section, Layer layer)
{
	bool ret;
	RenderData data(RenderData::TEXTURE_FULL);
	data.texture = App->tex.GetTexture(texture_id);
	data.camera = false;

	if (ret = (data.texture != nullptr))
	{
		data.rect = { int(cam.w * rect.x), int(cam.h * rect.y), int(cam.w * rect.w), int(cam.h * rect.h) };

		if (section != nullptr)
		{
			data.extra.section = *section;
			data.type = RenderData::TEXTURE_SECTION;
		}

		AddToLayer(layer, data);
	}
	//else
		//LOG("Cannot blit to screen. Invalid id %d", texture_id);

	return ret;
}

bool Render::Blit_Text(RenderedText* rendered_text, int x, int y, Layer layer, bool use_cam)
{
	bool ret = false;
	if (rendered_text != nullptr)
	{
		int width, height;
		if (ret = (rendered_text->GetSize(width, height)))
		{
			RenderData data(RenderData::TEXTURE_FULL);
			data.texture = rendered_text->GetTexture();
			data.camera = use_cam;

			if (use_cam)
				data.rect = { int(cam.x) + x, int(cam.y) + y, width, height };
			else
				data.rect = { x, y, width, height };

			AddToLayer(layer, data);
		}
		else
			LOG("Cannot blit text. Invalid text size");
	}
	else
		LOG("Cannot blit text. Invalid RenderedText");

	return ret;
}

bool Render::Blit_TextSized(RenderedText* rendered_text, SDL_Rect size, Layer layer, bool use_cam)
{
	bool ret;

	if (ret = (rendered_text != nullptr))
	{
		RenderData data(RenderData::TEXTURE_FULL);
		data.texture = rendered_text->GetTexture();
		data.camera = use_cam;

		if (use_cam)
			data.rect = { int(cam.x) + size.x, int(cam.y) + size.y, size.w, size.h };
		else
			data.rect = size;

		AddToLayer(layer, data);
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
	data.camera = use_camera;

	if (use_camera)
	{
		data.rect.x -= int(cam.x);
		data.rect.y -= int(cam.y);
	}

	AddToLayer(layer, data);
}

void Render::DrawQuadNormCoords(RectF rect, const SDL_Color color, bool filled, Layer layer)
{
	RenderData data(filled ? RenderData::QUAD_FILLED : RenderData::QUAD_EMPTY);
	data.texture = nullptr;
	data.rect = { int(rect.x * cam.w), int(rect.y * cam.h), int(rect.w * cam.w), int(rect.h * cam.h) };
	data.extra.color = color;
	data.camera = false;

	AddToLayer(layer, data);
}

void Render::DrawLine(const std::pair<int, int> a, const std::pair<int, int> b, const SDL_Color color, Layer layer, bool use_camera)
{
	RenderData data(RenderData::LINE);
	data.texture = nullptr;
	data.rect = { a.first, a.second, b.first, b.second };
	data.extra.color = color;
	data.camera = use_camera;

	if (use_camera)
	{
		data.rect.x -= int(cam.x);
		data.rect.y -= int(cam.y);
		data.rect.w -= int(cam.x);
		data.rect.h -= int(cam.y);
	}

	AddToLayer(layer, data);
}

void Render::DrawCircle(const std::pair<int, int> a, const std::pair<int, int> b, const SDL_Color color, Layer layer, bool use_camera)
{
	RenderData data(RenderData::CIRCLE);
	data.texture = nullptr;
	data.rect = { a.first, a.second, b.first, b.second };
	data.extra.color = color;
	data.camera = use_camera;

	if (use_camera)
	{
		data.rect.x -= int(cam.x);
		data.rect.y -= int(cam.y);
	}

	AddToLayer(layer, data);
}

Render::RenderData::RenderData(Type t) :
	type(t),
	texture(nullptr),
	rect({0, 0, 0, 0}),
	camera(false)
{
	extra.section = rect;
}

Render::RenderData::RenderData(const RenderData& copy) :
	type(copy.type),
	texture(copy.texture),
	rect(copy.rect),
	camera(copy.camera)
{
	if (type <= TEXTURE_SECTION)
		extra.section = copy.extra.section;
	else if (type > TEXTURE_SECTION)
		extra.color = copy.extra.color;
}
