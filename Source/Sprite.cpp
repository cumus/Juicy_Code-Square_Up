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
	offset({ 0.f, 0.f, 1.f, 1.f }),
	color({ 0, 0, 0, 255 })
{}

Sprite::Sprite(Gameobject* go, int id, SDL_Rect s, Layer l, RectF o, SDL_Color c) :
	Component(SPRITE, go),
	tex_id(id),
	section(s),
	layer(l),
	offset(o),
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
			App->render->Blit_Scale(tex_id,
				int(map_pos.first + (offset.x * offset.w * scale.x)),
				int(map_pos.second + (((offset.y * offset.h) + Map::GetBaseOffset()) * scale.y)),
				scale.x * offset.w,
				scale.y * offset.h,
				&section, layer);
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
	case BASE_CENTER:
	{
		tex_id = App->tex.Load("textures/Base_Center.png");
		animations[FULL_LIFE].Setup(section = { 0, 0, 166, 534 }, 1);
		animations[HALF_LIFE].Setup({ 0, 0, 166, 534 }, 1);
		animations[DESTROYED].Setup({ 0, 0, 166, 534 }, 1);
		offset = { 20.f, -434.f + Map::GetBaseOffset(), 0.3f, 0.3f };
		break;
	}
	case TOWER:
	{
		tex_id = App->tex.Load("textures/Tower.png");
		animations[FULL_LIFE].Setup(section = { 0, 0, 62, 115 }, 1);
		animations[HALF_LIFE].Setup({ 0, 0, 62, 115 }, 1);
		animations[DESTROYED].Setup({ 0, 0, 62, 115 }, 1);
		break;
	}
	case EDGE:
	{
		tex_id = App->tex.Load("textures/Base_Center.png");
		animations[FULL_LIFE].Setup(section = { 0, 0, 166, 534 }, 1);
		animations[HALF_LIFE].Setup({ 0, 0, 166, 534 }, 1);
		animations[DESTROYED].Setup({ 0, 0, 166, 534 }, 1);
		offset = { 20.f, -460.f, 0.3f, 0.3f };

		break; 
	}
	case UNIT_MELEE:
	{
		tex_id = App->tex.Load("textures/Char_killia1.png");
		animations[IDLE].Setup(section = { 41, 24, 89, 167 }, 6, 0.2f);
		//animations[MOVING_E].Setup();
		//animations[MOVING_W].Setup();
		//animations[MOVING_N].Setup();
		//animations[MOVING_S].Setup();
		animations[MOVING_NE].Setup(section = { 41, 24, 89, 167 }, 6, 0.2f);
		animations[MOVING_NW].Setup(section = { 41, 24, 89, 167 }, 6, 0.2f);
		animations[MOVING_SE].Setup(section = { 41, 24, 89, 167 }, 6, 0.2f);
		animations[MOVING_SW].Setup(section = { 41, 24, 89, 167 }, 6, 0.2f);
		animations[ATTACKING_E].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_W].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_N].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_S].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_NE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_NW].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_SE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_SW].Setup(section = { 30, 20, 100, 180 }, 6);
		//animations[DESTROYED].Setup();
		offset = { 5.f, -160.f, 0.6f, 0.6f };
		break; 
	}
	case ENEMY_MELEE:
		tex_id = App->tex.Load("textures/Enemy_hobbit1.png");
		animations[IDLE].Setup(section = { 30, 20, 100, 180 }, 6, 0.2f);
		//animations[MOVING_E].Setup();
		//animations[MOVING_W].Setup();
		//animations[MOVING_N].Setup();
		//animations[MOVING_S].Setup();
		animations[MOVING_NE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[MOVING_NW].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[MOVING_SE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[MOVING_SW].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_E].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_W].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_N].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_S].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_NE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_NW].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_SE].Setup(section = { 30, 20, 100, 180 }, 6);
		animations[ATTACKING_SW].Setup(section = { 30, 20, 100, 180 }, 6);
		//animations[DESTROYED].Setup();
		offset = { 0.f, 0.f, 0.5f, 0.5f };
		break;
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
