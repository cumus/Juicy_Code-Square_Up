#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Application.h"
#include "PathfindingManager.h"

#include <vector>


PathfindingManager::PathfindingManager()
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
	//RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void PathfindingManager::SetWalkabilityLayer(const MapLayer& layer)
{
	map = layer;
}

// Utility: return true if pos is inside the map boundaries
bool PathfindingManager::CheckBoundaries(iPoint& pos) 
{
	bool ret = false;
	if ((pos.x >= 0 && pos.x <= map.width) && (pos.y >= 0 && pos.y <= map.height)) ret = true;
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
	if (map.GetID(pos.x,pos.y) == 22 || map.GetID(pos.x, pos.y) == 23) ret = false;
	return ret;
}

// To request all tiles involved in the last generated path
/*std::vector<iPoint>* PathfindingManager::GetLastPath() 
{
	return &finalPath;
}*/


// PathNode -------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------------

PathNode::PathNode() : g(0), h(0), score(0), pos(0, 0), parent(nullptr) 
{}

PathNode::PathNode(iPoint pos, PathNode* parentNode) : pos(pos),parent(parentNode)
{
	if (parentNode != nullptr) g = parent->g + 1;
	else g = 0;
}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos),parent(node.parent)
{}

// ---------------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode PathfindingManager::GetNodeLowestScore(std::vector<PathNode> list)
{
	PathNode ret;
	int min = 0,nodeVectorPos=0;

	for (std::vector<PathNode>::iterator it = list.begin(); it != list.end(); it++)
	{
		if (min == 0)
		{
			min = it->score;
			ret = *it;			
		}
		else
		{
			if (it->score < min) 
			{
				min = it->score;
				ret = *it;
			}
		}
	}
	return ret;
}

//Utility: Returns boolean if found item
bool PathfindingManager::FindItemInVector(std::vector<PathNode>& vec, PathNode node)
{
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->pos == node.pos) return true;
	}
	return false;
}

//Utility: Delete item in vector
void PathfindingManager::RemoveItemInVector(std::vector<PathNode>& vec, PathNode node)
{
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->pos.x == node.pos.x && it->pos.y == node.pos.y)
		{
			vec.erase(it);
			break;
		}
	}
}

//Utility: Returns item of a vector
PathNode* PathfindingManager::GetItemInVector(std::vector<PathNode>& vec, PathNode node)
{
	PathNode* item = new PathNode();
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->pos == node.pos) return item = &*it;
	}
	return item;
}


// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
std::vector<PathNode> PathNode::FindWalkableAdjacents()
{
	std::vector<PathNode> list;
	iPoint cell;

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding.IsWalkable(cell))
		list.push_back(PathNode(cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding.IsWalkable(cell))
		list.push_back(PathNode(cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
		list.push_back(PathNode(cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
		list.push_back(PathNode(cell, this));

	return list;
}


// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
void PathNode::CalculateF(iPoint& destination)
{
	if (parent != nullptr) g = parent->g + 1;
	else g = 0;	
	h = pos.DistanceTo(destination);
	score = g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
std::vector<iPoint> PathfindingManager::CreatePath(iPoint& origin, iPoint& destination)
{
	std::vector<iPoint> finalPath;
	LOG("Origin! X=%d   y=%d",origin.x,origin.y);
	LOG("Destination! X=%d   y=%d", destination.x, destination.y);
	//BROFILER_CATEGORY("CreatePath", Profiler::Color::Azure)


	if (IsWalkable(origin) && IsWalkable(destination))//Give error
	{
		std::vector<PathNode> openList,closedList;
		PathNode originNode(origin, nullptr);
		
		openList.push_back(originNode);
		LOG("Start node added to open list");
		PathNode checkNode;
		while (openList.empty() == false)
		{
			checkNode = openList[0];
			LOG("Start loop");
			
			for (int i = 1; i < openList.size(); i++)
			{
				if (openList[i].score < checkNode.score || openList[i].score == checkNode.score && openList[i].h < checkNode.h)
				{
					checkNode = openList[i];
					LOG("1");
				}
			}
		
			closedList.push_back(checkNode); //Save node to evaluated node
			//openList.pop_back();
			//openList.erase(openList.begin());
			RemoveItemInVector(openList, checkNode);//Remove node from open list
			LOG("2");

			if (checkNode.pos == destination)
			{
				LOG("Destination reached");
				PathNode iteratorNode = closedList.back();
				PathNode auxNode;

				while (iteratorNode.pos != origin)
				{
					LOG("Jump pos.x: %d;pos.y: %d", iteratorNode.pos.x, iteratorNode.pos.y);
					finalPath.push_back(iteratorNode.pos);
					auxNode = *iteratorNode.parent;
					iteratorNode = auxNode;
				}
				LOG("Last position added");

				std::reverse(finalPath.begin(), finalPath.end());
				LOG("Path reversed");
				return finalPath;
			}

			std::vector<PathNode> adjacentCells;
			adjacentCells = checkNode.FindWalkableAdjacents();
			int length = adjacentCells.size();
			LOG("3");
			for (int a = 0; a < length; a++)
			{
				if (FindItemInVector(closedList, adjacentCells[a]) == false)
				{
					LOG("4");
					if (FindItemInVector(openList, adjacentCells[a]) == false)
					{
						LOG("5");
						adjacentCells[a].parent = &checkNode;
						adjacentCells[a].CalculateF(destination);
						openList.push_back(adjacentCells[a]); LOG("6");						
					}
					else
					{
						PathNode* temp = GetItemInVector(openList,adjacentCells[a]);
						if (temp->g < checkNode.g) checkNode.parent = temp;
					}					
				}
			}
			openList;
			adjacentCells.clear();					
		}
	}
	else
	{
		finalPath.push_back(destination);
		LOG("Path created.");
		return finalPath;
	}
}