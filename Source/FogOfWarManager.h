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
	void CreateFoWMap(int width, int height);
	void DeleteFoWMap();
	//Updates the data on the FoWMap based on the FoWEntities position and mask shape
	void UpdateFoWMap();
	void DrawFoWMap();
	//Tell the map that it needs to be updated the next frame
	void MapNeedsUpdate();

	//Returns the visibility state of the chosen tile (given its map coordinates)
	FoWDataStruct* GetFoWTileState(iPoint mapPos);
	//Returns true if the tile is inside the map boundaries, otherwise returns false
	bool CheckFoWTileBoundaries(iPoint mapPos);
	//Returns true if the tile is visible (there's no FOG in it) otherwise returns false
	bool CheckTileVisibility(iPoint mapPos);


public:
	//A number of precomputed circle masks for you to use ranging between a radius of 2 to a radius of 5
	unsigned short circleMasks[4][fow_MAX_CIRCLE_LENGTH * fow_MAX_CIRCLE_LENGTH] =
	{
		{//R2
		fow_ALL, fow_CNW, fow_NNN, fow_CNE, fow_ALL,
		fow_CNW, fow_JNW, fow_NON, fow_JNE, fow_CNE,
		fow_WWW, fow_NON, fow_NON, fow_NON, fow_EEE,
		fow_CSW, fow_JSW, fow_NON, fow_JSE, fow_CSE,
		fow_ALL, fow_CSW, fow_SSS, fow_CSE, fow_ALL,
		},
		{//R3
		fow_ALL, fow_ALL, fow_CNW, fow_NNN, fow_CNE, fow_ALL, fow_ALL,
		fow_ALL, fow_CNW, fow_JNW, fow_NON, fow_JNE, fow_CNE, fow_ALL,
		fow_CNW, fow_JNW, fow_NON, fow_NON, fow_NON, fow_JNE, fow_CNE,
		fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE,
		fow_CSW, fow_JSW, fow_NON, fow_NON, fow_NON, fow_JSE, fow_CSE,
		fow_ALL, fow_CSW, fow_JSW, fow_NON, fow_JSE, fow_CSE, fow_ALL,
		fow_ALL, fow_ALL, fow_CSW, fow_SSS, fow_CSE, fow_ALL, fow_ALL,
		},
		{//R4
		fow_ALL, fow_ALL, fow_CNW, fow_NNN, fow_NNN, fow_NNN, fow_CNE, fow_ALL, fow_ALL,
		fow_ALL, fow_CNW, fow_JNW, fow_NON, fow_NON, fow_NON, fow_JNE, fow_CNE, fow_ALL,
		fow_CNW, fow_JNW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_JNE, fow_CNE,
		fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE,
		fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE,
		fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE,
		fow_CSW, fow_JSW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_JSE, fow_CSE,
		fow_ALL, fow_CSW, fow_JSW, fow_NON, fow_NON, fow_NON, fow_JSE, fow_CSE, fow_ALL,
		fow_ALL, fow_ALL, fow_CSW, fow_SSS, fow_SSS, fow_SSS, fow_CSE, fow_ALL, fow_ALL,
		},
		{//R5
		fow_ALL, fow_ALL, fow_ALL, fow_ALL, fow_CNW, fow_NNN, fow_CNE, fow_ALL, fow_ALL, fow_ALL, fow_ALL,
		fow_ALL, fow_ALL, fow_CNW, fow_NNN, fow_JNW, fow_NON, fow_JNE, fow_NNN, fow_CNE, fow_ALL, fow_ALL,
		fow_ALL, fow_CNW, fow_JNW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_JNE, fow_CNE, fow_ALL,
		fow_ALL, fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE, fow_ALL,
		fow_CNW, fow_JNW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_JNE, fow_CNE,
		fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE,
		fow_CSW, fow_JSW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_JSE, fow_CSE,
		fow_ALL, fow_WWW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_EEE, fow_ALL,
		fow_ALL, fow_CSW, fow_JSW, fow_NON, fow_NON, fow_NON, fow_NON, fow_NON, fow_JSE, fow_CSE, fow_ALL,
		fow_ALL, fow_ALL, fow_CSW, fow_SSS, fow_JSW, fow_NON, fow_JSE, fow_SSS, fow_CSE, fow_ALL, fow_ALL,
		fow_ALL, fow_ALL, fow_ALL, fow_ALL, fow_CSW, fow_SSS, fow_CSE, fow_ALL, fow_ALL, fow_ALL, fow_ALL,
		},
	};

private:
	//This is where the FoWEntites are stored
	//static std::vector<Gameobject*> fowGos;
	//This is where we store our FoW information
	std::vector<std::vector<FoWDataStruct> > fowMap; //Stores all generated paths by units

	int smoothTexID = -1;
	int debugTexID = -1;

	//Map that we use to translate bits to Texture Id's
	std::map<unsigned short, int> bitToTextureTable;

	int width;
	int height;
	bool debugMode;
	bool foWMapNeedsRefresh;
};

#endif