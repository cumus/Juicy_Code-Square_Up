#include "Transform.h"
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
			Event::Push(TRANSFORM_MODIFIED, game_object,
				Cvar(global_parent_pos + pos),
				Cvar(global_parent_scale * scale));

		modified = false;
	}
}

void Transform::PostUpdate()
{}

void Transform::RecieveEvent(const Event & e)
{
	if (e.type == PARENT_TRANSFORM_MODIFIED)
	{
		global_parent_pos = e.data1.AsVec();
		global_parent_scale = e.data2.AsVec();
	}
}

void Transform::SetParent(Transform& parent)
{
	global_parent_pos = parent.GetGlobalPosition();
	global_parent_scale = parent.GetGlobalScale();
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
