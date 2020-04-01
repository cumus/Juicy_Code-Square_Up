#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

class Transform : public Component
{
public:

	Transform(Gameobject* game_object);
	~Transform();

	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;

	void RecieveEvent(const Event& e) override;

	void SetParent(Transform* parent);

	// Local Position
	vec		GetLocalPos() const { return pos; }
	float	GetLocalX() const { return pos.x; }
	float	GetLocalY() const { return pos.y; }
	float	GetLocalZ() const { return pos.z; }

	void SetLocalPos(const vec& p);
	void SetX(float val);
	void SetY(float val);
	void SetZ(float val);
	void MoveX(float val);
	void MoveY(float val);
	void MoveZ(float val);

	// Local Scale
	vec		GetLocalScale() const { return scale; }
	float	GetLocalScaleX() const { return scale.x; }
	float	GetLocalScaleY() const { return scale.y; }
	float	GetLocalScaleZ() const { return scale.z; }

	// Global values
	vec GetGlobalPosition() const { return global_parent_pos + pos; }
	vec GetGlobalScale() const { return global_parent_scale * scale; }

	bool GlobalPosIsDifferentFrom(vec global_pos) const;
	void ResetAABB();

	// Collision
	bool Intersects(std::pair<float, float> p) const;

private:

	vec pos;
	vec scale;

	vec global_parent_pos;
	vec global_parent_scale;

	std::pair<int, int> points[8];

	bool modified = false;
};

#endif // __TRANSFORM_H__