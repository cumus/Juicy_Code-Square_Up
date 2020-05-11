#include "SDL/include/SDL.h"
#include "QuadTree.h"
#include "Application.h"
#include "Render.h"
#include "Log.h"


Quadtree::Quadtree() : Quadtree(20, 5, 0, {0.0f,0.0f,1920.0f,1080.0f},nullptr)
{}

Quadtree::Quadtree(int maxObj, int maxlvl, int lvl, RectF bounds, Quadtree* p)
{
	maxObjects = maxObj;
	maxLevels = maxlvl;
	level = lvl;
	boundary = bounds;	
	parent = p;
	if (parent == nullptr) boundary.x -= 9100;
	children[0] = nullptr;
	children[1] = nullptr;
	children[2] = nullptr;
	children[3] = nullptr;
}

Quadtree::~Quadtree()
{}

void Quadtree::Init(int maxObj, int maxlvl, int lvl, RectF bounds, Quadtree* p)
{
	maxObjects = maxObj;
	maxLevels = maxlvl;
	level = lvl;
	boundary = bounds;
	parent = p;
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

std::vector<Quadtree*> Quadtree::GetChilds()
{
	std::vector<Quadtree*> childs;
	if (GotChilds())
	{
		childs.push_back(children[0]);
		childs.push_back(children[1]);
		childs.push_back(children[2]);
		childs.push_back(children[3]);
	}
	return childs;
}

bool Quadtree::GotChilds()
{
	if (children[0] != nullptr) return true;
	else return false;
}

void Quadtree::DebugDrawBounds()
{
	RectF quad = GetBounds();
	//LOG("Bounds X:%f/Y:%f/W:%f/H:%f", boundary.x, boundary.y, boundary.w, boundary.h);
	App->render->DrawQuad(SDL_Rect({ int(quad.x),int(quad.y),int(quad.w),int(quad.h) }), { 255,0,0,255 }, false, DEBUG_SCENE, true);
	if (GotChilds())
	{
		children[0]->DebugDrawBounds();
		children[1]->DebugDrawBounds();
		children[2]->DebugDrawBounds();
		children[3]->DebugDrawBounds();
	}
}

void Quadtree::Insert(Collider* obj)
{
	if (children[0] != nullptr)
	{
		int index = GetChildIndexForObject(obj->GetIsoPoints());
		if (index != THIS_TREE)
		{
			children[index]->Insert(obj);
			return;
		}
	}
	
	objects.push_back(obj);
	if (objects.size() > maxObjects && level < maxLevels && children[0] == nullptr)
	{
		//LOG("Quad full");
		Split();

		for (std::vector<Collider*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
		{
			int placeIndex = GetChildIndexForObject((*it)->GetIsoPoints());
			if (placeIndex != THIS_TREE)
			{
				children[placeIndex]->Insert(obj);
			}
		}
		objects.clear();
	}
}

void Quadtree::Remove(Collider* obj)
{
	int index = GetChildIndexForObject(obj->GetIsoPoints());
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

	//LOG("Found colliders size: %d", overlaps.size());
	/*for (std::vector<Collider*>::const_iterator it = overlaps.cbegin(); it != overlaps.cend(); ++it)
	{
		Manifold m = obj.Intersects(*it);
		if (m.colliding)
		{
			LOG("Got intersection");
			list.push_back((*it));
		}
	}*/
	//LOG("Final colliders list: %d", overlaps.size());
	return overlaps;
}

void Quadtree::Search(Collider& obj, std::vector<Collider*>& list)
{
	/*for (std::vector<Collider*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
	{
		list.push_back(*it);
	}*/
	
	if (children[0] != nullptr)
	{
		int index = GetChildIndexForObject(obj.GetIsoPoints());
		if(index == THIS_TREE)
		{
			for (int i = 0; i < 4; i++)
			{
				if (children[i]->IntersectsQuad(obj.GetIsoPoints()))
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
	else
	{
		for (std::vector<Collider*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
		{
			list.push_back(*it);
		}
	}
}

bool Quadtree::IntersectsQuad(const IsoLinesCollider objective)
{
	bool ret = false;
	const RectF coll = GetBounds();
//	LOG("Quad intersect obj X:%f/Y:%f",objective.x,objective.y);
	/*fPoint topRight1(coll.x + coll.w, coll.y);
	fPoint topRight2(objective.x + objective.w, objective.y);
	fPoint bottomLeft1(coll.x, coll.y + coll.h);
	fPoint bottomLeft2(objective.x, objective.y + objective.h);

	if (topRight1.y < bottomLeft2.y || bottomLeft1.y > topRight2.y ||
		topRight1.x < bottomLeft2.x || bottomLeft1.x > topRight2.x) //Non colliding
	{
		ret = false;
	}*/
	fPoint top(objective.top.first,objective.top.second);
	fPoint bot(objective.bot.first, objective.bot.second);
	fPoint left(objective.left.first, objective.left.second);
	fPoint right(objective.right.first, objective.right.second);

	if ((top.y > coll.y && top.y < coll.y+coll.h) ||
		(bot.y < coll.y + coll.h && bot.y > coll.y) ||
		(left.x > coll.x && left.x < coll.x+coll.w) || 
		(right.x < coll.x+coll.w && right.x > coll.x)) //Colliding
	{
		ret = true;
	}
	return ret;
}


void Quadtree::Split()
{
	float childWidth = boundary.w / 2;
	float childHeight = boundary.h / 2;

	children[CHILD_NW] = new Quadtree(maxObjects, maxLevels, level + 1, { boundary.x, boundary.y, childWidth, childHeight },this);
	children[CHILD_NE] = new Quadtree(maxObjects, maxLevels, level + 1,{ boundary.x+childWidth, boundary.y, childWidth, childHeight },this);
	children[CHILD_SW] = new Quadtree(maxObjects, maxLevels, level + 1,{ boundary.x, boundary.y + childHeight, childWidth, childHeight }, this);
	children[CHILD_SE] = new Quadtree(maxObjects, maxLevels, level + 1, { boundary.x + childWidth, boundary.y + childHeight, childWidth, childHeight }, this);
}


int Quadtree::GetChildIndexForObject(const IsoLinesCollider& objBound)
{
	int index = THIS_TREE;
	float verticalDividingLine = boundary.x + boundary.w * 0.5f;
	float horizontalDividingLine = boundary.y + boundary.h * 0.5f;

	/*bool north = objBound.y < horizontalDividingLine && (objBound.h + objBound.y < horizontalDividingLine);
	bool south = objBound.y > horizontalDividingLine;
	bool west = objBound.x < verticalDividingLine && (objBound.x + objBound.w < verticalDividingLine);
	bool east = objBound.x > verticalDividingLine;*/
	bool north = objBound.bot.second < horizontalDividingLine;
	bool south = objBound.top.second > horizontalDividingLine;
	bool west = objBound.right.first < verticalDividingLine;
	bool east = objBound.left.first > verticalDividingLine;

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
