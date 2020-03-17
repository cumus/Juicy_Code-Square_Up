#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Application.h"
#include "PathfindingManager.h"

#include <vector>
#include <algorithm>

PathfindingManager::PathfindingManager() : width(0), height(0)
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
	
	finalPath.clear();//Clear array
	//RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void PathfindingManager::SetMap(MapLayer layer)
{
	map = layer;
}

// Utility: return true if pos is inside the map boundaries
bool PathfindingManager::CheckBoundaries(iPoint& pos) 
{
	bool ret = false;
	if ((pos.x >= 0 && pos.x <= width) && (pos.y >= 0 && pos.y <= height)) ret = true;
	return ret;
}

// Utility: returns true if the tile is walkable
bool PathfindingManager::IsWalkable(iPoint& pos) 
{
	bool ret = false;
	if (CheckBoundaries(pos))
	{
		ret = GetTileAt(pos);
	}
	return ret;
}

// Utility: return the walkability value of a tile
bool PathfindingManager::GetTileAt(iPoint& pos) 
{
	bool ret = true;
	if (map.GetID(pos.x,pos.y) > 22) ret = false;	
	return ret;
}

// To request all tiles involved in the last generated path
std::vector<iPoint>* PathfindingManager::GetLastPath() 
{
	return &finalPath;
}


// PathNode -------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------------

PathNode::PathNode() : g(0), h(0), score(0), pos(0, 0), parent(nullptr) 
{}

PathNode::PathNode(iPoint pos, PathNode* parent) : pos(pos), parent(parent)
{
	if (parent != nullptr) g = parent->g + 1;
	else g = 0;
}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// ---------------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode* PathfindingManager::GetNodeLowestScore(std::vector<PathNode> &list, iPoint& destination)
{
	PathNode* ret = nullptr;
	int min = 0,nodeVectorPos=0;

	for (std::vector<PathNode>::iterator it = list.begin(); it != list.end(); it++)
	{
		it->CalculateF(destination);
		if (min == 0)
		{
			min = it->score;
			ret = &*it;			
		}
		else
		{
			if (it->score < min) 
			{
				min = it->score;
				ret = &*it;
			}
		}
	}
	return ret;
}

bool PathfindingManager::FindItemInVector(std::vector<PathNode>& vec, PathNode node)
{
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->pos == node.pos) return true;
	}
	return false;
}

void PathfindingManager::RemoveItemInVector(std::vector<PathNode>& vec, PathNode node)
{
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->pos == node.pos)
		{
			vec.erase(it);
			break;
		}
	}
}


// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
void PathNode::FindWalkableAdjacents(std::vector<PathNode> &list_to_fill) 
{
	iPoint cell;

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.push_back(PathNode(cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.push_back(PathNode(cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.push_back(PathNode(cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
		list_to_fill.push_back(PathNode(cell, this));
}


// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
void PathNode::CalculateF(iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);
	score = g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
std::vector<iPoint> PathfindingManager::CreatePath(iPoint& origin, iPoint& destination)
{
	//BROFILER_CATEGORY("CreatePath", Profiler::Color::Azure)
	finalPath.clear(); //Clear vector

	if (IsWalkable(origin) && IsWalkable(destination))//Give error
	{
		std::vector<PathNode> openList,closedList;
		PathNode originNode(origin, nullptr);

		openList.push_back(originNode);
		
		while (openList.empty() == false)
		{
			PathNode* checkNode;
			checkNode = GetNodeLowestScore(openList,destination);
			closedList.push_back(*checkNode); //Save lowest node to final list
			RemoveItemInVector(openList, *checkNode);//Remove node from vector
			
			if (checkNode->pos != destination)
			{
				std::vector<PathNode> adjacentCells;
				closedList.back().FindWalkableAdjacents(adjacentCells);

				for (std::vector<PathNode>::iterator it = adjacentCells.begin(); it != adjacentCells.end(); it++)
				{
					if(FindItemInVector(closedList,*it) == false)
					{
						if (FindItemInVector(openList, *it) == true)
						{
							PathNode probable_path = *it;
							probable_path.CalculateF(destination);
							if (probable_path.g > it->g) probable_path.parent = it->parent;
						}
						else
						{
							it->CalculateF(destination);
							openList.push_back(*it);
						}
					}
				}
				adjacentCells.clear();
			}
			else
			{
				//destinationReached = true;
				std::vector<PathNode>::iterator it = closedList.end();
				finalPath.push_back(it->pos); //Destination node
				do
				{			
					if (it->parent!=nullptr)
					{
						finalPath.push_back(it->parent->pos);
					}
					else
					{
						finalPath.push_back(closedList.front().pos);
					}
					--it;
				} while (it->parent != nullptr);
				std::reverse(finalPath.begin(),finalPath.end());
				return finalPath;
			}
		}
	}
	else
	{
		finalPath.push_back(destination);
		return finalPath;
	}
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