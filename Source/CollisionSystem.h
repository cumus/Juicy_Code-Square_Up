#ifndef __COLLISIONSYSTEM_H__
#define __COLLISIONSYSTEM_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Gameobject.h"
#include "Collider.h"
#include "QuadTree.h"

#include <vector>
#include <map>

enum CollisionLayer
{
	DEFAULT_LAYER = 0,
	SCENE_LAYER,
	HUD_LAYER,
	INPUT_LAYER,

	MAX_COLLISION_LAYERS,
};

class CollisionSystem
{
public:
	CollisionSystem();
	~CollisionSystem();

	void Add(std::vector<Gameobject*>& objects);
	void ProcessRemovals();
	void Update();
	void SetLayerCollision(CollisionLayer one, CollisionLayer two, bool collide);
	

private:
	void Resolve();
	void ProcessCollisions(std::vector<Gameobject*>& first, std::vector<Gameobject*>& second);

private:
	bool collisionLayers[MAX_COLLISION_LAYERS][MAX_COLLISION_LAYERS]; //Store layers collisions
	std::map<CollisionLayer, std::vector<Collider*>> layerColliders;
	//Quadtree collisionTree;
};


#endif // !__COLLISIONSYSTEM_H__