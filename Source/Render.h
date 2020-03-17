#ifndef __RENDER_H__
#define __RENDER_H__

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_pixels.h"
#include "Module.h"

struct SDL_Renderer;
class Sprite;

class Render : public Module
{
public:

	Render();
	~Render();

	void LoadConfig(bool empty_config) override;
	void SaveConfig() const override;

	bool Init() override;
	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	// Getters
	SDL_Renderer* GetSDLRenderer() const;
	SDL_Rect GetCameraRect() const;
	RectF GetCameraRectF() const;
	float GetZoom() const;

	// Viewport
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();

	// Blit
	bool Blit(int texture_id, int x, int y, const SDL_Rect* section = nullptr, bool use_cam = true) const;
	bool Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, const SDL_Rect* section = nullptr, bool use_cam = true) const;
	bool Blit_Rot(int texture_id, int x, int y, bool use_cam = true, const SDL_Rect* section = nullptr, int flip = 0, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;

	bool BlitNorm(int texture_id, RectF rect, const SDL_Rect* section = nullptr, bool draw_anyway = true) const;

	bool Blit_Text(const char* text, int x, int y, int font_id = -1, SDL_Color color = { 0, 0, 0, 255 }, unsigned int wrap_length = 1000u) const;
	bool Blit_TextSized(const char* text, SDL_Rect size, int font_id = -1, SDL_Color color = { 0, 0, 0, 255 }, unsigned int wrap_length = 1000u) const;

	bool DrawQuad(const SDL_Rect rect, SDL_Color color = { 0, 0, 0, 255 }, bool filled = true, bool use_camera = true) const;
	bool DrawQuadNormCoords(const RectF rect, SDL_Color color = { 0, 0, 0, 255 }, bool filled = true) const;
	bool DrawLine(const SDL_Point a, const SDL_Point b, SDL_Color color = { 0, 0, 0, 255 }, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, float radius, SDL_Color color = { 0, 0, 0, 255 }, bool use_camera = true) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

private:

	SDL_Renderer*	renderer = nullptr;
	SDL_Rect		viewport;
	SDL_Color		background;

	// Camera
	RectF cam;
	float zoom = 1.0f;

	// Config
	bool accelerated = true;
	bool vsync = false;
	bool target_texture = false;
};

#endif // __RENDER_H__