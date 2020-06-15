#ifndef __FOGOFWARMANAGER_H__
#define __FOGOFWARMANAGER_H__

#include "SDL2_image-2.0.5/include/SDL_image.h"
#include <vector>
#include <list>
#include <map>
#include "Point.h"
#include "Gameobject.h"
#include "FoWDefs.h"

struct FoWDataStruct
{
	unsigned short tileFogBits; //saves information about which type of fog are we in (useful for smooth edges)
	unsigned short tileShroudBits; //same as above but for shroud
};

class FogOfWarManager
{
public:
	FogOfWarManager();
	~FogOfWarManager();

	bool Init();
	void Update();
	bool CleanUp();

	//Resets the map to its shrouded state
	void ResetFoWMap();
	void CreateFoWMap();
	void DeleteFoWMap();
	//Updates the data on the FoWMap based on the FoWEntities position and mask shape
	void UpdateFoWMap();
	void DrawFoWMap();
	//Tell the map that it needs to be updated the next frame
	void MapNeedsUpdate();

	//Returns the visibility state of the chosen tile (given its map coordinates)
	FoWDataStruct GetFoWTileState(iPoint mapPos);
	//Returns true if the tile is inside the map boundaries, otherwise returns false
	bool CheckFoWTileBoundaries(iPoint mapPos);
	//Returns true if the tile is visible (there's no FOG in it) otherwise returns false
	bool CheckTileVisibility(iPoint mapPos);

public:

	bool debugMode;
	bool initiated;
	static std::vector<std::vector<bool> > fogMap;

private:

	//This is where we store our FoW information
	std::vector<std::vector<FoWDataStruct> > fowMap; 

	int smoothTexID = -1;
	int debugTexID = -1;

	//Map that we use to translate bits to Texture Id's
	std::map<unsigned short, int> bitToTextureTable;

	int width;
	int height;
	bool foWMapNeedsRefresh;
};

#endif