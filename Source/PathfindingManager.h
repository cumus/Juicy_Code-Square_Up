#ifndef __PATHFINDINGMANAGER_H__
#define __PATHFINDINGMANAGER_H__


#include "Point.h"
#include "MapContainer.h"
#include "Vector3.h"

#include <vector>
#include <map>


#define MAX_PATH_CALCULATIONS 40


// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Constructors
	PathNode();
	PathNode(iPoint pos, iPoint parent);
	PathNode(const PathNode& node);

	// Fills a vector of all valid adjacent pathnodes
	std::vector<PathNode> FindWalkableAdjacents();	
	// Calculate the F for a specific destination tile
	void CalculateF(iPoint destination);

	// -----------
	float g = 0;
	float h = 0;
	float score = 0;
	iPoint pos = iPoint({ 0, 0 });
	iPoint parentPos = iPoint({ -1, -1 });
};

struct UncompletedPath
{
	UncompletedPath();
	UncompletedPath(int ID,iPoint final,std::vector<PathNode> open,std::vector<PathNode> closed);
	UncompletedPath(const UncompletedPath& path);

	int length = 0;
	double ID;
	iPoint end;
	iPoint localStart;
	//PathNode lastNode;
	std::vector<PathNode> closedList;
	std::vector<PathNode> openList;
};

class PathfindingManager
{
public:

	PathfindingManager();
	~PathfindingManager();

	bool Init();
	bool CleanUp();

	int IteratePaths(int extra_ms);

	// Sets up the walkability map
	void SetWalkabilityLayer(const MapLayer& layer);

	// Main function to request a path from A to B
	std::vector<iPoint>* CreatePath( iPoint& origin, iPoint& destination,double ID);

	int ContinuePath(UncompletedPath path,int working_ms);

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries( iPoint& pos);

	// Utility: returns true is the tile is walkable
	bool IsWalkable( iPoint& pos);

	// Utility: return the walkability value of a tile
	bool GetTileAt( iPoint& pos);

	//Utility: Return true if tile is valid
	bool ValidTile(int x, int y);

	//Utility: Sets tile walkability
	void SetWalkabilityTile(int x,int y,bool estate);

	//Utility: Find node in vector and returns boolean
	bool FindItemInVector(std::vector<PathNode> vec,PathNode node);

	//Utility: Remove node in vector
	void RemoveItemInVector(std::vector<PathNode>& vec, PathNode node);

	// Utility: Get node in vector 
	PathNode GetItemInVector(std::vector<PathNode> vec, iPoint nodePos);

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
	void DebugShowUnitPath(double ID);

	//Utility: Updates already stored path or add it
	void UpdateStoredPaths(double ID, std::vector<iPoint> path);

	//Utility: Updates already pending path
	void UpdatePendingPaths(double ID, UncompletedPath info);

	//Utility: Delete one stored path
	void DeletePath(double ID);

	//Utility: Delete one stored path
	void DeletePendingPath(double ID);

	//Utility: Return one path found by ID
	std::vector<iPoint>* GetPath(double ID);

	//Utility: Return uncompleted path by ID
	UncompletedPath* GetToDoPath(double ID);

	//Utility: Check tile area
	bool CheckWalkabilityArea(vec pos, vec scale);

public:
	int debugTextureID;

private:
	bool debugAll = false;
	bool debugOne = false;
	int unitDebugID;
	MapLayer map;
	iPoint nullPoint = iPoint({ -1,-1 });
	std::vector<std::vector<bool> > walkabilityMap;
	std::map<double, std::vector<iPoint>> storedPaths; //Stores all generated paths by units
	std::map<double, UncompletedPath> toDoPaths; //Stores pending path for each id
	float msCount = 0;
};

#endif 