#ifndef __PATHFINDINGMANAGER_H__
#define __PATHFINDINGMANAGER_H__


#include "Point.h"
#include "Vector3.h"

#include <vector>
#include <list>

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255
#define NORMAL_MOVEMENT_COST 1
#define DIAGONAL_MOVEMENT_COST 2
#define MAX_PATH_ITERATIONS 150

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
	void SetMap(int width, int height, char* data);

	// Main function to request a path from A to B
	int CreatePath(const iPoint& origin, const iPoint& destination);

	// To request all tiles involved in the last generated path
	const std::vector<iPoint>* GetLastPath() const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	char GetTileAt(const iPoint& pos) const;

private:

	// size of the map
	int width = 0u;
	int height = 0u;
	// all map walkability values [0..255]
	char* map = nullptr;
	// we store the created path here
	std::vector<iPoint> last_path;
};

// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	int FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	int g = 0;
	int h = 0;
	iPoint pos = iPoint({ 0, 0 });
	const PathNode* parent = nullptr; // needed to reconstruct the path in the end
};


// ---------------------------------------------------------------------
// Struct template for PathNode list pointer
// ---------------------------------------------------------------------
/*template<class PathNode>
struct p2List_item
{
	PathNode               data;
	p2List_item<PathNode>* next;
	p2List_item<PathNode>* prev;

	inline p2List_item(const PathNode& _data)
	{
		data = _data;
		next = prev = NULL;
	}

	~p2List_item()
	{}
};*/

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	PathNode* Find(const iPoint& point);

	// Returns the Pathnode with lowest score in this list or NULL if empty
	PathNode* GetNodeLowestScore();

	// -----------
	// The list itself, note they are not pointers!
	std::list<PathNode> list;
};

#endif 