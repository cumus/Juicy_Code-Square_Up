#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Application.h"
#include "PathfindingManager.h"
#include "optick-1.3.0.0/include/optick.h"
#include "Map.h"
#include "Render.h"
#include "TextureManager.h"

#include <vector>
#include <algorithm>
#include <map>


PathfindingManager::PathfindingManager()
{
}

// Destructor
PathfindingManager::~PathfindingManager()
{
}

bool PathfindingManager::Init()
{
	bool ret = true;
	if (ret)
	{
		debugTextureID = App->tex.Load("textures/meta.png");
		LOG("Pathfinding manager initialized!");
	}
	else
		LOG("Pathfinding initialization failed!");
	return ret;
}

// Called before quitting
bool PathfindingManager::CleanUp()
{
	LOG("Freeing pathfinding library");
	return true;
}

int PathfindingManager::IteratePaths(int extra_ms)
{
	while (!toDoPaths.empty() && extra_ms > 0)
	{
		UncompletedPath path = toDoPaths.begin()->second;
		extra_ms = ContinuePath(path.lastNode, path.end,/*path.openList,*/ path.closedList,path.ID, extra_ms);
	}

	if (debugAll)
	{
		std::vector<iPoint> currentPath;
		SDL_Rect rect = { 0, 0, 64, 64 };		

		if (!storedPaths.empty())
		{
			for (std::map<int, std::vector<iPoint>>::iterator it = storedPaths.begin(); it != storedPaths.end(); ++it)
			{
				currentPath = it->second;
				if (!it->second.empty())
					for (std::vector<iPoint>::const_iterator it = currentPath.cbegin(); it != currentPath.cend(); ++it)
					{
						std::pair<int, int> render_pos = Map::I_MapToWorld(it->x, it->y);
						App->render->Blit(debugTextureID, render_pos.first, render_pos.second, &rect);
					}
			}
		}
	}
	else if (debugOne)
	{
		std::vector<iPoint> path;
		SDL_Rect rect = { 0, 0, 64, 64 };
		std::map<int, std::vector<iPoint>>::iterator it;
		it = storedPaths.find(unitDebugID);

		if (it != storedPaths.end() && !it->second.empty())
		{
			path = it->second;
			for (std::vector<iPoint>::const_iterator it = path.cbegin(); it != path.cend(); ++it)
			{
				std::pair<int, int> render_pos = Map::I_MapToWorld(it->x, it->y);
				App->render->Blit(debugTextureID, render_pos.first, render_pos.second, &rect);
			}
		}
	}

	return extra_ms;
}

UncompletedPath::UncompletedPath() : ID(0)
{}

UncompletedPath::UncompletedPath(int id, PathNode last,iPoint final, /*std::vector<PathNode> open,*/std::vector<PathNode> closed) : ID(id),lastNode(last),end(final),closedList(closed)//,openList(open)
{}

UncompletedPath::UncompletedPath(const UncompletedPath& path) : ID(path.ID),lastNode(path.lastNode),end(path.end),closedList(path.closedList)//,openList(path.openList)
{}



// PathNode -------------------------------------------------------------------------
// Constructors

PathNode::PathNode() : g(0), h(0), score(0), pos(0, 0), parentPos(iPoint({-1,-1}))
{}

PathNode::PathNode(iPoint nodePos, iPoint parent) : pos(nodePos), parentPos(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), score(node.score), pos(node.pos),parentPos(node.parentPos)
{}

#pragma region Paths management

//Utility: Delete all generated paths
void PathfindingManager::ClearAllPaths()
{
	storedPaths.clear();
}

//Utility: Prints unit path
void PathfindingManager::DebugShowUnitPath(int ID)
{
	if (debugOne) debugOne = false;
	else debugOne = true;
	unitDebugID = ID;
}

//Utility: Prints all paths
void PathfindingManager::DebugShowPaths() 
{
	int a = storedPaths.size();
	LOG("Debug paths: %d", a);
	if (debugAll) debugAll = false;
	else debugAll = true;	
}

//Utility: Updates already stored path or add it
void PathfindingManager::UpdateStoredPaths(int ID, std::vector<iPoint> path)
{
	std::map<int, std::vector<iPoint>>::iterator it;
	it = storedPaths.find(ID);

	if (it != storedPaths.end()) storedPaths[ID] = path;
	else storedPaths.insert(std::pair<int, std::vector<iPoint>>(ID, path));	
}

//Utility: Updates already pending path
void PathfindingManager::UpdatePendingPaths(int ID, UncompletedPath info)
{
	std::map<int, UncompletedPath>::iterator it;
	it = toDoPaths.find(ID);

	if (it != toDoPaths.end()) toDoPaths[ID] = info;
	else toDoPaths.insert(std::pair<int, UncompletedPath>(ID, info));
}

