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

	bool Awake(pugi::xml_node&) override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	// Load / Save
	bool Load(pugi::xml_node&) override;
	bool Save(pugi::xml_node&) const override;

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

	bool Blit_Text(const char* text, int x, int y, int font_id = -1, SDL_Color color = { 0, 0, 0, 255 }, unsigned int wrap_length = 1000u) const;
	bool Blit_TextSized(const char* text, SDL_Rect size, int font_id = -1, SDL_Color color = { 0, 0, 0, 255 }, unsigned int wrap_length = 1000u) const;

	bool DrawQuad(const SDL_Rect rect, SDL_Color color = { 0, 0, 0, 255 }, bool filled = true, bool use_camera = true) const;
	bool DrawQuadNormCoords(const RectF rect, SDL_Color color = { 0, 0, 0, 255 }, bool filled = true) const;
	bool DrawLine(const SDL_Point a, const SDL_Point b, SDL_Color color = { 0, 0, 0, 255 }, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, float radius, SDL_Color color = { 0, 0, 0, 255 }, bool use_camera = true) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

private:

	Uint32			flags = 0;
	SDL_Renderer*	renderer = nullptr;

	RectF cam;
	float zoom = 1.0f;

	SDL_Rect		viewport;
	SDL_Color		background;
};

#endif // __RENDER_H__