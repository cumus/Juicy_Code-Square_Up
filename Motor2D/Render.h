#ifndef __RENDER_H__
#define __RENDER_H__

#include "SDL/include/SDL.h"
#include "Module.h"

struct Sprite;

class Render : public Module
{
public:

	Render();
	~Render();

	bool Awake(pugi::xml_node&) override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	// Load / Save
	bool Load(pugi::xml_node&) override;
	bool Save(pugi::xml_node&) const override;

	// Viewport
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();

	// Blit
	bool Blit(int texture_id, int x, int y, const SDL_Rect* section = nullptr, bool use_cam = true) const;
	bool Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, bool use_cam = true, const SDL_Rect* section = nullptr) const;
	bool Blit_Rot(int texture_id, int x, int y, bool use_cam = true, const SDL_Rect* section = nullptr, int flip = 0, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:

	Uint32			flags = 0;
	SDL_Renderer*	renderer = nullptr;

	float cam_x, cam_y, cam_w, cam_h;
	SDL_Rect		viewport;
	SDL_Color		background;
};

#endif // __RENDER_H__