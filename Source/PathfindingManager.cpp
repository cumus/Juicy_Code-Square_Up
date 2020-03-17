#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Application.h"
#include "PathfindingManager.h"

#include "optick-1.3.0.0/include/optick.h"
#include "SDL/include/SDL_scancode.h"

#include <vector>

PathfindingManager::PathfindingManager() : map(NULL), width(0), height(0)
{
	//name.create("pathfinding");
}

// Destructor
PathfindingManager::~PathfindingManager()
{
	//RELEASE_ARRAY(map);
}

bool PathfindingManager::Init()
{
	bool ret = true;

	if (ret)
		LOG("Pathfinding manager initialized!");
	else
		LOG("Pathfinding initialization failed!");

	return ret;
}

// Called before quitting
bool PathfindingManager::CleanUp()
{
	LOG("Freeing pathfinding library");
	
	last_path.clear();//Clear array
	//RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void PathfindingManager::SetMap(int width, int height, char* data)
{
	this->width = width;
	this->height = height;

	//RELEASE_ARRAY(map);
	map = new char[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool PathfindingManager::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true fs the tile is walkable
bool PathfindingManager::IsWalkable(const iPoint& pos) const
{
	char t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
char PathfindingManager::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* PathfindingManager::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
PathNode* PathList::Find(const iPoint& point)
{
	//p2List_item<PathNode>* item = it;
	for (std::list<PathNode>::iterator it = list.begin(); it != list.end(); it++) 
	{
		if (it->pos == point)
			return &*it;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode* PathList::GetNodeLowestScore() 
{
	PathNode* ret = NULL;
	int min = 65535;

	for (std::list<PathNode>::iterator it = list.end(); it != list.end(); it--)
	{
		if (it->Score() < min)
		{
			min = it->Score();
			ret = &*it;
		}
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
int PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	int before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int PathfindingManager::CreatePath(const iPoint& origin, const iPoint& destination)
{
	//BROFILER_CATEGORY("CreatePath", Profiler::Color::Azure)

	/*if (IsWalkable(origin) && !IsWalkable(destination))
	{
		last_path.clear(); //Clear vector

		PathList open, close;
		PathNode originNode(0, origin.DistanceNoSqrt(destination), origin, nullptr);

		originNode.pos = origin;
		open.list.push_back(originNode);

		while ((open.list.size() > 0) && (close.list.size() < MAX_PATH_ITERATIONS))//Gives error
		{
			PathNode* item;

			item = open.GetNodeLowestScore();
			close.list.push_back(*item); //Save lowest node to final list
			open.list.remove(*item); //Delete lowest node from checked list

			if (item->pos != destination)
			{
				PathList adjacentSquares;
				close.list.end()->FindWalkableAdjacents(adjacentSquares);

				for (std::list<PathNode>::iterator it = adjacentSquares.list.begin(); it != adjacentSquares.list.end(); it++)
				{
					if (close.Find(it->pos))
					{
						continue;
					}
					else if (open.Find(it->pos))
					{
						PathNode probable_path = *open.Find(it->pos);
						it->CalculateF(destination);
						if (probable_path.g > it->g) probable_path.parent = it->parent;
					}
					else
					{
						it->CalculateF(destination);
						open.list.push_back(*it);
					}
				}
				adjacentSquares.list.clear();
			}
			else
			{
				for (std::list<PathNode>::iterator it = close.list.end(); it != close.list.begin(); --it)
				{
					last_path.push_back(it->pos);
					//if(it->parent == nullptr)
					//{
						//last_path.push_back(close.list.begin);
					//}
				}
				last_path.swap(last_path);
				return last_path.size();
			}
		}
	}
	else return - 1;*/
	return 0;
}

// ----------------------------------------------------------------------------------
// Example of pathfinding function to find path
// ----------------------------------------------------------------------------------

/*void PathfindTo(int detection_range, iPoint objective) 
{

	//if the player is close we create a path to him
	if (abs(objective.x - position.x) < detection_range))
	{
		iPoint origin = App->map->WorldToMap(position.x, position.y);
		iPoint destination = App->map->WorldToMap(player->position.x, player->position.y);
		App->pathfinding->CreatePath(origin, destination);
		going_after_player = true;
	}
	else { going_after_player = false; }

	//pathfinding debug
	if (going_after_player)
	{
		int x, y;
		SDL_Rect Debug_rect = { 0,0,32,32 };

		path_to_player = App->pathfinding->GetLastPath();

		for (uint i = 0; i < path_to_player->Count(); ++i)
		{
			iPoint pos = App->map->MapToWorld(path_to_player->At(i)->x, path_to_player->At(i)->y);
			Debug_rect.x = pos.x;
			Debug_rect.y = pos.y;
			if (App->collision->debug)App->render->DrawQuad(Debug_rect, 90, 850, 230, 40);
		}
	}
}*/