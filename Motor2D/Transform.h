#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"
#include "Defs.h"

class Transform : public Component
{
public:

	Transform(Gameobject* game_object);
	~Transform();

	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;

	void RecieveEvent(const Event& e) override;

	void SetParent(Transform& parent);

	// Position
	vec		GetLocalPos() const { return pos; }
	float	GetLocalX() const { return pos.x; }
	float	GetLocalY() const { return pos.y; }
	float	GetLocalZ() const { return pos.z; }

	// Scale
	vec		GetLocalScale() const { return scale; }
	float	GetLocalScaleX() const { return scale.x; }
	float	GetLocalScaleY() const { return scale.y; }
	float	GetLocalScaleZ() const { return scale.z; }

	vec GetGlobalPosition() const { return global_parent_pos + pos; }
	vec GetGlobalScale() const { return global_parent_scale * scale; }

	void SetX(float val);
	void SetY(float val);
	void SetZ(float val);
	void MoveX(float val);
	void MoveY(float val);
	void MoveZ(float val);

private:

	vec pos;
	vec scale;

	vec global_parent_pos;
	vec global_parent_scale;

	bool modified = false;
};

#endif // __TRANSFORM_H__