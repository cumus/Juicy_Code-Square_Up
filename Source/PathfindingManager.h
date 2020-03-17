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
	PathNode(iPoint pos, PathNode* parent = nullptr);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	void FindWalkableAdjacents(std::vector<PathNode>& list_to_fill);
	// Calculate the F for a specific destination tile
	void CalculateF(iPoint& destination);

	// -----------
	int g = 0;
	int h = 0;
	int score = 0;
	iPoint pos = iPoint({ 0, 0 });
	PathNode* parent; // needed to reconstruct the path in the end
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
	std::vector<iPoint>* GetLastPath() ;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries( iPoint& pos);

	// Utility: returns true is the tile is walkable
	bool IsWalkable( iPoint& pos);

	// Utility: return the walkability value of a tile
	bool GetTileAt( iPoint& pos);

	//Utility: eturns node with lowest score
	PathNode* GetNodeLowestScore(std::vector<PathNode>& node, iPoint& destination);

	//Utility: Find node in vector and returns boolean
	bool FindItemInVector(std::vector<PathNode>& vec,PathNode node);

	//Utility: Remove node in vector
	void RemoveItemInVector(std::vector<PathNode>& vec, PathNode node);

private:	

	// size of the map
	int width = 0;
	int height = 0;
	// all map walkability values [0..255]
	MapLayer map;
	// we store the created path here
	std::vector<iPoint> finalPath;
};

#endif 