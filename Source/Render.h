#ifndef __RENDER_H__
#define __RENDER_H__

#include "Module.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_pixels.h"
#include "Point.h"
#include <map>
#include <vector>

class Sprite;
class RenderedText;
struct SDL_Renderer;
struct SDL_Texture;

enum Layer : int
{
	BACKGROUND,
	MAP,
	WALKABILITY,
	DEBUG_MAP,
	BACK_SCENE,
	SCENE,
	FRONT_SCENE,
	DEBUG_SCENE,
	HUD,
	EDITOR,
	MAX_LAYERS
};

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

	void RecieveEvent(const Event& e) override;

	// Getters
	SDL_Renderer* GetSDLRenderer() const;
	SDL_Rect GetCameraRect() const;
	RectF GetCameraRectF() const;
	float GetZoom() const;
	std::pair<float, float> GetCameraCenter() const;

	// Viewport
	void SetupViewPort(float aspect_ratio);
	void ResetViewPort();
	std::pair<int, int> GetViewPortOffset();
	static std::pair<float, float> GetResRatio();

	// Blit
	bool Blit(int texture_id, int x, int y, const SDL_Rect* section = nullptr, Layer layer = SCENE, bool use_cam = true);
	bool Blit_Scale(int texture_id, int x, int y, float scale_x, float scale_y, const SDL_Rect* section = nullptr, Layer layer = SCENE, bool use_cam = true);
	bool BlitNorm(int texture_id, const RectF rect, const SDL_Rect* section = nullptr, Layer layer = SCENE);

	bool Blit_Text(RenderedText* rendered_text, int x, int y, Layer layer = SCENE, bool use_cam = true);
	bool Blit_TextSized(RenderedText* rendered_text, const SDL_Rect size, Layer layer = SCENE, bool use_cam = true);

	void DrawQuad(const SDL_Rect rect, const SDL_Color color = { 0, 0, 0, 255 }, bool filled = true, Layer layer = SCENE, bool use_camera = true);
	void DrawQuadNormCoords(const RectF rect, const SDL_Color color = { 0, 0, 0, 255 }, bool filled = true, Layer layer = SCENE);
	void DrawLine(const std::pair<int,int> a, const std::pair<int, int> b, const SDL_Color color = { 0, 0, 0, 255 }, Layer layer = SCENE, bool use_camera = true);
	void DrawCircle(const SDL_Rect rect, const SDL_Color color = { 0, 0, 0, 255 }, Layer layer = SCENE, bool use_camera = true);

	iPoint WorldToScreen(int x, int y) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

private:

	bool SetDrawColor(SDL_Color color);

private:

	SDL_Renderer* renderer = nullptr;
	SDL_Rect		viewport;
	SDL_Color		background;
	SDL_Color		draw_color;

	// Layer mapping
	struct RenderData
	{
		enum Type
		{
			TEXTURE_FULL,
			TEXTURE_SECTION,
			QUAD_FILLED,
			QUAD_EMPTY,
			LINE,
			CIRCLE,
			MAX_TYPES
		} type;

		RenderData(Type type);
		RenderData(const RenderData& copy);

		SDL_Texture* texture;
		SDL_Rect rect;
		union ExtraData
		{
			SDL_Rect section;
			SDL_Color color;
		} extra;
	};

	std::map<int, std::vector<RenderData>> layers[MAX_LAYERS] ;

	// Config
	bool accelerated = true;
	bool vsync = false;
	bool target_texture = false;

public:

	// Camera
	RectF cam;
	float zoom = 1.0f;
	static std::pair<float, float> target_res;
	static std::pair<float, float> res_ratio;
};

#endif // __RENDER_H__