#include "Minimap.h"
#include "Application.h"
#include "TextureManager.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "Editor.h"
#include "Map.h"
#include "Input.h"
#include "Log.h"
#include "JuicyMath.h"

#include <string>

Minimap::Minimap(Gameobject* go) :
	C_Image(go)
{
	LOG("Starting minimap");

	ally = { 0, 0, 0, 0 };
	enemy = { 0, 0, 0, 0 };
	building = { 0, 0, 0, 0 };

	minimap_camera = { 0, 0, 4, 4 };
	camera_color = { 255, 255, 255, 255 };
	tex_id = App->render->GetMinimap();

	App->win->GetWindowSize(window_width, window_height);
}

Minimap::~Minimap()
{
}

void Minimap::Update()
{
	SDL_Rect camera_getter = App->render->GetCameraRect();

	App->render->DrawQuad(output, { 255, 0, 0, 255 }, false, EDITOR, false);

	//From map to minimap viewport
	float tile_width, tile_height;
	map.GetTileSize_F(tile_width, tile_height);

	float map_width, map_height;
	map.GetMapSize(map_width, map_height);

	float map_pixelwidth = map_width * tile_width;
	float map_pixelheight = map_height * tile_height;

	float scale_x, scale_y;
	scale_x = output.w / map_pixelwidth;
	scale_y = output.h / map_pixelheight;

	minimap_camera.x = camera_getter.x;
	minimap_camera.y = camera_getter.y;
	minimap_camera.w = camera_getter.w * scale_x;
	minimap_camera.h = camera_getter.h * scale_y;
				 
	minimap_camera.x = minimap_camera.x * scale_x;
	minimap_camera.y = minimap_camera.y * scale_y;

	minimap_camera.x += output.x + output.w / 2;
	minimap_camera.y += output.y;

	App->render->DrawQuad(minimap_camera, camera_color, false, EDITOR, false);
//-------------------------------------------------------------------------------------
	
	//From minimap to map drag viewport
	if (App->input->GetMouseButtonDown(0) == KeyState::KEY_REPEAT)
	{
		int x, y;
		App->input->GetMousePosition(x, y);

		if (mouse_inside = JMath::PointInsideRect(x, y, output))
		{
			x -= output.x;
			y -= output.y;

			App->render->cam.x = (x / scale_x) - map_pixelwidth / 2 - App->render->cam.w / 2;
			App->render->cam.y = y / scale_y - App->render->cam.h / 2;
		}
	}
//-------------------------------------------------------------------------------------

	/*SDL_Surface* manipulable = new SDL_Surface();
	manipulable = SDL_ConvertSurface(base_image, base_image->format, SDL_SWSURFACE);

	for (std::list<_Point>::iterator it = point_queue.begin(); it != point_queue.end(); it++)
	{
		SDL_Rect representation;

		representation.x = scale_x * it->rect.x;
		representation.y = scale_y * it->rect.y;
		representation.w = scale_x * it->rect.w;
		representation.h = scale_y * it->rect.h;

		SDL_FillRect(manipulable, &representation, SDL_MapRGB(manipulable->format, it->color.r, it->color.g, it->color.b));
	}
	point_queue.clear();

	for (std::list<_Sprite>::iterator it = sprite_queue.begin(); it != sprite_queue.end(); it++)
	{
		SDL_Surface* img_to_map;
		img_to_map = it->sprite_img;

		img_to_map->clip_rect.x = scale_x * it->section.x;
		img_to_map->clip_rect.y = scale_y * it->section.y;
		img_to_map->clip_rect.w = scale_x * it->section.w;
		img_to_map->clip_rect.h = scale_y * it->section.h;

		SDL_BlitSurface(it->sprite_img, &it->section, manipulable, &img_to_map->clip_rect);
	}
	sprite_queue.clear();

	SDL_Rect up = { -App->render->cam.x * scale_x,-App->render->cam.y * scale_y ,App->render->cam.w * scale_x, 1 };
	SDL_FillRect(manipulable, &up, SDL_MapRGB(manipulable->format, 255, 255, 255));

	SDL_Rect down = { -App->render->cam.x * scale_x,-(App->render->cam.y - App->render->cam.h) * scale_y - 1 ,App->render->cam.w * scale_x, 1 };
	SDL_FillRect(manipulable, &down, SDL_MapRGB(manipulable->format, 255, 255, 255));

	SDL_Rect left = { -App->render->cam.x * scale_x,-App->render->cam.y * scale_y ,1 , App->render->cam.h * scale_y };
	SDL_FillRect(manipulable, &left, SDL_MapRGB(manipulable->format, 255, 255, 255));

	SDL_Rect right = { -(App->render->cam.x - App->render->cam.w) * scale_x - 1 , -App->render->cam.y * scale_y ,1, App->render->cam.h * scale_y };
	SDL_FillRect(manipulable, &right, SDL_MapRGB(manipulable->format, 255, 255, 255));


	SDL_Texture* texture_to_blit = SDL_CreateTextureFromSurface(renderer, manipulable);
	App->render->Blit(0, output.x - App->render->cam.x, output.y - App->render->cam.y);

	SDL_DestroyTexture(texture_to_blit);
	SDL_FreeSurface(manipulable);
	manipulable = nullptr;*/
}

void Minimap::AddToMinimap(SDL_Rect rect, SDL_Color color)
{
	_Point p;
	p.rect = rect;
	p.color = color;

	point_queue.push_back(p);
}

void Minimap::Draw_Sprite(SDL_Surface* img, int x, int y)
{
	_Sprite sprite;

	sprite.sprite_img = img;
	sprite.section.x = x;
	sprite.section.y = y;
	sprite.section.w = img->w;
	sprite.section.h = img->h;
	
	sprite_queue.push_back(sprite);
}

