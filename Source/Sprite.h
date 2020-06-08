#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Behaviour.h"

#include "Render.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_pixels.h"
#include <map>

class Sprite : public Component
{
public:

	Sprite(Gameobject* game_object, ComponentType type = SPRITE);
	Sprite(Gameobject* game_object, int tex_id, SDL_Rect section, Layer layer = SCENE, RectF offset = { 0.f, 0.f, 1.f, 1.f }, SDL_Color color = { 0, 0, 0, 255 });
	virtual ~Sprite() {}

	void PostUpdate() override;

	void SetSection(const SDL_Rect section);
	void SetColor(const SDL_Color color);
	float GetBuildEffectProgress() const;

public:

	RectF offset;

protected:

	int tex_id = -1;
	int tainted_tex_id = -1;
	SDL_Rect section;
	Layer layer = SCENE;
	SDL_Color color;
	float build_progress = 1.0f;
};

class Anim
{
public:
	Anim();
	Anim(const Anim& copy);

	void Setup(SDL_Rect rect, int total_frames, float frequency = 1.0f);
	const SDL_Rect Reset(bool random_start);
	bool Update(float& frame_timer);
	int GetSectionOffset() const;
	inline float Period() const;

private:

	float frequency = 1.0f;
	int max_frames = 0;
	SDL_Rect first_rect;

	int current_frame;
};

class AnimatedSprite : public Sprite
{
public:

	AnimatedSprite(Behaviour* unit);
	~AnimatedSprite();

	void Update() override;
	void StartBuildEffect(float duration);

private:

	float speed_mult = 1.0f;
	float frame_timer = 0.f;

	UnitState current_state;
	UnitState* unit_state;

	std::map<UnitState, Anim> animations;

	// Build effect
	float build_total_time = 0.0f;
	float build_timer = 0.0f;
};

#endif // __SPRITE_H__