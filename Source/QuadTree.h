#ifndef __QUADTREEH__
#define __QUADTREE_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Gameobject.h"
#include "Collider.h"

#include <vector>

#define THIS_TREE -1
#define CHILD_NE 0
#define CHILD_NW 1
#define CHILD_SE 2
#define CHILD_SW 3

class Quadtree
{
public:
	Quadtree();
	Quadtree(int maxObj,int maxlvl,int lvl, RectF bounds,Quadtree* parent);
	~Quadtree();

	void Init();
	void Clear();
	void Insert(Collider* obj);
	void Remove(Collider* obj);
	std::vector<Collider*> Search(Collider& obj);
	RectF GetBounds() { return boundary; }

private:
	void Split();
	void Search(Collider& obj, std::vector<Collider*>& overlap);
	int GetChildIndexForObject(const RectF& objBound);
	bool Intersects(const RectF objective);

private:
	int maxObjects;
	int maxLevels;
	Quadtree* parent;
	Quadtree* children[4];
	std::vector<Collider*> objects;
	int level;
	RectF boundary;
};

#endif