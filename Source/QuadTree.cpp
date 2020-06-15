#include "SDL/include/SDL.h"
#include "QuadTree.h"
#include "Application.h"
#include "Render.h"
#include "JuicyMath.h"
#include "Log.h"


Quadtree::Quadtree() : Quadtree(20, 1, 0, {0,0,1920,1080},nullptr)
{}

Quadtree::Quadtree(int maxObj, int maxlvl, int lvl, SDL_Rect bounds, Quadtree* p)
{
	maxObjects = maxObj;
	maxLevels = maxlvl;
	level = lvl;
	boundary = bounds;	
	parent = p;
	if (parent == nullptr) boundary.x -= boundary.w/2;
	children[0] = nullptr;
	children[1] = nullptr;
	children[2] = nullptr;
	children[3] = nullptr;
}

Quadtree::~Quadtree()
{}

void Quadtree::Init(int maxObj, int maxlvl, int lvl, SDL_Rect bounds, Quadtree* p)
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
			DEL(children[i]);
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
	SDL_Rect quad = GetBounds();
	App->render->DrawQuad(SDL_Rect({ quad.x,quad.y,quad.w,quad.h }), { 255,0,0,255 }, false, DEBUG_SCENE, true);
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
	if (IntersectBounds(*obj))
	{
		if (level >= maxLevels)
		{
			objects.push_back(obj);
		}
		else if (children[0] != nullptr)//Got childs
		{
			children[0]->Insert(obj);
			children[1]->Insert(obj);
			children[2]->Insert(obj);
			children[3]->Insert(obj);
		}
		else if (objects.size() < maxObjects) //No childs
		{
			objects.push_back(obj);
		}
		else
		{
			Split();
			objects.push_back(obj);
			for (std::vector<Collider*>::iterator it = objects.begin(); it != objects.end(); ++it)
			{
				children[0]->Insert(*it);
				children[1]->Insert(*it);
				children[2]->Insert(*it);
				children[3]->Insert(*it);
			}
			objects.clear();
		}
	}
}


void Quadtree::Remove(Collider* obj)
{
	if (children[0 != nullptr])
	{
		children[0]->Remove(obj);
		children[1]->Remove(obj);
		children[2]->Remove(obj);
		children[3]->Remove(obj);
	}
	else
	{
		if (IntersectBounds(*obj))//Inside quad
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
	}
}

std::vector<Collider*> Quadtree::Search(Collider& obj)
{
	std::vector<Collider*> overlaps;
	Search(obj, overlaps);
	return overlaps;
}

void Quadtree::Search(Collider& obj, std::vector<Collider*>& list)
{
	if (IntersectBounds(obj))//Inside quad
	{
		if (children[0] != nullptr)//Got childs
		{
			children[0]->Search(obj, list);
			children[1]->Search(obj, list);
			children[2]->Search(obj, list);
			children[3]->Search(obj, list);

		}
		else//No childs -> last quad
		{
			for (std::vector<Collider*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
				list.push_back(*it);
		}
	}
}

void Quadtree::Split()
{
	int childWidth = boundary.w / 2;
	int childHeight = boundary.h / 2;

	children[CHILD_NW] = new Quadtree(maxObjects, maxLevels, level + 1, { boundary.x, boundary.y, childWidth, childHeight },this);
	children[CHILD_NE] = new Quadtree(maxObjects, maxLevels, level + 1,{ boundary.x+childWidth, boundary.y, childWidth, childHeight },this);
	children[CHILD_SW] = new Quadtree(maxObjects, maxLevels, level + 1,{ boundary.x, boundary.y + childHeight, childWidth, childHeight }, this);
	children[CHILD_SE] = new Quadtree(maxObjects, maxLevels, level + 1, { boundary.x + childWidth, boundary.y + childHeight, childWidth, childHeight }, this);
}

bool Quadtree::IntersectBounds(Collider coll)
{
	//LOG("Point X:%f/Y:%f", coll.top.first, coll.top.second);
	//LOG("Bounds X:%d/Y:%d/W:%d/H:%d", boundary.x, boundary.y, boundary.x + boundary.w, boundary.y + boundary.h);
	SDL_Rect quadTreeRect = coll.GetColliderBounds();

	if (quadTreeRect.x > boundary.x + boundary.w || quadTreeRect.x+ quadTreeRect.w < boundary.x || quadTreeRect.y > boundary.y + boundary.h || quadTreeRect.y+ quadTreeRect.h < boundary.y) return false;
	else return true;

	/*if (int(coll.top.first) > boundary.x && int(coll.top.first) < boundary.x + boundary.w && int(coll.top.second) > boundary.y && int(coll.top.second) < boundary.y + boundary.h) { return true; }
	if (int(coll.bot.first) > boundary.x && int(coll.bot.first) < boundary.x + boundary.w && int(coll.bot.second) > boundary.y && int(coll.bot.second) < boundary.y + boundary.h) {  return true; }
	if (int(coll.right.first) > boundary.x && int(coll.right.first) < boundary.x + boundary.w && int(coll.right.second) > boundary.y && int(coll.right.second) < boundary.y + boundary.h) { return true; }
	if (int(coll.left.first) > boundary.x && int(coll.left.first) < boundary.x + boundary.w && int(coll.left.second) > boundary.y && int(coll.left.second) < boundary.y + boundary.h) { return true; }*/
}