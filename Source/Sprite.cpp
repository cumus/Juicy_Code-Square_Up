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
		{
			if (build_progress < 1.0f)
			{
				App->render->Blit_Scale(tainted_tex_id,
					int(map_pos.first),
					int(map_pos.second),
					scale.x * offset.w,
					scale.y * offset.h,
					&section, BACK_SCENE);

				int height = int(float(section.h) * build_progress);
				SDL_Rect target = { section.x, section.y + section.h - height, section.w, height };
				App->render->Blit_Scale(tex_id,
					int(map_pos.first),
					int(map_pos.second) + (scale.y * offset.h * (section.h - height)),
					scale.x * offset.w,
					scale.y * offset.h,
					&target, layer);
			}
			else
			{
				App->render->Blit_Scale(tex_id,
					int(map_pos.first),
					int(map_pos.second),
					scale.x * offset.w,
					scale.y * offset.h,
					&section, layer);
			}
		}
		else
		{
			App->render->DrawQuad({
			int(map_pos.first),
			int(map_pos.second),
			int(float(section.w) * scale.x * offset.w),
			int(float(section.h) * scale.y * offset.h) },
			color, layer);
		}
	}
}

void Sprite::SetSection(const SDL_Rect s)
{
	section = s;
}

void Sprite::SetColor(const SDL_Color clr)
{
	color = clr;
}

float Sprite::GetBuildEffectProgress() const
{
	return build_progress >= 1.0f ? 1.0f : build_progress;
}


