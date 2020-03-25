#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Application.h"
#include "PathfindingManager.h"
#include "optick-1.3.0.0/include/optick.h"

#include <vector>
#include <algorithm>


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
	bool ret = false;
	//LOG("Tile ID--%d", map.GetID(pos.x, pos.y));
	//if (map.GetID(pos.x,pos.y) == 22 || map.GetID(pos.x, pos.y) == 23) ret = false;
	if (map.GetID(pos.x, pos.y) == 0) ret = true;
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

PathNode::PathNode() : g(0), h(0), score(0), pos(0, 0), parentPos(iPoint({-1,-1}))
{}

PathNode::PathNode(iPoint nodePos, iPoint parent) : pos(nodePos), parentPos(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), score(node.score), pos(node.pos),parentPos(node.parentPos)
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
bool PathfindingManager::FindItemInVector(std::vector<PathNode> vec, PathNode node)
{
	for (std::vector<PathNode>::const_iterator it = vec.cbegin(); it != vec.cend(); ++it)
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
PathNode PathfindingManager::GetItemInVector(std::vector<PathNode>& vec, iPoint nodePos)
{
	PathNode item;
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (it->pos == nodePos) return item = *it;
	}
	return item;
}

//Utility: Get lowest score node in vector 
PathNode PathfindingManager::GetLowestScoreNode(std::vector<PathNode>& vec)
{
	PathNode item;
	int minScore = 0;
	for (std::vector<PathNode>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (minScore == 0) { item = *it; }
		if (it->score < item.score) item = *it;
	}
	return item;
}

//Recursive sort
void PathfindingManager::VectorQuicksort(std::vector<PathNode>& vec,int L, int R)
{
	int j = R;
	int i = L;
	int mid = i + (R - i) / 2;
	PathNode piv = vec[mid];
	PathNode temp;


	while (i<R || j>0) 
	{
		while (vec[i].score < piv.score)
			i++;
		while (vec[j].score > piv.score)
			j--;

		if (i <= j) 
		{
			temp = vec[i];
			vec[i] = vec[j];
			vec[j] = temp;
			i++;
			j--;
		}
		else 
		{
			if (i < R)
				VectorQuicksort(vec, i, R);
			if (j > L)
				VectorQuicksort(vec, L, j);
			return;
		}
	}
}

//Iterative sort
void PathfindingManager::VectorMergesort(std::vector<PathNode>& vec, int length)
{
	int curr_size;
	int left_start; 

	for (curr_size = 1; curr_size <= length - 1; curr_size = 2 * curr_size)
	{
		for (left_start = 0; left_start < length - 1; left_start += 2 * curr_size)
		{
			int mid = min(left_start + curr_size - 1, length - 1);
			int right_end = min(left_start + 2 * curr_size - 1, length - 1);
			Merge(vec, left_start, mid, right_end);
		}
	}
}

void PathfindingManager::Merge(std::vector<PathNode>& vec, int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;

	PathNode* L = new PathNode[n1];
	PathNode* R = new PathNode[n2];


	for (i = 0; i < n1; i++)
		L[i] = vec[l + i];
	for (j = 0; j < n2; j++)
		R[j] = vec[m + 1 + j];

	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2)
	{
		if (L[i].score <= R[j].score)
		{
			vec[k] = L[i];
			i++;
		}
		else
		{
			vec[k] = R[j];
			j++;
		}
		k++;
	}
	while (i < n1)
	{
		vec[k] = L[i];
		i++;
		k++;
	}
	while (j < n2)
	{
		vec[k] = R[j];
		j++;
		k++;
	}
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
	{
		list.push_back(PathNode(cell, this->pos));
	}

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	return list;
}


// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
void PathNode::CalculateF(iPoint destination)
{
	h = pos.DistanceTo(destination);
	score = g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
std::vector<iPoint> PathfindingManager::CreatePath(iPoint& origin, iPoint& destination)
{
	OPTICK_EVENT();
	std::vector<iPoint> finalPath;
	//LOG("Origin! X=%d   y=%d",origin.x,origin.y);
	//LOG("Destination! X=%d   y=%d", destination.x, destination.y);
	//BROFILER_CATEGORY("CreatePath", Profiler::Color::Azure)


	if (IsWalkable(destination))
	{
		int loops = 0;
		std::vector<PathNode> openList,closedList;
		PathNode originNode(origin, nullPoint);
		originNode.g = 0;
		originNode.CalculateF(destination);
		
		openList.push_back(originNode);
		LOG("Start node added to open list");

		PathNode checkNode;
		while (openList.empty() == false)
		{
			loops++;
			VectorQuicksort(openList, 0, openList.size() - 1);
			//VectorMergesort(openList,openList.size());
			checkNode = openList.front();
			//LOG("Start loop");
			
			/*for (int i = 1; i < openList.size(); i++)
			{
				if (openList[i].score < checkNode.score || openList[i].score == checkNode.score && openList[i].h < checkNode.h)
				{
					checkNode = openList[i];
					LOG("1");
				}
			}*/
		
			closedList.push_back(checkNode); //Save node to evaluated list
			openList.erase(openList.begin());
			//LOG("2");

			if (checkNode.pos == destination)
			{
				LOG("Destination reached");
				PathNode iteratorNode = closedList.back();

				while (iteratorNode.pos != origin)
				{
					//LOG("Jump pos.x: %d;pos.y: %d", iteratorNode.pos.x, iteratorNode.pos.y);
					finalPath.push_back(iteratorNode.pos);
					iteratorNode = GetItemInVector(closedList,iteratorNode.parentPos);
				}
				//LOG("Last position added");

				std::reverse(finalPath.begin(), finalPath.end());
				LOG("Path reversed");
				LOG("Loops done: %d", loops);
				return finalPath;
			}

			std::vector<PathNode> adjacentCells;
			adjacentCells = checkNode.FindWalkableAdjacents();
			int length = adjacentCells.size();
			//LOG("3");
			//LOG("Length: %d",length);

			for (int a = 0; a < length; a++)
			{
				//LOG("3.1");
				if (FindItemInVector(closedList, adjacentCells[a]) == false)//Assertion error sometimes
				{
					//LOG("4");
					if (FindItemInVector(openList, adjacentCells[a]) == false)
					{
						//LOG("5");
						adjacentCells[a].g = checkNode.g + 1;
						adjacentCells[a].CalculateF(destination);
						openList.push_back(adjacentCells[a]); 
						//LOG("6");						
					}
					else
					{
						if (adjacentCells[a].g < checkNode.g) checkNode.parentPos = adjacentCells[a].pos;
					}					
				}
			}
			adjacentCells.clear();					
		}
	}
	else
	{
		finalPath.push_back(origin);
		LOG("Unavailable destination!");
		return finalPath;
	}
}