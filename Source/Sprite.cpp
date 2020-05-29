#include "Sprite.h"
#include "Application.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"
#include "JuicyMath.h"

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

		map_pos.first += offset.x * offset.w * scale.x;
		map_pos.second += ((offset.y * offset.h) + Map::GetBaseOffset()) * scale.y;

		if (tex_id >= 0)
			App->render->Blit_Scale(tex_id,
				int(map_pos.first),
				int(map_pos.second),
				scale.x * offset.w,
				scale.y * offset.h,
				&section, layer);
		else
			App->render->DrawQuad({
			int(map_pos.first),
			int(map_pos.second),
			int(float(section.w) * scale.x * offset.w),
			int(float(section.h) * scale.y * offset.h) },
			color, layer);
	}
}

void Sprite::SetSection(const SDL_Rect s)
{
	section = s;
}

AnimatedSprite::AnimatedSprite(Behaviour* unit) : Sprite(unit->GetGameobject(), ANIM_SPRITE)
{
	switch (unit->GetType())
	{
	case BASE_CENTER:
	{
		tex_id = App->tex.Load("Assets/textures/BaseCenter.png");
		animations[NO_UPGRADE].Setup(section = { 0, 0, 546, 813 }, 4, 4.0f);
		animations[FIRST_UPGRADE].Setup({ 2812, 0, 562, 813 }, 4, 4.0f);
		animations[SECOND_UPGRADE].Setup({ 5625, 0, 562, 813 }, 4, 4.0f);
		animations[DESTROYED].Setup({ 8956, 0, 562, 813 }, 5, 8.0f);
		offset = { -60.0f, -780.0f + Map::GetBaseOffset(), 0.15f, 0.15f };
		break;
	}
	case TOWER:
	{
		tex_id = App->tex.Load("Assets/textures/Tower.png");
		animations[NO_UPGRADE].Setup(section = { 0, 0, 185, 746 }, 1, 4.0f);
		animations[DESTROYED].Setup({ 0, 0, 502, 747 }, 5, 4.0f);
		offset = { 15.0f, -715.0f + Map::GetBaseOffset(), 0.3f, 0.3f };
		break;
	}
	case EDGE:
	{
		tex_id = App->tex.Load("Assets/textures/Edge.png");
		animations[NO_UPGRADE].Setup(section = { 0, 0, 217, 134 }, 1);
		animations[DESTROYED].Setup({ 0, 0, 0, 0 }, 1);
		offset = { 0, -115.0f + Map::GetBaseOffset(), 0.3f, 0.3f };
		break;
	}
	case CAPSULE:
	{
		tex_id = App->tex.Load("Assets/textures/Capsule2.png");
		animations[FLY].Setup(section = { 0, 0, 105, 518 }, 18, 8.0f);
		animations[POSE].Setup({ 0, 519, 104, 360 }, 1);
		animations[OPEN].Setup({ 0, 519, 104, 360 }, 10, 4.0f);
		offset = { 2.0f, -360.0f + Map::GetBaseOffset(), 0.5f, 0.5f };
		break;
	}
	case LAB:
	{
		tex_id = App->tex.Load("Assets/textures/lab.png");
		animations[NO_UPGRADE].Setup(section = { 0, 0, 462, 488 }, 18, 5.0f);
		animations[FIRST_UPGRADE].Setup({ 0, 532, 462, 488 }, 18, 5.0f);
		animations[SECOND_UPGRADE].Setup({ 0, 1037, 462, 488 }, 18, 5.0f);
		animations[DESTROYED].Setup({ 8316, 0, 462, 488 }, 18, 8.0f);
		offset = { 0.0f, -500.0f, 0.13f, 0.13f };
		break;
	}
	case BARRACKS:
	{
		tex_id = App->tex.Load("Assets/textures/Barracks.png");
		animations[NO_UPGRADE].Setup(section = { 0, 0, 625, 485 }, 5, 5.0f);
		animations[FIRST_UPGRADE].Setup({ 3130, 0, 625, 485 }, 5, 5.0f);
		animations[SECOND_UPGRADE].Setup({ 6260, 0, 625, 485 }, 5, 5.0f);
		animations[DESTROYED].Setup({ 9390, 0, 625, 485 }, 5, 8.0f);
		offset = { 0.0f, -550.0f + Map::GetBaseOffset(), 0.1f, 0.1f };
		break;
	}
	case SPAWNER:
	{
		tex_id = App->tex.Load("Assets/textures/SpawnEnemy.png");
		animations[NO_UPGRADE].Setup(section = { 0, 0, 560, 397 }, 4, 5.0f);
		animations[DESTROYED].Setup({ 0, 0, 560, 397 }, 4, 5.0f);
		offset = { -205.f, -100.f, 0.5f, 0.5f };
		break;
	}
	case UNIT_MELEE:
	{
		tex_id = App->tex.Load("Assets/textures/Unit_Melee.png");
		animations[IDLE].Setup(section = { 2, 2, 418, 295 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 2, 295, 418, 295 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 2, 590, 418, 295 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 2, 885, 418, 295 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 2, 1180, 418, 295 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 2, 1475, 418, 295 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 2, 1770, 418, 295 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 2, 2065, 418, 295 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 2, 2360, 418, 295 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 2, 2655, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_S].Setup({ 2, 2950, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_E].Setup({ 2, 3245, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_W].Setup({ 2, 3540, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_NE].Setup({ 2, 3835, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_NW].Setup({ 2, 4130, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_SE].Setup({ 2, 4425, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_SW].Setup({ 2, 4720, 418, 295 }, 7, 8.0f);
		animations[DESTROYED].Setup({ 2, 5015, 418, 295 }, 8, 5.0f);
		offset = { -168.0f, -280.0f, 0.6f, 0.6f };
		break;
	}
	case ENEMY_MELEE:
		tex_id = App->tex.Load("Assets/textures/Enemy_Melee_Temp.png");
		animations[IDLE].Setup(section = { 0, 0, 464, 365 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 0, 365, 464, 365 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 0, 730, 464, 365 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 0, 1095, 464, 365 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 0, 1460, 464, 365 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 0, 1825, 464, 365 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 0, 2190, 464, 365 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 0, 2555, 464, 365 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 0, 2920, 464, 365 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 0, 3285, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_S].Setup({ 0, 3650, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_E].Setup({ 0, 4015, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_W].Setup({ 0, 4380, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NE].Setup({ 0, 4745, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NW].Setup({ 0, 5110, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SE].Setup({ 0, 5475, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SW].Setup({ 0, 5840, 464, 365 }, 6, 10.0f);
		animations[DESTROYED].Setup({ 0, 6205, 464, 365 }, 8, 5.0f);
		offset = { -215.0f, -350.f, 0.5f, 0.5f };
		break;
	case ENEMY_RANGED:
		tex_id = App->tex.Load("Assets/textures/Enemy_Ranged_Temp.png");
		animations[IDLE].Setup(section = { 0, 0, 464, 365 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 0, 365, 464, 365 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 0, 730, 464, 365 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 0, 1095, 464, 365 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 0, 1460, 464, 365 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 0, 1825, 464, 365 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 0, 2190, 464, 365 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 0, 2555, 464, 365 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 0, 2920, 464, 365 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 0, 3285, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_S].Setup({ 0, 3650, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_E].Setup({ 0, 4015, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_W].Setup({ 0, 4380, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NE].Setup({ 0, 4745, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NW].Setup({ 0, 5110, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SE].Setup({ 0, 5475, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SW].Setup({ 0, 5840, 464, 365 }, 6, 10.0f);
		animations[DESTROYED].Setup({ 0, 6205, 464, 365 }, 8, 5.0f);
		offset = { -225.0f, -280.f, 0.5f, 0.5f };
		break;
	case ENEMY_SUPER:
		tex_id = App->tex.Load("Assets/textures/Enemy_Super_Temp.png");
		animations[IDLE].Setup(section = { 0, 0, 464, 365 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 0, 365, 464, 365 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 0, 730, 464, 365 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 0, 1095, 464, 365 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 0, 1460, 464, 365 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 0, 1825, 464, 365 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 0, 2190, 464, 365 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 0, 2555, 464, 365 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 0, 2920, 464, 365 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 0, 3285, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_S].Setup({ 0, 3650, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_E].Setup({ 0, 4015, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_W].Setup({ 0, 4380, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NE].Setup({ 0, 4745, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NW].Setup({ 0, 5110, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SE].Setup({ 0, 5475, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SW].Setup({ 0, 5840, 464, 365 }, 6, 10.0f);
		animations[DESTROYED].Setup({ 0, 6205, 464, 365 }, 8, 5.0f);
		offset = { -225.0f, -280.f, 0.6f, 0.6f };
		break;
	case GATHERER:
		tex_id = App->tex.Load("Assets/textures/Unit_Gatherer.png");
		animations[IDLE].Setup(section = { 3, 2, 418, 295 }, 5, 6.0f);
		animations[MOVING_N].Setup({ 2, 295, 418, 295 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 2, 590, 418, 295 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 2, 885, 418, 295 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 2, 1180, 418, 295 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 2, 1475, 418, 295 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 2, 1770, 418, 295 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 2, 2065, 418, 295 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 2, 2360, 418, 295 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 2, 2655, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_S].Setup({ 2, 2950, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_E].Setup({ 2, 3245, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_W].Setup({ 2, 3540, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_NE].Setup({ 2, 3835, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_NW].Setup({ 2, 4130, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_SE].Setup({ 2, 4425, 418, 295 }, 7, 8.0f);
		animations[ATTACKING_SW].Setup({ 2, 4720, 418, 295 }, 7, 8.0f);
		animations[DESTROYED].Setup({ 2, 5015, 418, 295 }, 8, 5.0f);
		offset = { -168.0f, -280.0f, 0.6f, 0.6f };
		break;
	case UNIT_RANGED:
		tex_id = App->tex.Load("Assets/textures/Unit_Ranged.png");
		animations[IDLE].Setup(section = { 0, 0, 464, 365 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 0, 365, 464, 365 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 0, 730, 464, 365 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 0, 1095, 464, 365 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 0, 1460, 464, 365 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 0, 1825, 464, 365 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 0, 2190, 464, 365 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 0, 2555, 464, 365 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 0, 2920, 464, 365 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 0, 3285, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_S].Setup({ 0, 3650, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_E].Setup({ 0, 4015, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_W].Setup({ 0, 4380, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NE].Setup({ 0, 4745, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NW].Setup({ 0, 5110, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SE].Setup({ 0, 5475, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SW].Setup({ 0, 5840, 464, 365 }, 6, 10.0f);
		animations[DESTROYED].Setup({ 0, 6205, 464, 365 }, 8, 5.0f);
		offset = { -225.0f, -280.f, 0.5f, 0.5f };
		break;
	case UNIT_SUPER:
		tex_id = App->tex.Load("Assets/textures/Unit_Super.png");
		animations[IDLE].Setup(section = { 0, 0, 464, 365 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 0, 365, 464, 365 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 0, 730, 464, 365 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 0, 1095, 464, 365 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 0, 1460, 464, 365 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 0, 1825, 464, 365 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 0, 2190, 464, 365 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 0, 2555, 464, 365 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 0, 2920, 464, 365 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 0, 3285, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_S].Setup({ 0, 3650, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_E].Setup({ 0, 4015, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_W].Setup({ 0, 4380, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NE].Setup({ 0, 4745, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_NW].Setup({ 0, 5110, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SE].Setup({ 0, 5475, 464, 365 }, 6, 10.0f);
		animations[ATTACKING_SW].Setup({ 0, 5840, 464, 365 }, 6, 10.0f);
		animations[DESTROYED].Setup({ 0, 6205, 464, 365 }, 8, 5.0f);
		offset = { -225.0f, -280.f, 0.6f, 0.6f };
		break;
	default:
		break;
	}

	unit_state = unit->GetSpriteStatePtr();
	section = animations[current_state = *unit_state].Reset(current_state < ATTACKING);

	if (current_state < ATTACKING)
		frame_timer = JMath::RandomF(animations[current_state].Period());
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
		section = animations[current_state = *unit_state].Reset(current_state < ATTACKING);
		frame_timer = 0.f;
	}
	else
	{
		frame_timer += App->time.GetGameDeltaTime() * speed_mult;

		if (animations[current_state].Update(frame_timer))
			section.x = animations[current_state].GetSectionOffset();
	}		

	/*RectF abc = { float(section.x)/3814.0f,float(section.y)/5179.0f,float(section.w)/3814.0f,float(section.h)/5179.0f };
	App->render->DrawQuadNormCoords(abc);*/
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

const SDL_Rect Anim::Reset(bool random_start)
{
	current_frame = random_start ? int(JMath::RandomF(float(max_frames+1))) : 0;
	return first_rect;
}

bool Anim::Update(float& frame_timer)
{
	bool ret;
	if (ret = (frame_timer >= Period()))
	{
		frame_timer -= Period();
		if (++current_frame >= max_frames)
			current_frame = 0;
	}

	return ret;
}

int Anim::GetSectionOffset() const
{
	return first_rect.x + (first_rect.w * current_frame);
}

float Anim::Period() const
{
	return 1.0f / frequency;
}
