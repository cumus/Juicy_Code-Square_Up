#include "SDL/include/SDL.h"
#include "QuadTree.h"


Quadtree::Quadtree() : Quadtree(5, 5, 0, {0.0f,0.0f,1920.0f,1080.0f},nullptr)
{}

Quadtree::Quadtree(int maxObj, int maxlvl, int lvl, RectF bounds, Quadtree* p)
{
	maxObjects = maxObj;
	maxLevels = maxlvl;
	level = lvl;
	boundary = bounds;
	parent = p;
	children[0] = nullptr;
	children[1] = nullptr;
	children[2] = nullptr;
	children[3] = nullptr;
}

Quadtree::~Quadtree()
{}

void Quadtree::Init()
{

}

void Quadtree::Clear()
{
	objects.clear();
	for (int i = 0; i < 4; i++)
	{
		if (children[i] != nullptr)
		{
			children[i]->Clear();
			children[i] = nullptr;
		}
	}
}

void Quadtree::Insert(Collider* obj)
{
	if (children[0] != nullptr)
	{
		int index = GetChildIndexForObject(obj->GetColliderBounds());
		if (index != THIS_TREE)
		{
			children[index]->Insert(obj);
			return;
		}
	}
	
	objects.push_back(obj);
	if (objects.size() > maxObjects && level < maxLevels && children[0] != nullptr)
	{
		Split();

		for (std::vector<Collider*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
		{
			int placeIndex = GetChildIndexForObject((*it)->GetColliderBounds());
			if (placeIndex != THIS_TREE)
			{
				children[placeIndex]->Insert(obj);
				objects.erase(it);
			}
		}
	}
}

void Quadtree::Remove(Collider* obj)
{
	int index = GetChildIndexForObject(obj->GetColliderBounds());
	if (index == THIS_TREE || children[index] == nullptr)
	{
		for (std::vector<Collider*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
		{
			if ((*it)->GetID() == obj->GetID())
			{
				objects.erase(it);
				break;
			}
		}
	}
	else
	{
		return children[index]->Remove(obj);
	}
}

std::vector<Collider*> Quadtree::Search(Collider& obj)
{
	std::vector<Collider*> overlaps;
	std::vector<Collider*> list;
	Search(obj, overlaps);

	for (std::vector<Collider*>::const_iterator it = overlaps.cbegin(); it != overlaps.cend(); ++it)
	{
		if (obj.Intersects((*it)).colliding)
		{
			list.push_back((*it));
		}
	}

	return list;
}

void Quadtree::Search(Collider& obj, std::vector<Collider*>& list)
{
	list.insert(list.end(),objects.begin(),objects.end());
	if (children[0] != nullptr)
	{
		int index = GetChildIndexForObject(obj.GetColliderBounds());
		if(index == THIS_TREE)
		{
			for (int i = 0; i < 4; i++)
			{
				if (children[i]->Intersects(obj.GetColliderBounds()))
				{
					children[i]->Search(obj, list);
				}
			}
		}
		else
		{
			children[index]->Search(obj,list);
		}
	}
}

bool Quadtree::Intersects(const RectF objective)
{
	bool ret = false;
	const RectF coll = GetBounds();
	if (objective.x - objective.w > coll.x + coll.w ||
		objective.x + objective.w < coll.x - coll.w ||
		objective.y - objective.h > coll.y + coll.h ||
		objective.y + objective.h < coll.y - coll.h) //Intersects
	{
		ret = true;
	}
	return ret;
}


void Quadtree::Split()
{
	const float childWidth = boundary.w / 2;
	const float childHeight = boundary.h / 2;

	children[CHILD_NE] = new Quadtree(maxObjects, maxLevels, level + 1, { boundary.x + childWidth, boundary.y, childWidth, childHeight },this);
	children[CHILD_NW] = new Quadtree(maxObjects, maxLevels, level + 1,{ boundary.x, boundary.y, childWidth, childHeight },this);
	children[CHILD_SW] = new Quadtree(maxObjects, maxLevels, level + 1,{ boundary.x, boundary.y + childHeight, childWidth, childHeight }, this);
	children[CHILD_SE] = new Quadtree(maxObjects, maxLevels, level + 1, { boundary.x + childWidth, boundary.y + childHeight, childWidth, childHeight }, this);
}


int Quadtree::GetChildIndexForObject(const RectF& objBound)
{
	int index = THIS_TREE;
	float verticalDividingLine = boundary.x + boundary.w * 0.5f;
	float horizontalDividingLine = boundary.y + boundary.h * 0.5f;

	bool north = objBound.y < horizontalDividingLine && (objBound.h + objBound.y < horizontalDividingLine);
	bool south = objBound.y > horizontalDividingLine;
	bool west = objBound.x < verticalDividingLine && (objBound.x + objBound.w < verticalDividingLine);
	bool east = objBound.x > verticalDividingLine;

	if (east)
	{
		if (north) index = CHILD_NE;		
		else if (south) index = CHILD_SE;		
	}
	else if (west)
	{
		if (north) index = CHILD_NW;		
		else if (south) index = CHILD_SW;		
	}

	return index;
}