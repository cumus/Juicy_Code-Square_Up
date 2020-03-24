#ifndef __PATHFINDINGMANAGER_H__
#define __PATHFINDINGMANAGER_H__


#include "Point.h"
#include "MapContainer.h"

#include <vector>

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255
#define NORMAL_MOVEMENT_COST 1
#define DIAGONAL_MOVEMENT_COST 2
#define MAX_PATH_ITERATIONS 150

/*enum CellType
{
	NONE = 0,
	WATER,
	GROUND,
	BUILDING
};*/

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Constructors
	PathNode();
	PathNode(iPoint pos, iPoint parentPos);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
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
	std::vector<iPoint> CreatePath( iPoint& origin,  iPoint& destination);

	// To request all tiles involved in the last generated path
	//std::vector<iPoint>* GetLastPath();

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries( iPoint& pos);

	// Utility: returns true is the tile is walkable
	bool IsWalkable( iPoint& pos);

	// Utility: return the walkability value of a tile
	bool GetTileAt( iPoint& pos);

	//Utility: Returns node with lowest score
	PathNode GetNodeLowestScore(std::vector<PathNode> node);

	//Utility: Find node in vector and returns boolean
	bool FindItemInVector(std::vector<PathNode>& vec,PathNode node);

	//Utility: Remove node in vector
	void RemoveItemInVector(std::vector<PathNode>& vec, PathNode node);

	// Utility: Get node in vector 
	PathNode GetItemInVector(std::vector<PathNode>& vec, iPoint nodePos);
	

private:

	// all map walkability values [0..255]
	MapLayer map;
	iPoint nullPoint = iPoint({ -1,-1 });
};

#endif 