//Utility: Delete one stored path
void PathfindingManager::DeletePath(int ID)
{
	std::map<int, std::vector<iPoint>>::iterator it;
	it = storedPaths.find(ID);
	if (it != storedPaths.end()) storedPaths.erase(it);
}

//Utility: Delete one stored path
void PathfindingManager::DeletePendingPath(int ID)
{
	std::map<int, UncompletedPath>::iterator it;
	it = toDoPaths.find(ID);
	if (it != toDoPaths.end()) toDoPaths.erase(it);
}

//Utility: Return one path found by ID
std::vector<iPoint>* PathfindingManager::GetPath(int ID)
{
	std::vector<iPoint>* vec = nullptr;
	std::map<int, std::vector<iPoint>>::iterator it = storedPaths.find(ID);
	if (it != storedPaths.end())
	{
		vec = &it->second;
	}
	
	return vec;
}

UncompletedPath* PathfindingManager::GetToDoPath(int ID)
{
	UncompletedPath* vec = nullptr;
	std::map<int, UncompletedPath>::iterator it = toDoPaths.find(ID);
	if (it != toDoPaths.end())
	{
		vec = &it->second;
	}

	return vec;
}

#pragma endregion

#pragma region Path creation utils
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

// Fills a vector of all valid adjacent pathnodes
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

	// north-east
	cell.create(pos.x+1, pos.y + 1);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	// north-west
	cell.create(pos.x-1, pos.y + 1);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	// sud-east
	cell.create(pos.x + 1, pos.y-1);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	// sud-west
	cell.create(pos.x - 1, pos.y-1);
	if (App->pathfinding.IsWalkable(cell))
	{
		list.push_back(PathNode(cell, this->pos));
	}

	return list;
}

// Calculate the F for a specific destination tile
void PathNode::CalculateF(iPoint destination)
{
	h = pos.DistanceTo(destination);
	score = g + h;
}


#pragma endregion

#pragma region Ordenation algorithms
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

//Utility: Used by merge sort
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

#pragma endregion


// Main function to request a path from A to B
std::vector<iPoint> * PathfindingManager::CreatePath(iPoint& origin, iPoint& destination, int ID)
{
	std::vector<iPoint>* pathPointer = nullptr;
	std::vector<iPoint> finalPath;
	
	if (IsWalkable(destination))
	{
		//LOG("Destination X: %d, Y: %d", destination.x, destination.y);
		PathNode originNode(origin, nullPoint);
		originNode.g = 0;
		originNode.CalculateF(destination);

		std::vector<PathNode> closed;//,open;
		UncompletedPath path(ID, originNode,destination, /*open, */closed);
		UpdatePendingPaths(ID,path);
		LOG("Path added to queue");

		//finalPath.push_back(origin);
		UpdateStoredPaths(ID, finalPath);
		pathPointer = GetPath(ID);
	}
	else
	{
		finalPath.push_back(origin);
		UpdateStoredPaths(ID, finalPath);
		pathPointer = GetPath(ID);
		LOG("Unavailable destination");
	}

	return pathPointer;
	/*if (IsWalkable(destination))
	{
		int loops = 0;
		std::vector<PathNode> openList, closedList;
		PathNode originNode(origin, nullPoint);
		originNode.g = 0;
		originNode.CalculateF(destination);

		openList.push_back(originNode);
		LOG("Start node added to open list");

		PathNode checkNode;
		while (openList.empty() == false && loops <= STARTING_PATH_LENGTH)
		{			
			loops++;
			VectorQuicksort(openList, 0, openList.size() - 1);
			//VectorMergesort(openList,openList.size());
			checkNode = openList.front();
			closedList.push_back(checkNode); //Save node to evaluated list
			openList.erase(openList.begin());		

			if (checkNode.pos == destination) //Build final path
			{
				LOG("Destination reached");
				PathNode iteratorNode = closedList.back();
				while (iteratorNode.pos != origin)
				{
					finalPath.push_back(iteratorNode.pos);
					iteratorNode = GetItemInVector(closedList, iteratorNode.parentPos);
				}

				std::reverse(finalPath.begin(), finalPath.end());
				//LOG("Path reversed");
				LOG("Loops done: %d", loops);

				UpdateStoredPaths(ID, finalPath);
				std::vector<iPoint>* pathPointer = GetPath(ID);
				return pathPointer;
			}

			std::vector<PathNode> adjacentCells;
			adjacentCells = checkNode.FindWalkableAdjacents();
			int length = adjacentCells.size();

			for (int a = 0; a < length; a++) //Check neighbour cells
			{
				if (FindItemInVector(closedList, adjacentCells[a]) == false)//Assertion error sometimes
				{
					if (FindItemInVector(openList, adjacentCells[a]) == false)
					{
						adjacentCells[a].g = checkNode.g + 1;
						adjacentCells[a].CalculateF(destination);
						openList.push_back(adjacentCells[a]);
					}
					else
					{
						if (adjacentCells[a].g < checkNode.g) checkNode.parentPos = adjacentCells[a].pos;
					}
				}
			}
			adjacentCells.clear();

		}
		if (loops >= STARTING_PATH_LENGTH)
		{
			LOG("Path uncompleted, added to qeue");
			PathNode iteratorNode = closedList.back();
			while (iteratorNode.pos != origin)
			{
				finalPath.push_back(iteratorNode.pos);
				iteratorNode = GetItemInVector(closedList, iteratorNode.parentPos);
			}

			std::reverse(finalPath.begin(), finalPath.end());
			//LOG("Path reversed");

			UpdateStoredPaths(ID, finalPath);
			std::vector<iPoint>* pathPointer = GetPath(ID);
			return pathPointer;
		}
	}
	else
	{
		finalPath.push_back(origin);
		UpdateStoredPaths(ID, finalPath);
		std::vector<iPoint>* pathPointer = GetPath(ID);
		LOG("Unavailable destination!");
		return pathPointer;
	}*/
}

