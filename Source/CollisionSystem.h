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
	void ProcessRemovals(double id);
	void DeleteCollider(Collider coll);
	void Update();
	void SetLayerCollision(CollisionLayer one, CollisionLayer two, bool collide);
	void SetDebug();
	void Clear();
	Quadtree* GetQuadTree();

private:
	void Resolve();

private:
	bool collisionLayers[MAX_COLLISION_LAYERS][MAX_COLLISION_LAYERS]; //Store layers collisions
	//std::map<CollisionLayer, std::vector<Collider*>> layerColliders;
	std::vector<Collider*> layerColliders[MAX_COLLISION_LAYERS];
	Quadtree* collisionTree;
	bool debug;
};


#endif // !__COLLISIONSYSTEM_H__