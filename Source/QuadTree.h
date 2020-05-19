#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Gameobject.h"
#include "Collider.h"

#include <vector>

#define THIS_TREE -1
#define CHILD_NW 0
#define CHILD_NE 1
#define CHILD_SW 2
#define CHILD_SE 3

class Quadtree
{
public:
	Quadtree();
	Quadtree(int maxObj,int maxlvl,int lvl, SDL_Rect bounds,Quadtree* parent);
	~Quadtree();

	void Init(int maxObj, int maxlvl, int lvl, SDL_Rect bounds, Quadtree* p);
	void Clear();
	void Insert(Collider* obj);
	void Remove(Collider* obj);
	bool IntersectBounds(Collider coll);
	std::vector<Collider*> Search(Collider& obj);
	SDL_Rect GetBounds() { return boundary; }
	std::vector<Quadtree*> GetChilds();
	bool GotChilds();
	void DebugDrawBounds();
	void Split();
	//std::vector<Collider*> SearchSelection(std::pair<int,int> point);

private:
	
	void Search(Collider& obj, std::vector<Collider*>& overlap);
	//void SearchSelection(std::pair<int, int> point, std::vector<Collider*>& overlap);
	//int GetChildIndexForObject(const IsoLinesCollider& objBound);
	//int GetChildIndexForObject(std::pair<int,int> point);

private:
	int maxObjects;
	int maxLevels;
	Quadtree* parent;
	Quadtree* children[4];
	std::vector<Collider*> objects;
	int level;
	SDL_Rect boundary;
};

#endif // !__QUADTREE_H__