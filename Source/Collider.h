#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "SDL/include/SDL.h"
#include "Component.h"

enum CollisionLayer
{
	DEFAULT = 0,
	GAMEOBJECTS,
	TILEMAP,
};

struct Manifold
{
	bool colliding = false;
	const RectF* other;
};

class Collider : public Component
{
public:
	Collider(Gameobject* game_object, RectF coll, ComponentType type = COLLIDER);
	~Collider();

	
	Manifold Intersects(Collider* other);
	void ResolveOverlap(Manifold& m);

	void SetLayer(CollisionLayer lay) { layer = lay; }
	CollisionLayer GetCollLayer() { return layer; }	
	void SetColliderBounds(RectF& rect) { boundary = rect; }
	RectF& GetColliderBounds() { return boundary; }
	void SetOffset(RectF off) { offset = off; }

private:
	void SetPosition();

private:
	RectF boundary;
	RectF offset;
	CollisionLayer layer;
};

#endif