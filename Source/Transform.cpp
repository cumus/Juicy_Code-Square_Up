#include "Transform.h"
#include "Application.h"
#include "Scene.h"
#include "Map.h"
#include "Render.h"
#include "JuicyMath.h"
#include "Gameobject.h"
#include "Log.h"

#include "optick-1.3.0.0/include/optick.h"

Transform::Transform(Gameobject* go) : Component(TRANSFORM, go)
{
	pos.Set(0.f);
	scale.Set(1.f);

	global_parent_pos.Set(0.f);
	global_parent_scale.Set(1.f);

	modified = true;
}

Transform::~Transform()
{}

void Transform::PreUpdate()
{}

void Transform::Update()
{
	OPTICK_EVENT();

	if (modified)
	{
		if (game_object != nullptr)
			Event::Push(TRANSFORM_MODIFIED, game_object, Cvar(GetGlobalPosition()), Cvar(GetGlobalScale()));

		modified = false;
	}
}

void Transform::PostUpdate()
{
	if (Scene::DrawCollisions())
	{
		// Draw Isometric AABB
		SDL_Color color = { 255, 255, 0, 255 };

		// Base bottom
		App->render->DrawLine({ points[0].first, points[0].second }, { points[1].first, points[1].second }, color, DEBUG_SCENE);
		App->render->DrawLine({ points[1].first, points[1].second }, { points[2].first, points[2].second }, color, DEBUG_SCENE);

		// Base top
		App->render->DrawLine({ points[4].first, points[4].second }, { points[5].first, points[5].second }, color, DEBUG_SCENE);
		App->render->DrawLine({ points[5].first, points[5].second }, { points[6].first, points[6].second }, color, DEBUG_SCENE);
		App->render->DrawLine({ points[6].first, points[6].second }, { points[7].first, points[7].second }, color, DEBUG_SCENE);
		App->render->DrawLine({ points[7].first, points[7].second }, { points[4].first, points[4].second }, color, DEBUG_SCENE);

		// Vertical
		App->render->DrawLine({ points[0].first, points[0].second }, { points[4].first, points[4].second }, color, DEBUG_SCENE);
		App->render->DrawLine({ points[1].first, points[1].second }, { points[5].first, points[5].second }, color, DEBUG_SCENE);
		App->render->DrawLine({ points[2].first, points[2].second }, { points[6].first, points[6].second }, color, DEBUG_SCENE);
	}
}

void Transform::RecieveEvent(const Event & e)
{
	switch (e.type)
	{
	case PARENT_TRANSFORM_MODIFIED:
	{
		global_parent_pos = e.data1.AsVec();
		global_parent_scale = e.data2.AsVec();

		if (game_object != nullptr)
			Event::Push(TRANSFORM_MODIFIED, game_object, Cvar(GetGlobalPosition()), Cvar(GetGlobalScale()));

		break;
	}
	case TRANSFORM_MODIFIED:
	{
		ResetAABB();

		break;
	}
	default:
		break;
	}
}

void Transform::SetParent(Transform* parent)
{
	global_parent_pos = parent->GetGlobalPosition();
	global_parent_scale = parent->GetGlobalScale();
}

void Transform::SetLocalPos(const vec& p)
{
	if (pos != p)
	{
		pos = p;
		modified = true;
	}
}

void Transform::SetX(float val)
{
	if (pos.x != val)
	{
		pos.x = val;
		modified = true;
	}
}

void Transform::SetY(float val)
{
	if (pos.y != val)
	{
		pos.y = val;
		modified = true;
	}
}

void Transform::SetZ(float val)
{
	if (pos.z != val)
	{
		pos.z = val;
		modified = true;
	}
}

void Transform::MoveX(float val)
{
	if (val != 0)
	{
		pos.x += val;
		modified = true;
	}
}

void Transform::MoveY(float val)
{
	if (val != 0)
	{
		pos.y += val;
		modified = true;
	}
}

void Transform::MoveZ(float val)
{
	if (val != 0)
	{
		pos.z += val;
		modified = true;
	}
}

void Transform::ScaleX(float val)
{
	if (val != 1.0f)
	{
		scale.x *= val;
		modified = true;
	}
}

void Transform::ScaleY(float val)
{
	if (val != 1.0f)
	{
		scale.y *= val;
		modified = true;
	}
}

void Transform::ScaleZ(float val)
{
	if (val != 1.0f)
	{
		scale.z *= val;
		modified = true;
	}
}

bool Transform::GlobalPosIsDifferentFrom(vec global_pos) const
{
	return global_pos != GetGlobalPosition();
}

void Transform::ResetAABB()
{
	vec p = GetGlobalPosition();
	vec s = GetGlobalScale();
	std::pair<float, float> tile_size = Map::GetTileSize_F();
	std::pair<float, float> pos = Map::F_MapToWorld(p.x, p.y, p.z);

	float w = tile_size.first *= s.x;
	float h = tile_size.second *= s.y;

	points[0] = { int(pos.first),				int(pos.second + (h * 0.5f)) };
	points[1] = { int(pos.first + (w * 0.5f)),	int(pos.second + h) };
	points[2] = { int(pos.first + w),			int(pos.second + (h * 0.5f)) };
	points[3] = { int(pos.first + (w * 0.5f)),	int(pos.second) };
	points[4] = { int(pos.first),				int(pos.second + (h * 0.5f) - (s.z * h)) };
	points[5] = { int(pos.first + (w * 0.5f)),	int(pos.second + h - (s.z * h)) };
	points[6] = { int(pos.first + w),			int(pos.second + (h * 0.5f) - (s.z * h)) };
	points[7] = { int(pos.first + (w * 0.5f)),	int(pos.second - (s.z * h)) };

	float y_offset = Map::GetBaseOffset();
	for (int i = 0; i < 8; ++i)
		points[i].second += y_offset;// *0.5f;
}

bool Transform::Intersects(std::pair<float, float> p) const
{
	vec g_pos = GetGlobalPosition();
	vec g_scale = GetGlobalScale();

	return JMath::PointInsideRect(p.first, p.second, {
		g_pos.x - (0.5f * g_scale.x),
		g_pos.y - (0.5f * g_scale.y),
		g_scale.x,
		g_scale.y });
}

float Transform::DistanceTo(vec pos)
{
	return GetGlobalPosition().Distance(pos);
}
