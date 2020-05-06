#ifndef __COLLISIONSYSTEM_H__
#define __COLLISIONSYSTEM_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Gameobject.h"
#include "Collider.h"
#include "QuadTree.h"

#include <vector>
#include <map>


class CollisionSystem
{
public:
	CollisionSystem();
	~CollisionSystem();

	void Add(Gameobject* obj);
	void Add(Collider* coll);
	void Add(std::vector<Gameobject*>& objects);
	void ProcessRemovals();
	void ProcessRemovals(Gameobject* obj);
	void Update();
	void SetLayerCollision(CollisionLayer one, CollisionLayer two, bool collide);
	void SetDebug();
	void Clear();
	

private:
	void Resolve();
	//void ProcessCollisions();

private:
	bool collisionLayers[MAX_COLLISION_LAYERS][MAX_COLLISION_LAYERS]; //Store layers collisions
	std::map<CollisionLayer, std::vector<Collider*>> layerColliders;
	Quadtree* collisionTree;
	bool debug;
};


#endif // !__COLLISIONSYSTEM_H__