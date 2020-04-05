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
	Sprite(Gameobject* game_object, int tex_id, SDL_Rect section, Layer layer = SCENE, SDL_Color color = { 0, 0, 0, 255 });
	virtual ~Sprite() {}

	void PostUpdate() override;

	void SetSection(const SDL_Rect section);

protected:

	int tex_id = -1;
	SDL_Rect section;
	SDL_Color color;
	Layer layer = SCENE;
};

class Anim
{
public:
	Anim();
	Anim(const Anim& copy);

	void Setup(SDL_Rect rect, int total_frames, float frequency = 1.0f);
	void Reset();
	bool Update(float& frame_timer);
	int GetSectionOffset() const;

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

private:

	float speed_mult = 1.0f;
	float frame_timer = 0.f;

	UnitState current_state;
	UnitState* unit_state;

	std::map<UnitState, Anim> animations;
};

#endif // __SPRITE_H__