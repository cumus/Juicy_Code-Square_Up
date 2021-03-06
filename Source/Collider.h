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

enum CollisionLayer : int
{
	DEFAULT_COLL_LAYER = 0,
	SCENE_COLL_LAYER,
	BODY_COLL_LAYER,
	VISION_COLL_LAYER,
	ATTACK_COLL_LAYER,

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
};

struct IsoLinesCollider
{
	std::pair<float, float> left;
	std::pair<float, float> right;
	std::pair<float, float> bot;
	std::pair<float, float> top;
};

struct Manifold
{
	bool colliding = false;
	SDL_Rect otherColl;
	float overX, overY;
};

class Collider : public Component
{
public:
	Collider(Gameobject* game_object, RectF coll, ColliderType t = NON_TRIGGER, ColliderTag tag = DEFAULT_TAG, RectF offset = {0,0,0,0},CollisionLayer layer = SCENE_COLL_LAYER, ComponentType type = COLLIDER);
	~Collider();
	
	Manifold Intersects(Collider* other);
	void ResolveOverlap(Manifold& m);

	void SetLayer(CollisionLayer lay);
	CollisionLayer GetCollLayer();
	void SetColliderBounds(RectF& rect);
	SDL_Rect GetColliderBounds();
	void SetOffset(RectF off);
	void SetCollType(ColliderType t);
	ColliderType GetCollType();
	void SetColliderTag(ColliderTag tg);
	ColliderTag GetColliderTag();
	void DeleteCollision(double ID);
	void SetPosition();
	IsoLinesCollider GetIsoPoints();
	double GetGoID();
	void SetPointsOffset(std::pair<float,float> top,std::pair<float,float> bot,std::pair<float,float> right,std::pair<float,float> left);

public:

	Gameobject* parentGo;

private:

	RectF boundary;
	RectF offset;
	CollisionLayer layer;
	ColliderType collType;
	ColliderTag tag;
	std::vector<double> collisions;
	std::pair<float, float> tileSize;
	IsoLinesCollider isoDraw;
	double GoID;
	std::pair<float, float> topOffset;
	std::pair<float, float> botOffset;
	std::pair<float, float> rightOffset;
	std::pair<float, float> leftOffset;
};

#endif // !__COLLIDER_H__