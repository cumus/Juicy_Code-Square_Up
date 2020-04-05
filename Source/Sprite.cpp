#include "Sprite.h"
#include "Application.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Transform.h"

#include "optick-1.3.0.0/include/optick.h"

Sprite::Sprite(Gameobject* go, ComponentType type) :
	Component(type, go),
	tex_id(-1),
	section({ 0, 0, 0, 0 }),
	layer(SCENE),
	color({ 0, 0, 0, 255 })
{}

Sprite::Sprite(Gameobject* go, int id, SDL_Rect s, Layer l, SDL_Color c) :
	Component(SPRITE, go),
	tex_id(id),
	section(s),
	layer(l),
	color(c)
{}

void Sprite::PostUpdate()
{
	OPTICK_EVENT();

	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		vec scale = t->GetGlobalScale();
		std::pair<float, float> map_pos = Map::F_MapToWorld(pos.x, pos.y, pos.z);

		if (tex_id >= 0)
			App->render->Blit_Scale(tex_id, int(map_pos.first), int(map_pos.second), scale.x, scale.y, &section, layer);
		else
			App->render->DrawQuad({ int(map_pos.first), int(map_pos.second), section.w, section.h }, color, layer);
	}
}

void Sprite::SetSection(const SDL_Rect s)
{
	section = s;
}

AnimatedSprite::AnimatedSprite(Behaviour* unit) : Sprite(unit->GetGameobject(), ANIM_SPRITE)
{
	current_state = *(unit_state = unit->GetStatePtr());

	switch (unit->GetType())
	{
	case EDGE:
	{
		tex_id = App->tex.Load("textures/Char_killia1.png");
		animations[FULL_LIFE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[HALF_LIFE].Setup({ 30, 200, 100, 180 }, 6);
		animations[DESTROYED].Setup({ 30, 380, 100, 180 }, 7);
		break; 
	}
	case UNIT_MELEE:
	{
		tex_id = App->tex.Load("textures/Char_killia1.png");
		animations[IDLE].Setup(section = { 30, 20, 100, 180}, 6);
		break; 
	}
	case ENEMY_MELEE:
		break;
	default:
		break;
	}
}

AnimatedSprite::~AnimatedSprite()
{
	unit_state = nullptr;
	animations.clear();
}

void AnimatedSprite::Update()
{
	if (current_state != *unit_state)
	{
		animations[current_state = *unit_state].Reset();
		frame_timer = 0.f;
	}
	else
		frame_timer += App->time.GetGameDeltaTime() * speed_mult;

	if (animations[current_state].Update(frame_timer))
		section.x = animations[current_state].GetSectionOffset();
}

Anim::Anim() : 
	frequency(-1),
	max_frames(-1),
	first_rect({ 0, 0, 0, 0}),
	current_frame(0)
{}

Anim::Anim(const Anim& copy) :
frequency(copy.frequency),
max_frames(copy.max_frames),
first_rect(copy.first_rect),
current_frame(copy.current_frame)
{}

void Anim::Setup(SDL_Rect rect, int frames, float f)
{
	first_rect = rect;
	max_frames = frames;
	frequency = f;
	current_frame = 0;
}

void Anim::Reset()
{
	current_frame = 0;
}

bool Anim::Update(float& frame_timer)
{
	bool ret;
	if (ret = (frame_timer >= 1.0f / frequency))
	{
		frame_timer -= 1.0f / frequency;
		if (++current_frame >= max_frames)
			current_frame = 0;
	}

	return ret;
}

int Anim::GetSectionOffset() const
{
	return first_rect.x + (first_rect.w * current_frame);
}