int PathfindingManager::ContinuePath(PathNode origin, iPoint destination,/*std::vector<PathNode> open,*/std::vector<PathNode> closed, int ID, int working_ms)
{
	OPTICK_EVENT();
	Timer timer;

	std::vector<iPoint> finalPath;
	bool pathEnd = false;

	std::vector<PathNode> openList, closedList = closed;	
	openList.push_back(origin);
	//LOG("Start node added to open list");

	PathNode checkNode;
	while (openList.empty() == false && timer.ReadI() < working_ms && pathEnd==false)
	{	
		VectorQuicksort(openList, 0, openList.size() - 1);
		//VectorMergesort(openList,openList.size());
		checkNode = openList.front();
		closedList.push_back(checkNode); //Save node to evaluated list
		openList.erase(openList.begin());

		/*if (checkNode.pos == destination) //Final position reached
		{
			LOG("Destination reached");
			pathEnd = true;
			break;
		}*/

		std::vector<PathNode> adjacentCells;
		adjacentCells = checkNode.FindWalkableAdjacents();
		int length = adjacentCells.size();

		for (int a = 0; a < length; a++) //Check neighbour cells
		{
			if (adjacentCells[a].pos == destination)
			{
				//checkNode = adjacentCells[a];
				closedList.push_back(adjacentCells[a]);
				pathEnd = true;
				break;
			}
			if (FindItemInVector(closedList, adjacentCells[a]) == false)
			{
				if (FindItemInVector(openList, adjacentCells[a]) == false)
				{
					adjacentCells[a].g = checkNode.g + 1;
					adjacentCells[a].CalculateF(destination);
					openList.push_back(adjacentCells[a]);
				}
				else
				{
					if (adjacentCells[a].g < checkNode.g) checkNode.parentPos = adjacentCells[a].pos;
				}
			}
		}
		adjacentCells.clear();
	}

	std::vector<iPoint>* pathPointer = GetPath(ID);

	/*else //Update pending map list
	{
		if (checkNode.pos != origin)
		{
			
		}
	}*/
	if (!closedList.empty())
	{
		PathNode iteratorNode = closedList.back(); //Build path
		while (iteratorNode.pos != origin.pos)
		{
			//LOG("Node POS X: %d,Y: %d",iteratorNode.pos.x,iteratorNode.pos.y);
			//LOG("Origin POS X: %d,Y: %d", origin.pos.x, origin.pos.y);
			finalPath.push_back(iteratorNode.pos);
			iteratorNode = GetItemInVector(closedList, iteratorNode.parentPos);
		}
	}

	if (!finalPath.empty())
	{	
		std::reverse(finalPath.begin(), finalPath.end());		
		for (std::vector<iPoint>::iterator it = finalPath.begin(); it != finalPath.end(); it++) //Save new path positions
		{
			//LOG("Push new positions");
			pathPointer->push_back(*it);
		}

		UncompletedPath path(ID, checkNode, destination,/*openList,*/closedList);
		UpdatePendingPaths(ID, path);
	}

	if (pathEnd)//Delete pending path from map list
	{
		//LOG("Destination X: %d, Y: %d", destination.x, destination.y);
		LOG("Pending deleted");
		DeletePendingPath(ID);
	}

	return working_ms - timer.ReadI();
}