AnimatedSprite::AnimatedSprite(Behaviour* unit) : Sprite(unit->GetGameobject(), ANIM_SPRITE)
{
	switch (unit->GetType())
	{
	case BASE_CENTER:
	{
		tex_id = App->tex.Load("textures/Buildings.png"); //BaseCenter
		animations[NO_UPGRADE].Setup(section = { 0, 0, 155, 233 }, 4, 4.0f);
		animations[FIRST_UPGRADE].Setup({ 646, 0, 158, 234 }, 4, 4.0f);
		animations[SECOND_UPGRADE].Setup({ 1290, 0, 165, 239 }, 4, 4.0f);
		animations[DESTROYED].Setup({ 2106, 0, 130, 188 }, 5, 8.0f);
		offset = { -15.0f, -250.0f + Map::GetBaseOffset(), 0.5f, 0.5f };
		break;
	}
	case TOWER:
	{
		tex_id = App->tex.Load("textures/Buildings.png"); //Tower
		animations[NO_UPGRADE].Setup(section = { 0, 247, 179, 156 }, 1, 4.0f);
		animations[FIRST_UPGRADE].Setup({ 913, 0, 179, 156 }, 4, 4.0f);
		animations[SECOND_UPGRADE].Setup({ 1827, 0, 179, 156 }, 4, 4.0f);
		animations[DESTROYED].Setup({ 2106, 0, 130, 188 }, 5, 8.0f);
		offset = { -100.0f, -670.0f + Map::GetBaseOffset(), 0.3f, 0.3f };
		break;
	}
	case EDGE:
	{
		tex_id = App->tex.Load("textures/Edge.png"); 
		animations[NO_UPGRADE].Setup(section = { 0, 0, 217, 134 }, 1);
		animations[DESTROYED].Setup({ 0, 0, 0, 0 }, 1);
		offset = { 0, -115.0f + Map::GetBaseOffset(), 0.3f, 0.3f };
		break;
	}
	case CAPSULE:
	{
		tex_id = App->tex.Load("textures/Capsule2.png");
		animations[FLY].Setup(section = { 0, 0, 105, 518 }, 18, 8.0f);
		animations[POSE].Setup({ 0, 519, 104, 360 }, 1);
		animations[OPEN].Setup({ 0, 519, 104, 360 }, 10, 4.0f);
		offset = { 2.0f, -360.0f + Map::GetBaseOffset(), 0.5f, 0.5f };
		break;
	}
	case LAB:
	{
		tex_id = App->tex.Load("textures/lab.png"); //Lab
		animations[NO_UPGRADE].Setup(section = { 0, 0, 462, 488 }, 18, 5.0f);
		animations[FIRST_UPGRADE].Setup({ 0, 532, 462, 488 }, 18, 5.0f);
		animations[SECOND_UPGRADE].Setup({ 0, 1037, 462, 488 }, 18, 5.0f);
		animations[DESTROYED].Setup({ 8316, 0, 462, 488 }, 18, 8.0f);
		offset = { 0.0f, -500.0f, 0.13f, 0.13f };
		break;
	}
	case BARRACKS:
	{
		tex_id = App->tex.Load("textures/Buildings.png"); //Barraks
		animations[NO_UPGRADE].Setup(section = { 0, 247, 179, 156 }, 1, 4.0f);
		animations[FIRST_UPGRADE].Setup({ 913, 0, 179, 156 }, 4, 4.0f);
		animations[SECOND_UPGRADE].Setup({ 1827, 0, 179, 156 }, 4, 4.0f);
		animations[DESTROYED].Setup({ 2106, 0, 130, 188 }, 5, 8.0f);
		offset = { -100.0f, -670.0f + Map::GetBaseOffset(), 0.3f, 0.3f };
		break;
	}
	case SPAWNER:
	{
		tex_id = App->tex.Load("textures/Buildings.png"); //Spawn enemy
		animations[NO_UPGRADE].Setup(section = { 0, 0, 560, 397 }, 4, 5.0f);
		animations[DESTROYED].Setup({ 0, 0, 560, 397 }, 4, 5.0f);
		offset = { -205.f, -300.0f, 0.4f, 0.4f };
		break;
	}
	case UNIT_MELEE:
	{
		tex_id = App->tex.Load("textures/Unit_Melee.png");
		animations[IDLE].Setup(section = { 1, 1, 140, 99 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 1, 99, 140, 99 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 1, 198, 140, 99 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 1, 297, 140, 99 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 1, 396, 140, 99 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 1, 495, 140, 99 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 1, 594, 140, 99 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 1, 693, 140, 99 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 1, 792, 140, 99 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 1, 891, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_S].Setup({ 1, 990, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_E].Setup({ 1, 1089, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_W].Setup({ 1, 1188, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NE].Setup({ 1, 1287, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NW].Setup({ 1, 1386, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SE].Setup({ 1, 1485, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SW].Setup({ 1, 1584, 140, 99 }, 7, 8.0f);
		animations[DESTROYED].Setup({ 1, 1683, 140, 99 }, 8, 5.0f);
		offset = { -53.0f, -95.0f, 1.5f, 1.5f };
		break;
	}
	
		case ENEMY_MELEE:
		tex_id = App->tex.Load("textures/Enemy_Melee.png");
		animations[IDLE].Setup(section = { 1, 1, 140, 99 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 1, 99, 140, 99 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 1, 198, 140, 99 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 1, 297, 140, 99 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 1, 396, 140, 99 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 1, 495, 140, 99 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 1, 594, 140, 99 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 1, 693, 140, 99 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 1, 792, 140, 99 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 1, 891, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_S].Setup({ 1, 990, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_E].Setup({ 1, 1089, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_W].Setup({ 1, 1188, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NE].Setup({ 1, 1287, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NW].Setup({ 1, 1386, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SE].Setup({ 1, 1485, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SW].Setup({ 1, 1584, 140, 99 }, 7, 8.0f);
		animations[DESTROYED].Setup({ 1, 1683, 140, 99 }, 8, 5.0f);
		offset = { -53.0f, -95.0f, 1.5f, 1.5f };
			break;
	
	case ENEMY_RANGED:
		animations[IDLE].Setup(section = { 1, 1, 140, 99 }, 6, 6.0f);
		animations[MOVING_N].Setup({ 1, 99, 140, 99 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 1, 198, 140, 99 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 1, 297, 140, 99 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 1, 396, 140, 99 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 1, 495, 140, 99 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 1, 594, 140, 99 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 1, 693, 140, 99 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 1, 792, 140, 99 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 1, 891, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_S].Setup({ 1, 990, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_E].Setup({ 1, 1089, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_W].Setup({ 1, 1188, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NE].Setup({ 1, 1287, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NW].Setup({ 1, 1386, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SE].Setup({ 1, 1485, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SW].Setup({ 1, 1584, 140, 99 }, 7, 8.0f);
		animations[DESTROYED].Setup({ 1, 1683, 140, 99 }, 8, 5.0f);
		offset = { -53.0f, -95.0f, 1.5f, 1.5f };
			break;
	case ENEMY_SUPER:
		tex_id = App->tex.Load("textures/Enemy_Super_Temp.png");
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
		tex_id = App->tex.Load("textures/Unit_Gatherer.png");
		animations[IDLE].Setup(section = { 1, 1, 140, 99 }, 5, 5.0f);
		animations[MOVING_N].Setup({ 1, 99, 140, 99 }, 7, 8.0f);
		animations[MOVING_S].Setup({ 1, 198, 140, 99 }, 7, 8.0f);
		animations[MOVING_E].Setup({ 1, 297, 140, 99 }, 8, 8.0f);
		animations[MOVING_W].Setup({ 1, 396, 140, 99 }, 8, 8.0f);
		animations[MOVING_NE].Setup({ 1, 495, 140, 99 }, 8, 8.0f);
		animations[MOVING_NW].Setup({ 1, 594, 140, 99 }, 8, 8.0f);
		animations[MOVING_SE].Setup({ 1, 693, 140, 99 }, 8, 8.0f);
		animations[MOVING_SW].Setup({ 1, 792, 140, 99 }, 8, 8.0f);
		animations[ATTACKING_N].Setup({ 1, 891, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_S].Setup({ 1, 990, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_E].Setup({ 1, 1089, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_W].Setup({ 1, 1188, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NE].Setup({ 1, 1287, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_NW].Setup({ 1, 1386, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SE].Setup({ 1, 1485, 140, 99 }, 7, 8.0f);
		animations[ATTACKING_SW].Setup({ 1, 1584, 140, 99 }, 7, 8.0f);
		animations[DESTROYED].Setup({ 1, 1683, 140, 99 }, 8, 5.0f);
		offset = { -53.0f, -95.0f, 1.5f, 1.5f };
		break;
	case UNIT_RANGED:
		tex_id = App->tex.Load("textures/Unit_Ranged.png");
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
		tex_id = App->tex.Load("textures/Unit_Super.png");
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
	if (build_total_time > 1.0f)
	{
		build_timer += App->time.GetGameDeltaTime();

		if (build_timer >= build_total_time)
		{
			build_timer = build_total_time = frame_timer = 0.0f;
			build_progress = 2.0f;
			App->tex.Remove(tainted_tex_id);
		}
		else
		{
			build_progress = build_timer / build_total_time;
		}
	}
	
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
}

void AnimatedSprite::StartBuildEffect(float duration)
{
	build_total_time = duration;
	build_timer = build_progress = 0.0f;
	tainted_tex_id = App->tex.Load(App->tex.GetDataPtr(tex_id)->source.c_str(), true, 255, 255, 255, 50);
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
