#ifndef __PATHFINDINGMANAGER_H__
#define __PATHFINDINGMANAGER_H__


#include "Point.h"
#include "MapContainer.h"

#include <vector>
#include <map>

#define DEBUG_ID_TEXTURE 33

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Constructors
	PathNode();
	PathNode(iPoint pos, iPoint parentPos);
	PathNode(const PathNode& node);

	// Fills a vector of all valid adjacent pathnodes
	std::vector<PathNode> FindWalkableAdjacents();	
	// Calculate the F for a specific destination tile
	void CalculateF(iPoint destination);

	// -----------
	int g = 0;
	int h = 0;
	int score=0;
	iPoint pos = iPoint({ 0, 0 });
	iPoint parentPos = iPoint({ -1, -1 });
	//PathNode* parent; // needed to reconstruct the path in the end
};

class PathfindingManager
{
public:

	PathfindingManager();

	// Destructor
	~PathfindingManager();

	bool Init();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetWalkabilityLayer(const MapLayer& layer);

	// Main function to request a path from A to B
	std::vector<iPoint> CreatePath( iPoint& origin,  iPoint& destination,int ID=0);

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries( iPoint& pos);

	// Utility: returns true is the tile is walkable
	bool IsWalkable( iPoint& pos);

	// Utility: return the walkability value of a tile
	bool GetTileAt( iPoint& pos);

	//Utility: Find node in vector and returns boolean
	bool FindItemInVector(std::vector<PathNode> vec,PathNode node);

	//Utility: Remove node in vector
	void RemoveItemInVector(std::vector<PathNode>& vec, PathNode node);

	// Utility: Get node in vector 
	PathNode GetItemInVector(std::vector<PathNode>& vec, iPoint nodePos);

	//Utility: Quicksort for vector
	void VectorQuicksort(std::vector<PathNode>& vec,int L,int R);

	//Utility: Merge sort for vector
	void VectorMergesort(std::vector<PathNode>& vec, int R);

	//Utility: Used by merge sort
	void Merge(std::vector<PathNode>& vec,int l, int m, int r);

	//Utility: Delete all generated paths
	void ClearAllPaths();
	
	//Utility: Prints all paths
	void DebugShowPaths();

	//Utility: Prints unit path
	void DebugShowUnitPath(int ID);

	//Utility: Updates already stored path or add it
	void UpdateStoredPaths(int ID, std::vector<iPoint> path);

	//Utility: Delete one stored path
	void DeletePath(int ID);

	//Utility: Return one path found by ID
	std::vector<iPoint> GetPath(int ID);

private:
	MapLayer map;
	iPoint nullPoint = iPoint({ -1,-1 });
	std::map<int, std::vector<iPoint>> storedPaths; //Stores all generated paths by units
};

#endif 