#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "Component.h"
#include "SDL/include/SDL.h"

#include <vector>

enum ColliderType
{
	NON_TRIGGER,
	TRIGGER,
};

enum ColliderShape
{
	SQUARE,
	CIRCLE,
};

enum CollisionLayer
{
	DEFAULT_LAYER = 0,
	SCENE_LAYER,
	HUD_LAYER,
	INPUT_LAYER,

	MAX_COLLISION_LAYERS,
};

enum ColliderTag
{
	DEFAULT_TAG = 0,
	PLAYER_TAG,
	PLAYER_VISION_TAG,
	PLAYER_ATTACK_TAG,
	ENEMY_TAG,
	ENEMY_VISION_TAG,
	ENEMY_ATTACK_TAG,
	UI_TAG,
	BACKGROUND_TAG,
	BUILDING_TAG,
};

struct Manifold
{
	bool colliding = false;
	const RectF* other;
};


class Collider : public Component
{
public:
	Collider(Gameobject* game_object, RectF coll, ColliderType t=NON_TRIGGER, ColliderTag tag = DEFAULT_TAG,ComponentType type = COLLIDER);
	~Collider();

	
	Manifold Intersects(Collider* other);
	void ResolveOverlap(Manifold& m);

	void SetLayer(CollisionLayer lay) { layer = lay; }
	CollisionLayer GetCollLayer() { return layer; }	
	void SetColliderBounds(RectF& rect) { boundary = rect; }
	RectF& GetColliderBounds() { return boundary; }
	void SetOffset(RectF off) { offset = off; }
	void SetCollType(ColliderType t) { collType = t; }
	ColliderType GetCollType() { return collType; }
	void SetColliderTag(ColliderTag tg) { tag = tg; }
	ColliderTag GetColliderTag() { return tag; }
	void SaveCollision(double ID);
	bool GetCollisionState(double ID);
	void DeleteCollision(double ID);

private:
	void SetPosition();

private:
	RectF boundary;
	RectF offset;
	CollisionLayer layer;
	ColliderType collType;
	ColliderTag tag;
	std::vector<double> collisions;
};

#endif // !__COLLIDER_H__